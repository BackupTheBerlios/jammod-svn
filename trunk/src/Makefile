CC = gcc

CFLAGS = -I../include -isystem /usr/src/linux-2.6.0/include -D_GNU_SOURCE -std=gnu99
CFLAGS += -g -O0
CFLAGS += -W -Wall -Wmissing-prototypes -Wwrite-strings -Wcast-qual -Wfloat-equal -Wshadow -Wpointer-arith -Wbad-function-cast -Wsign-compare -Waggregate-return -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wredundant-decls -Wnested-externs -Winline -Wdisabled-optimization -Wno-long-long -Wstrict-prototypes -Wundef -pedantic -Werror
# -Wpadded

all: jammod test.o

clean:
	rm -f jammod *.o

jammod: main.c symbols.c loadfile.c linker.c kmem.c kmalloc.c module.c
	$(CC) $(CFLAGS) -o $@ $^

test.o: test.c
	$(CC) -c $(CFLAGS) -DMODULE -D__KERNEL__ -o $@ $^
	strip --strip-debug $@

test.S: test.c
	$(CC) -S -c $(CFLAGS) -DMODULE -D__KERNEL__ -o $@ $^
