#include <linux/kernel.h>

int param = 0xdeadbeef;

int init_module(void);
int init_module(void) {
    printk(KERN_EMERG "param = 0x%08x (%d)\n", param, param);
    return -1;
}
