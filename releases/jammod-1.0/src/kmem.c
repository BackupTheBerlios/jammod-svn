/*
 * kmem.c
 * $Id$
 *
 * Access to /dev/kmem. Some of this was copied from SucKIT.
 *
 * (c) 2003-2004 Max Kellermann (max@linuxtag.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#include "jammod.h"

#define ERR(x) ((ulong) x > 0xfffff000)

static int kmem_fd = 0;

int init_kmem(void) {
    assert(kmem_fd == 0);

    kmem_fd = open("/dev/kmem", O_RDWR, 0);
    if (kmem_fd < 0)
        return -1;

    return 0;
}

ssize_t rkm(address_t address, void *buf, size_t count) {
    off_t ret;

    assert(kmem_fd > 0);

    ret = lseek(kmem_fd, (off_t)address, SEEK_SET);
    if (ERR(ret))
        return -1;

    return read(kmem_fd, buf, count);
}

ssize_t wkm(address_t address, void *buf, size_t count) {
    off_t ret;

    assert(kmem_fd > 0);

    ret = lseek(kmem_fd, (off_t)address, SEEK_SET);
    if (ERR(ret))
        return -1;

    return write(kmem_fd, buf, count);
}
