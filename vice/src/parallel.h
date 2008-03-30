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

/* debug variable - set to 1 to generate output */
extern int parallel_debug;

/* state of the bus lines -> "if(parallel_eoi) { eoi is active }" */
extern char parallel_eoi;
extern char parallel_ndac;
extern char parallel_nrfd;
extern char parallel_dav;
extern char parallel_atn;

extern BYTE parallel_bus;	/* data lines */

/* Each device has a mask bit in the parallel_* handshake lines */
#define	PARALLEL_EMU	0x01
#define	PARALLEL_CPU	0x02
#define	PARALLEL_DRV0	0x04
#define	PARALLEL_DRV1	0x08
  
/* methods to set handshake lines active for the devices */
extern void parallel_set_eoi( char mask);
extern void parallel_set_ndac( char mask);
extern void parallel_set_nrfd( char mask);
extern void parallel_set_dav( char mask);
extern void parallel_set_atn( char mask);
extern void parallel_restore_set_atn( char mask);

/* methods to set handshake lines inactive for the devices */
extern void parallel_clr_eoi( char mask);
extern void parallel_clr_ndac( char mask);
extern void parallel_clr_nrfd( char mask);
extern void parallel_clr_dav( char mask);
extern void parallel_clr_atn( char mask);
extern void parallel_restore_clr_atn( char mask);


/* methods to set output lines for the computer */
#define	PARALLEL_SET_LINE(line,dev,mask)				\
    static inline void parallel_##dev##_set_##line##( char val ) 	\
    {									\
    	if (val) {							\
	    parallel_set_##line##(PARALLEL_##mask##);			\
        } else {							\
	    parallel_clr_##line##(~PARALLEL_##mask##);			\
    	}								\
    }

#define	PARALLEL_RESTORE_LINE(line,dev,mask)				\
    static inline void parallel_##dev##_restore_##line##( char val ) 	\
    {									\
    	if (val) {							\
	    parallel_restore_set_##line##(PARALLEL_##mask##);		\
        } else {							\
	    parallel_restore_clr_##line##(~PARALLEL_##mask##);		\
    	}								\
    }

/* Emulator functions */
PARALLEL_SET_LINE(eoi,emu,EMU)
PARALLEL_SET_LINE(dav,emu,EMU)
PARALLEL_SET_LINE(nrfd,emu,EMU)
PARALLEL_SET_LINE(ndac,emu,EMU)

void parallel_emu_set_bus( BYTE);

/* CPU functions */
PARALLEL_SET_LINE(eoi,cpu,CPU)
PARALLEL_SET_LINE(dav,cpu,CPU)
PARALLEL_SET_LINE(nrfd,cpu,CPU)
PARALLEL_SET_LINE(ndac,cpu,CPU)
PARALLEL_SET_LINE(atn,cpu,CPU)
PARALLEL_RESTORE_LINE(atn,cpu,CPU)

void parallel_cpu_set_bus( BYTE);

/* Drive 0 functions */
PARALLEL_SET_LINE(eoi,drv0,DRV0)
PARALLEL_SET_LINE(dav,drv0,DRV0)
PARALLEL_SET_LINE(nrfd,drv0,DRV0)
PARALLEL_SET_LINE(ndac,drv0,DRV0)

void parallel_drv0_set_bus( BYTE);

/* Drive 1 functions */
PARALLEL_SET_LINE(eoi,drv1,DRV1)
PARALLEL_SET_LINE(dav,drv1,DRV1)
PARALLEL_SET_LINE(nrfd,drv1,DRV1)
PARALLEL_SET_LINE(ndac,drv1,DRV1)

void parallel_drv1_set_bus( BYTE);

#endif
