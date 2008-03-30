/*
 * interrupt.h - Implementation of 6510 interrupts and alarms.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#include "6510core.h"
#include "types.h"

/* This handles the interrupt lines and the CPU alarms (i.e. events that happen
   at specified clock ticks during emulation).  */

#ifdef DEBUG
extern int debugflg;
#endif

/* ------------------------------------------------------------------------- */

/* Interrupts.  This is a bit of a mess...  */
enum {
    I_ACIA1,                    /* ACIA 1 */
    I_ACIA2,                    /* ACIA 2 (unused) */
    I_CIA1FL,                   /* CIA 1 */
    I_CIA2FL,                   /* CIA 2 */
    I_FREEZE,                   /* Cartridge freeze */
    I_PIA1,                     /* PIA1 */
    I_PIA2,                     /* PIA2 */
    I_RASTER,                   /* Raster compare */
    I_RESTORE,                  /* RESTORE key */
    I_REU,                      /* REU */
    I_TPI1,                     /* TPI 1 (CBM-II) */
    I_VIA1FL,                   /* VIA 1 */
    I_VIA2FL,                   /* VIA 2 */
    I_VIAFL,                    /* VIA (PET) */
    I_IEEEVIA1FL,               /* IEEE488 VIC20 VIA1 */
    I_IEEEVIA2FL,               /* IEEE488 VIC20 VIA2 */
    NUMOFINT
};

/* ------------------------------------------------------------------------- */

/* Define the number of cycles needed by the CPU to detect the NMI or IRQ.  */
#define INTERRUPT_DELAY 2

/* These are the available types of interrupt lines.  */
enum cpu_int {
    IK_NONE    = 0,
    IK_NMI     = 1 << 0,
    IK_IRQ     = 1 << 1,
    IK_RESET   = 1 << 2,
    IK_TRAP    = 1 << 3,
    IK_MONITOR = 1 << 4,
    IK_DMA     = 1 << 5
};

/* We not care about wasted space here, and make fixed-length large enough
   arrays since static allocation can be handled more easily...  */
struct cpu_int_status_s {
    /* Number of interrupt lines.  */
    int num_ints;

    /* Define, for each interrupt source, whether it has a pending interrupt
       (IK_IRQ, IK_NMI, IK_RESET and IK_TRAP) or not (IK_NONE).  */
    enum cpu_int pending_int[0x100];

    /* Number of active IRQ lines.  */
    int nirq;

    /* Tick when the IRQ was triggered.  */
    CLOCK irq_clk;

    /* Number of active NMI lines.  */
    int nnmi;

    /* Tick when the NMI was triggered.  */
    CLOCK nmi_clk;

    /* If 1, do a RESET.  */
    int reset;

    /* If 1, call the trapping function.  */
    int trap;

    /* Debugging function.  */
    void (*trap_func)(ADDRESS, void *data);

    /* Data to pass to the debugging function when called.  */
    void *trap_data;

    /* Pointer to the last executed opcode information.  */
    DWORD /*opcode_info_t*/ *last_opcode_info_ptr;

    /* Number of cycles we have stolen to the processor last time.  */
    int num_last_stolen_cycles;

    /* Clock tick at which these cycles have been stolen.  */
    CLOCK last_stolen_cycles_clk;

    enum cpu_int global_pending_int;

    void (*nmi_trap_func)(void);

    void (*reset_trap_func)(void);
};
typedef struct cpu_int_status_s cpu_int_status_t;

/* ------------------------------------------------------------------------- */

extern void interrupt_log_wrong_nirq(void);
extern void interrupt_log_wrong_nnmi(void);

extern void interrupt_trigger_dma(cpu_int_status_t *cs, CLOCK cpu_clk);
extern void interrupt_ack_dma(cpu_int_status_t *cs);

