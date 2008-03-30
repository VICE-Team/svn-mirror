
/*
 * ../../src/c64/c64tpi.c
 * This file is generated from ../../src/tpi-tmpl.c and ../../src/c64/c64tpi.def,
 * Do not edit!
 */
/*
 * tpi-tmpl.c - TPI 6525 template
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

#include <stdio.h>

#include "vice.h"
#include "types.h"
#include "tpi.h"


#include "parallel.h"
#include "tpi.h"

int tpidebug = 0;

static BYTE tpi[8];

void reset_tpi ( void ) {
	int i;
	for(i=0;i<8;i++) {
	  tpi[0] = 0;
	}


	/* assuming input after reset */
	par_set_atn(0);
	par_set_ndac(0);
	par_set_nrfd(0);
	par_set_dav(0);
	par_set_eoi(0);
	par_set_bus(0);

}

void store_tpi ( ADDRESS addr, BYTE byte ) {
	BYTE b;

	tpi[addr & 0x07] = byte;
	switch ( addr ) {
	case TPI_PA:
	case TPI_DDPA:
		b = (tpi[TPI_PA]^255) & tpi[TPI_DDPA];

		par_set_atn( b & 0x08 );
		par_set_dav( b & 0x10 );
		par_set_eoi( b & 0x20 );
		par_set_ndac( b & 0x40 );
		par_set_nrfd( b & 0x80 );
		return;
	case TPI_PB:
	case TPI_DDPB:
		b = (tpi[TPI_PB]^255) & tpi[TPI_DDPB];

		par_set_bus( b );
		return;
	case TPI_PC:
	case TPI_DDPC:
		b = (tpi[TPI_PC]^255) & tpi[TPI_DDPC];
		
	}
}

BYTE read_tpi ( ADDRESS addr ) {
	BYTE b = 0xff;
    	switch ( addr ) {
	case TPI_PA:

		b = par_atn ? 0 : 8;
		b += par_dav ? 0 : 16;
		b += par_eoi ? 0 : 32;
		b += par_ndac ? 0 : 64;
		b += par_nrfd ? 0 : 128;
		return ((b & (tpi[TPI_DDPA]^255))
					     | (tpi[TPI_PA] & tpi[TPI_DDPA]));
	case TPI_PB:

		b = par_bus;
		return ((b & (tpi[TPI_DDPB]^255))
					     | (tpi[TPI_PB] & tpi[TPI_DDPB]));
	case TPI_PC:
		
		return  ((b & (tpi[TPI_DDPC]^255))
					     | (tpi[TPI_PB] & tpi[TPI_DDPC]));
	default:
		return tpi[addr];
	}
}

BYTE peek_tpi (ADDRESS addr) {
	BYTE b = read_tpi(addr);
	return b;
}

