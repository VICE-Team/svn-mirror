/*
 * riot2d.c - RIOT2 emulation in the SFD1001, 8050 and 8250 disk drive.
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

#define myriot_init     riot2_init
#define myriot_signal   riot2_signal
#define myriot_reset    riot2_reset
#define myriot_store    riot2_store
#define myriot_read     riot2_read
#define myriot_peek     riot2_peek
#define myriot_set_flag riot2_set_flag
#define myriot_snapshot_write_module riot2_snapshot_write_module
#define myriot_snapshot_read_module riot2_snapshot_read_module

/*************************************************************************
 * CPU binding
 */

#include "interrupt.h"
#include "types.h"

#define myclk           (*(ctxptr->clk_ptr))
#define mycpu_clk_guard (ctxptr->cpu.clk_guard)
#define mycpu_rmw_flag  (ctxptr->cpu.rmw_flag)
#define mycpu_alarm_context (ctxptr->cpu.alarm_context)

/*
#define my_set_irq(fl, clk)                          \
        do {                                         \
        printf("set_int_d0(%d)\n",(fl));             \
        interrupt_set_irq(ctxptr->cpu.int_status,    \
                          (ctxptr->riot2p.irq_type), \
                          (fl) ? IK_IRQ : 0, (clk))  \
        ; } while(0)
*/
#define my_set_irq(fl, clk)                          \
        interrupt_set_irq(ctxptr->cpu.int_status,    \
                          (ctxptr->riot2p.irq_type), \
                          (fl) ? IK_IRQ : 0, (clk))

#define my_restore_irq(fl)                              \
        interrupt_set_irq_noclk(ctxptr->cpu.int_status, \
                                (ctxptr->riot2p.irq_type),(fl) ? IK_IRQ : 0)

/*************************************************************************
 * I/O
 */

#include <stdio.h>

#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "parallel.h"
#include "riot.h"
#include "riotd.h"


/* renaming formerly global symbols */
#define riotio          (ctxptr->riot2.riot_io)
#define oldpa           (ctxptr->riot2.old_pa)
#define oldpb           (ctxptr->riot2.old_pb)
#define riot_log        (ctxptr->riot2.log)
#define riot_alarm      (ctxptr->riot2.alarm)
#define riot_read_clk   (ctxptr->riot2.read_clk)
#define riot_read_offset (ctxptr->riot2.read_offset)
#define riot_last_read  (ctxptr->riot2.last_read)
#define edgectrl        (ctxptr->riot2.r_edgectrl)
#define irqfl           (ctxptr->riot2.r_irqfl)
#define irqline         (ctxptr->riot2.r_irqline)
#define ti_write_clk    (ctxptr->riot2.r_write_clk)
#define ti_N            (ctxptr->riot2.r_N)
#define ti_divider      (ctxptr->riot2.r_divider)
#define ti_irqen        (ctxptr->riot2.r_irqen)
#define atn_active      (ctxptr->riot2p.r_atn_active)
#define MYRIOT_NAME     (ctxptr->riot2.myname)

void riot2_setup_context(drive_context_t *ctxptr)
{
    ctxptr->riot2.log = LOG_ERR;
    ctxptr->riot2.read_clk = 0;
    ctxptr->riot2.read_offset = 0;
    ctxptr->riot2.last_read = 0;
    ctxptr->riot2.r_edgectrl = 0;
    ctxptr->riot2.r_irqfl = 0;
    ctxptr->riot2.r_irqline = 0;
    ctxptr->riot2p.r_atn_active = 0;
    ctxptr->riot2p.irq_type = (ctxptr->mynumber == 0) ? I_RIOTD0FL : I_RIOTD1FL;
    sprintf(ctxptr->riot2.myname, "RIOT2D%d", ctxptr->mynumber);
}


_RIOT_FUNC void set_handshake(drive_context_t *ctxptr, BYTE pa)
{
    /* IEEE handshake logic (named as in schematics):
        Inputs: /ATN    = inverted IEEE atn (true = active)
                ATNA    = pa bit 0
                /DACO   = pa bit 1
                RFDO    = pa bit 2
        Output: DACO    = /DACO & (ATN | ATNA) -> to IEEE via MC3446
                RFDO    = (/ATN == ATNA) & RFDO -> to IEEE via MC3446
    */
    /* RFDO = (/ATN == ATNA) & RFDO */
    ctxptr->func.parallel_set_nrfd((char)(
        !( ((atn_active ? 1 : 0) == (pa & 1)) && (pa & 4) )
        ));
    /* DACO = /DACO & (ATNA | ATN) */
    ctxptr->func.parallel_set_ndac((char)(
        !( (!(pa & 2)) && ((pa & 1) || (!atn_active)) )
        ));
}

