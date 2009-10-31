/*
 * flash040core.c - (AM)29F040(B) Flash emulation.
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

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "flash040.h"
#include "lib.h"
#include "log.h"
#include "maincpu.h"
#include "snapshot.h"
#include "types.h"

/* -------------------------------------------------------------------------- */

/*#define FLASH_DEBUG_ENABLED*/

#ifdef FLASH_DEBUG_ENABLED
#define FLASH_DEBUG(x) log_debug x
#else
#define FLASH_DEBUG(x)
#endif


struct flash_types_s {
    BYTE manufacturer_ID;
    BYTE device_ID;
    unsigned int size;
    unsigned int magic_1_addr;
    unsigned int magic_2_addr;
    unsigned int magic_1_mask;
    unsigned int magic_2_mask;
};
typedef struct flash_types_s flash_types_t;

static flash_types_t flash_types[FLASH040_TYPE_NUM] = {
    { 0x01, 0xa4, 0x80000, 0x5555, 0x2aaa, 0x7fff, 0x7fff },
    { 0x01, 0xa4, 0x80000, 0x555,  0x2aa,  0x7ff,  0x7ff  }
};

/* -------------------------------------------------------------------------- */

inline static int flash_magic_1(flash040_context_t *flash040_context, unsigned int addr)
{
    return ((addr & flash_types[flash040_context->flash_type].magic_1_mask) == flash_types[flash040_context->flash_type].magic_1_addr);
}

inline static int flash_magic_2(flash040_context_t *flash040_context, unsigned int addr)
{
    return ((addr & flash_types[flash040_context->flash_type].magic_2_mask) == flash_types[flash040_context->flash_type].magic_2_addr);
}

inline static void flash_erase_sector(flash040_context_t *flash040_context, unsigned int addr)
{
    unsigned int sector_addr = addr & 0xf0000;

    FLASH_DEBUG(("Erasing 0x%xXXXX", sector_addr >> 16));
    memset(&(flash040_context->flash_data[sector_addr]), 0xff, 0x10000);
    flash040_context->flash_dirty = 1;
}

inline static void flash_erase_chip(flash040_context_t *flash040_context)
{
    FLASH_DEBUG(("Erasing chip"));
    memset(flash040_context->flash_data, 0xff, flash_types[flash040_context->flash_type].size);
    flash040_context->flash_dirty = 1;
}

inline static int flash_program_byte(flash040_context_t *flash040_context, unsigned int addr, BYTE byte)
{
    BYTE old_data = flash040_context->flash_data[addr];
    BYTE new_data = old_data & byte;

    FLASH_DEBUG(("Programming 0x%05x with 0x%02x (%02x->%02x)", addr, byte, old_data, old_data & byte));
    flash040_context->program_byte = byte;
    flash040_context->flash_data[addr] = new_data;
    flash040_context->flash_dirty = 1;

    return (new_data == byte) ? 1 : 0;
}

inline static int flash_write_operation_status(flash040_context_t *flash040_context)
{
    return ((flash040_context->program_byte ^ 0x80) & 0x80)   /* DQ7 = inverse of programmed data */
         | ((maincpu_clk & 2) << 5)                           /* DQ6 = toggle bit (2 us) */
         | (1 << 5)                                           /* DQ5 = timeout */
         ;
}

