/*
 * vmachine.h - Miscellaneous system parameters.
 *
 * Written by
 *  Vesa-Matti Puro (vmp@lut.fi)
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta     (jopi@stekt.oulu.fi)
 *  André Fachat    (a.fachat@physik.tu-chemnitz.de)
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

/* Alarms.  */

#if 0

#define A_RASTERDRAW	0 	/* Draw one raster line. */
#define A_RASTERFETCH	1 	/* Video chip memory fetch. */
#define A_RASTER	2	/* Raster compare. */
#define A_CIA1TOD	3 	/* CIA 1 TOD alarm */
#define A_CIA1TA	4       /* CIA 1 timer A */
#define A_CIA1TB	5  	/* CIA 1 timer B */
#define A_CIA2TOD	6   	/* CIA 2 TOD alarm */
#define A_CIA2TA	7   	/* CIA 2 timer A */
#define A_CIA2TB	8   	/* CIA 2 timer B */

#ifdef HAVE_RS232
#define A_ACIA1		9   	/* ACIA */
#define A_RSUSER	10
#define NUMOFALRM	11
#else
#define NUMOFALRM       9
#endif

#define	A_VIA1T1	A_CIA1TA
#define	A_VIA1T2	A_CIA1TB
#define	A_VIA2T1	A_CIA2TA
#define	A_VIA2T2	A_CIA2TB

/* PET IRQs */
#define	A_VIAT1		A_CIA1TA	/* pet via t1 */
#define	A_VIAT2		A_CIA1TB	/* pet via t2 */

/* CBM-II IRQs */
#ifndef HAVE_RS232
#define A_ACIA1         A_CIA2TA
#endif

#endif

/* ------------------------------------------------------------------------- */

/* Interrupts.  This is a bit of a mess...  */

enum {
    I_ACIA1,                    /* ACIA 1 */
    I_ACIA2,                    /* ACIA 2 (unused) */
    I_CIA1FL,                   /* CIA 1 */
    I_CIA2FL,                   /* CIA 2 */
    I_FREEZE,                   /* Cartridge freeze */
    I_PIA1,                     /* PIA1 */
    I_PIA2,                     /* PIA2 */
    I_RASTER,                   /* Raster compare */
    I_RESTORE,                  /* RESTORE key */
    I_REU,                      /* REU */
    I_TPI1,                     /* TPI 1 (CBM-II) */
    I_VIA1FL,                   /* VIA 1 */
    I_VIA2FL,                   /* VIA 2 */
    I_VIAFL,                    /* VIA (PET) */
    NUMOFINT
};

#endif  /* VICE_VMACHINE_H */
