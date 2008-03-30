/*
 * c610tpi1.c - TPI 1 for CBM-II
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

#include "c610cia.h"
#include "crtc.h"
#include "datasette.h"
#include "drive.h"
#include "maincpu.h"
#include "parallel.h"

/*----------------------------------------------------------------------*/
/* renaming of exported functions */

#define mytpi_init tpi1_init
#define reset_mytpi reset_tpi1
#define store_mytpi store_tpi1
#define read_mytpi read_tpi1
#define peek_mytpi peek_tpi1
#define mytpi_set_int tpi1_set_int
#define mytpi_restore_int tpi1_restore_int
#define mytpi_write_snapshot_module tpi1_write_snapshot_module
#define mytpi_read_snapshot_module tpi1_read_snapshot_module

#define MYTPI_NAME      "TPI1"

/*----------------------------------------------------------------------*/
/* CPU binding */

#define	mycpu_set_int(a,b)	maincpu_set_irq((a),(b))
#define	mycpu_restore_int(a,b)	set_int_noclk(&maincpu_int_status,(a),(b))

#define mycpu_rmw_flag  rmw_flag
#define myclk           clk

#define	MYIRQ	IK_IRQ

#define	I_MYTPI I_TPI1

/*----------------------------------------------------------------------*/
/* TPI resources. */

static int tape1_sense = 0;

void tpi1_set_tape_sense(int v)
{
    tape1_sense = v;
}

/*----------------------------------------------------------------------*/
/* I/O */

_TPI_FUNC void tpi_set_ca(int a)
{
    crtc_set_char((a));
}

_TPI_FUNC void tpi_set_cb(int a)
{
}

_TPI_FUNC void tpi_reset(void)
{
    /* assuming input after reset */
    parallel_cpu_set_atn(0);
    parallel_cpu_set_ndac(0);
    parallel_cpu_set_nrfd(0);
    parallel_cpu_set_dav(0);
    parallel_cpu_set_eoi(0);
    parallel_cpu_set_bus(0xff);
    cia1_set_ieee_dir(0);
}

_TPI_FUNC void store_pa(BYTE byte)
{
    if (byte != oldpa)
    {
        BYTE tmp = ~byte;
        cia1_set_ieee_dir(byte & 2);
        if (byte & 2) {
    	    parallel_cpu_set_ndac(0);
	    parallel_cpu_set_nrfd(0);
            parallel_cpu_set_atn( tmp & 0x08 );
            parallel_cpu_set_dav( tmp & 0x10 );
            parallel_cpu_set_eoi( tmp & 0x20 );
        } else {
	    /* order is important */
            parallel_cpu_set_nrfd( tmp & 0x80 );
            parallel_cpu_set_ndac( tmp & 0x40 );
	    parallel_cpu_set_atn(0);
	    parallel_cpu_set_dav(0);
	    parallel_cpu_set_eoi(0);
        }
    }
}

_TPI_FUNC void undump_pa(BYTE byte)
{
    BYTE tmp = ~byte;
    cia1_set_ieee_dir(byte & 2);
    if (byte & 2) {
	parallel_cpu_set_ndac(0);
	parallel_cpu_set_nrfd(0);
        parallel_cpu_restore_atn( tmp & 0x08 );
        parallel_cpu_set_dav( tmp & 0x10 );
        parallel_cpu_set_eoi( tmp & 0x20 );
    } else {
	/* order is important */
        parallel_cpu_set_nrfd( tmp & 0x80 );
        parallel_cpu_set_ndac( tmp & 0x40 );
	parallel_cpu_restore_atn(0);
	parallel_cpu_set_dav(0);
	parallel_cpu_set_eoi(0);
    }
}

_TPI_FUNC void store_pb(BYTE byte) 
{
    if ((byte ^ oldpb) & 0x40)
        datasette_set_motor(!(byte & 0x40));
    if ((byte ^ oldpb) & 0x20)
        datasette_toggle_write_bit(byte & 0x20);
}

_TPI_FUNC void store_pc(BYTE byte) 
{
}

_TPI_FUNC void undump_pb(BYTE byte) 
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

    byte = 0x07;
    byte += parallel_atn ? 0 : 8;
    byte += parallel_dav ? 0 : 16;
    byte += parallel_eoi ? 0 : 32;
    byte += parallel_ndac ? 0 : 64;
    byte += parallel_nrfd ? 0 : 128;

    byte = (byte & ~tpi[TPI_DDPA]) | (tpi[TPI_PA] & tpi[TPI_DDPA]);

    return byte;
}

_TPI_FUNC BYTE read_pb(void)
{
    BYTE byte;

    byte = 0x7f;
    byte += tape1_sense ? 0x80 : 0;

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

