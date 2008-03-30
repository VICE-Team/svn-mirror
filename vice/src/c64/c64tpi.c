/*
 * c64tpi.c - IEEE488 interface for the C64.
 *
 * Written by
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
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

#include "c64mem.h"
#include "drive.h"
#include "drivecpu.h"
#include "parallel.h"
#include "interrupt.h"
#include "maincpu.h"
#include "tpi.h"
#include "tpicore.h"
#include "types.h"

/*----------------------------------------------------------------------*/
/* renaming of exported functions */


#define mytpi_init tpi_init
#define mytpi_reset tpi_reset
#define mytpi_store tpi_store
#define mytpi_read tpi_read
#define mytpi_peek tpi_peek
#define mytpi_set_int tpi_set_int
#define mytpi_snapshot_write_module tpi_snapshot_write_module
#define mytpi_snapshot_read_module tpi_snapshot_read_module

#define MYTPI_NAME      "TPI"

/*----------------------------------------------------------------------*/
/* CPU binding */

#define mycpu_set_int(a,b)              do {} while(0)
#define mycpu_restore_int(a,b)          do {} while(0)

#define mycpu_rmw_flag maincpu_rmw_flag
#define myclk maincpu_clk
#define mycpu_int_status maincpu_int_status

/*----------------------------------------------------------------------*/
/* I/O */

_TPI_FUNC void tpi_set_ca(int a)
{
}

_TPI_FUNC void tpi_set_cb(int a)
{
}

static int ieee_is_dev = 1;
static BYTE ieee_is_out = 1;

_TPI_FUNC void _tpi_reset(void)
{
    /* assuming input after reset */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_ndac(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_dav(0);
    parallel_cpu_set_eoi(0);
    parallel_cpu_set_bus(0xff);

    ieee_is_dev = 1;
    ieee_is_out = 1;
}

_TPI_FUNC void store_pa(BYTE byte)
{
    if (byte != oldpa)
    {
        BYTE tmp = ~byte;
        ieee_is_dev = byte & 0x01;
        ieee_is_out = byte & 0x02;

        parallel_cpu_set_bus((BYTE)(ieee_is_out ? oldpb : 0xff));

        if (ieee_is_out) {
            parallel_cpu_set_ndac(0);
            parallel_cpu_set_nrfd(0);
            parallel_cpu_set_dav((BYTE)(tmp & 0x10));
            parallel_cpu_set_eoi((BYTE)(tmp & 0x20));
        } else {
            parallel_cpu_set_nrfd((BYTE)(tmp & 0x80));
            parallel_cpu_set_ndac((BYTE)(tmp & 0x40));
            parallel_cpu_set_dav(0);
            parallel_cpu_set_eoi(0);
        }
        if (ieee_is_dev) {
            parallel_cpu_set_atn(0);
        } else {
            parallel_cpu_set_atn((BYTE)(tmp & 0x08));
        }
    }
}

_TPI_FUNC void store_pb(BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(ieee_is_out ? byte : 0xff));
}

_TPI_FUNC void undump_pa(BYTE byte)
{
    BYTE tmp = ~byte;
    ieee_is_dev = byte & 0x01;
    ieee_is_out = byte & 0x02;

    parallel_cpu_set_bus((BYTE)(ieee_is_out ? oldpb : 0xff));

    if (ieee_is_out) {
        parallel_cpu_set_ndac(0);
        parallel_cpu_set_nrfd(0);
        parallel_cpu_set_dav((BYTE)(tmp & 0x10));
        parallel_cpu_set_eoi((BYTE)(tmp & 0x20));
    } else {
        parallel_cpu_set_nrfd((BYTE)(tmp & 0x80));
        parallel_cpu_set_ndac((BYTE)(tmp & 0x40));
        parallel_cpu_set_dav(0);
        parallel_cpu_set_eoi(0);
    }
    if (ieee_is_dev) {
        parallel_cpu_restore_atn(0);
    } else {
        parallel_cpu_restore_atn((BYTE)(tmp & 0x08));
    }
}

_TPI_FUNC void undump_pb(BYTE byte)
{
    parallel_cpu_set_bus((BYTE)(ieee_is_out ? byte : 0xff));
}

_TPI_FUNC void store_pc(BYTE byte)
{
    /* 1 = active */
    mem_set_exrom((byte & 8) ? 0 : 1);
}

_TPI_FUNC void undump_pc(BYTE byte)
{
}

_TPI_FUNC BYTE read_pa(void)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    byte = 0xff;
    if (ieee_is_out) {
        if (parallel_nrfd)
            byte &= 0x7f;
        if (parallel_ndac)
            byte &= 0xbf;
    } else {
        if (parallel_dav)
            byte &= 0xef;
        if (parallel_eoi)
            byte &= 0xdf;
    }
    if (ieee_is_dev) {
        if (parallel_atn)
            byte &= 0xf7;
    }

    byte = (byte & ~tpi[TPI_DDPA]) | (tpi[TPI_PA] & tpi[TPI_DDPA]);

    return byte;
}

_TPI_FUNC BYTE read_pb(void)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(maincpu_clk);
    if (drive[1].enable)
        drive1_cpu_execute(maincpu_clk);

    byte = ieee_is_out ? 0xff : parallel_bus;
    byte = (byte & ~tpi[TPI_DDPB]) | (tpi[TPI_PB] & tpi[TPI_DDPB]);

    return byte;
}

_TPI_FUNC BYTE read_pc(void)
{
    BYTE byte;
    byte = (0xff & ~tpi[TPI_DDPC]) | (tpi[TPI_PC] & tpi[TPI_DDPC]);
    return byte;
}

#include "tpicore.c"

