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

struct cia_context_s;
#define CIA_SHARED_CODE
#define CIACONTEXT struct cia_context_s

#include "ciacore.h"

#include "ciad.h"
#include "clkguard.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "lib.h"
#include "types.h"


/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init      cia1571_init
#define mycia_reset     cia1571_reset
#define mycia_store     ciacore1571_store
#define mycia_read      ciacore1571_read
#define mycia_peek      ciacore1571_peek
#define mycia_set_flag  cia1571_set_flag
#define mycia_set_sdr   cia1571_set_sdr
#define mycia_snapshot_write_module cia1571_snapshot_write_module
#define mycia_snapshot_read_module cia1571_snapshot_read_module
#define MYCIA_NAME      (cia_context->myname)

/*************************************************************************
 * CPU binding
 */

#define MYCIA_INT       IK_IRQ

#define myclk           (*(cia_context->clk_ptr))
#define mycpu_rmw_flag  (*(cia_context->rmw_flag))

/* Renaming formerly global variables */
#define ciata           (cia_context->ta)
#define ciatb           (cia_context->tb)
#define cia_read_clk    (cia_context->read_clk)
#define cia_read_offset (cia_context->read_offset)
#define cia_last_read   (cia_context->last_read)
#define mycia_debugFlag (cia_context->debugFlag)
#define ciaier          (cia_context->c_cia[CIA_ICR])
#define cia_ta_alarm    (cia_context->ta_alarm)
#define cia_tb_alarm    (cia_context->tb_alarm)
#define cia_tod_alarm   (cia_context->tod_alarm)
#define ciaint          (cia_context->irqflags)
#define ciardi          (cia_context->rdi)
#define cia_tat         (cia_context->tat)
#define cia_tbt         (cia_context->tbt)
#define cia_todclk      (cia_context->todclk)
#define ciasr_bits      (cia_context->sr_bits)
#define cia_shifter     (cia_context->shifter)
#define cia_sdr_valid   (cia_context->sdr_valid)
#define oldpa           (cia_context->old_pa)
#define oldpb           (cia_context->old_pb)
#define ciatodalarm     (cia_context->todalarm)
#define ciatodlatch     (cia_context->todlatch)
#define ciatodstopped   (cia_context->todstopped)
#define ciatodlatched   (cia_context->todlatched)
#define ciatodticks     (cia_context->todticks)
#define cia_log         (cia_context->log)
#define cia             (cia_context->c_cia)

void REGPARM3 mycia_store(cia_context_t *cia_context, WORD addr, BYTE data);
BYTE REGPARM2 mycia_read(cia_context_t *cia_context, WORD addr);
BYTE REGPARM2 mycia_peek(cia_context_t *cia_context, WORD addr);

void REGPARM3 cia1571_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    mycia_store(&(ctxptr->cia1571), addr, data);
}

BYTE REGPARM2 cia1571_read(drive_context_t *ctxptr, WORD addr)
{
    return mycia_read(&(ctxptr->cia1571), addr);
}

BYTE REGPARM2 cia1571_peek(drive_context_t *ctxptr, WORD addr)
{
    return mycia_peek(&(ctxptr->cia1571), addr);
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

    interrupt_set_irq_noclk(drive_context->cpu.int_status, cia_context->int_num,
                            value);
}


/*************************************************************************
 * Hardware binding
 */

static inline void do_reset_cia(cia_context_t *cia_context)
{
}

static inline void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{
}

#define PRE_STORE_CIA
#define PRE_READ_CIA
#define PRE_PEEK_CIA

static inline void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
}

static inline void undump_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
}

static inline void store_ciapa(cia_context_t *cia_context, CLOCK rclk,
                               BYTE byte)
{
}

static inline void store_ciapb(cia_context_t *cia_context, CLOCK rclk,
                               BYTE byte)
{
}

static inline BYTE read_ciapa(cia_context_t *cia_context)
{
    return (0xff & ~cia[CIA_DDRA]) | (cia[CIA_PRA] & cia[CIA_DDRA]);
}

static inline BYTE read_ciapb(cia_context_t *cia_context)
{
    return (0xff & ~cia[CIA_DDRB]) | (cia[CIA_PRB] & cia[CIA_DDRB]);
}

static inline void read_ciaicr(cia_context_t *cia_context)
{
}

static inline void read_sdr(cia_context_t *cia_context)
{
}

static inline void store_sdr(cia_context_t *cia_context, BYTE byte)
{
    drivecia1571_context_t *cia1571p;

    cia1571p = (drivecia1571_context_t *)(cia_context->prv);

    iec_fast_drive_write((BYTE)byte, cia1571p->number);
}

/* special callback handling */

static void clk_overflow_callback(cia_context_t *, CLOCK, void *);

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&(drive0_context.cia1571), sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&(drive1_context.cia1571), sub, data);
}

static void int_ciad0ta(CLOCK c)
{
    int_ciata(&(drive0_context.cia1571), c);
}

static void int_ciad1ta(CLOCK c)
{
    int_ciata(&(drive1_context.cia1571), c);
}

static void int_ciad0tb(CLOCK c)
{
    int_ciatb(&(drive0_context.cia1571), c);
}

static void int_ciad1tb(CLOCK c)
{
    int_ciatb(&(drive1_context.cia1571), c);
}

static void int_ciad0tod(CLOCK c)
{
    int_ciatod((&drive0_context.cia1571), c);
}

static void int_ciad1tod(CLOCK c)
{
    int_ciatod(&(drive1_context.cia1571), c);
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

    ctxptr->cia1571.prv = lib_malloc(sizeof(drivecia1571_context_t));
    cia1571p = (drivecia1571_context_t *)(ctxptr->cia1571.prv);
    cia1571p->number = ctxptr->mynumber;

    ctxptr->cia1571.context = (void *)ctxptr;

    ctxptr->cia1571.rmw_flag = &(ctxptr->cpu.rmw_flag);
    ctxptr->cia1571.clk_ptr = ctxptr->clk_ptr;

    ctxptr->cia1571.todticks = 100000;
    ctxptr->cia1571.log = LOG_ERR;
    ctxptr->cia1571.read_clk = 0;
    ctxptr->cia1571.read_offset = 0;
    ctxptr->cia1571.last_read = 0;
    ctxptr->cia1571.debugFlag = 0;
    ctxptr->cia1571.irq_line = IK_IRQ;
    sprintf(ctxptr->cia1571.myname, "CIA1571D%d", ctxptr->mynumber);
    ctxptr->cia1571.int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status,
                                       ctxptr->cia1571.myname);

    cia1571p->drive_ptr = ctxptr->drive_ptr;
}

#include "ciacore.c"

/* POST_CIA_FUNCS */

