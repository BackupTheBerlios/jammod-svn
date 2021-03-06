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
#include <sys/utsname.h>
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

static void usage(void)  __attribute__ ((__noreturn__));
static void usage(void) {
    fprintf(stderr, "jammod v%s (c) 2003-2004 Max Kellermann (max@linuxtag.org)\n",
            PACKAGE_VERSION);
    fprintf(stderr, "usage: jammod [-m System.map] filename [param1=value1 ...]\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    int z, ret, verbose = 0;
    char *module_path = NULL, *system_map_path = NULL;
    unsigned char *module;
    size_t module_size;
    address_t sct, base, entry;
    ssize_t nbytes;
    char param[64];

    /* mini-getopt */
    for (z = 1; z < argc; z++) {
        if (argv[z][0] == '-') {
            switch (argv[z][1]) {
            case 'm':
                if (argv[z][2])
                    usage();
                z++;
                if (z >= argc)
                    usage();

                system_map_path = argv[z];
                
                break;
            case 'v':
                if (argv[z][2])
                    usage();
                verbose++;
                break;
            default:
                usage();
            }
        } else {
            if (module_path != NULL)
                usage();
            module_path = argv[z];
            z++;
            break;
        }
    }

    if (module_path == NULL)
        usage();

    if (system_map_path == NULL) {
        /* default to "/boot/System.map-`uname -r`" */
        char path[64];
        struct utsname un;

        uname(&un);
        snprintf(path, sizeof(path), "/boot/System.map-%s", un.release);
        system_map_path = strdup(path);
    }

    /* initialize */
    ret = init_symbols(system_map_path);
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to load kernel symbol table %s: %s\n",
                system_map_path, strerror(errno));
        exit(1);
    }

    /* load file, check ELF format */
    module = loadfile(module_path, &module_size);
    if (module == NULL) {
        fprintf(stderr, "jammod: failed to load %s: %s\n",
                module_path, strerror(errno));
        exit(1);
    }

    /* set arguments */
    for (; z < argc; z++) {
        char *eq;
        address_t address;
        int value, *dest;

        snprintf(param, sizeof(param), "%s", argv[z]);
        eq = strchr(param, '=');
        if (eq == NULL || eq == param || !eq[1])
            usage();

        *eq = 0;

        address = elf_get_symbol(module, 0, param, STT_OBJECT);
        if (address == 0) {
            fprintf(stderr, "jammod: symbol %s not found\n",
                    param);
            exit(EXIT_FAILURE);
        }

        dest = (int*)(module + address);

        value = (int)strtol(eq + 1, NULL, 0);

        if (verbose > 0)
            fprintf(stderr, "setting %s (offset 0x%08x) to 0x%08x, old value 0x%08x\n",
                    param, address, value, *dest);

        *dest = value;
    }

    /* open kernel */
    ret = init_kmem();
    if (ret < 0) {
        fprintf(stderr, "jammod: failed to open /dev/kmem: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    sct = get_symbol('D', "sys_call_table");
    if (sct == 0) {
        fputs("jammod: symbol 'sys_call_table' not found in kernel\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (verbose > 2)
        fprintf(stderr, "sys_call_table = 0x%08x\n", sct);

    /* write module to kernel space */
    base = kmalloc(sct, module_size, GFP_KERNEL);
    if (base == 0) {
        fprintf(stderr, "jammod: failed to kmalloc(%u)\n", module_size);
        exit(EXIT_FAILURE);
    }

    if (verbose > 0)
        fprintf(stderr, "base = 0x%08x\n", base);

    ret = elf_relocate(module, base);
    if (ret < 0) {
        fputs("jammod: failed to relocate\n", stderr);
        exit(EXIT_FAILURE);
    }

    nbytes = wkm(base, module, module_size);
    if (nbytes < (ssize_t)module_size) {
        kfree(sct, base);
        fprintf(stderr, "jammod: failed to copy module to kernel: %s\n",
                strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* init_module */
    entry = elf_get_symbol(module, base, "init_module", STT_FUNC);
    if (entry == 0) {
        kfree(sct, base);
        fputs("jammod: symbol 'init_module' not found in module\n", stderr);
        exit(EXIT_FAILURE);
    }

    if (verbose > 2)
        fprintf(stderr, "entry = 0x%08x\n", entry);

    ret = init_module(sct, entry);
    if (ret < 0) {
        fprintf(stderr, "jammod: module's init_module failed, return code %d\n",
                ret);
        kfree(sct, base);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "jammod: module loaded successfully\n");
    return EXIT_SUCCESS;
}
