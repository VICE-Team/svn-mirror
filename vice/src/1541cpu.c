/*
 * 1541cpu.c - Emulation of the 6502 processor in the Commodore 1541 floppy
 * disk drive.
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

#define __1541__

#include <stdio.h>
#include <sys/time.h>

#include "patchlevel.h"		/* UNSTABLE */
#include "autoconf.h"
#include "types.h"
#include "true1541.h"
#include "via.h"
#include "interrupt.h"
#include "ui.h"
#include "resources.h"
#include "viad.h"
#include "6510core.h"
#include "misc.h"
#include "mon.h"

/* -------------------------------------------------------------------------- */

/* Define this to enable tracing of 1541 instructions.  Warning: this slows
   it down!  */
#undef TRACE

/* Force `TRACE' in unstable versions.  */
#if defined UNSTABLE && !defined TRACE
#define TRACE
#endif

#ifdef TRACE
/* Flag: do we trace instructions while they are executed?  */
int true1541_traceflg;
#endif

/* Global clock counter.  */
CLOCK true1541_clk = 0L;

/* Interrupt/alarm status.  */
struct cpu_int_status true1541_int_status;

/* Value of clk for the last time true1541_cpu_execute() was called.  */
static CLOCK last_clk;

/* Number of cycles in excess we executed last time true1541_execute() was
   called.  */
static CLOCK last_exc_cycles;

/* This is non-zero each time a Read-Modify-Write instructions that accesses
   memory is executed.  We can emulate the RMW bug of the 6502 this way.  */
int true1541_rmw_flag = 0;

/* Information about the last executed opcode.  */
opcode_info_t true1541_last_opcode_info;

/* Public copy of the registers.  */
WORD true1541_program_counter;
BYTE true1541_accumulator;
BYTE true1541_x_register;
BYTE true1541_y_register;
BYTE true1541_stack_pointer;
int true1541_zero_flag;
int true1541_sign_flag;
int true1541_overflow_flag;
int true1541_break_flag;
int true1541_decimal_flag;
int true1541_interrupt_flag;
int true1541_carry_flag;

#define EXPORT_REGISTERS()				\
  do {							\
      true1541_program_counter = reg_pc;		\
      true1541_accumulator = reg_a;			\
      true1541_x_register = reg_x;			\
      true1541_y_register = reg_y;			\
      true1541_stack_pointer = reg_sp;			\
      true1541_zero_flag = LOCAL_ZERO();		\
      true1541_sign_flag = LOCAL_SIGN();		\
      true1541_overflow_flag = LOCAL_OVERFLOW();	\
      true1541_break_flag = LOCAL_BREAK();		\
      true1541_decimal_flag = LOCAL_DECIMAL();		\
      true1541_interrupt_flag = LOCAL_INTERRUPT();	\
      true1541_carry_flag = LOCAL_CARRY();		\
  } while (0)

#define IMPORT_REGISTERS()				\
  do {							\
      JUMP (true1541_program_counter);			\
      reg_a = true1541_accumulator;			\
      reg_x = true1541_x_register;			\
      reg_y = true1541_y_register;			\
      reg_sp = true1541_stack_pointer;			\
      LOCAL_SET_ZERO(true1541_zero_flag);		\
      LOCAL_SET_SIGN(true1541_sign_flag);		\
      LOCAL_SET_OVERFLOW(true1541_overflow_flag);	\
      LOCAL_SET_BREAK(true1541_break_flag);		\
      LOCAL_SET_DECIMAL(true1541_decimal_flag);		\
      LOCAL_SET_INTERRUPT(true1541_interrupt_flag);	\
      LOCAL_SET_CARRY(true1541_carry_flag);		\
  } while (0)

/* -------------------------------------------------------------------------- */

/* This defines the memory access for the 1541 CPU.  */

static BYTE REGPARM1 read_ram(ADDRESS address)
{
    return true1541_ram[address & 0x7ff];
}

