/*
 * c64cia1.c - Definitions for the first MOS6526 (CIA) chip in the C64
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

#include <stdio.h>

#include "c64.h"
#include "c64cia.h"
#include "clkguard.h"
#include "keyboard.h"
#include "log.h"
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
#define mycia_store ciacore1_store
#define mycia_read ciacore1_read
#define mycia_peek ciacore1_peek
#define mycia_set_flag cia1_set_flag
#define mycia_set_sdr cia1_set_sdr
#define mycia_snapshot_write_module cia1_snapshot_write_module
#define mycia_snapshot_read_module cia1_snapshot_read_module

#define mycia_debugFlag cia1_debugFlag
#define myciat_logfl cia1t_logfl

#include "interrupt.h"
#include "maincpu.h"

#define mycpu_clk_guard  maincpu_clk_guard
#define mycpu_int_status maincpu_int_status

void REGPARM3 mycia_store(cia_context_t *cia_context, WORD addr, BYTE data);
BYTE REGPARM2 mycia_read(cia_context_t *cia_context, WORD addr);
BYTE REGPARM2 mycia_peek(cia_context_t *cia_context, WORD addr);

void REGPARM2 cia1_store(WORD addr, BYTE data)
{
    mycia_store(&(machine_context.cia1), addr, data);
}

BYTE REGPARM1 cia1_read(WORD addr)
{
    return mycia_read(&(machine_context.cia1), addr);
}

BYTE REGPARM1 cia1_peek(WORD addr)
{
    return mycia_peek(&(machine_context.cia1), addr);
}

static void cia_set_int_clk(cia_context_t *cia_context, int value, CLOCK clk)
{
    interrupt_set_irq(maincpu_int_status, cia_context->int_num, value, clk);
}

static void cia_restore_int(cia_context_t *cia_context, int value)
{
    interrupt_set_irq_noclk(maincpu_int_status, cia_context->int_num, value);
}

/*************************************************************************
 * I/O
 */

void cia1_set_extended_keyboard_rows_mask(BYTE value)
{

}

static inline void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{

}

#define PRE_STORE_CIA   \
    vicii_handle_pending_alarms_external(maincpu_num_write_cycles());

#define PRE_READ_CIA    \
    vicii_handle_pending_alarms_external(0);

#define PRE_PEEK_CIA    \
    vicii_handle_pending_alarms_external(0);

static inline void do_reset_cia(cia_context_t *cia_context)
{

}

static inline void store_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{

}

static inline void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{

}

static inline void store_ciapb(cia_context_t *cia_context, CLOCK rclk,
                               BYTE byte)
{
    /* Falling edge triggers light pen.  */
    if ((byte ^ 0x10) & cia_context->old_pb & 0x10)
        vicii_trigger_light_pen(rclk);
}

static inline void undump_ciapb(cia_context_t *cia_context, CLOCK rclk,
                                BYTE byte)
{

}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapa(cia_context_t *cia_context)
{
    BYTE byte;
    {
        BYTE val = ~(cia_context->c_cia[CIA_DDRA]);
        BYTE msk = cia_context->old_pb & ~joystick_value[1];
        BYTE m;
        int i;

        for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
            if (!(msk & m))
                val &= ~rev_keyarr[i];

        byte = (val | (cia_context->c_cia[CIA_PRA]
               & cia_context->c_cia[CIA_DDRA])) & ~joystick_value[2];
    }
    return byte;
}

/* read_* functions must return 0xff if nothing to read!!! */
static inline BYTE read_ciapb(cia_context_t *cia_context)
{
    BYTE byte;
    {
        BYTE val = ~(cia_context->c_cia[CIA_DDRB]);
        BYTE msk = cia_context->old_pa & ~joystick_value[2];
        BYTE m;
        int i;

        for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
            if (!(msk & m))
                val &= ~keyarr[i];

        byte = (val | (cia_context->c_cia[CIA_PRB]
               & cia_context->c_cia[CIA_DDRB])) & ~joystick_value[1];
    }
    return byte;
}

static inline void read_ciaicr(cia_context_t *cia_context)
{
}

static inline void read_sdr(cia_context_t *cia_context)
{
}

static inline void store_sdr(cia_context_t *cia_context, BYTE byte)
{
#ifdef HAVE_RS232
    if (rsuser_enabled) {
        rsuser_tx_byte(byte);
    }
#endif
}

static void int_ciata(cia_context_t *cia_context, CLOCK offset);
static void int_ciatb(cia_context_t *cia_context, CLOCK offset);
static void int_ciatod(cia_context_t *cia_context, CLOCK offset);

static void clk_overflow_callback(cia_context_t *cia_context, CLOCK sub,
                                  void *data);

static void clk_overflow_callback_cia1(CLOCK sub, void *data)
{
    clk_overflow_callback(&(machine_context.cia1), sub, data);
}

static void int_cia1ta(CLOCK offset)
{
    int_ciata(&(machine_context.cia1), offset);
}

static void int_cia1tb(CLOCK offset)
{
    int_ciatb(&(machine_context.cia1), offset);
}

static void int_cia1tod(CLOCK offset)
{
    int_ciatod(&(machine_context.cia1), offset);
}

void cia1_init(cia_context_t *cia_context)
{
    char buffer[16];

    cia_context->log = log_open(cia_context->myname);

    cia_context->int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                        cia_context->myname);

    sprintf(buffer, "%s_TA", cia_context->myname);
    cia_context->ta_alarm = alarm_new(maincpu_alarm_context, buffer,
                                      int_cia1ta);
    sprintf(buffer, "%s_TB", cia_context->myname);
    cia_context->tb_alarm = alarm_new(maincpu_alarm_context, buffer,
                                      int_cia1tb);
    sprintf(buffer, "%s_TOD", cia_context->myname);
    cia_context->tod_alarm = alarm_new(maincpu_alarm_context, buffer,
                                       int_cia1tod);

    clk_guard_add_callback(mycpu_clk_guard, clk_overflow_callback_cia1, NULL);

    sprintf(buffer, "%s_TA", cia_context->myname);
    ciat_init(&(cia_context->ta), buffer, *(cia_context->clk_ptr),
              cia_context->ta_alarm);
    sprintf(buffer, "%s_TB", cia_context->myname);
    ciat_init(&(cia_context->tb), buffer, *(cia_context->clk_ptr),
              cia_context->tb_alarm);
}

void cia1_setup_context(machine_context_t *machine_context)
{
    machine_context->cia1.context = NULL;

    machine_context->cia1.rmw_flag = &maincpu_rmw_flag;
    machine_context->cia1.clk_ptr = &maincpu_clk;

    machine_context->cia1.todticks = 100000;
    machine_context->cia1.log = LOG_ERR;
    machine_context->cia1.read_clk = 0;
    machine_context->cia1.read_offset = 0;
    machine_context->cia1.last_read = 0;
    machine_context->cia1.debugFlag = 0;
    machine_context->cia1.irq_line = IK_IRQ;
    sprintf(machine_context->cia1.myname, "CIA1");
}

#include "ciacore.c"

/* POST_CIA_FUNCS */

