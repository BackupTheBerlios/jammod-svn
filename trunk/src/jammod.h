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

#define ERR(x) ((ulong) x > 0xfffff000)

/* the syscall we'll use for comunication and kmalloc() */
#define	OURCALL oldolduname
#define	OURSYS __NR_oldolduname
#define __NR_OURSYS OURSYS
#define __NR_OURCALL OURSYS

/* loadfile.c */
unsigned char *loadfile(const char *path, size_t *sizep);

/* linker.c */
int elf_relocate(unsigned char *data, unsigned offset);
unsigned elf_get_symbol(unsigned char *data, unsigned offset,
                        const char *name, unsigned char type);

/* symbols.c */
int init_symbols(const char *system_map_path);
unsigned get_symbol(char type, const char *name);
unsigned get_symbol_ex(char type, const char *name, ...);

/* kmem.c */
int open_kmem(void);
int rkm(int fd, void *buf, int count, unsigned long off);
int wkm(int fd, void *buf, int count, unsigned long off);

/* kmalloc.c */
unsigned kmalloc(int fd, unsigned sct, size_t size, int flags);
int kfree(int fd, unsigned sct, unsigned address);

/* module.c */
int init_module(int fd, unsigned sct, unsigned func);
