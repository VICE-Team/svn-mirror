/*
 * vic20via1.c - VIA1 emulation in the VIC20.
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
 *
 */

#include "vice.h"

#include <stdio.h>

#include "alarm.h"
#include "clkguard.h"
#include "datasette.h"
#include "drive.h"
#include "interrupt.h"
#include "keyboard.h"
#include "log.h"
#include "maincpu.h"
#include "types.h"
#include "via.h"
#include "vic20.h"
#include "vic20iec.h"
#include "vic20via.h"


void REGPARM2 via1_store(WORD addr, BYTE data)
{
    viacore_store(&(machine_context.via1), addr, data);
}

BYTE REGPARM1 via1_read(WORD addr)
{
    return viacore_read(&(machine_context.via1), addr);
}

BYTE REGPARM1 via1_peek(WORD addr)
{
    return viacore_peek(&(machine_context.via1), addr);
}

static void set_ca2(int state)
{
}

static void set_cb2(int state)
{
}

static void set_int(via_context_t *via_context, unsigned int int_num,
                    int value)
{
    interrupt_set_irq(maincpu_int_status, int_num, value,
                      *(via_context->clk_ptr));
}

static void undump_acr(via_context_t *via_context, BYTE byte)
{
}

inline void static store_acr(via_context_t *via_context, BYTE byte)
{
}

inline void static store_sr(via_context_t *via_context, BYTE byte)
{
}

inline void static store_t2l(via_context_t *via_context, BYTE byte)
{
}

static void undump_pra(via_context_t *via_context, BYTE byte)
{
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE myoldpa, WORD addr)
{
}

static void undump_prb(via_context_t *via_context, BYTE byte)
{
}

inline static void store_prb(via_context_t *via_context, BYTE byte,
                             BYTE myoldpb, WORD addr)
{
    if ((byte ^ myoldpb) & 8)
        datasette_toggle_write_bit((~(via_context->via[VIA_DDRB]) | byte)
                                   & 0x8);
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
}

static void reset(via_context_t *via_context)
{
/*iec_pcr_write(0x22);*/
}

inline static BYTE store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
    /* FIXME: this should use via_set_ca2() and via_set_cb2() */
    if (byte != via_context->via[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if ((tmp & 0x0c) != 0x0c)
            tmp |= 0x02;
        if ((tmp & 0xc0) != 0xc0) tmp |= 0x20;
           iec_pcr_write(tmp);
    }
    return byte;
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    BYTE byte;
    /* FIXME: not 100% sure about this... */
    BYTE val = ~(via_context->via[VIA_DDRA]);
    BYTE msk = via_context->oldpb;
    BYTE m;
    int i;

    for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
        if (!(msk & m))
            val &= ~rev_keyarr[i];

    byte = val | (via_context->via[VIA_PRA] & via_context->via[VIA_DDRA]);
    return byte;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;
    /* FIXME: not 100% sure about this... */
    BYTE val = ~(via_context->via[VIA_DDRB]);
    BYTE msk = via_context->oldpa;
    int m, i;

    for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
        if (!(msk & m))
            val &= ~keyarr[i];

    /* Bit 7 is mapped to the right direction of the joystick (bit
       3 in `joystick_value[]'). */
    if ((joystick_value[1] | joystick_value[2]) & 0x8)
        val &= 0x7f;

    byte = val | (via_context->via[VIA_PRB] & via_context->via[VIA_DDRB]);
    return byte;
}

static void clk_overflow_callback_via1(CLOCK sub, void *data)
{
    viacore_clk_overflow_callback(&(machine_context.via1), sub, data);
}

static void int_via1t1(CLOCK c)
{
    viacore_intt1(&(machine_context.via1), c);
}

static void int_via1t2(CLOCK c)
{
    viacore_intt2(&(machine_context.via1), c);
}

void via1_init(via_context_t *via_context)
{
    char buffer[16];

    via_context->log = log_open(via_context->my_module_name);

    sprintf(buffer, "%sT1", via_context->myname);
    via_context->t1_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_via1t1);
    sprintf(buffer, "%sT2", via_context->myname);
    via_context->t2_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_via1t2);

    via_context->int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                        via_context->myname);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback_via1, NULL);
}

void vic20via1_setup_context(machine_context_t *machine_context)
{
    via_context_t *via;

    via = &(machine_context->via1);

    via->context = NULL;

    via->rmw_flag = &maincpu_rmw_flag;
    via->clk_ptr = &maincpu_clk;

    sprintf(via->myname, "Via1");
    sprintf(via->my_module_name, "VIA1");
    via->read_clk = 0;
    via->read_offset = 0;
    via->last_read = 0;
    via->irq_line = IK_IRQ;
    via->log = LOG_ERR;

    via->undump_pra = undump_pra;
    via->undump_prb = undump_prb;
    via->undump_pcr = undump_pcr;
    via->undump_acr = undump_acr;
    via->store_pra = store_pra;
    via->store_prb = store_prb;
    via->store_pcr = store_pcr;
    via->store_acr = store_acr;
    via->store_sr = store_sr;
    via->store_t2l = store_t2l;
    via->read_pra = read_pra;
    via->read_prb = read_prb;
    via->set_int = set_int;
    via->set_ca2 = set_ca2;
    via->set_cb2 = set_cb2;
    via->reset = reset;
}

