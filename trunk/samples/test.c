/*
 * test.c
 * $Id$
 *
 * Demo kernel module. Prints a message.
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

#include <linux/kernel.h>

int init_module(void);
int init_module(void) {
    printk(KERN_EMERG "is there anybody out there?\n");
    printk(KERN_EMERG "just nod if you can hear me\n");
    return -1;
}
