/*
 * tapeunit.c - (Guess what?) Tape unit emulation.
 *
 * Written by
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#ifndef _TAPEUNIT_H
#define _TAPEUNIT_H

#include "tape.h"

#define  CAS_BUFFER_OFFSET  (ram[0xB2] | (ram[0xB3] << 8))  /* C64,128 TAPE1 */

#define  VERCK			0x93


#ifdef PET
#define FIND_RET		0xF
#define WRITE_RET		0xF
#define TRANSFER_RET		0xF	/* BASIC 1: Restore Normal IRQ */

#define FIND_2_RET		0xF
#define WRITE_2_RET		0xF
#define TRANSFER_2_RET		0xFC7B	/* BASIC 2: Restore Normal IRQ */

#define FIND_4_RET		0xF
#define WRITE_4_RET		0xF
#define TRANSFER_4_RET		0xFCC0	/* BASIC 4: Restore Normal IRQ */
#endif


#ifdef VIC20
#define  IRQTMP			0x2A0

#define FIND_RET		0xF7B5
#define WRITE_RET		0xF83E
#define TRANSFER_RET		0xFCCF	/* VIC20: Restore Normal IRQ */
#endif

#ifdef CBM64
#define  IRQTMP			0x2A0

#define FIND_RET		0xF732
#define WRITE_RET		0xF7C1
#define TRANSFER_RET		0xFC93	/* C64: Restore Normal IRQ */
#endif

#ifdef C128
#define  IRQTMP			0xA0A

#define FIND_RET		0xE8D6
#define WRITE_RET		0xE971
#define TRANSFER_RET		0xEE57	/* C128: Restore Normal IRQ */
#endif

#ifdef PET
#define SET_ST(b)	STORE(0x96, (LOAD(0x96) | b))
#define BSOUR			0xA5	/* Buffered Character for IEEE Bus */
#define TMP_IN			0xA5
#define IRQTMP                  0x000   /* FIXME... */
#else
#define SET_ST(b)	STORE(0x90, (LOAD(0x90) | b))
#define BSOUR			0x95	/* Buffered Character for Serial Bus */
#define TMP_IN			0xA4	/* Temp Data Area */
#endif

/* ------------------------------------------------------------------------- */

/* Global functions */
extern void  detach_tape_image(TAPE *tape);
extern int   attach_tape_image(TAPE *tape, const char *name, int mode);
extern int   check_tape(void);
extern int   initialize_tape(int number);

#endif /* ndef _TAPEUNIT_H */
