#include <linux/kernel.h>

int the_wall = 0;

void test(void);
void test(void) {
    printk("is there anybody out there?\n");
    printk("just nod if you can hear me\n");
    the_wall = 1;
}