static void REGPARM2 store_ram(ADDRESS address, BYTE value)
{
    true1541_ram[address & 0x7ff] = value;
}

static BYTE REGPARM1 read_rom(ADDRESS address)
{
    return true1541_rom[address & 0x3fff];
}

static BYTE REGPARM1 read_free(ADDRESS address)
{
    return address >> 8;
}

static void REGPARM2 store_free(ADDRESS address, BYTE value)
{
    return;
}

typedef BYTE REGPARM1 true1541_read_func_t(ADDRESS);
typedef void REGPARM2 true1541_store_func_t(ADDRESS, BYTE);

true1541_read_func_t *read_func[0x41];
true1541_store_func_t *store_func[0x41];

#define LOAD(a)		  (read_func[(a) >> 10](a))

#define LOAD_ZERO(a)	  (true1541_ram[(a) & 0xff])

#define LOAD_ADDR(a)      (LOAD(a) | (LOAD((a) + 1) << 8))

#define LOAD_ZERO_ADDR(a) (LOAD_ZERO(a) | (LOAD_ZERO((a) + 1) << 8))

#define STORE(a, b)	  (store_func[(a) >> 10]((a), (b)))

#define STORE_ZERO(a, b)  (true1541_ram[(a) & 0xff] = (b))


#define JUMP(addr)				\
  do {						\
      reg_pc = (addr);				\
      if (reg_pc < 0x1800) {			\
	  bank_base = true1541_ram;		\
	  if (reg_pc > 0x7ff)			\
	      bank_base -= 0x7ff;		\
      } else if (reg_pc >= 0xc000)		\
	  bank_base = true1541_rom - 0xc000;	\
      else					\
	  bank_base = NULL;			\
  } while (0)

#define pagezero	(true1541_ram)
#define pageone		(true1541_ram + 0x100)

/* -------------------------------------------------------------------------- */

/* This table is used to approximate the sync between the main and the 1541
   CPUs, since the two clock rates are different.  */
#define MAX_TICKS 0x1000
static unsigned long clk_conv_table[MAX_TICKS + 1];
static unsigned long clk_mod_table[MAX_TICKS + 1];

void true1541_set_sync_factor(unsigned int sync_factor)
{
    unsigned long i;

    app_resources.true1541SyncFactor = sync_factor;

    for (i = 0; i <= MAX_TICKS; i++) {
	unsigned long tmp = i * (unsigned long)sync_factor;

	clk_conv_table[i] = tmp / 0x10000;
	clk_mod_table[i] = tmp % 0x10000;
    }
}

/* -------------------------------------------------------------------------- */

static void reset(void)
{
    printf("1541 CPU: RESET\n");
    cpu_int_status_init(&true1541_int_status, TRUE1541_NUMOFALRM,
			TRUE1541_NUMOFINT, &true1541_last_opcode_info);
    true1541_int_status.alarm_handler[A_VIAD1T1] = int_viaD1t1;
    true1541_int_status.alarm_handler[A_VIAD1T2] = int_viaD1t2;
    true1541_int_status.alarm_handler[A_VIAD2T1] = int_viaD2t1;
    true1541_int_status.alarm_handler[A_VIAD2T2] = int_viaD2t2;
    true1541_clk = 6;
    reset_viaD1();
    reset_viaD2();
}

static void mem_init(void)
{
    int i;

    for (i = 0; i < 0x41; i++) {
	read_func[i] = read_free;
	store_func[i] = store_free;
    }
    for (i = 0x30; i < 0x40; i++) {
	read_func[i] = read_rom;
    }
    read_func[0x0] = read_func[0x1] = read_func[0x40] = read_ram;
    store_func[0x0] = store_func[0x1] = store_func[0x40] = store_ram;
    read_func[0x6] = read_viaD1;
    store_func[0x6] = store_viaD1;
    read_func[0x7] = read_viaD2;
    store_func[0x7] = store_viaD2;
}

