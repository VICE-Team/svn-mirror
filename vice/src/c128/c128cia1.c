/*
 * c128cia1.c - Definitions for the first MOS6526 (CIA) chip in the C128
 * ($DC00).
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "vice.h"

#include "ciacore.h"

#include "c64cia.h"
#include "drive.h"
#include "drivecpu.h"
#include "iecdrive.h"
#include "keyboard.h"
#include "types.h"
#include "vicii.h"

#ifdef HAVE_RS232
#include "rsuser.h"
#endif

/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init cia1_init
#define mycia_reset cia1_reset
#define mycia_store cia1_store
#define mycia_read cia1_read
#define mycia_peek cia1_peek
#define mycia_set_flag cia1_set_flag
#define mycia_set_sdr cia1_set_sdr
#define mycia_snapshot_write_module cia1_snapshot_write_module
#define mycia_snapshot_read_module cia1_snapshot_read_module
#define mycia_debugFlag cia1_debugFlag

#define MYCIA_NAME "CIA1"

/*************************************************************************
 * CPU binding
 */

#include "interrupt.h"
#include "maincpu.h"

#define MYCIA_INT IK_IRQ

#define myclk            maincpu_clk
#define mycpu_clk_guard  maincpu_clk_guard
#define mycpu_rmw_flag   maincpu_rmw_flag
#define mycpu_int_status maincpu_int_status

static void cia_set_int_clk(int value, CLOCK clk)
{
    interrupt_set_irq(maincpu_int_status, cia_int_num, value, clk);
}

static void cia_restore_int(int value)
{
    interrupt_set_irq_noclk(maincpu_int_status, cia_int_num, value);
}

#define mycpu_alarm_context maincpu_alarm_context

/*************************************************************************
 * I/O
 */

/* Mask for the extended keyboard rows.  */
static BYTE extended_keyboard_rows_mask;

void cia1_set_extended_keyboard_rows_mask(BYTE value)
{
    extended_keyboard_rows_mask = value;
}

static inline void pulse_ciapc(CLOCK rclk)
{

}

#define PRE_STORE_CIA \
    vicii_handle_pending_alarms_external(maincpu_num_write_cycles());

#define PRE_READ_CIA \
    vicii_handle_pending_alarms_external(0);

#define PRE_PEEK_CIA \
    vicii_handle_pending_alarms_external(0);

static inline void do_reset_cia(void)
{

}

static inline void store_ciapa(CLOCK rclk, BYTE b)
{

}

static inline void undump_ciapa(CLOCK rclk, BYTE b)
{

}

static inline void store_ciapb(CLOCK rclk, BYTE byte)
{
    /* Falling edge triggers light pen.  */
    if ((byte ^ 0x10) & oldpb & 0x10)
        vicii_trigger_light_pen(rclk);
}

static inline void undump_ciapb(CLOCK rclk, BYTE byte)
{

}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapa(void)
{
    BYTE byte;
    {
        BYTE val = ~cia[CIA_DDRA];
        BYTE msk = oldpb & ~joystick_value[1];
        BYTE m;
        int i;

        for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
            if (!(msk & m))
                val &= ~rev_keyarr[i];

        byte = (val | (cia[CIA_PRA] & cia[CIA_DDRA])) & ~joystick_value[2];
    }
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapb(void)
{
    BYTE byte;
    {
        BYTE val = ~cia[CIA_DDRB];
        BYTE msk = oldpa & ~joystick_value[2];
        BYTE m;
        int i;

        for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
            if (!(msk & m))
                val &= ~keyarr[i];

        for (m = 0x1, i = 8; i < 11; m <<= 1, i++)
            if (!(extended_keyboard_rows_mask & m))
                val &= ~keyarr[i];

        byte = (val | (cia[CIA_PRB] & cia[CIA_DDRB])) & ~joystick_value[1];
    }
    return byte;
}

static inline void read_ciaicr(void)
{
    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);
}

static inline void read_sdr(void)
{
    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);
}

static inline void store_sdr(BYTE byte)
{
    iec_fast_cpu_write((BYTE)byte);
#ifdef HAVE_RS232
    if (rsuser_enabled) {
        rsuser_tx_byte((BYTE)byte);
    }
#endif
}

#include "ciacore.c"

/* POST_CIA_FUNCS */

