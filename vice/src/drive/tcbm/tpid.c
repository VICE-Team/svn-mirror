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
#define mytpi_snapshot_write_module tpid_snapshot_write_module
#define mytpi_snapshot_read_module tpid_snapshot_read_module

#define MYTPI_NAME (ctxptr->tpid.myname)

/* Renaming formerly global variables */
#define tpi             (ctxptr->tpid.c_tpi)
#define irq_previous    (ctxptr->tpid.irq_previous)
#define irq_stack       (ctxptr->tpid.irq_stack)
#define tpi_last_read   (ctxptr->tpid.tpi_last_read)
#define tpi_int_num     (ctxptr->tpid.tpi_int_num)


/*----------------------------------------------------------------------*/
/* CPU binding */

#define mycpu_set_int(a,b)              do {} while(0)
#define mycpu_restore_int(a,b)          do {} while(0)

#define mycpu_rmw_flag (ctxptr->cpu.rmw_flag)
#define myclk (*(ctxptr->clk_ptr))
#define mycpu_int_status (ctxptr->cpu.int_status)

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
}

_TPI_FUNC void store_pa(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void store_pb(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void undump_pa(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void undump_pb(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void store_pc(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC void undump_pc(TPI_CONTEXT_PARAM BYTE byte)
{
}

_TPI_FUNC BYTE read_pa(TPI_CONTEXT_PARVOID)
{
    BYTE byte;

    byte = (0xff & ~tpi[TPI_DDPA]) | (tpi[TPI_PA] & tpi[TPI_DDPA]);

    return byte;
}

_TPI_FUNC BYTE read_pb(TPI_CONTEXT_PARVOID)
{
    BYTE byte;

    byte = (0xff & ~tpi[TPI_DDPB]) | (tpi[TPI_PB] & tpi[TPI_DDPB]);

    return byte;
}

_TPI_FUNC BYTE read_pc(TPI_CONTEXT_PARVOID)
{
    BYTE byte;

    byte = (0xff & ~tpi[TPI_DDPC]) | (tpi[TPI_PC] & tpi[TPI_DDPC]);

    return byte;
}

#include "tpicore.c"

