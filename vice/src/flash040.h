/*
 * flash040.h - (AM)29F040(B) Flash emulation.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#ifndef VICE_FLASH040_H
#define VICE_FLASH040_H

#include "types.h"

enum flash040_type_s {
    FLASH040_TYPE_NORMAL,
    FLASH040_TYPE_B,
    FLASH040_TYPE_NUM
};
typedef enum flash040_type_s flash040_type_t;

enum flash040_state_s {
    FLASH040_STATE_READ,
    FLASH040_STATE_MAGIC_1,
    FLASH040_STATE_MAGIC_2,
    FLASH040_STATE_AUTOSELECT,
    FLASH040_STATE_BYTE_PROGRAM,
    FLASH040_STATE_BYTE_PROGRAM_HAPPENING,
    FLASH040_STATE_ERASE_MAGIC_1,
    FLASH040_STATE_ERASE_MAGIC_2,
    FLASH040_STATE_ERASE_SELECT,
    FLASH040_STATE_CHIP_ERASE,
    FLASH040_STATE_SECTOR_ERASE,
    FLASH040_STATE_SECTOR_ERASE_SUSPEND,
};
typedef enum flash040_state_s flash040_state_t;

typedef struct flash040_context_s {
    BYTE *flash_data;
    flash040_state_t flash_state;

    flash040_type_t flash_type;

    BYTE last_read;
} flash040_context_t;

extern void flash040core_init(struct flash040_context_s *flash040_context,
                              flash040_type_t type, BYTE *data);
extern void flash040core_shutdown(struct flash040_context_s *flash040_context);
extern void flash040core_reset(struct flash040_context_s *flash040_context);

extern void REGPARM3 flash040core_store(struct flash040_context_s *flash040_context,
                                        unsigned int addr, BYTE data);
extern BYTE REGPARM2 flash040core_read(struct flash040_context_s *flash040_context,
                                       unsigned int addr);
extern BYTE REGPARM2 flash040core_peek(struct flash040_context_s *flash040_context,
                                       unsigned int addr);

#endif

