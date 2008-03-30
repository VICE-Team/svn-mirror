
/*
 * ../../../src/drive/drivecpu0.c
 * This file is generated from ../../../src/drive/drivecpu-tmpl.c and ../../../src/drive/drivecpu0.def,
 * Do not edit!
 */
/*
 * drivecpu.c - Template file of the 6502 processor in the Commodore 1541
 * floppy disk drive.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Andreas Boose (boose@unixserv.rz.fh-hannover.de)
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

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "types.h"
#include "drive.h"
#include "via.h"
#include "interrupt.h"
#include "ui.h"
#include "resources.h"
#include "viad.h"
#include "ciad.h"
#include "wd1770.h"
#include "6510core.h"
#include "misc.h"
#include "mon.h"
#include "drivecpu.h"
#include "snapshot.h"

/* -------------------------------------------------------------------------- */

/* Define this to enable tracing of 1541 instructions.  Warning: this slows
   it down!  */
#undef TRACE

/* Force `TRACE' in unstable versions.  */
#if 0 && defined UNSTABLE && !defined TRACE
#define TRACE
#endif

#ifdef TRACE
/* Flag: do we trace instructions while they are executed?  */
int drive0_traceflg;
#endif

/* Interrupt/alarm status.  */
struct cpu_int_status drive0_int_status;

/* Value of clk for the last time drive0_cpu_execute() was called.  */
static CLOCK last_clk;

/* Number of cycles in excess we executed last time drive0_cpu_execute()
   was called.  */
static CLOCK last_exc_cycles;

static CLOCK cycle_accum;
static BYTE *bank_base;

/* This is non-zero each time a Read-Modify-Write instructions that accesses
   memory is executed.  We can emulate the RMW bug of the 6502 this way.  */
int drive0_rmw_flag = 0;

/* Information about the last executed opcode.  */
opcode_info_t drive0_last_opcode_info;

/* Public copy of the registers.  */
mos6510_regs_t drive0_cpu_regs;

/* Monitor interface.  */
monitor_interface_t drive0_monitor_interface = {

    /* Pointer to the registers of the CPU.  */
    &drive0_cpu_regs,

    /* Pointer to the alarm/interrupt status.  */
    &drive0_int_status,

    /* Pointer to the machine's clock counter.  */
    &drive_clk[0],
#if 0
    /* Pointer to a function that writes to memory.  */
    drive0_read,

    /* Pointer to a function that reads from memory.  */
    drive0_store,
#endif

    0,
    NULL,
    NULL,
    drive0_bank_read,
    drive0_bank_peek,
    drive0_bank_store,

    /* Pointer to a function to disable/enable watchpoint checking.  */
    drive0_toggle_watchpoints

};

/* ------------------------------------------------------------------------- */

/* This defines the memory access for the 1541 CPU.  */

typedef BYTE REGPARM1 drive0_read_func_t(ADDRESS);
typedef void REGPARM2 drive0_store_func_t(ADDRESS, BYTE);

static drive0_read_func_t *read_func[0x41];
static drive0_store_func_t *store_func[0x41];
static drive0_read_func_t *read_func_watch[0x41];
static drive0_store_func_t *store_func_watch[0x41];
static drive0_read_func_t *read_func_nowatch[0x41];
static drive0_store_func_t *store_func_nowatch[0x41];

#define LOAD(a)		  (read_func[(a) >> 10]((ADDRESS)(a)))
#define LOAD_ZERO(a)	  (drive0_ram[(a) & 0xff])
#define LOAD_ADDR(a)      (LOAD(a) | (LOAD((a) + 1) << 8))
#define LOAD_ZERO_ADDR(a) (LOAD_ZERO(a) | (LOAD_ZERO((a) + 1) << 8))
#define STORE(a, b)	  (store_func[(a) >> 10]((ADDRESS)(a), (BYTE)(b)))
#define STORE_ZERO(a, b)  (drive0_ram[(a) & 0xff] = (b))


static BYTE REGPARM1 drive0_read_ram(ADDRESS address)
{
    /* FIXME: This breaks the 1541 RAM mirror!  */
    return drive0_ram[address & 0x1fff];
}

