/*
 * asmz80.c - Z80 Assembler-related utility functions.
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

#include "vice.h"

#include "asm.h"
#include "mon_assemble.h"
#include "mon_register.h"
#include "montypes.h"
#include "types.h"

static const int addr_mode_size[] = {
    /* 6502 */
    1, /* ASM_ADDR_MODE_IMPLIED */
    1, /* ASM_ADDR_MODE_ACCUMULATOR */
    2, /* ASM_ADDR_MODE_IMMEDIATE */
    2, /* ASM_ADDR_MODE_ZERO_PAGE */        /* INA $12 */
    2, /* ASM_ADDR_MODE_ZERO_PAGE_X */
    2, /* ASM_ADDR_MODE_ZERO_PAGE_Y */
    3, /* ASM_ADDR_MODE_ABSOLUTE */
    3, /* ASM_ADDR_MODE_ABSOLUTE_X */
    3, /* ASM_ADDR_MODE_ABSOLUTE_Y */
    3, /* ASM_ADDR_MODE_ABS_INDIRECT */     /* LD IX, ($1234) */
    2, /* ASM_ADDR_MODE_INDIRECT_X */
    2, /* ASM_ADDR_MODE_INDIRECT_Y */
    2, /* ASM_ADDR_MODE_RELATIVE */
    /* z80 */
    3, /* ASM_ADDR_MODE_ABSOLUTE_A */
    3, /* ASM_ADDR_MODE_ABSOLUTE_HL */      /* LD ($1234), HL */
    3, /* ASM_ADDR_MODE_ABSOLUTE_IX */
    3, /* ASM_ADDR_MODE_ABSOLUTE_IY */
    3, /* ASM_ADDR_MODE_Z80_ABSOLUTE_BC */
    3, /* ASM_ADDR_MODE_Z80_ABSOLUTE_DE */
    3, /* ASM_ADDR_MODE_Z80_ABSOLUTE_SP */
    2, /* ASM_ADDR_MODE_ABS_INDIRECT_ZP */
    3, /* ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT */
    3, /* ASM_ADDR_MODE_IMMEDIATE_16 */
    1, /* ASM_ADDR_MODE_REG_B */
    1, /* ASM_ADDR_MODE_REG_C */
    1, /* ASM_ADDR_MODE_REG_D */
    1, /* ASM_ADDR_MODE_REG_E */
    1, /* ASM_ADDR_MODE_REG_H */
    1, /* ASM_ADDR_MODE_REG_IXH */
    1, /* ASM_ADDR_MODE_REG_IYH */
    1, /* ASM_ADDR_MODE_REG_L */
    1, /* ASM_ADDR_MODE_REG_IXL */
    1, /* ASM_ADDR_MODE_REG_IYL */
    1, /* ASM_ADDR_MODE_REG_AF */
    1, /* ASM_ADDR_MODE_REG_BC */
    1, /* ASM_ADDR_MODE_REG_DE */
    1, /* ASM_ADDR_MODE_REG_HL */
    1, /* ASM_ADDR_MODE_REG_IX */
    1, /* ASM_ADDR_MODE_REG_IY */
    1, /* ASM_ADDR_MODE_REG_SP */
    1, /* ASM_ADDR_MODE_REG_IND_BC */
    1, /* ASM_ADDR_MODE_REG_IND_DE */
    1, /* ASM_ADDR_MODE_REG_IND_HL */
    2, /* ASM_ADDR_MODE_REG_IND_IX */
    2, /* ASM_ADDR_MODE_REG_IND_IY */
    1, /* ASM_ADDR_MODE_REG_IND_SP */
    3, /* ASM_ADDR_MODE_Z80_IND_IMMEDIATE */    /* LD (IX+$12), #$34 */
    2, /* ASM_ADDR_MODE_Z80_IND_REG */
};

/* Z80 instruction set table: https://clrhome.org/table/ */

