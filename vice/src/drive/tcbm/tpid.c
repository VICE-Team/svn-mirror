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

struct drive_context_s;
#define TPI_SHARED_CODE
#define TPICONTEXT struct drive_context_s

#include "drivetypes.h"
#include "iecdrive.h"
#include "interrupt.h"
#include "rotation.h"
#include "tpi.h"
#include "tpicore.h"
#include "tpid.h"
#include "types.h"

/*----------------------------------------------------------------------*/
/* renaming of exported functions */


#define mytpi_init tpid_init
#define mytpi_reset tpid_reset
#define mytpi_store tpid_store
#define mytpi_read tpid_read
#define mytpi_peek tpid_peek
#define mytpi_set_int tpid_set_int
#define mytpi_restore_int tpid_restore_int
#define mytpi_snapshot_write_module tpid_snapshot_write_module
#define mytpi_snapshot_read_module tpid_snapshot_read_module

#define MYTPI_NAME (ctxptr->tpid.myname)

/* Renaming formerly global variables */
#define tpi             (ctxptr->tpid.c_tpi)
#define irq_previous    (ctxptr->tpid.irq_previous)
#define irq_stack       (ctxptr->tpid.irq_stack)
#define tpi_last_read   (ctxptr->tpid.tpi_last_read)
#define tpi_int_num     (ctxptr->tpid.tpi_int_num)

#define oldpa (ctxptr->tpid.oldpa)
#define oldpb (ctxptr->tpid.oldpb)
#define oldpc (ctxptr->tpid.oldpc)

#define ca_state (ctxptr->tpid.ca_state)
#define cb_state (ctxptr->tpid.cb_state)

#define mytpi_log       (ctxptr->tpid.log)


/*----------------------------------------------------------------------*/
/* CPU binding */

#define mycpu_set_int(a,b)              do {} while(0)
#define mycpu_restore_int(a,b)          do {} while(0)

#define mycpu_rmw_flag (ctxptr->cpu.rmw_flag)
#define myclk (*(ctxptr->clk_ptr))
#define mycpu_int_status (ctxptr->cpu.int_status)

void tpid_setup_context(drive_context_t *ctxptr)
{
    sprintf(ctxptr->tpid.myname, "Drive%dTPI", ctxptr->mynumber);
    irq_previous = 0;
    irq_stack = 0;
    tpi_last_read = 0;
    tpi_int_num = interrupt_cpu_status_int_new(ctxptr->cpu.int_status,
                                               ctxptr->tpid.myname);
}

/*----------------------------------------------------------------------*/
/* I/O */

_TPI_FUNC void tpi_set_ca(TPI_CONTEXT_PARAM int a)
{
}

_TPI_FUNC void tpi_set_cb(TPI_CONTEXT_PARAM int a)
{
}

_TPI_FUNC void _tpi_reset(TPI_CONTEXT_PARVOID)
{
    plus4tcbm_update_pa(0xff, ctxptr->mynumber);
    plus4tcbm_update_pb(0xff, ctxptr->mynumber);
    plus4tcbm_update_pc(0xff, ctxptr->mynumber);
}

_TPI_FUNC void store_pa(TPI_CONTEXT_PARAM BYTE byte)
{
    plus4tcbm_update_pa(byte, ctxptr->mynumber);
}

_TPI_FUNC void store_pb(TPI_CONTEXT_PARAM BYTE byte)
{
    if (ctxptr->drive_ptr->byte_ready_active == 0x06)
        rotation_rotate_disk(ctxptr->drive_ptr);

    ctxptr->drive_ptr->GCR_write_value = byte;
}

_TPI_FUNC void undump_pa(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void undump_pb(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void store_pc(TPI_CONTEXT_PARAM BYTE byte)
{
    plus4tcbm_update_pc(byte, ctxptr->mynumber);

    ctxptr->drive_ptr->read_write_mode = byte & 0x10;

    if ((byte & 0x10) != (oldpc & 0x10)) {
        if (ctxptr->drive_ptr->byte_ready_active == 0x06)
            rotation_rotate_disk(ctxptr->drive_ptr);
        rotation_change_mode(ctxptr->mynumber);
    }
}

_TPI_FUNC void undump_pc(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC BYTE read_pa(TPI_CONTEXT_PARVOID)
{
    /* TCBM data port */
    BYTE byte;

    byte = (tpi[TPI_PA] | ~tpi[TPI_DDPA]) & plus4tcbm_outputa[ctxptr->mynumber];

    ctxptr->drive_ptr->byte_ready_level = 0;

    return byte;
}

_TPI_FUNC BYTE read_pb(TPI_CONTEXT_PARVOID)
{
    /* GCR data port */
    BYTE byte;

    rotation_byte_read(ctxptr->drive_ptr);

    byte = (tpi[TPI_PB] | ~tpi[TPI_DDPB]) & ctxptr->drive_ptr->GCR_read;

    ctxptr->drive_ptr->byte_ready_level = 0;

    return byte;
}

_TPI_FUNC BYTE read_pc(TPI_CONTEXT_PARVOID)
{
    /* TCBM control / GCR data control */
    BYTE byte;

    if (ctxptr->drive_ptr->byte_ready_active == 0x06)
        rotation_rotate_disk(ctxptr->drive_ptr);

    byte = (tpi[TPI_PC] | ~tpi[TPI_DDPC])
           /* Bit 0, 1 */
           & (plus4tcbm_outputb[ctxptr->mynumber] | ~0x03)
           /* Bit 3 */
           & ((plus4tcbm_outputc[ctxptr->mynumber] >> 4) | ~0x08)
           /* Bit 5 */
           & (~0x20)
           /* Bit 6 */
           & (rotation_sync_found(ctxptr->drive_ptr) ? 0xff : ~0x40)
           /* Bit 7 */
           & ((plus4tcbm_outputc[ctxptr->mynumber] << 1) | ~0x80);

    return byte;
}

void tpid_init(drive_context_t *ctxptr)
{
    mytpi_log = log_open(MYTPI_NAME);
}

#include "tpicore.c"