void drive_riot_set_atn(drive_context_t *ctxptr, int state)
{
    if (DRIVE_IS_OLDTYPE(ctxptr->drive_ptr->type)) {
        if (atn_active && !state) {
            riot2_signal(ctxptr, RIOT_SIG_PA7, RIOT_SIG_FALL);
        } else
        if (state && !atn_active) {
            riot2_signal(ctxptr, RIOT_SIG_PA7, RIOT_SIG_RISE);
        }
        atn_active = state;
        riot1_set_pardata(ctxptr);
        set_handshake(ctxptr, oldpa);
    }
}

_RIOT_FUNC void undump_pra(drive_context_t *ctxptr, BYTE byte)
{
    /* bit 0 = atna */
    set_handshake(ctxptr, byte);
    ctxptr->func.parallel_set_eoi((BYTE)(!(byte & 0x08)));
    ctxptr->func.parallel_set_dav((BYTE)(!(byte & 0x10)));
}

_RIOT_FUNC void store_pra(drive_context_t *ctxptr, BYTE byte)
{
    /* bit 0 = atna */
    /* bit 1 = /daco */
    /* bit 2 = rfdo */
    /* bit 3 = eoio */
    /* bit 4 = davo */
    set_handshake(ctxptr, byte);  /* handle atna, nrfd, ndac */
    ctxptr->func.parallel_set_eoi((BYTE)(!(byte & 0x08)));
    ctxptr->func.parallel_set_dav((BYTE)(!(byte & 0x10)));
}

_RIOT_FUNC void undump_prb(drive_context_t *ctxptr, BYTE byte)
{
    /* bit 3 Act LED 1 */
    /* bit 4 Act LED 0 */
    /* bit 5 Error LED */

    /* 1001 only needs LED 0 and Error LED */
    ctxptr->drive_ptr->led_status = (byte >> 4) & 0x03;

    if ((ctxptr->mynumber == 0) && (DRIVE_IS_DUAL(ctxptr->drive_ptr->type))) {
        drive[1].led_status = ((byte & 8) ? 1 : 0) | ((byte & 32) ? 2 : 0);
    }
}

_RIOT_FUNC void store_prb(drive_context_t *ctxptr, BYTE byte)
{
    /* bit 3 Act LED 1 */
    /* bit 4 Act LED 0 */
    /* bit 5 Error LED */

    /* 1001 only needs LED 0 and Error LED */
    ctxptr->drive_ptr->led_status = (byte >> 4) & 0x03;

    if ((ctxptr->mynumber == 0) && (DRIVE_IS_DUAL(ctxptr->drive_ptr->type))) {
        drive[1].led_status = ((byte & 8) ? 1 : 0) | ((byte & 32) ? 2 : 0);
    }
}

_RIOT_FUNC void riot_reset(drive_context_t *ctxptr)
{
    atn_active = 0;

    ctxptr->func.parallel_set_dav(0);
    ctxptr->func.parallel_set_eoi(0);

    set_handshake(ctxptr, oldpa);

    /* 1001 only needs LED 0 and Error LED */
    ctxptr->drive_ptr->led_status = 3;
}

_RIOT_FUNC BYTE read_pra(drive_context_t *ctxptr)
{
    BYTE byte = 0xff;
    if (!parallel_atn)
        byte -= 0x80;
    if (parallel_dav)
        byte -= 0x40;
    if (parallel_eoi)
        byte -= 0x20;
    return (byte & ~riotio[1]) | (riotio[0] & riotio[1]);
}

_RIOT_FUNC BYTE read_prb(drive_context_t *ctxptr)
{
    BYTE byte = 0xff;
    if (parallel_nrfd)
        byte -= 0x80;
    if (parallel_ndac)
        byte -= 0x40;

    if (ctxptr->mynumber == 0)
        byte -= 1;        /* device address bit 0 */
    byte -= 2;          /* device address bit 1 */
    byte -= 4;          /* device address bit 2 */

    return (byte & ~riotio[3]) | (riotio[2] & riotio[3]);
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

static void int_riot2d0(CLOCK c)
{
    int_riot(&drive0_context, c);
}

static void int_riot2d1(CLOCK c)
{
    int_riot(&drive1_context, c);
}

static riot_initdesc_t riot2_initdesc[] = {
    { &drive0_context.riot2, clk0_overflow_callback, int_riot2d0 },
    { &drive1_context.riot2, clk1_overflow_callback, int_riot2d1 }
};

void riot2_init(drive_context_t *ctxptr)
{
    riot_drive_init(ctxptr, riot2_initdesc);
}

#include "riotcore.c"

