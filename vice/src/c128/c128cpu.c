/*
 * c128cpu.c - Emulation of the main 6510 processor in the C128.
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

/* Because of the different memory access scheme, the C128 needs a special
   version of the CPU: we implement it here, keeping compatibility with the
   standard version.  Notice that this is not very well optimized, and surely
   looks like a big hack.  Well, that's fine with me for now.  */

#include "vice.h"

#include <stdio.h>
#include <sys/time.h>

#define _MAINCPU_C
#define _C128CPU_C

#include "maincpu.h"
#include "types.h"
#include "machine.h"
#include "vmachine.h"
#include "ui.h"
#include "resources.h"
#include "traps.h"
#include "mon.h"
#include "c128mem.h"
#include "misc.h"
#include "vdrive.h"
#include "6510core.h"

#include "interrupt.h"

/* ------------------------------------------------------------------------- */

/* This enables a special hack that can speed up the instruction fetch quite
   a lot, but does not work when a conditional branch instruction jumps from
   ROM to RAM or vice versa.  It does not work for the C128, so we disable
   it.  */
#undef INSTRUCTION_FETCH_HACK

/* ------------------------------------------------------------------------- */

/* The following #defines are useful for debugging and speed tuning.  */

/* If this is #defined, you can set the `traceflg' variable to non-zero to
   trace all the opcodes being executed.  This is mainly useful for
   debugging, and also makes things a bit slower.  */
/* #define TRACE */

/* Force `TRACE' in unstable versions.  */
#if 0 && defined UNSTABLE && !defined TRACE
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
DWORD last_opcode_info;

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

/* Public copy of the CPU registers.  As putting the registers into the
   function makes it faster, you have to generate a `TRAP' interrupt to have
   the values copied into this struct.  */
mos6510_regs_t maincpu_regs;

/* Trace flag.  Set this to a nonzero value from a debugger to trace the 6510
   instructions being executed.  */
#ifdef TRACE
int traceflg;
#endif

/* ------------------------------------------------------------------------- */

/* Interface to the monitor.  */
monitor_interface_t maincpu_monitor_interface = {

    /* Pointer to the registers of the CPU.  */
    &maincpu_regs,

    /* Pointer to the alarm/interrupt status.  */
    &maincpu_int_status,

    /* Pointer to the machine's clock counter.  */
    &clk,

#if 0
    /* Pointer to a function that writes to memory.  */
    mem_read,

    /* Pointer to a function that reads from memory.  */
    mem_store,
#endif

    0,
    mem_bank_list,
    mem_bank_from_name,
    mem_bank_read,
    mem_bank_peek,
    mem_bank_write,

    /* Pointer to a function to disable/enable watchpoint checking.  */
    mem_toggle_watchpoints
};

/* ------------------------------------------------------------------------- */

static void reset(void)
{
    printf("Main CPU: RESET\n");

    serial_reset();

    cpu_int_status_init(&maincpu_int_status,
			NUMOFALRM, NUMOFINT, &last_opcode_info);

    /* Do machine-specific initialization.  */
    machine_reset();

    clk = 6;			/* # of clock cycles needed for RESET.  */

    initialize_memory();
}

/* ------------------------------------------------------------------------- */

unsigned int reg_pc;
#define JUMP(addr)	(reg_pc = (addr))

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

    reset();

    if (start_address)
	JUMP(start_address);
    else
	JUMP(LOAD_ADDR(0xfffc));

    printf("Main CPU: starting at $%04X.\n", reg_pc);

    while (1) {

#  define CLK clk
#  define RMW_FLAG rmw_flag
#  define PAGE_ONE page_one
#  define LAST_OPCODE_INFO last_opcode_info
#  define TRACEFLG traceflg

#  define CPU_INT_STATUS maincpu_int_status

#  define CHECK_PENDING_ALARM() \
     (clk >= next_alarm_clk(&maincpu_int_status))

#  define CHECK_PENDING_INTERRUPT() \
     check_pending_interrupt(&maincpu_int_status)

#  define TRAP(addr) \
     maincpu_int_status.trap_func(addr);

#  define ROM_TRAP_HANDLER() \
     traps_handler()

#  define JAM()                                                         \
     do {                                                               \
        int tmp;                                                        \
                                                                        \
        EXPORT_REGISTERS();                                             \
        tmp = ui_jam_dialog("   " CPU_STR ": JAM at $%04X   ", reg_pc); \
        switch (tmp) {                                                  \
          case UI_JAM_RESET:                                            \
            DO_INTERRUPT(IK_RESET);                                     \
            break;                                                      \
          case UI_JAM_HARD_RESET:                                       \
            mem_powerup();                                              \
            DO_INTERRUPT(IK_RESET);                                     \
            break;                                                      \
          case UI_JAM_MONITOR:                                          \
            caller_space = e_comp_space;                                \
            mon(reg_pc);                                                \
            IMPORT_REGISTERS();                                         \
            break;                                                      \
          default:                                                      \
            CLK++;                                                      \
        }                                                               \
     } while (0)

#  define CALLER		e_comp_space

#  define ROM_TRAP_ALLOWED()    mem_rom_trap_allowed(reg_pc)

#  define GLOBAL_REGS           maincpu_regs

/* Define a "special" opcode fetch method.  We trust the code in `6510core.c'
   to evaluate `p0', `p1' and `p2' at most once per every emulated opcode.  */
#  define FETCH_OPCODE(x)
#  define p0                    LOAD(reg_pc)
#  define p1                    LOAD(reg_pc + 1)
#  define p2                    LOAD_ADDR(reg_pc + 1)

/* FIXME: This might cause complaints about unused variables...  Well, who
   cares?  */
#  define opcode_t      int

#  include "6510core.c"

    }
}
