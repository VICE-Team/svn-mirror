/*
 * cbm2iec.c - IEC bus handling for the CBM2.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include <stdio.h>

#include "cbm2iec.h"
#include "drive.h"
#include "iecdrive.h"
#include "types.h"


void cbm2iec_init(void)
{
}

void iec_update_ports(void)
{
    /* Not used for now.  */
}

void iec_update_ports_embedded(void)
{
    iec_update_ports();
}

iec_info_t *iec_get_drive_port(void)
{
    return NULL;
}

void parallel_cable_drive0_write(BYTE data, int handshake)
{
}

void parallel_cable_drive1_write(BYTE data, int handshake)
{
}

int iec_available_busses(void)
{
    return IEC_BUS_IEEE;
}

void iec_calculate_callback_index(void)
{
    /* This callback can be used for optimization.  */
}

