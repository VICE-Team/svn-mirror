/*
 * asm.h - 6510 assembler-related functions.
 *
 * Written by
 *   Vesa-Matti Puro (vmp@lut.fi)
 *   Jarkko Sonninen (sonninen@lut.fi)
 *   Jouko Valta (jopi@stekt.oulu.fi)
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm.h"
#include "misc.h"
#include "mshell.h"

extern BYTE ram[];		/* FIXME: ugly! */

/* ------------------------------------------------------------------------- */

int clength[] = { 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 2, 2, 2, 0 };

struct lookup_tag lookup[] = {

    /****  Positive  ****/

    /* 00 */ { "BRK",	IMPLIED, M_NONE, M_PC, 7, 0 },		/* Pseudo Absolute */
    /* 01 */ { "ORA",	INDIRECT_X, M_INDX, M_AC, 6, 0 },	/* (Indirect,X) */
    /* 02 */ { "JAM",	IMPLIED, M_NONE, M_NONE, 0, 0 },	/* TILT */
    /* 03 */ { "SLO",	INDIRECT_X, M_INDX, M_INDX, 8, 0 },

    /* 04 */ { "NOOP",	ZERO_PAGE, M_NONE, M_NONE, 3, 0 },
    /* 05 */ { "ORA",	ZERO_PAGE, M_ZERO, M_AC, 3, 0 },	/* Zeropage */
    /* 06 */ { "ASL",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },	/* Zeropage */
    /* 07 */ { "SLO",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },

    /* 08 */ { "PHP",	IMPLIED, M_SR, M_NONE, 3, 0 },
    /* 09 */ { "ORA",	IMMEDIATE, M_IMM, M_AC, 2, 0 },		/* Immediate */
    /* 0a */ { "ASL",	ACCUMULATOR, M_AC, M_AC, 2, 0 },	/* Accumulator */
    /* 0b */ { "ANC",	IMMEDIATE, M_ACIM, M_ACNC, 2, 0 },

    /* 0c */ { "NOOP",	ABSOLUTE, M_NONE, M_NONE, 4, 0 },
    /* 0d */ { "ORA",	ABSOLUTE, M_ABS, M_AC, 4, 0 },		/* Absolute */
    /* 0e */ { "ASL",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },		/* Absolute */
    /* 0f */ { "SLO",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },

    /* 10 */ { "BPL",	RELATIVE, M_REL, M_NONE, 2, 0 },
    /* 11 */ { "ORA",	INDIRECT_Y, M_INDY, M_AC, 5, 1 },	/* (Indirect),Y */
    /* 12 */ { "JAM",	IMPLIED, M_NONE, M_NONE, 0, 0 },	/* TILT */
    /* 13 */ { "SLO",	INDIRECT_Y, M_INDY, M_INDY, 8, 0 },

    /* 14 */ { "NOOP",	ZERO_PAGE_X, M_NONE, M_NONE, 4, 0 },
    /* 15 */ { "ORA",	ZERO_PAGE_X, M_ZERX, M_AC, 4, 0 },	/* Zeropage,X */
    /* 16 */ { "ASL",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },	/* Zeropage,X */
    /* 17 */ { "SLO",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },

    /* 18 */ { "CLC",	IMPLIED, M_NONE, M_FC, 2, 0 },
    /* 19 */ { "ORA",	ABSOLUTE_Y, M_ABSY, M_AC, 4, 1 },	/* Absolute,Y */
    /* 1a */ { "NOOP",	IMPLIED, M_NONE, M_NONE, 2, 0 },
    /* 1b */ { "SLO",	ABSOLUTE_Y, M_ABSY, M_ABSY, 7, 0 },

    /* 1c */ { "NOOP",	ABSOLUTE_X, M_NONE, M_NONE, 4, 1 },
    /* 1d */ { "ORA",	ABSOLUTE_X, M_ABSX, M_AC,   4, 1 },	/* Absolute,X */
    /* 1e */ { "ASL",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },	/* Absolute,X */
    /* 1f */ { "SLO",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },

    /* 20 */ { "JSR",	ABSOLUTE, M_ADDR, M_PC, 6, 0 },
    /* 21 */ { "AND",	INDIRECT_X, M_INDX, M_AC, 6, 0 },	/* (Indirect,X)*/
    /* 22 */ { "JAM",	IMPLIED, M_NONE, M_NONE,    0, 0 },	/* TILT */
    /* 23 */ { "RLA",	INDIRECT_X, M_INDX, M_INDX, 8, 0 },

    /* 24 */ { "BIT",	ZERO_PAGE, M_ZERO, M_NONE, 3, 0 },	/* Zeropage */
    /* 25 */ { "AND",	ZERO_PAGE, M_ZERO, M_AC,   3, 0 },	/* Zeropage */
    /* 26 */ { "ROL",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },	/* Zeropage */
    /* 27 */ { "RLA",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },

    /* 28 */ { "PLP",	IMPLIED, M_NONE, M_SR, 4, 0 },
    /* 29 */ { "AND",	IMMEDIATE, M_IMM, M_AC, 2, 0 },		/* Immediate */
    /* 2a */ { "ROL",	ACCUMULATOR, M_AC, M_AC, 2, 0 },	/* Accumulator */
    /* 2b */ { "ANC",	IMMEDIATE, M_ACIM, M_ACNC, 2, 0 },

    /* 2c */ { "BIT",	ABSOLUTE, M_ABS, M_NONE, 4, 0 },	/* Absolute */
    /* 2d */ { "AND",	ABSOLUTE, M_ABS, M_AC,  4, 0 },		/* Absolute */
    /* 2e */ { "ROL",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },		/* Absolute */
    /* 2f */ { "RLA",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },

    /* 30 */ { "BMI",	RELATIVE, M_REL, M_NONE, 2, 0 },
    /* 31 */ { "AND",	INDIRECT_Y, M_INDY, M_AC, 5, 1 },	/* (Indirect),Y */
    /* 32 */ { "JAM",	IMPLIED, M_NONE, M_NONE, 0, 0 },	/* TILT */
    /* 33 */ { "RLA",	INDIRECT_Y, M_INDY, M_INDY, 8, 0 },

    /* 34 */ { "NOOP",	ZERO_PAGE_X, M_NONE, M_NONE, 4, 0 },
    /* 35 */ { "AND",	ZERO_PAGE_X, M_ZERX, M_AC,   4, 0 },	/* Zeropage,X */
    /* 36 */ { "ROL",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },	/* Zeropage,X */
    /* 37 */ { "RLA",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },

    /* 38 */ { "SEC",	IMPLIED, M_NONE, M_FC, 2, 0 },
    /* 39 */ { "AND",	ABSOLUTE_Y, M_ABSY, M_AC, 4, 1 },	/* Absolute,Y */
    /* 3a */ { "NOOP",	IMPLIED, M_NONE, M_NONE,  2, 0 },
    /* 3b */ { "RLA",	ABSOLUTE_Y, M_ABSY, M_ABSY, 7, 0 },

    /* 3c */ { "NOOP",	ABSOLUTE_X, M_NONE, M_NONE, 4, 1 },
    /* 3d */ { "AND",	ABSOLUTE_X, M_ABSX, M_AC,   4, 1 },	/* Absolute,X */
    /* 3e */ { "ROL",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },	/* Absolute,X */
    /* 3f */ { "RLA",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },

    /* 40 */ { "RTI",	IMPLIED, M_NONE, M_PC, 6, 0 },
    /* 41 */ { "EOR",	INDIRECT_X, M_INDX, M_AC, 6, 0 },	/* (Indirect,X) */
    /* 42 */ { "JAM",	IMPLIED, M_NONE, M_NONE, 0, 0 },	/* TILT */
    /* 43 */ { "SRE",	INDIRECT_X, M_INDX, M_INDX, 8, 0 },

    /* 44 */ { "NOOP",	ZERO_PAGE, M_NONE, M_NONE, 3, 0 },
    /* 45 */ { "EOR",	ZERO_PAGE, M_ZERO, M_AC,   3, 0 },	/* Zeropage */
    /* 46 */ { "LSR",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },	/* Zeropage */
    /* 47 */ { "SRE",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },

    /* 48 */ { "PHA",	IMPLIED, M_AC, M_NONE,   3, 0 },
    /* 49 */ { "EOR",	IMMEDIATE, M_IMM, M_AC,  2, 0 },	/* Immediate */
    /* 4a */ { "LSR",	ACCUMULATOR, M_AC, M_AC, 2, 0 },	/* Accumulator */
    /* 4b */ { "ASR",	IMMEDIATE, M_ACIM, M_AC, 2, 0 },	/* (AC & IMM) >>1 */

    /* 4c */ { "JMP",	ABSOLUTE, M_ADDR, M_PC, 3, 0 },		/* Absolute */
    /* 4d */ { "EOR",	ABSOLUTE, M_ABS, M_AC,  4, 0 },		/* Absolute */
    /* 4e */ { "LSR",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },		/* Absolute */
    /* 4f */ { "SRE",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },

    /* 50 */ { "BVC",	RELATIVE, M_REL, M_NONE,  2, 0 },
    /* 51 */ { "EOR",	INDIRECT_Y, M_INDY, M_AC, 5, 1 },	/* (Indirect),Y */
    /* 52 */ { "JAM",	IMPLIED, M_NONE, M_NONE,  0, 0 },	/* TILT */
    /* 53 */ { "SRE",	INDIRECT_Y, M_INDY, M_INDY, 8, 0 },

    /* 54 */ { "NOOP",	ZERO_PAGE_X, M_NONE, M_NONE, 4, 0 },
    /* 55 */ { "EOR",	ZERO_PAGE_X, M_ZERX, M_AC,   4, 0 },	/* Zeropage,X */
    /* 56 */ { "LSR",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },	/* Zeropage,X */
    /* 57 */ { "SRE",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },

    /* 58 */ { "CLI",	IMPLIED, M_NONE, M_FI,     2, 0 },
    /* 59 */ { "EOR",	ABSOLUTE_Y, M_ABSY, M_AC,  4, 1 },	/* Absolute,Y */
    /* 5a */ { "NOOP",	IMPLIED, M_NONE, M_NONE,   2, 0 },
    /* 5b */ { "SRE",	ABSOLUTE_Y, M_ABSY, M_ABSY, 7, 0 },

    /* 5c */ { "NOOP",	ABSOLUTE_X, M_NONE, M_NONE, 4, 1 },
    /* 5d */ { "EOR",	ABSOLUTE_X, M_ABSX, M_AC,   4, 1 },	/* Absolute,X */
    /* 5e */ { "LSR",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },	/* Absolute,X */
    /* 5f */ { "SRE",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },

    /* 60 */ { "RTS",	IMPLIED, M_NONE, M_PC, 6, 0 },
    /* 61 */ { "ADC",	INDIRECT_X, M_INDX, M_AC, 6, 0 },	/* (Indirect,X) */
    /* 62 */ { "JAM",	IMPLIED, M_NONE, M_NONE, 0, 0 },	/* TILT */
    /* 63 */ { "RRA",	INDIRECT_X, M_INDX, M_INDX, 8, 0 },

    /* 64 */ { "NOOP",	ZERO_PAGE, M_NONE, M_NONE, 3, 0 },
    /* 65 */ { "ADC",	ZERO_PAGE, M_ZERO, M_AC,   3, 0 },	/* Zeropage */
    /* 66 */ { "ROR",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },	/* Zeropage */
    /* 67 */ { "RRA",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },

    /* 68 */ { "PLA",	IMPLIED, M_NONE, M_AC,   4, 0 },
    /* 69 */ { "ADC",	IMMEDIATE, M_IMM, M_AC,  2, 0 },	/* Immediate */
    /* 6a */ { "ROR",	ACCUMULATOR, M_AC, M_AC, 2, 0 },	/* Accumulator */
    /* 6b */ { "ARR",	IMMEDIATE, M_ACIM, M_AC, 2, 0 },	/* ARR isn't typo */

    /* 6c */ { "JMP",	ABS_INDIRECT, M_AIND, M_PC,  5, 0 },	/* Indirect */
    /* 6d */ { "ADC",	ABSOLUTE, M_ABS, M_AC,  4, 0 },		/* Absolute */
    /* 6e */ { "ROR",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },		/* Absolute */
    /* 6f */ { "RRA",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },

    /* 70 */ { "BVS",	RELATIVE, M_REL, M_NONE,  2, 0 },
    /* 71 */ { "ADC",	INDIRECT_Y, M_INDY, M_AC, 5, 1 },	/* (Indirect),Y */
    /* 72 */ { "JAM",	IMPLIED, M_NONE, M_NONE,  0, 0 },	/* TILT relative? */
    /* 73 */ { "RRA",	INDIRECT_Y, M_INDY, M_INDY, 8, 0 },

    /* 74 */ { "NOOP",	ZERO_PAGE_X, M_NONE, M_NONE, 4, 0 },
    /* 75 */ { "ADC",	ZERO_PAGE_X, M_ZERX, M_AC,   4, 0 },	/* Zeropage,X */
    /* 76 */ { "ROR",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },	/* Zeropage,X */
    /* 77 */ { "RRA",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },

    /* 78 */ { "SEI",	IMPLIED, M_NONE, M_FI, 2, 0 },
    /* 79 */ { "ADC",	ABSOLUTE_Y, M_ABSY, M_AC, 4, 1 },	/* Absolute,Y */
    /* 7a */ { "NOOP",	IMPLIED, M_NONE, M_NONE,  2, 0 },
    /* 7b */ { "RRA",	ABSOLUTE_Y, M_ABSY, M_ABSY, 7, 0 },

    /* 7c */ { "NOOP",	ABSOLUTE_X, M_NONE, M_NONE, 4, 1 },
    /* 7d */ { "ADC",	ABSOLUTE_X, M_ABSX, M_AC,   4, 1 },	/* Absolute,X */
    /* 7e */ { "ROR",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },	/* Absolute,X */
    /* 7f */ { "RRA",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },

    /****  Negative  ****/

    /* 80 */ { "NOOP",	IMMEDIATE, M_NONE, M_NONE, 2, 0 },
    /* 81 */ { "STA",	INDIRECT_X, M_AC, M_INDX,  6, 0 },	/* (Indirect,X) */
    /* 82 */ { "NOOP",	IMMEDIATE, M_NONE, M_NONE,  2, 0 },
    /* 83 */ { "SAX",	INDIRECT_X, M_ANXR, M_INDX, 6, 0 },

    /* 84 */ { "STY",	ZERO_PAGE, M_YR, M_ZERO,  3, 0 },	/* Zeropage */
    /* 85 */ { "STA",	ZERO_PAGE, M_AC, M_ZERO,  3, 0 },	/* Zeropage */
    /* 86 */ { "STX",	ZERO_PAGE, M_XR, M_ZERO,  3, 0 },	/* Zeropage */
    /* 87 */ { "SAX",	ZERO_PAGE, M_ANXR, M_ZERO, 3, 0 },

    /* 88 */ { "DEY",	IMPLIED, M_YR, M_YR, 2, 0 },
    /* 89 */ { "NOOP",	IMMEDIATE, M_NONE, M_NONE, 2, 0 },
    /* 8a */ { "TXA",	IMPLIED, M_XR, M_AC, 2, 0 },
    /****  very abnormal: usually AC = AC | #$EE & XR & #$oper  ****/
    /* 8b */ { "ANE",	IMMEDIATE, M_AXIM, M_AC, 2, 0 },

    /* 8c */ { "STY",	ABSOLUTE, M_YR, M_ABS, 4, 0 },		/* Absolute */
    /* 8d */ { "STA",	ABSOLUTE, M_AC, M_ABS, 4, 0 },		/* Absolute */
    /* 8e */ { "STX",	ABSOLUTE, M_XR, M_ABS, 4, 0 },		/* Absolute */
    /* 8f */ { "SAX",	ABSOLUTE, M_ANXR, M_ABS, 4, 0 },

    /* 90 */ { "BCC",	RELATIVE, M_REL, M_NONE, 2, 0 },
    /* 91 */ { "STA",	INDIRECT_Y, M_AC, M_INDY, 6, 0 },	/* (Indirect),Y */
    /* 92 */ { "JAM",	IMPLIED, M_NONE, M_NONE, 0, 0 },		/* TILT relative? */
    /* 93 */ { "SHA",	INDIRECT_Y, M_ANXR, M_STH0, 6, 0 },

    /* 94 */ { "STY",	ZERO_PAGE_X, M_YR, M_ZERX, 4, 0 },	/* Zeropage,X */
    /* 95 */ { "STA",	ZERO_PAGE_X, M_AC, M_ZERX, 4, 0 },	/* Zeropage,X */
    /* 96 */ { "STX",	ZERO_PAGE_Y, M_XR, M_ZERY, 4, 0 },	/* Zeropage,Y */
    /* 97 */ { "SAX",	ZERO_PAGE_Y, M_ANXR, M_ZERY, 4, 0 },

