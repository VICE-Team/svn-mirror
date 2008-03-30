
/*
 * c128cpu.c - Emulation of the main 6510 processor.
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

#include "mem.h"

/* ------------------------------------------------------------------------- */

/* MACHINE_STUFF should define/undef

 - NEED_REG_PC
 - NO_OPCODES
 - TRACE

 The following are optional:

 - PAGE_ZERO
 - PAGE_ONE
 - STORE_IND
 - LOAD_IND

*/

/* ------------------------------------------------------------------------- */

#define	NO_INSTRUCTION_FETCH_HACK

/* C128 needs external reg_pc */
#  define	NEED_REG_PC

/* Do not include include this function when `_mem_read_base_tab_ptr' is 
   not used.  Otherwise the native IRIX 6.2 compiler barfs.  */
#define DO_NOT_INCLUDE_MEM_READ_BASE 1

/* ------------------------------------------------------------------------- */

extern read_func_ptr_t _mem_read_tab[];
extern store_func_ptr_t _mem_write_tab[];

#  define	JUMP(addr)	(reg_pc = (addr))

#  define	PAGE_ZERO	page_zero

#  define	PAGE_ONE	page_one

#define STORE(addr, value) \
    (_mem_write_tab[(addr) >> 8])((addr), (value))

#define LOAD(addr) \
    (_mem_read_tab[(addr) >> 8])((addr))

#define STORE_ZERO(addr, value) \
    page_zero[(addr) & 0xff] = (value);

#define LOAD_ZERO(addr) \
    page_zero[(addr) & 0xff]


#include "../maincpu.c"

