/*
 * c64cartmem.c -- C64 cartridge handling.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#endif

#include "c64cart.h"
#include "c64mem.h"
#include "cartridge.h"
#include "interrupt.h"

/* Expansion port signals.  */
export_t export;

/* Exansion port ROML/ROMH images.  */
#ifdef AVOID_STATIC_ARRAYS
BYTE *roml_banks, *romh_banks;
#else
BYTE roml_banks[0x20000], romh_banks[0x20000];
#endif

/* Exansion port RAM images.  */
#ifdef AVOID_STATIC_ARRAYS
BYTE *export_ram0;
#else
BYTE export_ram0[0x2000];
#endif

/* Expansion port ROML/ROMH/RAM banking.  */
int roml_bank = 0, romh_bank = 0, export_ram = 0;

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
int ultimax = 0;

/* Super Snapshot configuration flags.  */
static BYTE ramconfig = 0xff, romconfig = 9;

/* Type of the cartridge attached.  */
int mem_cartridge_type = CARTRIDGE_NONE;

static void cartridge_config_changed(BYTE mode)
{
    export.game = mode & 1;
    export.exrom = ((mode >> 1) & 1) ^ 1;
    romh_bank = roml_bank = (mode >> 3) & 3;
    export_ram = (mode >> 5) & 1;
    pla_config_changed();
    if (mode & 0x40)
        cartridge_release_freeze();
    ultimax = export.game & (export.exrom ^ 1);
}

BYTE REGPARM1 cartridge_read_io1(ADDRESS addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        return rand();
      case CARTRIDGE_KCS_POWER:
        cartridge_config_changed(0);
        return roml_banks[0x1e00 + (addr & 0xff)];
      case CARTRIDGE_FINAL_III:
        return roml_banks[0x1e00 + (roml_bank << 13) + (addr & 0xff)];
      case CARTRIDGE_SIMONS_BASIC:
        cartridge_config_changed(0);
        return rand();
      case CARTRIDGE_SUPER_SNAPSHOT:
        return export_ram0[0x1e00 + (addr & 0xff)];
    }
    return rand();
}

void REGPARM2 cartridge_store_io1(ADDRESS addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        cartridge_config_changed(value);
        break;
      case CARTRIDGE_KCS_POWER:
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_SIMONS_BASIC:
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        export_ram0[0x1e00 + (addr & 0xff)] = value;
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        switch (mem_cartridge_type) {
          case CARTRIDGE_OCEAN:
            romh_bank = roml_bank = value & 15;
            break;
          case CARTRIDGE_FUNPLAY:
            romh_bank = roml_bank = ((value >> 2) | (value & 1)) & 15;
            break;
        }
        if (value & 0x80) {
            export.game = (value >> 4) & 1;
            export.exrom = 1;
        } else {
            export.game = export.exrom = 1;
        }
        pla_config_changed();
        ultimax = 0;
        break;
    }
    return;
}

BYTE REGPARM1 cartridge_read_io2(ADDRESS addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
      case CARTRIDGE_SUPER_SNAPSHOT:
      case CARTRIDGE_FINAL_III:
        if (mem_cartridge_type == CARTRIDGE_SUPER_SNAPSHOT
                               && (addr & 0xff) == 1)
            return ramconfig;
        if (export_ram && mem_cartridge_type == CARTRIDGE_ACTION_REPLAY)
            return export_ram0[0x1f00 + (addr & 0xff)];
        switch (roml_bank) {
          case 0:
            return roml_banks[addr & 0x1fff];
          case 1:
            return roml_banks[(addr & 0x1fff) + 0x2000];
          case 2:
            return roml_banks[(addr & 0x1fff) + 0x4000];
          case 3:
            return roml_banks[(addr & 0x1fff) + 0x6000];
        }
        break;
      case CARTRIDGE_KCS_POWER:
            return export_ram0[0x1f00 + (addr & 0xff)];
    }
    return rand();
}

