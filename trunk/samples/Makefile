TOPDIR = ..
include $(TOPDIR)/config.mk
include $(TOPDIR)/package.mk

CFLAGS += -isystem $(KERNEL_HEADERS) -D_GNU_SOURCE -std=gnu99
CFLAGS += -W -Wall -Wmissing-prototypes -Wwrite-strings -Wcast-qual -Wfloat-equal -Wshadow -Wpointer-arith -Wbad-function-cast -Wsign-compare -Waggregate-return -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wredundant-decls -Wnested-externs -Winline -Wdisabled-optimization -Wno-long-long -Wstrict-prototypes -Wundef -pedantic
CFLAGS += -DMODULE -D__KERNEL__

all: test.ko param.ko capall.ko

clean:
	rm -f *.ko

install:

%.ko: %.c
	test -f $(KERNEL_HEADERS)/linux/version.h
	$(CC) -c $(CFLAGS) -o $@ $^
	strip --strip-debug $@

%.S: %.c
	test -f $(KERNEL_HEADERS)/linux/version.h
	$(CC) -S -c $(CFLAGS) -o $@ $^
