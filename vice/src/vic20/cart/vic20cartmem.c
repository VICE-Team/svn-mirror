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
#include "finalexpansion.h"
#include "generic.h"
#include "vic-fp.h"
#include "megacart.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "types.h"
#include "vic20mem.h"
#include "vic20cartmem.h"

/* ------------------------------------------------------------------------- */

int mem_cartridge_type = CARTRIDGE_NONE;
int mem_cart_blocks = 0;

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 cartridge_read_ram123(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        vic20_cpu_last_data = generic_ram123_read(addr);
        break;
    case CARTRIDGE_VIC20_FP:
        vic20_cpu_last_data = vic_fp_ram123_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_ram123_read(addr);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        vic20_cpu_last_data = finalexpansion_ram123_read(addr);
        break;
    default:
        vic20_cpu_last_data = vic20_v_bus_last_data;
        break;
    }
    vic20_mem_v_bus_read(addr);
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_ram123(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_ram123_read(addr);
    case CARTRIDGE_VIC20_FP:
        return vic_fp_ram123_peek(addr);
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_ram123_peek(addr);
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        return finalexpansion_ram123_read(addr);
    default:
        break;
    }
    return 0;
}

void REGPARM2 cartridge_store_ram123(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_ram123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FP:
        vic_fp_ram123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_ram123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_ram123_store(addr, value);
        break;
    }
    vic20_mem_v_bus_store(addr);
}

BYTE REGPARM1 cartridge_read_blk1(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        vic20_cpu_last_data = generic_blk1_read(addr);
        break;
    case CARTRIDGE_VIC20_FP:
        vic20_cpu_last_data = vic_fp_blk1_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_blk123_read(addr);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        vic20_cpu_last_data = finalexpansion_blk1_read(addr);
        break;
    }
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_blk1(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk1_read(addr);
    case CARTRIDGE_VIC20_FP:
        return vic_fp_blk1_peek(addr);
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_blk123_read(addr);
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        return finalexpansion_blk1_read(addr);
    }
    return 0;
}

void REGPARM2 cartridge_store_blk1(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk1_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FP:
        vic_fp_blk1_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_blk123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_blk1_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk2(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        vic20_cpu_last_data = generic_blk2_read(addr);
        break;
    case CARTRIDGE_VIC20_FP:
        vic20_cpu_last_data = vic_fp_blk23_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_blk123_read(addr);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        vic20_cpu_last_data = finalexpansion_blk2_read(addr);
        break;
    }
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_blk2(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk2_read(addr);
    case CARTRIDGE_VIC20_FP:
        return vic_fp_blk23_read(addr);
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_blk123_read(addr);
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        return finalexpansion_blk2_read(addr);
    }
    return 0;
}

void REGPARM2 cartridge_store_blk2(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk2_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FP:
        vic_fp_blk23_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_blk123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_blk2_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk3(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        vic20_cpu_last_data = generic_blk3_read(addr);
        break;
    case CARTRIDGE_VIC20_FP:
        vic20_cpu_last_data = vic_fp_blk23_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_blk123_read(addr);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        vic20_cpu_last_data = finalexpansion_blk3_read(addr);
        break;
    }
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_blk3(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk3_read(addr);
    case CARTRIDGE_VIC20_FP:
        return vic_fp_blk23_read(addr);
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_blk123_read(addr);
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        return finalexpansion_blk3_read(addr);
    }
    return 0;
}

void REGPARM2 cartridge_store_blk3(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk3_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FP:
        vic_fp_blk23_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_blk123_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_blk3_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_blk5(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        vic20_cpu_last_data = generic_blk5_read(addr);
        break;
    case CARTRIDGE_VIC20_FP:
        vic20_cpu_last_data = vic_fp_blk5_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_blk5_read(addr);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        vic20_cpu_last_data = finalexpansion_blk5_read(addr);
        break;
    }
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_blk5(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        return generic_blk5_read(addr);
    case CARTRIDGE_VIC20_FP:
        return vic_fp_blk5_peek(addr);
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_blk5_read(addr);
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        return finalexpansion_blk5_read(addr);
    }
    return 0;
}

void REGPARM2 cartridge_store_blk5(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_blk5_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FP:
        vic_fp_blk5_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_blk5_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_blk5_store(addr, value);
        break;
    }
}

BYTE REGPARM1 cartridge_read_io2(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_FP:
        vic20_cpu_last_data = vic_fp_io2_read(addr);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_io2_read(addr);
        break;
    default:
        break;
    }
    vic20_mem_v_bus_read(addr);
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_io2(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_FP:
        return vic_fp_io2_peek(addr);
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_io2_peek(addr);
    default:
        break;
    }
    return 0;
}

void REGPARM2 cartridge_store_io2(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_FP:
        vic_fp_io2_store(addr, value);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_io2_store(addr, value);
        break;
    }
    vic20_mem_v_bus_store(addr);
}

BYTE REGPARM1 cartridge_read_io3(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        vic20_cpu_last_data = megacart_io3_read(addr);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        vic20_cpu_last_data = finalexpansion_io3_read(addr);
        break;
    default:
        break;
    }
    vic20_mem_v_bus_read(addr);
    return vic20_cpu_last_data;
}

BYTE REGPARM1 cartridge_peek_io3(WORD addr)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        return megacart_io3_peek(addr);
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        return finalexpansion_io3_peek(addr);
    default:
        break;
    }
    return 0;
}

void REGPARM2 cartridge_store_io3(WORD addr, BYTE value)
{
    vic20_cpu_last_data = value;
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_MEGACART:
        megacart_io3_store(addr, value);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_io3_store(addr, value);
        break;
    }
    vic20_mem_v_bus_store(addr);
}

/* ------------------------------------------------------------------------- */

void cartridge_init(void)
{
    generic_init();
    megacart_init();
    finalexpansion_init();
    vic_fp_init();
}

void cartridge_reset(void)
{
    switch (mem_cartridge_type) {
    case CARTRIDGE_VIC20_GENERIC:
        generic_reset();
        break;
    case CARTRIDGE_VIC20_FP:
        vic_fp_reset();
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_reset();
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_reset();
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
    case CARTRIDGE_VIC20_FP:
        vic_fp_config_setup(rawcart);
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_config_setup(rawcart);
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_config_setup(rawcart);
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
    case CARTRIDGE_VIC20_FP:
        vic_fp_detach();
        break;
    case CARTRIDGE_VIC20_MEGACART:
        megacart_detach();
        break;
    case CARTRIDGE_VIC20_FINAL_EXPANSION:
        finalexpansion_detach();
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
