/*
 * asm.h - Assembler-related utility functions.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on older code by
 *  Vesa-Matti Puro <vmp@lut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
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

#ifndef VICE_ASM_H
#define VICE_ASM_H

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
    ASM_ADDR_MODE_RELATIVE,
    ASM_ADDR_MODE_ABSOLUTE_A,
    ASM_ADDR_MODE_ABSOLUTE_HL,
    ASM_ADDR_MODE_ABSOLUTE_IX,
    ASM_ADDR_MODE_ABSOLUTE_IY,
    ASM_ADDR_MODE_ABS_INDIRECT_ZP,
    ASM_ADDR_MODE_IMMEDIATE_16,
    ASM_ADDR_MODE_REG_B,
    ASM_ADDR_MODE_REG_C,
    ASM_ADDR_MODE_REG_D,
    ASM_ADDR_MODE_REG_E,
    ASM_ADDR_MODE_REG_H,
    ASM_ADDR_MODE_REG_IXH,
    ASM_ADDR_MODE_REG_IYH,
    ASM_ADDR_MODE_REG_L,
    ASM_ADDR_MODE_REG_IXL,
    ASM_ADDR_MODE_REG_IYL,
    ASM_ADDR_MODE_REG_AF,
    ASM_ADDR_MODE_REG_BC,
    ASM_ADDR_MODE_REG_DE,
    ASM_ADDR_MODE_REG_HL,
    ASM_ADDR_MODE_REG_IX,
    ASM_ADDR_MODE_REG_IY,
    ASM_ADDR_MODE_REG_SP,
    ASM_ADDR_MODE_REG_IND_BC,
    ASM_ADDR_MODE_REG_IND_DE,
    ASM_ADDR_MODE_REG_IND_HL,
    ASM_ADDR_MODE_REG_IND_IX,
    ASM_ADDR_MODE_REG_IND_IY,
    ASM_ADDR_MODE_REG_IND_SP
};
typedef enum asm_addr_mode asm_addr_mode_t;

struct asm_opcode_info_s {
    const char *mnemonic;
    asm_addr_mode_t addr_mode;
};
typedef struct asm_opcode_info_s asm_opcode_info_t;

struct monitor_cpu_type_s;

#endif

