/*
 * drivecpu.c - 6502 processor emulation of CBM disk drives.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Patches by
 *  Andre Fachat <a.fachat@physik.tu-chemnitz.de>
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
#include <string.h>

#include "6510core.h"
#include "alarm.h"
#include "clkguard.h"
#include "debug.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivemem.h"
#include "drivetypes.h"
#include "interrupt.h"
#include "log.h"
#include "machine-drive.h"
#include "machine.h"
#include "mem.h"
#include "monitor.h"
#include "mos6510.h"
#include "rotation.h"
#include "snapshot.h"
#include "types.h"


#define DRIVE_CPU

static void drive_jam(drive_context_t *drv);

static BYTE drive_bank_read(drive_context_t *drv, int bank, WORD address);
static BYTE drive_bank_peek(drive_context_t *drv, int bank, WORD address);
static void drive_bank_store(drive_context_t *drv, int bank, WORD address,
                             BYTE value);
void drive_toggle_watchpoints(drive_context_t *drv, int flag);

interrupt_cpu_status_t *drive0_int_status_ptr;
interrupt_cpu_status_t *drive1_int_status_ptr;

monitor_interface_t *drive0_monitor_interface_get(void)
{
    return drive0_context.cpu.monitor_interface;
}

monitor_interface_t *drive1_monitor_interface_get(void)
{
    return drive1_context.cpu.monitor_interface;
}

/* non-time critical monitor functions; should be OK */
static BYTE drive0_bank_read(int bank, WORD adr)
{
    return drive_bank_read(&drive0_context, bank, adr);
}

static BYTE drive0_bank_peek(int bank, WORD adr)
{
    return drive_bank_peek(&drive0_context, bank, adr);
}

static void drive0_bank_store(int bank, WORD adr, BYTE val)
{
    drive_bank_store(&drive0_context, bank, adr, val);
}

static void drive0_toggle_watchpoints(int flag)
{
    drive_toggle_watchpoints(&drive0_context, flag);
}

static BYTE drive1_bank_read(int bank, WORD adr)
{
    return drive_bank_read(&drive1_context, bank, adr);
}

static BYTE drive1_bank_peek(int bank, WORD adr)
{
    return drive_bank_peek(&drive1_context, bank, adr);
}

static void drive1_bank_store(int bank, WORD adr, BYTE val)
{
    drive_bank_store(&drive1_context, bank, adr, val);
}

static void drive1_toggle_watchpoints(int flag)
{
    drive_toggle_watchpoints(&drive1_context, flag);
}

static void drive0_set_bank_base(void)
{
    drive_set_bank_base(&drive0_context);
}

static void drive1_set_bank_base(void)
{
    drive_set_bank_base(&drive1_context);
}

void drive_cpu_setup_context(drive_context_t *drv)
{
    monitor_interface_t *mi;

    drv->cpu.int_status = interrupt_cpu_status_new();
    interrupt_cpu_status_init(drv->cpu.int_status,
                              &(drv->cpu.last_opcode_info));
    drv->cpu.rmw_flag = 0;
    drv->cpu.d_bank_limit = -1;
    drv->cpu.pageone = NULL;
    sprintf(drv->cpu.snap_module_name, "DRIVECPU%d", drv->mynumber);
    sprintf(drv->cpu.identification_string, "DRIVE#%d", drv->mynumber + 8);
    drv->cpu.monitor_interface = monitor_interface_new();
    mi = drv->cpu.monitor_interface;
    mi->cpu_regs = &(drv->cpu.cpu_regs);
    mi->z80_cpu_regs = NULL;
    mi->int_status = drv->cpu.int_status;
    mi->clk = &(drive_clk[drv->mynumber]);
    mi->current_bank = 0;
    mi->mem_bank_list = NULL;
    mi->mem_bank_from_name = NULL;
    mi->get_line_cycle = NULL;

    if (drv->mynumber == 0) {
        mi->mem_bank_read = drive0_bank_read;
        mi->mem_bank_peek = drive0_bank_peek;
        mi->mem_bank_write = drive0_bank_store;
        mi->mem_ioreg_list_get = drive0_ioreg_list_get;
        mi->toggle_watchpoints_func = drive0_toggle_watchpoints;
        mi->set_bank_base = drive0_set_bank_base;

        drv->cpu.monspace = e_disk8_space;
        drive0_int_status_ptr = drv->cpu.int_status;
    } else {
        mi->mem_bank_read = drive1_bank_read;
        mi->mem_bank_peek = drive1_bank_peek;
        mi->mem_bank_write = drive1_bank_store;
        mi->mem_ioreg_list_get = drive1_ioreg_list_get;
        mi->toggle_watchpoints_func = drive1_toggle_watchpoints;
        mi->set_bank_base = drive1_set_bank_base;

        drv->cpu.monspace = e_disk9_space;
        drive1_int_status_ptr = drv->cpu.int_status;
    }
}

