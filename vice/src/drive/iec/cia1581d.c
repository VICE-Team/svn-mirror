/*
 *
 * cia1581d.c - Definitions for the MOS6526 (CIA) chip in the 1581
 * disk drive ($4000).  Notice that the real 1581 uses a 8520 CIA.
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
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "types.h"

/* set mycia_debugFlag to 1 to get output */
#undef CIA_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define mycia_init      cia1581_init
#define mycia_reset     cia1581_reset
#define mycia_store     cia1581_store
#define mycia_read      cia1581_read
#define mycia_peek      cia1581_peek
#define mycia_set_flag  cia1581_set_flag
#define mycia_set_sdr   cia1581_sdr
#define mycia_snapshot_write_module cia1581_snapshot_write_module
#define mycia_snapshot_read_module cia1581_snapshot_read_module
#define MYCIA_NAME      (ctxptr->cia1581.myname)

/*************************************************************************
 * CPU binding
 */

#define MYCIA_INT       IK_IRQ

#define myclk           (*(ctxptr->clk_ptr))
#define mycpu_clk_guard (ctxptr->cpu.clk_guard)
#define mycpu_rmw_flag  (ctxptr->cpu.rmw_flag)
#define mycpu_alarm_context (ctxptr->cpu.alarm_context)

/* Renaming formerly global variables */
#define ciata           (ctxptr->cia1581.ta)
#define ciatb           (ctxptr->cia1581.tb)
#define cia_read_clk    (ctxptr->cia1581.read_clk)
#define cia_read_offset (ctxptr->cia1581.read_offset)
#define cia_last_read   (ctxptr->cia1581.last_read)
#define mycia_debugFlag (ctxptr->cia1581.debugFlag)
#define ciaier          (ctxptr->cia1581.c_cia[CIA_ICR])
#define cia_ta_alarm    (ctxptr->cia1581.ta_alarm)
#define cia_tb_alarm    (ctxptr->cia1581.tb_alarm)
#define cia_tod_alarm   (ctxptr->cia1581.tod_alarm)
#define ciaint          (ctxptr->cia1581.irqflags)
#define ciardi          (ctxptr->cia1581.rdi)
#define cia_tat         (ctxptr->cia1581.tat)
#define cia_tbt         (ctxptr->cia1581.tbt)
#define cia_todclk      (ctxptr->cia1581.todclk)
#define ciasr_bits      (ctxptr->cia1581.sr_bits)
#define cia_shifter     (ctxptr->cia1581.shifter)
#define cia_sdr_valid   (ctxptr->cia1581.sdr_valid)
#define oldpa           (ctxptr->cia1581.old_pa)
#define oldpb           (ctxptr->cia1581.old_pb)
#define ciatodalarm     (ctxptr->cia1581.todalarm)
#define ciatodlatch     (ctxptr->cia1581.todlatch)
#define ciatodstopped   (ctxptr->cia1581.todstopped)
#define ciatodlatched   (ctxptr->cia1581.todlatched)
#define ciatodticks     (ctxptr->cia1581.todticks)
#define cia_log         (ctxptr->cia1581.log)
#define cia             (ctxptr->cia1581.c_cia)

#define iec_info        (ctxptr->c_iec_info)

#define cia_set_int_clk(value,clk) \
        interrupt_set_irq((ctxptr->cpu.int_status),(I_CIA1FL),(value),(clk))

#define cia_restore_int(value) \
        interrupt_set_irq_noclk((ctxptr->cpu.int_status),(I_CIA1FL),(value))


void cia1581_setup_context(drive_context_t *ctxptr)
{
   ctxptr->cia1581.todticks = 100000;
   ctxptr->cia1581.log = LOG_ERR;
   ctxptr->cia1581.read_clk = 0;
   ctxptr->cia1581.read_offset = 0;
   ctxptr->cia1581.last_read = 0;
   ctxptr->cia1581.debugFlag = 0;
   ctxptr->cia1581.irq_line = IK_IRQ;
   sprintf(ctxptr->cia1581.myname, "CIA1581D%d", ctxptr->mynumber);
}


