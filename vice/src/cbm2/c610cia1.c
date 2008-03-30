/* -*- C -*-
 *
 * c610cia1.def - Definitions for the MOS6526 (CIA) chip in the CBM-II
 *
 * Written by
 *   Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#define mycia_init cia1_init
#define reset_mycia reset_cia1
#define store_mycia store_cia1
#define read_mycia read_cia1
#define peek_mycia peek_cia1
#define mycpu_clk_guard maincpu_clk_guard
#define mycia_set_flag cia1_set_flag
#define mycia_set_sdr cia1_set_sdr
#define mycia_write_snapshot_module cia1_write_snapshot_module
#define mycia_read_snapshot_module cia1_read_snapshot_module
#define mycia_debugFlag cia1_debugFlag

#define MYCIA_NAME "CIA1"

/*************************************************************************
 * CPU binding
 */

#include "vmachine.h"
#include "maincpu.h"

#include "interrupt.h"

#define MYCIA_INT       IK_IRQ

#define myclk clk


#define cia_set_int_clk(value,clk) \
		tpi1_set_int(2,(value))

#define cia_restore_int(value) \
		tpi1_restore_int(2,(value))

#define mycpu_alarm_context maincpu_alarm_context

/*************************************************************************
 * I/O
 */

#define	cycles_per_sec		     machine_get_cycles_per_second()

#include "pruser.h"
#include "kbd.h"
#include "parallel.h"
#include "drive.h"
#include "c610cia.h"
#include "c610tpi.h"

#define	PRE_STORE_CIA
#define	PRE_READ_CIA
#define	PRE_PEEK_CIA


static int cia1_ieee_is_output;

void cia1_set_ieee_dir(int isout)
{
    cia1_ieee_is_output = isout;
    if(isout) {
	parallel_cpu_set_bus(~oldpa);
    } else {
	parallel_cpu_set_bus(0);
    }
}


static inline void do_reset_cia(void)
{
#ifdef HAVE_PRINTER
    pruser_write_strobe(1);
    pruser_write_data(0xff);
#endif
}

static inline void pulse_ciapc(CLOCK rclk) { }

static inline void store_ciapa(CLOCK rclk, BYTE byte)
{
	/* FIXME: PA0 and PA1 are used as selector for the 
	   Paddle 1/2 selection for the A/D converter. */
 	parallel_cpu_set_bus( cia1_ieee_is_output ? ~byte : 0 );
}

static inline void undump_ciapa(CLOCK rclk, BYTE byte)
{
 	parallel_cpu_set_bus( cia1_ieee_is_output ? ~byte : 0 );
}

static inline void store_sdr(BYTE byte) {}

static inline void undump_ciapb(CLOCK rclk, BYTE b) {}

static inline void store_ciapb(CLOCK rclk, BYTE byte)
{
#ifdef HAVE_PRINTER
    pruser_write_data(byte);
    pruser_write_strobe(0);
    pruser_write_strobe(1);
#endif
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapa(void)
{
    BYTE byte;
    if (drive[0].enable)
        drive0_cpu_execute();
    if (drive[1].enable)
        drive1_cpu_execute();

    /* this reads the 8 bit IEEE488 data bus, but joystick 1 and 2 buttons
       can pull down inputs pa6 and pa7 resp. */
    byte = parallel_bus;
    if(parallel_debug) {
        log_message(LOG_DEFAULT,
                    "read: parallel_bus=%02x, pra=%02x, ddra=%02x -> %02x\n",
                    parallel_bus, cia[CIA_PRA], cia[CIA_DDRA], byte);
    }
    byte = ((byte & ~cia[CIA_DDRA]) | (cia[CIA_PRA] & cia[CIA_DDRA]))
		& ~( ((joystick_value[1] & 0x10) ? 0x40 : 0) 
		| ((joystick_value[2] & 0x10) ? 0x80 : 0) );
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapb(void)
{
    BYTE byte;

    byte = ((0xff & ~cia[CIA_DDRB]) | (cia[CIA_PRB] & cia[CIA_DDRB]))
		& ~( (joystick_value[1] & 0x0f) | ((joystick_value[2] & 0x0f) << 4) );
    return byte;
}

static inline void read_ciaicr(void) {}

#include "ciacore.c"

#ifdef HAVE_PRINTER
void pruser_set_busy(int flank)
{
    if(!flank) {
        cia1_set_flag();
    }
}
#endif


