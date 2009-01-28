/*
 * parse.c - Parse keyboard mapping file
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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
#ifndef VICE_PARSE_H
#define VICE_PARSE_H

#include "kbd.h"

/* ------------------------------------------------------------------------ */

typedef struct _convmap
{
    keyconv map[2][0x100];  // Conversion Map
                            // 0 = unshifted, 1 = shifted
    int lshift_row;         // Location of virt shift key
    int lshift_col;         // Location of virt shift key
    int rshift_row;         // Location of virt shift key
    int rshift_col;         // Location of virt shift key

    int entries;            // number of valid entries

    int symbolic;           // this is true if we have a symbolic map
} convmap;

extern convmap keyconvmap;

/* ------------------------------------------------------------------------ */

extern int load_keymap_file(const char *fname);

#endif
