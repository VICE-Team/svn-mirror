/*
 * drivecpucore.c - Template file of the 6502 processor in the Commodore 1541,
 * 1541-II, 1571, 1581, 2031 and 1001 floppy disk drive.
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

/* Interrupt/alarm status.  */
struct cpu_int_status mydrive_int_status;
alarm_context_t mydrive_alarm_context;

/* Clk guard.  */
clk_guard_t mydrive_clk_guard;

/* ------------------------------------------------------------------------ */

/* Drive RAM.  */
static BYTE drive_ram[DRIVE_RAM_SIZE];

/* Value of clk for the last time mydrive_cpu_execute() was called.  */
static CLOCK last_clk;

/* Number of cycles in excess we executed last time mydrive_cpu_execute()
   was called.  */
static CLOCK last_exc_cycles;

static CLOCK cycle_accum;
static BYTE *bank_base;
static int bank_limit = -1;

/* Drive CPU JAM handling.  */
static void drive_jam(void);

/* This is non-zero each time a Read-Modify-Write instructions that accesses
   memory is executed.  We can emulate the RMW bug of the 6502 this way.  */
int mydrive_rmw_flag = 0;

/* Information about the last executed opcode.  */
static opcode_info_t drive_last_opcode_info;

/* Public copy of the registers.  */
static mos6510_regs_t drive_cpu_regs;

/* ------------------------------------------------------------------------- */

/* This defines the memory access for the drive CPU.  */

typedef BYTE REGPARM1 mydrive_read_func_t(ADDRESS);
typedef void REGPARM2 mydrive_store_func_t(ADDRESS, BYTE);

static mydrive_read_func_t *read_func[0x101];
static mydrive_store_func_t *store_func[0x101];
static mydrive_read_func_t *read_func_watch[0x101];
static mydrive_store_func_t *store_func_watch[0x101];
static mydrive_read_func_t *read_func_nowatch[0x101];
static mydrive_store_func_t *store_func_nowatch[0x101];

#define LOAD(a)		  (read_func[(a) >> 8]((ADDRESS)(a)))
#define LOAD_ZERO(a)	  (drive_ram[(a) & 0xff])
#define LOAD_ADDR(a)      (LOAD(a) | (LOAD((a) + 1) << 8))
#define LOAD_ZERO_ADDR(a) (LOAD_ZERO(a) | (LOAD_ZERO((a) + 1) << 8))
#define STORE(a, b)	  (store_func[(a) >> 8]((ADDRESS)(a), (BYTE)(b)))
#define STORE_ZERO(a, b)  (drive_ram[(a) & 0xff] = (b))

static BYTE REGPARM1 mydrive_read_1001_io(ADDRESS address)
{
    if (address & 0x80) {
	return read_myriot2(address);
    } 
    return read_myriot1(address);
}

static void REGPARM2 mydrive_store_1001_io(ADDRESS address, BYTE byte)
{
    if (address & 0x80) {
	store_myriot2(address, byte);
    } else {
        store_myriot1(address, byte);
    }
}

static BYTE REGPARM1 mydrive_read_1001zero_ram(ADDRESS address)
{
    return drive_ram[address & 0xff];
}

static void REGPARM2 mydrive_store_1001zero_ram(ADDRESS address, BYTE byte)
{
    drive_ram[address & 0xff] = byte;
}

static BYTE REGPARM1 mydrive_read_1001buffer_ram(ADDRESS address)
{
    return drive_ram[(((address >> 2) & 0x1c00) | (address & 0x03ff)) - 0x300];
}

static void REGPARM2 mydrive_store_1001buffer_ram(ADDRESS address, BYTE byte)
{
    drive_ram[(((address >> 2) & 0x1c00) | (address & 0x03ff)) - 0x300] = byte;
}

static BYTE REGPARM1 mydrive_read_ram(ADDRESS address)
{
    /* FIXME: This breaks the 1541 RAM mirror!  */
    return drive_ram[address & 0x1fff];
}

static void REGPARM2 mydrive_store_ram(ADDRESS address, BYTE value)
{
    /* FIXME: This breaks the 1541 RAM mirror!  */
    drive_ram[address & 0x1fff] = value;
}

static BYTE REGPARM1 mydrive_read_rom(ADDRESS address)
{
    return drive[mynumber].rom[address & 0x7fff];
}

