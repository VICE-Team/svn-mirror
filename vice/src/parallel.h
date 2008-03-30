/*
 * parallel.h
 *
 * Written by
 *  André Fachat (a.fachat@physik.tu-chemnitz.de)
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

/* This file contains the exported interface to the iec488 emulator.
 * The iec488 emulator then calls (modifed) routines from serial.c
 * to use the standard floppy interface.
 * The current state of the bus and methods to set output lines
 * are exported.
 * This hardware emulation is necessary, as different PET kernels would
 * need different traps. But it's also much faster than the (hardware
 * simulated) serial bus, as it's parallel. So we need no traps.
 */

#ifndef _PARALLEL_H
#define _PARALLEL_H

#include "types.h"

/* state of the bus lines - if(par_eoi) { eoi is detected } */
extern char par_eoi;
extern char par_ndac;
extern char par_nrfd;
extern char par_dav;
extern char par_atn;

extern BYTE par_bus;	/* data lines */

/* methods to set output lines */
extern void par_set_eoi( char );
extern void par_set_ndac( char );
extern void par_set_nrfd( char );
extern void par_set_dav( char );
extern void par_set_atn( char );

extern void par_set_bus( BYTE );

extern int pardebug;

#endif
