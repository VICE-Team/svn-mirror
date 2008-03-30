/*
 *
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
 * */

#include "vice.h"

struct drive_context_s;
#define CIA_SHARED_CODE
#define CIACONTEXT struct drive_context_s

#include "ciacore.h"

#include "ciad.h"
#include "clkguard.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "types.h"
#include "utils.h"

/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init      cia1571_init
#define mycia_reset     cia1571_reset
#define mycia_store     cia1571_store
#define mycia_read      cia1571_read
#define mycia_peek      cia1571_peek
#define mycia_set_flag  cia1571_set_flag
#define mycia_set_sdr   cia1571_sdr
#define mycia_snapshot_write_module cia1571_snapshot_write_module
#define mycia_snapshot_read_module cia1571_snapshot_read_module
#define MYCIA_NAME      (ctxptr->cia1571.myname)

/*************************************************************************
 * CPU binding
 */

#define MYCIA_INT       IK_IRQ

#define myclk           (*(ctxptr->clk_ptr))
#define mycpu_clk_guard (ctxptr->cpu.clk_guard)
#define mycpu_rmw_flag  (ctxptr->cpu.rmw_flag)
#define mycpu_alarm_context (ctxptr->cpu.alarm_context)

/* Renaming formerly global variables */
#define ciata           (ctxptr->cia1571.ta)
#define ciatb           (ctxptr->cia1571.tb)
#define cia_read_clk    (ctxptr->cia1571.read_clk)
#define cia_read_offset (ctxptr->cia1571.read_offset)
#define cia_last_read   (ctxptr->cia1571.last_read)
#define mycia_debugFlag (ctxptr->cia1571.debugFlag)
#define ciaier          (ctxptr->cia1571.c_cia[CIA_ICR])
#define cia_ta_alarm    (ctxptr->cia1571.ta_alarm)
#define cia_tb_alarm    (ctxptr->cia1571.tb_alarm)
#define cia_tod_alarm   (ctxptr->cia1571.tod_alarm)
#define ciaint          (ctxptr->cia1571.irqflags)
#define ciardi          (ctxptr->cia1571.rdi)
#define cia_tat         (ctxptr->cia1571.tat)
#define cia_tbt         (ctxptr->cia1571.tbt)
#define cia_todclk      (ctxptr->cia1571.todclk)
#define ciasr_bits      (ctxptr->cia1571.sr_bits)
#define cia_shifter     (ctxptr->cia1571.shifter)
#define cia_sdr_valid   (ctxptr->cia1571.sdr_valid)
#define oldpa           (ctxptr->cia1571.old_pa)
#define oldpb           (ctxptr->cia1571.old_pb)
#define ciatodalarm     (ctxptr->cia1571.todalarm)
#define ciatodlatch     (ctxptr->cia1571.todlatch)
#define ciatodstopped   (ctxptr->cia1571.todstopped)
#define ciatodlatched   (ctxptr->cia1571.todlatched)
#define ciatodticks     (ctxptr->cia1571.todticks)
#define cia_log         (ctxptr->cia1571.log)
#define cia             (ctxptr->cia1571.c_cia)


#define cia_set_int_clk(value,clk) \
        interrupt_set_irq((ctxptr->cpu.int_status),(I_CIA1FL),(value),(clk))

#define cia_restore_int(value) \
        interrupt_set_irq_noclk((ctxptr->cpu.int_status),(I_CIA1FL),(value))


void cia1571_setup_context(drive_context_t *ctxptr)
{
    ctxptr->cia1571.todticks = 100000;
    ctxptr->cia1571.log = LOG_ERR;
    ctxptr->cia1571.read_clk = 0;
    ctxptr->cia1571.read_offset = 0;
    ctxptr->cia1571.last_read = 0;
    ctxptr->cia1571.debugFlag = 0;
    ctxptr->cia1571.irq_line = IK_IRQ;
    sprintf(ctxptr->cia1571.myname, "CIA1571D%d", ctxptr->mynumber);
}


/*************************************************************************
 * Hardware binding
 */

static inline void do_reset_cia(drive_context_t *ctxptr)
{

}

static inline void pulse_ciapc(drive_context_t *ctxptr, CLOCK rclk) { }

