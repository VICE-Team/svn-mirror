/*
 * c610tpi2.c - TPI 2 for CBM-II
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

#include "kbd.h"
#include "maincpu.h"

/*----------------------------------------------------------------------*/
/* renaming of exported functions */

#define mytpi_init tpi2_init
#define reset_mytpi reset_tpi2
#define store_mytpi store_tpi2
#define read_mytpi read_tpi2
#define peek_mytpi peek_tpi2
#define mytpi_set_int tpi2_set_int
#define mytpi_restore_int tpi2_restore_int
#define mytpi_write_snapshot_module tpi2_write_snapshot_module
#define mytpi_read_snapshot_module tpi2_read_snapshot_module

#define MYTPI_NAME      "TPI2"

/*----------------------------------------------------------------------*/
/* CPU binding */

#define mycpu_set_int(a,b)		do {} while(0)
#define mycpu_restore_int(a,b)		do {} while(0)

#define mycpu_rmw_flag  rmw_flag
#define myclk           clk

/*----------------------------------------------------------------------*/
/* I/O */

static BYTE cbm2_model_port_mask = 0xc0;

void set_cbm2_model_port_mask(BYTE val) 
{
    cbm2_model_port_mask = val & 0xc0;
}

_TPI_FUNC void tpi_set_ca(int a)
{
}

_TPI_FUNC void tpi_set_cb(int a)
{
}

_TPI_FUNC void tpi_reset(void)
{
}

_TPI_FUNC void store_pa(BYTE byte)
{
}

_TPI_FUNC void store_pb(BYTE byte)
{
}

_TPI_FUNC void store_pc(BYTE byte)
{
}

_TPI_FUNC void undump_pa(BYTE byte)
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
    byte = (0xff & ~tpi[TPI_DDPA]) | (tpi[TPI_PA] & tpi[TPI_DDPA]);
    return byte;
}

_TPI_FUNC BYTE read_pb(void)
{
    BYTE byte;
    byte = (0xff & ~tpi[TPI_DDPB]) | (tpi[TPI_PB] & tpi[TPI_DDPB]);
    return byte;
}

_TPI_FUNC BYTE read_pc(void)
{
    BYTE byte;
    BYTE val = ~tpi[TPI_DDPC] | 0xc0;
    int msk = (oldpa & 0xff) | ((oldpb << 8) & 0xff00);
    int m;
    int i;

    for (m = 0x1, i = 0; i < 16; m <<= 1, i++)
        if (!(msk & m))
            val &= ~keyarr[i];
/*
if(val) printf("mask=%04x, val=%02x (pa=%02x/%02x, pb=%02x/%02x)\n",
		msk, b, tpi[TPI_PA], tpi[TPI_DDPA], tpi[TPI_PB], tpi[TPI_DDPB]);
*/
    byte = (val & 0x3f) | cbm2_model_port_mask;

    return byte;
}

#include "tpicore.c"