/* ------------------------------------------------------------------------- */

#define LOAD(a)           (drv->cpud.read_func[(a) >> 8](drv, (WORD)(a)))
#define LOAD_ZERO(a)      (drv->cpud.read_func[0](drv, (WORD)(a)))
#define LOAD_ADDR(a)      (LOAD(a) | (LOAD((a) + 1) << 8))
#define LOAD_ZERO_ADDR(a) (LOAD_ZERO(a) | (LOAD_ZERO((a) + 1) << 8))
#define STORE(a, b)       (drv->cpud.store_func[(a) >> 8](drv, (WORD)(a), \
                          (BYTE)(b)))
#define STORE_ZERO(a, b)  (drv->cpud.store_func[0](drv, (WORD)(a), \
                          (BYTE)(b)))

/* FIXME: pc can not jump to VIA adress space in 1541 and 1571 emulation.  */
/* FIXME: SFD1001 does not use bank_base at all due to messy memory mapping.
   We should use tables like in maincpu instead (AF) */
#define JUMP(addr)                                      \
    do {                                                \
        reg_pc = (addr);                                \
        if (drv->drive_ptr->type == 1001) {             \
            drv->cpu.d_bank_base = NULL;                \
            drv->cpu.d_bank_limit = -1;                 \
        } else if (reg_pc < 0x2000) {                   \
            drv->cpu.d_bank_base = drv->cpud.drive_ram; \
            drv->cpu.d_bank_limit = 0x07fd;             \
        } else if (reg_pc >= drv->drive_ptr->rom_start) {        \
            drv->cpu.d_bank_base = drv->drive_ptr->rom - 0x8000; \
            drv->cpu.d_bank_limit = 0xfffd;             \
        } else {                                        \
            drv->cpu.d_bank_base = NULL;                \
            drv->cpu.d_bank_limit = -1;                 \
        }                                               \
    } while (0)

/* ------------------------------------------------------------------------- */

/* This is the external interface for memory access.  */

BYTE REGPARM2 drive_read(drive_context_t *drv, WORD address)
{
    return drv->cpud.read_func[address >> 8](drv, address);
}

void REGPARM3 drive_store(drive_context_t *drv, WORD address, BYTE value)
{
    drv->cpud.store_func[address >> 8](drv, address, value);
}

/* This is the external interface for banked memory access.  */

static BYTE drive_bank_read(drive_context_t *drv, int bank, WORD address)
{
    return drv->cpud.read_func[address >> 8](drv, address);
}

/* FIXME: use peek in IO area */
static BYTE drive_bank_peek(drive_context_t *drv, int bank, WORD address)
{
    return drv->cpud.read_func[address >> 8](drv, address);
}

static void drive_bank_store(drive_context_t *drv, int bank, WORD address,
                             BYTE value)
{
    drv->cpud.store_func[address >> 8](drv, address, value);
}

/* ------------------------------------------------------------------------- */

static void cpu_reset(drive_context_t *drv)
{
    int preserve_monitor;

    preserve_monitor = drv->cpu.int_status->global_pending_int & IK_MONITOR;

    log_message(drv->drive_ptr->log, "RESET.");

    interrupt_cpu_status_reset(drv->cpu.int_status);

    *(drv->clk_ptr) = 6;
    rotation_reset(drv->mynumber);
    machine_drive_reset(drv);

    if (preserve_monitor)
        interrupt_monitor_trap_on(drv->cpu.int_status);
}

void drive_toggle_watchpoints(drive_context_t *drv, int flag)
{
    if (flag) {
        memcpy(drv->cpud.read_func, drv->cpud.read_func_watch,
               sizeof(drive_read_func_t *) * 0x101);
        memcpy(drv->cpud.store_func, drv->cpud.store_func_watch,
               sizeof(drive_store_func_t *) * 0x101);
    } else {
        memcpy(drv->cpud.read_func, drv->cpud.read_func_nowatch,
               sizeof(drive_read_func_t *) * 0x101);
        memcpy(drv->cpud.store_func, drv->cpud.store_func_nowatch,
               sizeof(drive_store_func_t *) * 0x101);
    }
}

