/*
 * maincpu.c - Emulation of the 6502 processor in the Commodore 1541 floppy
 * disk drive.
 *
 * Written by
 *   Ettore Perazzoli (ettore@comm2000.it)
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

/* This is the header for emulation of the MOS 6502 CPU in the 1541 drive. */

#ifndef _1541CPU_H
#define _1541CPU_H

/* It would be nice to pack these into a struct, but they were not in the early
   versions of the main CPU so... */
extern int true1541_rmw_flag;
extern WORD true1541_program_counter;
extern BYTE true1541_accumulator;
extern BYTE true1541_x_register;
extern BYTE true1541_y_register;
extern BYTE true1541_stack_pointer;
extern int true1541_zero_flag;
extern int true1541_sign_flag;
extern int true1541_overflow_flag;
extern int true1541_break_flag;
extern int true1541_decimal_flag;
extern int true1541_interrupt_flag;
extern int true1541_carry_flag;

extern CLOCK true1541_clk;
extern int true1541_traceflg;
extern int true1541_cpu_running;

extern void true1541_cpu_init(void);
extern void true1541_cpu_reset(void);
extern void true1541_cpu_sleep(void);
extern void true1541_cpu_wake_up(void);
extern int true1541_cpu_prevent_clk_overflow(void);

#endif
