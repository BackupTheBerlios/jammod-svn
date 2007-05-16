TOPDIR = ..
include $(TOPDIR)/config.mk
include $(TOPDIR)/package.mk

CFLAGS += -isystem $(KERNEL_HEADERS) -D_GNU_SOURCE -std=gnu99
CFLAGS += -W -Wall -Wmissing-prototypes -Wwrite-strings -Wcast-qual -Wfloat-equal -Wshadow -Wpointer-arith -Wbad-function-cast -Wsign-compare -Waggregate-return -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wredundant-decls -Wnested-externs -Winline -Wdisabled-optimization -Wno-long-long -Wstrict-prototypes -Wundef -pedantic

all: jammod

clean:
	rm -f jammod

install: all
	install -d -m 0755 $(DESTDIR)$(PREFIX)/sbin
	install -m 0755 jammod $(DESTDIR)$(PREFIX)/sbin

jammod: main.c symbols.c loadfile.c linker.c kmem.c kmalloc.c module.c
	test -f $(KERNEL_HEADERS)/linux/version.h || test -f $(KERNEL_HEADERS)/linux/uts.h
	$(CC) $(CFLAGS) -o $@ $^
