/* -*- C -*-
 * c64tpi.c - IEEE488 interface for the C64.
 *
 * Written by
 *   André Fachat (a.fachat@physik.tu-chemnitz.de)
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

#include "tpicore.h"

#include "parallel.h"
#include "drive.h"
#include "tpi.h"
#include "maincpu.h"

/*----------------------------------------------------------------------*/
/* renaming of exported functions */


#define mytpi_init tpi_init
#define reset_mytpi reset_tpi
#define store_mytpi store_tpi
#define read_mytpi read_tpi
#define peek_mytpi peek_tpi
#define mytpi_set_int tpi_set_int
#define mytpi_write_snapshot_module tpi_write_snapshot_module
#define mytpi_read_snapshot_module tpi_read_snapshot_module

#define	MYTPI_NAME	"TPI"

/*----------------------------------------------------------------------*/
/* CPU binding */

#define	mycpu_set_int(a,b)		do {} while(0)
#define	mycpu_restore_int(a,b)		do {} while(0)

#define	mycpu_rmw_flag	rmw_flag
#define	myclk		clk

/*----------------------------------------------------------------------*/
/* I/O */

_TPI_FUNC void tpi_set_ca(int a)
{
}

_TPI_FUNC void tpi_set_cb(int a)
{
}

static int ieee_is_dev = 1;
static int ieee_is_out = 1;

_TPI_FUNC void tpi_reset(void)
{
    /* assuming input after reset */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_ndac(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_dav(0);
    parallel_cpu_set_eoi(0);
    parallel_cpu_set_bus(0);

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

        parallel_cpu_set_bus(ieee_is_out ? ~oldpb : 0);

        if (ieee_is_out) {
            parallel_cpu_set_ndac( 0 );
            parallel_cpu_set_nrfd( 0 );
            parallel_cpu_set_dav( tmp & 0x10 );
            parallel_cpu_set_eoi( tmp & 0x20 );
        } else {
            parallel_cpu_set_nrfd( tmp & 0x80 );
            parallel_cpu_set_ndac( tmp & 0x40 );
            parallel_cpu_set_dav( 0 );
            parallel_cpu_set_eoi( 0 );
        }
        if (ieee_is_dev) {
            parallel_cpu_set_atn( 0 );
        } else {
            parallel_cpu_set_atn( tmp & 0x08 );
        }
    }
}

_TPI_FUNC void store_pb(BYTE byte)
{
    parallel_cpu_set_bus( ieee_is_out ? ~byte : 0);
}

_TPI_FUNC void undump_pa(BYTE byte)
{
    BYTE tmp = ~byte;
    ieee_is_dev = byte & 0x01;
    ieee_is_out = byte & 0x02;

    parallel_cpu_set_bus(ieee_is_out ? ~oldpb : 0);

    if (ieee_is_out) {
        parallel_cpu_set_ndac( 0 );
        parallel_cpu_set_nrfd( 0 );
        parallel_cpu_set_dav( tmp & 0x10 );
        parallel_cpu_set_eoi( tmp & 0x20 );
    } else {
        parallel_cpu_set_nrfd( tmp & 0x80 );
        parallel_cpu_set_ndac( tmp & 0x40 );
        parallel_cpu_set_dav( 0 );
        parallel_cpu_set_eoi( 0 );
    }
    if (ieee_is_dev) {
        parallel_cpu_set_atn( 0 );
    } else {
        parallel_cpu_set_atn( tmp & 0x08 );
    }
}

_TPI_FUNC void undump_pb(BYTE byte)
{
    parallel_cpu_set_bus( ieee_is_out ? ~byte : 0);
}

_TPI_FUNC void store_pc(BYTE byte)
{
}

_TPI_FUNC void undump_pc(BYTE byte)
{
}

_TPI_FUNC BYTE read_pa(void)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].enable)
        drive1_cpu_execute(clk);

    byte = 0xff;
    if (ieee_is_out) {
        if (parallel_nrfd) byte &= 0x7f;
        if (parallel_ndac) byte &= 0xbf;
    } else {
        if (parallel_dav) byte &= 0xef;
        if (parallel_eoi) byte &= 0xdf;
    }
    if (ieee_is_dev) {
        if (parallel_atn) byte &= 0xf7;
    }

    byte = (byte & ~tpi[TPI_DDPA]) | (tpi[TPI_PA] & tpi[TPI_DDPA]);

    return byte;
}

_TPI_FUNC BYTE read_pb(void)
{
    BYTE byte;

    if (drive[0].enable)
        drive0_cpu_execute(clk);
    if (drive[1].enable)
        drive1_cpu_execute(clk);

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

