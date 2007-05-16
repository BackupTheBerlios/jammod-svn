/*
 * jammod.h
 * $Id$
 *
 * Master header file.
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

/* the syscall we'll use for comunication and kmalloc() */
#define	OURCALL oldolduname
#define	OURSYS __NR_oldolduname
#define __NR_OURSYS OURSYS
#define __NR_OURCALL OURSYS

/* types */

typedef unsigned address_t;

/* loadfile.c */

unsigned char *loadfile(const char *path, size_t *sizep);

/* linker.c */

int elf_relocate(unsigned char *data, address_t offset);
address_t elf_get_symbol(const unsigned char *data, address_t offset,
                         const char *name, unsigned char type);

/* symbols.c */

int init_symbols(const char *system_map_path);
address_t get_symbol(char type, const char *name);
address_t get_symbol_ex(char type, const char *name, ...);

/* kmem.c */

int init_kmem(void);
ssize_t rkm(address_t address, void *buf, size_t count);
ssize_t wkm(address_t address, void *buf, size_t count);

/* kmalloc.c */

address_t kmalloc(address_t sct, size_t size, int flags);
int kfree(address_t sct, address_t address);

/* module.c */

int init_module(address_t sct, address_t func);
