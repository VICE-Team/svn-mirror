/*
 * via2drive1.c - VIA2 emulation in the 1541 disk drive.
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
 *  Daniel Sladic (sladic@eecg.toronto.edu)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#define mycpu drive1
#define myclk drive_clk[1]
#define myrmwf drive1_rmw_flag
#define myvia via2d1

#define I_MYVIAFL I_VIA2D1FL
#define MYVIA_INT VIA2D1_INT
#define A_MYVIAT1 A_VIA2D1T1
#define A_MYVIAT2 A_VIA2D1T2

#define mycpu_int_status drive1_int_status
#define mycpu_unset_alarm drive1_unset_alarm
#define mycpu_set_alarm_clk drive1_set_alarm_clk

#define reset_myvia reset_via2d1
#define store_myvia store_via2d1
#define read_myvia read_via2d1
#define peek_myvia peek_via2d1

#define myvia_log via2d1_log
#define myvia_signal via2d1_signal
#define myvia_prevent_clk_overflow via2d1_prevent_clk_overflow
#define myvia_read_snapshot_module via2d1_read_snapshot_module
#define myvia_write_snapshot_module via2d1_write_snapshot_module

#define int_myviat1 int_via2d1t1
#define int_myviat2 int_via2d1t2

#include "vice.h"
#include "viacore.h"

#include "drive.h"
#include "viad.h"

#define VIA_SET_CA2(a)
#define VIA_SET_CB2(a)

#define	via_set_int	drive1_set_irq
#define	VIA2D1_INT IK_IRQ

/* #define VIA2D1_TIMER_DEBUG */

inline static void store_pra(BYTE byte, BYTE oldpa, ADDRESS addr)
{
    if (drive[1].byte_ready_active == 0x06)
        drive_rotate_disk(&drive[1]);
    drive[1].GCR_write_value = byte;
}

static void undump_pra(BYTE byte)
{

}

inline static void store_prb(BYTE byte, BYTE oldpb, ADDRESS addr)
{
    drive[1].led_status = byte & 8;
    if (((oldpb ^ byte) & 0x3) && (byte & 0x4)) {
        /* Stepper motor */
        if ((oldpb & 0x3) == ((byte + 1) & 0x3))
            drive_move_head(-1, 1);
        else if ((oldpb & 0x3) == ((byte - 1) & 0x3))
            drive_move_head(+1, 1);
    }
    if ((oldpb ^ byte) & 0x60)     /* Zone bits */
        drive[1].rotation_table_ptr = drive[1].rotation_table[(byte >> 5) & 0x3];
    if ((oldpb ^ byte) & 0x04)     /* Motor on/off */
        drive[1].byte_ready_active = (drive[1].byte_ready_active & ~0x04)
                                     | (byte & 0x04);
}

static void undump_prb(BYTE byte)
{
    drive[1].led_status = byte & 8;
    drive[1].rotation_table_ptr = drive[1].rotation_table[(byte >> 5) & 0x3];
    drive[1].byte_ready_active = (drive[1].byte_ready_active & ~0x04)
                                 | (byte & 0x04);
}

inline static BYTE store_pcr(BYTE byte, ADDRESS addr)
{
    /* FIXME: this should use VIA_SET_CA2() and VIA_SET_CB2() */
    if(byte != via2d1[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if((tmp & 0xc0) != 0xc0)
            tmp |= 0x20;
        /* insert_your_favourite_drive_function_here(tmp);
        bit 5 is the write output to the analog circuitry:
        0 = writing, 0x20 = reading */
        drive_update_viad2_pcr(tmp, &drive[1]);
        if ((byte & 0x20) != (via2d1[addr] & 0x20)) {
            if (drive[1].byte_ready_active == 0x06)
                drive_rotate_disk(&drive[1]);
            drive[1].finish_byte = 1;
        }
        byte = tmp;
    }
    return byte;
}

static void undump_pcr(BYTE byte)
{
    drive_update_viad2_pcr(byte, &drive[1]);
}

static void undump_acr(BYTE byte)
{
}

inline void static store_acr(BYTE byte)
{
}

inline void static store_sr(BYTE byte)
{
}

inline void static store_t2l(BYTE byte)
{
}

static void res_via(void)
{
    drive[1].led_status = 8;
    drive_update_ui_status();
}

inline static BYTE read_pra(ADDRESS addr)
{
    BYTE byte;

    if (drive[1].attach_clk != (CLOCK)0) {
        if (drive_clk[1] - drive[1].attach_clk < DRIVE_ATTACH_DELAY)
            drive[1].GCR_read = 0;
        drive[1].attach_clk = (CLOCK)0;
    } else {
        if (drive[1].byte_ready_active == 0x06)
            drive_rotate_disk(&drive[1]);
    }
    byte = ((drive[1].GCR_read & ~via2d1[VIA_DDRA])
        | (via2d1[VIA_PRA] & via2d1[VIA_DDRA] ));
    if (drive[1].type == DRIVE_TYPE_1571)
        if (drive[1].byte_ready)
            drive[1].byte_ready = 0;
    return byte;
}

inline static BYTE read_prb(void)
{
    BYTE byte;

	byte = (drive_read_viad2_prb(&drive[1]) & ~via2d1[VIA_DDRB])
			| (via2d1[VIA_PRB] & via2d1[VIA_DDRB] );
    return byte;
}

#include "viacore.c"