static BYTE REGPARM1 mydrive_read_free(ADDRESS address)
{
    return address >> 8;
}

static void REGPARM2 mydrive_store_free(ADDRESS address, BYTE value)
{
    return;
}

/* This defines the watchpoint memory access for the drive CPU.  */

static BYTE REGPARM1 mydrive_read_watch(ADDRESS address)
{
    mon_watch_push_load_addr(address, mymonspace);
    return read_func_nowatch[address>>8](address);
}

static void REGPARM2 mydrive_store_watch(ADDRESS address, BYTE value)
{
    mon_watch_push_store_addr(address, mymonspace);
    store_func_nowatch[address>>8](address, value);
}

/* FIXME: pc can not jump to VIA adress space in 1541 and 1571 emulation.  */
/* FIXME: SFD1001 does not use bank_base at all due to messy memory mapping.
   We should use tables like in maincpu instead (AF) */
#define JUMP(addr)                                    \
    do {                                              \
        reg_pc = (addr);                              \
	if (drive[mynumber].type == 1001) {	      \
	    bank_base = NULL;			      \
	    bank_limit = -1; 			      \
	} else if (reg_pc < 0x2000) {                 \
            bank_base = drive_ram;                    \
            bank_limit = 0x07fd;                      \
        } else if (reg_pc >= 0x8000) {                \
            bank_base = drive[mynumber].rom - 0x8000; \
            bank_limit = 0xfffd;                      \
        } else {                                      \
            bank_base = NULL;                         \
            bank_limit = -1;                          \
        }                                             \
    } while (0)

#define pagezero	(drive_ram)
static BYTE *pageone = NULL;

/* ------------------------------------------------------------------------- */

/* This is the external interface for memory access.  */

BYTE REGPARM1 mydrive_read(ADDRESS address)
{
    return read_func[address >> 8](address);
}

void REGPARM2 mydrive_store(ADDRESS address, BYTE value)
{
    store_func[address >> 8](address, value);
}

/* This is the external interface for banked memory access.  */

static BYTE drive_bank_read(int bank, ADDRESS address)
{
    return read_func[address >> 8](address);
}

/* FIXME: use peek in IO area */
static BYTE drive_bank_peek(int bank, ADDRESS address)
{
    return read_func[address >> 8](address);
}

static void drive_bank_store(int bank, ADDRESS address, BYTE value)
{
    store_func[address >> 8](address, value);
}

/* Monitor interface.  */
monitor_interface_t mydrive_monitor_interface = {

    /* Pointer to the registers of the CPU.  */
    &drive_cpu_regs,

    /* Pointer to the alarm/interrupt status.  */
    &mydrive_int_status,

    /* Pointer to the machine's clock counter.  */
    &drive_clk[mynumber],
#if 0
    /* Pointer to a function that writes to memory.  */
    mydrive_read,

    /* Pointer to a function that reads from memory.  */
    mydrive_store,
#endif

    0,
    NULL,
    NULL,
    drive_bank_read,
    drive_bank_peek,
    drive_bank_store,

    /* Pointer to a function to disable/enable watchpoint checking.  */
    mydrive_toggle_watchpoints

};

/* ------------------------------------------------------------------------- */


/* This table is used to approximate the sync between the main and the
   drive CPU, since the two clock rates are different.  */
#define MAX_TICKS 0x1000
#ifdef AVOID_STATIC_ARRAYS
static unsigned long *clk_conv_table;
static unsigned long *clk_mod_table;
#else
static unsigned long clk_conv_table[MAX_TICKS + 1];
static unsigned long clk_mod_table[MAX_TICKS + 1];
#endif

void mydrive_cpu_set_sync_factor(unsigned int sync_factor)
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

    preserve_monitor = mydrive_int_status.global_pending_int & IK_MONITOR;

    log_message(drive[mynumber].log, "RESET.");
    cpu_int_status_init(&mydrive_int_status, DRIVE_NUMOFINT,
                        &drive_last_opcode_info);

    drive_clk[mynumber] = 6;
    myvia1_reset();
    myvia2_reset();
    mycia1571_reset();
    mycia1581_reset();
    mywd1770_reset();
    myriot1_reset();
    myriot2_reset();
    fdc_reset(mynumber, drive[mynumber].type == DRIVE_TYPE_1001);

    if (preserve_monitor)
	monitor_trap_on(&mydrive_int_status);
}

#ifdef _MSC_VER
#pragma optimize("",off);
#endif

