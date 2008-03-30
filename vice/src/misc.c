/*
 * misc.c - Miscellaneous functions for debugging.
 *
 * Written by
 *  Vesa-Matti Puro (vmp@lut.fi)
 *  Jarkko Sonninen (sonninen@lut.fi)
 *  Jouko Valta     (jopi@stekt.oulu.fi)
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

/*
 * This file contains misc funtions to help debugging.
 * Included are:
 *	o Show numeric conversions
 *	o Show CPU registers
 *	o Show Stack contents
 *	o Print binary number
 *	o Print instruction hexas from memory
 *	o Print instruction from memory
 *	o Decode instruction
 *	o Find effective address for operand
 *	o Create a copy of string
 *	o Move memory
 *
 * sprint_opcode returns mnemonic code of machine instruction.
 * sprint_binary returns binary form of given code (8bit)
 *
 */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "maincpu.h"
#include "misc.h"
#include "asm.h"
#include "resources.h"
#include "mon.h"

char   *sprint_binary(BYTE code)
{
    static char bin[9];
    int     i;

    bin[8] = 0;		/* Terminator. */

    for (i = 0; i < 8; i++) {
	bin[i] = (code & 128) ? '1' : '0';
	code <<= 1;
    }

    return bin;
}


/* ------------------------------------------------------------------------- */

char   *sprint_opcode(ADDRESS counter, int base)
{
    BYTE    x = mem_read(counter);
    BYTE    p1 = mem_read(counter + 1);
    BYTE    p2 = mem_read(counter + 2);

    return sprint_disassembled(counter, x, p1, p2, base);
}


char   *sprint_disassembled(ADDRESS counter,
                            BYTE x, BYTE p1, BYTE p2, int hex_mode)
{
    static char buff[20];
    const char *string;
    char *buffp, *addr_name;
    int addr_mode;
    int ival;

    ival = p1 & 0xFF;

    buffp = buff;
    string = lookup[x].mnemonic;
    addr_mode = lookup[x].addr_mode;

    sprintf(buff, "$%02X %s", x, string); /* Print opcode and mnemonic. */
    while (*++buffp);

    switch (addr_mode) {
	/*
	 * Bits 0 and 1 are usual marks for X and Y indexed addresses, i.e.
	 * if  bit #0 is set addressing mode is X indexed something and if
	 * bit #1 is set addressing mode is Y indexed something. This is not
	 * from MOS6502, but convention used in this program. See
	 * "vmachine.h" for details.
	 */

	/* Print arguments of the machine instruction. */

      case IMPLIED:
	break;

      case ACCUMULATOR:
	sprintf(buffp, " A");
	break;

      case IMMEDIATE:
	sprintf(buffp, (hex_mode ? " #$%02X" : " %3d"), ival);
	break;

      case ZERO_PAGE:
	sprintf(buffp, (hex_mode ? " $%02X" : " %3d"), ival);
	break;

      case ZERO_PAGE_X:
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " $%02X,X" : " %3d,X"), ival);
        else
	   sprintf(buffp, " %s,X", addr_name);
	break;

      case ZERO_PAGE_Y:
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " $%02X,Y" : " %3d,Y"), ival);
        else
	   sprintf(buffp, " %s,Y", addr_name);
	break;

      case ABSOLUTE:
	ival |= ((p2 & 0xFF) << 8);
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " $%04X" : " %5d"), ival);
        else
	   sprintf(buffp, " %s", addr_name);
	break;

      case ABSOLUTE_X:
	ival |= ((p2 & 0xFF) << 8);
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " $%04X,X" : " %5d,X"), ival);
        else
	   sprintf(buffp, " %s,X", addr_name);
	break;

      case ABSOLUTE_Y:
	ival |= ((p2 & 0xFF) << 8);
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " $%04X,Y" : " %5d,Y"), ival);
        else
	   sprintf(buffp, " %s,Y", addr_name);
	break;

      case INDIRECT_X:
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " ($%02X,X)" : " (%3d,X)"), ival);
        else
	   sprintf(buffp, " (%s,X)", addr_name);
	break;

      case INDIRECT_Y:
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " ($%02X),Y" : " (%3d),Y"), ival);
        else
	   sprintf(buffp, " (%s),Y", addr_name);
	break;

      case ABS_INDIRECT:
	ival |= ((p2 & 0xFF) << 8);
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " ($%04X)" : " (%5d)"), ival);
        else
	   sprintf(buffp, " (%s)", addr_name);
	break;

      case RELATIVE:
	if (0x80 & ival)
	    ival -= 256;
	ival += counter;
	ival += 2;
        if ( !(addr_name = mon_symbol_table_lookup_name(e_comp_space, ival)) )
	   sprintf(buffp, (hex_mode ? " $%04X" : " %5d"), ival);
        else
	   sprintf(buffp, " %s", addr_name);
	break;
    }

    return buff;
}
