/*
 * symbols.c
 * $Id$
 *
 * Kernel symbol table functions.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "jammod.h"

struct symbol {
    unsigned address;
    char type;
    char *name;
};

static struct symbol *symbols;
static unsigned symbol_count;

int init_symbols(const char *system_map_path) {
    FILE *file;
    char name[128];
    unsigned size = 0;
    int ret;
    struct symbol symbol;

    symbol_count = 0;
    size = 1024;
    symbols = malloc(size * sizeof(*symbols));

    /* XXX: don't hardcode */
    file = fopen(system_map_path, "r");
    if (file == NULL)
        return -1;

    while (!feof(file)) {
        /* parse line */
        memset(&symbol, 0, sizeof(symbol));
        ret = fscanf(file, "%x %c %127s\n",
                     &symbol.address, &symbol.type,
                     name);
        if (ret < 3)
            continue;

        symbol.name = strdup(name);

        /* add symbol to array */
        if (symbol_count >= size) {
            size += 1024;
            symbols = realloc(symbols, size * sizeof(*symbols));
        }

        symbols[symbol_count++] = symbol;
    }

    fclose(file);

    symbols = realloc(symbols, symbol_count * sizeof(*symbols));

    return 0;
}

unsigned get_symbol(char type, const char *name) {
    unsigned z, weak_value = 0;

    for (z = 0; z < symbol_count; z++) {
        if ((type == 0 || symbols[z].type == type) &&
            strcmp(symbols[z].name, name) == 0) {
            if (type == 0 && symbols[z].type == 'W') {
                /* weak symbol - store but continue the search for
                   strong symbol */
                weak_value = symbols[z].address;
            } else {
                /* strong symbol found */
                return symbols[z].address;
            }
        }
    }

    /* no strong symbol found - return weak symbol (if present) */
    return weak_value;
}

unsigned get_symbol_ex(char type, const char *name, ...) {
    va_list ap;
    unsigned addr;

    /* try first name */
    addr = get_symbol(type, name);
    if (addr)
        return addr;

    /* try other names until NULL argument is found */
    va_start(ap, name);
    while ((name = va_arg(ap, const char*)) != NULL) {
        addr = get_symbol(type, name);
        if (addr)
            return addr;
    }
    va_end(ap);

    /* nothing found */
    return 0;
}
