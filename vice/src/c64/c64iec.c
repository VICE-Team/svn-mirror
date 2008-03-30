/*
 * c64iec.c - IEC bus handling for the C64.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Teemu Rantanen <tvr@cs.hut.fi>
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
#include <string.h>

#include "c64.h"
#include "c64cart.h"
#include "c64cia.h"
#include "c64iec.h"
#include "cartridge.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "maincpu.h"
#include "iecdrive.h"
#include "types.h"
#include "via.h"


/* Status of the IEC bus signals.  */
static iec_info_t iec_info;

static BYTE iec_old_atn = 0x10;
static BYTE parallel_cable_cpu_value = 0xff;
static BYTE parallel_cable_drive0_value = 0xff;
static BYTE parallel_cable_drive1_value = 0xff;

int iec_callback_index = 0;

void c64iec_init(void)
{
    memset(&iec_info, 0xff, sizeof(iec_info_t));
    iec_info.drive_port = 0x85;
}

inline static void iec_update_cpu_bus(BYTE data)
{
    iec_info.cpu_bus = (((data << 2) & 0x80)
                       | ((data << 2) & 0x40)
                       | ((data << 1) & 0x10));
}

inline static void iec_update_ports(void)
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
    drive0_cpu_execute(maincpu_clk);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive[0].type != DRIVE_TYPE_2031) {
            if (drive[0].type != DRIVE_TYPE_1581)
                viacore_signal(&(drive0_context.via1d1541), VIA_SIG_CA1,
                               iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    ciacore_set_flag(&(drive0_context.cia1581));
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
    drive1_cpu_execute(maincpu_clk);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive[1].type != DRIVE_TYPE_2031) {
            if (drive[1].type != DRIVE_TYPE_1581)
                viacore_signal(&(drive1_context.via1d1541), VIA_SIG_CA1,
                               iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    ciacore_set_flag(&(drive1_context.cia1581));
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
    drive0_cpu_execute(maincpu_clk);
    drive1_cpu_execute(maincpu_clk);

    iec_update_cpu_bus(data);

    if (iec_old_atn != (iec_info.cpu_bus & 0x10)) {
        iec_old_atn = iec_info.cpu_bus & 0x10;
        if (drive[0].type != DRIVE_TYPE_2031) {
            if (drive[0].type != DRIVE_TYPE_1581)
                viacore_signal(&(drive0_context.via1d1541), VIA_SIG_CA1,
                               iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    ciacore_set_flag(&(drive0_context.cia1581));
        }
        if (drive[1].type != DRIVE_TYPE_2031) {
            if (drive[1].type != DRIVE_TYPE_1581)
                viacore_signal(&(drive1_context.via1d1541), VIA_SIG_CA1,
                               iec_old_atn ? 0 : VIA_SIG_RISE);
            else
                if (!iec_old_atn)
                    ciacore_set_flag(&(drive1_context.cia1581));
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
    iec_update_cpu_bus(data);
    iec_old_atn = iec_info.cpu_bus & 0x10;
}

#if 0
BYTE iec_cpu_read(void)
{
    if (!drive[0].enable && !drive[1].enable)
        return (iec_info.iec_fast_1541 & 0x30) << 2;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);
    return iec_info.cpu_port;
}
#endif

iec_info_t *iec_get_drive_port(void)
{
    return &iec_info;
}

void parallel_cable_drive0_write(BYTE data, int handshake)
{
    if (handshake)
        ciacore_set_flag(&(machine_context.cia2));
    parallel_cable_drive0_value = data;
}

void parallel_cable_drive1_write(BYTE data, int handshake)
{
    if (handshake)
        ciacore_set_flag(&(machine_context.cia2));
    parallel_cable_drive1_value = data;
}

BYTE parallel_cable_drive_read(int handshake)
{
    if (handshake)
        ciacore_set_flag(&(machine_context.cia2));
    return parallel_cable_cpu_value & parallel_cable_drive0_value
        & parallel_cable_drive1_value;
}

void parallel_cable_cpu_write(BYTE data)
{
    if (!drive[0].enable && !drive[1].enable)
        return;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    parallel_cable_cpu_value = data;
}

BYTE parallel_cable_cpu_read(void)
{
    if (!drive[0].enable && !drive[1].enable)
        return 0;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);
    return parallel_cable_cpu_value & parallel_cable_drive0_value
        & parallel_cable_drive1_value;
}

void parallel_cable_cpu_pulse(void)
{
    if (!drive[0].enable && !drive[1].enable)
        return;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    viacore_signal(&(drive0_context.via1d1541), VIA_SIG_CB1, VIA_SIG_FALL);
    viacore_signal(&(drive1_context.via1d1541), VIA_SIG_CB1, VIA_SIG_FALL);
}

void parallel_cable_cpu_undump(BYTE data)
{
    parallel_cable_cpu_value = data;
}

/* This function is called from ui_update_menus() */
int iec_available_busses(void)
{
    return IEC_BUS_IEC
        | ((c64cart_type == CARTRIDGE_IEEE488) ? IEC_BUS_IEEE : 0);
}

void iec_calculate_callback_index(void)
{
    iec_callback_index = (drive[0].enable ? 1 : 0)
                         | (drive[1].enable ? 2 : 0);
}

void iec_fast_drive_write(BYTE data, unsigned int dnr)
{
/* The C64 does not use fast IEC.  */
}

void iec_fast_drive_direction(int direction, unsigned int dnr)
{
}

