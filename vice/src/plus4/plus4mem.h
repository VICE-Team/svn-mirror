/*
 * plus4mem.h -- Plus4 memory handling.
 *
 * Written by
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

#ifndef _PLUS4MEM_H
#define _PLUS4MEM_H

#include "types.h"

#define PLUS4_RAM_SIZE        0x10000
#define PLUS4_BASIC_ROM_SIZE  0x4000
#define PLUS4_KERNAL_ROM_SIZE 0x4000
#define PLUS4_CART8K_SIZE     0x2000
#define PLUS4_CART16K_SIZE    0x4000

extern BYTE mem_basic_rom[];
extern BYTE mem_kernal_rom[];
extern BYTE extromlo1[PLUS4_BASIC_ROM_SIZE];
extern BYTE extromlo2[PLUS4_BASIC_ROM_SIZE];
extern BYTE extromlo3[PLUS4_BASIC_ROM_SIZE];
extern BYTE extromhi1[PLUS4_KERNAL_ROM_SIZE];
extern BYTE extromhi2[PLUS4_KERNAL_ROM_SIZE];
extern BYTE extromhi3[PLUS4_KERNAL_ROM_SIZE];

extern BYTE REGPARM1 kernal_read(WORD addr);
extern void REGPARM2 kernal_store(WORD addr, BYTE value);

extern int plus4_mem_init_resources(void);
extern int plus4_mem_init_cmdline_options(void);

extern void mem_config_ram_set(unsigned int config);
extern BYTE *mem_get_tedmem_base(unsigned int segment);

extern void mem_proc_port_trigger_flux_change(unsigned int on);
extern void pio1_set_tape_sense(int sense);

#endif /* _PLUS4MEM_H */

