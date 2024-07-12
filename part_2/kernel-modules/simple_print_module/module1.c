#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void) {
  pr_info("My test module loaded!");

  return 0;
}

void cleanup_module(void) {
  pr_info("My test module cleanup!");
}

MODULE_LICENSE("GPL");
