/*
 * scpu64cpu.c - Emulation of the main 65816 processor.
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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

/* NULL */
#include <stdio.h>

#include "vice.h"

#include "interrupt.h"
#include "6510core.h"
#include "clkguard.h"
#include "alarm.h"
#include "main65816cpu.h"
#include "mem.h"
#include "scpu64mem.h"
#include "vicii.h"
#include "viciitypes.h"

/* ------------------------------------------------------------------------- */

/* MACHINE_STUFF should define/undef

 - NEED_REG_PC
 - TRACE

 The following are optional:

 - PAGE_ZERO
 - PAGE_ONE
 - STORE_IND
 - LOAD_IND
 - DMA_FUNC
 - DMA_ON_RESET

*/

/* ------------------------------------------------------------------------- */
#define CYCLE_EXACT_ALARM

static int fastmode = 1;
static CLOCK buffer_finish, buffer_finish_half;
static CLOCK maincpu_diff, maincpu_accu;
int scpu64_emulation_mode;
#ifdef CYCLE_EXACT_ALARM
alarm_context_t *maincpu_alarm_context = NULL;
#endif

int scpu64_get_half_cycle(void)
{
    if (fastmode) {
        return maincpu_accu / 1000000;
    }
    return -1;
}

static inline void alarms(void)
{
#ifdef CYCLE_EXACT_ALARM
    while (maincpu_clk >= alarm_context_next_pending_clk(maincpu_alarm_context)) {
        alarm_context_dispatch(maincpu_alarm_context, maincpu_clk);
    }
#endif
}

static inline void scpu64_clock_add(int amount, int write)
{
    if (fastmode) {
        maincpu_accu += maincpu_diff * amount;
        while (maincpu_accu > 20000000) {
            maincpu_accu -= 20000000;
            /* no alarm processing if in emulation mode and doing a write, this is for VICII */
            if (!scpu64_emulation_mode || !write) {
                alarms();
            }
            maincpu_clk++;
        }
    } else {
        /* no alarm processing if in emulation mode and doing a write, this is for VICII */
        if (!scpu64_emulation_mode || !write) {
            while (amount) {
                amount--;
                alarms();
                maincpu_clk++;
            }
        } else {
            maincpu_clk += amount;
        }
    }
}

static inline void wait_buffer(void)
{
    if (buffer_finish > maincpu_clk || (buffer_finish == maincpu_clk && buffer_finish_half > maincpu_accu)) {
        maincpu_accu = buffer_finish_half;
        alarms();
        maincpu_clk = buffer_finish;
    }
}

void scpu64_clock_readwrite_stretch_eprom(void)
{
    if (fastmode) {
        maincpu_accu += maincpu_diff * 3;
        if (maincpu_accu > 20000000) {
            maincpu_accu -= 20000000;
            alarms();
            maincpu_clk++;
        }
    }
}

void scpu64_clock_read_stretch_io(void)
{
    if (fastmode) {
        wait_buffer();
        while (maincpu_accu < 31730000) { /* measured */
            maincpu_accu += maincpu_diff;
        }
        maincpu_accu -= 20600000;
        alarms();
        maincpu_clk++;
    }
}

void scpu64_clock_write_stretch_io_cia(void)
{
    if (fastmode) {
        wait_buffer();
        while (maincpu_accu < 35950000) { /* measured */
            maincpu_accu += maincpu_diff;
        }
        maincpu_accu -= 20000000;
        alarms();
        maincpu_clk++;
    }
}

void scpu64_clock_write_stretch_io(void)
{
    if (fastmode) {
        wait_buffer();
        while (maincpu_accu < 31730000) { /* measured */
            maincpu_accu += maincpu_diff;
        }
        maincpu_accu -= 20600000;
        alarms();
        maincpu_clk++;
    }
}

void scpu64_clock_write_stretch_io_long(void)
{
    if (fastmode) {
        wait_buffer();
        while (maincpu_accu < 38000000) { /* measured */
            maincpu_accu += maincpu_diff;
        }
        maincpu_accu -= 20600000;
        alarms();
        maincpu_clk++;
    }
}

void scpu64_clock_write_stretch(void)
{
    if (fastmode) {
        wait_buffer();
        buffer_finish = maincpu_clk + 1;
        buffer_finish_half = 13000000;
    }
}

void scpu64_set_fastmode(int mode)
{
    if (fastmode != mode) {
        if (!mode) {
            if (maincpu_accu > 17700000) {
                alarms();
                maincpu_clk++;
            }
            maincpu_accu = 17700000; /* measured */
            alarms();
            maincpu_clk++;
        }
        fastmode = mode;
        maincpu_resync_limits();
    }
}

int scpu64_get_fastmode(void)
{
    return fastmode;
}


/* TODO: refresh */
static DWORD simm_cell;
static DWORD simm_row_mask = ~(2048 * 4 - 1);

void scpu64_set_simm_row_size(int value)
{
    simm_row_mask = ~((1 << value) - 1);
}

void scpu64_clock_read_stretch_simm(DWORD addr)
{
    if (fastmode) {
        if (!((simm_cell ^ addr) & ~3)) {
            return; /* same cell, no delay */
        } else if ((simm_cell ^ addr) & simm_row_mask) {
            /* different row, two and half delay */
            maincpu_accu += maincpu_diff * 2 + (maincpu_diff >> 1);
        } else if (!(((simm_cell + 4) ^ addr) & ~3)) {
            simm_cell = addr;
            return; /* next cell, no delay */
        } else {
            maincpu_accu += maincpu_diff;/* same row, one delay */
        }
        simm_cell = addr;
        if (maincpu_accu > 20000000) {
            maincpu_accu -= 20000000;
            alarms();
            maincpu_clk++;
        }
    }
}