void drive_cpu_reset_clk(drive_context_t *drv)
{
    drv->cpu.last_clk = maincpu_clk;
    drv->cpu.last_exc_cycles = 0;
}

void drive_cpu_reset(drive_context_t *drv)
{
    int preserve_monitor;

    *(drv->clk_ptr) = 0;
    drive_cpu_reset_clk(drv);

    preserve_monitor = drv->cpu.int_status->global_pending_int & IK_MONITOR;

    interrupt_cpu_status_reset(drv->cpu.int_status);

    if (preserve_monitor)
        interrupt_monitor_trap_on(drv->cpu.int_status);

    /* FIXME -- ugly, should be changed in interrupt.h */
    interrupt_trigger_reset(drv->cpu.int_status, *(drv->clk_ptr));
}

void drive_cpu_early_init(drive_context_t *drv)
{
    drv->cpu.clk_guard = clk_guard_new(drv->clk_ptr, CLOCK_MAX
                                       - CLKGUARD_SUB_MIN);

    drv->cpu.alarm_context = alarm_context_new(drv->cpu.identification_string);

    machine_drive_init(drv);
}

void drive_cpu_shutdown(drive_context_t *drv)
{
    if (drv->cpu.alarm_context != NULL)
        alarm_context_destroy(drv->cpu.alarm_context);
    if (drv->cpu.clk_guard != NULL)
        clk_guard_destroy(drv->cpu.clk_guard);

    monitor_interface_destroy(drv->cpu.monitor_interface);
    interrupt_cpu_status_destroy(drv->cpu.int_status);
}

void drive_cpu_init(drive_context_t *drv, int type)
{
    drive_mem_init(drv, type);
    drive_cpu_reset(drv);
}

inline void drive_cpu_wake_up(drive_context_t *drv)
{
    /* FIXME: this value could break some programs, or be way too high for
       others.  Maybe we should put it into a user-definable resource.  */
    if (maincpu_clk - drv->cpu.last_clk > 0xffffff
        && *(drv->clk_ptr) > 934639) {
        log_message(drv->drive_ptr->log, "Skipping cycles.");
        drv->cpu.last_clk = maincpu_clk;
    }
}

inline void drive_cpu_sleep(drive_context_t *drv)
{
    /* Currently does nothing.  But we might need this hook some day.  */
}

/* Make sure the drive clock counters never overflow; return nonzero if
   they have been decremented to prevent overflow.  */
CLOCK drive_cpu_prevent_clk_overflow(drive_context_t *drv, CLOCK sub)
{
    if (sub != 0) {
        /* First, get in sync with what the main CPU has done.  Notice that
           `clk' has already been decremented at this point.  */
        if (drv->drive_ptr->enable) {
            if (drv->cpu.last_clk < sub) {
                /* Hm, this is kludgy.  :-(  */
                if (drive[0].enable)
                    drive0_cpu_execute(maincpu_clk + sub);
                if (drive[1].enable)
                    drive1_cpu_execute(maincpu_clk + sub);
            }
            drv->cpu.last_clk -= sub;
        } else {
            drv->cpu.last_clk = maincpu_clk;
        }
    }

    /* Then, check our own clock counters.  */
    return clk_guard_prevent_overflow(drv->cpu.clk_guard);
}

/* Handle a ROM trap. */
inline static DWORD drive_trap_handler(drive_context_t *drv)
{
    if (MOS6510_REGS_GET_PC(&(drv->cpu.cpu_regs)) == 0xec9b) {
        /* Idle loop */
        MOS6510_REGS_SET_PC(&(drv->cpu.cpu_regs), 0xebff);
        if (drv->drive_ptr->idling_method == DRIVE_IDLE_TRAP_IDLE) {
            CLOCK next_clk;

            next_clk = alarm_context_next_pending_clk(drv->cpu.alarm_context);

            if (next_clk > drv->cpu.stop_clk)
                next_clk = drv->cpu.stop_clk;

            *(drv->clk_ptr) = next_clk;
        }
        return 0;
    }
    if (MOS6510_REGS_GET_PC(&(drv->cpu.cpu_regs)) == 0xc0be) {
        /* 1581 job code */
        MOS6510_REGS_SET_PC(&(drv->cpu.cpu_regs), 0xc197);
        machine_drive_handle_job_code(drv->mynumber);
        return 0;
    }
    return -1;
}

