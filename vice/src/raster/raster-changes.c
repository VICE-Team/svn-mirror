/*
 * raster-changes.c - Handling of changes within a raster line.
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

#include "raster-changes.h"



void
raster_changes_init (raster_changes_t *changes)
{
  /* FIXME: More?  */
  changes->count = 0;
}

raster_changes_t *
raster_changes_new (void)
{
  raster_changes_t *new;

  new = xmalloc (sizeof (raster_changes_t));
  raster_changes_init (new);

  return new;
}
