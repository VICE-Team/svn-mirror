/*
 * interrupt.c - Implementation of CPU interrupts and alarms.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "interrupt.h"
#include "log.h"
#include "snapshot.h"
#include "types.h"

/* ------------------------------------------------------------------------- */

/* Initialization.  */
void interrupt_cpu_status_init(cpu_int_status_t *cs, int num_ints,
                               opcode_info_t *last_opcode_info_ptr)
{
    memset(cs, 0, sizeof(cpu_int_status_t));
    cs->num_ints = num_ints;
    cs->last_opcode_info_ptr = last_opcode_info_ptr;
    cs->num_last_stolen_cycles = 0;
    cs->last_stolen_cycles_clk = (CLOCK)0;
    cs->global_pending_int = IK_NONE;
    cs->nmi_trap_func = NULL;
    cs->reset_trap_func = NULL;
}

void interrupt_set_nmi_trap_func(cpu_int_status_t *cs,
                                 void (*nmi_trap_func)(void))
{
    cs->nmi_trap_func = nmi_trap_func;
}

void interrupt_set_reset_trap_func(cpu_int_status_t *cs,
                                 void (*reset_trap_func)(void))
{
    cs->reset_trap_func = reset_trap_func;
}

/* Move all the CLOCK time references forward/backward.  */
void interrupt_cpu_status_time_warp(cpu_int_status_t *cs, CLOCK warp_amount,
                                    int warp_direction)
{
    if (warp_direction == 0)
        return;

    if (warp_direction > 0) {
        cs->irq_clk += warp_amount;
        cs->nmi_clk += warp_amount;
        cs->last_stolen_cycles_clk += warp_amount;
    } else {
        if (cs->irq_clk > warp_amount) {
            cs->irq_clk -= warp_amount;
        } else {
            cs->irq_clk = (CLOCK) 0;
        }
        if (cs->nmi_clk > warp_amount) {
            cs->nmi_clk -= warp_amount;
        } else {
            cs->nmi_clk = (CLOCK) 0;
        }
        if (cs->last_stolen_cycles_clk > warp_amount) {
            cs->last_stolen_cycles_clk -= warp_amount;
        } else {
            cs->last_stolen_cycles_clk = (CLOCK) 0;
        }
    }
}

void interrupt_log_wrong_nirq(void)
{
    log_error(LOG_DEFAULT, "interrupt_set_irq(): wrong nirq!");
}

void interrupt_log_wrong_nnmi(void)
{
    log_error(LOG_DEFAULT, "set_nmi(): wrong nnmi!");
}

/* ------------------------------------------------------------------------- */

/* These functions are used by snaphots only: they update the IRQ/NMI line
   value, but do not update the variables used to emulate the timing.  This
   is necessary to allow the chip modules to dump their own IRQ/NMI status
   information; the global timing status is stored in the CPU module (see
   `interrupt_write_snapshot()' and `interrupt_read_snapshot()'). */

void interrupt_set_irq_noclk(cpu_int_status_t *cs, int int_num, int value)
{
    if (value) {
        if (!(cs->pending_int[int_num] & IK_IRQ)) {
            cs->nirq++;
            cs->global_pending_int |= IK_IRQ;
            cs->pending_int[int_num] |= IK_IRQ;
        }
    } else {
        if (cs->pending_int[int_num] & IK_IRQ) {
            if (cs->nirq > 0) {
                cs->pending_int[int_num] &= ~IK_IRQ;
                if (--cs->nirq == 0)
                    cs->global_pending_int &= ~IK_IRQ;
            }
        }
    }
}

void interrupt_set_nmi_noclk(cpu_int_status_t *cs, int int_num, int value)
{
    if (value) {
        if (!(cs->pending_int[int_num] & IK_NMI)) {
            if (cs->nnmi == 0 && !(cs->global_pending_int & IK_NMI))
                cs->global_pending_int |= IK_NMI;
            cs->nnmi++;
            cs->pending_int[int_num] |= IK_NMI;
        }
    } else {
        if (cs->pending_int[int_num] & IK_NMI) {
            if (cs->nnmi > 0) {
                cs->nnmi--;
                cs->pending_int[int_num] &= ~IK_NMI;
                if (maincpu_clk == cs->nmi_clk)
                    cs->global_pending_int &= ~IK_NMI;
            }
        }
    }
}