void mydrive_mem_init(int type)
{
    int i;

    for (i = 0; i < 0x101; i++) {
	read_func_watch[i] = mydrive_read_watch;
	store_func_watch[i] = mydrive_store_watch;
	read_func_nowatch[i] = mydrive_read_free;
	store_func_nowatch[i] = mydrive_store_free;
    }

    /* FIXME: ROM mirrors! */
    /* Setup firmware ROM.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II ||
        type == DRIVE_TYPE_2031 || type == DRIVE_TYPE_1001)
        for (i = 0xC0; i < 0x100; i++)
            read_func_nowatch[i] = mydrive_read_rom;

    if (type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_1581)
        for (i = 0x80; i < 0x100; i++)
            read_func_nowatch[i] = mydrive_read_rom;

    if (type != DRIVE_TYPE_1001) {
	pageone = drive_ram + 0x100;

        /* Setup drive RAM.  */
	for (i = 0x00; i < 0x08; i++) {
            read_func_nowatch[i] = mydrive_read_ram;
            store_func_nowatch[i] = mydrive_store_ram;
	}
        if (type == DRIVE_TYPE_1581)
            for (i = 0x08; i < 0x20; i++) {
                read_func_nowatch[i] = mydrive_read_ram;
                store_func_nowatch[i] = mydrive_store_ram;
            }
    } else {
	/* The 1001/8050/8250 have 256 byte at $00xx, mirrored at 
	   $01xx, $04xx, $05xx, $08xx, $09xx, $0cxx, $0dxx.
	   (From the 2 RIOT's 128 byte RAM each. The RIOT's I/O fill
	   the gaps, x00-7f the first and x80-ff the second, at 
	   $02xx, $03xx, $06xx, $07xx, $0axx, $0bxx, $0exx, $0fxx). 
	   Then we have 4k of buffers, at $1000-13ff, 2000-23ff, 3000-33ff
	   and 4000-43ff, each mirrored at $x400-$x7fff, $x800-$xbff, 
	   and $xc00-$xfff. 
	
	   Here we set zeropage, stack and buffer RAM as well as I/O */

	pageone = drive_ram;

	for (i = 0; i <= 0x10; i += 4) {
	   read_func_nowatch[i] = mydrive_read_1001zero_ram;
	   store_func_nowatch[i] = mydrive_store_1001zero_ram;
	   read_func_nowatch[i + 1] = mydrive_read_1001zero_ram;
	   store_func_nowatch[i + 1] = mydrive_store_1001zero_ram;
	   read_func_nowatch[i + 2] = mydrive_read_1001_io;
	   store_func_nowatch[i + 2] = mydrive_store_1001_io;
	   read_func_nowatch[i + 3] = mydrive_read_1001_io;
	   store_func_nowatch[i + 3] = mydrive_store_1001_io;
	}
	for (i = 0x10; i <= 0x50; i ++) {
	   read_func_nowatch[i] = mydrive_read_1001buffer_ram;
	   store_func_nowatch[i] = mydrive_store_1001buffer_ram;
	}
    }

    /* Setup 1541, 1541-II and 1571 VIAs.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_2031) {
	for (i = 0x18; i < 0x1C; i++) {
            read_func_nowatch[i] = read_myvia1;
            store_func_nowatch[i] = store_myvia1;
	}
	for (i = 0x1C; i < 0x20; i++) {
            read_func_nowatch[i] = read_myvia2;
            store_func_nowatch[i] = store_myvia2;
	}
    }

    /* Setup 1571 CIA.  */
    if (type == DRIVE_TYPE_1571) {
	for (i = 0x40; i < 0x44; i++) {
            read_func_nowatch[i] = read_mycia1571;
            store_func_nowatch[i] = store_mycia1571;
	}
	for (i = 0x20; i < 0x24; i++) {
            read_func_nowatch[i] = read_mywd1770;
            store_func_nowatch[i] = store_mywd1770;
	}
    }

    /* Setup 1581 CIA.  */
    if (type == DRIVE_TYPE_1581) {
	for (i = 0x40; i < 0x44; i++) {
            read_func_nowatch[i] = read_mycia1581;
            store_func_nowatch[i] = store_mycia1581;
	}
	for (i = 0x60; i < 0x64; i++) {
            read_func_nowatch[i] = read_mywd1770;
            store_func_nowatch[i] = store_mywd1770;
	}
    }

    read_func_nowatch[0x100] = read_func_nowatch[0];
    store_func_nowatch[0x100] = store_func_nowatch[0];

    memcpy(read_func, read_func_nowatch, sizeof(mydrive_read_func_t *) * 0x101);
    memcpy(store_func, store_func_nowatch, sizeof(mydrive_store_func_t *) * 0x101);
}

