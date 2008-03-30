/*
 * tpid.c
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

#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "rotation.h"
#include "tpi.h"
#include "tpid.h"
#include "types.h"


#define mytpi_init tpid_init
#define mytpi_set_int tpid_set_int
#define mytpi_restore_int tpid_restore_int


void REGPARM3 tpid_store(drive_context_t *ctxptr, WORD addr, BYTE data)
{
    tpicore_store(&(ctxptr->tpid), addr, data);
}

BYTE REGPARM2 tpid_read(drive_context_t *ctxptr, WORD addr)
{
    return tpicore_read(&(ctxptr->tpid), addr);
}

BYTE REGPARM2 tpid_peek(drive_context_t *ctxptr, WORD addr)
{
    return tpicore_peek(&(ctxptr->tpid), addr);
}

static void set_int(unsigned int int_num, int value)
{
}

static void restore_int(unsigned int int_num, int value)
{
}

static void set_ca(tpi_context_t *tpi_context, int a)
{
}

static void set_cb(tpi_context_t *tpi_context, int a)
{
}

static void reset(tpi_context_t *tpi_context)
{
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    plus4tcbm_update_pa(0xff, tpip->number);
    plus4tcbm_update_pb(0xff, tpip->number);
    plus4tcbm_update_pc(0xff, tpip->number);
}

static void store_pa(tpi_context_t *tpi_context, BYTE byte)
{
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    plus4tcbm_update_pa(byte, tpip->number);
}

static void store_pb(tpi_context_t *tpi_context, BYTE byte)
{
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    if (tpip->drive_ptr->byte_ready_active == 0x06)
        rotation_rotate_disk(tpip->drive_ptr);

    tpip->drive_ptr->GCR_write_value = byte;
}

static void undump_pa(tpi_context_t *tpi_context, BYTE byte)
{
}

static void undump_pb(tpi_context_t *tpi_context, BYTE byte)
{
}

static void store_pc(tpi_context_t *tpi_context, BYTE byte)
{
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    plus4tcbm_update_pc(byte, tpip->number);

    tpip->drive_ptr->read_write_mode = byte & 0x10;

    if ((byte & 0x10) != (tpi_context->oldpc & 0x10)) {
        if (tpip->drive_ptr->byte_ready_active == 0x06)
            rotation_rotate_disk(tpip->drive_ptr);
        rotation_change_mode(tpip->number);
    }
}

static void undump_pc(tpi_context_t *tpi_context, BYTE byte)
{
}

static BYTE read_pa(tpi_context_t *tpi_context)
{
    /* TCBM data port */
    BYTE byte;
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    byte = (tpi_context->c_tpi[TPI_PA] | ~(tpi_context->c_tpi)[TPI_DDPA])
           & plus4tcbm_outputa[tpip->number];

    tpip->drive_ptr->byte_ready_level = 0;

    return byte;
}

static BYTE read_pb(tpi_context_t *tpi_context)
{
    /* GCR data port */
    BYTE byte;
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    rotation_byte_read(tpip->drive_ptr);

    byte = (tpi_context->c_tpi[TPI_PB] | ~(tpi_context->c_tpi)[TPI_DDPB])
           & tpip->drive_ptr->GCR_read;

    tpip->drive_ptr->byte_ready_level = 0;

    return byte;
}

static BYTE read_pc(tpi_context_t *tpi_context)
{
    /* TCBM control / GCR data control */
    BYTE byte;
    drivetpi_context_t *tpip;

    tpip = (drivetpi_context_t *)(tpi_context->prv);

    if (tpip->drive_ptr->byte_ready_active == 0x06)
        rotation_rotate_disk(tpip->drive_ptr);

    byte = (tpi_context->c_tpi[TPI_PC] | ~(tpi_context->c_tpi)[TPI_DDPC])
           /* Bit 0, 1 */
           & (plus4tcbm_outputb[tpip->number] | ~0x03)
           /* Bit 3 */
           & ((plus4tcbm_outputc[tpip->number] >> 4) | ~0x08)
           /* Bit 5 */
           & (~0x20)
           /* Bit 6 */
           & (rotation_sync_found(tpip->drive_ptr) ? 0xff : ~0x40)
           /* Bit 7 */
           & ((plus4tcbm_outputc[tpip->number] << 1) | ~0x80);

    return byte;
}

void tpid_init(drive_context_t *ctxptr)
{
    tpi_context_t *tpi_context;

    tpi_context = &(ctxptr->tpid);

    tpi_context->log = log_open(tpi_context->myname);
}

void tpid_setup_context(drive_context_t *ctxptr)
{
    drivetpi_context_t *tpip;
    tpi_context_t *tpi_context;

    tpi_context = &(ctxptr->tpid);

    tpi_context->prv = lib_malloc(sizeof(drivetpi_context_t));
    tpip = (drivetpi_context_t *)(tpi_context->prv);
    tpip->number = ctxptr->mynumber;

    tpi_context->context = (void *)ctxptr;

    tpi_context->rmw_flag = &(ctxptr->cpu.rmw_flag);
    tpi_context->clk_ptr = ctxptr->clk_ptr;

    tpi_context->myname = lib_msprintf("Drive%dTPI", ctxptr->mynumber);

    tpicore_setup_context(tpi_context);

    tpi_context->tpi_int_num
        = interrupt_cpu_status_int_new(ctxptr->cpu.int_status,
                                       tpi_context->myname);
    tpi_context->irq_line = IK_IRQ;
    tpip->drive_ptr = ctxptr->drive_ptr;

    tpi_context->store_pa = store_pa;
    tpi_context->store_pb = store_pb;
    tpi_context->store_pc = store_pc;
    tpi_context->read_pa = read_pa;
    tpi_context->read_pb = read_pb;
    tpi_context->read_pc = read_pc;
    tpi_context->undump_pa = undump_pa;
    tpi_context->undump_pb = undump_pb;
    tpi_context->undump_pc = undump_pc;
    tpi_context->reset = reset;
    tpi_context->set_ca = set_ca;
    tpi_context->set_cb = set_cb;
    tpi_context->set_int = set_int;
    tpi_context->restore_int = restore_int;
}

