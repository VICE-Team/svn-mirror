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

void reset_tpi ( void ) {
	int i;
	for(i=0;i<8;i++) {
	  tpi[0] = 0;
	}

	RESET

}

void store_tpi ( ADDRESS addr, BYTE byte ) {
	BYTE b;

	tpi[addr & 0x07] = byte;
	switch ( addr ) {
	case TPI_PA:
	case TPI_DDPA:
		b = (tpi[TPI_PA]^255) & tpi[TPI_DDPA];
		STORE_PA
		return;
	case TPI_PB:
	case TPI_DDPB:
		b = (tpi[TPI_PB]^255) & tpi[TPI_DDPB];
		STORE_PB
		return;
	case TPI_PC:
	case TPI_DDPC:
		b = (tpi[TPI_PC]^255) & tpi[TPI_DDPC];
		STORE_PC
	}
}

BYTE read_tpi ( ADDRESS addr ) {
	BYTE b = 0xff;
    	switch ( addr ) {
	case TPI_PA:
		READ_PA
		return ((b & (tpi[TPI_DDPA]^255))
					     | (tpi[TPI_PA] & tpi[TPI_DDPA]));
	case TPI_PB:
		READ_PB
		return ((b & (tpi[TPI_DDPB]^255))
					     | (tpi[TPI_PB] & tpi[TPI_DDPB]));
	case TPI_PC:
		READ_PC
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

