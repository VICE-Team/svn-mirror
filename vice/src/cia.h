/*
 * cia.h - Definitions for MOS6526 (CIA) chip emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _CIA_H
#define _CIA_H

#include "types.h"

#define byte2bcd(byte)  ((16*((byte)/10)+((byte)%10))&0xff)
#define bcd2byte(bcd)   ((10*((bcd&0xf0)/16)+((bcd)&0xf))&0xff)

/* MOS 6526 registers */

#define CIA_PRA		0  /* Port A */
#define CIA_PRB		1  /* Port B */
#define CIA_DDRA	2  /* Data direction register for port A */
#define CIA_DDRB	3  /* Data direction register for port B */

#define CIA_TAL		4  /* Timer A low */
#define CIA_TAH		5  /* Timer A high */
#define CIA_TBL		6  /* Timer B low */
#define CIA_TBH		7  /* Timer B high */

#define CIA_TOD_TEN	8  /* Time Of Day clock 1/10 s */
#define CIA_TOD_SEC	9  /* Time Of Day clock sec */
#define CIA_TOD_MIN	10 /* Time Of Day clock min */
#define CIA_TOD_HR	11 /* Time Of Day clock hour */

#define CIA_SDR		12 /* Serial port shift register */
#define CIA_ICR		13 /* Interrupt control register */
#define CIA_CRA		14 /* Control register A */
#define CIA_CRB		15 /* Control register B */


/* Interrupt Masks */

#define CIA_IM_SET	0x80	/* Control Bit */
#define CIA_IM_TA	1	/* Timer A underflow */
#define CIA_IM_TB	2	/* Timer B underflow */
#define CIA_IM_TOD	4	/* TOD Clock Alarm */
#define CIA_IM_SDR	8	/* Shift Register completion */
#define CIA_IM_FLG	16	/* Handshake */

#endif  /* _CIA_H */
