/*
 * maincpu.h - Emulation of the main 6510 processor.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _MAINCPU_H
#define _MAINCPU_H

#include "types.h"
#include "mem.h"
#include "6510core.h"

/* ------------------------------------------------------------------------- */

/* Information about the last opcode executed by the CPU.  */
extern int last_opcode_info;

/* The lowest 8 bits are the opcode number.  */
#define OPINFO_NUMBER_MASK	       	(last_opcode_info & 0xff)

/* If this is set to 1, the opcode has delayed interrupts by one more cycle
   (this happens with conditional jumps when jump is taken.  */
#define OPINFO_DELAYS_INTERRUPT_MASK	(last_opcode_info & 0x100)

/* The VIC-II emulation needs this.  */
#if defined CBM64
#define EXTERN_PC
extern unsigned int reg_pc;
#endif

/* ------------------------------------------------------------------------- */

/* 6510 Registers.  */
/* FIXME: This must be removed some day.  It is only used to make the old
   code happy.  */

extern ADDRESS program_counter;
extern BYTE accumulator;
extern BYTE x_register, y_register, stack_pointer;
extern int zero_flag;
extern int sign_flag;
extern int overflow_flag;
extern int break_flag;
extern int decimal_flag;
extern int interrupt_flag;
extern int carry_flag;
extern int rmw_flag;
extern CLOCK clk;

#define AC	accumulator
#define XR	x_register
#define YR	y_register
#define SP	stack_pointer
#define PC	program_counter
#define PCH	((PC>>8)&0xff)
#define PCL	(PC&0xff)

#define ZF	zero_flag
#define SF	sign_flag
#define OF	overflow_flag
#define BF	break_flag
#define DF	decimal_flag
#define IF	interrupt_flag
#define CF	carry_flag

/* These define the position of the status flags in the P (`status')
   register.  */
#define S_SIGN		0x80
#define S_OVERFLOW	0x40
#define S_UNUSED	0x20
#define S_BREAK		0x10
#define S_DECIMAL	0x08
#define S_INTERRUPT	0x04
#define S_ZERO		0x02
#define S_CARRY		0x01

/* ------------------------------------------------------------------------- */

#ifdef CBM64

extern CLOCK _maincpu_opcode_write_cycles[];

/* Return the number of write accesses in the last opcode emulated. */
inline static CLOCK maincpu_num_write_cycles(void)
{
    return _maincpu_opcode_write_cycles[OPINFO_NUMBER(last_opcode_info)];
}

#endif

extern void mainloop(ADDRESS start_address);

#endif /* !defined(_MAINCPU_H) */
