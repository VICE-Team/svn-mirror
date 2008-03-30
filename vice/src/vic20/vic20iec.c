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
#include "iecdrive.h"
#include "resources.h"
#include "drive.h"
#include "viad.h"
#include "ciad.h"

#define NOT(x) ((x)^1)

static BYTE cpu_data, cpu_clock, cpu_atn;
static BYTE drive_data, drive_clock, drive_atna, drive_data_modifier;
static BYTE drive2_data, drive2_clock, drive2_atna, drive2_data_modifier;
static BYTE bus_data, bus_clock, bus_atn;
static BYTE cpu_bus_val;
static BYTE drive_bus_val, drive2_bus_val;

inline void resolve_bus_signals(void)
{
    bus_atn = NOT(cpu_atn);
    bus_clock = NOT(cpu_clock) & (drive[0].enable ? NOT(drive_clock) : 0xff)
                               & (drive[1].enable ? NOT(drive2_clock) : 0xff);
    bus_data = (drive[0].enable
                     ? NOT(drive_data) & NOT(drive_data_modifier) : 0xff)
                 & (drive[1].enable 
                     ? NOT(drive2_data) & NOT(drive2_data_modifier) : 0xff)
                 & NOT(cpu_data);
#if BUS_DBG
    printf("SB: [%ld]  data:%d clock:%d atn:%d\n",
	   drive_clk[0], bus_data, bus_clock, bus_atn);
#endif
}

void iec_drive0_write(BYTE data)
{
    static int last_write = 0;

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

void iec_drive1_write(BYTE data)
{
    static int last_write = 0;

    data = ~data;
    drive2_data = ((data & 2) >> 1);
    drive2_clock = ((data & 8) >> 3);
    drive2_atna = ((data & 16) >> 4);
    drive2_data_modifier = (NOT(cpu_atn) ^ NOT(drive2_atna));

    if (last_write != (data & 26)) {
        resolve_bus_signals();
    }
    last_write = data & 26;
}

BYTE iec_drive0_read(void)
{
    drive_bus_val = bus_data | (bus_clock << 2) | (bus_atn << 7);
    return drive_bus_val;
}

BYTE iec_drive1_read(void)
{
    drive2_bus_val = bus_data | (bus_clock << 2) | (bus_atn << 7);
    return drive2_bus_val;
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
    if (!drive[0].enable && !drive[1].enable)
	return 0;

    if (drive[0].enable)
        drive0_cpu_execute();
    if (drive[1].enable)
        drive1_cpu_execute();

    cpu_bus_val = (bus_data << 1) | (bus_clock << 0) | (bus_atn << 7);

    return cpu_bus_val;
}

void iec_pa_write(BYTE data)
{
    static int last_write = 0;

    if (!drive[0].enable && !drive[1].enable)
	return;

    if (drive[0].enable)
        drive0_cpu_execute();
    if (drive[1].enable)
        drive1_cpu_execute();

    /* Signal ATN interrupt to the drives.  */
    if ((cpu_atn == 0) && (data & 128)) {
        if (drive[0].enable) {
            if (drive[0].type != DRIVE_TYPE_1581)
                via1d0_signal(VIA_SIG_CA1, VIA_SIG_RISE);
            else
                cia1581d0_set_flag();
        }
        if (drive[1].enable) {
            if (drive[1].type != DRIVE_TYPE_1581)
                via1d1_signal(VIA_SIG_CA1, VIA_SIG_RISE);
            else
                cia1581d1_set_flag();
        }
    }

    /* Release ATN signal.  */
    if (!(data & 128)) {
        if (drive[0].enable) {
            if (drive[0].type != DRIVE_TYPE_1581)
                via1d0_signal(VIA_SIG_CA1, 0);
        }
        if (drive[1].enable) {
            if (drive[1].type != DRIVE_TYPE_1581)
                via1d1_signal(VIA_SIG_CA1, 0);
        }
    }

    cpu_atn = ((data & 128) >> 7);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));
    drive2_data_modifier = (NOT(cpu_atn) ^ NOT(drive2_atna));

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

    if (!drive[0].enable && !drive[1].enable)
	return;

    if (drive[0].enable)
        drive0_cpu_execute();
    if (drive[1].enable)
        drive1_cpu_execute();

    cpu_data = ((data & 32) >> 5);
    cpu_clock = ((data & 2) >> 1);
    drive_data_modifier = (NOT(cpu_atn) ^ NOT(drive_atna));
    drive2_data_modifier = (NOT(cpu_atn) ^ NOT(drive2_atna));

    if (last_write != (data & 34))
	resolve_bus_signals();

    last_write = data & 34;
}

void iec_fast_drive_write(BYTE data)
{
/* The VIC20 does not use fast IEC.  */
}

iec_info_t *iec_get_drive_port(void)
{
    return NULL;
}

void parallel_cable_drive_write(BYTE data, int handshake)
{
}

BYTE parallel_cable_drive_read(int handshake)
{
    return 0;
}

int iec_available_busses(void)
{
    return IEC_BUS_IEC /* | IEC_BUS_IEEE */;
}


