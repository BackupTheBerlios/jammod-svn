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

static int read_syscall(address_t sct, unsigned syscall, address_t *valuep) {
    return rkm(sct + syscall * sizeof(*valuep), valuep, sizeof(*valuep));
}

static int write_syscall(address_t sct, unsigned syscall, address_t value) {
    return wkm(sct + syscall * sizeof(value), &value, sizeof(value));               
}

int init_module(address_t sct, address_t func) {
    int ret, ret2;
    address_t old_syscall;

    /* backup old system call, install kmalloc as system call */
    ret = read_syscall(sct, __NR_INIT_MODULE, &old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_INIT_MODULE, strerror(errno));
        return 0;
    }

    ret = write_syscall(sct, __NR_INIT_MODULE, func);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_INIT_MODULE, strerror(errno));
        return 0;
    }

    /* do it */
    ret2 = INIT_MODULE();

    /* restore old system call */
    ret = write_syscall(sct, __NR_INIT_MODULE, old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to restore syscall %u: %s\n",
                __NR_INIT_MODULE, strerror(errno));
    }

    return ret2;
}
