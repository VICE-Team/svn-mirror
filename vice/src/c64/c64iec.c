/*
 * c64iec.c - IEC bus handling for the C64.
 *
 * Written by
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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
#include "resources.h"
#include "viad.h"
#include "types.h"
#include "iecdrive.h"
#include "true1541.h"
#include "c64cia.h"

/* FIXME: ugly name `drive_data'.  */
static BYTE drive_bus, drive_data, cpu_bus;

/* This is the IEC line status as seen by the CIA and VIA ports.  */
static BYTE drive_port, cpu_port;

/* This is the IEC line status when true1541 emulation is off.  */
static BYTE iec_fast_1541;

static BYTE parallel_cable_cpu_value = 0xff;
static BYTE parallel_cable_drive_value = 0xff;

inline static void update_ports(void)
{
    cpu_port = cpu_bus & drive_bus;
    drive_port = (((cpu_port >> 4) & 0x4)
		  | (cpu_port >> 7)
		  | ((cpu_bus << 3) & 0x80));
}

void iec_drive_write(BYTE data)
{
    drive_bus = (((data << 3) & 0x40)
		 | ((data << 6) & ((~data ^ cpu_bus) << 3) & 0x80));
    drive_data = data;
    update_ports();
}

BYTE iec_drive_read(void)
{
    return drive_port;
}


/* The C64 has all bus lines in one I/O byte in a CIA.  If this byte is read
   or modified, these routines are called.  */

void iec_cpu_write(BYTE data)
{
    if (!true1541_enabled) {
	iec_fast_1541 = data;
	return;
    }

    true1541_cpu_execute();

    cpu_bus = (((data << 2) & 0x80)
	       | ((data << 2) & 0x40)
	       | ((data << 1) & 0x10));

    /* FIXME: this is slow, we should avoid doing it when not necessary.  */
    set_atn(!(cpu_bus & 0x10));

    drive_bus = (((drive_data << 3) & 0x40)
		 | ((drive_data << 6)
		    & ((~drive_data ^ cpu_bus) << 3)
		    & 0x80));

    update_ports();
}

BYTE iec_cpu_read(void)
{
    if (!true1541_enabled)
	return ((iec_fast_1541 & 0x30) << 2);

    true1541_cpu_execute();
    return cpu_port;
}


void parallel_cable_drive_write(BYTE data, int handshake)
{
    if (handshake)
	cia2_set_flag();
    parallel_cable_drive_value = data;
}

BYTE parallel_cable_drive_read(int handshake)
{
    if (handshake)
	cia2_set_flag();
    return parallel_cable_cpu_value & parallel_cable_drive_value;
}

void parallel_cable_cpu_write(BYTE data, int handshake)
{
    if (!true1541_enabled)
        return;

    true1541_cpu_execute();
    if (handshake)
	viaD1_signal(VIA_SIG_CB1, VIA_SIG_FALL);
    parallel_cable_cpu_value = data;
}

BYTE parallel_cable_cpu_read(void)
{
    if (!true1541_enabled)
        return 0;

    true1541_cpu_execute();
    viaD1_signal(VIA_SIG_CB1, VIA_SIG_FALL);
    return parallel_cable_cpu_value & parallel_cable_drive_value;
}


