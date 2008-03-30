/*
 * c128iec.c - IEC bus handling for the C128.
 *
 * Written by
 *  Andreas Boose (boose@linux.rz.fh-hannover.de)
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

#include <stdio.h>

#include "vice.h"

#include "c64cia.h"
#include "c64iec.h"
#include "ciad.h"
#include "drive.h"
#include "iecdrive.h"
#include "resources.h"
#include "types.h"
#include "viad.h"

iec_info_t iec_info; /* FIXME: Make static again */

static BYTE iec_old_atn = 0x10;
static BYTE parallel_cable_cpu_value = 0xff;
static BYTE parallel_cable_drive0_value = 0xff;
static BYTE parallel_cable_drive1_value = 0xff;

int iec_callback_index = 0;

inline void iec_update_ports(void)
{
    iec_info.cpu_port = iec_info.cpu_bus & iec_info.drive_bus
                          & iec_info.drive2_bus;
    iec_info.drive_port = iec_info.drive2_port = (((iec_info.cpu_port >> 4)
          & 0x4)
                           | (iec_info.cpu_port >> 7)
                           | ((iec_info.cpu_bus << 3) & 0x80));
}

void iec_update_ports_embedded(void)
{
    iec_update_ports();
}

void iec_drive0_write(BYTE data)
{
    iec_info.drive_bus = (((data << 3) & 0x40)
                          | ((data << 6) & ((~data ^ iec_info.cpu_bus) << 3)
                             & 0x80));
    iec_info.drive_data = data;
    iec_update_ports();
}

void iec_drive1_write(BYTE data)
{
    iec_info.drive2_bus = (((data << 3) & 0x40)
                          | ((data << 6) & ((~data ^ iec_info.cpu_bus) << 3)
                             & 0x80));
    iec_info.drive2_data = data;
    iec_update_ports();
}

BYTE iec_drive0_read(void)
{
    return iec_info.drive_port;
}

BYTE iec_drive1_read(void)
{
    return iec_info.drive2_port;
}


/* The C64 has all bus lines in one I/O byte in a CIA.  If this byte is read
   or modified, these routines are called.  */

/* No drive is enabled.  */
void iec_cpu_write_conf0(BYTE data)
{
	iec_info.iec_fast_1541 = data;
}

/* Only the first drive is enabled.  */
void iec_cpu_write_conf1(BYTE data)
{
    drive0_cpu_execute(clk);

    iec_info.cpu_bus = (((data << 2) & 0x80)
                        | ((data << 2) & 0x40)
                        | ((data << 1) & 0x10));

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive[0].type != DRIVE_TYPE_2031) {
            if (drive[0].type != DRIVE_TYPE_1581)
                via1d0_signal(VIA_SIG_CA1, iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    cia1581d0_set_flag();
        }
    }
    if (drive[0].type != DRIVE_TYPE_2031) {
        if (drive[0].type != DRIVE_TYPE_1581)
            iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                          | ((iec_info.drive_data << 6)
                          & ((~iec_info.drive_data ^ iec_info.cpu_bus) << 3)
                          & 0x80));
        else
            iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                          | ((iec_info.drive_data << 6)
                          & ((iec_info.drive_data | iec_info.cpu_bus) << 3)
                          & 0x80));

    }
    iec_update_ports();
}

/* Only the second drive is enabled.  */
void iec_cpu_write_conf2(BYTE data)
{
    drive1_cpu_execute(clk);

    iec_info.cpu_bus = (((data << 2) & 0x80)
                        | ((data << 2) & 0x40)
                        | ((data << 1) & 0x10));

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive[1].type != DRIVE_TYPE_2031) {
            if (drive[1].type != DRIVE_TYPE_1581)
                via1d1_signal(VIA_SIG_CA1, iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    cia1581d1_set_flag();
        }
    }
    if (drive[1].type != DRIVE_TYPE_2031) {
        if (drive[1].type != DRIVE_TYPE_1581)
            iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                          | ((iec_info.drive2_data << 6)
                          & ((~iec_info.drive2_data ^ iec_info.cpu_bus) << 3)
                          & 0x80));
        else
            iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                          | ((iec_info.drive2_data << 6)
                          & ((iec_info.drive2_data | iec_info.cpu_bus) << 3)
                          & 0x80));
    }
    iec_update_ports();
}

