/** \file   mon_disassemble.c
 * \brief   The VICE built-in monitor, disassembler module
 *
 * \author  Daniel Sladic <sladic@eecg.toronto.edu>
 * \author  Ettore Perazzoli <ettore@comm2000.it>
 * \author  Andreas Boose <viceteam@t-online.de>
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 */

/*
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

#include "mem.h"
#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "asm.h"
#include "console.h"
#include "log.h"
#include "mon_disassemble.h"
#include "mon_util.h"
#include "monitor.h"
#include "types.h"
#include "uimon.h"

/*****************************************************************************/

#define SKIP_PREFIX_6809() remove_6809_prefix(mon_cpu_type->cpu_type, &prefix, opc)

static void remove_6809_prefix(CPU_TYPE_t cpu_type, int *prefix, uint8_t *opc)
{
    if (cpu_type == CPU_6809) {
        if (opc[0] == 0x10 || opc[0] == 0x11) {
            ++*prefix;
        }
    }
}

static const char *get_6309_bitwise_reg(uint8_t val)
{
    switch (val & 0xc0) {
        case 0x00:
            return "CC";
        case 0x80:
            return "A";
        case 0xC0:
            return "B";
    }
    return "?";
}

static const char * const reg6809[] = {
    "D", "X", "Y", "U", "S", "PC", "?R6", "?R7",
    "A", "B", "CC", "DP", "?RC", "?RD", "?RE", "?RF"
};

static const char * const reg6309[] = {
    "D", "X", "Y", "U", "S", "PC", "W", "V",
    "A", "B", "CC", "DP", "0", "0", "E", "F"
};

const char index_reg6809[] = { 'X', 'Y', 'U', 'S' };

/*****************************************************************************/

#define SKIP_PREFIX_Z80()   remove_z80_prefix(mon_cpu_type->cpu_type, opc, &ival)

static void remove_z80_prefix(CPU_TYPE_t cpu_type, uint8_t *opc, unsigned int *ival)
{
    if (cpu_type == CPU_Z80) {
        if (opc[0] == 0xed || opc[0] == 0xdd || opc[0] == 0xfd) {
            *ival = (uint32_t)(opc[2] & 0xff);
            opc[2] = opc[3];
            opc[3] = opc[4];
        }
    }
}

/*****************************************************************************/

/*
 * Used by all disassembly variants.
 * Independent of memory config, since it receives the instruction's bytes
 * explicitly.
 */