static void REGPARM2 drive0_store_ram(ADDRESS address, BYTE value)
{
    /* FIXME: This breaks the 1541 RAM mirror!  */
    drive0_ram[address & 0x1fff] = value;
}

static BYTE REGPARM1 drive0_read_rom(ADDRESS address)
{
    return drive[0].rom[address & 0x7fff];
}

static BYTE REGPARM1 drive0_read_free(ADDRESS address)
{
    return address >> 8;
}

static void REGPARM2 drive0_store_free(ADDRESS address, BYTE value)
{
    return;
}

/* This defines the watchpoint memory access for the 1541 CPU.  */

static BYTE REGPARM1 drive0_read_watch(ADDRESS address)
{
    mon_watch_push_load_addr(address, e_disk_space);
    return read_func_nowatch[address>>10](address);
}

static void REGPARM2 drive0_store_watch(ADDRESS address, BYTE value)
{
    mon_watch_push_store_addr(address, e_disk_space);
    store_func_nowatch[address>>10](address, value);
}
/* FIXME: pc can not jump to VIA adress space in 1541 and 1571 emulation.  */
#define JUMP(addr)				\
  do {						\
      reg_pc = (addr);				\
      if (reg_pc < 0x2000) {			\
	  bank_base = drive0_ram;		\
      } else if (reg_pc >= 0x8000)		\
	  bank_base = drive[0].rom - 0x8000;	\
      else					\
	  bank_base = NULL;			\
  } while (0)

#define pagezero	(drive0_ram)
#define pageone		(drive0_ram + 0x100)

/* ------------------------------------------------------------------------- */

/* This is the external interface for memory access.  */

BYTE REGPARM1 drive0_read(ADDRESS address)
{
    return read_func[address >> 10](address);
}

void REGPARM2 drive0_store(ADDRESS address, BYTE value)
{
    store_func[address >> 10](address, value);
}

/* This is the external interface for banked memory access.  */

BYTE drive0_bank_read(int bank, ADDRESS address)
{
    return read_func[address >> 10](address);
}

/* FIXME: use peek in IO area */
BYTE drive0_bank_peek(int bank, ADDRESS address)
{
    return read_func[address >> 10](address);
}

void drive0_bank_store(int bank, ADDRESS address, BYTE value)
{
    store_func[address >> 10](address, value);
}

/* ------------------------------------------------------------------------- */

/* This table is used to approximate the sync between the main and the 1541
   CPUs, since the two clock rates are different.  */
#define MAX_TICKS 0x1000
#ifdef AVOID_STATIC_ARRAYS
static unsigned long *clk_conv_table;
static unsigned long *clk_mod_table;
#else
static unsigned long clk_conv_table[MAX_TICKS + 1];
static unsigned long clk_mod_table[MAX_TICKS + 1];
#endif

void drive0_cpu_set_sync_factor(unsigned int sync_factor)
{
    unsigned long i;

#ifdef AVOID_STATIC_ARRAYS    
    if (!clk_conv_table)
    {
	clk_conv_table = xmalloc(sizeof(*clk_conv_table)*(MAX_TICKS + 1));
	clk_mod_table = xmalloc(sizeof(*clk_mod_table)*(MAX_TICKS + 1));
    }
#endif

    for (i = 0; i <= MAX_TICKS; i++) {
	unsigned long tmp = i * (unsigned long)sync_factor;

	clk_conv_table[i] = tmp / 0x10000;
	clk_mod_table[i] = tmp % 0x10000;
    }
}

/* ------------------------------------------------------------------------- */

static void reset(void)
{
    int preserve_monitor;

    preserve_monitor = drive0_int_status.global_pending_int & IK_MONITOR;

    printf("DRIVE#8: RESET\n");
    cpu_int_status_init(&drive0_int_status, DRIVE_NUMOFINT,
			DRIVE_NUMOFALRM, &drive0_last_opcode_info);
    drive0_int_status.alarm_handler[A_VIA1D0T1] = int_via1d0t1;
    drive0_int_status.alarm_handler[A_VIA1D0T2] = int_via1d0t2;
    drive0_int_status.alarm_handler[A_VIA2D0T1] = int_via2d0t1;
    drive0_int_status.alarm_handler[A_VIA2D0T2] = int_via2d0t2;
    drive0_int_status.alarm_handler[A_CIA1571D0TOD] = int_cia1571d0tod;
    drive0_int_status.alarm_handler[A_CIA1571D0TA] = int_cia1571d0ta;
    drive0_int_status.alarm_handler[A_CIA1571D0TB] = int_cia1571d0tb;
    drive0_int_status.alarm_handler[A_CIA1581D0TOD] = int_cia1581d0tod;
    drive0_int_status.alarm_handler[A_CIA1581D0TA] = int_cia1581d0ta;
    drive0_int_status.alarm_handler[A_CIA1581D0TB] = int_cia1581d0tb;

    drive_clk[0] = 6;
    reset_via1d0();
    reset_via2d0();
    reset_cia1571d0();
    reset_cia1581d0();
    reset_wd1770d0();
    if (preserve_monitor)
	monitor_trap_on(&drive0_int_status);
}

