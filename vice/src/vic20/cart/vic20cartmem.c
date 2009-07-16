/*
 * vic20cartmem.c -- VIC20 Cartridge memory handling.
 *
 * Written by
 *  Daniel Kahlin <daniel@kahlin.net>
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

#include "cartridge.h"
#include "generic.h"
#include "megacart.h"
#include "machine.h"

#ifdef WATCOM_COMPILE
#include "../mem.h"
#else
#include "mem.h"
#endif

#include "resources.h"
#include "types.h"
#include "vic20cartmem.h"

/* ------------------------------------------------------------------------- */

int mem_cartridge_type = CARTRIDGE_NONE;
int mem_cart_blocks = 0;

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 cartridge_read_io2(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_io2_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_io2(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        megacart_io2_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_io3(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_io3_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_io3(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        megacart_io3_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_ram123(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_ram123_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_ram123_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_ram123(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_ram123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_ram123_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk1(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk1_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_mem_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_blk1(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk1_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_mem_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk2(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk2_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_mem_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_blk2(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk2_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_mem_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk3(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk3_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_mem_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_blk3(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk3_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_mem_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk5(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk5_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_mem_read(addr);
        break;
    }
    return 0xff;
}

void REGPARM2 cartridge_store_blk5(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk5_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_mem_store(addr, value);
        break;
    }
}

/* ------------------------------------------------------------------------- */

void cartridge_init(void)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_init();
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_init();
        break;
    }
}

void cartridge_reset(void)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_reset();
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_reset();
        break;
    }
}

void cartridge_attach(int type, BYTE *rawcart)
{
    int cartridge_reset;

    mem_cartridge_type = type;
#if 0
    switch (type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_config_setup(rawcart);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_config_setup(rawcart);
        break;
    default:
        mem_cartridge_type = CARTRIDGE_NONE;
    }
#endif

    resources_get_int("CartridgeReset", &cartridge_reset);

    if (cartridge_reset != 0) {
        /* "Turn off machine before inserting cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }
}

void cartridge_detach(int type)
{
    int cartridge_reset;

    switch (type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_detach();
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_detach();
        break;
    }
    mem_cartridge_type = CARTRIDGE_NONE;
    /* this is probably redundant as it is also performed by the
       local detach functions. */
    mem_cart_blocks = 0;
    mem_initialize_memory();

    resources_get_int("CartridgeReset", &cartridge_reset);

    if (cartridge_reset != 0) {
        /* "Turn off machine before removing cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }
}

/* ------------------------------------------------------------------------- */