void true1541_cpu_reset(void)
{
    true1541_clk = 0;
    last_clk = clk;
    last_exc_cycles = 0;
    cpu_int_status_init(&true1541_int_status,
			TRUE1541_NUMOFALRM, TRUE1541_NUMOFINT,
			&true1541_last_opcode_info);
    true1541_trigger_reset();
}

void true1541_cpu_init(void)
{
    mem_init();
    true1541_cpu_reset();
    true1541_set_sync_factor(app_resources.true1541SyncFactor);
}

inline void true1541_cpu_wake_up(void)
{
    /* FIXME: this value could break some programs, or be way too high for
       others.  Maybe we should put it into the app_resources.  */
    if (clk - last_clk > 0xffffff && true1541_clk > 934639) {
	printf("1541: skipping cycles.\n");
	last_clk = clk;
    }
}

inline void true1541_cpu_sleep(void)
{
    /* Currently does nothing.  But we might need this hook some day.  */
}

/* Make sure the 1541 clock counters never overflow; return nonzero if they
   have been decremented to prevent overflow.  */
int true1541_cpu_prevent_clk_overflow(void)
{
    int retval = 0;

    /* If this happens, the CPU has prevented the clock overflow, so we have
       to get in sync with it.  */
    if (last_clk > clk) {
	last_clk -= PREVENT_CLK_OVERFLOW_SUB;
	retval = 1;
    }

    /* Now check our own clock counters, and subtract from them if they are
       going to overflow.  */
    if (prevent_clk_overflow(&true1541_int_status, &true1541_clk)) {
	viaD1_prevent_clk_overflow();
	viaD2_prevent_clk_overflow();
	retval = 1;
    }

    return retval;
}

/* -------------------------------------------------------------------------- */

/* Execute up to the current main CPU clock value.  This automatically
   calculates the corresponding number of clock ticks in the drive.  */
void true1541_cpu_execute(void)
{
    static BYTE reg_a, reg_x, reg_y, reg_sp, reg_p, flag_n, flag_z;
    static BYTE *bank_base;
    static unsigned int reg_pc;
    static CLOCK cycle_accum;
    CLOCK cycles;

    true1541_cpu_wake_up();

    cycles = clk - last_clk;

    while (cycles > 0) {
	CLOCK stop_clk;

	if (cycles > MAX_TICKS) {
	    stop_clk = (true1541_clk + clk_conv_table[MAX_TICKS]
			- last_exc_cycles);
	    cycle_accum += clk_mod_table[MAX_TICKS];
	    cycles -= MAX_TICKS;
	} else {
	    stop_clk = (true1541_clk + clk_conv_table[cycles]
			- last_exc_cycles);
	    cycle_accum += clk_mod_table[cycles];
	    cycles = 0;
	}

	if (cycle_accum >= 0x10000) {
	    cycle_accum -= 0x10000;
	    stop_clk++;
	}

	while (true1541_clk < stop_clk) {

#ifdef IO_AREA_WARNING
#warning IO_AREA_WARNING
	    if (!bank_base)
		printf ("Executing from I/O area at $%04X: "
			"$%02X $%02X $%04X at clk %ld\n",
			reg_pc, p0, p1, p2, clk);
#endif

/* Include the 6502/6510 CPU emulation core.  */

#define CLK true1541_clk
#define RMW_FLAG true1541_rmw_flag
#define PAGE_ONE (true1541_ram + 0x100)
#define LAST_OPCODE_INFO (true1541_last_opcode_info)
#define TRACEFLG true1541_traceflg

#define CPU_INT_STATUS true1541_int_status

#define JAM()                                                     \
    do {                                                          \
        UiError("   " CPU_STR ": JAM at $%04X   ", reg_pc);       \
        DO_INTERRUPT(IK_RESET);                                   \
    } while (0)

#define ROM_TRAP_ALLOWED() 1

#define ROM_TRAP_HANDLER() \
    true1541_trap_handler()

#include "6510core.c"

	}

        last_exc_cycles = true1541_clk - stop_clk;
    }

    last_clk = clk;
    true1541_cpu_sleep();
}
