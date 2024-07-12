#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/rwlock.h>
#include <linux/uaccess.h>

#define BF_SZ 32

static int major = 0;
static rwlock_t lock;
static char kernel_buf[BF_SZ] = "Hello!\0";

static ssize_t test_read(struct file *fd, char __user *buff, size_t size, loff_t *off) {
	size_t rc;

	read_lock(&lock);
	rc = simple_read_from_buffer(buff, size, off, kernel_buf, BF_SZ);
	read_unlock(&lock);

	return rc;
}

static ssize_t test_write(struct file *fd, const char __user *buff, size_t size, loff_t *off) {
	size_t rc = 0;
	if(size > 32) {
		return -EINVAL;
	}

	write_lock(&lock);
	rc = simple_write_to_buffer(kernel_buf, BF_SZ, off, buff, size);
	write_unlock(&lock);

	return rc;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = test_read,
	.write = test_write
};


int init_module(void) {
	pr_info("My read write test module loaded!");
  	rwlock_init(&lock);
	major = register_chrdev(major, "test02", &fops);

	if(major < 0) {
		return major;
	}
	pr_info("Major number is %d.\n", major);

	return 0;
}

void cleanup_module(void) {
	unregister_chrdev(major, "test02");
	pr_info("My read write test module cleanup!");
}

MODULE_LICENSE("GPL");
