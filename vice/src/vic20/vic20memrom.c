/*
 * vic20memrom.c -- VIC20 ROM access.
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

#include "vic20mem.h"
#include "vic20memrom.h"
#include "types.h"


BYTE vic20memrom_kernal_rom[VIC20_KERNAL_ROM_SIZE];
BYTE vic20memrom_kernal_trap_rom[VIC20_KERNAL_ROM_SIZE];
BYTE vic20memrom_basic_rom[VIC20_BASIC_ROM_SIZE];
/* The second 0x400 handles a possible segfault by a wraparound of the
   chargen by setting it to $8c00.  FIXME: This does not cause the exact
   behavior to be emulated though!  */
BYTE vic20memrom_chargen_rom[0x400 + VIC20_CHARGEN_ROM_SIZE + 0x400];


BYTE REGPARM1 vic20memrom_kernal_read(WORD addr)
{
    return vic20memrom_kernal_rom[addr & 0x1fff];
}

BYTE REGPARM1 vic20memrom_basic_read(WORD addr)
{
    return vic20memrom_basic_rom[addr & 0x1fff];
}

BYTE REGPARM1 vic20memrom_chargen_read(WORD addr)
{
    return vic20memrom_chargen_rom[0x400 + (addr & 0xfff)];
}

BYTE REGPARM1 vic20memrom_trap_read(WORD addr)
{
    switch (addr & 0xf000) {
      case 0xe000:
      case 0xf000:
        return vic20memrom_kernal_trap_rom[addr & 0x1fff];
    }

    return 0;
}

void REGPARM2 vic20memrom_trap_store(WORD addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0xe000:
      case 0xf000:
        vic20memrom_kernal_trap_rom[addr & 0x1fff] = value;
        break;
    }
}

BYTE REGPARM1 rom_read(WORD addr)
{
    switch (addr & 0xf000) {
      case 0x8000:
        return vic20memrom_chargen_read(addr);
      case 0xc000:
      case 0xd000:
        return vic20memrom_basic_read(addr);
      case 0xe000:
      case 0xf000:
        return vic20memrom_kernal_read(addr);
    }

    return 0;
}

void REGPARM2 rom_store(WORD addr, BYTE value)
{
    switch (addr & 0xf000) {
      case 0x8000:
        vic20memrom_chargen_rom[0x400 + (addr & 0x0fff)] = value;
        break;
      case 0xc000:
      case 0xd000:
        vic20memrom_basic_rom[addr & 0x1fff] = value;
        break;
      case 0xe000:
      case 0xf000:
        vic20memrom_kernal_rom[addr & 0x1fff] = value;
        break;
    }
}

