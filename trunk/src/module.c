/*
 * module.c
 * $Id$
 *
 * Communication with the kernel module.
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

#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <linux/unistd.h>

#include "jammod.h"
#include "syscall.h"

static int read_syscall(int fd, unsigned sct, unsigned syscall, unsigned *valuep) {
    return rkm(fd, valuep, sizeof(*valuep),
               sct + syscall * sizeof(*valuep));
}

static int write_syscall(int fd, unsigned sct, unsigned syscall, unsigned value) {
    return wkm(fd, &value, sizeof(value),
               sct + syscall * sizeof(value));
}

int init_module(int fd, unsigned sct, unsigned func) {
    int ret, ret2;
    unsigned old_syscall;

    /* backup old system call, install kmalloc as system call */
    ret = read_syscall(fd, sct, __NR_INIT_MODULE, &old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_INIT_MODULE, strerror(errno));
        return 0;
    }

    ret = write_syscall(fd, sct, __NR_INIT_MODULE, func);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_INIT_MODULE, strerror(errno));
        return 0;
    }

    /* do it */
    ret2 = INIT_MODULE();

    /* restore old system call */
    ret = write_syscall(fd, sct, __NR_INIT_MODULE, old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to restore syscall %u: %s\n",
                __NR_INIT_MODULE, strerror(errno));
    }

    return ret2;
}