static void drive_generic_dma(void)
{
    /* Generic DMA hosts can be implemented here.
       Not very likey for disk drives. */
}

/* -------------------------------------------------------------------------- */

/* Return nonzero if a pending NMI should be dispatched now.  This takes
   account for the internal delays of the 6510, but does not actually check
   the status of the NMI line.  */
inline static int interrupt_check_nmi_delay(interrupt_cpu_status_t *cs,
                                            CLOCK cpu_clk)
{
    CLOCK nmi_clk = cs->nmi_clk + INTERRUPT_DELAY;

    /* Branch instructions delay IRQs and NMI by one cycle if branch
       is taken with no page boundary crossing.  */
    if (OPINFO_DELAYS_INTERRUPT(*cs->last_opcode_info_ptr))
        nmi_clk++;

    if (cpu_clk >= nmi_clk)
        return 1;

    return 0;
}

/* Return nonzero if a pending IRQ should be dispatched now.  This takes
   account for the internal delays of the 6510, but does not actually check
   the status of the IRQ line.  */
inline static int interrupt_check_irq_delay(interrupt_cpu_status_t *cs,
                                            CLOCK cpu_clk)
{
    CLOCK irq_clk = cs->irq_clk + INTERRUPT_DELAY;

    /* Branch instructions delay IRQs and NMI by one cycle if branch
       is taken with no page boundary crossing.  */
    if (OPINFO_DELAYS_INTERRUPT(*cs->last_opcode_info_ptr))
        irq_clk++;

    /* If an opcode changes the I flag from 1 to 0, the 6510 needs
       one more opcode before it triggers the IRQ routine.  */
    if (cpu_clk >= irq_clk && !OPINFO_ENABLES_IRQ(*cs->last_opcode_info_ptr))
        return 1;

    return 0;
}

/* -------------------------------------------------------------------------- */
/* Execute up to the current main CPU clock value.  This automatically
   calculates the corresponding number of clock ticks in the drive.  */
void drivex_cpu_execute(drive_context_t *drv, CLOCK clk_value)
{
    CLOCK cycles;

/* #Define the variables for the CPU registers.  In the drive, there is no
   exporting/importing and we just use global variables.  This also makes it
   possible to let the monitor access the CPU status without too much
   headache.   */
#define reg_a   (drv->cpu.cpu_regs.a)
#define reg_x   (drv->cpu.cpu_regs.x)
#define reg_y   (drv->cpu.cpu_regs.y)
#define reg_pc  (drv->cpu.cpu_regs.pc)
#define reg_sp  (drv->cpu.cpu_regs.sp)
#define reg_p   (drv->cpu.cpu_regs.p)
#define flag_z  (drv->cpu.cpu_regs.z)
#define flag_n  (drv->cpu.cpu_regs.n)

    drive_cpu_wake_up(drv);

    if (clk_value > drv->cpu.last_clk)
        cycles = clk_value - drv->cpu.last_clk;
    else
        cycles = 0;

    while (cycles > 0) {
        if (cycles > MAX_TICKS) {
            drv->cpu.stop_clk = (*(drv->clk_ptr)
                                + drv->cpud.clk_conv_table[MAX_TICKS]
                                - drv->cpu.last_exc_cycles);
            drv->cpu.cycle_accum += drv->cpud.clk_mod_table[MAX_TICKS];
            cycles -= MAX_TICKS;
        } else {
            drv->cpu.stop_clk = (*(drv->clk_ptr)
                                + drv->cpud.clk_conv_table[cycles]
                                - drv->cpu.last_exc_cycles);
            drv->cpu.cycle_accum += drv->cpud.clk_mod_table[cycles];
            cycles = 0;
        }

        if (drv->cpu.cycle_accum >= 0x10000) {
            drv->cpu.cycle_accum -= 0x10000;
            (drv->cpu.stop_clk)++;
        }

        while (*(drv->clk_ptr) < drv->cpu.stop_clk) {

/* Include the 6502/6510 CPU emulation core.  */

#define CLK (*(drv->clk_ptr))
#define RMW_FLAG (drv->cpu.rmw_flag)
#define PAGE_ONE (drv->cpu.pageone)
#define LAST_OPCODE_INFO (drv->cpu.last_opcode_info)
#define TRACEFLG (debug.drivecpu_traceflg[drv->mynumber])

#define CPU_INT_STATUS (drv->cpu.int_status)

#define ALARM_CONTEXT (drv->cpu.alarm_context)

#define JAM() drive_jam(drv)

#define ROM_TRAP_ALLOWED() 1

#define ROM_TRAP_HANDLER() drive_trap_handler(drv)

#define CALLER (drv->cpu.monspace)

#define DMA_FUNC drive_generic_dma()

#define DMA_ON_RESET

#define _drive_byte_ready_egde_clear()                \
    do {                                              \
        if (drv->drive_ptr->byte_ready_active == 0x6) \
            rotation_rotate_disk(drv->drive_ptr);     \
        drv->drive_ptr->byte_ready_edge = 0;          \
    } while (0)

#define _drive_byte_ready() ((drv->drive_ptr->byte_ready_active == 0x6) \
                                ? rotation_rotate_disk(drv->drive_ptr), \
                                drv->drive_ptr->byte_ready_edge : 0)    \

#define cpu_reset() (cpu_reset)(drv)
#define bank_limit (drv->cpu.d_bank_limit)
#define bank_base (drv->cpu.d_bank_base)

#include "6510core.c"

        }

        drv->cpu.last_exc_cycles = *(drv->clk_ptr) - drv->cpu.stop_clk;
    }

    drv->cpu.last_clk = clk_value;
    drive_cpu_sleep(drv);
}