#define PRE_STORE_CIA
#define PRE_READ_CIA
#define PRE_PEEK_CIA

static inline void store_sdr(drive_context_t *ctxptr, BYTE byte)
{
    iec_fast_drive_write((BYTE)cia_shifter);
}

static inline void undump_ciapa(drive_context_t *ctxptr, CLOCK rclk, BYTE b)
{

}

static inline void undump_ciapb(drive_context_t *ctxptr, CLOCK rclk, BYTE b)
{

}

static inline void store_ciapa(drive_context_t *ctxptr, CLOCK rclk, BYTE byte)
{

}

static inline void store_ciapb(drive_context_t *ctxptr, CLOCK rclk, BYTE byte)
{

}

static inline BYTE read_ciapa(drive_context_t *ctxptr)
{
    return (0xff & ~cia[CIA_DDRA]) | (cia[CIA_PRA] & cia[CIA_DDRA]);
}

static inline BYTE read_ciapb(drive_context_t *ctxptr)
{
    return (0xff & ~cia[CIA_DDRB]) | (cia[CIA_PRB] & cia[CIA_DDRB]);
}

static inline void read_ciaicr(drive_context_t *ctxptr)
{

}



/* special callback handling */

static void clk_overflow_callback(drive_context_t *, CLOCK, void *);

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&drive0_context, sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&drive1_context, sub, data);
}

static void int_ciad0ta(CLOCK c)
{
    int_ciata(&drive0_context, c);
}

static void int_ciad1ta(CLOCK c)
{
    int_ciata(&drive1_context, c);
}

static void int_ciad0tb(CLOCK c)
{
    int_ciatb(&drive0_context, c);
}

static void int_ciad1tb(CLOCK c)
{
    int_ciatb(&drive1_context, c);
}

static void int_ciad0tod(CLOCK c)
{
    int_ciatod(&drive0_context, c);
}

static void int_ciad1tod(CLOCK c)
{
    int_ciatod(&drive1_context, c);
}

static cia_initdesc_t cia1571_initdesc[2] = {
    { &drive0_context.cia1571,
      clk0_overflow_callback, int_ciad0ta, int_ciad0tb, int_ciad0tod },
    { &drive1_context.cia1571,
      clk1_overflow_callback, int_ciad1ta, int_ciad1tb, int_ciad1tod }
};

void cia1571_init(drive_context_t *ctxptr)
{
    cia_drive_init(ctxptr, cia1571_initdesc);
}


/* this function is shared between cia1571 and cia1581 */

void cia_drive_init(drive_context_t *ctxptr, const cia_initdesc_t *cia_desc)
{
    char buffer[16];
    const cia_initdesc_t *cd = &cia_desc[ctxptr->mynumber];

    if (cd->cia_ptr->log == LOG_ERR)
        cd->cia_ptr->log = log_open(cd->cia_ptr->myname);

    cd->cia_ptr->ta_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));
    cd->cia_ptr->tb_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));
    cd->cia_ptr->tod_alarm = (alarm_t *)xmalloc(sizeof(alarm_t));

    sprintf(buffer, "%s_TA", cd->cia_ptr->myname);
    alarm_init(cd->cia_ptr->ta_alarm, mycpu_alarm_context,
               buffer, cd->int_ta);
    sprintf(buffer, "%s_TB", cd->cia_ptr->myname);
    alarm_init(cd->cia_ptr->tb_alarm, mycpu_alarm_context,
               buffer, cd->int_tb);
    sprintf(buffer, "%s_TOD", cd->cia_ptr->myname);
    alarm_init(cd->cia_ptr->tod_alarm, mycpu_alarm_context,
               buffer, cd->int_tod);

    clk_guard_add_callback(mycpu_clk_guard, cd->clk, NULL);

    sprintf(buffer, "%s_TA", cd->cia_ptr->myname);
    ciat_init(&(cd->cia_ptr->ta), buffer, myclk, cd->cia_ptr->ta_alarm);
    sprintf(buffer, "%s_TB", cd->cia_ptr->myname);
    ciat_init(&(cd->cia_ptr->tb), buffer, myclk, cd->cia_ptr->tb_alarm);
}

#include "ciacore.c"

/* POST_CIA_FUNCS */

