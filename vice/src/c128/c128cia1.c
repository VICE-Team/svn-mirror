/* -*- C -*-

 * c128cia1.c - Definitions for the first MOS6526 (CIA) chip in the C128
 * ($DC00).
 *
 * Written by
 *   André Fachat (fachat@physik.tu-chemnitz.de)
 *   Ettore Perazzoli (ettore@comm2000.it)
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

#define mycia_init cia1_init
#define mycia_reset cia1_reset
#define store_mycia store_cia1
#define read_mycia read_cia1
#define peek_mycia peek_cia1
#define mycia_set_flag cia1_set_flag
#define mycia_set_sdr cia1_set_sdr
#define mycia_write_snapshot_module cia1_write_snapshot_module
#define mycia_read_snapshot_module cia1_read_snapshot_module
#define mycia_debugFlag cia1_debugFlag

#define MYCIA_NAME "CIA1"

/*************************************************************************
 * CPU binding
 */

#include "maincpu.h"
#include "interrupt.h"

#define MYCIA_INT       IK_IRQ

#define myclk 		clk
#define mycpu_clk_guard maincpu_clk_guard
#define	mycpu_rmw_flag	rmw_flag

#define cia_set_int_clk(value,clk) \
                set_int(&maincpu_int_status,(I_CIA1FL),(value),(clk))

#define cia_restore_int(value) \
                set_int_noclk(&maincpu_int_status,(I_CIA1FL),(value))

#define mycpu_alarm_context maincpu_alarm_context

/*************************************************************************
 * I/O
 */

#include "c64cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "kbd.h"
#include "iecdrive.h"
#include "keyboard.h"
#ifdef HAVE_RS232
#include "rsuser.h"
#endif
#include "vicii.h"

/* Flag: Are the 3 C128 extended rows enabled?  */
static int extended_keyboard_rows_enabled;

/* Mask for the extended keyboard rows.  */
static BYTE extended_keyboard_rows_mask;

void cia1_enable_extended_keyboard_rows(int flag)
{
    extended_keyboard_rows_enabled = flag;
}

void cia1_set_extended_keyboard_rows_mask(BYTE value)
{
    extended_keyboard_rows_mask = value;
}

static inline void pulse_ciapc(CLOCK rclk) { }

#define PRE_STORE_CIA \
    vic_ii_handle_pending_alarms(maincpu_num_write_cycles());

#define PRE_READ_CIA \
    vic_ii_handle_pending_alarms(0);

#define PRE_PEEK_CIA \
    vic_ii_handle_pending_alarms(0);

static inline void do_reset_cia(void) {}
static inline void store_ciapa(CLOCK rclk, BYTE b) {}
static inline void undump_ciapa(CLOCK rclk, BYTE b) {}

static inline void store_sdr(BYTE byte)
{
    iec_fast_cpu_write(cia[CIA_SDR]);
#ifdef HAVE_RS232
    if (rsuser_enabled) {
	rsuser_tx_byte(cia[CIA_SDR]);
    }
#endif
}

static inline void store_ciapb(CLOCK rclk, BYTE byte)
{
    {
	/* Handle software-triggered light pen.  */
	if ( (byte ^ oldpb) & 0x10) {
	    vic_ii_trigger_light_pen(rclk);
	}
    }
}

static inline void undump_ciapb(CLOCK rclk, BYTE byte) {}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapa(void)
{
    BYTE byte;
    {
	BYTE val = oldpa;
	BYTE msk = (oldpb) & ~joystick_value[2];
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~rev_keyarr[i];
	byte = val & ~joystick_value[2];
    }
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapb(void)
{
    BYTE byte;
    {
	BYTE val = ~cia[CIA_DDRB];
	BYTE msk = (oldpa) & ~joystick_value[1];
	BYTE m;
	int i;

	for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
	    if (!(msk & m))
		val &= ~keyarr[i];

	if (extended_keyboard_rows_enabled)
	    for (m = 0x1, i = 8; i < 11; m <<= 1, i++)
		if (!(extended_keyboard_rows_mask & m))
		    val &= ~keyarr[i];

	byte = (val | (cia[CIA_PRB] & cia[CIA_DDRB]) ) & ~joystick_value[1];
    }
    return byte;
}

static inline void read_ciaicr(void)
{
    if (drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].enable)
        drive1_cpu_execute(clk);
}

#include "ciacore.c"

/* POST_CIA_FUNCS */

