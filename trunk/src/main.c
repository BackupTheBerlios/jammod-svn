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

#include "jammod.h"

/* the syscall we'll use for comunication and kmalloc() */
#define	OURCALL oldolduname
#define	OURSYS __NR_oldolduname
#define __NR_OURSYS OURSYS
#define __NR_OURCALL OURSYS

#define STT_FUNC 2

#define ERR(x) ((ulong) x > 0xfffff000)

int query_module(const char *name, int which,
                 void *buf, size_t bufsize, size_t *ret);

ulong old80;

int main(int argc, char **argv) {
    int fd, ret;
    ulong sct;
    ulong kmalloc, gfp;
    ulong oldsys;
    unsigned char *module;

    if (argc != 2) {
        fprintf(stderr, "usage: jammod filename\n");
        exit(1);
    }

    ret = init_symbols();
    if (ret < 0) {
        fputs("jammod: failed to load kernel symbol table\n", stderr);
        exit(1);
    }

    module = loadfile(argv[1]);
    if (module == NULL) {
        fprintf(stderr, "jammod: failed to load %s: %s\n",
                argv[1], strerror(errno));
        exit(1);
    }

    ret = elf_relocate(module, 0);
    if (ret < 0) {
        fputs("jammod: failed to resolve\n", stderr);
        exit(1);
    }

    elf_get_symbol(module, 0, "test", STT_FUNC);

    fd = open_kmem();
    if (fd < 0) {
        printf("FUCK: Can't open kmem for read/write (%d)\n", -fd);
        return 1;
    }

    sct = get_symbol('D', "sys_call_table");
    if (!sct) {
        printf("FUCK: Can't find sys_call_table[]\n");
        close(fd);
        return 1;
    }

    printf("sct[]=0x%08x, ", (uint) sct);

    kmalloc = get_symbol_ex('T', "kmalloc", "_kmalloc", "__kmalloc");
    if (!kmalloc) {
        printf("FUCK: Can't find kmalloc()!\n");
        close(fd);
        return 1;
    }

    printf("kmalloc()=0x%08x, gfp=0x%x\n", (uint) kmalloc, (uint) gfp);

    if (ERR(rkml(fd, &oldsys, sct + OURSYS * 4))) {
        printf("FUCK: Can't read syscall %d addr\n", OURSYS);
        close(fd);
        return 1;
    }

    wkml(fd, kmalloc, sct + OURSYS * 4);

    /*kmalloc = (ulong) get_kma(fd, sct & 0xff000000, &gfp, get_kma_hint());*/

}