static void drive0_mem_init(int type)
{
    int i;

    for (i = 0; i < 0x41; i++) {
	read_func_watch[i] = drive0_read_watch;
	store_func_watch[i] = drive0_store_watch;
	read_func_nowatch[i] = drive0_read_free;
	store_func_nowatch[i] = drive0_store_free;
    }

    /* Setup firmware ROM.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_2031)
        for (i = 0x30; i < 0x40; i++)
            read_func_nowatch[i] = drive0_read_rom;

    if (type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_1581)
        for (i = 0x20; i < 0x40; i++)
            read_func_nowatch[i] = drive0_read_rom;

    /* Setup drive RAM.  */
    read_func_nowatch[0x0] = read_func_nowatch[0x1] = drive0_read_ram;
    store_func_nowatch[0x0] = store_func_nowatch[0x1] = drive0_store_ram;
    read_func_nowatch[0x40] = drive0_read_ram;
    store_func_nowatch[0x40] = drive0_store_ram;

    if (type == DRIVE_TYPE_1581)
        for (i = 0x0; i < 0x8; i++) {
            read_func_nowatch[i] = drive0_read_ram;
            store_func_nowatch[i] = drive0_store_ram;
        }

    /* Setup 1541 and 1571 VIAs.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1571
        || type == DRIVE_TYPE_2031) {
        read_func_nowatch[0x6] = read_via1d0;
        store_func_nowatch[0x6] = store_via1d0;
        read_func_nowatch[0x7] = read_via2d0;
        store_func_nowatch[0x7] = store_via2d0;
    }

    /* Setup 1571 CIA.  */
    if (type == DRIVE_TYPE_1571) {
        read_func_nowatch[0x10] = read_cia1571d0;
        store_func_nowatch[0x10] = store_cia1571d0;
        /* FIXME: Do not use WD1770 yet.
        read_func_nowatch[0x18] = read_wd1770d0;
        store_func_nowatch[0x18] = store_wd1770d0;
        */
    }

    /* Setup 1581 CIA.  */
    if (type == DRIVE_TYPE_1581) {
        read_func_nowatch[0x10] = read_cia1581d0;
        store_func_nowatch[0x10] = store_cia1581d0;
        read_func_nowatch[0x18] = read_wd1770d0;
        store_func_nowatch[0x18] = store_wd1770d0;
    }

    memcpy(read_func, read_func_nowatch, sizeof(drive0_read_func_t *) * 0x41);
    memcpy(store_func, store_func_nowatch, sizeof(drive0_store_func_t *) * 0x41);
}

void drive0_toggle_watchpoints(int flag)
{
    if (flag) {
        memcpy(read_func, read_func_watch,
               sizeof(drive0_read_func_t *) * 0x41);
        memcpy(store_func, store_func_watch,
               sizeof(drive0_store_func_t *) * 0x41);
    } else {
        memcpy(read_func, read_func_nowatch,
               sizeof(drive0_read_func_t *) * 0x41);
        memcpy(store_func, store_func_nowatch,
               sizeof(drive0_store_func_t *) * 0x41);
    }
}

void drive0_cpu_reset(void)
{
    int preserve_monitor;

    drive_clk[0] = 0;
    last_clk = 0;
    last_exc_cycles = 0;

    preserve_monitor = drive0_int_status.global_pending_int & IK_MONITOR;

    cpu_int_status_init(&drive0_int_status,
			DRIVE_NUMOFALRM, DRIVE_NUMOFINT,
			&drive0_last_opcode_info);

    if (preserve_monitor)
	monitor_trap_on(&drive0_int_status);

    drive0_trigger_reset();
}

