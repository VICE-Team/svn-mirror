/*
 * petmem.h - PET memory handling.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _PETMEM_H
#define _PETMEM_H

#include "types.h"

#define RAM_SIZE		0x8000
#define ROM_SIZE		0x8000
#define CHARGEN_ROM_SIZE	0x1000

#define	PET2001_CHECKSUM	55094
#define	PET3032_CHECKSUM	31319
#define	PET4032_CHECKSUM	1643
#define	PET8032_CHECKSUM_A	8647	/* my kernel (a.fachat) */
#define	PET8032_CHECKSUM_B	8227	/* jouko's kernel */

typedef BYTE REGPARM1 read_func_t(ADDRESS addr);
typedef read_func_t *read_func_ptr_t;
typedef void REGPARM2 store_func_t(ADDRESS addr, BYTE value);
typedef store_func_t *store_func_ptr_t;

extern read_func_ptr_t _mem_read_tab[0x101];
extern store_func_ptr_t _mem_write_tab[0x101];
extern BYTE *_mem_read_base_tab[0x101];

extern BYTE ram[];
extern BYTE rom[];
extern BYTE chargen_rom[CHARGEN_ROM_SIZE];

extern void initialize_memory(void);
extern void mem_powerup(void);
extern int mem_load(void);
extern void mem_get_basic_text(ADDRESS *start, ADDRESS *end);
extern void mem_set_basic_text(ADDRESS start, ADDRESS end);

extern int rom_loaded;

extern read_func_t read_zero, read_ram, read_rom, read_io;
extern store_func_t store_zero, store_ram, store_rom, store_io;

/* ------------------------------------------------------------------------- */

#define STORE(addr, value)  (*_mem_write_tab[(addr) >> 8])((addr), (value))

#define LOAD(addr)	    (*_mem_read_tab[(addr) >> 8])((addr))

#define STORE_ZERO(addr, value)	store_zero((addr), (value))
#define LOAD_ZERO(addr)		ram[(addr) & 0xff]
#define LOAD_ADDR(addr)		((LOAD((addr) + 1) << 8) | LOAD(addr))
#define LOAD_ZERO_ADDR(addr)	((LOAD_ZERO((addr) + 1) << 8) | LOAD_ZERO(addr))

inline static BYTE *mem_read_base(int addr)
{
    BYTE *p = _mem_read_base_tab[addr >> 8];

    if (p == 0)
	return p;

    return p - (addr & 0xff00);
}

#endif
