/*
 * via.h - VIA emulation.
 *
 * Written by
 *   Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _VIA_H
#define _VIA_H

#include "types.h"

/*
 * MOS 6522 registers
 */
#define VIA_PRB		0  /* Port B */
#define VIA_PRA		1  /* Port A */
#define VIA_DDRB	2  /* Data direction register for port B */
#define VIA_DDRA	3  /* Data direction register for port A */

#define VIA_T1CL	4  /* Timer 1 count low */
#define VIA_T1CH	5  /* Timer 1 count high */
#define VIA_T1LL	6  /* Timer 1 latch low */
#define VIA_T1LH	7  /* Timer 1 latch high */
#define VIA_T2CL	8  /* Timer 2 count low - read only */
#define VIA_T2LL	8  /* Timer 2 latch low - write only */
#define VIA_T2CH	9  /* Timer 2 latch/count high */

#define VIA_SR		10 /* Serial port shift register */
#define VIA_ACR		11 /* Auxiliary control register */
#define VIA_PCR		12 /* Peripheral control register */

#define VIA_IFR		13 /* Interrupt flag register */
#define VIA_IER		14 /* Interrupt control register */
#define VIA_PRA_NHS	15 /* Port A with no handshake */

/*
 * Interrupt Masks
 */

/* MOS 6522 */
#define VIA_IM_IRQ	128	/* Control Bit */
#define VIA_IM_T1	64	/* Timer 1 underflow */
#define VIA_IM_T2	32	/* Timer 2 underflow */
#define VIA_IM_CB1	16	/* Handshake */
#define VIA_IM_CB2	8	/* Handshake */
#define VIA_IM_SR	4	/* Shift Register completion */
#define VIA_IM_CA1	2	/* Handshake */
#define VIA_IM_CA2	1	/* Handshake */


/* 
 * signal values (for signaling edges on the control lines)
 */

#define	VIA_SIG_CA1	0
#define	VIA_SIG_CA2	1
#define	VIA_SIG_CB1	2
#define	VIA_SIG_CB2	3

#define	VIA_SIG_FALL	0
#define	VIA_SIG_RISE	1

/* ------------------------------------------------------------------------- */

/* Prototypes.  These are different for the PET and the VIC20. */
/* FIXME: we might separate these into different header files for each. */

#if defined(VIC20)

extern void reset_via1(void);
extern void via1_signal(int line, int edge);
extern void REGPARM2 store_via1(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via1(ADDRESS addr);
extern int int_via1t1(long offset);
extern int int_via1t2(long offset);
extern void via1_prevent_clk_overflow(void);

extern void reset_via2(void);
extern void via2_signal(int line, int edge);
extern void REGPARM2 store_via2(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via2(ADDRESS addr);
extern int int_via2t1(long offset);
extern int int_via2t2(long offset);
extern void via2_prevent_clk_overflow(void);

#elif defined(PET)

extern void reset_via(void);
extern void via_signal(int line, int edge);
extern void REGPARM2 store_via(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 read_via(ADDRESS addr);
extern int int_viat1(long offset);
extern int int_viat2(long offset);
extern void via_prevent_clk_overflow(void);
extern int show_keyarr(void);

#endif

#endif  /* _VIA_H */

