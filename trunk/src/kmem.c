/*
 * Taken from SucKIT 1.3b; its license file says:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * $Id: routines for work with /dev/kmem
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#include "jammod.h"

#define DEFAULT_KMEM "/dev/kmem"

#define ERR(x) ((ulong) x > 0xfffff000)

/* wtf this does ? ;) */
int	open_kmem(void)
{
	return open(DEFAULT_KMEM, O_RDWR, 0);
}

/* shortcut for "read kernel memory", it's equivalent
   to pread(), but I think it might not be a good idea to use
   syscall presented in kernel 2.2.x */
int	rkm(int fd, void *buf, int count, ulong off)
{
	int	i;
	
	i = lseek(fd, off, SEEK_SET);
	if (ERR(i))
		return i;
	return read(fd, buf, count);
}

int	wkm(int fd, void *buf, int count, ulong off)
{
	int	i;
	
	i = lseek(fd, off, SEEK_SET);
	if (ERR(i))
		return i;
	return write(fd, buf, count);
}
