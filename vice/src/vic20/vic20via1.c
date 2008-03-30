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
#include "vic20.h"
#include "vic20iec.h"
#include "vic20via.h"


#define myclk           (*(via_context->clk_ptr))
#define myvia           (via_context->via)
#define myviaifr        (via_context->ifr)
#define myviaier        (via_context->ier)
#define myviatal        (via_context->tal)
#define myviatbl        (via_context->tbl)
#define myviatau        (via_context->tau)
#define myviatbu        (via_context->tbu)
#define myviatai        (via_context->tai)
#define myviatbi        (via_context->tbi)
#define myviapb7        (via_context->pb7)
#define myviapb7x       (via_context->pb7x)
#define myviapb7o       (via_context->pb7o)
#define myviapb7xx      (via_context->pb7xx)
#define myviapb7sx      (via_context->pb7sx)
#define oldpa           (via_context->oldpa)
#define oldpb           (via_context->oldpb)
#define myvia_ila       (via_context->ila)
#define myvia_ilb       (via_context->ilb)
#define ca2_state       (via_context->ca2_state)
#define cb2_state       (via_context->cb2_state)
#define myvia_t1_alarm  (via_context->t1_alarm)
#define myvia_t2_alarm  (via_context->t2_alarm)

#define via_read_clk    (via_context->read_clk)
#define via_read_offset (via_context->read_offset)
#define via_last_read   (via_context->last_read)
#define snap_module_name (via_context->my_module_name)

#define myvia_int_num   (via_context->int_num)
#define MYVIA_INT       (via_context->irq_line)

#define mycpu_rmw_flag  (*(via_context->rmw_flag))

#define myvia_reset     via1_reset

#define myvia_store     via1x_store
#define myvia_read      via1x_read
#define myvia_peek      via1x_peek

void REGPARM2 myvia_store(via_context_t *via_context, WORD addr, BYTE data);
BYTE REGPARM1 myvia_read(via_context_t *via_context, WORD addr);
BYTE REGPARM1 myvia_peek(via_context_t *via_context, WORD addr);

void REGPARM2 via1_store(WORD addr, BYTE data)
{
    myvia_store(&(machine_context.via1), addr, data);
}

BYTE REGPARM1 via1_read(WORD addr)
{
    return myvia_read(&(machine_context.via1), addr);
}

BYTE REGPARM1 via1_peek(WORD addr)
{
    return myvia_peek(&(machine_context.via1), addr);
}

#define myvia_log       (via_context->log)
#define myvia_signal    via1_signal
#define myvia_snapshot_read_module via1_snapshot_read_module
#define myvia_snapshot_write_module via1_snapshot_write_module


static void via_set_ca2(int state)
{
}

static void via_set_cb2(int state)
{
}

static void via_set_int(via_context_t *via_context, unsigned int int_num,
                        int value)
{
    interrupt_set_irq(maincpu_int_status, int_num, value,
                      *(via_context->clk_ptr));
}

void vic20via1_setup_context(machine_context_t *machine_context)
{
    machine_context->via1.context = NULL;

    machine_context->via1.rmw_flag = &maincpu_rmw_flag;
    machine_context->via1.clk_ptr = &maincpu_clk;

    sprintf(machine_context->via1.myname, "Via1");
    sprintf(machine_context->via1.my_module_name, "VIA1");
    machine_context->via1.read_clk = 0;
    machine_context->via1.read_offset = 0;
    machine_context->via1.last_read = 0;
    machine_context->via1.irq_line = IK_IRQ;
    machine_context->via1.log = LOG_ERR;
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
        datasette_toggle_write_bit((~myvia[VIA_DDRB] | byte) & 0x8);
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
}

static void res_via(via_context_t *via_context)
{
/*iec_pcr_write(0x22);*/
}

inline static BYTE store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
    /* FIXME: this should use via_set_ca2() and via_set_cb2() */
    if (byte != myvia[VIA_PCR]) {
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
    BYTE val = ~myvia[VIA_DDRA];
    BYTE msk = oldpb;
    BYTE m;
    int i;

    for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
        if (!(msk & m))
            val &= ~rev_keyarr[i];

    byte = val | (myvia[VIA_PRA] & myvia[VIA_DDRA]);
    return byte;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;
    /* FIXME: not 100% sure about this... */
    BYTE val = ~myvia[VIA_DDRB];
    BYTE msk = oldpa;
    int m, i;

    for (m = 0x1, i = 0; i < 8; m <<= 1, i++)
        if (!(msk & m))
            val &= ~keyarr[i];

    /* Bit 7 is mapped to the right direction of the joystick (bit
       3 in `joystick_value[]'). */
    if ((joystick_value[1] | joystick_value[2]) & 0x8)
        val &= 0x7f;

    byte = val | (myvia[VIA_PRB] & myvia[VIA_DDRB]);
    return byte;
}

static void clk_overflow_callback(via_context_t *, CLOCK, void *);
static void int_myviat1(via_context_t *, CLOCK);
static void int_myviat2(via_context_t *, CLOCK);

static void clk_overflow_callback_via1(CLOCK sub, void *data)
{
    clk_overflow_callback(&(machine_context.via1), sub, data);
}

static void int_via1t1(CLOCK c)
{
    int_myviat1(&(machine_context.via1), c);
}

static void int_via1t2(CLOCK c)
{
    int_myviat2(&(machine_context.via1), c);
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

#define VIA_SHARED_CODE

#include "viacore.c"

