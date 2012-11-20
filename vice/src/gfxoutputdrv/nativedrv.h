/*
 * nativedrv.h - native screenshot common code header.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_NATIVEDRV_H
#define VICE_NATIVEDRV_H

#include "types.h"

typedef struct native_data_s {
    BYTE *colormap;
    int xsize;
    int ysize;
    const char *filename;
} native_data_t;

typedef struct native_color_sort_s {
    BYTE color;
    int amount;
} native_color_sort_t;

extern void gfxoutput_init_doodle(void);
extern void gfxoutput_init_koala(void);

#endif
