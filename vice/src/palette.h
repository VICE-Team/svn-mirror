/*
 * palette.h - Palette handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _PALETTE_H
#define _PALETTE_H

#include "types.h"

typedef struct palette_entry {
    char *name;
    BYTE red;
    BYTE green;
    BYTE blue;
    BYTE dither;
} palette_entry_t;

typedef struct palette {
    int num_entries;
    palette_entry_t *entries;
} palette_t;

#ifdef __riscos
#define PALETTE_FILE_EXTENSION "/vpl"
#else
#define PALETTE_FILE_EXTENSION  ".vpl"
#endif

palette_t *palette_create(int num_entries, const char *entry_names[]);
extern void palette_free(palette_t *p);
extern int palette_set_entry(palette_t *p, int number, BYTE red, BYTE green, BYTE blue, BYTE dither);
extern int palette_copy(palette_t *dest, const palette_t *src);
extern int palette_load(const char *file_name, palette_t *palette_return);
extern int palette_save(const char *file_name, const palette_t *palette);

#endif /* _PALETTE_H */
