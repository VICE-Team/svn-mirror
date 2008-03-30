/*
 * vic20ieeevia1.c - IEEE488 interface VIA1 emulation in the VIC-1112.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
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

#include "alarm.h"
#include "clkguard.h"
#include "drive.h"
#include "drivecpu.h"
#include "interrupt.h"
#include "log.h"
#include "maincpu.h"
#include "parallel.h"
#include "types.h"
#include "vic20.h"


void REGPARM2 ieeevia1_store(WORD addr, BYTE data)
{
    viacore_store(&(machine_context.ieeevia1), addr, data);
}

BYTE REGPARM1 ieeevia1_read(WORD addr)
{
    return viacore_read(&(machine_context.ieeevia1), addr);
}

BYTE REGPARM1 ieeevia1_peek(WORD addr)
{
    return viacore_peek(&(machine_context.ieeevia1), addr);
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
    parallel_cpu_set_dav((BYTE)(!(byte & 0x01)));
    parallel_cpu_set_nrfd((BYTE)(!(byte & 0x02)));
    parallel_cpu_set_ndac((BYTE)(!(byte & 0x04)));
}

inline static void store_prb(via_context_t *via_context, BYTE byte,
                             BYTE myoldpb, WORD addr)
{
    parallel_cpu_set_dav((BYTE)(!(byte & 0x01)));
    parallel_cpu_set_nrfd((BYTE)(!(byte & 0x02)));
    parallel_cpu_set_ndac((BYTE)(!(byte & 0x04)));
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
}

static void reset(via_context_t *via_context)
{
    parallel_cpu_set_dav(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_ndac(0);
}

inline static BYTE store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
    return byte;
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    return 0xff;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    byte = 255
        - (parallel_atn  ? 0x80 : 0)
        - (parallel_ndac ? 0x40 : 0)
        - (parallel_nrfd ? 0x20 : 0)
        - (parallel_dav  ? 0x10 : 0)
        - (parallel_eoi  ? 0x08 : 0);

    /* none of the load changes output register value -> std. masking */
    byte = ((byte & ~(via_context->via[VIA_DDRB]))
           | (via_context->via[VIA_PRB] & via_context->via[VIA_DDRB]));
    return byte;
}

static void clk_overflow_callback_ieeevia1(CLOCK sub, void *data)
{
    viacore_clk_overflow_callback(&(machine_context.ieeevia1), sub, data);
}

static void int_ieeevia1t1(CLOCK c)
{
    viacore_intt1(&(machine_context.ieeevia1), c);
}

static void int_ieeevia1t2(CLOCK c)
{
    viacore_intt2(&(machine_context.ieeevia1), c);
}

void ieeevia1_init(via_context_t *via_context)
{
    char buffer[16];

    via_context->log = log_open(via_context->my_module_name);

    sprintf(buffer, "%sT1", via_context->myname);
    via_context->t1_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_ieeevia1t1);
    sprintf(buffer, "%sT2", via_context->myname);
    via_context->t2_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_ieeevia1t2);

    via_context->int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                        via_context->myname);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback_ieeevia1,
                           NULL);
}

void vic20ieeevia1_setup_context(machine_context_t *machine_context)
{
    via_context_t *via;

    via = &(machine_context->ieeevia1);

    via->context = NULL;

    via->rmw_flag = &maincpu_rmw_flag;
    via->clk_ptr = &maincpu_clk;

    sprintf(via->myname, "IeeeVia1");
    sprintf(via->my_module_name, "IeeeVia1");
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

