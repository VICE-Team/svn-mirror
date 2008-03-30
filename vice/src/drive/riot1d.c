/*
 * riot1d.c - RIOT1 emulation in the SFD1001, 8050 and 8250 disk drive.
 *
 * Written by
 *  Andre' Fachat <fachat@physik.tu-chemnitz.de>
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

struct drive_context_s;
#define RIOT_SHARED_CODE
#define RIOTCONTEXT     struct drive_context_s

#include "riotcore.h"

#undef RIOT_TIMER_DEBUG

/*************************************************************************
 * Renaming exported functions
 */

#define myriot_init     riot1_init
#define myriot_signal   riot1_signal
#define myriot_reset    riot1_reset
#define myriot_store    riot1_store
#define myriot_read     riot1_read
#define myriot_peek     riot1_peek
#define myriot_set_flag riot1_set_flag
#define myriot_snapshot_write_module riot1_snapshot_write_module
#define myriot_snapshot_read_module riot1_snapshot_read_module

/*************************************************************************
 * CPU binding
 */

#include "interrupt.h"

#define myclk           (*(ctxptr->clk_ptr))
#define mycpu_clk_guard (ctxptr->cpu.clk_guard)
#define mycpu_rmw_flag  (ctxptr->cpu.rmw_flag)
#define mycpu_alarm_context (ctxptr->cpu.alarm_context)

#define my_set_irq(fl, clk)

#define my_restore_irq(fl)

/*************************************************************************
 * I/O
 */

#include <stdio.h>

#include "clkguard.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "parallel.h"
#include "riotd.h"
#include "types.h"
#include "utils.h"


/* renaming formerly global symbols */
#define riotio          (ctxptr->riot1.riot_io)
#define oldpa           (ctxptr->riot1.old_pa)
#define oldpb           (ctxptr->riot1.old_pb)
#define riot_log        (ctxptr->riot1.log)
#define riot_alarm      (ctxptr->riot1.alarm)
#define riot_read_clk   (ctxptr->riot1.read_clk)
#define riot_read_offset (ctxptr->riot1.read_offset)
#define riot_last_read  (ctxptr->riot1.last_read)
#define edgectrl        (ctxptr->riot1.r_edgectrl)
#define irqfl           (ctxptr->riot1.r_irqfl)
#define irqline         (ctxptr->riot1.r_irqline)
#define ti_write_clk    (ctxptr->riot1.r_write_clk)
#define ti_N            (ctxptr->riot1.r_N)
#define ti_divider      (ctxptr->riot1.r_divider)
#define ti_irqen        (ctxptr->riot1.r_irqen)
#define MYRIOT_NAME     (ctxptr->riot1.myname)

void riot1_setup_context(drive_context_t *ctxptr)
{
    ctxptr->riot1.log = LOG_ERR;
    ctxptr->riot1.read_clk = 0;
    ctxptr->riot1.read_offset = 0;
    ctxptr->riot1.last_read = 0;
    ctxptr->riot1.r_edgectrl = 0;
    ctxptr->riot1.r_irqfl = 0;
    ctxptr->riot1.r_irqline = 0;
    sprintf(ctxptr->riot1.myname, "RIOT1D%d", ctxptr->mynumber);
}


static void undump_pra(drive_context_t *ctxptr, BYTE byte)
{
}

inline static void store_pra(drive_context_t *ctxptr, BYTE byte)
{
}

static void undump_prb(drive_context_t *ctxptr, BYTE byte)
{
    ctxptr->func.parallel_set_bus(byte);
}

inline static void store_prb(drive_context_t *ctxptr, BYTE byte)
{
    ctxptr->func.parallel_set_bus((BYTE)(parallel_atn ? 0xff : byte));
}

void riot1_set_pardata(drive_context_t *ctxptr)
{
    store_prb(ctxptr, oldpb);
}

static void riot_reset(drive_context_t *ctxptr)
{
    store_prb(ctxptr, 0xff);
}

inline static BYTE read_pra(drive_context_t *ctxptr)
{
    return (parallel_bus & ~riotio[1]) | (riotio[0] & riotio[1]);
}

inline static BYTE read_prb(drive_context_t *ctxptr)
{
    return (0xff & ~riotio[3]) | (riotio[2] & riotio[3]);
}


/* special callback handling */
static void clk_overflow_callback(drive_context_t *, CLOCK, void*);
static void int_riot(drive_context_t *, CLOCK);

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&drive0_context, sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    clk_overflow_callback(&drive1_context, sub, data);
}

static void int_riot1d0(CLOCK c)
{
    int_riot(&drive0_context, c);
}

static void int_riot1d1(CLOCK c)
{
    int_riot(&drive1_context, c);
}

static riot_initdesc_t riot1_initdesc[] = {
    { &drive0_context.riot1, clk0_overflow_callback, int_riot1d0 },
    { &drive1_context.riot1, clk1_overflow_callback, int_riot1d1 }
};

void riot1_init(drive_context_t *ctxptr)
{
    riot_drive_init(ctxptr, riot1_initdesc);
}

void riot_drive_init(drive_context_t *ctxptr, const riot_initdesc_t *riot_desc)
{
    char buffer[16];
    const riot_initdesc_t *rd = &riot_desc[ctxptr->mynumber];

    if (rd->riot_ptr->log == LOG_ERR)
        rd->riot_ptr->log = log_open(rd->riot_ptr->myname);

    rd->riot_ptr->alarm = (alarm_t *)xmalloc(sizeof(alarm_t));

    sprintf(buffer, "%sT1", rd->riot_ptr->myname);
    alarm_init(rd->riot_ptr->alarm, mycpu_alarm_context, buffer, rd->int_t1);

    clk_guard_add_callback(mycpu_clk_guard, rd->clk, NULL);
}

#include "riotcore.c"

