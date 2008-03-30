/*
 * c128cpu.c - Emulation of the main 6510 processor.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "vice.h"

#include "mem.h"
#include "z80.h"

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

/* C128 needs external reg_pc */
#define NEED_REG_PC

/* Put Z80 registers into monitor sturct.  */
#define HAVE_Z80_REGS

/* ------------------------------------------------------------------------- */

extern read_func_ptr_t _mem_read_tab[];
extern store_func_ptr_t _mem_write_tab[];

#define PAGE_ZERO mem_page_zero

#define PAGE_ONE mem_page_one

#define LOAD_ZERO(addr) \
    read_zero(addr)

#define DMA_FUNC \
    z80_mainloop(CPU_INT_STATUS, ALARM_CONTEXT)

#define DMA_ON_RESET \
                EXPORT_REGISTERS();                \
                DMA_FUNC;                          \
                interrupt_ack_dma(CPU_INT_STATUS); \
                IMPORT_REGISTERS();                \
                JUMP(LOAD_ADDR(0xfffc));

extern void vic_ii_delay_clk(void);

#define CPU_DELAY_CLK vic_ii_delay_clk();

#include "../maincpu.c"