#ifdef _MSC_VER
#pragma optimize("",on);
#endif

void mydrive_toggle_watchpoints(int flag)
{
    if (flag) {
        memcpy(read_func, read_func_watch,
               sizeof(mydrive_read_func_t *) * 0x101);
        memcpy(store_func, store_func_watch,
               sizeof(mydrive_store_func_t *) * 0x101);
    } else {
        memcpy(read_func, read_func_nowatch,
               sizeof(mydrive_read_func_t *) * 0x101);
        memcpy(store_func, store_func_nowatch,
               sizeof(mydrive_store_func_t *) * 0x101);
    }
}

void mydrive_cpu_reset_clk(void)
{
    last_clk = clk;
    last_exc_cycles = 0;
}

void mydrive_cpu_reset(void)
{
    int preserve_monitor;

    drive_clk[mynumber] = 0;
    mydrive_cpu_reset_clk();

    preserve_monitor = mydrive_int_status.global_pending_int & IK_MONITOR;

    cpu_int_status_init(&mydrive_int_status,
			DRIVE_NUMOFINT,
			&drive_last_opcode_info);

    if (preserve_monitor)
	monitor_trap_on(&mydrive_int_status);

    mydrive_trigger_reset();
}

void mydrive_cpu_early_init(void)
{
    clk_guard_init(&mydrive_clk_guard, &drive_clk[mynumber],
                   CLOCK_MAX - 0x100000);

    alarm_context_init(&mydrive_alarm_context, IDENTIFICATION_STRING);

    myvia1_init();
    myvia2_init();
    mycia1571_init();
    mycia1581_init();
    mywd1770_init();
    myriot1_init();
    myriot2_init();
    fdc_init(mynumber, drive_ram + 0x100, &drive[mynumber].rom[0x4000]);
}

void mydrive_cpu_init(int type)
{
    mydrive_mem_init(type);
    mydrive_cpu_reset();
}

inline void mydrive_cpu_wake_up(void)
{
    /* FIXME: this value could break some programs, or be way too high for
       others.  Maybe we should put it into a user-definable resource.  */
    if (clk - last_clk > 0xffffff && drive_clk[mynumber] > 934639) {
printf("CLK: %i LAST: %i\n",clk,last_clk);
        log_message(drive[mynumber].log, "Skipping cycles.");
        last_clk = clk;
    }
}

inline void mydrive_cpu_sleep(void)
{
    /* Currently does nothing.  But we might need this hook some day.  */
}

/* Make sure the 1541 clock counters never overflow; return nonzero if they
   have been decremented to prevent overflow.  */
CLOCK mydrive_cpu_prevent_clk_overflow(CLOCK sub)
{
    if (sub != 0) {
        /* First, get in sync with what the main CPU has done.  Notice that
           `clk' has already been decremented at this point.  */
        if (drive[mynumber].enable) {
            if (last_clk < sub) {
                /* Hm, this is kludgy.  :-(  */
                mydrive_cpu_execute(clk + sub);
            }
            last_clk -= sub;
        } else {
            last_clk = clk;
        }
    }

    /* Then, check our own clock counters.  */
    return clk_guard_prevent_overflow(&mydrive_clk_guard);
}

/* Handle a ROM trap. */
inline static int drive_trap_handler(void)
{
    if (MOS6510_REGS_GET_PC(&drive_cpu_regs) == 0xec9b) {
        /* Idle loop */
        MOS6510_REGS_SET_PC(&drive_cpu_regs, 0xebff);
        if (drive[mynumber].idling_method == DRIVE_IDLE_TRAP_IDLE) {
            CLOCK next_clk;

            next_clk = alarm_context_next_pending_clk(&mydrive_alarm_context);
            drive_clk[mynumber] = next_clk;
        }
        return 0;
    }
    if (MOS6510_REGS_GET_PC(&drive_cpu_regs) == 0xc0be) {
        /* 1581 job code */
        MOS6510_REGS_SET_PC(&drive_cpu_regs, 0xc197);
        wd1770_handle_job_code(mynumber);
        return 0;
    }
    return 1;
}

