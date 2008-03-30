/*
 * mousedrv.x - Mouse handling for RISC OS
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* This is a first rough implementation of mouse emulation for MS-DOS.
   A smarter and less buggy emulation is of course possible. */

#include "mouse.h"
#include "mousedrv.h"


void mousedrv_mouse_changed(void)
{
}

int mousedrv_resources_init(void)
{
  return 0;
}

int mousedrv_cmdline_options_init(void)
{
  return 0;
}

void mousedrv_init(void)
{
}

BYTE mousedrv_get_x(void)
{
  return 0;
}

BYTE mousedrv_get_y(void)
{
  return 0;
}
