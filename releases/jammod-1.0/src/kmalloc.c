/*
 * kmalloc.c
 * $Id$
 *
 * Calls kmalloc and kfree from userspace. Idea taken from SucKIT.
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

address_t kmalloc(address_t sct, size_t size, int flags) {
    int ret;
    address_t symbol_kmalloc, old_syscall, address;

    /* get symbol */
    symbol_kmalloc = get_symbol_ex('T', "kmalloc", "_kmalloc", "__kmalloc");
    if (symbol_kmalloc == 0) {
        fputs("jammod: cannot find symbol kmalloc\n", stderr);
        return 0;
    }

    /* backup old system call, install kmalloc as system call */
    ret = read_syscall(sct, __NR_KMALLOC, &old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_KMALLOC, strerror(errno));
        return 0;
    }

    ret = write_syscall(sct, __NR_KMALLOC, symbol_kmalloc);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_KMALLOC, strerror(errno));
        return 0;
    }

    /* do it */
    address = KMALLOC(size, flags);

    /* restore old system call */
    ret = write_syscall(sct, __NR_KMALLOC, old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to restore syscall %u: %s\n",
                __NR_KMALLOC, strerror(errno));
    }

    return address;
}

int kfree(address_t sct, address_t address) {
    int ret;
    address_t symbol_kfree, old_syscall;

    /* get symbol */
    symbol_kfree = get_symbol_ex('T', "kfree", "_kfree", "__kfree");
    if (symbol_kfree == 0) {
        fputs("jammod: cannot find symbol kfree\n", stderr);
        return -1;
    }

    /* backup old system call, install kmalloc as system call */
    ret = read_syscall(sct, __NR_KFREE, &old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_KFREE, strerror(errno));
        return -1;
    }

    ret = write_syscall(sct, __NR_KFREE, symbol_kfree);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to read syscall %u: %s\n",
                __NR_KFREE, strerror(errno));
        return -1;
    }

    /* do it */
    KFREE(address);

    /* restore old system call */
    ret = write_syscall(sct, __NR_KFREE, old_syscall);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to restore syscall %u: %s\n",
                __NR_KFREE, strerror(errno));
    }

    return 0;
}
