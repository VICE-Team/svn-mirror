/*
 * misc.h - Miscellaneous functions for debugging.
 *
 * Written by
 *  Vesa-Matti Puro (vmp@lut.fi)
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta     (jopi@stekt.oulu.fi)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#ifndef X64_MISC_H
#define X64_MISC_H

#include "types.h"


extern void    show_bases ( char *line, int mode );
extern void    show ( void );
extern void    print_stack ( BYTE sp );
extern char   *sprint_binary ( BYTE code );
extern char   *sprint_ophex ( ADDRESS p);
extern char   *sprint_opcode ( ADDRESS counter, int base );
extern char   *sprint_disassembled ( ADDRESS counter, BYTE x, BYTE p1, BYTE p2, int base );
extern int     eff_address(ADDRESS counter, int step);

#endif  /* X64_MISC_H */