/* ------------------------------------------------------------------------- */

void drive_set_bank_base(drive_context_t *drv)
{
    JUMP(reg_pc);
}

/* Inlining this fuction makes no sense and would only bloat the code.  */
static void drive_jam(drive_context_t *drv)
{
    unsigned int tmp;
    char *dname = "  Drive";

    switch(drv->drive_ptr->type) {
      case DRIVE_TYPE_1541:
        dname = "  1541";
        break;
      case DRIVE_TYPE_1541II:
        dname = "1541-II";
        break;
      case DRIVE_TYPE_1551:
        dname = "  1551";
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
      case DRIVE_TYPE_2040:
        dname = "  2040";
        break;
      case DRIVE_TYPE_3040:
        dname = "  3040";
        break;
      case DRIVE_TYPE_4040:
        dname = "  4040";
        break;
      case DRIVE_TYPE_8050:
        dname = "  8050";
        break;
      case DRIVE_TYPE_8250:
        dname = "  8250";
      break;
    }

    tmp = machine_jam("%s CPU: JAM at $%04X  ", dname, (int)reg_pc);
    switch (tmp) {
      case JAM_RESET:
        reg_pc = 0xeaa0;
        drive_set_bank_base(drv);
        machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
        break;
      case JAM_HARD_RESET:
        reg_pc = 0xeaa0;
        drive_set_bank_base(drv);
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
        break;
      case JAM_MONITOR:
        caller_space = drv->cpu.monspace;
        monitor_startup();
        break;
      default:
        CLK++;
    }
}

/* ------------------------------------------------------------------------- */

#define SNAP_MAJOR 1
#define SNAP_MINOR 1

