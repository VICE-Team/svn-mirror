/*
 * asm.h - 6510 assembler-related definitions.
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

#ifndef _ASM_H
#define _ASM_H

#include "types.h"

#define NUM_OF_MNEMS	56
#define TOTAL_CODES	256
#define MAXARG		19	/* command + addr + 16 bytes + 1 */

/* ADDRESSING MODES */

#define IMPLIED		0
#define ACCUMULATOR	1
#define IMMEDIATE	2

#define ZERO_PAGE	3
#define ZERO_PAGE_X	4
#define ZERO_PAGE_Y	5

#define ABSOLUTE	6
#define ABSOLUTE_X	7
#define ABSOLUTE_Y	8

#define ABS_INDIRECT	9
#define INDIRECT_X	10
#define INDIRECT_Y	11

#define RELATIVE	12

#define ASS_CODE	13


extern struct lookup_tag {
    const char    *mnemonic;	/* Selfdocumenting? */
    short          addr_mode;
    unsigned char  source;
    unsigned char  destination;
    unsigned char  cycles;
    unsigned char  pbc_fix;	/* Cycle for Page Boundary Crossing */
} lookup[];

extern int clength[];
extern char *modename[];

/* Addressing mode (addr_mode) is used when instruction is diassembled
 * or assembled by diassembler or assembler. This is used i.e.
 * in function char *sprint_opcode() in the file misc.c.
 *
 * MOS6502 addressing modes are #defined in the file "vmachine.h".
 *
 * Mnemonic is character string telling the name of the instruction.
 */

#define M_NONE	0
#define M_AC 	1
#define M_XR	2
#define M_YR	3
#define M_SP	4
#define M_SR	5
#define M_PC	6
#define M_IMM	7
#define M_ZERO	8
#define M_ZERX	9
#define M_ZERY	10
#define M_ABS	11
#define M_ABSX	12
#define M_ABSY	13
#define M_AIND	14
#define M_INDX	15
#define M_INDY	16
#define M_REL	17
#define M_FC	18
#define M_FD	19
#define M_FI	20
#define M_FV	21
#define M_ADDR	22

#define M_TRAP	23

#ifndef NO_UNDOC_CMDS
#define M_ACIM	24	/* Source: AC & IMMED (bus collision) */
#define M_ANXR	25	/* Source: AC & XR (bus collision) */
#define M_AXIM	26	/* Source: (AC | #EE) & XR & IMMED (bus collision) */
#define M_ACNC	27	/* Dest: M_AC and Carry = Negative */
#define M_ACXR	28	/* Dest: M_AC, M_XR */

#define M_SABY	29	/* Source: (ABS_Y & SP) (bus collision) */
#define M_ACXS	30	/* Dest: M_AC, M_XR, M_SP */
#define M_STH0	31	/* Dest: Store (src & Addr_Hi+1) to (Addr +0x100) */
#define M_STH1	32
#define M_STH2	33
#define M_STH3	34

#define M_MAX	34
#else
#define M_ACIM	M_NONE
#define M_ANXR	M_NONE
#define M_AXIM	M_NONE
#define M_ACNC	M_NONE
#define M_ACXR	M_NONE

#define M_SABY	M_NONE
#define M_ACXS	M_NONE
#define M_STH0	M_NONE
#define M_STH1	M_NONE
#define M_STH2	M_NONE
#define M_STH3	M_NONE

#define M_MAX	23
#endif

/* optimise searching a little */
#define OP_MNEM_SPC	0x04

#define OP_IMPL_MIN	0x00
#define OP_IMPL_MAX	0xfa
#define OP_IMPL_SPC	0x02

#define OP_IMM_MIN	0x09
#define OP_IMM_MAX	0xeb
#define OP_IMM_SPC	0x01	/* not used */

#define OP_ACCU_MIN	0x0a
#define OP_ACCU_MAX	0x6a
#define OP_ACCU_SPC	0x20

#define OP_ABS_MIN	0x0c
#define OP_ABS_MAX	0xff
#define OP_ABS_SPC	0x04