static const asm_opcode_info_t opcode_list[] = {
    /* 00 */ { "NOP",        ASM_ADDR_MODE_IMPLIED },
    /* 01 */ { "LD BC,",     ASM_ADDR_MODE_IMMEDIATE_16 },
    /* 02 */ { "LD (BC),A",  ASM_ADDR_MODE_IMPLIED },
    /* 03 */ { "INC",        ASM_ADDR_MODE_REG_BC },
    /* 04 */ { "INC",        ASM_ADDR_MODE_REG_B },
    /* 05 */ { "DEC",        ASM_ADDR_MODE_REG_B },
    /* 06 */ { "LD B,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 07 */ { "RLCA",       ASM_ADDR_MODE_IMPLIED },
    /* 08 */ { "EX AF,AF'",  ASM_ADDR_MODE_IMPLIED },
    /* 09 */ { "ADD HL,BC",  ASM_ADDR_MODE_IMPLIED },
    /* 0a */ { "LD A,(BC)",  ASM_ADDR_MODE_IMPLIED },
    /* 0b */ { "DEC",        ASM_ADDR_MODE_REG_BC },
    /* 0c */ { "INC",        ASM_ADDR_MODE_REG_C },
    /* 0d */ { "DEC",        ASM_ADDR_MODE_REG_C },
    /* 0e */ { "LD C,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 0f */ { "RRCA",       ASM_ADDR_MODE_IMPLIED },
    /* 10 */ { "DJNZ",       ASM_ADDR_MODE_RELATIVE },
    /* 11 */ { "LD DE,",     ASM_ADDR_MODE_IMMEDIATE_16 },
    /* 12 */ { "LD (DE),A",  ASM_ADDR_MODE_IMPLIED },
    /* 13 */ { "INC",        ASM_ADDR_MODE_REG_DE },
    /* 14 */ { "INC",        ASM_ADDR_MODE_REG_D },
    /* 15 */ { "DEC",        ASM_ADDR_MODE_REG_D },
    /* 16 */ { "LD D,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 17 */ { "RLA",        ASM_ADDR_MODE_IMPLIED },
    /* 18 */ { "JR",         ASM_ADDR_MODE_RELATIVE },
    /* 19 */ { "ADD HL,DE",  ASM_ADDR_MODE_IMPLIED },
    /* 1a */ { "LD A,(DE)",  ASM_ADDR_MODE_IMPLIED },
    /* 1b */ { "DEC",        ASM_ADDR_MODE_REG_DE },
    /* 1c */ { "INC",        ASM_ADDR_MODE_REG_E },
    /* 1d */ { "DEC",        ASM_ADDR_MODE_REG_E },
    /* 1e */ { "LD E,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 1f */ { "RRA",        ASM_ADDR_MODE_IMPLIED },
    /* 20 */ { "JR NZ,",     ASM_ADDR_MODE_RELATIVE },
    /* 21 */ { "LD HL,",     ASM_ADDR_MODE_IMMEDIATE_16 },
    /* 22 */ { "LD",         ASM_ADDR_MODE_ABSOLUTE_HL },
    /* 23 */ { "INC",        ASM_ADDR_MODE_REG_HL },
    /* 24 */ { "INC",        ASM_ADDR_MODE_REG_H },
    /* 25 */ { "DEC",        ASM_ADDR_MODE_REG_H },
    /* 26 */ { "LD H,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 27 */ { "DAA",        ASM_ADDR_MODE_IMPLIED },
    /* 28 */ { "JR Z,",      ASM_ADDR_MODE_RELATIVE },
    /* 29 */ { "ADD HL,HL",  ASM_ADDR_MODE_IMPLIED },
    /* 2a */ { "LD HL,",     ASM_ADDR_MODE_ABS_INDIRECT },
    /* 2b */ { "DEC",        ASM_ADDR_MODE_REG_HL },
    /* 2c */ { "INC",        ASM_ADDR_MODE_REG_L },
    /* 2d */ { "DEC",        ASM_ADDR_MODE_REG_L },
    /* 2e */ { "LD L,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 2f */ { "CPL",        ASM_ADDR_MODE_IMPLIED },
    /* 30 */ { "JR NC,",     ASM_ADDR_MODE_RELATIVE },
    /* 31 */ { "LD SP,",     ASM_ADDR_MODE_IMMEDIATE_16 },
    /* 32 */ { "LD",         ASM_ADDR_MODE_ABSOLUTE_A },
    /* 33 */ { "INC",        ASM_ADDR_MODE_REG_SP },
    /* 34 */ { "INC",        ASM_ADDR_MODE_REG_IND_HL },
    /* 35 */ { "DEC",        ASM_ADDR_MODE_REG_IND_HL },
    /* 36 */ { "LD (HL),",   ASM_ADDR_MODE_IMMEDIATE },
    /* 37 */ { "SCF",        ASM_ADDR_MODE_IMPLIED },
    /* 38 */ { "JR C,",      ASM_ADDR_MODE_RELATIVE },
    /* 39 */ { "ADD HL,SP",  ASM_ADDR_MODE_IMPLIED },
    /* 3a */ { "LD A,",      ASM_ADDR_MODE_ABS_INDIRECT },
    /* 3b */ { "DEC",        ASM_ADDR_MODE_REG_SP },
    /* 3c */ { "INC",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 3d */ { "DEC",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 3e */ { "LD A,",      ASM_ADDR_MODE_IMMEDIATE },
    /* 3f */ { "CCF",        ASM_ADDR_MODE_IMPLIED },
    /* 40 */ { "LD B,B",     ASM_ADDR_MODE_IMPLIED },
    /* 41 */ { "LD B,C",     ASM_ADDR_MODE_IMPLIED },
    /* 42 */ { "LD B,D",     ASM_ADDR_MODE_IMPLIED },
    /* 43 */ { "LD B,E",     ASM_ADDR_MODE_IMPLIED },
    /* 44 */ { "LD B,H",     ASM_ADDR_MODE_IMPLIED },
    /* 45 */ { "LD B,L",     ASM_ADDR_MODE_IMPLIED },
    /* 46 */ { "LD B,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 47 */ { "LD B,A",     ASM_ADDR_MODE_IMPLIED },
    /* 48 */ { "LD C,B",     ASM_ADDR_MODE_IMPLIED },
    /* 49 */ { "LD C,C",     ASM_ADDR_MODE_IMPLIED },
    /* 4a */ { "LD C,D",     ASM_ADDR_MODE_IMPLIED },
    /* 4b */ { "LD C,E",     ASM_ADDR_MODE_IMPLIED },
    /* 4c */ { "LD C,H",     ASM_ADDR_MODE_IMPLIED },
    /* 4d */ { "LD C,L",     ASM_ADDR_MODE_IMPLIED },
    /* 4e */ { "LD C,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 4f */ { "LD C,A",     ASM_ADDR_MODE_IMPLIED },
    /* 50 */ { "LD D,B",     ASM_ADDR_MODE_IMPLIED },
    /* 51 */ { "LD D,C",     ASM_ADDR_MODE_IMPLIED },
    /* 52 */ { "LD D,D",     ASM_ADDR_MODE_IMPLIED },
    /* 53 */ { "LD D,E",     ASM_ADDR_MODE_IMPLIED },
    /* 54 */ { "LD D,H",     ASM_ADDR_MODE_IMPLIED },
    /* 55 */ { "LD D,L",     ASM_ADDR_MODE_IMPLIED },
    /* 56 */ { "LD D,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 57 */ { "LD D,A",     ASM_ADDR_MODE_IMPLIED },
    /* 58 */ { "LD E,B",     ASM_ADDR_MODE_IMPLIED },
    /* 59 */ { "LD E,C",     ASM_ADDR_MODE_IMPLIED },
    /* 5a */ { "LD E,D",     ASM_ADDR_MODE_IMPLIED },
    /* 5b */ { "LD E,E",     ASM_ADDR_MODE_IMPLIED },
    /* 5c */ { "LD E,H",     ASM_ADDR_MODE_IMPLIED },
    /* 5d */ { "LD E,L",     ASM_ADDR_MODE_IMPLIED },
    /* 5e */ { "LD E,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 5f */ { "LD E,A",     ASM_ADDR_MODE_IMPLIED },
    /* 60 */ { "LD H,B",     ASM_ADDR_MODE_IMPLIED },
    /* 61 */ { "LD H,C",     ASM_ADDR_MODE_IMPLIED },
    /* 62 */ { "LD H,D",     ASM_ADDR_MODE_IMPLIED },
    /* 63 */ { "LD H,E",     ASM_ADDR_MODE_IMPLIED },
    /* 64 */ { "LD H,H",     ASM_ADDR_MODE_IMPLIED },
    /* 65 */ { "LD H,L",     ASM_ADDR_MODE_IMPLIED },
    /* 66 */ { "LD H,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 67 */ { "LD H,A",     ASM_ADDR_MODE_IMPLIED },
    /* 68 */ { "LD L,B",     ASM_ADDR_MODE_IMPLIED },
    /* 69 */ { "LD L,C",     ASM_ADDR_MODE_IMPLIED },
    /* 6a */ { "LD L,D",     ASM_ADDR_MODE_IMPLIED },
    /* 6b */ { "LD L,E",     ASM_ADDR_MODE_IMPLIED },
    /* 6c */ { "LD L,H",     ASM_ADDR_MODE_IMPLIED },
    /* 6d */ { "LD L,L",     ASM_ADDR_MODE_IMPLIED },
    /* 6e */ { "LD L,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 6f */ { "LD L,A",     ASM_ADDR_MODE_IMPLIED },
    /* 70 */ { "LD (HL),B",  ASM_ADDR_MODE_IMPLIED },
    /* 71 */ { "LD (HL),C",  ASM_ADDR_MODE_IMPLIED },
    /* 72 */ { "LD (HL),D",  ASM_ADDR_MODE_IMPLIED },
    /* 73 */ { "LD (HL),E",  ASM_ADDR_MODE_IMPLIED },
    /* 74 */ { "LD (HL),H",  ASM_ADDR_MODE_IMPLIED },
    /* 75 */ { "LD (HL),L",  ASM_ADDR_MODE_IMPLIED },
    /* 76 */ { "HALT",       ASM_ADDR_MODE_IMPLIED },
    /* 77 */ { "LD (HL),A",  ASM_ADDR_MODE_IMPLIED },
    /* 78 */ { "LD A,B",     ASM_ADDR_MODE_IMPLIED },
    /* 79 */ { "LD A,C",     ASM_ADDR_MODE_IMPLIED },
    /* 7a */ { "LD A,D",     ASM_ADDR_MODE_IMPLIED },
    /* 7b */ { "LD A,E",     ASM_ADDR_MODE_IMPLIED },
    /* 7c */ { "LD A,H",     ASM_ADDR_MODE_IMPLIED },
    /* 7d */ { "LD A,L",     ASM_ADDR_MODE_IMPLIED },
    /* 7e */ { "LD A,(HL)",  ASM_ADDR_MODE_IMPLIED },
    /* 7f */ { "LD A,A",     ASM_ADDR_MODE_IMPLIED },
    /* 80 */ { "ADD",        ASM_ADDR_MODE_REG_B },
    /* 81 */ { "ADD",        ASM_ADDR_MODE_REG_C },
    /* 82 */ { "ADD",        ASM_ADDR_MODE_REG_D },
    /* 83 */ { "ADD",        ASM_ADDR_MODE_REG_E },
    /* 84 */ { "ADD",        ASM_ADDR_MODE_REG_H },
    /* 85 */ { "ADD",        ASM_ADDR_MODE_REG_L },
    /* 86 */ { "ADD",        ASM_ADDR_MODE_REG_IND_HL },
    /* 87 */ { "ADD",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 88 */ { "ADC",        ASM_ADDR_MODE_REG_B },
    /* 89 */ { "ADC",        ASM_ADDR_MODE_REG_C },
    /* 8a */ { "ADC",        ASM_ADDR_MODE_REG_D },
    /* 8b */ { "ADC",        ASM_ADDR_MODE_REG_E },
    /* 8c */ { "ADC",        ASM_ADDR_MODE_REG_H },
    /* 8d */ { "ADC",        ASM_ADDR_MODE_REG_L },
    /* 8e */ { "ADC",        ASM_ADDR_MODE_REG_IND_HL },
    /* 8f */ { "ADC",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 90 */ { "SUB",        ASM_ADDR_MODE_REG_B },
    /* 91 */ { "SUB",        ASM_ADDR_MODE_REG_C },
    /* 92 */ { "SUB",        ASM_ADDR_MODE_REG_D },
    /* 93 */ { "SUB",        ASM_ADDR_MODE_REG_E },
    /* 94 */ { "SUB",        ASM_ADDR_MODE_REG_H },
    /* 95 */ { "SUB",        ASM_ADDR_MODE_REG_L },
    /* 96 */ { "SUB",        ASM_ADDR_MODE_REG_IND_HL },
    /* 97 */ { "SUB",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 98 */ { "SBC",        ASM_ADDR_MODE_REG_B },
    /* 99 */ { "SBC",        ASM_ADDR_MODE_REG_C },
    /* 9a */ { "SBC",        ASM_ADDR_MODE_REG_D },
    /* 9b */ { "SBC",        ASM_ADDR_MODE_REG_E },
    /* 9c */ { "SBC",        ASM_ADDR_MODE_REG_H },
    /* 9d */ { "SBC",        ASM_ADDR_MODE_REG_L },
    /* 9e */ { "SBC",        ASM_ADDR_MODE_REG_IND_HL },
    /* 9f */ { "SBC",        ASM_ADDR_MODE_ACCUMULATOR },
    /* a0 */ { "AND",        ASM_ADDR_MODE_REG_B },
    /* a1 */ { "AND",        ASM_ADDR_MODE_REG_C },
    /* a2 */ { "AND",        ASM_ADDR_MODE_REG_D },
    /* a3 */ { "AND",        ASM_ADDR_MODE_REG_E },
    /* a4 */ { "AND",        ASM_ADDR_MODE_REG_H },
    /* a5 */ { "AND",        ASM_ADDR_MODE_REG_L },
    /* a6 */ { "AND",        ASM_ADDR_MODE_REG_IND_HL },
    /* a7 */ { "AND",        ASM_ADDR_MODE_ACCUMULATOR },
    /* a8 */ { "XOR",        ASM_ADDR_MODE_REG_B },
    /* a9 */ { "XOR",        ASM_ADDR_MODE_REG_C },
    /* aa */ { "XOR",        ASM_ADDR_MODE_REG_D },
    /* ab */ { "XOR",        ASM_ADDR_MODE_REG_E },
    /* ac */ { "XOR",        ASM_ADDR_MODE_REG_H },
    /* ad */ { "XOR",        ASM_ADDR_MODE_REG_L },
    /* ae */ { "XOR",        ASM_ADDR_MODE_REG_IND_HL },
    /* af */ { "XOR",        ASM_ADDR_MODE_ACCUMULATOR },
    /* b0 */ { "OR",         ASM_ADDR_MODE_REG_B },
    /* b1 */ { "OR",         ASM_ADDR_MODE_REG_C },
    /* b2 */ { "OR",         ASM_ADDR_MODE_REG_D },
    /* b3 */ { "OR",         ASM_ADDR_MODE_REG_E },
    /* b4 */ { "OR",         ASM_ADDR_MODE_REG_H },
    /* b5 */ { "OR",         ASM_ADDR_MODE_REG_L },
    /* b6 */ { "OR",         ASM_ADDR_MODE_REG_IND_HL },
    /* b7 */ { "OR",         ASM_ADDR_MODE_ACCUMULATOR },
    /* b8 */ { "CP",         ASM_ADDR_MODE_REG_B },
    /* b9 */ { "CP",         ASM_ADDR_MODE_REG_C },
    /* ba */ { "CP",         ASM_ADDR_MODE_REG_D },
    /* bb */ { "CP",         ASM_ADDR_MODE_REG_E },
    /* bc */ { "CP",         ASM_ADDR_MODE_REG_H },
    /* bd */ { "CP",         ASM_ADDR_MODE_REG_L },
    /* be */ { "CP",         ASM_ADDR_MODE_REG_IND_HL },
    /* bf */ { "CP",         ASM_ADDR_MODE_ACCUMULATOR },
    /* c0 */ { "RET NZ",     ASM_ADDR_MODE_IMPLIED },
    /* c1 */ { "POP",        ASM_ADDR_MODE_REG_BC },
    /* c2 */ { "JP NZ,",     ASM_ADDR_MODE_ABSOLUTE },
    /* c3 */ { "JP",         ASM_ADDR_MODE_ABSOLUTE },
    /* c4 */ { "CALL NZ,",   ASM_ADDR_MODE_ABSOLUTE },
    /* c5 */ { "PUSH",       ASM_ADDR_MODE_REG_BC },
    /* c6 */ { "ADD",        ASM_ADDR_MODE_IMMEDIATE },
    /* c7 */ { "RST 00",     ASM_ADDR_MODE_IMPLIED },
    /* c8 */ { "RET Z",      ASM_ADDR_MODE_IMPLIED },
    /* c9 */ { "RET",        ASM_ADDR_MODE_IMPLIED },
    /* ca */ { "JP Z,",      ASM_ADDR_MODE_ABSOLUTE },
    /* cb */ { "PFX_CB",     ASM_ADDR_MODE_IMPLIED },   /* prefix for BIT instructions */
    /* cc */ { "CALL Z,",    ASM_ADDR_MODE_ABSOLUTE },
    /* cd */ { "CALL",       ASM_ADDR_MODE_ABSOLUTE },
    /* ce */ { "ADC",        ASM_ADDR_MODE_IMMEDIATE },
    /* cf */ { "RST 08",     ASM_ADDR_MODE_IMPLIED },
    /* d0 */ { "RET NC",     ASM_ADDR_MODE_IMPLIED },
    /* d1 */ { "POP",        ASM_ADDR_MODE_REG_DE },
    /* d2 */ { "JP NC,",     ASM_ADDR_MODE_ABSOLUTE },
    /* d3 */ { "OUTA",       ASM_ADDR_MODE_ZERO_PAGE },
    /* d4 */ { "CALL NC,",   ASM_ADDR_MODE_ABSOLUTE },
    /* d5 */ { "PUSH",       ASM_ADDR_MODE_REG_DE },
    /* d6 */ { "SUB",        ASM_ADDR_MODE_IMMEDIATE },
    /* d7 */ { "RST 10",     ASM_ADDR_MODE_IMPLIED },
    /* d8 */ { "RET C",      ASM_ADDR_MODE_IMPLIED },
    /* d9 */ { "EXX",        ASM_ADDR_MODE_IMPLIED },
    /* da */ { "JP C,",      ASM_ADDR_MODE_ABSOLUTE },
    /* db */ { "INA",        ASM_ADDR_MODE_ZERO_PAGE },
    /* dc */ { "CALL C,",    ASM_ADDR_MODE_ABSOLUTE },
    /* dd */ { "PFX_DD",     ASM_ADDR_MODE_IMPLIED },   /* prefix for IX instructions */
    /* de */ { "SBC",        ASM_ADDR_MODE_IMMEDIATE },
    /* df */ { "RST 18",     ASM_ADDR_MODE_IMPLIED },
    /* e0 */ { "RET PO",     ASM_ADDR_MODE_IMPLIED },
    /* e1 */ { "POP",        ASM_ADDR_MODE_REG_HL },
    /* e2 */ { "JP PO,",     ASM_ADDR_MODE_ABSOLUTE },
    /* e3 */ { "EX (SP),HL", ASM_ADDR_MODE_IMPLIED },
    /* e4 */ { "CALL PO,",   ASM_ADDR_MODE_ABSOLUTE },
    /* e5 */ { "PUSH",       ASM_ADDR_MODE_REG_HL },
    /* e6 */ { "AND",        ASM_ADDR_MODE_IMMEDIATE },
    /* e7 */ { "RST 20",     ASM_ADDR_MODE_IMPLIED },
    /* e8 */ { "RET PE",     ASM_ADDR_MODE_IMPLIED },
    /* e9 */ { "JP (HL)",    ASM_ADDR_MODE_IMPLIED },
    /* ea */ { "JP PE,",     ASM_ADDR_MODE_ABSOLUTE },
    /* eb */ { "EX DE,HL",   ASM_ADDR_MODE_IMPLIED },
    /* ec */ { "CALL PE,",   ASM_ADDR_MODE_ABSOLUTE },
    /* ed */ { "PFX_ED",     ASM_ADDR_MODE_IMPLIED },   /* prefix for extended instructions */
    /* ee */ { "XOR",        ASM_ADDR_MODE_IMMEDIATE },
    /* ef */ { "RST 28",     ASM_ADDR_MODE_IMPLIED },
    /* f0 */ { "RET P",      ASM_ADDR_MODE_IMPLIED },
    /* f1 */ { "POP",        ASM_ADDR_MODE_REG_AF },
    /* f2 */ { "JP P,",      ASM_ADDR_MODE_ABSOLUTE },
    /* f3 */ { "DI",         ASM_ADDR_MODE_IMPLIED },
    /* f4 */ { "CALL P,",    ASM_ADDR_MODE_ABSOLUTE },
    /* f5 */ { "PUSH",       ASM_ADDR_MODE_REG_AF },
    /* f6 */ { "OR",         ASM_ADDR_MODE_IMMEDIATE },
    /* f7 */ { "RST 30",     ASM_ADDR_MODE_IMPLIED },
    /* f8 */ { "RET M",      ASM_ADDR_MODE_IMPLIED },
    /* f9 */ { "LD SP,HL",   ASM_ADDR_MODE_IMPLIED },
    /* fa */ { "JP M,",      ASM_ADDR_MODE_ABSOLUTE },
    /* fb */ { "EI",         ASM_ADDR_MODE_IMPLIED },
    /* fc */ { "CALL M,",    ASM_ADDR_MODE_ABSOLUTE },
    /* fd */ { "PFX_FD",     ASM_ADDR_MODE_IMPLIED },   /* prefix for IY instructions */
    /* fe */ { "CP",         ASM_ADDR_MODE_IMMEDIATE },
    /* ff */ { "RST 38",     ASM_ADDR_MODE_IMPLIED }
};

/* BIT instructions */
static const asm_opcode_info_t opcode_list_cb[] = {
    /* 00 */ { "RLC",        ASM_ADDR_MODE_REG_B },
    /* 01 */ { "RLC",        ASM_ADDR_MODE_REG_C },
    /* 02 */ { "RLC",        ASM_ADDR_MODE_REG_D },
    /* 03 */ { "RLC",        ASM_ADDR_MODE_REG_E },
    /* 04 */ { "RLC",        ASM_ADDR_MODE_REG_H },
    /* 05 */ { "RLC",        ASM_ADDR_MODE_REG_L },
    /* 06 */ { "RLC",        ASM_ADDR_MODE_REG_IND_HL },
    /* 07 */ { "RLC",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 08 */ { "RRC",        ASM_ADDR_MODE_REG_B },
    /* 09 */ { "RRC",        ASM_ADDR_MODE_REG_C },
    /* 0a */ { "RRC",        ASM_ADDR_MODE_REG_D },
    /* 0b */ { "RRC",        ASM_ADDR_MODE_REG_E },
    /* 0c */ { "RRC",        ASM_ADDR_MODE_REG_H },
    /* 0d */ { "RRC",        ASM_ADDR_MODE_REG_L },
    /* 0e */ { "RRC",        ASM_ADDR_MODE_REG_IND_HL },
    /* 0f */ { "RRC",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 10 */ { "RL",         ASM_ADDR_MODE_REG_B },
    /* 11 */ { "RL",         ASM_ADDR_MODE_REG_C },
    /* 12 */ { "RL",         ASM_ADDR_MODE_REG_D },
    /* 13 */ { "RL",         ASM_ADDR_MODE_REG_E },
    /* 14 */ { "RL",         ASM_ADDR_MODE_REG_H },
    /* 15 */ { "RL",         ASM_ADDR_MODE_REG_L },
    /* 16 */ { "RL",         ASM_ADDR_MODE_REG_IND_HL },
    /* 17 */ { "RL",         ASM_ADDR_MODE_ACCUMULATOR },
    /* 18 */ { "RR",         ASM_ADDR_MODE_REG_B },
    /* 19 */ { "RR",         ASM_ADDR_MODE_REG_C },
    /* 1a */ { "RR",         ASM_ADDR_MODE_REG_D },
    /* 1b */ { "RR",         ASM_ADDR_MODE_REG_E },
    /* 1c */ { "RR",         ASM_ADDR_MODE_REG_H },
    /* 1d */ { "RR",         ASM_ADDR_MODE_REG_L },
    /* 1e */ { "RR",         ASM_ADDR_MODE_REG_IND_HL },
    /* 1f */ { "RR",         ASM_ADDR_MODE_ACCUMULATOR },
    /* 20 */ { "SLA",        ASM_ADDR_MODE_REG_B },
    /* 21 */ { "SLA",        ASM_ADDR_MODE_REG_C },
    /* 22 */ { "SLA",        ASM_ADDR_MODE_REG_D },
    /* 23 */ { "SLA",        ASM_ADDR_MODE_REG_E },
    /* 24 */ { "SLA",        ASM_ADDR_MODE_REG_H },
    /* 25 */ { "SLA",        ASM_ADDR_MODE_REG_L },
    /* 26 */ { "SLA",        ASM_ADDR_MODE_REG_IND_HL },
    /* 27 */ { "SLA",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 28 */ { "SRA",        ASM_ADDR_MODE_REG_B },
    /* 29 */ { "SRA",        ASM_ADDR_MODE_REG_C },
    /* 2a */ { "SRA",        ASM_ADDR_MODE_REG_D },
    /* 2b */ { "SRA",        ASM_ADDR_MODE_REG_E },
    /* 2c */ { "SRA",        ASM_ADDR_MODE_REG_H },
    /* 2d */ { "SRA",        ASM_ADDR_MODE_REG_L },
    /* 2e */ { "SRA",        ASM_ADDR_MODE_REG_IND_HL },
    /* 2f */ { "SRA",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 30 */ { "SLL",        ASM_ADDR_MODE_REG_B },         /* undoc */
    /* 31 */ { "SLL",        ASM_ADDR_MODE_REG_C },         /* undoc */
    /* 32 */ { "SLL",        ASM_ADDR_MODE_REG_D },         /* undoc */
    /* 33 */ { "SLL",        ASM_ADDR_MODE_REG_E },         /* undoc */
    /* 34 */ { "SLL",        ASM_ADDR_MODE_REG_H },         /* undoc */
    /* 35 */ { "SLL",        ASM_ADDR_MODE_REG_L },         /* undoc */
    /* 36 */ { "SLL",        ASM_ADDR_MODE_REG_IND_HL },    /* undoc */
    /* 37 */ { "SLL",        ASM_ADDR_MODE_ACCUMULATOR },   /* undoc */
    /* 38 */ { "SRL",        ASM_ADDR_MODE_REG_B },
    /* 39 */ { "SRL",        ASM_ADDR_MODE_REG_C },
    /* 3a */ { "SRL",        ASM_ADDR_MODE_REG_D },
    /* 3b */ { "SRL",        ASM_ADDR_MODE_REG_E },
    /* 3c */ { "SRL",        ASM_ADDR_MODE_REG_H },
    /* 3d */ { "SRL",        ASM_ADDR_MODE_REG_L },
    /* 3e */ { "SRL",        ASM_ADDR_MODE_REG_IND_HL },
    /* 3f */ { "SRL",        ASM_ADDR_MODE_ACCUMULATOR },
    /* 40 */ { "BIT 0,",     ASM_ADDR_MODE_REG_B },
    /* 41 */ { "BIT 0,",     ASM_ADDR_MODE_REG_C },
    /* 42 */ { "BIT 0,",     ASM_ADDR_MODE_REG_D },
    /* 43 */ { "BIT 0,",     ASM_ADDR_MODE_REG_E },
    /* 44 */ { "BIT 0,",     ASM_ADDR_MODE_REG_H },
    /* 45 */ { "BIT 0,",     ASM_ADDR_MODE_REG_L },
    /* 46 */ { "BIT 0,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 47 */ { "BIT 0,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 48 */ { "BIT 1,",     ASM_ADDR_MODE_REG_B },
    /* 49 */ { "BIT 1,",     ASM_ADDR_MODE_REG_C },
    /* 4a */ { "BIT 1,",     ASM_ADDR_MODE_REG_D },
    /* 4b */ { "BIT 1,",     ASM_ADDR_MODE_REG_E },
    /* 4c */ { "BIT 1,",     ASM_ADDR_MODE_REG_H },
    /* 4d */ { "BIT 1,",     ASM_ADDR_MODE_REG_L },
    /* 4e */ { "BIT 1,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 4f */ { "BIT 1,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 50 */ { "BIT 2,",     ASM_ADDR_MODE_REG_B },
    /* 51 */ { "BIT 2,",     ASM_ADDR_MODE_REG_C },
    /* 52 */ { "BIT 2,",     ASM_ADDR_MODE_REG_D },
    /* 53 */ { "BIT 2,",     ASM_ADDR_MODE_REG_E },
    /* 54 */ { "BIT 2,",     ASM_ADDR_MODE_REG_H },
    /* 55 */ { "BIT 2,",     ASM_ADDR_MODE_REG_L },
    /* 56 */ { "BIT 2,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 57 */ { "BIT 2,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 58 */ { "BIT 3,",     ASM_ADDR_MODE_REG_B },
    /* 59 */ { "BIT 3,",     ASM_ADDR_MODE_REG_C },
    /* 5a */ { "BIT 3,",     ASM_ADDR_MODE_REG_D },
    /* 5b */ { "BIT 3,",     ASM_ADDR_MODE_REG_E },
    /* 5c */ { "BIT 3,",     ASM_ADDR_MODE_REG_H },
    /* 5d */ { "BIT 3,",     ASM_ADDR_MODE_REG_L },
    /* 5e */ { "BIT 3,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 5f */ { "BIT 3,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 60 */ { "BIT 4,",     ASM_ADDR_MODE_REG_B },
    /* 61 */ { "BIT 4,",     ASM_ADDR_MODE_REG_C },
    /* 62 */ { "BIT 4,",     ASM_ADDR_MODE_REG_D },
    /* 63 */ { "BIT 4,",     ASM_ADDR_MODE_REG_E },
    /* 64 */ { "BIT 4,",     ASM_ADDR_MODE_REG_H },
    /* 65 */ { "BIT 4,",     ASM_ADDR_MODE_REG_L },
    /* 66 */ { "BIT 4,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 67 */ { "BIT 4,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 68 */ { "BIT 5,",     ASM_ADDR_MODE_REG_B },
    /* 69 */ { "BIT 5,",     ASM_ADDR_MODE_REG_C },
    /* 6a */ { "BIT 5,",     ASM_ADDR_MODE_REG_D },
    /* 6b */ { "BIT 5,",     ASM_ADDR_MODE_REG_E },
    /* 6c */ { "BIT 5,",     ASM_ADDR_MODE_REG_H },
    /* 6d */ { "BIT 5,",     ASM_ADDR_MODE_REG_L },
    /* 6e */ { "BIT 5,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 6f */ { "BIT 5,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 70 */ { "BIT 6,",     ASM_ADDR_MODE_REG_B },
    /* 71 */ { "BIT 6,",     ASM_ADDR_MODE_REG_C },
    /* 72 */ { "BIT 6,",     ASM_ADDR_MODE_REG_D },
    /* 73 */ { "BIT 6,",     ASM_ADDR_MODE_REG_E },
    /* 74 */ { "BIT 6,",     ASM_ADDR_MODE_REG_H },
    /* 75 */ { "BIT 6,",     ASM_ADDR_MODE_REG_L },
    /* 76 */ { "BIT 6,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 77 */ { "BIT 6,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 78 */ { "BIT 7,",     ASM_ADDR_MODE_REG_B },
    /* 79 */ { "BIT 7,",     ASM_ADDR_MODE_REG_C },
    /* 7a */ { "BIT 7,",     ASM_ADDR_MODE_REG_D },
    /* 7b */ { "BIT 7,",     ASM_ADDR_MODE_REG_E },
    /* 7c */ { "BIT 7,",     ASM_ADDR_MODE_REG_H },
    /* 7d */ { "BIT 7,",     ASM_ADDR_MODE_REG_L },
    /* 7e */ { "BIT 7,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 7f */ { "BIT 7,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 80 */ { "RES 0,",     ASM_ADDR_MODE_REG_B },
    /* 81 */ { "RES 0,",     ASM_ADDR_MODE_REG_C },
    /* 82 */ { "RES 0,",     ASM_ADDR_MODE_REG_D },
    /* 83 */ { "RES 0,",     ASM_ADDR_MODE_REG_E },
    /* 84 */ { "RES 0,",     ASM_ADDR_MODE_REG_H },
    /* 85 */ { "RES 0,",     ASM_ADDR_MODE_REG_L },
    /* 86 */ { "RES 0,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 87 */ { "RES 0,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 88 */ { "RES 1,",     ASM_ADDR_MODE_REG_B },
    /* 89 */ { "RES 1,",     ASM_ADDR_MODE_REG_C },
    /* 8a */ { "RES 1,",     ASM_ADDR_MODE_REG_D },
    /* 8b */ { "RES 1,",     ASM_ADDR_MODE_REG_E },
    /* 8c */ { "RES 1,",     ASM_ADDR_MODE_REG_H },
    /* 8d */ { "RES 1,",     ASM_ADDR_MODE_REG_L },
    /* 8e */ { "RES 1,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 8f */ { "RES 1,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 90 */ { "RES 2,",     ASM_ADDR_MODE_REG_B },
    /* 91 */ { "RES 2,",     ASM_ADDR_MODE_REG_C },
    /* 92 */ { "RES 2,",     ASM_ADDR_MODE_REG_D },
    /* 93 */ { "RES 2,",     ASM_ADDR_MODE_REG_E },
    /* 94 */ { "RES 2,",     ASM_ADDR_MODE_REG_H },
    /* 95 */ { "RES 2,",     ASM_ADDR_MODE_REG_L },
    /* 96 */ { "RES 2,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 97 */ { "RES 2,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* 98 */ { "RES 3,",     ASM_ADDR_MODE_REG_B },
    /* 99 */ { "RES 3,",     ASM_ADDR_MODE_REG_C },
    /* 9a */ { "RES 3,",     ASM_ADDR_MODE_REG_D },
    /* 9b */ { "RES 3,",     ASM_ADDR_MODE_REG_E },
    /* 9c */ { "RES 3,",     ASM_ADDR_MODE_REG_H },
    /* 9d */ { "RES 3,",     ASM_ADDR_MODE_REG_L },
    /* 9e */ { "RES 3,",     ASM_ADDR_MODE_REG_IND_HL },
    /* 9f */ { "RES 3,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* a0 */ { "RES 4,",     ASM_ADDR_MODE_REG_B },
    /* a1 */ { "RES 4,",     ASM_ADDR_MODE_REG_C },
    /* a2 */ { "RES 4,",     ASM_ADDR_MODE_REG_D },
    /* a3 */ { "RES 4,",     ASM_ADDR_MODE_REG_E },
    /* a4 */ { "RES 4,",     ASM_ADDR_MODE_REG_H },
    /* a5 */ { "RES 4,",     ASM_ADDR_MODE_REG_L },
    /* a6 */ { "RES 4,",     ASM_ADDR_MODE_REG_IND_HL },
    /* a7 */ { "RES 4,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* a8 */ { "RES 5,",     ASM_ADDR_MODE_REG_B },
    /* a9 */ { "RES 5,",     ASM_ADDR_MODE_REG_C },
    /* aa */ { "RES 5,",     ASM_ADDR_MODE_REG_D },
    /* ab */ { "RES 5,",     ASM_ADDR_MODE_REG_E },
    /* ac */ { "RES 5,",     ASM_ADDR_MODE_REG_H },
    /* ad */ { "RES 5,",     ASM_ADDR_MODE_REG_L },
    /* ae */ { "RES 5,",     ASM_ADDR_MODE_REG_IND_HL },
    /* af */ { "RES 5,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* b0 */ { "RES 6,",     ASM_ADDR_MODE_REG_B },
    /* b1 */ { "RES 6,",     ASM_ADDR_MODE_REG_C },
    /* b2 */ { "RES 6,",     ASM_ADDR_MODE_REG_D },
    /* b3 */ { "RES 6,",     ASM_ADDR_MODE_REG_E },
    /* b4 */ { "RES 6,",     ASM_ADDR_MODE_REG_H },
    /* b5 */ { "RES 6,",     ASM_ADDR_MODE_REG_L },
    /* b6 */ { "RES 6,",     ASM_ADDR_MODE_REG_IND_HL },
    /* b7 */ { "RES 6,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* b8 */ { "RES 7,",     ASM_ADDR_MODE_REG_B },
    /* b9 */ { "RES 7,",     ASM_ADDR_MODE_REG_C },
    /* ba */ { "RES 7,",     ASM_ADDR_MODE_REG_D },
    /* bb */ { "RES 7,",     ASM_ADDR_MODE_REG_E },
    /* bc */ { "RES 7,",     ASM_ADDR_MODE_REG_H },
    /* bd */ { "RES 7,",     ASM_ADDR_MODE_REG_L },
    /* be */ { "RES 7,",     ASM_ADDR_MODE_REG_IND_HL },
    /* bf */ { "RES 7,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* c0 */ { "SET 0,",     ASM_ADDR_MODE_REG_B },
    /* c1 */ { "SET 0,",     ASM_ADDR_MODE_REG_C },
    /* c2 */ { "SET 0,",     ASM_ADDR_MODE_REG_D },
    /* c3 */ { "SET 0,",     ASM_ADDR_MODE_REG_E },
    /* c4 */ { "SET 0,",     ASM_ADDR_MODE_REG_H },
    /* c5 */ { "SET 0,",     ASM_ADDR_MODE_REG_L },
    /* c6 */ { "SET 0,",     ASM_ADDR_MODE_REG_IND_HL },
    /* c7 */ { "SET 0,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* c8 */ { "SET 1,",     ASM_ADDR_MODE_REG_B },
    /* c9 */ { "SET 1,",     ASM_ADDR_MODE_REG_C },
    /* ca */ { "SET 1,",     ASM_ADDR_MODE_REG_D },
    /* cb */ { "SET 1,",     ASM_ADDR_MODE_REG_E },
    /* cc */ { "SET 1,",     ASM_ADDR_MODE_REG_H },
    /* cd */ { "SET 1,",     ASM_ADDR_MODE_REG_L },
    /* ce */ { "SET 1,",     ASM_ADDR_MODE_REG_IND_HL },
    /* cf */ { "SET 1,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* d0 */ { "SET 2,",     ASM_ADDR_MODE_REG_B },
    /* d1 */ { "SET 2,",     ASM_ADDR_MODE_REG_C },
    /* d2 */ { "SET 2,",     ASM_ADDR_MODE_REG_D },
    /* d3 */ { "SET 2,",     ASM_ADDR_MODE_REG_E },
    /* d4 */ { "SET 2,",     ASM_ADDR_MODE_REG_H },
    /* d5 */ { "SET 2,",     ASM_ADDR_MODE_REG_L },
    /* d6 */ { "SET 2,",     ASM_ADDR_MODE_REG_IND_HL },
    /* d7 */ { "SET 2,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* d8 */ { "SET 3,",     ASM_ADDR_MODE_REG_B },
    /* d9 */ { "SET 3,",     ASM_ADDR_MODE_REG_C },
    /* da */ { "SET 3,",     ASM_ADDR_MODE_REG_D },
    /* db */ { "SET 3,",     ASM_ADDR_MODE_REG_E },
    /* dc */ { "SET 3,",     ASM_ADDR_MODE_REG_H },
    /* dd */ { "SET 3,",     ASM_ADDR_MODE_REG_L },
    /* de */ { "SET 3,",     ASM_ADDR_MODE_REG_IND_HL },
    /* df */ { "SET 3,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* e0 */ { "SET 4,",     ASM_ADDR_MODE_REG_B },
    /* e1 */ { "SET 4,",     ASM_ADDR_MODE_REG_C },
    /* e2 */ { "SET 4,",     ASM_ADDR_MODE_REG_D },
    /* e3 */ { "SET 4,",     ASM_ADDR_MODE_REG_E },
    /* e4 */ { "SET 4,",     ASM_ADDR_MODE_REG_H },
    /* e5 */ { "SET 4,",     ASM_ADDR_MODE_REG_L },
    /* e6 */ { "SET 4,",     ASM_ADDR_MODE_REG_IND_HL },
    /* e7 */ { "SET 4,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* e8 */ { "SET 5,",     ASM_ADDR_MODE_REG_B },
    /* e9 */ { "SET 5,",     ASM_ADDR_MODE_REG_C },
    /* ea */ { "SET 5,",     ASM_ADDR_MODE_REG_D },
    /* eb */ { "SET 5,",     ASM_ADDR_MODE_REG_E },
    /* ec */ { "SET 5,",     ASM_ADDR_MODE_REG_H },
    /* ed */ { "SET 5,",     ASM_ADDR_MODE_REG_L },
    /* ee */ { "SET 5,",     ASM_ADDR_MODE_REG_IND_HL },
    /* ef */ { "SET 5,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* f0 */ { "SET 6,",     ASM_ADDR_MODE_REG_B },
    /* f1 */ { "SET 6,",     ASM_ADDR_MODE_REG_C },
    /* f2 */ { "SET 6,",     ASM_ADDR_MODE_REG_D },
    /* f3 */ { "SET 6,",     ASM_ADDR_MODE_REG_E },
    /* f4 */ { "SET 6,",     ASM_ADDR_MODE_REG_H },
    /* f5 */ { "SET 6,",     ASM_ADDR_MODE_REG_L },
    /* f6 */ { "SET 6,",     ASM_ADDR_MODE_REG_IND_HL },
    /* f7 */ { "SET 6,",     ASM_ADDR_MODE_ACCUMULATOR },
    /* f8 */ { "SET 7,",     ASM_ADDR_MODE_REG_B },
    /* f9 */ { "SET 7,",     ASM_ADDR_MODE_REG_C },
    /* fa */ { "SET 7,",     ASM_ADDR_MODE_REG_D },
    /* fb */ { "SET 7,",     ASM_ADDR_MODE_REG_E },
    /* fc */ { "SET 7,",     ASM_ADDR_MODE_REG_H },
    /* fd */ { "SET 7,",     ASM_ADDR_MODE_REG_L },
    /* fe */ { "SET 7,",     ASM_ADDR_MODE_REG_IND_HL },
    /* ff */ { "SET 7,",     ASM_ADDR_MODE_ACCUMULATOR }
};

/* IX instructions */
static const asm_opcode_info_t opcode_list_dd[] = {
    /* dd 00 */ { "NOP",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 01 */ { "LD BC,",     ASM_ADDR_MODE_IMMEDIATE_16 },       /* undoc */
    /* dd 02 */ { "LD (BC),A",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 03 */ { "INC",        ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* dd 04 */ { "INC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd 05 */ { "DEC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd 06 */ { "LD B,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 07 */ { "RLCA",       ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 08 */ { "EX AF,AF'",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 09 */ { "ADD IX,BC",  ASM_ADDR_MODE_IMPLIED },
    /* dd 0a */ { "LD A,(BC)",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 0b */ { "DEC",        ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* dd 0c */ { "INC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd 0d */ { "DEC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd 0e */ { "LD C,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 0f */ { "RRCA",       ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 10 */ { "DJNZ",       ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* dd 11 */ { "LD DE,",     ASM_ADDR_MODE_IMMEDIATE_16 },       /* undoc */
    /* dd 12 */ { "LD (DE),A",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 13 */ { "INC",        ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* dd 14 */ { "INC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd 15 */ { "DEC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd 16 */ { "LD D,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 17 */ { "RLA",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 18 */ { "JR",         ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* dd 19 */ { "ADD IX,DE",  ASM_ADDR_MODE_IMPLIED },
    /* dd 1a */ { "LD A,(DE)",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 1b */ { "DEC",        ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* dd 1c */ { "INC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd 1d */ { "DEC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd 1e */ { "LD E,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 1f */ { "RRA",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 20 */ { "JR NZ,",     ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* dd 21 */ { "LD IX,",     ASM_ADDR_MODE_IMMEDIATE_16 },
    /* dd 22 */ { "LD",         ASM_ADDR_MODE_ABSOLUTE_IX },
    /* dd 23 */ { "INC",        ASM_ADDR_MODE_REG_IX },
    /* dd 24 */ { "INC",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd 25 */ { "DEC",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd 26 */ { "LD IXH,",    ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 27 */ { "DAA",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 28 */ { "JR Z,",      ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* dd 29 */ { "ADD IX,IX",  ASM_ADDR_MODE_IMPLIED },
    /* dd 2a */ { "LD IX,",     ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },
    /* dd 2b */ { "DEC",        ASM_ADDR_MODE_REG_IX },
    /* dd 2c */ { "INC",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd 2d */ { "DEC",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd 2e */ { "LD IXL,",    ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 2f */ { "CPL",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 30 */ { "JR NC,",     ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* dd 31 */ { "LD SP,",     ASM_ADDR_MODE_IMMEDIATE_16 },       /* undoc */
    /* dd 32 */ { "LD",         ASM_ADDR_MODE_ABSOLUTE_A },         /* undoc */
    /* dd 33 */ { "INC",        ASM_ADDR_MODE_REG_SP },             /* undoc */
    /* dd 34 */ { "INC",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd 35 */ { "DEC",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd 36 */ { "LD (IX",     ASM_ADDR_MODE_Z80_IND_IMMEDIATE },
    /* dd 37 */ { "SCF",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 38 */ { "JR C,",      ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* dd 39 */ { "ADD IX,SP",  ASM_ADDR_MODE_IMPLIED },
    /* dd 3a */ { "LD A,",      ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },   /* undoc */
    /* dd 3b */ { "DEC",        ASM_ADDR_MODE_REG_SP },             /* undoc */
    /* dd 3c */ { "INC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd 3d */ { "DEC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd 3e */ { "LD A,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd 3f */ { "CCF",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 40 */ { "LD B,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 41 */ { "LD B,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 42 */ { "LD B,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 43 */ { "LD B,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 44 */ { "LD B,IXH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 45 */ { "LD B,IXL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 46 */ { "LD B,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 47 */ { "LD B,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 48 */ { "LD C,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 49 */ { "LD C,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 4a */ { "LD C,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 4b */ { "LD C,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 4c */ { "LD C,IXH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 4d */ { "LD C,IXL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 4e */ { "LD C,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 4f */ { "LD C,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 50 */ { "LD D,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 51 */ { "LD D,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 52 */ { "LD D,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 53 */ { "LD D,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 54 */ { "LD D,IXH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 55 */ { "LD D,IXL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 56 */ { "LD D,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 57 */ { "LD D,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 58 */ { "LD E,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 59 */ { "LD E,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 5a */ { "LD E,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 5b */ { "LD E,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 5c */ { "LD E,IXH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 5d */ { "LD E,IXL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 5e */ { "LD E,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 5f */ { "LD E,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 60 */ { "LD IXH,B",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 61 */ { "LD IXH,C",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 62 */ { "LD IXH,D",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 63 */ { "LD IXH,E",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 64 */ { "LD IXH,IXH", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 65 */ { "LD IXH,IXL", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 66 */ { "LD H,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 67 */ { "LD IXH,A",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 68 */ { "LD IXL,B",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 69 */ { "LD IXL,C",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 6a */ { "LD IXL,D",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 6b */ { "LD IXL,E",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 6c */ { "LD IXL,IXH", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 6d */ { "LD IXL,IXL", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 6e */ { "LD L,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 6f */ { "LD IXL,A",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 70 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 71 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 72 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 73 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 74 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 75 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 76 */ { "HALT",       ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 77 */ { "LD (IX+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* dd 78 */ { "LD A,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 79 */ { "LD A,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 7a */ { "LD A,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 7b */ { "LD A,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 7c */ { "LD A,IXH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 7d */ { "LD A,IXL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 7e */ { "LD A,",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd 7f */ { "LD A,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd 80 */ { "ADD",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd 81 */ { "ADD",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd 82 */ { "ADD",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd 83 */ { "ADD",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd 84 */ { "ADD",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd 85 */ { "ADD",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd 86 */ { "ADD",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd 87 */ { "ADD",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd 88 */ { "ADC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd 89 */ { "ADC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd 8a */ { "ADC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd 8b */ { "ADC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd 8c */ { "ADC",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd 8d */ { "ADC",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd 8e */ { "ADC",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd 8f */ { "ADC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd 90 */ { "SUB",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd 91 */ { "SUB",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd 92 */ { "SUB",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd 93 */ { "SUB",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd 94 */ { "SUB",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd 95 */ { "SUB",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd 96 */ { "SUB",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd 97 */ { "SUB",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd 98 */ { "SBC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd 99 */ { "SBC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd 9a */ { "SBC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd 9b */ { "SBC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd 9c */ { "SBC",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd 9d */ { "SBC",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd 9e */ { "SBC",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd 9f */ { "SBC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd a0 */ { "AND",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd a1 */ { "AND",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd a2 */ { "AND",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd a3 */ { "AND",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd a4 */ { "AND",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd a5 */ { "AND",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd a6 */ { "AND",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd a7 */ { "AND",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd a8 */ { "XOR",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd a9 */ { "XOR",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd aa */ { "XOR",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd ab */ { "XOR",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd ac */ { "XOR",        ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd ad */ { "XOR",        ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd ae */ { "XOR",        ASM_ADDR_MODE_REG_IND_IX },
    /* dd af */ { "XOR",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd b0 */ { "OR",         ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd b1 */ { "OR",         ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd b2 */ { "OR",         ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd b3 */ { "OR",         ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd b4 */ { "OR",         ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd b5 */ { "OR",         ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd b6 */ { "OR",         ASM_ADDR_MODE_REG_IND_IX },
    /* dd b7 */ { "OR",         ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd b8 */ { "CP",         ASM_ADDR_MODE_REG_B },              /* undoc */
    /* dd b9 */ { "CP",         ASM_ADDR_MODE_REG_C },              /* undoc */
    /* dd ba */ { "CP",         ASM_ADDR_MODE_REG_D },              /* undoc */
    /* dd bb */ { "CP",         ASM_ADDR_MODE_REG_E },              /* undoc */
    /* dd bc */ { "CP",         ASM_ADDR_MODE_REG_IXH },            /* undoc */
    /* dd bd */ { "CP",         ASM_ADDR_MODE_REG_IXL },            /* undoc */
    /* dd be */ { "CP",         ASM_ADDR_MODE_REG_IND_IX },
    /* dd bf */ { "CP",         ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* dd c0 */ { "RET NZ",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd c1 */ { "POP",        ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* dd c2 */ { "JP NZ,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd c3 */ { "JP",         ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd c4 */ { "CALL NZ,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd c5 */ { "PUSH",       ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* dd c6 */ { "ADD",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd c7 */ { "RST 00",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd c8 */ { "RET Z",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd c9 */ { "RET",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd ca */ { "JP Z,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd cb */ { "PFX_CB",     ASM_ADDR_MODE_IMPLIED },   /* prefix for IX BIT instructions */
    /* dd cc */ { "CALL Z,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd cd */ { "CALL",       ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd ce */ { "ADC",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd cf */ { "RST 08",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd d0 */ { "RET NC",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd d1 */ { "POP",        ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* dd d2 */ { "JP NC,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd d3 */ { "OUTA",       ASM_ADDR_MODE_ZERO_PAGE },          /* undoc */
    /* dd d4 */ { "CALL NC,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd d5 */ { "PUSH",       ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* dd d6 */ { "SUB",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd d7 */ { "RST 10",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd d8 */ { "RET C",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd d9 */ { "EXX",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd da */ { "JP C,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd db */ { "INA",        ASM_ADDR_MODE_ZERO_PAGE },          /* undoc */
    /* dd dc */ { "CALL C,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd dd */ { "NOP",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd de */ { "SBC",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd df */ { "RST 18",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd e0 */ { "RET PO",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd e1 */ { "POP",        ASM_ADDR_MODE_REG_IX },
    /* dd e2 */ { "JP PO,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd e3 */ { "EX (SP),IX", ASM_ADDR_MODE_IMPLIED },
    /* dd e4 */ { "CALL PO,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd e5 */ { "PUSH",       ASM_ADDR_MODE_REG_IX },
    /* dd e6 */ { "AND",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd e7 */ { "RST 20",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd e8 */ { "RET PE",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd e9 */ { "JP (IX)",    ASM_ADDR_MODE_IMPLIED },
    /* dd ea */ { "JP PE,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd eb */ { "EX DE,HL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd ec */ { "CALL PE,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd ed */ { "PFX_ED",     ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd ee */ { "XOR",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd ef */ { "RST 28",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd f0 */ { "RET P",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd f1 */ { "POP",        ASM_ADDR_MODE_REG_AF },             /* undoc */
    /* dd f2 */ { "JP P,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd f3 */ { "DI",         ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd f4 */ { "CALL P,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd f5 */ { "PUSH",       ASM_ADDR_MODE_REG_AF },             /* undoc */
    /* dd f6 */ { "OR",         ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd f7 */ { "RST 30",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd f8 */ { "RET M",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd f9 */ { "LD SP,IX",   ASM_ADDR_MODE_IMPLIED },
    /* dd fa */ { "JP M,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd fb */ { "EI",         ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd fc */ { "CALL M,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* dd fd */ { "NOP",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd fe */ { "CP",         ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* dd ff */ { "RST 38",     ASM_ADDR_MODE_IMPLIED }             /* undoc */
};

/* IX BIT instructions */
static const asm_opcode_info_t opcode_list_dd_cb[] = {
    /* dd cb 00 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 01 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 02 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 03 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 04 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 05 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 06 */ { "RLC",     ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 07 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 08 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 09 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 0a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 0b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 0c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 0d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 0e */ { "RRC",     ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 0f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 10 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 11 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 12 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 13 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 14 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 15 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 16 */ { "RL",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 17 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 18 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 19 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 1a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 1b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 1c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 1d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 1e */ { "RR",      ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 1f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 20 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 21 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 22 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 23 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 24 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 25 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 26 */ { "SLA",     ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 27 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 28 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 29 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 2a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 2b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 2c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 2d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 2e */ { "SRA",     ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 2f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 30 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 31 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 32 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 33 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 34 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 35 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 36 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 37 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 38 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 39 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 3a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 3b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 3c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 3d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 3e */ { "SRL",     ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 3f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 40 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 41 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 42 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 43 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 44 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 45 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 46 */ { "BIT 0,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 47 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 48 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 49 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 4a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 4b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 4c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 4d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 4e */ { "BIT 1,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 4f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 50 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 51 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 52 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 53 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 54 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 55 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 56 */ { "BIT 2,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 57 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 58 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 59 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 5a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 5b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 5c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 5d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 5e */ { "BIT 3,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 5f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 60 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 61 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 62 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 63 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 64 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 65 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 66 */ { "BIT 4,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 67 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 68 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 69 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 6a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 6b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 6c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 6d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 6e */ { "BIT 5,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 6f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 70 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 71 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 72 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 73 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 74 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 75 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 76 */ { "BIT 6,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 77 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 78 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 79 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 7a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 7b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 7c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 7d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 7e */ { "BIT 7,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 7f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 80 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 81 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 82 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 83 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 84 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 85 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 86 */ { "RES 0,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 87 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 88 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 89 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 8a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 8b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 8c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 8d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 8e */ { "RES 1,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 8f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 90 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 91 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 92 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 93 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 94 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 95 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 96 */ { "RES 2,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 97 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 98 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 99 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 9a */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 9b */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 9c */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 9d */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb 9e */ { "RES 3,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb 9f */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a0 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a1 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a2 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a3 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a4 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a5 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a6 */ { "RES 4,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb a7 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a8 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb a9 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb aa */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ab */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ac */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ad */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ae */ { "RES 5,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb af */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b0 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b1 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b2 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b3 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b4 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b5 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b6 */ { "RES 6,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb b7 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b8 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb b9 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ba */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb bb */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb bc */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb bd */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb be */ { "RES 7,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb bf */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c0 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c1 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c2 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c3 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c4 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c5 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c6 */ { "SET 0,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb c7 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c8 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb c9 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ca */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb cb */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb cc */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb cd */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ce */ { "SET 1,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb cf */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d0 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d1 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d2 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d3 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d4 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d5 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d6 */ { "SET 2,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb d7 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d8 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb d9 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb da */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb db */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb dc */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb dd */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb de */ { "SET 3,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb df */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e0 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e1 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e2 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e3 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e4 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e5 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e6 */ { "SET 4,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb e7 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e8 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb e9 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ea */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb eb */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ec */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ed */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb ee */ { "SET 5,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb ef */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f0 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f1 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f2 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f3 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f4 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f5 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f6 */ { "SET 6,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb f7 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f8 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb f9 */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb fa */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb fb */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb fc */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb fd */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* dd cb fe */ { "SET 7,",  ASM_ADDR_MODE_REG_IND_IX },
    /* dd cb ff */ { "",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
};

/* extended instructions */
static const asm_opcode_info_t opcode_list_ed[] = {
    /* ed 00 - 3F are undefined */
    /* ed 00 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 01 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 02 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 03 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 04 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 05 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 06 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 07 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 08 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 09 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 0a */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 0b */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 0c */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 0d */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 0e */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 0f */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 10 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 11 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 12 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 13 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 14 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 15 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 16 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 17 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 18 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 19 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 1a */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 1b */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 1c */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 1d */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 1e */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 1f */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 20 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 21 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 22 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 23 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 24 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 25 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 26 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 27 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 28 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 29 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 2a */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 2b */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 2c */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 2d */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 2e */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 2f */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 30 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 31 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 32 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 33 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 34 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 35 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 36 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 37 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 38 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 39 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 3a */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 3b */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 3c */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 3d */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 3e */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 3f */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */

    /* ed 40 - 7F are valid opcodes */
    /* ed 40 */ { "IN B,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 41 */ { "OUT (C),B",   ASM_ADDR_MODE_IMPLIED },
    /* ed 42 */ { "SBC HL,BC",   ASM_ADDR_MODE_IMPLIED },
    /* ed 43 */ { "LD",          ASM_ADDR_MODE_Z80_ABSOLUTE_BC },
    /* ed 44 */ { "NEG",         ASM_ADDR_MODE_IMPLIED },
    /* ed 45 */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 46 */ { "IM 0",        ASM_ADDR_MODE_IMPLIED },
    /* ed 47 */ { "LD I,A",      ASM_ADDR_MODE_IMPLIED },
    /* ed 48 */ { "IN C,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 49 */ { "OUT (C),C",   ASM_ADDR_MODE_IMPLIED },
    /* ed 4a */ { "ADC HL,BC",   ASM_ADDR_MODE_IMPLIED },
    /* ed 4b */ { "LD BC,",      ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },
    /* ed 4c */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 4d */ { "RETI",        ASM_ADDR_MODE_IMPLIED },
    /* ed 4e */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc IM 0/1 */
    /* ed 4f */ { "LD R,A",      ASM_ADDR_MODE_IMPLIED },
    /* ed 50 */ { "IN D,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 51 */ { "OUT (C),D",   ASM_ADDR_MODE_IMPLIED },
    /* ed 52 */ { "SBC HL,DE",   ASM_ADDR_MODE_IMPLIED },
    /* ed 53 */ { "LD",          ASM_ADDR_MODE_Z80_ABSOLUTE_DE },
    /* ed 54 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 55 */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 56 */ { "IM 1",        ASM_ADDR_MODE_IMPLIED },
    /* ed 57 */ { "LD A,I",      ASM_ADDR_MODE_IMPLIED },
    /* ed 58 */ { "IN E,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 59 */ { "OUT (C),E",   ASM_ADDR_MODE_IMPLIED },
    /* ed 5a */ { "ADC HL,DE",   ASM_ADDR_MODE_IMPLIED },
    /* ed 5b */ { "LD DE,",      ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },
    /* ed 5c */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 5d */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 5e */ { "IM 2",        ASM_ADDR_MODE_IMPLIED },
    /* ed 5f */ { "LD A,R",      ASM_ADDR_MODE_IMPLIED },
    /* ed 60 */ { "IN H,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 61 */ { "OUT (C),H",   ASM_ADDR_MODE_IMPLIED },
    /* ed 62 */ { "SBC HL,HL",   ASM_ADDR_MODE_IMPLIED },
    /* ed 63 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc LD (**), HL */
    /* ed 64 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 65 */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 66 */ { "IM 0",        ASM_ADDR_MODE_IMPLIED },
    /* ed 67 */ { "RRD",         ASM_ADDR_MODE_IMPLIED },
    /* ed 68 */ { "IN L,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 69 */ { "OUT (C),L",   ASM_ADDR_MODE_IMPLIED },
    /* ed 6a */ { "ADC HL,HL",   ASM_ADDR_MODE_IMPLIED },
    /* ed 6b */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc LD HL, (**) */
    /* ed 6c */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 6d */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 6e */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc IM 0/1 */
    /* ed 6f */ { "RLD",         ASM_ADDR_MODE_IMPLIED },
    /* ed 70 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc IN (C) */
    /* ed 71 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc OUT (C),0 */
    /* ed 72 */ { "SBC HL,SP",   ASM_ADDR_MODE_IMPLIED },
    /* ed 73 */ { "LD",          ASM_ADDR_MODE_Z80_ABSOLUTE_SP },
    /* ed 74 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 75 */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 76 */ { "IM 1",        ASM_ADDR_MODE_IMPLIED },
    /* ed 77 */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc */
    /* ed 78 */ { "IN A,(C)",    ASM_ADDR_MODE_IMPLIED },
    /* ed 79 */ { "OUT (C),A",   ASM_ADDR_MODE_IMPLIED },
    /* ed 7a */ { "ADC HL,SP",   ASM_ADDR_MODE_IMPLIED },
    /* ed 7b */ { "LD SP,",      ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },
    /* ed 7c */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc NEG */
    /* ed 7d */ { "RETN",        ASM_ADDR_MODE_IMPLIED },
    /* ed 7e */ { "IM 2",        ASM_ADDR_MODE_IMPLIED },
    /* ed 7f */ { "",            ASM_ADDR_MODE_IMPLIED },           /* undoc */

    /* ed 80 - 9F are not defined */
    /* ed 80 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 81 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 82 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 83 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 84 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 85 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 86 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 87 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 88 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 89 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 8a */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 8b */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 8c */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 8d */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 8e */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 8f */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 90 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 91 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 92 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 93 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 94 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 95 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 96 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 97 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 98 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 99 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 9a */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 9b */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 9c */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 9d */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 9e */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed 9f */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */

    /* ed A0 - BF are valid opcodes */
    /* ed a0 */ { "LDI",         ASM_ADDR_MODE_IMPLIED },
    /* ed a1 */ { "CPI",         ASM_ADDR_MODE_IMPLIED },
    /* ed a2 */ { "INI",         ASM_ADDR_MODE_IMPLIED },
    /* ed a3 */ { "OUTI",        ASM_ADDR_MODE_IMPLIED },
    /* ed a4 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed a5 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed a6 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed a7 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed a8 */ { "LDD",         ASM_ADDR_MODE_IMPLIED },
    /* ed a9 */ { "CPD",         ASM_ADDR_MODE_IMPLIED },
    /* ed aa */ { "IND",         ASM_ADDR_MODE_IMPLIED },
    /* ed ab */ { "OUTD",        ASM_ADDR_MODE_IMPLIED },
    /* ed ac */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ad */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ae */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed af */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed b0 */ { "LDIR",        ASM_ADDR_MODE_IMPLIED },
    /* ed b1 */ { "CPIR",        ASM_ADDR_MODE_IMPLIED },
    /* ed b2 */ { "INIR",        ASM_ADDR_MODE_IMPLIED },
    /* ed b3 */ { "OTIR",        ASM_ADDR_MODE_IMPLIED },
    /* ed b4 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed b5 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed b6 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed b7 */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed b8 */ { "LDDR",        ASM_ADDR_MODE_IMPLIED },
    /* ed b9 */ { "CPDR",        ASM_ADDR_MODE_IMPLIED },
    /* ed ba */ { "INDR",        ASM_ADDR_MODE_IMPLIED },
    /* ed bb */ { "OTDR",        ASM_ADDR_MODE_IMPLIED },
    /* ed bc */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed bd */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed be */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed bf */ { "",            ASM_ADDR_MODE_IMPLIED },            /* undoc */

    /* ed C0 - FF are undefined */
    /* ed c0 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c1 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c2 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c3 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c4 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c5 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c6 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c7 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c8 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed c9 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ca */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed cb */ { "NOP",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed cc */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed cd */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ce */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed cf */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d0 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d1 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d2 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d3 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d4 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d5 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d6 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d7 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d8 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed d9 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed da */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed db */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed dc */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed dd */ { "NOP",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed de */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed df */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e0 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e1 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e2 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e3 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e4 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e5 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e6 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e7 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e8 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed e9 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ea */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed eb */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ec */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ed */ { "NOP",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ee */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ef */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f0 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f1 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f2 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f3 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f4 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f5 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f6 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f7 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f8 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed f9 */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed fa */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed fb */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed fc */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed fd */ { "NOP",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed fe */ { "",           ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* ed ff */ { "",           ASM_ADDR_MODE_IMPLIED }             /* undoc */
};

/* IY instructions */
static const asm_opcode_info_t opcode_list_fd[] = {
    /* fd 00 */ { "NOP",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 01 */ { "LD BC,",     ASM_ADDR_MODE_IMMEDIATE_16 },       /* undoc */
    /* fd 02 */ { "LD (BC),A",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 03 */ { "INC",        ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* fd 04 */ { "INC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd 05 */ { "DEC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd 06 */ { "LD B,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 07 */ { "RLCA",       ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 08 */ { "EX AF,AF'",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 09 */ { "ADD IY,BC",  ASM_ADDR_MODE_IMPLIED },
    /* fd 0a */ { "LD A,(BC)",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 0b */ { "DEC",        ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* fd 0c */ { "INC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd 0d */ { "DEC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd 0e */ { "LD C,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 0f */ { "RRCA",       ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 10 */ { "DJNZ",       ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* fd 11 */ { "LD DE,",     ASM_ADDR_MODE_IMMEDIATE_16 },       /* undoc */
    /* fd 12 */ { "LD (DE),A",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 13 */ { "INC",        ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* fd 14 */ { "INC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd 15 */ { "DEC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd 16 */ { "LD D,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 17 */ { "RLA",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 18 */ { "JR",         ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* fd 19 */ { "ADD IY,DE",  ASM_ADDR_MODE_IMPLIED },
    /* fd 1a */ { "LD A,(DE)",  ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 1b */ { "DEC",        ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* fd 1c */ { "INC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd 1d */ { "DEC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd 1e */ { "LD E,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 1f */ { "RRA",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 20 */ { "JR NZ,",     ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* fd 21 */ { "LD IY,",     ASM_ADDR_MODE_IMMEDIATE_16 },
    /* fd 22 */ { "LD",         ASM_ADDR_MODE_ABSOLUTE_IY },
    /* fd 23 */ { "INC",        ASM_ADDR_MODE_REG_IY },
    /* fd 24 */ { "INC",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd 25 */ { "DEC",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd 26 */ { "LD IYH,",    ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 27 */ { "DAA",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 28 */ { "JR Z,",      ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* fd 29 */ { "ADD IY,IY",  ASM_ADDR_MODE_IMPLIED },
    /* fd 2a */ { "LD IY,",     ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },
    /* fd 2b */ { "DEC",        ASM_ADDR_MODE_REG_IY },
    /* fd 2c */ { "INC",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd 2d */ { "DEC",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd 2e */ { "LD IYL,",    ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 2f */ { "CPL",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 30 */ { "JR NC,",     ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* fd 31 */ { "LD SP,",     ASM_ADDR_MODE_IMMEDIATE_16 },       /* undoc */
    /* fd 32 */ { "LD",         ASM_ADDR_MODE_ABSOLUTE_A },         /* undoc */
    /* fd 33 */ { "INC",        ASM_ADDR_MODE_REG_SP },             /* undoc */
    /* fd 34 */ { "INC",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd 35 */ { "DEC",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd 36 */ { "LD (IY",     ASM_ADDR_MODE_Z80_IND_IMMEDIATE },
    /* fd 37 */ { "SCF",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 38 */ { "JR C,",      ASM_ADDR_MODE_RELATIVE },           /* undoc */
    /* fd 39 */ { "ADD IY,SP",  ASM_ADDR_MODE_IMPLIED },
    /* fd 3a */ { "LD A,",      ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT },   /* undoc */
    /* fd 3b */ { "DEC",        ASM_ADDR_MODE_REG_SP },             /* undoc */
    /* fd 3c */ { "INC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd 3d */ { "DEC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd 3e */ { "LD A,",      ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd 3f */ { "CCF",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 40 */ { "LD B,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 41 */ { "LD B,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 42 */ { "LD B,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 43 */ { "LD B,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 44 */ { "LD B,IYH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 45 */ { "LD B,IYL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 46 */ { "LD B,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 47 */ { "LD B,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 48 */ { "LD C,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 49 */ { "LD C,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 4a */ { "LD C,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 4b */ { "LD C,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 4c */ { "LD C,IYH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 4d */ { "LD C,IYL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 4e */ { "LD C,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 4f */ { "LD C,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 50 */ { "LD D,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 51 */ { "LD D,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 52 */ { "LD D,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 53 */ { "LD D,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 54 */ { "LD D,IYH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 55 */ { "LD D,IY",    ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 56 */ { "LD D,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 57 */ { "LD D,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 58 */ { "LD E,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 59 */ { "LD E,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 5a */ { "LD E,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 5b */ { "LD E,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 5c */ { "LD E,IYH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 5d */ { "LD E,IYL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 5e */ { "LD E,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 5f */ { "LD E,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 60 */ { "LD IYH,B",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 61 */ { "LD IYH,C",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 62 */ { "LD IYH,D",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 63 */ { "LD IYH,E",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 64 */ { "LD IYH,IYH", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 65 */ { "LD IYH,IYL", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 66 */ { "LD H,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 67 */ { "LD IYH,A",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 68 */ { "LD IYL,B",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 69 */ { "LD IYL,C",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 6a */ { "LD IYL,D",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 6b */ { "LD IYL,E",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 6c */ { "LD IYL,IYH", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 6d */ { "LD IYL,IYL", ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 6e */ { "LD L,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 6f */ { "LD IYL,A",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 70 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 71 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 72 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 73 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 74 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 75 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 76 */ { "HALT",       ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 77 */ { "LD (IY+",    ASM_ADDR_MODE_Z80_IND_REG },
    /* fd 78 */ { "LD A,B",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 79 */ { "LD A,C",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 7a */ { "LD A,D",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 7b */ { "LD A,E",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 7c */ { "LD A,IYH",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 7d */ { "LD A,IYL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 7e */ { "LD A,",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd 7f */ { "LD A,A",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd 80 */ { "ADD",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd 81 */ { "ADD",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd 82 */ { "ADD",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd 83 */ { "ADD",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd 84 */ { "ADD",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd 85 */ { "ADD",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd 86 */ { "ADD",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd 87 */ { "ADD",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd 88 */ { "ADC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd 89 */ { "ADC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd 8a */ { "ADC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd 8b */ { "ADC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd 8c */ { "ADC",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd 8d */ { "ADC",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd 8e */ { "ADC",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd 8f */ { "ADC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd 90 */ { "SUB",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd 91 */ { "SUB",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd 92 */ { "SUB",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd 93 */ { "SUB",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd 94 */ { "SUB",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd 95 */ { "SUB",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd 96 */ { "SUB",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd 97 */ { "SUB",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd 98 */ { "SBC",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd 99 */ { "SBC",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd 9a */ { "SBC",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd 9b */ { "SBC",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd 9c */ { "SBC",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd 9d */ { "SBC",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd 9e */ { "SBC",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd 9f */ { "SBC",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd a0 */ { "AND",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd a1 */ { "AND",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd a2 */ { "AND",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd a3 */ { "AND",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd a4 */ { "AND",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd a5 */ { "AND",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd a6 */ { "AND",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd a7 */ { "AND",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd a8 */ { "XOR",        ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd a9 */ { "XOR",        ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd aa */ { "XOR",        ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd ab */ { "XOR",        ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd ac */ { "XOR",        ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd ad */ { "XOR",        ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd ae */ { "XOR",        ASM_ADDR_MODE_REG_IND_IY },
    /* fd af */ { "XOR",        ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd b0 */ { "OR",         ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd b1 */ { "OR",         ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd b2 */ { "OR",         ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd b3 */ { "OR",         ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd b4 */ { "OR",         ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd b5 */ { "OR",         ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd b6 */ { "OR",         ASM_ADDR_MODE_REG_IND_IY },
    /* fd b7 */ { "OR",         ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd b8 */ { "CP",         ASM_ADDR_MODE_REG_B },              /* undoc */
    /* fd b9 */ { "CP",         ASM_ADDR_MODE_REG_C },              /* undoc */
    /* fd ba */ { "CP",         ASM_ADDR_MODE_REG_D },              /* undoc */
    /* fd bb */ { "CP",         ASM_ADDR_MODE_REG_E },              /* undoc */
    /* fd bc */ { "CP",         ASM_ADDR_MODE_REG_IYH },            /* undoc */
    /* fd bd */ { "CP",         ASM_ADDR_MODE_REG_IYL },            /* undoc */
    /* fd be */ { "CP",         ASM_ADDR_MODE_REG_IND_IY },
    /* fd bf */ { "CP",         ASM_ADDR_MODE_ACCUMULATOR },        /* undoc */
    /* fd c0 */ { "RET NZ",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd c1 */ { "POP",        ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* fd c2 */ { "JP NZ,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd c3 */ { "JP",         ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd c4 */ { "CALL NZ,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd c5 */ { "PUSH",       ASM_ADDR_MODE_REG_BC },             /* undoc */
    /* fd c6 */ { "ADD",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd c7 */ { "RST 00",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd c8 */ { "RET Z",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd c9 */ { "RET",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd ca */ { "JP Z,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd cb */ { "PFX_CB",     ASM_ADDR_MODE_IMPLIED },  /* prefix for IY BIT instructions */
    /* fd cc */ { "CALL Z,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd cd */ { "CALL",       ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd ce */ { "ADC",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd cf */ { "RST 08",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd d0 */ { "RET NC",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd d1 */ { "POP",        ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* fd d2 */ { "JP NC,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd d3 */ { "OUTA",       ASM_ADDR_MODE_ZERO_PAGE },          /* undoc */
    /* fd d4 */ { "CALL NC,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd d5 */ { "PUSH",       ASM_ADDR_MODE_REG_DE },             /* undoc */
    /* fd d6 */ { "SUB",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd d7 */ { "RST 10",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd d8 */ { "RET C",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd d9 */ { "EXX",        ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd da */ { "JP C,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd db */ { "INA",        ASM_ADDR_MODE_ZERO_PAGE },          /* undoc */
    /* fd dc */ { "CALL C,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd dd */ { "NOP",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd de */ { "SBC",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd df */ { "RST 18",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd e0 */ { "RET PO",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd e1 */ { "POP",        ASM_ADDR_MODE_REG_IY },
    /* fd e2 */ { "JP PO,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd e3 */ { "EX (SP),IY", ASM_ADDR_MODE_IMPLIED },
    /* fd e4 */ { "CALL PO,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd e5 */ { "PUSH",       ASM_ADDR_MODE_REG_IY },
    /* fd e6 */ { "AND",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd e7 */ { "RST 20",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd e8 */ { "RET PE",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd e9 */ { "JP (IY)",    ASM_ADDR_MODE_IMPLIED },
    /* fd ea */ { "JP PE,",     ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd eb */ { "EX DE,HL",   ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd ec */ { "CALL PE,",   ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd ed */ { "PFX_ED",     ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd ee */ { "XOR",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd ef */ { "RST 28",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd f0 */ { "RET P",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd f1 */ { "POP",        ASM_ADDR_MODE_REG_AF },             /* undoc */
    /* fd f2 */ { "JP P,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd f3 */ { "DI",         ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd f4 */ { "CALL P,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd f5 */ { "PUSH",       ASM_ADDR_MODE_REG_AF },             /* undoc */
    /* fd f6 */ { "OR",         ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd f7 */ { "RST 30",     ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd f8 */ { "RET M",      ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd f9 */ { "LD SP,IY",   ASM_ADDR_MODE_IMPLIED },
    /* fd fa */ { "JP M,",      ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd fb */ { "EI",         ASM_ADDR_MODE_IMPLIED },            /* undoc */
    /* fd fc */ { "CALL M,",    ASM_ADDR_MODE_ABSOLUTE },           /* undoc */
    /* fd fd */ { "NOP",        ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd fe */ { "CP",         ASM_ADDR_MODE_IMMEDIATE },          /* undoc */
    /* fd ff */ { "RST 38",     ASM_ADDR_MODE_IMPLIED }             /* undoc */
};

/* IY BIT instructions */
static const asm_opcode_info_t opcode_list_fd_cb[] = {
    /* fd cb 00 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 01 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 02 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 03 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 04 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 05 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 06 */ { "RLC",     ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 07 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 08 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 09 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 0a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 0b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 0c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 0d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 0e */ { "RRC",     ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 0f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 10 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 11 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 12 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 13 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 14 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 15 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 16 */ { "RL",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 17 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 18 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 19 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 1a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 1b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 1c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 1d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 1e */ { "RR",      ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 1f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 20 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 21 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 22 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 23 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 24 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 25 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 26 */ { "SLA",     ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 27 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 28 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 29 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 2a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 2b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 2c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 2d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 2e */ { "SRA",     ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 2f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 30 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 31 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 32 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 33 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 34 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 35 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 36 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 37 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 38 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 39 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 3a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 3b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 3c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 3d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 3e */ { "SRL",     ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 3f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 40 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 41 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 42 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 43 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 44 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 45 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 46 */ { "BIT 0,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 47 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 48 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 49 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 4a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 4b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 4c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 4d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 4e */ { "BIT 1,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 4f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 50 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 51 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 52 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 53 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 54 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 55 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 56 */ { "BIT 2,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 57 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 58 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 59 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 5a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 5b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 5c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 5d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 5e */ { "BIT 3,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 5f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 60 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 61 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 62 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 63 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 64 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 65 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 66 */ { "BIT 4,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 67 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 68 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 69 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 6a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 6b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 6c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 6d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 6e */ { "BIT 5,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 6f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 70 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 71 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 72 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 73 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 74 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 75 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 76 */ { "BIT 6,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 77 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 78 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 79 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 7a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 7b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 7c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 7d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 7e */ { "BIT 7,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 7f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 80 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 81 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 82 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 83 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 84 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 85 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 86 */ { "RES 0,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 87 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 88 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 89 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 8a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 8b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 8c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 8d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 8e */ { "RES 1,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 8f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 90 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 91 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 92 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 93 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 94 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 95 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 96 */ { "RES 2,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 97 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 98 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 99 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 9a */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 9b */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 9c */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 9d */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb 9e */ { "RES 3,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb 9f */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a0 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a1 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a2 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a3 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a4 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a5 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a6 */ { "RES 4,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb a7 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a8 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb a9 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb aa */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ab */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ac */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ad */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ae */ { "RES 5,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb af */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b0 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b1 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b2 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b3 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b4 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b5 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b6 */ { "RES 6,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb b7 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b8 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb b9 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ba */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb bb */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb bc */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb bd */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb be */ { "RES 7,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb bf */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c0 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c1 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c2 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c3 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c4 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c5 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c6 */ { "SET 0,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb c7 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c8 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb c9 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ca */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb cb */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb cc */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb cd */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ce */ { "SET 1,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb cf */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d0 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d1 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d2 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d3 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d4 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d5 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d6 */ { "SET 2,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb d7 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d8 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb d9 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb da */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb db */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb dc */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb dd */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb de */ { "SET 3,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb df */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e0 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e1 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e2 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e3 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e4 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e5 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e6 */ { "SET 4,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb e7 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e8 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb e9 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ea */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb eb */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ec */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ed */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb ee */ { "SET 5,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb ef */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f0 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f1 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f2 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f3 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f4 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f5 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f6 */ { "SET 6,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb f7 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f8 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb f9 */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb fa */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb fb */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb fc */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb fd */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
    /* fd cb fe */ { "SET 7,",  ASM_ADDR_MODE_REG_IND_IY },
    /* fd cb ff */ { "",        ASM_ADDR_MODE_IMPLIED },             /* undoc */
};

static const asm_opcode_info_t *asm_opcode_info_get(unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3)
{
    if (p0 == 0xcb) {
        return opcode_list_cb + p1;
    }
    if (p0 == 0xdd) {       /* IX instructions */
        if (p1 == 0xcb) {   /* IX BIT instructions */
            return opcode_list_dd_cb + p3;
        }
        return opcode_list_dd + p1;
    }
    if (p0 == 0xed) {
        return opcode_list_ed + p1;
    }
    if (p0 == 0xfd) {       /* IY instructions */
        if (p1 == 0xcb) {   /* IY BIT instructions */
            return opcode_list_fd_cb + p3;
        }
        return opcode_list_fd + p1;
    }
    return opcode_list + p0;
}

static unsigned int asm_addr_mode_get_size(unsigned int mode, unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3)
{
    if (p0 == 0xcb) {
        return addr_mode_size[mode] + 1;
    }
    if (p0 == 0xdd) {       /* IX instructions */
        if (p1 == 0xcb) {   /* IX BIT instructions */
            return 4; /* addr_mode_size[mode] + 2; */
        }
        return addr_mode_size[mode] + 1;
    }
    if (p0 == 0xed) {
        return addr_mode_size[mode] + 1;
    }
    if (p0 == 0xfd) {       /* IY instructions */
        if (p1 == 0xcb) {   /* IY BIT instructions */
            return 4; /* addr_mode_size[mode] + 2; */
        }
        return addr_mode_size[mode] + 1;
    }
    return addr_mode_size[mode];
}

void asmz80_init(monitor_cpu_type_t *monitor_cpu_type)
{
    monitor_cpu_type->cpu_type = CPU_Z80;
    monitor_cpu_type->asm_addr_mode_get_size = asm_addr_mode_get_size;
    monitor_cpu_type->asm_opcode_info_get = asm_opcode_info_get;

    /* Once we have a generic processor specific init, this will move.  */
    mon_assemblez80_init(monitor_cpu_type);
    mon_registerz80_init(monitor_cpu_type);
}
