/*
 * maincpu.c - Emulation of the main 6510 processor.
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

#include "vice.h"

#include <stdio.h>
#include <sys/time.h>

#include "autoconf.h"
#include "maincpu.h"
#include "types.h"
#include "vmachine.h"
#include "ui.h"
#include "resources.h"
#include "traps.h"
#include "mon.h"
#include "mem.h"
#include "misc.h"
#include "drive.h"
#include "machspec.h"
#include "patchlevel.h"		/* UNSTABLE */
#include "true1541.h"
#include "6510core.h"

#define _MAINCPU_C
#include "interrupt.h"

/* ------------------------------------------------------------------------- */

/* This enables a special hack that can speed up the instruction fetch quite
   a lot, but does not work when a conditional branch instruction jumps from
   ROM to RAM or vice versa.  It still works if one program lies in the I/O
   space, though.  Keeping this defined should be OK for everything, and
   makes things much faster.  */
#define INSTRUCTION_FETCH_HACK

/* ------------------------------------------------------------------------- */

/* The following #defines are useful for debugging and speed tuning.  */

/* If this is #defined, you can set the `traceflg' variable to non-zero to
   trace all the opcodes being executed.  This is mainly useful for
   debugging, and also makes things a bit slower.  */
#undef TRACE

/* Print a message whenever a program attempts to execute instructions fetched
   from the I/O area.  */
#undef IO_AREA_WARNING

/* Run without interpreting opcodes (just fetch them from memory).  */
#undef NO_OPCODES

/* Do not handle CPU alarms, but measure speed instead.  */
#undef EVALUATE_SPEED

/* Use a global variable for Program Counter.  This makes it slower, but also
   makes debugging easier.  This is needed by the VIC-II emulation, so avoid
   #undefining or #defining it in case it is already #defined.  */
#if !defined EXTERN_PC
#undef EXTERN_PC
#endif

/* Force `TRACE' in unstable versions.  */
#if defined UNSTABLE && !defined TRACE
#define TRACE
#endif

/* ------------------------------------------------------------------------- */

struct cpu_int_status maincpu_int_status;

/* Global clock counter.  */
CLOCK clk = 0L;

/* This is flag is set to 1 each time a Read-Modify-Write instructions that
   accesses memory is executed.  We can emulate the RMW bug of the 6510 this
   way.  VERY important notice: Always assign 1 for true, 0 for false!  Some
   functions depend on this to do some optimization.  */
int rmw_flag = 0;

/* Information about the last executed opcode.  This is used to know the
   number of write cycles in the last executed opcode and to delay interrupts
   by one more cycle if necessary, as happens with conditional branch opcodes
   when the branch is taken.  */
int last_opcode_info;

/* Number of write cycles for each 6510 opcode.  */
CLOCK _maincpu_opcode_write_cycles[] = {
	    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
    /* $00 */  3, 0, 0, 2, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 2, 2, /* $00 */
    /* $10 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, /* $10 */
    /* $20 */  2, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, /* $20 */
    /* $30 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, /* $30 */
    /* $40 */  0, 0, 0, 2, 0, 0, 2, 2, 1, 0, 0, 0, 0, 0, 2, 2, /* $40 */
    /* $50 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, /* $50 */
    /* $60 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, /* $60 */
    /* $70 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, /* $70 */
    /* $80 */  0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, /* $80 */
    /* $90 */  0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, /* $90 */
    /* $A0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* $A0 */
    /* $B0 */  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* $B0 */
    /* $C0 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, /* $C0 */
    /* $D0 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2, /* $D0 */
    /* $E0 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 2, 2, /* $E0 */
    /* $F0 */  0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 2, 0, 0, 2, 2  /* $F0 */
	    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
};

/* These are the public CPU variables.  It would be nice to use a cleaner
   interface with a polite struct, but we would have to rework all the
   utilities (monitor, xdebugger, etc.) and macros and it is too big deal for
   now.  */
WORD program_counter;
BYTE accumulator;
BYTE x_register, y_register, stack_pointer;
int zero_flag;
int sign_flag;
int overflow_flag;
int break_flag;
int decimal_flag;
int interrupt_flag;
int carry_flag;

#define EXPORT_REGISTERS()			\
  do {						\
      program_counter = reg_pc;			\
      accumulator = reg_a;			\
      x_register = reg_x;			\
      y_register = reg_y;			\
      stack_pointer = reg_sp;			\
      zero_flag = LOCAL_ZERO();			\
      sign_flag = LOCAL_SIGN();			\
      overflow_flag = LOCAL_OVERFLOW();		\
      break_flag = LOCAL_BREAK();		\
      decimal_flag = LOCAL_DECIMAL();		\
      interrupt_flag = LOCAL_INTERRUPT();	\
      carry_flag = LOCAL_CARRY();		\
  } while (0)

#define IMPORT_REGISTERS()			\
  do {						\
      JUMP (program_counter);			\
      reg_a = accumulator;			\
      reg_x = x_register;			\
      reg_y = y_register;			\
      reg_sp = stack_pointer;			\
      LOCAL_SET_ZERO (zero_flag);		\
      LOCAL_SET_SIGN (sign_flag);		\
      LOCAL_SET_OVERFLOW (overflow_flag);	\
      LOCAL_SET_BREAK (break_flag);		\
      LOCAL_SET_DECIMAL (decimal_flag);		\
      LOCAL_SET_INTERRUPT (interrupt_flag);	\
      LOCAL_SET_CARRY (carry_flag);		\
  } while (0)

