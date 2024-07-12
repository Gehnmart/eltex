#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void) {
  pf_info("My test module loaded!");

  return 0;
}

int cleanup_module(void) {
  pf_info("My test module cleanup!");

  return 0;
}

MODULE_LICENSE("GPL");