/* Set the IRQ line state.  */
inline static void interrupt_set_irq(cpu_int_status_t *cs, int int_num,
                                     int value, CLOCK cpu_clk)
{
    if (value) {                /* Trigger the IRQ.  */
        if (!(cs->pending_int[int_num] & IK_IRQ)) {
            cs->nirq++;
            cs->global_pending_int = (enum cpu_int)
                (cs->global_pending_int | IK_IRQ);
            cs->pending_int[int_num] = (enum cpu_int)
                (cs->pending_int[int_num] | IK_IRQ);

#if 1
            /* This makes sure that IRQ delay is correctly emulated when
               cycles are stolen from the CPU.  */
            if (cs->last_stolen_cycles_clk <= cpu_clk) {
                cs->irq_clk = cpu_clk;
            } else {
                cs->irq_clk = cs->last_stolen_cycles_clk - 1;
            }
#endif
        }
    } else {                    /* Remove the IRQ condition.  */
        if (cs->pending_int[int_num] & IK_IRQ) {
            if (cs->nirq > 0) {
                cs->pending_int[int_num] = (enum cpu_int)
                        (cs->pending_int[int_num] & ~IK_IRQ);
                if (--cs->nirq == 0)
                    cs->global_pending_int = (enum cpu_int)
                        (cs->global_pending_int & ~IK_IRQ);
            } else
                interrupt_log_wrong_nirq();
        }
    }
}

/* Set the NMI line state.  */
inline static void interrupt_set_nmi(cpu_int_status_t *cs, int int_num,
                                     int value, CLOCK cpu_clk)
{
    if (value) {                /* Trigger the NMI.  */
        if (!(cs->pending_int[int_num] & IK_NMI)) {
            if (cs->nnmi == 0 && !(cs->global_pending_int & IK_NMI)) {
                cs->global_pending_int = (enum cpu_int)
                        (cs->global_pending_int | IK_NMI);
#if 1
                /* This makes sure that NMI delay is correctly emulated when
                   cycles are stolen from the CPU.  */
                if (cs->last_stolen_cycles_clk <= cpu_clk) {
                    cs->nmi_clk = cpu_clk;
                } else {
                    cs->nmi_clk = cs->last_stolen_cycles_clk - 1;
                }
#endif
            }
            if (cs->nmi_trap_func)
                cs->nmi_trap_func();
            cs->nnmi++;
            cs->pending_int[int_num] = (enum cpu_int)
                (cs->pending_int[int_num] | IK_NMI);
        }
    } else {                    /* Remove the NMI condition.  */
        if (cs->pending_int[int_num] & IK_NMI) {
            if (cs->nnmi > 0) {
                cs->nnmi--;
                cs->pending_int[int_num] = (enum cpu_int)
                        (cs->pending_int[int_num] & ~IK_NMI);
                if (cpu_clk == cs->nmi_clk)
                    cs->global_pending_int = (enum cpu_int)
                        (cs->global_pending_int & ~IK_NMI);
            } else
                interrupt_log_wrong_nnmi();
        }
    }
}

/* Change the interrupt line state: this can be used to change both NMI
   and IRQ lines.  It is slower than `interrupt_set_nmi()' and
   `interrupt_set_irq()', but is left for backward compatibility (it works
   like the old `setirq()').  */
inline static void interrupt_set_int(cpu_int_status_t *cs, int int_num,
                                     enum cpu_int value, CLOCK cpu_clk)
{
    interrupt_set_nmi(cs, int_num, (int)(value & IK_NMI), cpu_clk);
    interrupt_set_irq(cs, int_num, (int)(value & IK_IRQ), cpu_clk);
}

/* ------------------------------------------------------------------------- */

/* Return the current status of the IRQ, NMI, RESET and TRAP lines.  */
inline static enum cpu_int interrupt_check_pending_interrupt(
    cpu_int_status_t *cs)
{
    return cs->global_pending_int;
}

/* Return nonzero if a pending NMI should be dispatched now.  This takes
   account for the internal delays of the 6510, but does not actually check
   the status of the NMI line.  */
