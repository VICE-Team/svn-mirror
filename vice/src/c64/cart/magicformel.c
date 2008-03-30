/*
 * magicformel.c - Cartridge handling, Magic Formel cart.
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
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64mem.h"
#include "magicformel.h"
#include "types.h"
#include "vicii-phi1.h"


static const c64export_resource_t export_res = {
    "Magic Formel", 1, 1, 1, 1
};

static unsigned int ram_page = 0;


BYTE REGPARM1 magicformel_io1_read(WORD addr)
{
    return export_ram0[(ram_page << 8) + (addr & 0xff)];
}

void REGPARM2 magicformel_io1_store(WORD addr, BYTE value)
{
    export_ram0[(ram_page << 8) + (addr & 0xff)] = value;
}

BYTE REGPARM1 magicformel_io2_read(WORD addr)
{
    return vicii_read_phi1();
}

void REGPARM2 magicformel_io2_store(WORD addr, BYTE value)
{
/*log_debug("IO2 WRITE ADDR %02x", addr & 0xff);*/

    if ((addr & 0xf8) == 0)
        romh_bank = addr & 7;
    if ((addr & 0xff) == 0x11)
        cartridge_config_changed(2, 2, CMODE_READ);
    if ((addr & 0xff) >= 0x80)
        ram_page = addr & 0x1f;
}

BYTE REGPARM1 magicformel_roml_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 magicformel_roml_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

void REGPARM2 magicformel_romh_store(WORD addr, BYTE value)
{
    export_ram0[addr & 0x1fff] = value;
}

BYTE REGPARM1 magicformel_1000_7fff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 magicformel_1000_7fff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 magicformel_a000_bfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 magicformel_a000_bfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 magicformel_c000_cfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 magicformel_c000_cfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 magicformel_d000_dfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 magicformel_d000_dfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

void magicformel_freeze(void)
{
}

void magicformel_config_init(void)
{
    cartridge_config_changed(2, 3, CMODE_READ);
}

void magicformel_reset(void)
{
}

void magicformel_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x10000);
    memcpy(romh_banks, rawcart, 0x10000);
    cartridge_config_changed(2, 3, CMODE_READ);
}

int magicformel_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 7; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return -1;

        if (chipheader[0xb] > 7)
            return -1;

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1)
            return -1;
    }

    if (c64export_add(&export_res) < 0)
        return -1;

    return 0;
}

void magicformel_detach(void)
{
    c64export_remove(&export_res);
}

