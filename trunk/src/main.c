/*
 * main.c
 * $Id$
 *
 * Main source file.
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
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <asm/unistd.h>
#include "gfp.h"

#include "jammod.h"

#define STT_OBJECT 1
#define STT_FUNC 2

int main(int argc, char **argv) {
    int fd, ret;
    ulong sct;
    unsigned char *module;
    size_t module_size;
    unsigned base, entry;
    ssize_t nbytes;

    if (argc != 2) {
        fprintf(stderr, "usage: jammod filename\n");
        exit(1);
    }

    /* initialize */
    ret = init_symbols("/boot/System.map-2.6.0");
    if (ret < 0) {
        fputs("jammod: failed to load kernel symbol table\n", stderr);
        exit(1);
    }

    /* load file, check ELF format */
    module = loadfile(argv[1], &module_size);
    if (module == NULL) {
        fprintf(stderr, "jammod: failed to load %s: %s\n",
                argv[1], strerror(errno));
        exit(1);
    }

    /* open kernel */
    fd = open_kmem();
    if (fd < 0) {
        fprintf(stderr, "jammod: failed to kmem: %s\n", strerror(errno));
        return 1;
    }

    sct = get_symbol('D', "sys_call_table");
    if (sct == 0) {
        fputs("jammod: symbol 'sys_call_table' not found in kernel\n", stderr);
        exit(1);
    }

    /* write module to kernel space */
    base = kmalloc(fd, sct, module_size, GFP_KERNEL);
    if (base == 0) {
        fprintf(stderr, "jammod: failed to kmalloc(%u)\n", module_size);
    }

    ret = elf_relocate(module, base);
    if (ret < 0) {
        fputs("jammod: failed to relocate\n", stderr);
        exit(1);
    }

    nbytes = wkm(fd, module, module_size, base);
    if (nbytes < (ssize_t)module_size) {
        kfree(fd, sct, base);
        fprintf(stderr, "jammod: failed to copy module to kernel: %s\n",
                strerror(errno));
        exit(1);
    }

    /* init_module */
    entry = elf_get_symbol(module, base, "init_module", STT_FUNC);
    if (entry == 0) {
        kfree(fd, sct, base);
        fputs("jammod: symbol 'init_module' not found in module\n", stderr);
        exit(1);
    }

    ret = init_module(fd, sct, entry);
    if (ret < 0) {
        fprintf(stderr, "jammod: module's init_module failed, return code %d\n",
                ret);
        kfree(fd, sct, base);
        exit(1);
    }

    fprintf(stderr, "jammod: module loaded successfully\n");
    return 0;
}
