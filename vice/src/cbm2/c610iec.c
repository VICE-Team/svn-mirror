/*
 * petiec.c - IEC bus handling for the VIC20.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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
#include "drive.h"
#include "iecdrive.h"

void iec_update_ports(void)
{
    /* Not used for now.  */
}

void iec_drive0_write(BYTE data)
{
    log_error(LOG_DEFAULT, "BUG: iec_drive0_write(%02x).", data);
}

void iec_drive1_write(BYTE data)
{
    log_error(LOG_DEFAULT, "BUG: iec_drive1_write(%02x).", data);
}

BYTE iec_drive0_read(void)
{
    log_error(LOG_DEFAULT, "BUG: iec_drive0_read().");
    return 0;
}

BYTE iec_drive1_read(void)
{
    log_error(LOG_DEFAULT, "BUG: iec_drive1_read().");
    return 0;
}

void iec_fast_drive_write(BYTE data)
{
/* The VIC20 does not use fast IEC.  */
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

BYTE parallel_cable_drive_read(int handshake)
{
    return 0;
}

int iec_available_busses(void)
{
    return /* IEC_BUS_IEC |*/ IEC_BUS_IEEE;
}

void iec_calculate_callback_index(void)
{
    /* This callback can be used for optimization.  */
}