/*************************************************************************
 * Hardware binding
 */

static inline void do_reset_cia(drive_context_t *ctxptr)
{
    iec_info = iec_get_drive_port();

    ctxptr->drive_ptr->led_status = 1;
}

static inline void pulse_ciapc(drive_context_t *ctxptr, CLOCK rclk) { }

#define PRE_STORE_CIA
#define PRE_READ_CIA
#define PRE_PEEK_CIA

static inline void store_sdr(drive_context_t *ctxptr, BYTE byte)
{
    iec_fast_drive_write(cia_shifter);
}

static inline void undump_ciapa(drive_context_t *ctxptr, CLOCK rclk, BYTE b)
{
    ctxptr->drive_ptr->led_status = (b & 0x40) ? 1 : 0;
}

static inline void undump_ciapb(drive_context_t *ctxptr, CLOCK rclk, BYTE b)
{

}

static inline void store_ciapa(drive_context_t *ctxptr, CLOCK rclk, BYTE byte)
{
    ctxptr->drive_ptr->led_status = (byte & 0x40) ? 1 : 0;
}

static inline void store_ciapb(drive_context_t *ctxptr, CLOCK rclk, BYTE byte)
{
    if (byte != oldpb) {
        if (iec_info != NULL) {
            BYTE *drive_bus, *drive_data;
            if (ctxptr->mynumber == 0) {
                drive_bus = &(iec_info->drive_bus);
                drive_data = &(iec_info->drive_data);
            } else {
                drive_bus = &(iec_info->drive2_bus);
                drive_data = &(iec_info->drive2_data);
            }
            *drive_data = ~byte;
            *drive_bus = ((((*drive_data) << 3) & 0x40)
                | (((*drive_data) << 6)
                & (((*drive_data) | iec_info->cpu_bus) << 3) & 0x80));
            iec_info->cpu_port = iec_info->cpu_bus & iec_info->drive_bus
                & iec_info->drive2_bus;
            iec_info->drive_port = iec_info->drive2_port = (((iec_info->cpu_port >> 4) & 0x4)
                | (iec_info->cpu_port >> 7)
                | ((iec_info->cpu_bus << 3) & 0x80));
        } else {
            ctxptr->func.iec_write((BYTE)(~byte));
        }
    }
}

static inline BYTE read_ciapa(drive_context_t *ctxptr)
{
    return ((8 * (ctxptr->mynumber)) & ~cia[CIA_DDRA])
           | (cia[CIA_PRA] & cia[CIA_DDRA]);
}

static inline BYTE read_ciapb(drive_context_t *ctxptr)
{
    if (iec_info != NULL)
    {
        BYTE *drive_port = (ctxptr->mynumber == 0) ? &(iec_info->drive_port)
                                                   : &(iec_info->drive2_port);
        return (((cia[CIA_PRB] & 0x1a) | (*drive_port)) ^ 0x85)
               | (ctxptr->drive_ptr->read_only ? 0 : 0x40);
    }
    else
        return (((cia[CIA_PRB] & 0x1a) | ctxptr->func.iec_read()) ^ 0x85)
               | (ctxptr->drive_ptr->read_only ? 0 : 0x40);
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

static cia_initdesc_t cia1581_initdesc[2] = {
    { &drive0_context.cia1581,
      clk0_overflow_callback, int_ciad0ta, int_ciad0tb, int_ciad0tod },
    { &drive1_context.cia1581,
      clk1_overflow_callback, int_ciad1ta, int_ciad1tb, int_ciad1tod }
};


void cia1581_init(drive_context_t *ctxptr)
{
    cia_drive_init(ctxptr, cia1581_initdesc);
}


#include "ciacore.c"

/* POST_CIA_FUNCS */

