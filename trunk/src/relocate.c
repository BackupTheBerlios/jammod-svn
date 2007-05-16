/* -*- Mode: C; tab-width: 8; indent-tabs-mode: 1; c-basic-offset: 8 -*- */

/*
 * (c) 2003-2007 Max Kellermann (max@linuxtag.org)
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
 * The code in this file is mostly taken from
 * linux/arch/X/kernel/module.c.
 */

#include "jammod.h"

int apply_relocate(const Elf32_Rel *rel,
		   address_t sym_value,
		   address_t *location,
		   address_t final_location)
{
#ifdef __i386__
	switch (ELF32_R_TYPE(rel->r_info)) {
	case R_386_32:
		/* We add the value into the location given */
		*location += sym_value;
		break;
	case R_386_PC32:
		/* Add the value, subtract its postition */
		*location += sym_value - final_location;
		break;
	default:
		return -1;
	}
#else
#error Sorry, jammod has not been ported to this architecture.
#endif

	return 0;
}