#ifdef INSTRUCTION_FETCH_HACK

#  define JUMP(addr)							\
       do {								\
	   reg_pc = (addr);						\
	   bank_base = mem_read_base(reg_pc);				\
       } while (0)

#else  /* !INSTRUCTION_FETCH_HACK */

#  define JUMP(addr)	(reg_pc = (addr))

#endif /* !INSTRUCTION_FETCH_HACK */

/* Trace flag.  Set this to a nonzero value from a debugger to trace the 6510
   instructions being executed.  */
#ifdef TRACE
int traceflg;
#endif

/* ------------------------------------------------------------------------- */

#ifdef EVALUATE_SPEED

#define EVALUATE_INTERVAL	10000000L

inline static void evaluate_speed(unsigned long clk)
{
    static unsigned long old_clk;
    static unsigned long next_clk = EVALUATE_INTERVAL;
    static double old_time;

    if (clk > next_clk) {
	struct timeval tv;
	double current_time;

	gettimeofday (&tv, NULL);
	current_time = (double)tv.tv_sec + ((double)tv.tv_usec) / 1000000.0;

	if (old_clk)
	    printf ("%ld cycles in %f seconds: %f%% speed\n",
		    clk - old_clk, current_time - old_time,
		    100.0 * (((double)(clk - old_clk)
			      / (current_time - old_time)) / 1108405.0));

	old_clk = clk;
	next_clk = old_clk + EVALUATE_INTERVAL;
	old_time = current_time;
    }
}

#endif /* EVALUATE_SPEED */

/* ------------------------------------------------------------------------- */

static void reset(void)
{
    printf("Main CPU: RESET\n");

    serial_reset();

    cpu_int_status_init(&maincpu_int_status,
			NUMOFALRM, NUMOFINT, &last_opcode_info);

    /* Do machine-specific initialization.  */
    machine_reset();

#if 0
#if defined CBM64
#elif defined PET

#endif /* End of machine-specific initialization.  */
#endif

    initialize_memory();

    clk = 6;			/* # of clock cycles needed for RESET.  */
}

/* ------------------------------------------------------------------------- */

#ifdef EXTERN_PC
unsigned int reg_pc;
#endif

void mainloop(ADDRESS start_address)
{
    /* Notice that using a struct for these would make it a lot slower (at
       least, on gcc 2.7.2.x).  */
    BYTE reg_a = 0;
    BYTE reg_x = 0;
    BYTE reg_y = 0;
    BYTE reg_p = 0;
    BYTE reg_sp = 0;
    BYTE flag_n = 0;
    BYTE flag_z = 0;
#ifndef EXTERN_PC
    unsigned int reg_pc;
#endif
#ifdef INSTRUCTION_FETCH_HACK
    BYTE *bank_base;
#endif

    reset();

    if (start_address)
	JUMP(start_address);
    else
	JUMP(LOAD_ADDR(0xfffc));

    printf("Main CPU: starting at $%04X.\n", reg_pc);

    while (1) {

#ifdef EVALUATE_SPEED
	evaluate_speed(clk);
#endif /* !EVALUATE_SPEED */

#ifdef IO_AREA_WARNING
	if (!bank_base)
	    printf ("Executing from I/O area at $%04X: "
		    "$%02X $%02X $%04X at clk %ld\n", reg_pc, p0, p1, p2, clk);
#endif

#ifdef NO_OPCODES

	clk += 4;

#else  /* !NO_OPCODES */

#  define CLK clk
#  define RMW_FLAG rmw_flag
#  define PAGE_ONE (ram + 0x100)
#  define LAST_OPCODE_INFO last_opcode_info
#  define TRACEFLG traceflg

#  define CPU_INT_STATUS	maincpu_int_status

#  define CHECK_PENDING_ALARM() \
     (clk >= next_alarm_clk(&maincpu_int_status))

#  define CHECK_PENDING_INTERRUPT() \
     check_pending_interrupt(&maincpu_int_status)

#  define TRAP(addr) \
     maincpu_int_status.trap_func(addr);

#  define ROM_TRAP_HANDLER() \
     trap_handler()

#  define JAM()								\
     do {								\
        int tmp;							\
									\
        EXPORT_REGISTERS();						\
        tmp = UiJamDialog("   " CPU_STR ": JAM at $%04X   ", reg_pc);	\
        if (tmp == 0) {							\
	    DO_INTERRUPT(IK_RESET);					\
        } else {							\
	    if (tmp == 1) {						\
		caller_space = e_comp_space;				\
	        mon(program_counter);					\
	        IMPORT_REGISTERS();					\
            } else {							\
	        CLK++;							\
            }								\
        }								\
     } while (0)

#  define FORCE_INPUT           mon_force_import(e_comp_space)

#  define ROM_TRAP_ALLOWED()    rom_trap_allowed(reg_pc)
    
#  include "6510core.c"

#endif /* !NO_OPCODES */

    }
}
