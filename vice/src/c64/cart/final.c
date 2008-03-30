/*
 * final.c - Cartridge handling, Final cart.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64mem.h"
#include "cartridge.h"
#include "final.h"
#include "interrupt.h"
#include "types.h"


static const c64export_resource_t export_res_v1 = {
    "Final V1", 1, 1, 1, 0
};

static const c64export_resource_t export_res_v3 = {
    "Final V3", 1, 1, 1, 0
};


BYTE REGPARM1 final_v1_io1_read(WORD addr)
{
    cartridge_config_changed(0x42, 0x42, CMODE_READ);
    return roml_banks[0x1e00 + (addr & 0xff)];
}

void REGPARM2 final_v1_io1_store(WORD addr, BYTE value)
{
    cartridge_config_changed(0x42, 0x42, CMODE_WRITE);
}

BYTE REGPARM1 final_v1_io2_read(WORD addr)
{
    cartridge_config_changed(1, 1, CMODE_READ);
    return roml_banks[0x1f00 + (addr & 0xff)];
}

void REGPARM2 final_v1_io2_store(WORD addr, BYTE value)
{
    cartridge_config_changed(1, 1, CMODE_WRITE);
}

BYTE REGPARM1 final_v3_io1_read(WORD addr)
{
    return roml_banks[0x1e00 + (roml_bank << 13) + (addr & 0xff)];
}

void REGPARM2 final_v3_io1_store(WORD addr, BYTE value)
{
}

BYTE REGPARM1 final_v3_io2_read(WORD addr)
{
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
    return 0;
}

void REGPARM2 final_v3_io2_store(WORD addr, BYTE value)
{
    if ((addr & 0xff) == 0xff)  {
        /* FIXME: Change this to call `cartridge_config_changed'.  */
        cartridge_romhbank_set(value & 3);
        cartridge_romlbank_set(value & 3);
        export.game = ((value >> 5) & 1) ^ 1;
        export.exrom = ((value >> 4) & 1) ^ 1;
        mem_pla_config_changed();
        cart_ultimax_phi1 = export.game & (export.exrom ^ 1);
        cart_ultimax_phi2 = export.game & (export.exrom ^ 1);
        if ((value & 0x30) == 0x10)
            cartridge_trigger_freeze_nmi_only();
        if (value & 0x40)
            cartridge_release_freeze();
    }
}

BYTE REGPARM1 final_v1_roml_read(WORD addr)
{
    if (export_ram)
        return export_ram0[addr & 0x1fff];

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 final_v1_roml_store(WORD addr, BYTE value)
{
    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
}

BYTE REGPARM1 final_v3_roml_read(WORD addr)
{
    if (export_ram)
        return export_ram0[addr & 0x1fff];

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 final_v3_roml_store(WORD addr, BYTE value)
{
    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
}

void final_v1_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
}

void final_v3_freeze(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
}

void final_v1_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
}

void final_v3_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
}

void final_v1_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(1, 1, CMODE_READ);
}

void final_v3_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
    memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
    memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
    memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
    memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
    memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
    cartridge_config_changed(1, 1, CMODE_READ);
}

int final_v1_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1)
        return -1;

    if (chipheader[0xc] != 0x80 || chipheader[0xe] != 0x40)
        return -1;

    if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1)
        return -1;

    if (c64export_add(&export_res_v1) < 0)
        return -1;

    return 0;
}

int final_v3_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 3; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return -1;

        if (chipheader[0xb] > 3)
            return -1;

        if (fread(&rawcart[chipheader[0xb] << 14], 0x4000, 1, fd) < 1)
            return -1;
    }

    if (c64export_add(&export_res_v3) < 0)
        return -1;

    return 0;
}

void final_v1_detach(void)
{
    c64export_remove(&export_res_v1);
}

void final_v3_detach(void)
{
    c64export_remove(&export_res_v3);
}