    /* 98 */ { "TYA",	IMPLIED, M_YR, M_AC, 2, 0 },
    /* 99 */ { "STA",	ABSOLUTE_Y, M_AC, M_ABSY, 5, 0 },	/* Absolute,Y */
    /* 9a */ { "TXS",	IMPLIED, M_XR, M_SP, 2, 0 },
    /*** This is very mysterious command ... */
    /* 9b */ { "SHS",	ABSOLUTE_Y, M_ANXR, M_STH3, 5, 0 },

    /* 9c */ { "SHY",	ABSOLUTE_X, M_YR, M_STH2, 5, 0 },
    /* 9d */ { "STA",	ABSOLUTE_X, M_AC, M_ABSX, 5, 0 },	/* Absolute,X */
    /* 9e */ { "SHX",	ABSOLUTE_Y, M_XR, M_STH1, 5, 0 },
    /* 9f */ { "SHA",	ABSOLUTE_Y, M_ANXR, M_STH1, 5, 0 },

    /* a0 */ { "LDY",	IMMEDIATE, M_IMM, M_YR, 2, 0 },		/* Immediate */
    /* a1 */ { "LDA",	INDIRECT_X, M_INDX, M_AC, 6, 0 },	/* (Indirect,X) */
    /* a2 */ { "LDX",	IMMEDIATE, M_IMM, M_XR, 2, 0 },		/* Immediate */
    /* a3 */ { "LAX",	INDIRECT_X, M_INDX, M_ACXR, 6, 0 },	/* (indirect,X) */

    /* a4 */ { "LDY",	ZERO_PAGE, M_ZERO, M_YR, 3, 0 },		/* Zeropage */
    /* a5 */ { "LDA",	ZERO_PAGE, M_ZERO, M_AC, 3, 0 },		/* Zeropage */
    /* a6 */ { "LDX",	ZERO_PAGE, M_ZERO, M_XR, 3, 0 },		/* Zeropage */
    /* a7 */ { "LAX",	ZERO_PAGE, M_ZERO, M_ACXR, 3, 0 },

    /* a8 */ { "TAY",	IMPLIED, M_AC, M_YR,    2, 0 },
    /* a9 */ { "LDA",	IMMEDIATE, M_IMM, M_AC, 2, 0 },		/* Immediate */
    /* aa */ { "TAX",	IMPLIED, M_AC, M_XR,    2, 0 },
    /* ab */ { "LXA",	IMMEDIATE, M_ACIM, M_ACXR, 2, 0 },	/* LXA isn't a typo */

    /* ac */ { "LDY",	ABSOLUTE, M_ABS, M_YR, 4, 0 },		/* Absolute */
    /* ad */ { "LDA",	ABSOLUTE, M_ABS, M_AC, 4, 0 },		/* Absolute */
    /* ae */ { "LDX",	ABSOLUTE, M_ABS, M_XR, 4, 0 },		/* Absolute */
    /* af */ { "LAX",	ABSOLUTE, M_ABS, M_ACXR, 4, 0 },

    /* b0 */ { "BCS",	RELATIVE, M_REL, M_NONE,  2, 0 },
    /* b1 */ { "LDA",	INDIRECT_Y, M_INDY, M_AC, 5, 1 },	/* (indirect),Y */
    /* b2 */ { "JAM",	IMPLIED, M_NONE, M_NONE,  0, 0 },	/* TILT */
    /* b3 */ { "LAX",	INDIRECT_Y, M_INDY, M_ACXR, 5, 1 },

    /* b4 */ { "LDY",	ZERO_PAGE_X, M_ZERX, M_YR, 4, 0 },	/* Zeropage,X */
    /* b5 */ { "LDA",	ZERO_PAGE_X, M_ZERX, M_AC, 4, 0 },	/* Zeropage,X */
    /* b6 */ { "LDX",	ZERO_PAGE_Y, M_ZERY, M_XR, 4, 0 },	/* Zeropage,Y */
    /* b7 */ { "LAX",	ZERO_PAGE_Y, M_ZERY, M_ACXR, 4, 0 },

    /* b8 */ { "CLV",	IMPLIED, M_NONE, M_FV,    2, 0 },
    /* b9 */ { "LDA",	ABSOLUTE_Y, M_ABSY, M_AC, 4, 1 },	/* Absolute,Y */
    /* ba */ { "TSX",	IMPLIED, M_SP, M_XR,      2, 0 },
    /* bb */ { "LAS",	ABSOLUTE_Y, M_SABY, M_ACXS, 4, 1 },

    /* bc */ { "LDY",	ABSOLUTE_X, M_ABSX, M_YR, 4, 1 },	/* Absolute,X */
    /* bd */ { "LDA",	ABSOLUTE_X, M_ABSX, M_AC, 4, 1 },	/* Absolute,X */
    /* be */ { "LDX",	ABSOLUTE_Y, M_ABSY, M_XR, 4, 1 },	/* Absolute,Y */
    /* bf */ { "LAX",	ABSOLUTE_Y, M_ABSY, M_ACXR, 4, 1 },

    /* c0 */ { "CPY",	IMMEDIATE, M_IMM, M_NONE, 2, 0 },	/* Immediate */
    /* c1 */ { "CMP",	INDIRECT_X, M_INDX, M_NONE, 6, 0 },	/* (Indirect,X) */
    /* c2 */ { "NOOP",	IMMEDIATE, M_NONE, M_NONE, 2, 0 },	/* occasional TILT */
    /* c3 */ { "DCP",	INDIRECT_X, M_INDX, M_INDX, 8, 0 },

    /* c4 */ { "CPY",	ZERO_PAGE, M_ZERO, M_NONE, 3, 0 },	/* Zeropage */
    /* c5 */ { "CMP",	ZERO_PAGE, M_ZERO, M_NONE, 3, 0 },	/* Zeropage */
    /* c6 */ { "DEC",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },	/* Zeropage */
    /* c7 */ { "DCP",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },

    /* c8 */ { "INY",	IMPLIED, M_YR, M_YR, 2, 0 },
    /* c9 */ { "CMP",	IMMEDIATE, M_IMM, M_NONE, 2, 0 },	/* Immediate */
    /* ca */ { "DEX",	IMPLIED, M_XR, M_XR, 2, 0 },
    /* cb */ { "SBX",	IMMEDIATE, M_IMM, M_XR, 2, 0 },

    /* cc */ { "CPY",	ABSOLUTE, M_ABS, M_NONE, 4, 0 },		/* Absolute */
    /* cd */ { "CMP",	ABSOLUTE, M_ABS, M_NONE, 4, 0 },		/* Absolute */
    /* ce */ { "DEC",	ABSOLUTE, M_ABS, M_ABS,  6, 0 },		/* Absolute */
    /* cf */ { "DCP",	ABSOLUTE, M_ABS, M_ABS,  6, 0 },

    /* d0 */ { "BNE",	RELATIVE, M_REL, M_NONE, 2, 0 },
    /* d1 */ { "CMP",	INDIRECT_Y, M_INDY, M_NONE, 5, 1 },	/* (Indirect),Y */
    /* d2 */ { "JAM",	IMPLIED, M_NONE, M_NONE,    0, 0 },	/* TILT */
    /* d3 */ { "DCP",	INDIRECT_Y, M_INDY, M_INDY, 8, 0 },

    /* d4 */ { "NOOP",	ZERO_PAGE_X, M_NONE, M_NONE, 4, 0 },
    /* d5 */ { "CMP",	ZERO_PAGE_X, M_ZERX, M_NONE, 4, 0 },	/* Zeropage,X */
    /* d6 */ { "DEC",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },	/* Zeropage,X */
    /* d7 */ { "DCP",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },

    /* d8 */ { "CLD",	IMPLIED, M_NONE, M_FD, 2, 0 },
    /* d9 */ { "CMP",	ABSOLUTE_Y, M_ABSY, M_NONE, 4, 1 },	/* Absolute,Y */
    /* da */ { "NOOP",	IMPLIED, M_NONE, M_NONE,    2, 0 },
    /* db */ { "DCP",	ABSOLUTE_Y, M_ABSY, M_ABSY, 7, 0 },

    /* dc */ { "NOOP",	ABSOLUTE_X, M_NONE, M_NONE, 4, 1 },
    /* dd */ { "CMP",	ABSOLUTE_X, M_ABSX, M_NONE, 4, 1 },	/* Absolute,X */
    /* de */ { "DEC",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },	/* Absolute,X */
    /* df */ { "DCP",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },

    /* e0 */ { "CPX",	IMMEDIATE, M_IMM, M_NONE, 2, 0 },	/* Immediate */
    /* e1 */ { "SBC",	INDIRECT_X, M_INDX, M_AC, 6, 0 },	/* (Indirect,X) */
    /* e2 */ { "NOOP",	IMMEDIATE, M_NONE, M_NONE,  2, 0 },
    /* e3 */ { "ISB",	INDIRECT_X, M_INDX, M_INDX, 8, 0 },

    /* e4 */ { "CPX",	ZERO_PAGE, M_ZERO, M_NONE, 3, 0 },	/* Zeropage */
    /* e5 */ { "SBC",	ZERO_PAGE, M_ZERO, M_AC,   3, 0 },	/* Zeropage */
    /* e6 */ { "INC",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },	/* Zeropage */
    /* e7 */ { "ISB",	ZERO_PAGE, M_ZERO, M_ZERO, 5, 0 },

    /* e8 */ { "INX",	IMPLIED, M_XR, M_XR,     2, 0 },
    /* e9 */ { "SBC",	IMMEDIATE, M_IMM, M_AC,  2, 0 },		/* Immediate */
    /* ea */ { "NOP",	IMPLIED, M_NONE, M_NONE, 2, 0 },
    /* eb */ { "USBC",	IMMEDIATE, M_IMM, M_AC,  2, 0 },		/* same as e9 */

    /* ec */ { "CPX",	ABSOLUTE, M_ABS, M_NONE, 4, 0 },		/* Absolute */
    /* ed */ { "SBC",	ABSOLUTE, M_ABS, M_AC,  4, 0 },		/* Absolute */
    /* ee */ { "INC",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },		/* Absolute */
    /* ef */ { "ISB",	ABSOLUTE, M_ABS, M_ABS, 6, 0 },

    /* f0 */ { "BEQ",	RELATIVE, M_REL, M_NONE,  2, 0 },
    /* f1 */ { "SBC",	INDIRECT_Y, M_INDY, M_AC, 5, 1 },	/* (Indirect),Y */
    /* f2 */ { "JAM",	IMPLIED, M_NONE, M_NONE,  0, 0 },	/* TILT */
    /* f3 */ { "ISB",	INDIRECT_Y, M_INDY, M_INDY, 8, 0 },

    /* f4 */ { "NOOP",	ZERO_PAGE_X, M_NONE, M_NONE, 4, 0 },
    /* f5 */ { "SBC",	ZERO_PAGE_X, M_ZERX, M_AC,   4, 0 },	/* Zeropage,X */
    /* f6 */ { "INC",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },	/* Zeropage,X */
    /* f7 */ { "ISB",	ZERO_PAGE_X, M_ZERX, M_ZERX, 6, 0 },

    /* f8 */ { "SED",	IMPLIED, M_NONE, M_FD,    2, 0 },
    /* f9 */ { "SBC",	ABSOLUTE_Y, M_ABSY, M_AC, 4, 1 },	/* Absolute,Y */
    /* fa */ { "NOOP",	IMPLIED, M_NONE, M_NONE,  2, 0 },
    /* fb */ { "ISB",	ABSOLUTE_Y, M_ABSY, M_ABSY, 7, 0 },

