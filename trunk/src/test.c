#include <linux/kernel.h>

int the_wall = 0xdeadbeef;

int init_module(void);
int init_module(void) {
    printk("is there anybody out there?\n");
    printk("just nod if you can hear me\n");
    the_wall = 1;
    return -2;
}
