/*
 * vic20cartmem.h -- VIC20 Cartridge memory handling.
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

#ifndef VICE_VIC20CARTMEM_H
#define VICE_VIC20CARTMEM_H

#include "types.h"

#define CART_READ_VIC20MEM             -1
#define CART_READ_THROUGH               0
#define CART_READ_VALID                 1

extern int mem_cartridge_type;
extern int mem_cart_blocks;

uint8_t cartridge_read_ram123(uint16_t addr);
uint8_t cartridge_peek_ram123(uint16_t addr);
void cartridge_store_ram123(uint16_t addr, uint8_t value);
uint8_t cartridge_read_blk1(uint16_t addr);
uint8_t cartridge_peek_blk1(uint16_t addr);
void cartridge_store_blk1(uint16_t addr, uint8_t value);
uint8_t cartridge_read_blk2(uint16_t addr);
uint8_t cartridge_peek_blk2(uint16_t addr);
void cartridge_store_blk2(uint16_t addr, uint8_t value);
uint8_t cartridge_read_blk3(uint16_t addr);
uint8_t cartridge_peek_blk3(uint16_t addr);
void cartridge_store_blk3(uint16_t addr, uint8_t value);
uint8_t cartridge_read_blk5(uint16_t addr);
uint8_t cartridge_peek_blk5(uint16_t addr);
void cartridge_store_blk5(uint16_t addr, uint8_t value);

#endif
