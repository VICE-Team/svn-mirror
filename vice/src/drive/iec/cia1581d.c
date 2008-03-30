/*
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
 *
 */

#include "vice.h"

#include <stdio.h>

#include "ciad.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "types.h"


#define mycia_init      cia1581_init


void REGPARM3 cia1581_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    ciacore_store(&(ctxptr->cia1581), addr, data);
}

BYTE REGPARM2 cia1581_read(drive_context_t *ctxptr, WORD addr)
{
    return ciacore_read(&(ctxptr->cia1581), addr);
}

BYTE REGPARM2 cia1581_peek(drive_context_t *ctxptr, WORD addr)
{
    return ciacore_peek(&(ctxptr->cia1581), addr);
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

    interrupt_restore_irq(drive_context->cpu.int_status,
                            cia_context->int_num, value);
}

/*************************************************************************
 * Hardware binding
 */

static void do_reset_cia(cia_context_t *cia_context)
{
    drivecia1581_context_t *cia1581p;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);

    cia1581p->iec_info = iec_get_drive_port();

    cia1581p->drive_ptr->led_status = 1;
}

static void pulse_ciapc(cia_context_t *cia_context, CLOCK rclk)
{
}

#define PRE_STORE_CIA
#define PRE_READ_CIA
#define PRE_PEEK_CIA

static void undump_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
    drivecia1581_context_t *cia1581p;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);

    cia1581p->drive_ptr->led_status = (b & 0x40) ? 1 : 0;
}

static void undump_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE b)
{
}

static void store_ciapa(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    drivecia1581_context_t *cia1581p;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);

    cia1581p->drive_ptr->led_status = (byte & 0x40) ? 1 : 0;
}

static void store_ciapb(cia_context_t *cia_context, CLOCK rclk, BYTE byte)
{
    drivecia1581_context_t *cia1581p;
    drive_context_t *drive_context;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);
    drive_context = (drive_context_t *)(cia_context->context);

    if (byte != cia_context->old_pb) {
        if (cia1581p->iec_info != NULL) {
            BYTE *drive_bus, *drive_data;
            if (cia1581p->number == 0) {
                drive_bus = &(cia1581p->iec_info->drive_bus);
                drive_data = &(cia1581p->iec_info->drive_data);
            } else {
                drive_bus = &(cia1581p->iec_info->drive2_bus);
                drive_data = &(cia1581p->iec_info->drive2_data);
            }
            *drive_data = ~byte;
            *drive_bus = ((((*drive_data) << 3) & 0x40)
                | (((*drive_data) << 6)
                & (((*drive_data) | cia1581p->iec_info->cpu_bus) << 3) & 0x80));
            cia1581p->iec_info->cpu_port = cia1581p->iec_info->cpu_bus & cia1581p->iec_info->drive_bus
                & cia1581p->iec_info->drive2_bus;
            cia1581p->iec_info->drive_port = cia1581p->iec_info->drive2_port
                = (((cia1581p->iec_info->cpu_port >> 4) & 0x4)
                | (cia1581p->iec_info->cpu_port >> 7)
                | ((cia1581p->iec_info->cpu_bus << 3) & 0x80));
        } else {
            drive_context->func.iec_write((BYTE)(~byte));
        }

        iec_fast_drive_direction(byte & 0x20, cia1581p->number);
    }
}

static BYTE read_ciapa(cia_context_t *cia_context)
{
    drivecia1581_context_t *cia1581p;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);

    return ((8 * (cia1581p->number)) & ~(cia_context->c_cia[CIA_DDRA]))
           | (cia_context->c_cia[CIA_PRA] & cia_context->c_cia[CIA_DDRA]);
}

static BYTE read_ciapb(cia_context_t *cia_context)
{
    drive_context_t *drive_context;
    drivecia1581_context_t *cia1581p;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);
    drive_context = (drive_context_t *)(cia_context->context);

    if (cia1581p->iec_info != NULL) {
        BYTE *drive_port;

        drive_port = (cia1581p->number == 0)
                     ? &(cia1581p->iec_info->drive_port)
                     : &(cia1581p->iec_info->drive2_port);
        return (((cia_context->c_cia[CIA_PRB] & 0x1a) | (*drive_port)) ^ 0x85)
            | (cia1581p->drive_ptr->read_only ? 0 : 0x40);
    } else {
        return (((cia_context->c_cia[CIA_PRB] & 0x1a)
            | drive_context->func.iec_read()) ^ 0x85)
            | (cia1581p->drive_ptr->read_only ? 0 : 0x40);
    }
}

static void read_ciaicr(cia_context_t *cia_context)
{
}

static void read_sdr(cia_context_t *cia_context)
{
}

static void store_sdr(cia_context_t *cia_context, BYTE byte)
{
    drivecia1581_context_t *cia1581p;

    cia1581p = (drivecia1581_context_t *)(cia_context->prv);

    iec_fast_drive_write(byte, cia1581p->number);
}

static void clk0_overflow_callback(CLOCK sub, void *data)
{
    ciacore_clk_overflow_callback(&(drive0_context.cia1581), sub, data);
}

static void clk1_overflow_callback(CLOCK sub, void *data)
{
    ciacore_clk_overflow_callback(&(drive1_context.cia1581), sub, data);
}

static void int_ciad0ta(CLOCK c)
{
    ciacore_intta(&(drive0_context.cia1581), c);
}

static void int_ciad1ta(CLOCK c)
{
    ciacore_intta(&(drive1_context.cia1581), c);
}

static void int_ciad0tb(CLOCK c)
{
    ciacore_inttb(&(drive0_context.cia1581), c);
}

static void int_ciad1tb(CLOCK c)
{
    ciacore_inttb(&(drive1_context.cia1581), c);
}

static void int_ciad0tod(CLOCK c)
{
    ciacore_inttod(&(drive0_context.cia1581), c);
}

static void int_ciad1tod(CLOCK c)
{
    ciacore_inttod(&(drive1_context.cia1581), c);
}

static const cia_initdesc_t cia1581_initdesc[2] = {
    { &drive0_context.cia1581,
      clk0_overflow_callback, int_ciad0ta, int_ciad0tb, int_ciad0tod },
    { &drive1_context.cia1581,
      clk1_overflow_callback, int_ciad1ta, int_ciad1tb, int_ciad1tod }
};


void cia1581_init(drive_context_t *ctxptr)
{
    cia_drive_init(ctxptr, cia1581_initdesc);
}

void cia1581_setup_context(drive_context_t *ctxptr)
{
    drivecia1581_context_t *cia1581p;
    cia_context_t *cia;

    cia = &(ctxptr->cia1581);

    cia->prv = lib_malloc(sizeof(drivecia1581_context_t));
    cia1581p = (drivecia1581_context_t *)(cia->prv);
    cia1581p->number = ctxptr->mynumber;

    cia->context = (void *)ctxptr;

    cia->rmw_flag = &(ctxptr->cpu.rmw_flag);
    cia->clk_ptr = ctxptr->clk_ptr;

    cia->todticks = 100000;

    ciacore_setup_context(cia);

    cia->debugFlag = 0;
    cia->irq_line = IK_IRQ;
    sprintf(cia->myname, "CIA1581D%d", ctxptr->mynumber);
    cia->int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status, cia->myname);

    cia1581p->drive_ptr = ctxptr->drive_ptr;
    cia1581p->iec_info = ctxptr->c_iec_info;

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

