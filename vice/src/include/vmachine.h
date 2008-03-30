/*
 * vmachine.h - Miscellaneous system parameters.
 *
 * Written by
 *  Vesa-Matti Puro (vmp@lut.fi)
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta     (jopi@stekt.oulu.fi)
 *  Andre' Fachat   (a.fachat@physik.tu-chemnitz.de)
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

/* This needs to be cleaned up! */

#ifndef VICE_VMACHINE_H
#define VICE_VMACHINE_H


#include "vice.h"

/*
 * CPU alarms.
 */

#define A_RASTERDRAW	0 	/* Draw one raster line. */
#define A_RASTERFETCH	1 	/* Video chip memory fetch. */
#define A_RASTER	2	/* Raster compare. */
#define A_CIA1TOD	3 	/* CIA 1 TOD alarm */
#define A_CIA1TA	4       /* CIA 1 timer A */
#define A_CIA1TB	5  	/* CIA 1 timer B */
#define A_CIA2TOD	6   	/* CIA 2 TOD alarm */
#define A_CIA2TA	7   	/* CIA 2 timer A */
#define A_CIA2TB	8   	/* CIA 2 timer B */

#define NUMOFALRM	9

#define	A_VIA1T1	A_CIA1TA
#define	A_VIA1T2	A_CIA1TB
#define	A_VIA2T1	A_CIA2TA
#define	A_VIA2T2	A_CIA2TB

#ifdef PET
/* PET IRQs */
#define	A_VIAT1		A_CIA1TA	/* pet via t1 */
#define	A_VIAT2		A_CIA1TB	/* pet via t2 */
#endif


/*
 * Interrupts.
 */

#define I_RASTERDRAW     0      /* Draw one raster line */
#define I_RASTERFETCH    1      /* Video chip memory fetch */
#define I_RASTER	 2	/* Raster compare */

#define I_BRK		 3	/* Software interrupt */

/* CIA 1 / VIA 2 IRQs */
#define I_CIA1FL	 4	/* CIA 1 FLAG1 */
#define I_CIA1SP	 5	/* CIA 1 serial port */
#define I_CIA1TOD	 6	/* CIA 1 TOD alarm */
#define I_CIA1TA	 7      /* CIA 1 timer A */
#define I_CIA1TB	 8 	/* CIA 1 timer B */

/* CIA 2 / VIA 1 NMIs */
#define I_CIA2FL	 9 	/* CIA 2 FLAG 1 */

#define I_CIA2SP	 10 	/* CIA 2 serial port */
#define I_CIA2TOD	 11 	/* CIA 2 TOD alarm */
#define I_CIA2TA	 12	/* CIA 2 timer A */
#define I_CIA2TB	 13	/* CIA 2 timer B */

/* VIA 2 IRQs */
#define	I_VIA1FL	I_CIA1FL

/* VIA 1 NMIs */
#define	I_VIA2FL	I_CIA2FL

#ifdef PET
/* PET IRQs */
#define	I_VIAFL		I_CIA1FL	/* pet via flag */
#define	I_PIA1		I_CIA1TA	/* pet via flag */
#define	I_PIA2		I_CIA1TB	/* pet via flag */
#endif

/* SPECIAL */
#define I_SPECX		14	/* Special "interrupt" for x_loop */
#define I_RESTORE	15	/* Restore key NMI */
#define I_RESET		16	/* Reset in startup */

#define NUMOFINT        17

#endif  /* VICE_VMACHINE_H */