inline static int interrupt_check_nmi_delay(cpu_int_status_t *cs, CLOCK cpu_clk)
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
inline static int interrupt_check_irq_delay(cpu_int_status_t *cs, CLOCK cpu_clk)
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

/* This function must be called by the CPU emulator when a pending NMI
   request is served.  */
inline static void interrupt_ack_nmi(cpu_int_status_t *cs)
{
    cs->global_pending_int = (enum cpu_int)
        (cs->global_pending_int & ~IK_NMI);
}

/* Asynchronously steal `num' cycles from the CPU, starting from cycle
   `start_clk'.  */
inline static void interrupt_steal_cycles(cpu_int_status_t *cs, CLOCK start_clk,
                                          CLOCK *clk_ptr, int num, CLOCK sub)
{
    if (num == 0)
        return;

    if (start_clk == cs->last_stolen_cycles_clk)
        cs->num_last_stolen_cycles += num;
    else
        cs->num_last_stolen_cycles = num;

    *clk_ptr += num;
    cs->last_stolen_cycles_clk = start_clk + num + sub;

    cs->irq_clk += num;
    cs->nmi_clk += num;
}

/* ------------------------------------------------------------------------- */

/* Extern functions.  These are defined in `interrupt.c'.  */

struct snapshot_module_s;

extern void interrupt_trigger_reset(cpu_int_status_t *cs, CLOCK cpu_clk);
extern void interrupt_ack_reset(cpu_int_status_t *cs);
extern void interrupt_set_reset_trap_func(cpu_int_status_t *cs,
                                        void (*reset_trap_func)(void));
extern void interrupt_trigger_trap(cpu_int_status_t *cs,
                                   void (*trap_func)(ADDRESS, void *data),
                                   void *data, CLOCK cpu_clk);
extern void interrupt_do_trap(cpu_int_status_t *cs, ADDRESS address);

extern void interrupt_monitor_trap_on(cpu_int_status_t *cs);
extern void interrupt_monitor_trap_off(cpu_int_status_t *cs);

extern void interrupt_cpu_status_init(cpu_int_status_t *cs, int num_ints,
                                      opcode_info_t *last_opcode_info_ptr);
extern void interrupt_cpu_status_time_warp(cpu_int_status_t *cs,
                                           CLOCK warp_amount,
                                           int warp_direction);

extern int interrupt_read_snapshot(cpu_int_status_t *cs,
                                   struct snapshot_module_s *m);
extern int interrupt_write_snapshot(cpu_int_status_t *cs,
                                    struct snapshot_module_s *m);

extern void interrupt_set_irq_noclk(cpu_int_status_t *cs, int int_num,
                                    int value);
extern void interrupt_set_nmi_noclk(cpu_int_status_t *cs, int int_num,
                                    int value);
extern void interrupt_set_int_noclk(cpu_int_status_t *cs, int int_num,
                          enum cpu_int value);
extern int interrupt_get_irq(cpu_int_status_t *cs, int int_num);
extern int interrupt_get_nmi(cpu_int_status_t *cs, int int_num);
extern void interrupt_set_nmi_trap_func(cpu_int_status_t *cs,
                                        void (*nmi_trap_func)(void));

extern enum cpu_int interrupt_get_int(cpu_int_status_t *cs, int int_num);

/* ------------------------------------------------------------------------- */

extern cpu_int_status_t maincpu_int_status;
extern CLOCK maincpu_clk;

extern cpu_int_status_t *drive0_int_status_ptr;
extern cpu_int_status_t *drive1_int_status_ptr;
extern CLOCK drive_clk[2];

/* For convenience...  */

#define maincpu_set_irq(int_num, value) \
    interrupt_set_irq(&maincpu_int_status, (int_num), (value), maincpu_clk)