int drive_cpu_snapshot_write_module(drive_context_t *drv, snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, drv->cpu.snap_module_name,
                               ((BYTE)(SNAP_MAJOR)), ((BYTE)(SNAP_MINOR)));
    if (m == NULL)
        return -1;

    if (0
        || SMW_DW(m, (DWORD) *(drv->clk_ptr)) < 0
        || SMW_B(m, (BYTE)MOS6510_REGS_GET_A(&(drv->cpu.cpu_regs))) < 0
        || SMW_B(m, (BYTE)MOS6510_REGS_GET_X(&(drv->cpu.cpu_regs))) < 0
        || SMW_B(m, (BYTE)MOS6510_REGS_GET_Y(&(drv->cpu.cpu_regs))) < 0
        || SMW_B(m, (BYTE)MOS6510_REGS_GET_SP(&(drv->cpu.cpu_regs))) < 0
        || SMW_W(m, (WORD)MOS6510_REGS_GET_PC(&(drv->cpu.cpu_regs))) < 0
        || SMW_B(m, (BYTE)MOS6510_REGS_GET_STATUS(&(drv->cpu.cpu_regs))) < 0
        || SMW_DW(m, (DWORD)(drv->cpu.last_opcode_info)) < 0
        || SMW_DW(m, (DWORD)(drv->cpu.last_clk)) < 0
        || SMW_DW(m, (DWORD)(drv->cpu.cycle_accum)) < 0
        || SMW_DW(m, (DWORD)(drv->cpu.last_exc_cycles)) < 0
        )
        goto fail;

    if (interrupt_write_snapshot(drv->cpu.int_status, m) < 0)
        goto fail;

    if (drv->drive_ptr->type == DRIVE_TYPE_1541
        || drv->drive_ptr->type == DRIVE_TYPE_1541II
        || drv->drive_ptr->type == DRIVE_TYPE_1551
        || drv->drive_ptr->type == DRIVE_TYPE_1571
        || drv->drive_ptr->type == DRIVE_TYPE_2031) {
        if (SMW_BA(m, drv->cpud.drive_ram, 0x800) < 0)
            goto fail;
    }

    if (drv->drive_ptr->type == DRIVE_TYPE_1581) {
        if (SMW_BA(m, drv->cpud.drive_ram, 0x2000) < 0)
            goto fail;
    }
    if (DRIVE_IS_OLDTYPE(drv->drive_ptr->type)) {
        if (SMW_BA(m, drv->cpud.drive_ram, 0x1100) < 0)
            goto fail;
    }

    if (interrupt_write_new_snapshot(drv->cpu.int_status, m) < 0)
        goto fail;

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

int drive_cpu_snapshot_read_module(drive_context_t *drv, snapshot_t *s)
{
    BYTE major, minor;
    snapshot_module_t *m;
    BYTE a, x, y, sp, status;
    WORD pc;

    m = snapshot_module_open(s, drv->cpu.snap_module_name, &major, &minor);
    if (m == NULL)
        return -1;

    /* Before we start make sure all devices are reset.  */
    drive_cpu_reset(drv);

    /* XXX: Assumes `CLOCK' is the same size as a `DWORD'.  */
    if (0
        || SMR_DW(m, drv->clk_ptr) < 0
        || SMR_B(m, &a) < 0
        || SMR_B(m, &x) < 0
        || SMR_B(m, &y) < 0
        || SMR_B(m, &sp) < 0
        || SMR_W(m, &pc) < 0
        || SMR_B(m, &status) < 0
        || SMR_DW_INT(m, (int*)&(drv->cpu.last_opcode_info)) < 0
        || SMR_DW(m, &(drv->cpu.last_clk)) < 0
        || SMR_DW(m, &(drv->cpu.cycle_accum)) < 0
        || SMR_DW(m, &(drv->cpu.last_exc_cycles)) < 0
        )
        goto fail;

    MOS6510_REGS_SET_A(&(drv->cpu.cpu_regs), a);
    MOS6510_REGS_SET_X(&(drv->cpu.cpu_regs), x);
    MOS6510_REGS_SET_Y(&(drv->cpu.cpu_regs), y);
    MOS6510_REGS_SET_SP(&(drv->cpu.cpu_regs), sp);
    MOS6510_REGS_SET_PC(&(drv->cpu.cpu_regs), pc);
    MOS6510_REGS_SET_STATUS(&(drv->cpu.cpu_regs), status);

    log_message(drv->drive_ptr->log, "RESET (For undump).");

    interrupt_cpu_status_reset(drv->cpu.int_status);

    machine_drive_reset(drv);

    if (interrupt_read_snapshot(drv->cpu.int_status, m) < 0)
        goto fail;

    if (drv->drive_ptr->type == DRIVE_TYPE_1541
        || drv->drive_ptr->type == DRIVE_TYPE_1541II
        || drv->drive_ptr->type == DRIVE_TYPE_1551
        || drv->drive_ptr->type == DRIVE_TYPE_1571
        || drv->drive_ptr->type == DRIVE_TYPE_2031) {
        if (SMR_BA(m, drv->cpud.drive_ram, 0x800) < 0)
            goto fail;
    }

    if (drv->drive_ptr->type == DRIVE_TYPE_1581) {
        if (SMR_BA(m, drv->cpud.drive_ram, 0x2000) < 0)
            goto fail;
    }

    if (DRIVE_IS_OLDTYPE(drv->drive_ptr->type)) {
        if (SMR_BA(m, drv->cpud.drive_ram, 0x1100) < 0)
            goto fail;
    }

    /* Update `*bank_base'.  */
    JUMP(reg_pc);

    interrupt_read_new_snapshot(drv->cpu.int_status, m);

    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