    /* fc */ { "NOOP",	ABSOLUTE_X, M_NONE, M_NONE, 4, 1 },
    /* fd */ { "SBC",	ABSOLUTE_X, M_ABSX, M_AC,   4, 1 },	/* Absolute,X */
    /* fe */ { "INC",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 },	/* Absolute,X */
    /* ff */ { "ISB",	ABSOLUTE_X, M_ABSX, M_ABSX, 7, 0 }
};

char *modename[] = {
    "IMPLIED", "ACCUMULATOR", "IMMEDIATE",
#ifdef EXTENDED_CPU
    "IMMEDIATE_WORD", "ZERO_PAGE", "ZERO_PAGE_X", "ZERO_PAGE_Y", "ABSOLUTE",
    "ABSOLUTE_X", "ABSOLUTE_Y", "INDIRECT", "INDIRECT_X", "INDIRECT_Y",
    "INDIRECT_Z", "ABS_INDIRECT", "ABS_INDIR_X", "RELATIVE", "RELATIVE_LONG",
    "ZERO_RELATIVE", "STACK_RELATIVE_Y",
#else
    "ZERO_PAGE", "ZERO_PAGE_X", "ZERO_PAGE_Y", "ABSOLUTE", "ABSOLUTE_X",
    "ABSOLUTE_Y", "ABS_INDIRECT", "INDIRECT_X", "INDIRECT_Y", "RELATIVE",
#endif
    "ASS_CODE"
};	       /* ADDRESSING MODES */


/* ------------------------------------------------------------------------- */


/* local */

static int interpret_line(char *, ADDRESS *, int );
int interpret_instr(char *, ADDRESS , int );
int find_instr(char *);
static int parse_arguments(ADDRESS , int , char *, BYTE *, int *, int );


/*
 * This function implements a simple MOS6502 assembler.
 * The interpreter selects default number base according to the
 * value of hexflg.
 */

int     ass(ADDRESS addr, int mode)
{
    char   *line, prompt[10];
    int     cnt;
    int     errors = 0;

    for (;;) {
	sprintf(prompt, ".%04x ", addr);
	line = read_line(prompt, 0);    /* mode & MODE_INF */

	if (line == NULL || *line == 'x')
	    break;

	else
	    if ((cnt = interpret_line(line, &addr, mode)) >= 0) {
		/* addr += cnt; */
	    }
	    else {
		printf("**** %04X ", addr);
		switch(cnt) {

		    /* Line syntax */
		  case E_SYNTAX: printf(EM_SYNTAX);
		    break;
		  case E_PARSE_ERROR: printf(EM_PARSE_ERROR);
		    break;
		  case E_TOO_MANY_ERRORS: /* Don't print */
		    break;

		    /* Assembler */
		  case E_BAD_IDENTIFIER: printf(EM_BAD_IDENTIFIER);
		    break;
		  case E_SYMBOL_UNDEFINED: printf(EM_SYMBOL_UNDEFINED);
		    break;
		  case E_SYMBOL_REDEF: printf(EM_SYMBOL_REDEF);
		    break;
		  case E_PC_DECREMENT: printf(EM_PC_DECREMENT); /*non-intract*/
		    break;

		    /* Mnemonic */
		  case E_BAD_MNEM: printf(EM_BAD_MNEM);
		    break;
		  case E_LONG_BRANCH: printf(EM_LONG_BRANCH);
		    break;
		  case E_MISSING_OPER: printf(EM_MISSING_OPER);
		    break;

		    /* Operand syntax */
		  case E_PARAMETER_SYNTAX: printf(EM_PARAMETER_SYNTAX);
		    break;
		  case E_TOO_MANY_COMMAS: printf(EM_TOO_MANY_COMMAS);
		    break;
		  case E_RIGHT_PARENTHESIS: printf(EM_RIGHT_PARENTHESIS);
		    break;
		  case E_LEFT_PARENTHESIS: printf(EM_LEFT_PARENTHESIS);
		    break;
		  case E_PARENTHESIS: printf(EM_PARENTHESIS);
		    break;
		  case E_MIXED_XY: printf(EM_MIXED_XY);
		    break;
		  case E_MISSING_XY: printf(EM_MISSING_XY);
		    break;
		  case E_BAD_INDEX: printf(EM_BAD_INDEX);
		    break;
		  default:
		    printf("SYNTAX ERROR (%d)", cnt);
		}
 		printf(": %s ****\n", line);

		if ( ++errors >= ERRORS_TO_STOP) {
		    printf("%s\nStop.\n\n", EM_TOO_MANY_ERRORS);
		    return (E_TOO_MANY_ERRORS);
		}
	    }
    } /* for */

    return (E_OK);
}


/*
 * This routine interprets assembly lines which are not direct mode
 * commands for the program itself.
 * Second iteration is done if the line contains address declaration.
 */

static int interpret_line(char *line, ADDRESS *addr, int mode)
{
    int     len = 0;
    int attempt = 0;

    ADDRESS adr = *addr;

    do {
	while (*line && isspace((int)*line))
	    line++;

	if (!*line)
	    return 0; /* line doesn't contain any text */

	/*
	 * Machine language commands
	 * Return if any mnemonic is found or error occurred.
	 */

#ifdef HAS_ZILOG_Z80
	if (mode & MODE_ZILOG) {
	    if ((len =  parse_z80_instr(line, adr, ram, mode)) > 0) {
		*addr = (len + adr);   /* addr is changed only on success */
		return (len);
	    }
	} else
#endif
	if ((len =  interpret_instr(line, adr, mode)) > 0) {
	    *addr = (len + adr);   /* addr is changed only on success */
	    return (len);
	}

	if (len != E_BAD_MNEM)
	    return (len);

	/*
	 * Change address
	 */

	if (!*line || !sconv(line, 0, mode | MODE_QUERY))
	    return(E_BAD_IDENTIFIER);

	adr = sconv(line, 0, mode);
	while(*++line > ' ');

	if (mode & MODE_VERBOSE)
	    printf("\nrescan %04x ===>%s<===\n", adr, line);

    } while(*line && !attempt++);

    return (E_SYNTAX);
}	/* end of interpret_line  */