/* -------------------------------------------------------------------------- */

/* Execute up to the current main CPU clock value.  This automatically
   calculates the corresponding number of clock ticks in the drive.  */
void mydrive_cpu_execute(CLOCK clk_value)
{
    CLOCK cycles;

/* #Define the variables for the CPU registers.  In the drive, there is no
   exporting/importing and we just use global variables.  This also makes it
   possible to let the monitor access the CPU status without too much
   headache.   */
#define reg_a   drive_cpu_regs.reg_a
#define reg_x   drive_cpu_regs.reg_x
#define reg_y   drive_cpu_regs.reg_y
#define reg_pc  drive_cpu_regs.reg_pc
#define reg_sp  drive_cpu_regs.reg_sp
#define reg_p   drive_cpu_regs.reg_p
#define flag_z  drive_cpu_regs.flag_z
#define flag_n  drive_cpu_regs.flag_n

    mydrive_cpu_wake_up();

    if (clk_value > last_clk)
        cycles = clk_value - last_clk;
    else
        cycles = 0;

    while (cycles > 0) {
        CLOCK stop_clk;

	if (cycles > MAX_TICKS) {
	    stop_clk = (drive_clk[mynumber] + clk_conv_table[MAX_TICKS]
			- last_exc_cycles);
	    cycle_accum += clk_mod_table[MAX_TICKS];
	    cycles -= MAX_TICKS;
	} else {
	    stop_clk = (drive_clk[mynumber] + clk_conv_table[cycles]
			- last_exc_cycles);
	    cycle_accum += clk_mod_table[cycles];
	    cycles = 0;
	}

	if (cycle_accum >= 0x10000) {
	    cycle_accum -= 0x10000;
	    stop_clk++;
	}

        while (drive_clk[mynumber] < stop_clk) {

#ifdef IO_AREA_WARNING
#warning IO_AREA_WARNING
	    if (!bank_base)
		fprintf(drive[mynumber].log,
                        "Executing from I/O area at $%04X: "
                        "$%02X $%02X $%04X at clk %ld\n",
                        reg_pc, p0, p1, p2, clk_value);
#endif

/* Include the 6502/6510 CPU emulation core.  */

#define CLK drive_clk[mynumber]
#define RMW_FLAG mydrive_rmw_flag
#define PAGE_ONE pageone
#define LAST_OPCODE_INFO (drive_last_opcode_info)
#define TRACEFLG mydrive_traceflg

#define CPU_INT_STATUS mydrive_int_status

#define ALARM_CONTEXT mydrive_alarm_context

#define JAM() drive_jam()

#define ROM_TRAP_ALLOWED() 1

#define ROM_TRAP_HANDLER() drive_trap_handler()

#define CALLER mymonspace

#define _drive_set_byte_ready(value) drive[mynumber].byte_ready = value

#define _drive_byte_ready() ((drive[mynumber].byte_ready_active == 0x6)	\
                                ? drive_rotate_disk(&drive[mynumber]),	\
                                drive[mynumber].byte_ready : 0)		\

#include "6510core.c"

        }

        last_exc_cycles = drive_clk[mynumber] - stop_clk;
    }

    last_clk = clk_value;
    mydrive_cpu_sleep();
}

/* ------------------------------------------------------------------------- */

void mydrive_set_bank_base(void)
{
    JUMP(reg_pc);
}

/* Inlining this fuction makes no sense and would only bloat the code.  */
static void drive_jam(void)
{
    int tmp;
    char *dname = "  Drive";

    switch(drive[mynumber].type) {
      case DRIVE_TYPE_1541:
        dname = "  1541";
        break;
      case DRIVE_TYPE_1541II:
        dname = "1541-II";
        break;
      case DRIVE_TYPE_1571:
        dname = "  1571";
        break;
      case DRIVE_TYPE_1581:
        dname = "  1581";
        break;
      case DRIVE_TYPE_2031:
        dname = "  2031";
        break;
      case DRIVE_TYPE_1001:
        dname = "  1001";
        break;
    }

    tmp = ui_jam_dialog("%s CPU: JAM at $%04X  ", dname, reg_pc);
    switch (tmp) {
      case UI_JAM_RESET:
        reg_pc = 0xeaa0;
        mydrive_set_bank_base();
        maincpu_trigger_reset();
        break;
      case UI_JAM_HARD_RESET:
        reg_pc = 0xeaa0;
        mydrive_set_bank_base();
        mem_powerup();
        maincpu_trigger_reset();
        break;
      case UI_JAM_MONITOR:
        caller_space = mymonspace;
        mon(reg_pc);
        break;
      default:
        CLK++;
    }
}