void REGPARM2 cartridge_store_io2(ADDRESS addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        if (export_ram)
            export_ram0[0x1f00 + (addr & 0xff)] = value;
        break;
      case CARTRIDGE_KCS_POWER:
        export_ram0[0x1f00 + (addr & 0xff)] = value;
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        if ((addr & 0xff) == 0) {
            romconfig = (value == 2) ? 1 : 9;
            romconfig = (romconfig & 0xdf) | ((ramconfig == 0) ? 0x20 : 0);
            if ((value & 0x7f) == 0)
                romconfig = 35;
            if ((value & 0x7f) == 1 || (value & 0x7f) == 3)
                romconfig = 0;
            if ((value & 0x7f) == 6) {
                romconfig = 9;
                cartridge_release_freeze();
            }
            if ((value & 0x7f) == 9)
                romconfig = 6;
            cartridge_config_changed(romconfig);
        }
        if ((addr & 0xff) == 1) {
            if(((ramconfig - 1) & 0xff) == value) {
                ramconfig = value;
                romconfig |= 35;
            }
            if(((ramconfig + 1) & 0xff) == value) {
                ramconfig = value;
                romconfig &= 0xdd;
            }
            cartridge_config_changed(romconfig);
        }
        break;
      case CARTRIDGE_FINAL_III:
        if ((addr & 0xff) == 0xff)  {
            /* FIXME: Change this to call `cartridge_config_changed'.  */
            romh_bank = roml_bank = value & 3;
            export.game = ((value >> 5) & 1) ^ 1;
            export.exrom = ((value >> 4) & 1) ^ 1;
            pla_config_changed();
            ultimax = export.game & (export.exrom ^ 1);
            if ((value & 0x30) == 0x10)
                maincpu_set_nmi(I_FREEZE, IK_NMI);
            if (value & 0x40)
                cartridge_release_freeze();
        }
        break;
      case CARTRIDGE_SUPER_GAMES:
        romh_bank = roml_bank = value & 3;
        if (value & 0x4) {
            export.game = 0;
            export.exrom = 1;
        } else {
            export.game = export.exrom = 1;
        }
        if (value == 0xc)
            export.game = export.exrom = 0;
        pla_config_changed();
        break;
    }
    return;
}

BYTE REGPARM1 read_roml(ADDRESS addr)
{
    if (export_ram)
        return export_ram0[addr & 0x1fff];
    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

BYTE REGPARM1 read_romh(ADDRESS addr)
{
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

void REGPARM2 store_roml(ADDRESS addr, BYTE value)
{
    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
    return;
}

void cartridge_init_config(void)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
      case CARTRIDGE_KCS_POWER:
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_SUPER_GAMES:
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_FINAL_III:
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GENERIC_16KB:
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_ULTIMAX:
        cartridge_config_changed(3);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        cartridge_config_changed(9);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        cartridge_config_changed(1);
        store_io1((ADDRESS) 0xde00, 0);
        break;
    }
}

void cartridge_attach(int type, BYTE *rawcart)
{
    mem_cartridge_type = type;
    roml_bank = romh_bank = 0;
    switch (type) {
      case CARTRIDGE_GENERIC_8KB:
        memcpy(roml_banks, rawcart, 0x2000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GENERIC_16KB:
        memcpy(roml_banks, rawcart, 0x2000);
        memcpy(romh_banks, &rawcart[0x2000], 0x2000);
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_ACTION_REPLAY:
        memcpy(roml_banks, rawcart, 0x8000);
        memcpy(romh_banks, rawcart, 0x8000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_KCS_POWER:
        memcpy(roml_banks, rawcart, 0x2000);
        memcpy(romh_banks, &rawcart[0x2000], 0x2000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_FINAL_III:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
        memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
        memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
        memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        cartridge_config_changed(9);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        memcpy(roml_banks, rawcart, 0x2000 * 16);
        memcpy(romh_banks, &rawcart[0x2000 * 16], 0x2000 * 16);
        break;
      case CARTRIDGE_ULTIMAX:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x0000], 0x2000);
        cartridge_config_changed(3);
        break;
      case CARTRIDGE_SUPER_GAMES:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
        memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
        memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
        memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
        cartridge_config_changed(0);
        break;
      default:
        mem_cartridge_type = CARTRIDGE_NONE;
    }
    return;
}

void cartridge_detach(int type)
{
    cartridge_config_changed(6);
    mem_cartridge_type = CARTRIDGE_NONE;
    return;
}

void cartridge_freeze(int type)
{
    if (type == CARTRIDGE_ACTION_REPLAY || type == CARTRIDGE_SUPER_SNAPSHOT)
        cartridge_config_changed(35);
    if (type == CARTRIDGE_KCS_POWER || type == CARTRIDGE_FINAL_III)
        cartridge_config_changed(3);
}

