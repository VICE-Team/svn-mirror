/*
 * petdummy.c - dummy functions needed for PET
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#include "types.h"
#include "iecdrive.h"




/* Dummies needed for RISC OS version (accessed by ui.c) */
void cartridge_detach_image(void)
{
}

void cartridge_trigger_freeze(void)
{
}

CLOCK vic_ii_fetch_clk, vic_ii_draw_clk;
/* This is really of type vic_ii_t */
int vic_ii;

int vic_ii_raster_draw_alarm_handler(long offset)
{
  return 0;
}

int vic_ii_raster_fetch_alarm_handler(long offset)
{
  return 0;
}
