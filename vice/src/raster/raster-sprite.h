/*
 * raster-sprite.h - Sprite handling.
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

#ifndef _RASTER_SPRITE_H
#define _RASTER_SPRITE_H

struct _raster_sprite
  {
    /* Physical X, Y coordinates.  Note: `x' can be negative, when the sprite
       "wraps" to the left!  It can also be different from the value in the
       corresponding register in the video chip.  */
    int x, y;

    /* Expansion flags. */
    int x_expanded, y_expanded;

    /* Multicolor mode flag. */
    int multicolor;

    /* If 0, the sprite is in in foreground; if 1, it is in background.  */
    int in_background;

    /* Primary sprite color.  */
    unsigned int color;

    /* Sprite memory pointer (for drawing).  */
    int memptr;

    /* Value to add to memptr at the next memory fetch.  */
    int memptr_inc;

    /* Sprite expansion flip flop.  */
    int exp_flag;

    /* DMA activation flag. If != 0, memory access is enabled for this
       sprite.  */
    int dma_flag;
  };
typedef struct _raster_sprite raster_sprite_t;



void raster_sprite_init (raster_sprite_t *s);
raster_sprite_t *raster_sprite_new (void);

#endif /* _RASTER_SPRITE_H */
