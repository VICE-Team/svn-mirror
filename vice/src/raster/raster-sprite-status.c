/*
 * raster-sprite-status.c - Sprite status handling for the raster emulation.
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

#include "vice.h"

#include "utils.h"

#include "raster-sprite-status.h"



void
raster_sprite_status_init (raster_sprite_status_t *s,
			   unsigned int num_sprites)
{
  unsigned int i;

  s->num_sprites = num_sprites;

  s->draw_function = NULL;

  s->sprites = xmalloc (sizeof (*s->sprites) * num_sprites);

  s->visible_msk = 0;
  s->dma_msk = 0;
  s->new_dma_msk = 0;

  s->sprite_data_1 = xmalloc (sizeof (DWORD) * num_sprites);
  s->sprite_data_2 = xmalloc (sizeof (DWORD) * num_sprites);

  s->sprite_data = s->sprite_data_1;
  s->new_sprite_data = s->sprite_data_2;

  for (i = 0; i < num_sprites; i++)
    raster_sprite_init (&s->sprites[i]);
}

raster_sprite_status_t *
raster_sprite_status_new (unsigned int num_sprites)
{
  raster_sprite_status_t *new;

  new = xmalloc (sizeof (raster_sprite_status_t));
  raster_sprite_status_init (new, num_sprites);

  return new;
}

void
raster_sprite_status_set_draw_function (raster_sprite_status_t *status,
                                 raster_sprite_status_draw_function_t function)
{
  status->draw_function = function;
}