void drive0_cpu_init(int type)
{
    drive0_mem_init(type);
    drive0_cpu_reset();
}

inline void drive0_cpu_wake_up(void)
{
    /* FIXME: this value could break some programs, or be way too high for
       others.  Maybe we should put it into a user-definable resource.  */
    if (clk - last_clk > 0xffffff && drive_clk[0] > 934639) {
	printf("1541: skipping cycles.\n");
	last_clk = clk;
    }
}

inline void drive0_cpu_sleep(void)
{
    /* Currently does nothing.  But we might need this hook some day.  */
}

/* Make sure the 1541 clock counters never overflow; return nonzero if they
   have been decremented to prevent overflow.  */
CLOCK drive0_cpu_prevent_clk_overflow(CLOCK sub)
{
    CLOCK our_sub;

    /* First, get in sync with what the main CPU has done.  */
    last_clk -= sub;

    /* Then, check our own clock counters, and subtract from them if they are
       going to overflow.  The `baseval' is 1 because we don't need the
       number of cycles subtracted to be multiple of a particular value
       (unlike the main CPU).  */
    our_sub = prevent_clk_overflow(&drive0_int_status, &drive_clk[0], 1);
    if (our_sub > 0) {
	via1d0_prevent_clk_overflow(sub);
	via2d0_prevent_clk_overflow(sub);
	cia1571d0_prevent_clk_overflow(sub);
	cia1581d0_prevent_clk_overflow(sub);
	wd1770d0_prevent_clk_overflow(sub);
    }

    /* Let the caller know what we have done.  */
    return our_sub;
}

/* -------------------------------------------------------------------------- */

/* Execute up to the current main CPU clock value.  This automatically
   calculates the corresponding number of clock ticks in the drive.  */
void drive0_cpu_execute(void)
{
    static int old_reg_pc;
    CLOCK cycles;

/* #Define the variables for the CPU registers.  In the drive, there is no
   exporting/importing and we just use global variables.  This also makes it
   possible to let the monitor access the CPU status without too much
   headache.   */
#define reg_a   drive0_cpu_regs.reg_a
#define reg_x   drive0_cpu_regs.reg_x
#define reg_y   drive0_cpu_regs.reg_y
#define reg_pc  drive0_cpu_regs.reg_pc
#define reg_sp  drive0_cpu_regs.reg_sp
#define reg_p   drive0_cpu_regs.reg_p
#define flag_z  drive0_cpu_regs.flag_z
#define flag_n  drive0_cpu_regs.flag_n

    drive0_cpu_wake_up();

    if (old_reg_pc != reg_pc) {
	/* Update `bank_base'.  */
	JUMP(reg_pc);
	old_reg_pc = reg_pc;
    }

    cycles = clk - last_clk;

    while (cycles > 0) {
	CLOCK stop_clk;

	if (cycles > MAX_TICKS) {
	    stop_clk = (drive_clk[0] + clk_conv_table[MAX_TICKS]
			- last_exc_cycles);
	    cycle_accum += clk_mod_table[MAX_TICKS];
	    cycles -= MAX_TICKS;
	} else {
	    stop_clk = (drive_clk[0] + clk_conv_table[cycles]
			- last_exc_cycles);
	    cycle_accum += clk_mod_table[cycles];
	    cycles = 0;
	}

	if (cycle_accum >= 0x10000) {
	    cycle_accum -= 0x10000;
	    stop_clk++;
	}

	while (drive_clk[0] < stop_clk) {

#ifdef IO_AREA_WARNING
#warning IO_AREA_WARNING
	    if (!bank_base)
		printf ("Executing from I/O area at $%04X: "
			"$%02X $%02X $%04X at clk %ld\n",
			reg_pc, p0, p1, p2, clk);
#endif

/* Include the 6502/6510 CPU emulation core.  */

#define CLK drive_clk[0]
#define RMW_FLAG drive0_rmw_flag
#define PAGE_ONE (drive0_ram + 0x100)
#define LAST_OPCODE_INFO (drive0_last_opcode_info)
#define TRACEFLG drive0_traceflg

#define CPU_INT_STATUS drive0_int_status

/* FIXME:  We should activate the monitor here.  */
#define JAM()                                                           \
    do {                                                                \
        ui_jam_dialog("   " CPU_STR ": JAM at $%04X   ", reg_pc);       \
        DO_INTERRUPT(IK_RESET);                                         \
    } while (0)

#define ROM_TRAP_ALLOWED() 1

#define ROM_TRAP_HANDLER() \
    drive0_trap_handler()

#define CALLER e_disk_space

#define _drive_set_byte_ready(value) drive[0].byte_ready = value

#define _drive_byte_ready() ((drive[0].byte_ready_active == 0x6)	\
                                ? drive_rotate_disk(0),		\
                                drive[0].byte_ready : 0)		\

#include "6510core.c"

	}

        last_exc_cycles = drive_clk[0] - stop_clk;
    }

    last_clk = clk;
    old_reg_pc = reg_pc;
    drive0_cpu_sleep();
}