/*
 * Machine language commands
 * This routine interprets all 6502 intructions. On success data is
 * stored to memory and number of bytes is returned, otherwise possible
 * error code is returned.
 */

int interpret_instr(char *line, ADDRESS adr, int mode)
{
    int     i = 0;
    int     instr = 0;
    int     len = 0;
    unsigned char arg[4];

    /*
     * First all legal mnemonics are searched and if right instruction is
     * found the remaining line is parsed. The numeric code of  the
     * instruction is returned and arguments are returned in arg array.
     */

    if ((i = find_instr(line)) >= 0) {

	if (lookup[i].mnemonic[3]) ++line;	/* USBC, NOOP, RBMn ... */

	instr = parse_arguments
	    (adr, i, (strlen(line) > 3 ? &line[3] : NULL), arg, &len, mode);

	if (instr < 0)
	    return (instr);	/* Error code from parse_arguments */

	ram[adr] = instr;
	if (len)
	    memcpy(ram + adr + 1, arg, len);

	if (!(mode & MODE_QUIET)) {
	    printf(".%04X %02X ", adr, instr);
	    switch (len) {
	      case 0:
		printf("\t%s\n", sprint_opcode(adr, 1));
		break;
	      case 1:
		printf("%02X\t%s\n", arg[0], sprint_opcode(adr, 1));
		break;
	      case 2:
		printf("%02X %02X\t%s\n", arg[0], arg[1],
		       sprint_opcode(adr, 1));
	    }
	}
	return (++len);

    }  /* end of matching mnemonic */

    return (E_BAD_MNEM);
}	/* end of interpret_instr */



int find_instr(char *line)
{
    int i;

    while (*line && isspace((int)*line))
        line++;

    if (!*line)
        return (-1); /* line doesn't contain any mnemonic */

    for (i = 0; line[i] && i < 3; i++) {
        if (isalpha ((int)line[i]))
            line[i] = toupper (line[i]);
    }

    for (i = 0; i < TOTAL_CODES; i++)
        if (0 == strncmp(lookup[i].mnemonic, line, 3) &&
            (lookup[i].mnemonic[3] < ' ' ||	lookup[i].mnemonic[3] == line[3]))
            return (i);		/* USBC, NOOP, RBMn ... */

    return (-1);
}


