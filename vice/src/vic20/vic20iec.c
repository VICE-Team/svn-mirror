/*
 * vic20.c - IEC bus handling for the VIC20.
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
#include "resources.h"
#include "iecdrive.h"

#define NOT(x) ((x)^1)

static BYTE cpu_data, cpu_clock, cpu_atn;
static BYTE drive_data, drive_clock, drive_atna, drive_data_modifier;
static BYTE bus_data, bus_clock, bus_atn;
static BYTE cpu_bus_val;
static BYTE drive_bus_val;

inline void resolve_bus_signals(void)
{
    bus_atn = NOT(cpu_atn);
    bus_clock = (NOT(cpu_clock) & NOT(drive_clock));
    bus_data = (NOT(drive_data) & NOT(drive_data_modifier) & NOT(cpu_data));

#if BUS_DBG
    printf("SB: [%ld]  data:%d clock:%d atn:%d\n",
	   true1541_clk, bus_data, bus_clock, bus_atn);
#endif
}

void iec_drive_write(BYTE data)
{
    static int last_write = 0;

    if (!app_resources.true1541)
	return;

    data = ~data;
    drive_data = ((data & 2) >> 1);
    drive_clock = ((data & 8) >> 3);
    drive_atna = ((data & 16) >> 4);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 26)) {
	resolve_bus_signals();
    }
    last_write = data & 26;
}

BYTE iec_drive_read(void)
{
    if (!app_resources.true1541)
	return 0;

    drive_bus_val = bus_data | (bus_clock << 2) | (bus_atn << 7);

    return drive_bus_val;
}

/*
   The VIC20 has a strange bus layout for the serial IEC bus.

     VIA1 CA2 CLK out
     VIA1 CB1 SRQ in
     VIA1 CB2 DATA out
     VIA2 PA0 CLK in
     VIA2 PA1 DATA in
     VIA2 PA7 ATN out

 */

/* These two routines are called for VIA2 Port A. */

BYTE iec_pa_read(void)

{
    if (!app_resources.true1541)
	return 0;

    true1541_cpu_execute();

    cpu_bus_val = (bus_data << 1) | (bus_clock << 0) | (bus_atn << 7);

    return cpu_bus_val;
}

void iec_pa_write(BYTE data)
{
    static int last_write = 0;

    if (!app_resources.true1541)
	return;

    true1541_cpu_execute();

    if ((cpu_atn == 0) && (data & 128))
	set_atn(1);

    if (!(data & 128))
	set_atn(0);

    cpu_atn = ((data & 128) >> 7);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 128))
	resolve_bus_signals();

    last_write = data & 128;
}


/* This routine is called for VIA1 PCR (= CA2 and CB2).
   Although Cx2 uses three bits for control, we assume the calling routine has
   set bit 5 and bit 1 to the real output value for CB2 (DATA out) and CA2 (CLK
   out) resp. (25apr1997 AF) */

void iec_pcr_write(BYTE data)
{
    static int last_write = 0;

    if (!app_resources.true1541)
	return;

    true1541_cpu_execute();

    cpu_data = ((data & 32) >> 5);
    cpu_clock = ((data & 2) >> 1);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));

    if (last_write != (data & 34))
	resolve_bus_signals();

    last_write = data & 34;
}

void parallel_cable_drive_write(BYTE data, int handshake)
{
}

BYTE parallel_cable_drive_read(int handshake)
{
    return 0;
}