/* Both drive are enabled.  */
void iec_cpu_write_conf3(BYTE data)
{
    drive0_cpu_execute(clk);
    drive1_cpu_execute(clk);

    iec_info.cpu_bus = (((data << 2) & 0x80)
                        | ((data << 2) & 0x40)
                        | ((data << 1) & 0x10));

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive[0].type != DRIVE_TYPE_2031) {
            if (drive[0].type != DRIVE_TYPE_1581)
                via1d0_signal(VIA_SIG_CA1, iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    cia1581d0_set_flag();
        }
        if (drive[1].type != DRIVE_TYPE_2031) {
            if (drive[1].type != DRIVE_TYPE_1581)
                via1d1_signal(VIA_SIG_CA1, iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    cia1581d1_set_flag();
        }
    }
    if (drive[0].type != DRIVE_TYPE_2031) {
        if (drive[0].type != DRIVE_TYPE_1581)
            iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                          | ((iec_info.drive_data << 6)
                          & ((~iec_info.drive_data ^ iec_info.cpu_bus) << 3)
                          & 0x80));
        else
            iec_info.drive_bus = (((iec_info.drive_data << 3) & 0x40)
                          | ((iec_info.drive_data << 6)
                          & ((iec_info.drive_data | iec_info.cpu_bus) << 3)
                          & 0x80));
    }
    if (drive[1].type != DRIVE_TYPE_2031) {
        if (drive[1].type != DRIVE_TYPE_1581)
            iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                          | ((iec_info.drive2_data << 6)
                          & ((~iec_info.drive2_data ^ iec_info.cpu_bus) << 3)
                          & 0x80));
        else
            iec_info.drive2_bus = (((iec_info.drive2_data << 3) & 0x40)
                          | ((iec_info.drive2_data << 6)
                          & ((iec_info.drive2_data | iec_info.cpu_bus) << 3)
                          & 0x80));
    }
    iec_update_ports();
}

void iec_cpu_undump(BYTE data)
{
    iec_info.cpu_bus = (((data << 2) & 0x80)
                        | ((data << 2) & 0x40)
                        | ((data << 1) & 0x10));
    iec_old_atn = iec_info.cpu_bus & 0x10;
}

BYTE iec_cpu_read(void)
{
    if (!drive[0].enable && !drive[1].enable)
	return (iec_info.iec_fast_1541 & 0x30) << 2;

    if (drive[0].enable)
	drive0_cpu_execute(clk);
    if (drive[1].enable)
	drive1_cpu_execute(clk);
    return iec_info.cpu_port;
}

void iec_fast_cpu_write(BYTE data)
{
    if (drive[0].enable) {
           drive0_cpu_execute(clk);
       if (drive[0].type == 1571)
           cia1571d0_set_sdr(data);
       if (drive[0].type == 1581)
           cia1581d0_set_sdr(data);
    }
    if (drive[1].enable) {
           drive1_cpu_execute(clk);
       if (drive[1].type == 1571)
           cia1571d1_set_sdr(data);
       if (drive[1].type == 1581)
           cia1581d1_set_sdr(data);
    }
}

void iec_fast_drive_write(BYTE data)
{
    cia1_set_sdr(data);
}

iec_info_t *iec_get_drive_port(void)
{
    return &iec_info;
}

void parallel_cable_drive0_write(BYTE data, int handshake)
{
    if (handshake)
        cia2_set_flag();
    parallel_cable_drive0_value = data;
}

void parallel_cable_drive1_write(BYTE data, int handshake)
{
    if (handshake)
        cia2_set_flag();
    parallel_cable_drive1_value = data;
}

BYTE parallel_cable_drive_read(int handshake)
{
    if (handshake)
	cia2_set_flag();
    return parallel_cable_cpu_value & parallel_cable_drive0_value
        & parallel_cable_drive1_value;
}

void parallel_cable_cpu_write(BYTE data)
{
    if (!drive[0].enable && !drive[1].enable)
        return;

    if (drive[0].enable)
	drive0_cpu_execute(clk);
    if (drive[1].enable)
	drive1_cpu_execute(clk);

    parallel_cable_cpu_value = data;
}

void parallel_cable_cpu_pulse(void)
{
    if (!drive[0].enable && !drive[1].enable)
        return;

    if (drive[0].enable)
	drive0_cpu_execute(clk);
    if (drive[1].enable)
	drive1_cpu_execute(clk);

    via1d0_signal(VIA_SIG_CB1, VIA_SIG_FALL);
    via1d1_signal(VIA_SIG_CB1, VIA_SIG_FALL);
}

BYTE parallel_cable_cpu_read(void)
{
    if (!drive[0].enable && !drive[1].enable)
        return 0;

    if (drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].enable)
        drive1_cpu_execute(clk);
    return parallel_cable_cpu_value & parallel_cable_drive0_value
        & parallel_cable_drive1_value;
}

void parallel_cable_cpu_undump(BYTE data)
{
    parallel_cable_cpu_value = data;
}

int iec_available_busses(void)
{
    return IEC_BUS_IEC | IEC_BUS_IEEE;
}

void iec_calculate_callback_index(void)
{
    iec_callback_index = (drive[0].enable ? 1 : 0)
                           | (drive[1].enable ? 2 : 0);
}

