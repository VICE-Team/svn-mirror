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

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "types.h"

#include "snapshot.h"

typedef BYTE REGPARM1 read_func_t(ADDRESS addr);
typedef read_func_t *read_func_ptr_t;
typedef void REGPARM2 store_func_t(ADDRESS addr, BYTE value);
typedef store_func_t *store_func_ptr_t;

extern read_func_ptr_t *_mem_read_tab_ptr;
extern store_func_ptr_t *_mem_write_tab_ptr;
extern BYTE **_mem_read_base_tab_ptr;
extern int *mem_read_limit_tab_ptr;

#ifdef AVOID_STATIC_ARRAYS
extern BYTE *ram;
#else
extern BYTE ram[];
#endif
extern int ram_size;
#ifdef AVOID_STATIC_ARRAYS
extern BYTE *chargen_rom;
#else
extern BYTE chargen_rom[];
#endif
extern BYTE *page_zero;
extern BYTE *page_one;

extern unsigned int old_reg_pc;

extern const char *mem_romset_resources_list[];

extern void initialize_memory(void);
extern void mem_powerup(void);
extern int mem_load(void);
extern void mem_get_basic_text(ADDRESS * start, ADDRESS * end);
extern void mem_set_basic_text(ADDRESS start, ADDRESS end);
extern void mem_set_tape_sense(int value);
extern void mem_toggle_watchpoints(int flag);
extern int mem_rom_trap_allowed(ADDRESS addr);
extern void mem_set_bank_pointer(BYTE **base, int *limit);

extern read_func_t read_rom, read_zero;
extern store_func_t store_rom, store_zero;

extern read_func_t mem_read;
extern store_func_t mem_store;

/* ------------------------------------------------------------------------- */

/* Banked memory access functions for the monitor.  */

extern const char **mem_bank_list(void);
extern int mem_bank_from_name(const char *name);
extern BYTE mem_bank_read(int bank, ADDRESS addr);
extern BYTE mem_bank_peek(int bank, ADDRESS addr);
extern void mem_bank_write(int bank, ADDRESS addr, BYTE byte);

/* Snapshots.  */

extern int mem_write_snapshot_module(snapshot_t *s, int save_roms);
extern int mem_read_snapshot_module(snapshot_t *s);

#endif