/* ------------------------------------------------------------------------- */

static char snap_module_name[] = MYCPU_NAME;
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int mydrive_cpu_write_snapshot_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (0
        || snapshot_module_write_dword(m, (DWORD) drive_clk[mynumber]) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_a) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_x) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_y) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_sp) < 0
        || snapshot_module_write_word(m, (WORD) reg_pc) < 0
        || snapshot_module_write_byte(m, (BYTE) reg_p) < 0
        || snapshot_module_write_dword(m, (DWORD) drive_last_opcode_info) < 0
        || snapshot_module_write_dword(m, (DWORD) last_clk) < 0
        || snapshot_module_write_dword(m, (DWORD) cycle_accum) < 0
        || snapshot_module_write_dword(m, (DWORD) last_exc_cycles) < 0
        )
        goto fail;

    if (interrupt_write_snapshot(&mydrive_int_status, m) < 0)
        goto fail;

    if (drive[mynumber].type == DRIVE_TYPE_1541
        || drive[mynumber].type == DRIVE_TYPE_1541II
        || drive[mynumber].type == DRIVE_TYPE_1571
        || drive[mynumber].type == DRIVE_TYPE_2031) {
        if (snapshot_module_write_byte_array(m, drive_ram, 0x800) < 0)
            goto fail;
    }

    if (drive[mynumber].type == DRIVE_TYPE_1581) {
        if (snapshot_module_write_byte_array(m, drive_ram, 0x2000) < 0)
            goto fail;
    }
    if (drive[mynumber].type == DRIVE_TYPE_1001) {
        if (snapshot_module_write_byte_array(m, drive_ram, 0x1100) < 0)
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

int mydrive_cpu_read_snapshot_module(snapshot_t *s)
{
    BYTE major, minor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &major, &minor);
    if (m == NULL)
        return -1;

    /* Before we start make sure all devices are reset.  */
    mydrive_cpu_reset();

    /* XXX: Assumes `CLOCK' is the same size as a `DWORD'.  */
    if (0
        || snapshot_module_read_dword(m, &drive_clk[mynumber]) < 0
        || snapshot_module_read_byte(m, &reg_a) < 0
        || snapshot_module_read_byte(m, &reg_x) < 0
        || snapshot_module_read_byte(m, &reg_y) < 0
        || snapshot_module_read_byte(m, &reg_sp) < 0
        || read_word_into_unsigned_int(m, &reg_pc) < 0
        || snapshot_module_read_byte(m, &reg_p) < 0
        || snapshot_module_read_dword(m, &drive_last_opcode_info) < 0
        || snapshot_module_read_dword(m, &last_clk) < 0
        || snapshot_module_read_dword(m, &cycle_accum) < 0
        || snapshot_module_read_dword(m, &last_exc_cycles) < 0
        )
        goto fail;

    log_message(drive[mynumber].log, "RESET (For undump).");

    cpu_int_status_init(&mydrive_int_status, DRIVE_NUMOFINT,
                        &drive_last_opcode_info);

    myvia1_reset();
    myvia2_reset();
    mycia1571_reset();
    mycia1581_reset();
    mywd1770_reset();
    myriot1_reset();
    myriot2_reset();

    if (interrupt_read_snapshot(&mydrive_int_status, m) < 0)
        goto fail;

    if (drive[mynumber].type == DRIVE_TYPE_1541
        || drive[mynumber].type == DRIVE_TYPE_1541II
        || drive[mynumber].type == DRIVE_TYPE_1571
        || drive[mynumber].type == DRIVE_TYPE_2031) {
        if (snapshot_module_read_byte_array(m, drive_ram, 0x800) < 0)
            goto fail;
    }

    if (drive[mynumber].type == DRIVE_TYPE_1581) {
        if (snapshot_module_read_byte_array(m, drive_ram, 0x2000) < 0)
            goto fail;
    }

    if (drive[mynumber].type == DRIVE_TYPE_1001) {
        if (snapshot_module_read_byte_array(m, drive_ram, 0x1100) < 0)
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

