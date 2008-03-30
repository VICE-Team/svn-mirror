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
#include "interrupt.h"
#include "keyboard.h"
#include "log.h"
#include "maincpu.h"
#include "types.h"
#include "vicii.h"

#ifdef HAVE_RS232
#include "rsuser.h"
#endif


#define mycia_init cia1_init


void REGPARM2 cia1_store(WORD addr, BYTE data)
{
    ciacore_store(&(machine_context.cia1), addr, data);
}

BYTE REGPARM1 cia1_read(WORD addr)
{
    return ciacore_read(&(machine_context.cia1), addr);
}

BYTE REGPARM1 cia1_peek(WORD addr)
{
    return ciacore_peek(&(machine_context.cia1), addr);
}

static void cia_set_int_clk(cia_context_t *cia_context, int value, CLOCK clk)
{
    interrupt_set_irq(maincpu_int_status, cia_context->int_num, value, clk);
}

static void cia_restore_int(cia_context_t *cia_context, int value)
{
    interrupt_restore_irq(maincpu_int_status, cia_context->int_num, value);
}

/*************************************************************************
 * I/O
 */

void cia1_set_extended_keyboard_rows_mask(BYTE value)
{

}

static void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{

}

static void pre_store(void)
{
    vicii_handle_pending_alarms_external(maincpu_num_write_cycles());
}

static void pre_read(void)
{
    vicii_handle_pending_alarms_external(0);
}

static void pre_peek(void)
{
    vicii_handle_pending_alarms_external(0);
}

static void do_reset_cia(cia_context_t *cia_context)
{

}

static void store_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{

}

static void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{

}

static void store_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    /* Falling edge triggers light pen.  */
    if ((byte ^ 0x10) & cia_context->old_pb & 0x10)
        vicii_trigger_light_pen(rclk);
}

static void undump_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{

}

/* read_* functions must return 0xff if nothing to read!!! */
static BYTE read_ciapa(cia_context_t *cia_context)
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
static BYTE read_ciapb(cia_context_t *cia_context)
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

static void read_ciaicr(cia_context_t *cia_context)
{
}

static void read_sdr(cia_context_t *cia_context)
{
}

static void store_sdr(cia_context_t *cia_context, BYTE byte)
{
#ifdef HAVE_RS232
    if (rsuser_enabled) {
        rsuser_tx_byte(byte);
    }
#endif
}

static void clk_overflow_callback_cia1(CLOCK sub, void *data)
{
    ciacore_clk_overflow_callback(&(machine_context.cia1), sub, data);
}

static void int_cia1ta(CLOCK offset)
{
    ciacore_intta(&(machine_context.cia1), offset);
}

static void int_cia1tb(CLOCK offset)
{
    ciacore_inttb(&(machine_context.cia1), offset);
}

static void int_cia1tod(CLOCK offset)
{
    ciacore_inttod(&(machine_context.cia1), offset);
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

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback_cia1,
                           NULL);

    sprintf(buffer, "%s_TA", cia_context->myname);
    ciat_init(&(cia_context->ta), buffer, *(cia_context->clk_ptr),
              cia_context->ta_alarm);
    sprintf(buffer, "%s_TB", cia_context->myname);
    ciat_init(&(cia_context->tb), buffer, *(cia_context->clk_ptr),
              cia_context->tb_alarm);
}

void cia1_setup_context(machine_context_t *machine_context)
{
    cia_context_t *cia;

    cia = &(machine_context->cia1);

    cia->context = NULL;

    cia->rmw_flag = &maincpu_rmw_flag;
    cia->clk_ptr = &maincpu_clk;

    cia->todticks = 100000;
    cia->log = LOG_ERR;
    cia->read_clk = 0;
    cia->read_offset = 0;
    cia->last_read = 0;
    cia->debugFlag = 0;
    cia->irq_line = IK_IRQ;
    sprintf(cia->myname, "CIA1");

    cia->undump_ciapa = undump_ciapa;
    cia->undump_ciapb = undump_ciapb;
    cia->store_ciapa = store_ciapa;
    cia->store_ciapb = store_ciapb;
    cia->store_sdr = store_sdr;
    cia->read_ciapa = read_ciapa;
    cia->read_ciapb = read_ciapb;
    cia->read_ciaicr = read_ciaicr;
    cia->read_sdr = read_sdr;
    cia->cia_set_int_clk = cia_set_int_clk;
    cia->cia_restore_int = cia_restore_int;
    cia->do_reset_cia = do_reset_cia;
    cia->pulse_ciapc = pulse_ciapc;
    cia->pre_store = pre_store;
    cia->pre_read = pre_read;
    cia->pre_peek = pre_peek;
}