#define maincpu_set_irq_clk(int_num, value, clk) \
    interrupt_set_irq(&maincpu_int_status, (int_num), (value), (clk))
#define maincpu_set_nmi(int_num, value) \
    interrupt_set_nmi(&maincpu_int_status, (int_num), (value), maincpu_clk)
#define maincpu_set_nmi_clk(int_num, value, clk) \
    interrupt_set_nmi(&maincpu_int_status, (int_num), (value), (clk))
#define maincpu_set_int(int_num, value) \
    interrupt_set_int(&maincpu_int_status, (int_num), (value), maincpu_clk)
#define maincpu_set_int_clk(int_num, value, clk) \
    interrupt_set_int(&maincpu_int_status, (int_num), (value), (clk))
#define maincpu_set_int_noclk(int_num, value) \
    interrupt_set_int_noclk(&maincpu_int_status, (int_num), (value))
#define maincpu_trigger_reset() \
    interrupt_trigger_reset(&maincpu_int_status, maincpu_clk)
#define maincpu_trigger_dma() \
    interrupt_trigger_dma(&maincpu_int_status, maincpu_clk)
#define maincpu_trigger_trap(trap_func, data) \
    interrupt_trigger_trap(&maincpu_int_status, (trap_func), (data), maincpu_clk)
#define maincpu_steal_cycles(start_clk, num, sub) \
    interrupt_steal_cycles(&maincpu_int_status, (start_clk), &maincpu_clk, (num), (sub))

#define drive0_set_irq(int_num, value) \
    interrupt_set_irq(drive0_int_status_ptr, (int_num), (value), drive_clk[0])
#define drive1_set_irq(int_num, value)    \
    interrupt_set_irq(drive1_int_status_ptr, (int_num), (value), drive_clk[1])
#define drive0_set_irq_clk(int_num, value, clk) \
    interrupt_set_irq(drive0_int_status_ptr, (int_num), (value), (clk))
#define drive1_set_irq_clk(int_num, value, clk) \
    interrupt_set_irq(drive1_int_status_ptr, (int_num), (value), (clk))
#define drive0_set_nmi(int_num, value) \
    interrupt_set_nmi(drive0_int_status_ptr, (int_num), (value), drive_clk[0])
#define drive1_set_nmi(int_num, value) \
    interrupt_set_nmi(drive1_int_status_ptr, (int_num), (value), drive_clk[1])
#define drive0_set_nmi_clk(int_num, value, clk) \
    interrupt_set_nmi(drive0_int_status_ptr, (int_num), (value), (clk))
#define drive1_set_nmi_clk(int_num, value, clk) \
    interrupt_set_nmi(drive1_int_status_ptr, (int_num), (value), (clk))
#define drive0_set_int(int_num, value) \
    interrupt_set_int(drive0_int_status_ptr, (int_num), (value), drive_clk[0])
#define drive1_set_int(int_num, value) \
    interrupt_set_int(drive1_int_status_ptr, (int_num), (value), drive_clk[1])
#define drive0_set_int_clk(int_num, value, clk) \
    interrupt_set_int_clk(drive0_int_status_ptr, (int_num), (value), (clk))
#define drive1_set_int_clk(int_num, value, clk) \
    interrupt_set_int_clk(drive1_int_status_ptr, (int_num), (value), (clk))
#define drive0_trigger_reset() \
    interrupt_trigger_reset(drive0_int_status_ptr, drive_clk[0] + 1)
#define drive1_trigger_reset() \
    interrupt_trigger_reset(drive1_int_status_ptr, drive_clk[1] + 1)
#define drive0_trigger_trap(trap_func, data) \
    interrupt_trigger_trap(drive0_int_status_ptr, (trap_func), (data), \
                           drive_clk[0] + 1)
#define drive1_trigger_trap(trap_func, data) \
    interrupt_trigger_trap(drive1_int_status_ptr, (trap_func), (data), \
                           drive_clk[1] + 1)

#endif

