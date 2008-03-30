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

INCLUDES

int tpidebug = 0;

static BYTE tpi[8];

static BYTE oldpa;	/* current PA output `as measured' */
static BYTE oldpb;	/* current PB output `as measured' */
static BYTE oldpc;	/* current PC output `as measured' */

void reset_tpi ( void ) {
	int i;
	for(i=0;i<8;i++) {
	  tpi[0] = 0;
	}

	oldpa = 0xff;
	oldpb = 0xff;
	oldpc = 0xff;

	RESET

}

void store_tpi ( ADDRESS addr, BYTE byte ) {

	tpi[addr & 0x07] = byte;

	switch ( addr ) {
	case TPI_PA:
	case TPI_DDPA:
		byte = (tpi[TPI_PA]^255) & tpi[TPI_DDPA];
		STORE_PA
		oldpa = byte;
		return;
	case TPI_PB:
	case TPI_DDPB:
		byte = (tpi[TPI_PB]^255) & tpi[TPI_DDPB];
		STORE_PB
		oldpb = byte;
		return;
	case TPI_PC:
	case TPI_DDPC:
		byte = (tpi[TPI_PC]^255) & tpi[TPI_DDPC];
		STORE_PC
		oldpc = byte;
	}
}

BYTE read_tpi ( ADDRESS addr ) {
	BYTE byte = 0xff;
    	switch ( addr ) {
	case TPI_PA:
		READ_PA
		return ((byte & ~tpi[TPI_DDPA])
					     | (tpi[TPI_PA] & tpi[TPI_DDPA]));
	case TPI_PB:
		READ_PB
		return ((byte & ~tpi[TPI_DDPB])
					     | (tpi[TPI_PB] & tpi[TPI_DDPB]));
	case TPI_PC:
		READ_PC
		return  ((byte & ~tpi[TPI_DDPC])
					     | (tpi[TPI_PB] & tpi[TPI_DDPC]));
	default:
		return tpi[addr];
	}
}

BYTE peek_tpi (ADDRESS addr) {
	BYTE b = read_tpi(addr);
	return b;
}

