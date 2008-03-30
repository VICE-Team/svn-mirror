/*
 * cia1571d.c - Definitions for the MOS6526 (CIA) chip in the 1571
 * disk drive ($4000).
 *
 * Written by
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

#include "cia.h"
#include "ciad.h"
#include "clkguard.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "types.h"


#define mycia_init      cia1571_init


void REGPARM3 cia1571_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    ciacore_store(&(ctxptr->cia1571), addr, data);
}

BYTE REGPARM2 cia1571_read(drive_context_t *ctxptr, WORD addr)
{
    return ciacore_read(&(ctxptr->cia1571), addr);
}

BYTE REGPARM2 cia1571_peek(drive_context_t *ctxptr, WORD addr)
{
    return ciacore_peek(&(ctxptr->cia1571), addr);
}

static void cia_set_int_clk(cia_context_t *cia_context, int value, CLOCK clk)
{
    drive_context_t *drive_context;

    drive_context = (drive_context_t *)(cia_context->context);

    interrupt_set_irq(drive_context->cpu.int_status, cia_context->int_num,
                      value, clk);
}

static void cia_restore_int(cia_context_t *cia_context, int value)
{
    drive_context_t *drive_context;

    drive_context = (drive_context_t *)(cia_context->context);

    interrupt_restore_irq(drive_context->cpu.int_status, cia_context->int_num,
                            value);
}


/*************************************************************************
 * Hardware binding
 */

static void do_reset_cia(cia_context_t *cia_context)
{
}

static void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{
}

static void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
}

static void undump_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
}

static void store_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
}

static void store_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
}

static BYTE read_ciapa(cia_context_t *cia_context)
{
    return (0xff & ~(cia_context->c_cia[CIA_DDRA]))
        | (cia_context->c_cia[CIA_PRA] & cia_context->c_cia[CIA_DDRA]);
}

static BYTE read_ciapb(cia_context_t *cia_context)
{
    return (0xff & ~(cia_context->c_cia[CIA_DDRB]))
        | (cia_context->c_cia[CIA_PRB] & cia_context->c_cia[CIA_DDRB]);
}

static void read_ciaicr(cia_context_t *cia_context)
{
}

static void read_sdr(cia_context_t *cia_context)
{
}

static void store_sdr(cia_context_t *cia_context, BYTE byte)
{
    drivecia1571_context_t *cia1571p;

    cia1571p = (drivecia1571_context_t *)(cia_context->prv);

    iec_fast_drive_write((BYTE)byte, cia1571p->number);
}

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    ciacore_clk_overflow_callback(&(drive0_context.cia1571), sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    ciacore_clk_overflow_callback(&(drive1_context.cia1571), sub, data);
}

static void int_ciad0ta(CLOCK c)
{
    ciacore_intta(&(drive0_context.cia1571), c);
}

static void int_ciad1ta(CLOCK c)
{
    ciacore_intta(&(drive1_context.cia1571), c);
}

static void int_ciad0tb(CLOCK c)
{
    ciacore_inttb(&(drive0_context.cia1571), c);
}

static void int_ciad1tb(CLOCK c)
{
    ciacore_inttb(&(drive1_context.cia1571), c);
}

static void int_ciad0tod(CLOCK c)
{
    ciacore_inttod((&drive0_context.cia1571), c);
}

static void int_ciad1tod(CLOCK c)
{
    ciacore_inttod(&(drive1_context.cia1571), c);
}

static const cia_initdesc_t cia1571_initdesc[2] = {
    { &drive0_context.cia1571,
      clk0_overflow_callback, int_ciad0ta, int_ciad0tb, int_ciad0tod },
    { &drive1_context.cia1571,
      clk1_overflow_callback, int_ciad1ta, int_ciad1tb, int_ciad1tod }
};

void cia1571_init(drive_context_t *ctxptr)
{
    cia_drive_init(ctxptr, cia1571_initdesc);
}

/* This function is shared between cia1571 and cia1581 */
void cia_drive_init(drive_context_t *ctxptr, const cia_initdesc_t *cia_desc)
{
    char buffer[16];
    const cia_initdesc_t *cd = &cia_desc[ctxptr->mynumber];

    if (cd->cia_ptr->log == LOG_ERR)
        cd->cia_ptr->log = log_open(cd->cia_ptr->myname);

    sprintf(buffer, "%s_TA", cd->cia_ptr->myname);
    cd->cia_ptr->ta_alarm = alarm_new(ctxptr->cpu.alarm_context, buffer,
                                      cd->int_ta);
    sprintf(buffer, "%s_TB", cd->cia_ptr->myname);
    cd->cia_ptr->tb_alarm = alarm_new(ctxptr->cpu.alarm_context, buffer,
                                      cd->int_tb);
    sprintf(buffer, "%s_TOD", cd->cia_ptr->myname);
    cd->cia_ptr->tod_alarm = alarm_new(ctxptr->cpu.alarm_context, buffer,
                                       cd->int_tod);

    clk_guard_add_callback(ctxptr->cpu.clk_guard, cd->clk, NULL);

    sprintf(buffer, "%s_TA", cd->cia_ptr->myname);
    ciat_init(&(cd->cia_ptr->ta), buffer, *(ctxptr->cia1571.clk_ptr),
              cd->cia_ptr->ta_alarm);
    sprintf(buffer, "%s_TB", cd->cia_ptr->myname);
    ciat_init(&(cd->cia_ptr->tb), buffer, *(ctxptr->cia1571.clk_ptr),
              cd->cia_ptr->tb_alarm);
}

void cia1571_setup_context(drive_context_t *ctxptr)
{
    drivecia1571_context_t *cia1571p;
    cia_context_t *cia;

    cia = &(ctxptr->cia1571);

    cia->prv = lib_malloc(sizeof(drivecia1571_context_t));
    cia1571p = (drivecia1571_context_t *)(cia->prv);
    cia1571p->number = ctxptr->mynumber;

    cia->context = (void *)ctxptr;

    cia->rmw_flag = &(ctxptr->cpu.rmw_flag);
    cia->clk_ptr = ctxptr->clk_ptr;

    cia->todticks = 100000;

    ciacore_setup_context(cia);

    cia->debugFlag = 0;
    cia->irq_line = IK_IRQ;
    sprintf(cia->myname, "CIA1571D%d", ctxptr->mynumber);
    cia->int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status, cia->myname);

    cia1571p->drive_ptr = ctxptr->drive_ptr;

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
    cia->pre_store = NULL;
    cia->pre_read = NULL;
    cia->pre_peek = NULL;
}

