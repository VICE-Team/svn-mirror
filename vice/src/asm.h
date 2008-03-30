/*
 * asm.h - Assembler-related utility functions.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Based on older code by
 *  Vesa-Matti Puro (vmp@lut.fi)
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta (jopi@stekt.oulu.fi)
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

#ifndef _ASM_H
#define _ASM_H

#include "types.h"

enum asm_addr_mode {
    ASM_ADDR_MODE_IMPLIED,
    ASM_ADDR_MODE_ACCUMULATOR,
    ASM_ADDR_MODE_IMMEDIATE,
    ASM_ADDR_MODE_ZERO_PAGE,
    ASM_ADDR_MODE_ZERO_PAGE_X,
    ASM_ADDR_MODE_ZERO_PAGE_Y,
    ASM_ADDR_MODE_ABSOLUTE,
    ASM_ADDR_MODE_ABSOLUTE_X,
    ASM_ADDR_MODE_ABSOLUTE_Y,
    ASM_ADDR_MODE_ABS_INDIRECT,
    ASM_ADDR_MODE_INDIRECT_X,
    ASM_ADDR_MODE_INDIRECT_Y,
    ASM_ADDR_MODE_RELATIVE
};
typedef enum asm_addr_mode asm_addr_mode_t;

struct asm_opcode_info {
    const char *mnemonic;
    asm_addr_mode_t addr_mode;
};
typedef struct asm_opcode_info asm_opcode_info_t;

asm_opcode_info_t *asm_opcode_info_get(BYTE number);
unsigned int asm_addr_mode_get_size(asm_addr_mode_t mode);

#endif  /* _ASM_H */
