/*
 * loadfile.c
 * $Id$
 *
 * Load a whole file into memory.
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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "jammod.h"

unsigned char *loadfile(const char *path, size_t *sizep) {
    int fd, ret;
    struct stat st;
    unsigned char *buffer;
    ssize_t nbytes;

    ret = stat(path, &st);
    if (ret < 0)
        return NULL;

    if (!S_ISREG(st.st_mode)) {
        errno = EINVAL;
        return NULL;
    }

    buffer = malloc(st.st_size);
    if (buffer == NULL)
        return NULL;

    fd = open(path, O_RDONLY);
    if (fd < 0) {
        free(buffer);
        return NULL;
    }

    nbytes = read(fd, buffer, st.st_size);
    if (nbytes < st.st_size) {
        free(buffer);
        return NULL;
    }

    close(fd);

    *sizep = st.st_size;
    return buffer;
}