static void REGPARM3 flash040core_store_internal(flash040_context_t *flash040_context,
                                                 unsigned int addr, BYTE byte)
{
#ifdef FLASH_DEBUG_ENABLED
    flash040_state_t old_state = flash040_context->flash_state;
#endif

    switch (flash040_context->flash_state) {
        case FLASH040_STATE_READ:
            if (flash_magic_1(flash040_context, addr) && (byte == 0xaa)) {
                flash040_context->flash_state = FLASH040_STATE_MAGIC_1;
            }
            break;

        case FLASH040_STATE_MAGIC_1:
            if (flash_magic_2(flash040_context, addr) && (byte == 0x55)) {
                flash040_context->flash_state = FLASH040_STATE_MAGIC_2;
            } else {
                flash040_context->flash_state = FLASH040_STATE_READ;
            }
            break;

        case FLASH040_STATE_MAGIC_2:
            if (flash_magic_1(flash040_context, addr)) {
                switch (byte) {
                    case 0x90:
                        flash040_context->flash_state = FLASH040_STATE_AUTOSELECT;
                        break;
                    case 0xa0:
                        flash040_context->flash_state = FLASH040_STATE_BYTE_PROGRAM;
                        break;
                    case 0x80:
                        flash040_context->flash_state = FLASH040_STATE_ERASE_MAGIC_1;
                        break;
                    default:
                        flash040_context->flash_state = FLASH040_STATE_READ;
                        break;
                }
            } else {
                flash040_context->flash_state = FLASH040_STATE_READ;
            }
            break;

        case FLASH040_STATE_BYTE_PROGRAM:
            /* TODO setup alarm to not have the operation complete instantly */
            if (flash_program_byte(flash040_context, addr, byte)) {
                flash040_context->flash_state = FLASH040_STATE_BYTE_PROGRAM_HAPPENING;
            } else {
                flash040_context->flash_state = FLASH040_STATE_BYTE_PROGRAM_ERROR;
            }
            break;

        case FLASH040_STATE_ERASE_MAGIC_1:
            if (flash_magic_1(flash040_context, addr) && (byte == 0xaa)) {
                flash040_context->flash_state = FLASH040_STATE_ERASE_MAGIC_2;
            } else {
                flash040_context->flash_state = FLASH040_STATE_READ;
            }
            break;

        case FLASH040_STATE_ERASE_MAGIC_2:
            if (flash_magic_2(flash040_context, addr) && (byte == 0x55)) {
                flash040_context->flash_state = FLASH040_STATE_ERASE_SELECT;
            } else {
                flash040_context->flash_state = FLASH040_STATE_READ;
            }
            break;

        case FLASH040_STATE_ERASE_SELECT:
            if (flash_magic_1(flash040_context, addr) && (byte == 0x10)) {
                /* TODO setup alarm to not have the operation complete instantly */
                flash_erase_chip(flash040_context);
                flash040_context->flash_state = FLASH040_STATE_CHIP_ERASE;
            } else if (byte == 0x30) {
                /* TODO setup alarm to not have the operation complete instantly */
                flash_erase_sector(flash040_context, addr);
                flash040_context->flash_state = FLASH040_STATE_SECTOR_ERASE;
            } else {
                flash040_context->flash_state = FLASH040_STATE_READ;
            }
            break;

        case FLASH040_STATE_SECTOR_ERASE:
            if (byte == 0xb0) {
                flash040_context->flash_state = FLASH040_STATE_SECTOR_ERASE_SUSPEND;
            }
            break;

        case FLASH040_STATE_SECTOR_ERASE_SUSPEND:
            if (byte == 0x30) {
                flash040_context->flash_state = FLASH040_STATE_SECTOR_ERASE;
            }
            break;

        case FLASH040_STATE_BYTE_PROGRAM_ERROR:
        case FLASH040_STATE_AUTOSELECT:
            if (byte == 0xf0) {
                flash040_context->flash_state = FLASH040_STATE_READ;
            }
            break;

        case FLASH040_STATE_BYTE_PROGRAM_HAPPENING:
        case FLASH040_STATE_CHIP_ERASE:
        default:
            break;
    }

    FLASH_DEBUG(("Write %02x to %05x, state %i->%i", byte, addr, (int)old_state, (int)flash040_context->flash_state));
}

/* -------------------------------------------------------------------------- */

void REGPARM3 flash040core_store(flash040_context_t *flash040_context, unsigned int addr, BYTE byte)
{
    if (maincpu_rmw_flag) {
        maincpu_clk--;
        flash040core_store_internal(flash040_context, addr, flash040_context->last_read);
        maincpu_clk++;
    }

    flash040core_store_internal(flash040_context, addr, byte);
}


