/*
 * petdummy.c - dummy functions needed for PET and CBM-II.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#include "config.h"
#include "types.h"
#include "iecdrive.h"




/* Dummies needed for RISC OS version (accessed by ui.c) */
void cartridge_detach_image(void)
{
}

CLOCK vic_ii_fetch_clk, vic_ii_draw_clk;

int int_rasterfetch(long offset)
{
  return 0;
}

void iec_fast_drive_write(BYTE data)
{
}

void iec_update_ports(void)
{
}

void iec_calculate_callback_index(void)
{
}

int iec_available_busses(void)
{
  return 0;
}

iec_info_t *iec_get_drive_port(void)
{
  return NULL;
}

void iec_drive0_write(BYTE data)
{
}

BYTE iec_drive0_read(void)
{
  return 0;
}

void iec_drive1_write(BYTE data)
{
}

BYTE iec_drive1_read(void)
{
  return 0;
}

BYTE parallel_cable_drive_read(int handshake)
{
  return 0;
}

void parallel_cable_drive0_write(BYTE data, int handshake)
{
}

void parallel_cable_drive1_write(BYTE data, int handshake)
{
}
