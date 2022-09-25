/*
 * c128cart.h -- c128 cartridge memory interface.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#ifndef VICE_C128CART_H
#define VICE_C128CART_H

extern int c128cartridge_cmdline_options_init(void);
extern int c128cartridge_resources_init(void);
extern void c128cartridge_resources_shutdown(void);

/* only x128 actually implements this function */
extern void c128cartridge_setup_interface(void);

/* Image of the external function ROM.  */
#define EXTERNAL_FUNCTION_ROM_SIZE  0x8000
#define EXTERNAL_FUNCTION_ROM_BANKS 8       /* max: comal80 has 6 */
extern uint8_t ext_function_rom[EXTERNAL_FUNCTION_ROM_SIZE * EXTERNAL_FUNCTION_ROM_BANKS];
extern uint8_t ext_function_rom_bank;

extern uint8_t external_function_rom_read(uint16_t addr);
extern void external_function_rom_store(uint16_t addr, uint8_t value);
extern void external_function_top_shared_store(uint16_t addr, uint8_t value);

// /* FIXME: get rid of these */
// #define EXT_FUNCTION_NONE   0
// #define EXT_FUNCTION_ROM    1

#endif