/* Symbol definitions */

#define SYMBOL_BYTE	1
#define SYMBOL_WORD	2
#define SYMBOL_FOUND	16
#define SYMBOL_SET	32
#define SYMBOL_VALID	64

#define SYMBOL_MAX_CHARS 8
	/* For portability, labels should be 6 characters or less. */

/* Error messages */

#define ERRORS_TO_STOP	20	/* screenfull on terminal */

#define E_OK			0

 /* Warnings */

#define E_UNDOCUMENTED		(-1)
#define E_SIZE			(-2)
#define E_LARGE_VALUE		(-3)
#define E_LONG_NAME		(-4)
#define E_FORWARD_REF		(-5)

 /* Errors */
#define E_ERROR			(-64)	/* General error */

/* Line Syntax */
#define E_SYNTAX		(E_ERROR)
#define E_PARSE_ERROR		(E_ERROR -1)
#define E_TOO_MANY_ERRORS	(E_ERROR -2)

/* Assembler */
#define E_BAD_IDENTIFIER	(E_ERROR -8)
#define E_BAD_DIRECTIVE		(E_ERROR -9)
#define E_SYMBOL_UNDEFINED	(E_ERROR -10)
#define E_SYMBOL_REDEF		(E_ERROR -11)
#define E_PC_DECREMENT		(E_ERROR -12)

/* Mnemonic */
#define E_BAD_MNEM		(E_ERROR -16)
#define E_LONG_BRANCH		(E_ERROR -17)
#define E_MISSING_OPER		(E_ERROR -18)

/* Operand Syntax */
#define E_PARAMETER_SYNTAX	(E_ERROR -24)
#define E_TOO_MANY_COMMAS	(E_ERROR -25)
#define E_RIGHT_PARENTHESIS	(E_ERROR -26)
#define E_LEFT_PARENTHESIS	(E_ERROR -27)
#define E_PARENTHESIS		(E_ERROR -28)

#define E_MIXED_XY		(E_ERROR -30)
#define E_MISSING_XY		(E_ERROR -31)
#define E_BAD_INDEX		(E_ERROR -32)



 /* Warnings */
#define EM_UNDOCUMENTED		"Undocumented opcode used"
#define EM_SIZE			"Operand length changed"
#define EM_LARGE_VALUE		"Value too large"
#define EM_LONG_NAME		"Symbol name too long"
#define EM_FORWARD_REF		"Forward reference"

 /* Errors */
#define EM_SYNTAX		"Syntax error"
#define EM_PARSE_ERROR		"Parse error - I don't get it"
#define EM_TOO_MANY_ERRORS	"Keep your filthy fingers off here!"

#define EM_BAD_IDENTIFIER	"Identifier error"
#define EM_BAD_DIRECTIVE	"Unrecognised directive"
#define EM_SYMBOL_UNDEFINED	"Undefined symbol"
#define EM_SYMBOL_REDEF 	"Symbol redefined"
#define EM_PC_DECREMENT		"PC decrement"

#define EM_BAD_MNEM		"Illegal mnemonic"
#define EM_LONG_BRANCH		"Branch out of range"
#define EM_MISSING_OPER		"Operand missing"

#define EM_PARAMETER_SYNTAX 	"Parameter syntax error"
#define EM_TOO_MANY_COMMAS  	"Too many commas found"
#define EM_RIGHT_PARENTHESIS	"Too many right parenthesis, 1 is maximum"
#define EM_LEFT_PARENTHESIS 	"Too many left parenthesis, 1 is maximum"
#define EM_PARENTHESIS	    	"Not equally right and left parenthesis"

#define EM_MIXED_XY         	"Too many index registers"
#define EM_MISSING_XY		"Index register missing"
#define EM_BAD_INDEX		"Invalid index register"

/* ------------------------------------------------------------------------- */

extern int ass(ADDRESS addr, int mode);
extern int interpret_instr(char *line, ADDRESS adr, int mode);

#endif  /* _ASM_H */