void interrupt_set_int_noclk(cpu_int_status_t *cs, int int_num,
                             enum cpu_int value)
{
    interrupt_set_nmi(cs, int_num, (int)(value & IK_NMI), maincpu_clk);
    interrupt_set_irq(cs, int_num, (int)(value & IK_IRQ), maincpu_clk);
}

int interrupt_get_irq(cpu_int_status_t *cs, int int_num)
{
    return cs->pending_int[int_num] & IK_IRQ;
}

int interrupt_get_nmi(cpu_int_status_t *cs, int int_num)
{
    return cs->pending_int[int_num] & IK_NMI;
}

enum cpu_int get_int(cpu_int_status_t *cs, int int_num)
{
    return cs->pending_int[int_num];
}

/* ------------------------------------------------------------------------- */

void interrupt_trigger_dma(cpu_int_status_t *cs, CLOCK cpu_clk)
{
    cs->global_pending_int = (enum cpu_int)
        (cs->global_pending_int | IK_DMA);
}

void interrupt_ack_dma(cpu_int_status_t *cs)
{
    cs->global_pending_int = (enum cpu_int)
        (cs->global_pending_int & ~IK_DMA);
}

/* ------------------------------------------------------------------------- */

/* Trigger a RESET.  This resets the machine.  */
void interrupt_trigger_reset(cpu_int_status_t *cs, CLOCK cpu_clk)
{
    cs->global_pending_int |= IK_RESET;

        if (cs->reset_trap_func)
                cs->reset_trap_func();
}

/* Acknowledge a RESET condition, by removing it.  */
void interrupt_ack_reset(cpu_int_status_t *cs)
{
    cs->global_pending_int &= ~IK_RESET;
}

/* Trigger a TRAP.  This is a special condition that can be used for
   debugging.  `trap_func' will be called with PC as the argument when this
   condition is detected.  */
void interrupt_trigger_trap(cpu_int_status_t *cs, void (*trap_func)(ADDRESS,
                  void *data), void *data, CLOCK cpu_clk)
{
    cs->global_pending_int |= IK_TRAP;
    cs->trap_func = trap_func;
    cs->trap_data = data;
}

/* Dispatch the TRAP condition.  */
void interrupt_do_trap(cpu_int_status_t *cs, ADDRESS address)
{
    cs->global_pending_int &= ~IK_TRAP;
    cs->trap_func(address, cs->trap_data);
}

void interrupt_monitor_trap_on(cpu_int_status_t *cs)
{
    cs->global_pending_int |= IK_MONITOR;
}

void interrupt_monitor_trap_off(cpu_int_status_t *cs)
{
    cs->global_pending_int &= ~IK_MONITOR;
}

/* ------------------------------------------------------------------------- */

int interrupt_write_snapshot(cpu_int_status_t *cs, snapshot_module_t *m)
{
    /* FIXME: could we avoid some of this info?  */
    if (snapshot_module_write_dword(m, cs->irq_clk) < 0
        || snapshot_module_write_dword(m, cs->nmi_clk) < 0
        || snapshot_module_write_dword(m, (DWORD)cs->num_last_stolen_cycles) < 0
        || snapshot_module_write_dword(m, cs->last_stolen_cycles_clk) < 0)
        return -1;

    return 0;
}

int interrupt_read_snapshot(cpu_int_status_t *cs, snapshot_module_t *m)
{
    int i;
    DWORD dw;

    for (i = 0; i < cs->num_ints; i++)
        cs->pending_int[i] = IK_NONE;
    cs->global_pending_int = IK_NONE;
    cs->nirq = cs->nnmi = cs->reset = cs->trap = 0;

    if (snapshot_module_read_dword(m, &cs->irq_clk) < 0
        || snapshot_module_read_dword(m, &cs->nmi_clk) < 0)
        return -1;

    if (snapshot_module_read_dword(m, &dw) < 0)
        return -1;
    cs->num_last_stolen_cycles = dw;

    if (snapshot_module_read_dword(m, &dw) < 0)
        return -1;
    cs->last_stolen_cycles_clk = dw;

    return 0;
}

