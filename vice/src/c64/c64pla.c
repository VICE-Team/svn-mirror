/*
 * c64pla.c -- C64 PLA handling.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "c64pla.h"
#include "datasette.h"
#include "mem.h"


/* Processor port.  */
pport_t pport;

/* Tape motor status.  */
static BYTE old_port_data_out = 0xff;

/* Tape write line status.  */
static BYTE old_port_write_bit = 0xff;


void c64pla_config_changed(int tape_sense, int caps_sense)
{
    pport.data_out = (pport.data_out & ~pport.dir)
                     | (pport.data & pport.dir);

    ram[1] = (((pport.data | ~pport.dir) & (pport.data_out | 0x17))
             & 0xbf) | (caps_sense << 6);

    if (!(pport.dir & 0x20))
      ram[1] &= 0xdf;

    if (tape_sense && !(pport.dir & 0x10))
      ram[1] &= 0xef;

    if (((pport.dir & pport.data) & 0x20) != old_port_data_out) {
        old_port_data_out = (pport.dir & pport.data) & 0x20;
        datasette_set_motor(!old_port_data_out);
    }

    if (((~pport.dir | pport.data) & 0x8) != old_port_write_bit) {
        old_port_write_bit = (~pport.dir | pport.data) & 0x8;
        datasette_toggle_write_bit((~pport.dir | pport.data) & 0x8);
    }

    ram[0] = pport.dir;
}