void scpu64_clock_write_stretch_simm(DWORD addr)
{
    if (fastmode) {
        if ((simm_cell ^ addr) & simm_row_mask) {
            maincpu_accu += maincpu_diff * 2;/* different row, two delay */
        } else {
            maincpu_accu += maincpu_diff;/* same row, one delay */
        }
        simm_cell = addr;
        if (maincpu_accu > 20000000) {
            maincpu_accu -= 20000000;
            alarms();
            maincpu_clk++;
        }
    }
}

static void clk_overflow_callback(CLOCK sub, void *unused_data)
{
    if (buffer_finish > sub) {
        buffer_finish -= sub;
    } else {
        buffer_finish = 0;
    }
}

#define CPU_ADDITIONAL_INIT() clk_guard_add_callback(maincpu_clk_guard, clk_overflow_callback, NULL)

/* SCPU64 needs external reg_pc */
#define NEED_REG_PC

#define STORE(addr, value) \
    do { \
        DWORD tmpx1 = (addr); \
        BYTE tmpx2 = (value); \
        if (tmpx1 & ~0xffff) { \
            mem_store2(tmpx1, tmpx2); \
        } else { \
            (*_mem_write_tab_ptr[tmpx1 >> 8])((WORD)tmpx1, tmpx2); \
        } \
    } while (0)

#define LOAD(addr) \
    (((addr) & ~0xffff)?mem_read2(addr):(*_mem_read_tab_ptr[(addr) >> 8])((WORD)(addr)))

#define STORE_LONG(addr, value) store_long(addr, value)

static inline void store_long(DWORD addr, BYTE value)
{
    if (addr & ~0xffff) {
        mem_store2(addr, value);
    } else {
        (*_mem_write_tab_ptr[addr >> 8])((WORD)addr, value);
    }
    scpu64_clock_add(1, 1);
}

#define LOAD_LONG(addr) load_long(addr)

static inline BYTE load_long(DWORD addr)
{
    BYTE tmp;

    if ((addr) & ~0xffff) {
        tmp = mem_read2(addr);
    } else {
        tmp = (*_mem_read_tab_ptr[(addr) >> 8])((WORD)addr);
    }
    scpu64_clock_add(1, 0);
    return tmp;
}

#define CUSTOM_INTERRUPT_DELAY
/* Return nonzero if a pending NMI should be dispatched now.  This takes
   account for the internal delays of the 65SC02, but does not actually check
   the status of the NMI line.  */
inline static int interrupt_check_nmi_delay(interrupt_cpu_status_t *cs,
                                            CLOCK cpu_clk)
{
    CLOCK nmi_clk;

    if (!fastmode) {
        nmi_clk = cs->nmi_clk + INTERRUPT_DELAY;

        /* Branch instructions delay IRQs and NMI by one cycle if branch
           is taken with no page boundary crossing.  */
        if (OPINFO_DELAYS_INTERRUPT(*cs->last_opcode_info_ptr)) {
            nmi_clk++;
        }
    } else {
        nmi_clk = cs->nmi_clk;
    }

    if (cpu_clk >= nmi_clk) {
        return 1;
    }

    return 0;
}

/* Return nonzero if a pending IRQ should be dispatched now.  This takes
   account for the internal delays of the 65802, but does not actually check
   the status of the IRQ line.  */
inline static int interrupt_check_irq_delay(interrupt_cpu_status_t *cs,
                                            CLOCK cpu_clk)
{
    CLOCK irq_clk;

    if (!fastmode) {
        irq_clk = cs->irq_clk + INTERRUPT_DELAY;

        /* Branch instructions delay IRQs and NMI by one cycle if branch
           is taken with no page boundary crossing.  */
        if (OPINFO_DELAYS_INTERRUPT(*cs->last_opcode_info_ptr)) {
            irq_clk++;
        }

        /* If an opcode changes the I flag from 1 to 0, the 65802 needs
           one more opcode before it triggers the IRQ routine.  */
    } else {
        irq_clk = cs->irq_clk;
    }
    if (cpu_clk >= irq_clk) {
        if (!OPINFO_ENABLES_IRQ(*cs->last_opcode_info_ptr)) {
            return 1;
        } else {
            cs->global_pending_int |= IK_IRQPEND;
        }
    }
    return 0;
}

#define EMULATION_MODE_CHANGED scpu64_emulation_mode = reg_emul

#define CLK_ADD(clock, amount) scpu64_clock_add(amount, 0)

#define CPU_ADDITIONAL_RESET() (buffer_finish = maincpu_clk, buffer_finish_half = 0, maincpu_accu = 0, maincpu_diff = machine_get_cycles_per_second())

#define FETCH_PARAM_DUMMY(addr) scpu64_clock_add(1, 0)
#define LOAD_LONG_DUMMY(addr) scpu64_clock_add(1, 0)

#define LOAD_INT_ADDR(addr)                        \
    if (scpu64_interrupt_reroute()) {              \
        reg_pc = LOAD_LONG(addr + 0xf80000);       \
        reg_pc |= LOAD_LONG(addr + 0xf80001) << 8; \
    } else {                                       \
        reg_pc = LOAD_LONG(addr);                  \
        reg_pc |= LOAD_LONG(addr + 1) << 8;        \
    }
/* ------------------------------------------------------------------------- */

#include "../main65816cpu.c"
