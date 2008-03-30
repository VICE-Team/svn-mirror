/*
 * vic20ieeevia2.c - IEEE488 interface VIA2 emulation in the VIC-1112.
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

#define myvia_reset     ieeevia2_reset

#define myvia_store     ieeevia2x_store
#define myvia_read      ieeevia2x_read
#define myvia_peek      ieeevia2x_peek

void REGPARM2 myvia_store(via_context_t *via_context, WORD addr, BYTE data);
BYTE REGPARM1 myvia_read(via_context_t *via_context, WORD addr);
BYTE REGPARM1 myvia_peek(via_context_t *via_context, WORD addr);

void REGPARM2 ieeevia2_store(WORD addr, BYTE data)
{
    myvia_store(&(machine_context.ieeevia2), addr, data);
}

BYTE REGPARM1 ieeevia2_read(WORD addr)
{
    return myvia_read(&(machine_context.ieeevia2), addr);
}

BYTE REGPARM1 ieeevia2_peek(WORD addr)
{
    return myvia_peek(&(machine_context.ieeevia2), addr);
}

#define myvia_log       (via_context->log)
#define myvia_signal    ieeevia2_signal
#define myvia_snapshot_read_module ieeevia2_snapshot_read_module
#define myvia_snapshot_write_module ieeevia2_snapshot_write_module


static void via_set_ca2(int state)
{
    parallel_cpu_set_atn((char)(state ? 0 : 1));
}

static void via_set_cb2(int state)
{
    parallel_cpu_set_eoi((BYTE)(state ? 0 : 1));
}

static void via_set_int(via_context_t *via_context, unsigned int int_num,
                        int value)
{
    interrupt_set_irq(maincpu_int_status, int_num, value,
                      *(via_context->clk_ptr));
}

void vic20ieeevia2_setup_context(machine_context_t *machine_context)
{
    machine_context->ieeevia2.context = NULL;

    machine_context->ieeevia2.rmw_flag = &maincpu_rmw_flag;
    machine_context->ieeevia2.clk_ptr = &maincpu_clk;

    sprintf(machine_context->ieeevia2.myname, "IeeeVia2");
    sprintf(machine_context->ieeevia2.my_module_name, "IeeeVia2");
    machine_context->ieeevia2.read_clk = 0;
    machine_context->ieeevia2.read_offset = 0;
    machine_context->ieeevia2.last_read = 0;
    machine_context->ieeevia2.irq_line = IK_IRQ;
    machine_context->ieeevia2.log = LOG_ERR;
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
    parallel_cpu_set_bus(byte);
}

inline static void store_pra(via_context_t *via_context, BYTE byte,
                             BYTE myoldpa, WORD addr)
{
    parallel_cpu_set_bus(byte);
}

static void undump_prb(via_context_t *via_context, BYTE byte)
{
}

inline static void store_prb(via_context_t *via_context, BYTE byte,
                             BYTE myoldpb, WORD addr)
{
}

static void undump_pcr(via_context_t *via_context, BYTE byte)
{
}

static void res_via(via_context_t *via_context)
{
    parallel_cpu_set_bus(0xff); /* all data lines high, because of input mode */
}

inline static BYTE store_pcr(via_context_t *via_context, BYTE byte, WORD addr)
{
#if 0
    if (byte != myvia[VIA_PCR]) {
        register BYTE tmp = byte;
        /* first set bit 1 and 5 to the real output values */
        if((tmp & 0x0c) != 0x0c) tmp |= 0x02;
        if((tmp & 0xc0) != 0xc0) tmp |= 0x20;
        parallel_cpu_set_atn( (byte & 2) ? 0 : 1 );
        parallel_cpu_set_eoi( (byte & 0x20) ? 0 : 1 );
    }
#endif
    return byte;
}

inline static BYTE read_prb(via_context_t *via_context)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    byte = (parallel_bus & ~myvia[VIA_DDRB])
           | (myvia[VIA_PRB] & myvia[VIA_DDRB]);
    return byte;
}

inline static BYTE read_pra(via_context_t *via_context, WORD addr)
{
    return 0xff;
}

static void clk_overflow_callback(via_context_t *, CLOCK, void *);
static void int_myviat1(via_context_t *, CLOCK);
static void int_myviat2(via_context_t *, CLOCK);

static void clk_overflow_callback_ieeevia2(CLOCK sub, void *data)
{
    clk_overflow_callback(&(machine_context.ieeevia2), sub, data);
}

static void int_ieeevia2t1(CLOCK c)
{
    int_myviat1(&(machine_context.ieeevia2), c);
}

static void int_ieeevia2t2(CLOCK c)
{
    int_myviat2(&(machine_context.ieeevia2), c);
}

void ieeevia2_init(via_context_t *via_context)
{
    char buffer[16];

    via_context->log = log_open(via_context->my_module_name);

    sprintf(buffer, "%sT1", via_context->myname);
    via_context->t1_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_ieeevia2t1);
    sprintf(buffer, "%sT2", via_context->myname);
    via_context->t2_alarm = alarm_new(maincpu_alarm_context, buffer,
                            int_ieeevia2t2);

    via_context->int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                        via_context->myname);

    clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback_ieeevia2,
                           NULL);}

#define VIA_SHARED_CODE

#include "viacore.c"

