/*
 * ieeeflash64pia.c -- PIA chip emulation on IEEE Flash! 64 cart
 *
 * Written by
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
 *  Christopher Bongaarts <cab@bongalow.net>
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

#include "drive.h"
#include "interrupt.h"
#include "maincpu.h"
#include "parallel.h"
#include "ieeeflash64pia.h"
#include "piacore.h"
#include "ieeeflash64.h"
#include "types.h"

/* TODO: consider using mc6821core instead rather than piacore.
    though the two chips are basically identical except for the
    clock pin ("enable" vs. phi2) */

/* ------------------------------------------------------------------------- */
/* Renaming exported functions */

#define MYPIA_NAME      "IF64PIA"

#define mypia_init pia_if64_init
#define mypia_reset pia_if64_reset
#define mypia_store pia_if64_store
#define mypia_read pia_if64_read
#define mypia_peek pia_if64_peek
#define mypia_snapshot_write_module pia_if64_snapshot_write_module
#define mypia_snapshot_read_module pia_if64_snapshot_read_module
#define mypia_signal pia_if64_signal
#define mypia_dump pia_if64_dump

static piareg mypia;
static uint8_t oldpa;
static uint8_t oldpb;

/* ------------------------------------------------------------------------- */
/* CPU binding */
static void my_set_int(unsigned int pia_int_num, int a)
{
/*
    maincpu_set_irq(pia_int_num, a ? IK_IRQ : IK_NONE);
*/
}

static void my_restore_int(unsigned int pia_int_num, int a)
{
/*
    interrupt_restore_irq(maincpu_int_status, pia_int_num,
                          a ? IK_IRQ : IK_NONE);
*/
}

#define mycpu_rmw_flag   maincpu_rmw_flag
#define myclk            maincpu_clk
#define mycpu_int_status maincpu_int_status

/* ------------------------------------------------------------------------- */
/* I/O */
/* - 6821 pin mappings:
        PA0-7 = IEEE data lines D1-8
        PB0 = dev8 switch
        PB1 = dev9/10 switch
        PB2 = dev4 switch
        PB3 = IFC (reset)
        PB4 = EOI
        PB5 = DAV
        PB6 = NRFD
        PB7 = NDAC
        CA2 = ATN
*/

static int ieee_is_out(void) {
    /* c64tpi tracks port bits, lets just cheat and check
       the data bus ddr */
    return mypia.ddr_a != 0;
}

static void pia_set_ca2(int a)
{
    parallel_cpu_set_atn((uint8_t)((a) ? 0 : 1));
}

static void pia_set_cb2(int a)
{
    /* pin unused */
}

static void pia_reset(void)
{
   /* assuming input after reset */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_ndac(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_dav(0);
    parallel_cpu_set_eoi(0);
    parallel_cpu_set_bus(0xff);
}

static void store_pa(uint8_t byte)
{
    if (byte != oldpa) {
        parallel_cpu_set_bus(byte);
        oldpa = byte;
    }
}

static void undump_pa(uint8_t byte)
{
    parallel_cpu_set_bus(byte);
}

static void store_pb(uint8_t byte)
{
    if (byte != oldpb) {
        uint8_t tmp = ~byte;

        if (ieee_is_out()) {
            parallel_cpu_set_ndac(0);
            parallel_cpu_set_nrfd(0);
            parallel_cpu_set_dav((uint8_t)(tmp & 0x20));
            parallel_cpu_set_eoi((uint8_t)(tmp & 0x10));
        } else {
            parallel_cpu_set_nrfd((uint8_t)(tmp & 0x40));
            parallel_cpu_set_ndac((uint8_t)(tmp & 0x80));
            parallel_cpu_set_dav(0);
            parallel_cpu_set_eoi(0);
        }
        oldpb = byte;
    }
}

static void undump_pb(uint8_t byte)
{
    store_pb(byte);
}

static uint8_t read_pa(void)
{
    uint8_t byte;

    drive_cpu_execute_all(maincpu_clk);

#ifdef DEBUG
    if (debug.ieee) {
        log_message(mypia_log,
                    "read pia2 port A %x, parallel_bus=%x, gives %x.",
                    mypia.port_a, parallel_bus,
                    (unsigned int)((parallel_bus & ~mypia.ddr_a)
                     | (mypia.port_a & mypia.ddr_a)));
    }
#endif

    byte = (parallel_bus & ~mypia.ddr_a) | (mypia.port_a & mypia.ddr_a);
    return byte;
}

static uint8_t read_pb(void)
{
    uint8_t byte;

    drive_cpu_execute_all(maincpu_clk);

    byte = 0xf8;
    if (ieee_is_out()) {
        if (parallel_nrfd) {
            byte &= 0xbf;
        }
        if (parallel_ndac) {
            byte &= 0x7f;
        }
    } else {
        if (parallel_dav) {
            byte &= 0xdf;
        }
        if (parallel_eoi) {
            byte &= 0xef;
        }
    }

    /* reflect device routing switches 0 = "off" 1 = "on" */
    byte |= ieeeflash64_switch8;
    byte |= ieeeflash64_switch910 << 1;
    byte |= ieeeflash64_switch4 << 2;

    return byte;
}

#include "piacore.c"
