/*
 *
 * cia1571drive1.c - Definitions for the MOS6526 (CIA) chip in the 1571
 * disk drive ($4000).
 *
 * Written by
 *   Andreas Boose (boose@linux.rz.fh-hannover.de)
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
 * */

#include "ciacore.h"

/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init cia1571d1_init
#define reset_mycia reset_cia1571d1
#define store_mycia store_cia1571d1
#define read_mycia read_cia1571d1
#define peek_mycia peek_cia1571d1
#define mycpu_clk_guard drive1_clk_guard
#define mycia_prevent_clk_overflow cia1571d1_prevent_clk_overflow
#define mycia_set_flag cia1571d1_set_flag
#define mycia_set_sdr cia1571d1_set_sdr
#define mycia_write_snapshot_module cia1571d1_write_snapshot_module
#define mycia_read_snapshot_module cia1571d1_read_snapshot_module
#define mycia_debugFlag cia1571d1_debugFlag

#define MYCIA_NAME "CIA1571D1"

/*************************************************************************
 * CPU binding
 */

#include "drive.h"
#include "ciad.h"
#include "iecdrive.h"

#include "interrupt.h"

#define MYCIA_INT       IK_IRQ

#define myclk drive_clk[1]

#define cia_set_int_clk(value,clk) \
                set_int(&drive1_int_status,(I_CIA1FL),(value),(clk))

#define cia_restore_int(value) \
                set_int_noclk(&drive1_int_status,(I_CIA1FL),(value))

#define mycpu_alarm_context drive1_alarm_context

static inline void do_reset_cia(void)
{

}

static inline void pulse_ciapc(CLOCK rclk) { }


#define PRE_STORE_CIA
#define PRE_READ_CIA
#define PRE_PEEK_CIA

static inline void store_sdr(BYTE byte)
{
    iec_fast_drive_write(cia[CIA_SDR]);
}

static inline void undump_ciapa(CLOCK rclk, BYTE b)
{

}

static inline void undump_ciapb(CLOCK rclk, BYTE b)
{

}

static inline void store_ciapa(CLOCK rclk, BYTE byte)
{

}

static inline void store_ciapb(CLOCK rclk, BYTE byte)
{

}

static inline BYTE read_ciapa(void)
{
    return (0xff & ~cia[CIA_DDRA]) | (cia[CIA_PRA] & cia[CIA_DDRA]);
}

static inline BYTE read_ciapb(void)
{
    return (0xff & ~cia[CIA_DDRB]) | (cia[CIA_PRB] & cia[CIA_DDRB]);
}

static inline void read_ciaicr(void)
{

}

#include "ciacore.c"

/* POST_CIA_FUNCS */

