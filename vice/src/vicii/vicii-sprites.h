/*
 * vicii-sprites.h - Sprites for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _VIC_II_SPRITES_H
#define _VIC_II_SPRITES_H

/* This defines the stolen sprite cycles for all the values of `dma_msk'.  */
/* The table derives from what Christian Bauer <bauec002@physik.uni-mainz.de>
   says in both the "VIC-Article" and Frodo's `VIC_SC.cpp' source file.  */

struct vic_ii_sprites_fetch_s
{
    int cycle, num;
    unsigned int first, last;
};
typedef struct vic_ii_sprites_fetch_s vic_ii_sprites_fetch_t;

extern const vic_ii_sprites_fetch_t vic_ii_sprites_fetch_table[256][4];
extern const int vic_ii_sprites_crunch_table[64];

extern void vic_ii_sprites_init(void);
extern void vic_ii_sprites_set_double_size(int enabled);
extern void vic_ii_sprites_set_x_position(unsigned int num,
                                          int new_x, int raster_x);

#endif