static const char *mon_disassemble_to_string_internal(MEMSPACE memspace,
                                                      unsigned int addr, uint8_t opc[5],
                                                      int hex_mode, unsigned *opc_size_p,
                                                      monitor_cpu_type_t *mon_cpu_type)
{
    static char buff[256];
    const char *string;
    char *buffp, *addr_name;
    int addr_mode, is_undoc;
    unsigned int opc_size;
    unsigned int ival;    /* Use unsigned int because DWORD may be a long. */
    uint16_t ival2;
    const asm_opcode_info_t *opinfo;
    int prefix = 0;
    int8_t sval;

#define x       opc[0]
#define p1      opc[1]
#define p2      opc[2]
#define p3      opc[3]
#define p4      opc[4]

    ival = (uint32_t)(p1 & 0xff);

    buffp = buff;

    if (!mon_cpu_type) {
        mon_cpu_type = monitor_cpu_for_memspace[memspace];
    }
    opinfo = (mon_cpu_type->asm_opcode_info_get)(x, p1, p2, p3);
    string = opinfo->mnemonic;
    addr_mode = opinfo->addr_mode & ~ASM_ADDR_MODE_UNDOC;
    is_undoc = opinfo->addr_mode & ASM_ADDR_MODE_UNDOC;
    opc_size = (mon_cpu_type->asm_addr_mode_get_size)((unsigned int)(addr_mode), x, p1, p2, p3);

    if (opc_size_p) {
        *opc_size_p = opc_size;
    }

    switch (opc_size) {
        case 1:
            sprintf(buff, "%02X          %s%s",
                    x,
                    is_undoc ? "*" : "", string);
            break;
        case 2:
            sprintf(buff, "%02X %02X       %s%s",
                    x, (unsigned int)p1 & 0xff,
                    is_undoc ? "*" : "", string);
            break;
        case 3:
            sprintf(buff, "%02X %02X %02X    %s%s",
                    x, (unsigned int)p1 & 0xff, (unsigned int)p2 & 0xff,
                    is_undoc ? "*" : "", string);
            break;
        case 4:
            sprintf(buff, "%02X %02X %02X %02X %s%s",
                    x, (unsigned int)p1 & 0xff, (unsigned int)p2 & 0xff, (unsigned int)p3 & 0xff,
                    is_undoc ? "*" : "", string);
            break;
        case 5:
            sprintf(buff, "%02X%02X%02X %02X%02X %s%s",
                    x, (unsigned int)p1 & 0xff, (unsigned int)p2 & 0xff, (unsigned int)p3 & 0xff, (unsigned int)p4 & 0xff,
                    is_undoc ? "*" : "", string);
            break;
        default:
            mon_out("Invalid opcode length: %u\n", opc_size);
            sprintf(buff, "            %s%s",
                    is_undoc ? "*" : "", string);
    }

    while (*++buffp) {
    }

    /* skip prefix of extended instructions */
    switch (addr_mode) {
        case ASM_ADDR_MODE_ABSOLUTE:
        case ASM_ADDR_MODE_ABSOLUTE_A:
        case ASM_ADDR_MODE_IMMEDIATE:
        case ASM_ADDR_MODE_RELATIVE:
        case ASM_ADDR_MODE_IMMEDIATE_16:
        case ASM_ADDR_MODE_ZERO_PAGE:
        case ASM_ADDR_MODE_ABS_INDIRECT:
        case ASM_ADDR_MODE_ABSOLUTE_HL:
            SKIP_PREFIX_Z80();
            break;
    }

    /* Print arguments of the machine instruction. */
    switch (addr_mode) {
        case ASM_ADDR_MODE_IMPLIED:
            break;

        case ASM_ADDR_MODE_ACCUMULATOR:
            sprintf(buffp, " A");
            break;

        case ASM_ADDR_MODE_IMMEDIATE:
            sprintf(buffp, (hex_mode ? " #$%02X" : " #%3u"), ival);
            break;

        case ASM_ADDR_MODE_Z80_IND_IMMEDIATE:
            sprintf(buffp, (hex_mode ? "+$%02X), #$%02X" : "+%3u), #%3u"), p2, p3);
            break;

        case ASM_ADDR_MODE_ABSOLUTE_LONG:
        case ASM_ADDR_MODE_ABSOLUTE_LONG_X:
        case ASM_ADDR_MODE_ABSOLUTE_IX:
        case ASM_ADDR_MODE_ABSOLUTE_IY:
        case ASM_ADDR_MODE_Z80_ABSOLUTE_BC:
        case ASM_ADDR_MODE_Z80_ABSOLUTE_DE:
        case ASM_ADDR_MODE_Z80_ABSOLUTE_SP:
        case ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT:
            ival |= (uint32_t)((p3 & 0xff) << 16);
            /* fall through */
        case ASM_ADDR_MODE_ABSOLUTE:
        case ASM_ADDR_MODE_ABSOLUTE_A:
        case ASM_ADDR_MODE_ABSOLUTE_X:
        case ASM_ADDR_MODE_ABSOLUTE_Y:
        case ASM_ADDR_MODE_ABSOLUTE_HL:
        case ASM_ADDR_MODE_ABS_INDIRECT:
        case ASM_ADDR_MODE_ABS_INDIRECT_X:
        case ASM_ADDR_MODE_ABS_IND_LONG:
            ival |= (uint32_t)((p2 & 0xff) << 8);
            /* fall through */
        case ASM_ADDR_MODE_ZERO_PAGE:
        case ASM_ADDR_MODE_ZERO_PAGE_X:
        case ASM_ADDR_MODE_ZERO_PAGE_Y:
        case ASM_ADDR_MODE_INDIRECT:
        case ASM_ADDR_MODE_INDIRECT_LONG:
        case ASM_ADDR_MODE_INDIRECT_LONG_Y:
        case ASM_ADDR_MODE_INDIRECT_X:
        case ASM_ADDR_MODE_INDIRECT_Y:
            *buffp++ = ' ';
            switch (addr_mode) {
                case ASM_ADDR_MODE_ABSOLUTE_IX:
                case ASM_ADDR_MODE_ABSOLUTE_IY:
                case ASM_ADDR_MODE_Z80_ABSOLUTE_BC:
                case ASM_ADDR_MODE_Z80_ABSOLUTE_DE:
                case ASM_ADDR_MODE_Z80_ABSOLUTE_SP:
                case ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT:
                    ival >>= 8;
                    /* fall through */
                case ASM_ADDR_MODE_INDIRECT:
                case ASM_ADDR_MODE_INDIRECT_X:
                case ASM_ADDR_MODE_INDIRECT_Y:
                case ASM_ADDR_MODE_ABS_INDIRECT:
                case ASM_ADDR_MODE_ABS_INDIRECT_X:
                case ASM_ADDR_MODE_ABSOLUTE_A:
                case ASM_ADDR_MODE_ABSOLUTE_HL:
                    *buffp++ = '(';
                    break;
                case ASM_ADDR_MODE_INDIRECT_LONG:
                case ASM_ADDR_MODE_INDIRECT_LONG_Y:
                case ASM_ADDR_MODE_ABS_IND_LONG:
                    *buffp++ = '[';
                    break;
                default:
                    break;
            }
            if (!(addr_name = mon_symbol_table_lookup_name(e_comp_space, (uint16_t)ival))) {
                switch (addr_mode) {
                    case ASM_ADDR_MODE_ABSOLUTE_LONG:
                    case ASM_ADDR_MODE_ABSOLUTE_LONG_X:
                        sprintf(buffp, (hex_mode ? "$%06X" : "%8u"), ival);
                        buffp += strlen(buffp);
                        break;
                    case ASM_ADDR_MODE_ABSOLUTE:
                    case ASM_ADDR_MODE_ABSOLUTE_A:
                    case ASM_ADDR_MODE_ABSOLUTE_X:
                    case ASM_ADDR_MODE_ABSOLUTE_Y:
                    case ASM_ADDR_MODE_ABSOLUTE_HL:
                    case ASM_ADDR_MODE_Z80_ABSOLUTE_BC:
                    case ASM_ADDR_MODE_Z80_ABSOLUTE_DE:
                    case ASM_ADDR_MODE_Z80_ABSOLUTE_SP:
                    case ASM_ADDR_MODE_ABSOLUTE_IX:
                    case ASM_ADDR_MODE_ABSOLUTE_IY:
                    case ASM_ADDR_MODE_ABS_INDIRECT:
                    case ASM_ADDR_MODE_ABS_INDIRECT_X:
                    case ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT:
                    case ASM_ADDR_MODE_ABS_IND_LONG:
                        sprintf(buffp, (hex_mode ? "$%04X" : "%5u"), ival);
                        buffp += strlen(buffp);
                        break;
                    default:
                        sprintf(buffp, (hex_mode ? "$%02X" : "%3u"), ival);
                        buffp += strlen(buffp);
                        break;
                }
            } else {
                sprintf(buffp, "%s", addr_name);
                buffp += strlen(buffp);
            }
            switch (addr_mode) {
                default:
                    break;
                case ASM_ADDR_MODE_INDIRECT:
                case ASM_ADDR_MODE_ABS_INDIRECT:
                case ASM_ADDR_MODE_Z80_ABS_INDIRECT_EXT:
                    strcpy(buffp, ")");
                    break;
                case ASM_ADDR_MODE_INDIRECT_X:
                case ASM_ADDR_MODE_ABS_INDIRECT_X:
                    strcpy(buffp, ",X)");
                    break;
                case ASM_ADDR_MODE_INDIRECT_Y:
                    strcpy(buffp, "),Y");
                    break;
                case ASM_ADDR_MODE_ABSOLUTE_A:
                    strcpy(buffp, "),A");
                    break;
                case ASM_ADDR_MODE_ABSOLUTE_HL:
                    strcpy(buffp, "),HL");
                    break;
                case ASM_ADDR_MODE_Z80_ABSOLUTE_BC:
                    strcpy(buffp, "),BC");
                    break;
                case ASM_ADDR_MODE_Z80_ABSOLUTE_DE:
                    strcpy(buffp, "),DE");
                    break;
                case ASM_ADDR_MODE_Z80_ABSOLUTE_SP:
                    strcpy(buffp, "),SP");
                    break;
                case ASM_ADDR_MODE_ABSOLUTE_IX:
                    strcpy(buffp, "),IX");
                    break;
                case ASM_ADDR_MODE_ABSOLUTE_IY:
                    strcpy(buffp, "),IY");
                    break;
                case ASM_ADDR_MODE_INDIRECT_LONG:
                case ASM_ADDR_MODE_ABS_IND_LONG:
                    strcpy(buffp, "]");
                    break;
                case ASM_ADDR_MODE_INDIRECT_LONG_Y:
                    strcpy(buffp, "],Y");
                    break;
                case ASM_ADDR_MODE_ZERO_PAGE_X:
                case ASM_ADDR_MODE_ABSOLUTE_X:
                case ASM_ADDR_MODE_ABSOLUTE_LONG_X:
                    strcpy(buffp, ",X");
                    break;
                case ASM_ADDR_MODE_ZERO_PAGE_Y:
                case ASM_ADDR_MODE_ABSOLUTE_Y:
                    strcpy(buffp, ",Y");
                    break;
            }
            break;

        case ASM_ADDR_MODE_STACK_RELATIVE:
            sprintf(buffp, (hex_mode ? " $%02X,S" : " %3u,S"), ival);
            break;

        case ASM_ADDR_MODE_STACK_RELATIVE_Y:
            sprintf(buffp, (hex_mode ? " ($%02X,S),Y" : " (%3u,S),Y"), ival);
            break;

        case ASM_ADDR_MODE_MOVE:
            if (hex_mode) {
                sprintf(buffp, " $%02X,$%02X", (unsigned int)p2 & 0xff, ival);
            } else {
                sprintf(buffp, " %3d,%3u", p2 & 0xff, ival);
            }
            break;

        case ASM_ADDR_MODE_RELATIVE:
            if (0x80 & ival) {
                ival -= 256;
            }
            ival += addr;
            ival += 2;
            ival &= 0xffff;
            *buffp++ = ' ';
            if (!(addr_name = mon_symbol_table_lookup_name(e_comp_space, (uint16_t)ival))) {
                sprintf(buffp, (hex_mode ? "$%04X" : "%5u"), ival);
            } else {
                sprintf(buffp, "%s", addr_name);
            }
            break;

        case ASM_ADDR_MODE_RELATIVE_LONG:
            ival |= (p2 & 0xff) << 8;
            if (0x8000 & ival) {
                ival -= 65536;
            }
            ival += addr;
            ival += 3;
            ival &= 0xffff;
            *buffp++ = ' ';
            if (!(addr_name = mon_symbol_table_lookup_name(e_comp_space, (uint16_t)ival))) {
                sprintf(buffp, (hex_mode ? "$%04X" : "%5u"), ival);
            } else {
                sprintf(buffp, "%s", addr_name);
            }
            break;

        case ASM_ADDR_MODE_ZERO_PAGE_RELATIVE:
            ival2 = (p2 & 0xff);
            if (0x80 & ival2) {
                ival2 -= 256;
            }
            ival2 += addr;
            ival2 += 3;
            *buffp++ = ' ';
            if (!(addr_name = mon_symbol_table_lookup_name(e_comp_space, (uint16_t)ival))) {
                sprintf(buffp, (hex_mode ? "$%02X" : "%3u"), ival);
                buffp += strlen(buffp);
            } else {
                sprintf(buffp, "%s", addr_name);
                buffp += strlen(buffp);
            }
            *buffp++ = ',';
            if (!(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival2))) {
                sprintf(buffp, (hex_mode ? "$%04X" : "%5u"), ival2);
            } else {
                sprintf(buffp, "%s", addr_name);
            }
            break;

        case ASM_ADDR_MODE_IMMEDIATE_16:
            ival |= (uint32_t)((p2 & 0xff) << 8);
            sprintf(buffp, (hex_mode ? " #$%04X" : " #%5u"), ival);
            break;

        case ASM_ADDR_MODE_REG_B:
            sprintf(buffp, " B");
            break;

        case ASM_ADDR_MODE_REG_C:
            sprintf(buffp, " C");
            break;

        case ASM_ADDR_MODE_REG_D:
            sprintf(buffp, " D");
            break;

        case ASM_ADDR_MODE_REG_E:
            sprintf(buffp, " E");
            break;

        case ASM_ADDR_MODE_REG_H:
            sprintf(buffp, " H");
            break;

        case ASM_ADDR_MODE_REG_IXH:
            sprintf(buffp, " IXH");
            break;

        case ASM_ADDR_MODE_REG_IYH:
            sprintf(buffp, " IYH");
            break;

        case ASM_ADDR_MODE_REG_L:
            sprintf(buffp, " L");
            break;

        case ASM_ADDR_MODE_REG_IXL:
            sprintf(buffp, " IXL");
            break;

        case ASM_ADDR_MODE_REG_IYL:
            sprintf(buffp, " IYL");
            break;

        case ASM_ADDR_MODE_REG_AF:
            sprintf(buffp, " AF");
            break;

        case ASM_ADDR_MODE_REG_BC:
            sprintf(buffp, " BC");
            break;

        case ASM_ADDR_MODE_REG_DE:
            sprintf(buffp, " DE");
            break;

        case ASM_ADDR_MODE_REG_HL:
            sprintf(buffp, " HL");
            break;

        case ASM_ADDR_MODE_REG_IX:
            sprintf(buffp, " IX");
            break;

        case ASM_ADDR_MODE_REG_IY:
            sprintf(buffp, " IY");
            break;

        case ASM_ADDR_MODE_REG_SP:
            sprintf(buffp, " SP");
            break;

        case ASM_ADDR_MODE_REG_IND_BC:
            sprintf(buffp, " (BC)");
            break;

        case ASM_ADDR_MODE_REG_IND_DE:
            sprintf(buffp, " (DE)");
            break;

        case ASM_ADDR_MODE_REG_IND_HL:
            sprintf(buffp, " (HL)");
            break;

        case ASM_ADDR_MODE_Z80_IND_REG:
            buffp--;
            sprintf(buffp, (hex_mode ? "%c$%02X), %c" : "%c%3u), %c"),
                    (p2 & 0x80) ? '-' : '+',
                    (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2),
                    "BCDEHL?A"[p1 & 7]);
            break;

        case ASM_ADDR_MODE_REG_IND_IX:
            sprintf(buffp, (hex_mode ? " (IX%c$%02X)" : " (IX%c%3u)"),
                    (p2 & 0x80) ? '-' : '+',
                    (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2));
            break;

        case ASM_ADDR_MODE_IND_IX_REG:
            if ((p3 & 7) == 6) {
                sprintf(buffp, (hex_mode ? " (IX%c$%02X)" : " (IX%c%3u)"),
                        (p2 & 0x80) ? '-' : '+',
                        (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2));
            } else {
                sprintf(buffp, (hex_mode ? " (IX%c$%02X), %c" : " (IX%c%3u), %c"),
                        (p2 & 0x80) ? '-' : '+',
                        (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2),
                        "BCDEHL?A"[p3 & 7]);
            }
            break;

        case ASM_ADDR_MODE_REG_IND_IY:
            sprintf(buffp, (hex_mode ? " (IY%c$%02X)" : " (IY%c%3u)"),
                    (p2 & 0x80) ? '-' : '+', (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2));
            break;

        case ASM_ADDR_MODE_IND_IY_REG:
            if ((p3 & 7) == 6) {
                sprintf(buffp, (hex_mode ? " (IY%c$%02X)" : " (IY%c%3u)"),
                        (p2 & 0x80) ? '-' : '+',
                        (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2));
            } else {
                sprintf(buffp, (hex_mode ? " (IY%c$%02X), %c" : " (IY%c%3u), %c"),
                        (p2 & 0x80) ? '-' : '+',
                        (unsigned int)((p2 & 0x80) ? (p2 ^ 0xff) + 1 : p2),
                        "BCDEHL?A"[p3 & 7]);
            }
            break;

        case ASM_ADDR_MODE_REG_IND_SP:
            sprintf(buffp, " (SP)");
            break;

        /* 6809 modes */
        case ASM_ADDR_MODE_ILLEGAL:
            break;

        case ASM_ADDR_MODE_IMM_BYTE:
            SKIP_PREFIX_6809();
            sprintf(buffp, " #$%02X", opc[prefix + 1]);
            break;

        case ASM_ADDR_MODE_IMM_WORD:
            SKIP_PREFIX_6809();
            ival = (opc[prefix + 1] << 8) + opc[prefix + 2];
            sprintf(buffp, " #$%04X", ival);
            break;

        case ASM_ADDR_MODE_IMM_DWORD:
            SKIP_PREFIX_6809();
            ival = (opc[prefix + 1] << 24) + (opc[prefix + 2] << 16) + (opc[prefix + 3] << 8) + opc[prefix + 4];
            sprintf(buffp, " #$%08X", ival);
            break;

        case ASM_ADDR_MODE_DIRECT:
            SKIP_PREFIX_6809();
            sprintf(buffp, " $%02X", opc[prefix + 1]);
            break;

        case ASM_ADDR_MODE_IM_DIRECT:   /* unused ? */
            SKIP_PREFIX_6809();
            sprintf(buffp, " #$%02X,<$%02X", opc[prefix + 1], opc[prefix + 2]);
            break;

        case ASM_ADDR_MODE_EXTENDED:
            SKIP_PREFIX_6809();
            ival = (opc[prefix + 1] << 8) + opc[prefix + 2];
            sprintf(buffp, " $%04X", ival);
            break;

        case ASM_ADDR_MODE_IM_EXTENDED:
            SKIP_PREFIX_6809();
            ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
            sprintf(buffp, " #$%02X,$%04X", opc[prefix + 1], ival);
            break;

        case ASM_ADDR_MODE_INDEXED:     /* post-byte determines sub-mode */
            {
                char R;
                SKIP_PREFIX_6809();
                ival = opc[prefix + 1];
                R = index_reg6809[(ival >> 5) & 3];

                if ((ival & 0x80) == 0) {
                    int offset = ival & 0x1F;
                    if (offset & 0x10) {        /* sign extend 5-bit value */
                        offset -= 0x20;
                    }
                    sprintf(buffp, " %s$%02X,%c", (offset < 0) ? "-" : "", (unsigned int)abs(offset), R);
                    break;
                }

                switch (ival & 0x1f) {
                    /* ASM_ADDR_MODE_INDEXED_INC1   0x00*/
                    case 0x00:
                        sprintf(buffp, " ,%c+", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2   0x01*/
                    case 0x01:
                        sprintf(buffp, " ,%c++", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC1      0x02*/
                    case 0x02:
                        sprintf(buffp, " ,-%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2   0x03*/
                    case 0x03:
                        sprintf(buffp, " ,--%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0   0x04*/
                    case 0x04:
                        sprintf(buffp, " ,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB   0x05*/
                    case 0x05:
                        sprintf(buffp, " B,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA   0x06*/
                    case 0x06:
                        sprintf(buffp, " A,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8   0x08*/
                    case 0x08:
                        sval = (int8_t)opc[prefix + 2];
                        sprintf(buffp, " %s$%02X,%c", (sval < 0) ? "-" : "", (unsigned int)abs(sval), R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16  0x09*/
                    case 0x09:      /* TODO should signed! (the disasms that I looked at have this as unsigned 16bit, so it's fine [marco]) */
                        sprintf(buffp, " $%04X,%c",
                                (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD   0x0B*/
                    case 0x0B:
                        sprintf(buffp, " D,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8 0x0C*/
                    case 0x0C:
                        sval = (int8_t)opc[prefix + 2];
                        sprintf(buffp, " %s$%02X,PC /* $%04X,PCR */",
                                (sval < 0) ? "-" : "", (unsigned int)abs(sval), (int8_t)opc[prefix + 2] + addr + opc_size);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16        0x0D*/
                    case 0x0D:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " $%04X,PC /* $%04X,PCR */",
                                ival, (uint16_t)(ival + addr + opc_size));
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2_IND       0x11*/
                    case 0x11:
                        sprintf(buffp, " [,%c++]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2_IND       0x13*/
                    case 0x13:
                        sprintf(buffp, " [,--%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0_IND       0x14*/
                    case 0x14:
                        sprintf(buffp, " [,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB_IND       0x15*/
                    case 0x15:
                        sprintf(buffp, " [B,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA_IND       0x16*/
                    case 0x16:
                        sprintf(buffp, " [A,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8_IND       0x18*/
                    case 0x18:
                        sval = (int8_t)opc[prefix + 2];
                        sprintf(buffp, " [%s$%02X,%c]", (sval < 0) ? "-" : "", (unsigned int)abs(sval), R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16_IND 0x19*/
                    case 0x19:
                        sprintf(buffp, " [$%04X,%c]",
                                (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD_IND       0x1B*/
                    case 0x1B:
                        sprintf(buffp, " [D,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8_IND   0x1C*/
                    case 0x1C:
                        sval = (int8_t)opc[prefix + 2];
                        sprintf(buffp, " [%s$%02X,PC] /* [$%04X,PCR] */",
                                (sval < 0) ? "-" : "", (unsigned int)abs(sval), (int8_t)opc[prefix + 2] + addr + opc_size);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16_IND  0x1D*/
                    case 0x1D:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " [$%04X,PC] /* [$%04X,PCR] */",
                                ival, (uint16_t)(ival + addr + opc_size));
                        break;

                    /* ASM_ADDR_MODE_EXTENDED_INDIRECT    0x1F*/
                    case 0x1F:
                        sprintf(buffp, " [$%04X]",
                                (unsigned int)((opc[prefix + 2] << 8) + opc[prefix + 3]));
                        break;

                    /* ASM_ADDR_MODE_INDEXED_07     0x07*/
                    /* ASM_ADDR_MODE_INDEXED_0A     0x0A*/
                    /* ASM_ADDR_MODE_INDEXED_0E     0x0E*/
                    /* ASM_ADDR_MODE_INDEXED_0F     0x0F*/
                    /* ASM_ADDR_MODE_INDEXED_10     0x10*/
                    /* ASM_ADDR_MODE_INDEXED_12     0x12*/
                    /* ASM_ADDR_MODE_INDEXED_17     0x17*/
                    /* ASM_ADDR_MODE_INDEXED_1A     0x1A*/
                    /* ASM_ADDR_MODE_INDEXED_1E     0x1E*/
                    default:
                        sprintf(buffp, " ???");
                        break;
                }
            }
            break;

        case ASM_ADDR_MODE_F6809_INDEXED:     /* post-byte determines sub-mode */
            {
                char R;
                SKIP_PREFIX_6809();
                ival = opc[prefix + 1];
                R = index_reg6809[(ival >> 5) & 3];

                if ((ival & 0x80) == 0) {
                    int offset = ival & 0x1F;
                    if (offset & 0x10) {        /* sign extend 5-bit value */
                        offset -= 0x20;
                    }
                    sprintf(buffp, " %d,%c", offset, R);
                    break;
                }

                switch (ival & 0x1f) {
                    /* ASM_ADDR_MODE_INDEXED_INC1   0x00*/
                    case 0x00:
                        sprintf(buffp, " ,%c+", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2   0x01*/
                    case 0x01:
                        sprintf(buffp, " ,%c++", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC1      0x02*/
                    case 0x02:
                        sprintf(buffp, " ,-%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2   0x03*/
                    case 0x03:
                        sprintf(buffp, " ,--%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0       0x04*/
                    /* ASM_ADDR_MODE_INDEXED_F6809_OFF0 0x07*/
                    case 0x04:
                    case 0x07:
                        sprintf(buffp, " ,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB   0x05*/
                    case 0x05:
                        sprintf(buffp, " B,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA   0x06*/
                    case 0x06:
                        sprintf(buffp, " A,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8   0x08*/
                    case 0x08:      /* TODO should be signed! */
                        sprintf(buffp, " $%02X,%c", opc[prefix + 2], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16  0x09*/
                    case 0x09:      /* TODO should signed! */
                        sprintf(buffp, " $%04X,%c",
                                (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_F6809_OFFPCORFF 0x0A*/
                    case 0x0A:
                        sprintf(buffp, " PCL");
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD   0x0B*/
                    case 0x0B:
                        sprintf(buffp, " D,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8 0x0C*/
                    case 0x0C:
                        sprintf(buffp, " $%04X,PCR /* $%02X,PC */", (int8_t)opc[prefix + 2] + addr + opc_size, opc[prefix + 2]);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16        0x0D*/
                    case 0x0D:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " $%04X,PCR /* $%04X,PC */", (uint16_t)(ival + addr + opc_size), ival);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_F6809_EXTENDED 0x0F*/
                    case 0x0F:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " $%04X", ival);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_F6809_INC1_IND 0x10*/
                    case 0x10:
                        sprintf(buffp, " [,%c+]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2_IND       0x11*/
                    case 0x11:
                        sprintf(buffp, " [,%c++]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_F6809_DEC1_IND 0x12*/
                    case 0x12:
                        sprintf(buffp, " [,-%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2_IND       0x13*/
                    case 0x13:
                        sprintf(buffp, " [,--%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0_IND       0x14*/
                    /* ASM_ADDR_MODE_INDEXED_F6809_OFF0_IND 0x17*/
                    case 0x14:
                    case 0x17:
                        sprintf(buffp, " [,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB_IND       0x15*/
                    case 0x15:
                        sprintf(buffp, " [B,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA_IND       0x16*/
                    case 0x16:
                        sprintf(buffp, " [A,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8_IND       0x18*/
                    case 0x18:
                        sprintf(buffp, " [$%02X,%c]", opc[prefix + 2], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16_IND 0x19*/
                    case 0x19:
                        sprintf(buffp, " [$%04X,%c]",
                                (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_F6809_OFFPCORFF_IND 0x1A*/
                    case 0x1A:
                        sprintf(buffp, " [PCL]");
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD_IND       0x1B*/
                    case 0x1B:
                        sprintf(buffp, " [D,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8_IND   0x1C*/
                    case 0x1C:
                        sprintf(buffp, " [$%04X,PCR] /* [$%02X,PC] */", (int8_t)opc[prefix + 2] + addr + opc_size, opc[prefix + 2]);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16_IND  0x1D*/
                    case 0x1D:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " [$%04X,PCR] /* [$%04X,PC] */", (uint16_t)(ival + addr + opc_size), ival);
                        break;

                    /* ASM_ADDR_MODE_EXTENDED_INDIRECT    0x1F*/
                    case 0x1F:
                        sprintf(buffp, " [$%04X]",
                                (unsigned int)((opc[prefix + 2] << 8) + opc[prefix + 3]));
                        break;

                    /* ASM_ADDR_MODE_INDEXED_0E     0x0E*/
                    /* ASM_ADDR_MODE_INDEXED_1E     0x1E*/
                    default:
                        sprintf(buffp, " ???");
                        break;
                }
            }
            break;

        case ASM_ADDR_MODE_H6309_INDEXED:     /* post-byte determines sub-mode */
            {
                char R;
                SKIP_PREFIX_6809();
                ival = opc[prefix + 1];
                R = index_reg6809[(ival >> 5) & 3];

                if ((ival & 0x80) == 0) {
                    int offset = ival & 0x1F;
                    if (offset & 0x10) {        /* sign extend 5-bit value */
                        offset -= 0x20;
                    }
                    sprintf(buffp, " %d,%c", offset, R);
                    break;
                }

                switch (ival & 0x1f) {
                    /* ASM_ADDR_MODE_INDEXED_INC1   0x00*/
                    case 0x00:
                        sprintf(buffp, " ,%c+", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2   0x01*/
                    case 0x01:
                        sprintf(buffp, " ,%c++", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC1      0x02*/
                    case 0x02:
                        sprintf(buffp, " ,-%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2   0x03*/
                    case 0x03:
                        sprintf(buffp, " ,--%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0       0x04*/
                    case 0x04:
                        sprintf(buffp, " ,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB   0x05*/
                    case 0x05:
                        sprintf(buffp, " B,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA   0x06*/
                    case 0x06:
                        sprintf(buffp, " A,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFE 0x07*/
                    case 0x07:
                        sprintf(buffp, " E,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8   0x08*/
                    case 0x08:      /* TODO should be signed! */
                        sprintf(buffp, " $%02X,%c", opc[prefix + 2], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16  0x09*/
                    case 0x09:      /* TODO should signed! */
                        sprintf(buffp, " $%04X,%c",
                                (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFF 0x0A*/
                    case 0x0A:
                        sprintf(buffp, " F,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD   0x0B*/
                    case 0x0B:
                        sprintf(buffp, " D,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8 0x0C*/
                    case 0x0C:
                        sprintf(buffp, " $%04X,PCR /* $%02X,PC */", (int8_t)opc[prefix + 2] + addr + opc_size, opc[prefix + 2]);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16        0x0D*/
                    case 0x0D:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " $%04X,PCR /* $%04X,PC */", (uint16_t)(ival + addr + opc_size), ival);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFW 0x0E*/
                    case 0x0E:
                        sprintf(buffp, " W,%c", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFWREL 0x0F*/
                    case 0x0F:
                        switch (ival & 0x60) {
                            /* ,W */
                            case 0x00:
                                sprintf(buffp, " ,W");
                                break;

                            /* 16bit,W */
                            case 0x20:
                                sprintf(buffp, " $%04X,W",
                                        (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3]);
                                break;

                            /* ,W++ */
                            case 0x40:
                                sprintf(buffp, " ,W++");
                                break;

                            /* ,--W */
                            case 0x60:
                                sprintf(buffp, " ,--W");
                                break;
                        }
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFWREL_IND 0x10*/
                    case 0x10:
                        switch (ival & 0x60) {
                            /* [,W] */
                            case 0x00:
                                sprintf(buffp, " [,W]");
                                break;

                            /* [16bit,W] */
                            case 0x20:
                                sprintf(buffp, " [$%04X,W]",
                                        (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3]);
                                break;

                            /* [,W++] */
                            case 0x40:
                                sprintf(buffp, " [,W++]");
                                break;

                            /* [,--W] */
                            case 0x60:
                                sprintf(buffp, " [,--W]");
                                break;
                        }
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2_IND       0x11*/
                    case 0x11:
                        sprintf(buffp, " [,%c++]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_DEC1_IND 0x12*/
                    case 0x12:
                        sprintf(buffp, " [,-%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2_IND       0x13*/
                    case 0x13:
                        sprintf(buffp, " [,--%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0_IND       0x14*/
                    case 0x14:
                        sprintf(buffp, " [,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB_IND       0x15*/
                    case 0x15:
                        sprintf(buffp, " [B,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA_IND       0x16*/
                    case 0x16:
                        sprintf(buffp, " [A,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFE_IND 0x17*/
                    case 0x17:
                        sprintf(buffp, " [E,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8_IND       0x18*/
                    case 0x18:
                        sprintf(buffp, " [$%02X,%c]", opc[prefix + 2], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16_IND 0x19*/
                    case 0x19:
                        sprintf(buffp, " [$%04X,%c]",
                                (unsigned int)(opc[prefix + 2] << 8) + opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFF_IND 0x1A*/
                    case 0x1A:
                        sprintf(buffp, " [F,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD_IND       0x1B*/
                    case 0x1B:
                        sprintf(buffp, " [D,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8_IND   0x1C*/
                    case 0x1C:
                        sprintf(buffp, " [$%04X,PCR] /* [$%02X,PC] */", (int8_t)opc[prefix + 2] + addr + opc_size, opc[prefix + 2]);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16_IND  0x1D*/
                    case 0x1D:
                        ival = (opc[prefix + 2] << 8) + opc[prefix + 3];
                        sprintf(buffp, " [$%04X,PCR] /* [$%04X,PC] */", (uint16_t)(ival + addr + opc_size), ival);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFW_IND 0x1E*/
                    case 0x1E:
                        sprintf(buffp, " [W,%c]", R);
                        break;

                    /* ASM_ADDR_MODE_EXTENDED_INDIRECT    0x1F*/
                    case 0x1F:
                        sprintf(buffp, " [$%04X]",
                                (unsigned int)((opc[prefix + 2] << 8) + opc[prefix + 3]));
                        break;
                }
            }
            break;

        case ASM_ADDR_MODE_IM_INDEXED:     /* post-byte determines sub-mode */
            {
                char R;
                SKIP_PREFIX_6809();
                ival = opc[prefix + 2];
                R = index_reg6809[(ival >> 5) & 3];

                if ((ival & 0x80) == 0) {
                    int offset = ival & 0x1F;
                    if (offset & 0x10) {        /* sign extend 5-bit value */
                        offset -= 0x20;
                    }
                    sprintf(buffp, " #$%02X,%d,%c", opc[prefix + 1], offset, R);
                    break;
                }

                switch (ival & 0x1f) {
                    /* ASM_ADDR_MODE_INDEXED_INC1   0x00*/
                    case 0x00:
                        sprintf(buffp, " #$%02X,%c+", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2   0x01*/
                    case 0x01:
                        sprintf(buffp, " #$%02X,%c++", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC1      0x02*/
                    case 0x02:
                        sprintf(buffp, " #$%02X,-%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2   0x03*/
                    case 0x03:
                        sprintf(buffp, " #$%02X,--%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0       0x04*/
                    case 0x04:
                        sprintf(buffp, " #$%02X,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB   0x05*/
                    case 0x05:
                        sprintf(buffp, " #$%02X,B,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA   0x06*/
                    case 0x06:
                        sprintf(buffp, " #$%02X,A,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFE 0x07*/
                    case 0x07:
                        sprintf(buffp, " #$%02X,E,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8   0x08*/
                    case 0x08:      /* TODO should be signed! */
                        sprintf(buffp, " #$%02X,$%02X,%c", opc[prefix + 1], opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16  0x09*/
                    case 0x09:      /* TODO should signed! */
                        sprintf(buffp, " #$%02X,$%04X,%c",
                                opc[prefix + 1],
                                (unsigned int)(opc[prefix + 3] << 8) + opc[prefix + 4], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFF 0x0A*/
                    case 0x0A:
                        sprintf(buffp, " #$%02X,F,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD   0x0B*/
                    case 0x0B:
                        sprintf(buffp, " #$%02X,D,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8 0x0C*/
                    case 0x0C:
                        sprintf(buffp, " #$%02X,$%04X,PCR /* $%02X,PC */", opc[prefix + 1], (int8_t)opc[prefix + 3] + addr + opc_size, opc[prefix + 3]);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16        0x0D*/
                    case 0x0D:
                        ival = (opc[prefix + 3] << 8) + opc[prefix + 4];
                        sprintf(buffp, " #$%02X,$%04X,PCR /* $%04X,PC */", opc[prefix + 1], (uint16_t)(ival + addr + opc_size), ival);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFW 0x0E*/
                    case 0x0E:
                        sprintf(buffp, " #$%02X,W,%c", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFWREL 0x0F*/
                    case 0x0F:
                        switch (ival & 0x60) {
                            /* ,W */
                            case 0x00:
                                sprintf(buffp, " #$%02X,W", opc[prefix + 1]);
                                break;

                            /* 16bit,W */
                            case 0x20:
                                sprintf(buffp, " #$%02X,$%04X,W",
                                        opc[prefix + 1],
                                        (unsigned int)(opc[prefix + 3] << 8) + opc[prefix + 4]);
                                break;

                            /* ,W++ */
                            case 0x40:
                                sprintf(buffp, " #$%02X,W++", opc[prefix + 1]);
                                break;

                            /* ,--W */
                            case 0x60:
                                sprintf(buffp, " #$%02X,--W", opc[prefix + 1]);
                                break;
                        }
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFWREL_IND 0x10*/
                    case 0x10:
                        switch (ival & 0x60) {
                            /* [,W] */
                            case 0x00:
                                sprintf(buffp, " #$%02X,[,W]", opc[prefix + 1]);
                                break;

                            /* [16bit,W] */
                            case 0x20:
                                sprintf(buffp, " #$%02X,[$%04X,W]",
                                        opc[prefix + 1],
                                        (unsigned int)(opc[prefix + 3] << 8) + opc[prefix + 4]);
                                break;

                            /* [,W++] */
                            case 0x40:
                                sprintf(buffp, " #$%02X,[,W++]", opc[prefix + 1]);
                                break;

                            /* [,--W] */
                            case 0x60:
                                sprintf(buffp, " #$%02X,[,--W]", opc[prefix + 1]);
                                break;
                        }
                        break;

                    /* ASM_ADDR_MODE_INDEXED_INC2_IND       0x11*/
                    case 0x11:
                        sprintf(buffp, " #$%02X,[,%c++]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_DEC1_IND 0x12*/
                    case 0x12:
                        sprintf(buffp, " #$%02X,[,-%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_DEC2_IND       0x13*/
                    case 0x13:
                        sprintf(buffp, " #$%02X,[,--%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF0_IND       0x14*/
                    case 0x14:
                        sprintf(buffp, " #$%02X,[,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFB_IND       0x15*/
                    case 0x15:
                        sprintf(buffp, " #$%02X,[B,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFA_IND       0x16*/
                    case 0x16:
                        sprintf(buffp, " #$%02X,[A,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFE_IND 0x17*/
                    case 0x17:
                        sprintf(buffp, " #$%02X,[E,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF8_IND       0x18*/
                    case 0x18:
                        sprintf(buffp, " #$%02X,[$%02X,%c]", opc[prefix + 1], opc[prefix + 3], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFF16_IND 0x19*/
                    case 0x19:
                        sprintf(buffp, " #$%02X,[$%04X,%c]",
                                opc[prefix + 1],
                                (unsigned int)(opc[prefix + 3] << 8) + opc[prefix + 4], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFF_IND 0x1A*/
                    case 0x1A:
                        sprintf(buffp, " #$%02X,[F,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFD_IND       0x1B*/
                    case 0x1B:
                        sprintf(buffp, " #$%02X,[D,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC8_IND   0x1C*/
                    case 0x1C:
                        sprintf(buffp, " #$%02X,[$%04X,PCR] /* [$%02X,PC] */", opc[prefix + 1], (int8_t)opc[prefix + 3] + addr + opc_size, opc[prefix + 3]);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_OFFPC16_IND  0x1D*/
                    case 0x1D:
                        ival = (opc[prefix + 3] << 8) + opc[prefix + 4];
                        sprintf(buffp, " #$%02X,[$%04X,PCR] /* [$%04X,PC] */",
                                (uint16_t)(ival + addr + opc_size), ival, 0U /*FIXME*/);
                        break;

                    /* ASM_ADDR_MODE_INDEXED_H6309_OFFW_IND 0x1E*/
                    case 0x1E:
                        sprintf(buffp, " #$%02X,[W,%c]", opc[prefix + 1], R);
                        break;

                    /* ASM_ADDR_MODE_EXTENDED_INDIRECT    0x1F*/
                    case 0x1F:
                        sprintf(buffp, " #$%02X,[$%04X]",
                                opc[prefix + 1],
                                (unsigned int)((opc[prefix + 3] << 8) + opc[prefix + 4]));
                        break;
                }
            }
            break;

        case ASM_ADDR_MODE_BITWISE:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s,%d,%d,<$%02X", get_6309_bitwise_reg(opc[prefix + 1]), (opc[prefix + 1] & 0x38) >> 3, opc[prefix + 1] & 7, opc[prefix + 2]);
            break;

        case ASM_ADDR_MODE_REL_BYTE:
            SKIP_PREFIX_6809();
            sprintf(buffp, " $%04X", (int8_t)opc[prefix + 1] + addr + opc_size);
            break;

        case ASM_ADDR_MODE_REL_WORD:
            SKIP_PREFIX_6809();
            sprintf(buffp, " $%04X", (uint16_t)((opc[prefix + 1] << 8) + opc[prefix + 2] + addr + opc_size));
            break;

        case ASM_ADDR_MODE_REG_POST:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s,%s", reg6809[opc[prefix + 1] >> 4], reg6809[opc[prefix + 1] & 15]);
            break;

        case ASM_ADDR_MODE_H6309_REG_POST:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s,%s", reg6309[opc[prefix + 1] >> 4], reg6309[opc[prefix + 1] & 15]);
            break;

        case ASM_ADDR_MODE_TFM_PP:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s+,%s+", reg6309[opc[prefix + 1] >> 4], reg6309[opc[prefix + 1] & 15]);
            break;

        case ASM_ADDR_MODE_TFM_MM:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s-,%s-", reg6309[opc[prefix + 1] >> 4], reg6309[opc[prefix + 1] & 15]);
            break;

        case ASM_ADDR_MODE_TFM_PC:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s+,%s", reg6309[opc[prefix + 1] >> 4], reg6309[opc[prefix + 1] & 15]);
            break;

        case ASM_ADDR_MODE_TFM_CP:
            SKIP_PREFIX_6809();
            sprintf(buffp, " %s,%s+", reg6309[opc[prefix + 1] >> 4], reg6309[opc[prefix + 1] & 15]);
            break;

        case ASM_ADDR_MODE_SYS_POST:
        case ASM_ADDR_MODE_USR_POST:
            SKIP_PREFIX_6809();
            ival = opc[prefix + 1];
            strcat(buffp, " ");
            if (ival & 0x80) {
                strcat(buffp, "PC,");
            }
            if (ival & 0x40) {
                strcat(buffp, addr_mode == ASM_ADDR_MODE_USR_POST ? "S," : "U,");
            }
            if (ival & 0x20) {
                strcat(buffp, "Y,");
            }
            if (ival & 0x10) {
                strcat(buffp, "X,");
            }
            if (ival & 0x08) {
                strcat(buffp, "DP,");
            }
            if ((ival & 0x06) == 0x06) {
                strcat(buffp, "D,");
            } else {
                if (ival & 0x04) {
                    strcat(buffp, "B,");
                }
                if (ival & 0x02) {
                    strcat(buffp, "A,");
                }
            }
            if (ival & 0x01) {
                strcat(buffp, "CC,");
            }
            buffp[strlen(buffp) - 1] = '\0';
            break;
        case ASM_ADDR_MODE_DOUBLE: /* not a real addressing mode */
            break;
    }

    return buff;
}

#undef x
#undef p1
#undef p2
#undef p3
#undef p4

/*
 * Disassemble an instruction based on the current mem_config (implies bank
 * "cpu" but possibly differently configured).
 */
static const char* mon_disassemble_instr_memconfig_internal(unsigned *opc_size, MON_ADDR addr)
{
    static char buff[256];
    uint8_t opc[5];
    MEMSPACE mem;
    uint16_t loc;
    int hex_mode = 1;
    const char *dis_inst;
    int mem_config;

    mem = addr_memspace(addr);
    loc = addr_location(addr);
    mem_config = mem == e_comp_space ? mem_get_current_bank_config() : 0;

    opc[0] = mon_get_mem_val_nosfx(mem, mem_config, loc);
    opc[1] = mon_get_mem_val_nosfx(mem, mem_config, (uint16_t)(loc + 1));
    opc[2] = mon_get_mem_val_nosfx(mem, mem_config, (uint16_t)(loc + 2));
    opc[3] = mon_get_mem_val_nosfx(mem, mem_config, (uint16_t)(loc + 3));
    opc[4] = mon_get_mem_val_nosfx(mem, mem_config, (uint16_t)(loc + 4));

    dis_inst = mon_disassemble_to_string_internal(mem, loc, opc, hex_mode, opc_size, monitor_cpu_for_memspace[mem]);

    sprintf(buff, ".%s:%04x  %s", mon_memspace_string[mem], loc, dis_inst);

    return buff;
}

/*
 * Disassemble an instruction based on the currently set bank.
 */
static const char* mon_disassemble_instr_bank_internal(unsigned *opc_size, MON_ADDR addr)
{
    static char buff[256];
    uint8_t opc[5];
    MEMSPACE mem;
    uint16_t loc;
    int hex_mode = 1;
    const char *dis_inst;
    int bank;

    mem = addr_memspace(addr);
    loc = addr_location(addr);
    bank = mon_interfaces[mem]->current_bank;

    opc[0] = mon_get_mem_val_ex_nosfx(mem, bank, (uint16_t)(loc + 0));
    opc[1] = mon_get_mem_val_ex_nosfx(mem, bank, (uint16_t)(loc + 1));
    opc[2] = mon_get_mem_val_ex_nosfx(mem, bank, (uint16_t)(loc + 2));
    opc[3] = mon_get_mem_val_ex_nosfx(mem, bank, (uint16_t)(loc + 3));
    opc[4] = mon_get_mem_val_ex_nosfx(mem, bank, (uint16_t)(loc + 4));

    dis_inst = mon_disassemble_to_string_internal(mem, loc, opc, hex_mode, opc_size, monitor_cpu_for_memspace[mem]);

    sprintf(buff, ".%s:%04x  %s", mon_memspace_string[mem], loc, dis_inst);

    return buff;
}


/*
 * Used by DEBUG cpu trace.
 * Independent of memory config, since it receives the instruction's bytes
 * explicitly.
 */
const char *mon_disassemble_to_string(MEMSPACE memspace, unsigned int addr,
                                      unsigned int x, unsigned int p1, unsigned int p2, unsigned int p3,
                                      int hex_mode, const char *cpu_type)
{
    uint8_t opc[5];

    opc[0] = x;
    opc[1] = p1;
    opc[2] = p2;
    opc[3] = p3;
    opc[4] = 0;

    return mon_disassemble_to_string_internal(memspace, addr, opc, hex_mode, NULL, monitor_find_cpu_type_from_string(cpu_type));
}

/*
 * Used by monitor CPU history (chist).
 * Independent of memory config, since it receives the instruction's bytes
 * explicitly.
 */
const char *mon_disassemble_to_string_ex(MEMSPACE memspace, unsigned int addr,
                                         unsigned int x, unsigned int p1, unsigned int p2, unsigned int p3,
                                         int hex_mode, unsigned *opc_size_p)
{
    uint8_t opc[5];

    opc[0] = x;
    opc[1] = p1;
    opc[2] = p2;
    opc[3] = p3;
    opc[4] = 0;

    return mon_disassemble_to_string_internal(memspace, addr, opc, hex_mode, opc_size_p, monitor_cpu_for_memspace[memspace]);
}

/*
 * Used specifically by the monitor D command,
 * so it should use the monitor's currently set bank.
 */
static unsigned mon_disassemble_instr(MON_ADDR addr, int *line_count)
{
    MEMSPACE mem;
    uint16_t loc;
    char *label;
    unsigned opc_size;

    mem = addr_memspace(addr);
    loc = addr_location(addr);

    /* Print the label for this location - if we have one */
    label = mon_symbol_table_lookup_name(mem, loc);
    if (label) {
        mon_out(".%s:%04x   %s:\n", mon_memspace_string[mem], loc, label);
    }

    /* Print the disassembled instruction */
    mon_out("%s\n", mon_disassemble_instr_bank_internal(&opc_size, addr));

    /* a line with label takes two lines */
    if (line_count) {
        *line_count = label ? 2 : 1;
    }

    return opc_size;
}

/*
 * Used by monitor breakpoints and single step,
 * so it should use the current cpu's memory configuration.
 *
 * FIXME: both callers explicitly set mon_interfaces[...]->current_bank to
 * "cpu" bank but that is unneeded, since memory access based on mem_config
 * implies the "cpu" bank anyway (possibly differently configured).
 */
void mon_disassemble_with_regdump(MEMSPACE mem, unsigned int addr)
{
    monitor_cpu_type_t *monitor_cpu;
    const char *dis_inst;
    unsigned opc_size;

    monitor_cpu = monitor_cpu_for_memspace[mem];

    dis_inst = mon_disassemble_instr_memconfig_internal(&opc_size, addr);
    if (monitor_cpu->mon_register_print_ex) {
        mon_out("%-35s - %s ", dis_inst, monitor_cpu->mon_register_print_ex(mem));
    } else {
        mon_out("%s ", dis_inst);
    }
    mon_stopwatch_show("", "\n");
}

#define BAD_ADDR (new_addr(e_invalid_space, 0))

/*
 * Used specifically by the monitor D command,
 * so it should use the monitor's bank (mem_config).
 */
void mon_disassemble_lines(MON_ADDR start_addr, MON_ADDR end_addr)
{
    MEMSPACE mem;
    long len, i, bytes;
    int limitlines = (end_addr == BAD_ADDR);
    int linesleft;
    static int last_known_yres = 25;

    len = mon_evaluate_address_range(&start_addr, &end_addr, FALSE, DEFAULT_DISASSEMBLY_SIZE);
    if (console_log) {
        last_known_yres = console_log->console_yres;
    }
    linesleft = last_known_yres - 1;

    if (len < 0) {
        log_error(LOG_DEFAULT, "Invalid address range");
        return;
    }

    mem = addr_memspace(start_addr);
    dot_addr[mem] = start_addr;

    i = 0;
    while ((i < len) || (limitlines == 1)) {
        int line_count; /* Number of lines printed by disassembly */
        bytes = mon_disassemble_instr(dot_addr[mem], &line_count);
        i += bytes;
        mon_inc_addr_location(&(dot_addr[mem]), (unsigned int)bytes);
        if (mon_stop_output != 0) {
            break;
        }
        if (limitlines) {
            /* if there is a label on the NEXT line, we add an extra line so we
               abort early and do not scroll up the top line */
            if (mon_symbol_table_lookup_name(addr_memspace(dot_addr[mem]),
                                             addr_location(dot_addr[mem]))) {
                line_count++;
            }
            linesleft -= line_count;
            if (linesleft <= 0) {
                break;
            }
        }
    }
}
