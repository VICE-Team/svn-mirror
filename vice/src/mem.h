/*
 * mem.h - Memory interface.
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _MEM_H_
#define _MEM_H_

#include "types.h"

typedef BYTE REGPARM1 read_func_t(ADDRESS addr);
typedef read_func_t *read_func_ptr_t;
typedef void REGPARM2 store_func_t(ADDRESS addr, BYTE value);
typedef store_func_t *store_func_ptr_t;

extern read_func_ptr_t *_mem_read_tab_ptr;
extern store_func_ptr_t *_mem_write_tab_ptr;
extern BYTE **_mem_read_base_tab_ptr;

extern BYTE ram[];
extern int ram_size;
extern BYTE chargen_rom[];
extern int rom_loaded;		/* FIXME: ugly! */

extern void initialize_memory(void);
extern void mem_powerup(void);
extern int mem_load(void);
extern void mem_attach_cartridge(int type, BYTE *rawcart);
extern void mem_detach_cartridge(int type);
extern void mem_freeze_cartridge(int type);
extern void mem_get_basic_text(ADDRESS *start, ADDRESS *end);
extern void mem_set_basic_text(ADDRESS start, ADDRESS end);
extern void mem_set_tape_sense(int value);
extern void mem_toggle_watchpoints(int flag);
extern int mem_rom_trap_allowed(ADDRESS addr);

extern read_func_t read_rom, read_zero;
extern store_func_t store_rom, store_zero;

extern read_func_t mem_read;
extern store_func_t mem_store;

/* ------------------------------------------------------------------------- */

/* Memory access macros.  */

/* We don't want "normal" modules to use them: they should use the
   higher-level function instead.  The CPU code does need them for speed,
   though.  */

#ifdef _MAINCPU_C

#define STORE(addr, value) \
    (*_mem_write_tab_ptr[(addr) >> 8])((addr), (value))

#define LOAD(addr) \
    (*_mem_read_tab_ptr[(addr) >> 8])((addr))

#define STORE_ZERO(addr, value) \
    store_zero((addr), (value))

#define LOAD_ZERO(addr) \
    ram[(addr) & 0xff]

#define LOAD_ADDR(addr) \
    ((LOAD((addr) + 1) << 8) | LOAD(addr))

#define LOAD_ZERO_ADDR(addr) \
    ((LOAD_ZERO((addr) + 1) << 8) | LOAD_ZERO(addr))

inline static BYTE *mem_read_base(int addr)
{
    BYTE *p = _mem_read_base_tab_ptr[addr >> 8];

    if (p == 0)
	return p;

    return p - (addr & 0xff00);
}

#endif

#endif