static int parse_arguments(ADDRESS adr, int code, char *line, BYTE *arg, int *len, int mode)
{
    int     tstflg = mode & MODE_VERBOSE;
    int     comma_found = 0;	/* True if argument contains comma. */
    int     right_parenthesis = 0;
    int     left_parenthesis = 0;
    int     x_found = 0;
    int     y_found = 0;
    int     z_found = 0;
    int     zero_page = 0;
    int     addr_mode = 0;
    int     i, ival;
    char   *linep;

    while (line && *line && isspace((int)*line))
	line++;

    if (NULL == line || !*line) {	/* IMPLIED addressing mode. */
	for (i = code; i <= OP_IMPL_MAX; i += OP_IMPL_SPC)
	    if (((IMPLIED == lookup[i].addr_mode) ||
		 (ACCUMULATOR == lookup[i].addr_mode)) &&
		0 == strcmp(lookup[code].mnemonic, lookup[i].mnemonic)) {
		*len = 0;
		return i;
	    }
	return (E_MISSING_OPER);
    }

    if (tstflg)
	printf("CODE %d strlen %d --%s--\n", code, (int)strlen(line), line);

    /*
     * Explicit ACCUMULATOR addressing. If the length of the argument is 1
     * and if this argument is A, appropriate machine code with right address
     * mode is right is chosen, otherwise E_PARAMETER_SYNTAX is returned.
     * This errorcode means that argument is A (=accumulator), but this
     * instruction does not support accumulator addressing. If the parameter
     * A is not present then it matches IMPLIED address mode above.
     */

    if (mode & MODE_SYMBOL) {
	if (1 == strlen(line) && (*line == 'A' || *line == 'a')) {
	    for (i = code; i <= OP_ACCU_MAX; i += OP_MNEM_SPC)
		if (ACCUMULATOR == lookup[i].addr_mode &&
		    0 == strcmp(lookup[code].mnemonic, lookup[i].mnemonic)) {
		    *len = 0;
		    return i;
		}
	    return E_PARAMETER_SYNTAX;
	}
	if (tstflg)
	    printf("ACCU testattu\n");
    }

    /* IMMEDIATE addressing mode */

    if (*line == '#') {
	for (i = code; i < OP_IMM_MAX; ++i)
	    if (IMMEDIATE == lookup[i].addr_mode &&
		0 == strcmp(lookup[code].mnemonic, lookup[i].mnemonic)) {
		*len = 1;
		*arg = (BYTE) sconv(&line[1], 0, mode);
		return i;
	    }
	return E_PARAMETER_SYNTAX;
    }

    /* Next check if relative addressing mode was used. */

    if (RELATIVE == lookup[i = code].addr_mode) {
	if ((mode & MODE_HEX) && *line == '$')		/* Skip if not prefix */
	    ++line;
	ival = sconv(line, 0, mode);
	*len = 1;

	if (tstflg) {
	    printf("addressing mode RELATIVE\n");
	    printf("rel disp %d  %04x\n", ival - adr - 2, ival - adr - 2);
	}

	if ((ival -= (adr + 2)) <= 127 && ival >= -128) {
	    *arg = (BYTE) ((ival) & 0xFF);
	    return i;
	}
	return E_LONG_BRANCH;
    }
    /*
     * Now it's time to parse absolute and zeropage addressing modes with
     * their indexed and indirect versions. First check if there are any
     * commas. And count also parenthesis.
     *
     * There are some addressing modes not implemented on each processor,
     * but they are not #ifdef'ed out to keep the parser more consistent.
     * With 6510 family, the additional modes will be filtered out via
     * failing to find any command actually using those addressing modes.
     */

    linep = line;

    while (*linep)
	switch (*linep++) {

	  case ')':
	    ++right_parenthesis;
	    break;
	  case '(':
	    ++left_parenthesis;
	    break;

	  case ',':
	    ++comma_found;
	    while (*linep == ' ')
		linep++;

	    if (*linep) {
		switch (toupper(*linep)) {
		  case 'X':
		    ++x_found;
		    ++linep;
		    break;
		  case 'Y':
		    ++y_found;
		    ++linep;
		    break;
		  case 'Z':
		    ++z_found;
		    ++linep;
#ifdef EXTENDED_CPU
		    break;
		  case 'S':			/* (disp,sp),y */
		    if (!strncmp ("sp),y", linep, 5)) {
			addr_mode = STACK_RELATIVE_Y;
			linep += 5;
			/* To implement other modes, set  comma_found = 0; */
		    }
#endif
		}  /* switch */

		if (*linep && !isspace((int)*linep) && *linep != ')')
		    return E_BAD_INDEX;
	    }
	}

    if (comma_found > 1)	/* There cannot be more than 1 comma found! */
	return E_TOO_MANY_COMMAS;

    if (left_parenthesis > 1)		/* Only one '(' can be found. */
	return E_LEFT_PARENTHESIS;

    if (right_parenthesis > 1)		/* Only one ')' can be found. */
	return E_RIGHT_PARENTHESIS;

    if (left_parenthesis != right_parenthesis)	/* Must be equal. */
	return E_PARENTHESIS;

    if (comma_found && !(x_found || y_found || z_found))
	return E_MISSING_XY;

    if ((x_found + y_found + z_found) > 1)
	return E_MIXED_XY;		/* Only one index register allowed */

    linep = line;
    if (right_parenthesis)
	while (*linep && (*linep++ != '('));

    ival = sconv(linep, 0, mode);

    if (tstflg)
	printf("integer: %d\n", ival);


    if (right_parenthesis && !comma_found)
	addr_mode = ABS_INDIRECT;

    if (ival < 256)
	zero_page = 1;

    if (zero_page) {
	if (!right_parenthesis && !comma_found)
	    addr_mode = ZERO_PAGE;
	if (x_found)
	    addr_mode = (!right_parenthesis ? ZERO_PAGE_X : INDIRECT_X);
	if (y_found)
	    addr_mode = (!right_parenthesis ? ZERO_PAGE_Y : INDIRECT_Y);
#ifdef EXTENDED_CPU
	if (z_found && right_parenthesis)
	    addr_mode = INDIRECT_Z;
	if (right_parenthesis && !comma_found)
	    addr_mode = INDIRECT;
#endif
    }
    else {
	if (!right_parenthesis) {
	    if (!comma_found)
		addr_mode = ABSOLUTE;
	    if (x_found)
		addr_mode = ABSOLUTE_X;
	    if (y_found)
		addr_mode = ABSOLUTE_Y;
	}
#ifdef EXTENDED_CPU
	else if (x_found && right_parenthesis)
	    addr_mode = ABS_INDIR_X;
#endif
    }


    if (tstflg)
	printf("addressing mode 1 : %s  -- code $%02x\n",
	    modename[addr_mode], code);

    if (!addr_mode)
	return E_SYNTAX;

    /*
     * Addressing mode was found and the appropriate opcode is being selected
     */

    for (i = code; i < TOTAL_CODES; ++i)
	if (addr_mode == lookup[i].addr_mode &&
	    0 == strcmp(lookup[code].mnemonic, lookup[i].mnemonic)) {
	    switch (addr_mode) {

	      case ZERO_PAGE:
	      case ZERO_PAGE_X:
	      case ZERO_PAGE_Y:
	      case INDIRECT_X:
	      case INDIRECT_Y:
#ifdef EXTENDED_CPU
	      case INDIRECT_Z:
	      case INDIRECT:
	      case STACK_RELATIVE_Y:
#endif
		*len = 1;
		*arg = (BYTE) ival;
		break;

	      case ABS_INDIRECT:
		if ((ival & 0xff) == 0xff)
		    printf("%04X: Warning: Indirect JMP($%04X)\n", adr, ival);
		/* The famous bug in jmp($indirect) */
		/* fall trough */

#ifdef EXTENDED_CPU
	      case ABS_INDIR_X:
#endif
	      case ABSOLUTE:
	      case ABSOLUTE_X:
	      case ABSOLUTE_Y:
		*len = 2;
		*arg++ = (BYTE) ival & 0xFF;
		*arg = (BYTE) (ival >> 8) & 0xFF;
	    }
	    return i;
	}
    /*
     * If machine code has not zeropage addressing mode, absolute addressing
     * mode is tried instead.
     */

    if (tstflg)
	printf("addressing mode 2 : ABSOLUTE\n");


    if (ZERO_PAGE == addr_mode || ZERO_PAGE_Y == addr_mode) {
	addr_mode += (ABSOLUTE - ZERO_PAGE);
	for (i = code; i <= OP_ABS_MAX; i += OP_ABS_SPC)
	    if (addr_mode == lookup[i].addr_mode &&
		0 == strcmp(lookup[code].mnemonic, lookup[i].mnemonic)) {
		*len = 2;
		*arg++ = (BYTE) ival & 0xFF;
		*arg = (BYTE) (ival >> 8) & 0xFF;
		return i;
	    }
    }
    if(tstflg)
	fprintf(stderr, "addressing mode failed\n");

    return E_SYNTAX;	/* Illegal address mode */
}