BYTE REGPARM2 flash040core_read(flash040_context_t *flash040_context, unsigned int addr)
{
    BYTE value;
#ifdef FLASH_DEBUG_ENABLED
    flash040_state_t old_state = flash040_context->flash_state;
#endif

    switch (flash040_context->flash_state) {
        case FLASH040_STATE_AUTOSELECT:
            switch (addr & 0xff) {
                case 0x00:
                    value = flash_types[flash040_context->flash_type].manufacturer_ID;
                    break;
                case 0x01:
                    value = flash_types[flash040_context->flash_type].device_ID;
                    break;
                case 0x02:
                    value = 0;
                    break;
                default:
                    value = flash040_context->flash_data[addr];
                    break;
            }
            break;

        case FLASH040_STATE_BYTE_PROGRAM_ERROR:
            value = flash_write_operation_status(flash040_context);
            break;

        case FLASH040_STATE_SECTOR_ERASE_SUSPEND:
        case FLASH040_STATE_BYTE_PROGRAM_HAPPENING:
        case FLASH040_STATE_CHIP_ERASE:
        case FLASH040_STATE_SECTOR_ERASE:
            /* TODO return write operation status. Until alarms are introduced, just reset the state and fall through */
            flash040_context->flash_state = FLASH040_STATE_READ;

        default:
            /* The state doesn't reset if a read occurs during a command sequence */
            /* fall through */
        case FLASH040_STATE_READ:
            value = flash040_context->flash_data[addr];
            break;
    }

#ifdef FLASH_DEBUG_ENABLED
    if (old_state != FLASH040_STATE_READ) {
        FLASH_DEBUG(("Read %02x from %05x, state %i->%i", value, addr, (int)old_state, (int)flash040_context->flash_state));
    }
#endif

    flash040_context->last_read = value;
    return value;
}

BYTE REGPARM2 flash040core_peek(flash040_context_t *flash040_context, unsigned int addr)
{
    return flash040_context->flash_data[addr];
}

void flash040core_reset(flash040_context_t *flash040_context)
{
    FLASH_DEBUG(("Reset"));
    flash040_context->flash_state = FLASH040_STATE_READ;
}

void flash040core_init(struct flash040_context_s *flash040_context,
                       flash040_type_t type, BYTE *data)
{
    FLASH_DEBUG(("Init"));
    flash040_context->flash_data = data;
    flash040_context->flash_type = type;
    flash040_context->flash_state = FLASH040_STATE_READ;
    flash040_context->flash_dirty = 0;
}

void flash040core_shutdown(flash040_context_t *flash040_context)
{
    FLASH_DEBUG(("Shutdown"));
}

/* -------------------------------------------------------------------------- */

#define FLASH040_DUMP_VER_MAJOR   0
#define FLASH040_DUMP_VER_MINOR   0

int flash040core_snapshot_write_module(snapshot_t *s, flash040_context_t *flash040_context, const char *name)
{
    snapshot_module_t *m;
    BYTE state;

    m = snapshot_module_create(s, name, FLASH040_DUMP_VER_MAJOR, FLASH040_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    state = (BYTE)(flash040_context->flash_state);

    if (0
        || (SMW_B(m, state) < 0)
        || (SMW_B(m, flash040_context->program_byte) < 0)
        || (SMW_B(m, flash040_context->last_read) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int flash040core_snapshot_read_module(snapshot_t *s, flash040_context_t *flash040_context, const char *name)
{
    BYTE vmajor, vminor, state;
    snapshot_module_t *m;

    m = snapshot_module_open(s, name, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if (vmajor != FLASH040_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    if (0
        || (SMR_B(m, &state) < 0)
        || (SMR_B(m, &(flash040_context->program_byte)) < 0)
        || (SMR_B(m, &(flash040_context->last_read)) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    flash040_context->flash_state = (flash040_state_t)state;
    return 0;
}
