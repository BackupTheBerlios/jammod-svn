#include <linux/kernel.h>

int init_module(void);
int init_module(void) {
    printk(KERN_EMERG "is there anybody out there?\n");
    printk(KERN_EMERG "just nod if you can hear me\n");
    return -1;
}