/* ------------------------------------------------------------------------- */

static char snap_module_name[] = "MYCPU";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int drive0_cpu_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (0
        || snapshot_module_write_dword(m, (DWORD) drive_clk[0]) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_a) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_x) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_y) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_sp) < 0
        || snapshot_module_write_word(m, (WORD) reg_pc) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_p) < 0
        || snapshot_module_write_dword(m, (DWORD) drive0_last_opcode_info) < 0
        || snapshot_module_write_dword(m, (DWORD) last_clk) < 0
        || snapshot_module_write_dword(m, (DWORD) cycle_accum) < 0
        || snapshot_module_write_dword(m, (DWORD) last_exc_cycles) < 0
        )
        goto fail;

    if (interrupt_write_snapshot(&drive0_int_status, m) < 0)
        goto fail;

    if (drive[0].type == DRIVE_TYPE_1541
        || drive[0].type == DRIVE_TYPE_1571
        || drive[0].type == DRIVE_TYPE_2031) {
        if (snapshot_module_write_byte_array(m, drive0_ram, 0x800) < 0)
            goto fail;
    }

    if (drive[0].type == DRIVE_TYPE_1581) {
        if (snapshot_module_write_byte_array(m, drive0_ram, 0x2000) < 0)
            goto fail;
    }

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

static int read_word_into_unsigned_int(snapshot_module_t *m,
                                       unsigned int *value_return)
{
    WORD b;

    if (snapshot_module_read_word(m, &b) < 0)
        return -1;
    *value_return = (unsigned int) b;
    return 0;
}

int drive0_cpu_read_snapshot_module(snapshot_t *s)
{
    BYTE major, minor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &major, &minor);
    if (m == NULL)
        return -1;

    /* XXX: Assumes `CLOCK' is the same size as a `DWORD'.  */
    if (0
        || snapshot_module_read_dword(m, &drive_clk[0]) < 0
        || snapshot_module_read_byte(m, &reg_a) < 0
        || snapshot_module_read_byte(m, &reg_x) < 0
        || snapshot_module_read_byte(m, &reg_y) < 0
        || snapshot_module_read_byte(m, &reg_sp) < 0
        || read_word_into_unsigned_int(m, &reg_pc) < 0
        || snapshot_module_read_byte(m, &reg_p) < 0
        || snapshot_module_read_dword(m, &drive0_last_opcode_info) < 0
        || snapshot_module_read_dword(m, &last_clk) < 0
        || snapshot_module_read_dword(m, &cycle_accum) < 0
        || snapshot_module_read_dword(m, &last_exc_cycles) < 0
        )
        goto fail;

    if (interrupt_read_snapshot(&drive0_int_status, m) < 0)
        goto fail;

    if (drive[0].type == DRIVE_TYPE_1541
        || drive[0].type == DRIVE_TYPE_1571
        || drive[0].type == DRIVE_TYPE_2031) {
        if (snapshot_module_read_byte_array(m, drive0_ram, 0x800) < 0)
            goto fail;
    }

    if (drive[0].type == DRIVE_TYPE_1581) {
        if (snapshot_module_read_byte_array(m, drive0_ram, 0x2000) < 0)
            goto fail;
    }

    /* Update `*bank_base'.  */
    JUMP(reg_pc);

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

