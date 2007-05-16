/*
 * linker.c
 * $Id$
 *
 * The module linker - relocate an ELF object for insertion to kernel
 * memory.
 *
 * (c) 2003 Max Kellermann (max@linuxtag.org)
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

/*
 * Yes I know everybody can see here that I have no idea of ELF and
 * linking in general. I read the ELF spec, the Linux kernel
 * headers. Then I hacked on this source until it worked for my test
 * modules. Someday I will do my homework and rewrite this file.
 */

#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <linux/elf.h>

#include "jammod.h"

static int relocate_rel(unsigned char *data, address_t offset,
                        const Elf32_Rel *rel,
                        Elf32_Shdr *rel_progbits,
                        const Elf32_Sym *sym,
                        const char *strtab) {
    Elf32_Ehdr *hdr = (struct elf32_hdr*)data;
    address_t address = rel_progbits->sh_offset + rel->r_offset;
    address_t *p = (address_t*)(data + address);
    address_t value;
    const char *name;
    Elf32_Shdr *shdr;

    switch (ELF32_ST_TYPE(sym->st_info)) {
    case STT_NOTYPE:
        /* symbol from kernel */

        /* get symbol name */
        name = strtab + sym->st_name;

        /* resolve the symbol using kernel symbol table */
        value = get_symbol(0, name);
        if (value == 0) {
            fprintf(stderr, "cannot to resolve %s\n", name);
            return -1;
        }

        break;

    case STT_SECTION:
        /* local symbol I */

        if (sym->st_shndx >= hdr->e_shnum) {
            fprintf(stderr, "sym->st_shndx out of bounds\n");
            return -1;
        }

        shdr = (Elf32_Shdr*)(data + hdr->e_shoff +
                             sym->st_shndx * hdr->e_shentsize);

        value = offset + shdr->sh_offset;
        break;

    case STT_OBJECT:
    case STT_FUNC:
        /* local symbol II */

        if (sym->st_shndx >= hdr->e_shnum) {
            fprintf(stderr, "sym->st_shndx out of bounds\n");
            return -1;
        }

        shdr = (Elf32_Shdr*)(data + hdr->e_shoff +
                             sym->st_shndx * hdr->e_shentsize);
        value = offset + shdr->sh_offset + sym->st_value;
        break;

    default:
        fprintf(stderr, "unkown ST_TYPE: %d\n", ELF32_ST_TYPE(sym->st_info));
        return -1;
    }

    if (ELF32_R_TYPE(rel->r_info) == R_386_PC32) {
        /* convert to relative address */
        value -= offset + address;
    }

    /* relocate it */
    *p += value;

    return 0;
}

static int relocate_rel_array(unsigned char *data, address_t offset,
                              const Elf32_Rel *rel, unsigned rel_count,
                              Elf32_Shdr *rel_progbits,
                              const Elf32_Sym *sym, unsigned sym_count,
                              const char *strtab) {
    int ret;

    for (; rel_count > 0; rel_count--, rel++) {
        Elf32_Word sym_index = ELF32_R_SYM(rel->r_info);

        if (sym_index >= sym_count) {
            fputs("sym_index out of bounds\n", stderr);
            return -1;
        }

        ret = relocate_rel(data, offset, rel, rel_progbits,
                           sym + sym_index, strtab);
        if (ret < 0)
            return -1;
    }

    return 0;
}

int elf_relocate(unsigned char *data, address_t offset) {
    Elf32_Ehdr *hdr = (struct elf32_hdr*)data;
    Elf32_Shdr *sechdrs, *progbits_shdr = NULL, *rel_progbits;
    Elf32_Half i;
    Elf32_Rel *rel = NULL;
    Elf32_Sym *sym = NULL;
    unsigned rel_count, sym_count = 0;
    char *strtab = NULL;
    int ret;

    /* verify ELF header */
    if (memcmp(hdr->e_ident, ELFMAG, SELFMAG) != 0 ||
        hdr->e_ident[4] != ELFCLASS32 ||
        hdr->e_type != ET_REL ||
        hdr->e_machine != EM_386 ||
        hdr->e_version != EV_CURRENT ||
        hdr->e_shentsize != sizeof(*sechdrs)) {
        errno = EINVAL;
        return -1;
    }

    sechdrs = (Elf32_Shdr*)(data + hdr->e_shoff);

    for (i = 0; i < hdr->e_shnum; i++) {
        Elf32_Shdr *shdr = sechdrs + i;
        switch (shdr->sh_type) {
        case SHT_SYMTAB:
            sym = (Elf32_Sym*)(data + shdr->sh_offset);
            sym_count = shdr->sh_size / sizeof(*sym);
            break;
        case SHT_STRTAB:
            strtab = (char*)(data + shdr->sh_offset);
            break;
        }
    }

    for (i = 0; i < hdr->e_shnum; i++) {
        Elf32_Shdr *shdr = sechdrs + i;
        switch (shdr->sh_type) {
        case SHT_PROGBITS:
            progbits_shdr = shdr;
            break;
        case SHT_REL:
            if (progbits_shdr == NULL) {
                fprintf(stderr, "no last_shdr\n");
                continue;
            }
                
            rel_progbits = progbits_shdr;
            rel = (Elf32_Rel*)(data + shdr->sh_offset);
            rel_count = shdr->sh_size / sizeof(*rel);

            ret = relocate_rel_array(data, offset,
                                     rel, rel_count, rel_progbits,
                                     sym, sym_count,
                                     strtab);
            if (ret < 0)
                return -1;

            break;
        }
    }

    return 0;
}

address_t elf_get_symbol(unsigned char *data, address_t offset,
                         const char *name, unsigned char type) {
    Elf32_Ehdr *hdr = (struct elf32_hdr*)data;
    Elf32_Shdr *sechdrs, *symtab = NULL;
    char *strtab = NULL;
    unsigned i, sym_count;
    const Elf32_Sym *sym;

    /* verify ELF header */
    if (memcmp(hdr->e_ident, ELFMAG, SELFMAG) != 0 ||
        hdr->e_ident[4] != ELFCLASS32 ||
        hdr->e_type != ET_REL ||
        hdr->e_machine != EM_386 ||
        hdr->e_version != EV_CURRENT ||
        hdr->e_shentsize != sizeof(*sechdrs))
        return 0;

    sechdrs = (Elf32_Shdr*)(data + hdr->e_shoff);

    /* find strtab and symtab */

    for (i = 0; i < hdr->e_shnum; i++) {
        Elf32_Shdr *shdr = sechdrs + i;
        if (shdr->sh_type == SHT_STRTAB)
            strtab = (char*)(data + shdr->sh_offset);
        else if (shdr->sh_type == SHT_SYMTAB)
            symtab = shdr;
    }

    if (strtab == NULL || symtab == NULL)
        return 0;

    /* look for symbol in symtab */

    sym = (const Elf32_Sym*)(data + symtab->sh_offset);
    sym_count = symtab->sh_size / sizeof(*sym);

    for (i = 0; i < sym_count; i++, sym++) {
        if (ELF32_ST_TYPE(sym->st_info) != type)
            continue;
        if (strcmp(strtab + sym->st_name, name) != 0)
            continue;

        if (sym->st_shndx >= hdr->e_shnum) {
            fprintf(stderr, "sym->st_shndx out of bounds\n");
            return 0;
        }

        return offset + sechdrs[sym->st_shndx].sh_offset + sym->st_value;
    }

    return 0;
}
