/*
 * magicdeskplus.c - Cartridge handling, Magic Desk Plus cart.
 *
 * Based on the VICE Magic Desk cartridge implementation written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * Original Magic Desk Plus concept and initial implementation by
 *  CrystalCT
 *
 * Reworked and adapted to the current VICE cartridge architecture by
 *  Claus Schlereth
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

/* #define MAGICDESKPLUS_DEBUG */

#include "vice.h"

#include <stdio.h>
#include <string.h>

#define CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64cartsystem.h"
#undef CARTRIDGE_INCLUDE_SLOTMAIN_API
#include "c64mem.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "export.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "magicdeskplus.h"
#include "monitor.h"
#include "ram.h"
#include "resources.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"

#ifdef MAGICDESKPLUS_DEBUG
#define DBG(x) log_printf x
#else
#define DBG(x)
#endif

/*
    "Magic Desk Plus" Cartridge

    https://github.com/crystalct/MagicDeskPlus

    256 KiB, 512 KiB, and 1024 KiB variants exist

    Supported Magic Desk Plus CRT revisions:
      Revision 0  - ROM + SRAM + 32K EEPROM
      Revision 1  - ROM + SRAM + 8k EEPROM
      Revision 2  - ROM + 32k EEPROM
      Revision 3  - ROM + 8k EEPROM
      Revision 4  - ROM + SRAM

    Memory map:
      $8000-$9FFF   ROM window (8K game)
      $DF00-$DFFF   256-byte SRAM/EEPROM data window

    IO1 registers:
      $DE00
        bits 0-6   ROM bank number
        bit 7      EXROM (1 = cartridge disabled)
      $DE01
        SRAM/EEPROM page register
      $DE03
        SRAM/EEPROM control register

    https://crystalct.itch.io/c64-murder-on-the-mississippi-remastered
    uses 128k SRAM and 8k EEPROM

*/

#define MAXBANKS 128
#define MAGICDESKPLUS_SRAM_SIZE       131072
#define MAGICDESKPLUS_EEPROM_8K_SIZE  8192
#define MAGICDESKPLUS_EEPROM_32K_SIZE 32768

static uint8_t regval = 0;
static uint8_t bankmask = 0x7f;

/* Magic Desk Plus SRAM and EEPROM variables */
static int magicdeskplus_enabled = 0;
static int magicdeskplus_revision = MAGICDESKPLUS_REV_SRAM_EEPROM_32K;

static uint8_t *magicdeskplus_sram_ram = NULL;
static char *magicdeskplus_sram_filename = NULL;
static int magicdeskplus_sram_write_image = 0;
static int magicdeskplus_sram_enabled = 0;

static char *magicdeskplus_eeprom_filename = NULL;
static int magicdeskplus_eeprom_write_image = 0;
static int magicdeskplus_eeprom_enabled = 0;

static uint8_t magicdeskplus_page_reg = 0;
static uint8_t magicdeskplus_ctrl_reg = 0;

static int set_magicdeskplus_revision(int val, void *param);

/* return SRAM size for current revision, 0 if none */
static int magicdeskplus_sram_size(void) {
    switch(magicdeskplus_revision) {
        case MAGICDESKPLUS_REV_SRAM_EEPROM_32K:
        case MAGICDESKPLUS_REV_SRAM_EEPROM_8K:
        case MAGICDESKPLUS_REV_SRAM:
            return MAGICDESKPLUS_SRAM_SIZE;
        default:
            break;
    }
    return 0;
}

/* return EEPROM size for current revision, 0 if none */
static int magicdeskplus_eeprom_size(void) {
    switch(magicdeskplus_revision) {
        case MAGICDESKPLUS_REV_SRAM_EEPROM_32K:
        case MAGICDESKPLUS_REV_EEPROM_32K:
            return MAGICDESKPLUS_EEPROM_32K_SIZE;
        case MAGICDESKPLUS_REV_SRAM_EEPROM_8K:
        case MAGICDESKPLUS_REV_EEPROM_8K:
            return MAGICDESKPLUS_EEPROM_8K_SIZE;
        default:
            break;
    }
    return 0;
}

/* RAMINITPARAM for SRAM pattern initialization */
static RAMINITPARAM ramparam = {
    .start_value = 255,
    .value_invert = 2,
    .value_offset = 1,

    .pattern_invert = 0x100,
    .pattern_invert_value = 255,

    .random_start = 0,
    .random_repeat = 0,
    .random_chance = 0,
};

/* Parallel EEPROM structure */
typedef struct magicdeskplus_eeprom_s {
    uint8_t *data;
    size_t size;
    char *filename;
    int dirty;
} magicdeskplus_eeprom_t;

static magicdeskplus_eeprom_t magicdeskplus_eeprom = {NULL, 0, NULL, 0};

/* (re)allocate eeprom buffer for given size */
static void magicdeskplus_eeprom_init(size_t size) {
    magicdeskplus_eeprom.size = size;
    if (magicdeskplus_eeprom.data) {
        lib_free(magicdeskplus_eeprom.data);
    }
    if (size) {
        magicdeskplus_eeprom.data = lib_malloc(size);
        memset(magicdeskplus_eeprom.data, 0xff, size);
    }
    magicdeskplus_eeprom.dirty = 0;
}

/* read eeprom file, size must match the expected size */
static int magicdeskplus_eeprom_open_image(const char *filename) {
    FILE *fd;
    size_t size;

    if ((filename == NULL) ||
        (*filename == '\0')) {
        return -1;
    }

    if (magicdeskplus_eeprom.filename != NULL) {
        lib_free(magicdeskplus_eeprom.filename);
    }
    magicdeskplus_eeprom.filename = lib_strdup(filename);

    magicdeskplus_eeprom_init(magicdeskplus_eeprom_size());

    fd = fopen(filename, "rb");
    if (fd == NULL) {
        return -1;
    }

    fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    if (size != magicdeskplus_eeprom_size()) {
        fclose(fd);
        log_message(LOG_DEFAULT,
                    "MAGICDESKPLUS: Invalid EEPROM image size (must be %d bytes).", magicdeskplus_eeprom_size());
        return -1;
    }

    if (fread(magicdeskplus_eeprom.data, 1, size, fd) < size) {
        fclose(fd);
        return -1;
    }
    fclose(fd);
    return 0;
}

/* close EEPROM image, deallocate associated buffers */
static void magicdeskplus_eeprom_close_image(void) {
    if (magicdeskplus_eeprom.data != NULL) {
        lib_free(magicdeskplus_eeprom.data);
        magicdeskplus_eeprom.data = NULL;
    }
    if (magicdeskplus_eeprom.filename != NULL) {
        lib_free(magicdeskplus_eeprom.filename);
        magicdeskplus_eeprom.filename = NULL;
    }
    magicdeskplus_eeprom.size = 0;
    magicdeskplus_eeprom.dirty = 0;
}

/* save EEPROM image to a file */
static int magicdeskplus_eeprom_save_image(const char *filename) {
    FILE *fd;

    if (magicdeskplus_eeprom.data == NULL ||
        filename == NULL ||
        *filename == '\0') {
        return -1;
    }

    fd = fopen(filename, "wb");
    if (fd == NULL) {
        return -1;
    }

    if (fwrite(magicdeskplus_eeprom.data, 1, magicdeskplus_eeprom.size, fd) != magicdeskplus_eeprom.size) {
        fclose(fd);
        return -1;
    }
    fclose(fd);
    magicdeskplus_eeprom.dirty = 0;
    return 0;
}

/* flush EEPROM image when it is dirty */
static int magicdeskplus_eeprom_flush_image(void) {
    log_message(LOG_DEFAULT, "MAGICDESKPLUS: Flushing EEPROM image. Dirty: %d\n",
                magicdeskplus_eeprom.dirty);
    if (magicdeskplus_eeprom.filename != NULL &&
        magicdeskplus_eeprom.dirty) {
        return magicdeskplus_eeprom_save_image(magicdeskplus_eeprom.filename);
    }
    return 0;
}

/* read from EEPROM */
static uint8_t magicdeskplus_eeprom_read(uint16_t addr) {
    if (magicdeskplus_eeprom.data != NULL) {
        uint8_t page_mask = (magicdeskplus_eeprom.size == MAGICDESKPLUS_EEPROM_8K_SIZE) ? 0x1f : 0x7f;
        uint32_t offset =
            (((uint32_t)(magicdeskplus_page_reg & page_mask)) << 8) | (addr & 0xff);
        if (offset < magicdeskplus_eeprom.size) {
            return magicdeskplus_eeprom.data[offset];
        }
    }
    return 0xff;
}

/* write to EEPROM */
static void magicdeskplus_eeprom_store(uint16_t addr, uint8_t value) {
    if (magicdeskplus_eeprom.data != NULL) {
        uint8_t page_mask = (magicdeskplus_eeprom.size == MAGICDESKPLUS_EEPROM_8K_SIZE) ? 0x1f : 0x7f;
        uint32_t offset =
            (((uint32_t)(magicdeskplus_page_reg & page_mask)) << 8) | (addr & 0xff);
        if (offset < magicdeskplus_eeprom.size) {
            magicdeskplus_eeprom.data[offset] = value;
            magicdeskplus_eeprom.dirty = 1;
        }
    }
}

/* SRAM activate/deactivate helpers */
static void magicdeskplus_sram_activate(void) {
    if (magicdeskplus_sram_size()) {
        magicdeskplus_sram_enabled = 1;
        if (magicdeskplus_sram_ram == NULL) {
            magicdeskplus_sram_ram = lib_malloc(MAGICDESKPLUS_SRAM_SIZE);
            ram_init_with_pattern(magicdeskplus_sram_ram, MAGICDESKPLUS_SRAM_SIZE, &ramparam);
            if (magicdeskplus_sram_filename != NULL &&
                *magicdeskplus_sram_filename != '\0' &&
                util_file_load(magicdeskplus_sram_filename, magicdeskplus_sram_ram,
                              MAGICDESKPLUS_SRAM_SIZE, UTIL_FILE_LOAD_RAW) < 0 &&
                util_file_exists(magicdeskplus_sram_filename)) {
                log_error(LOG_DEFAULT, "MAGICDESKPLUS: Cannot load SRAM image %s.",
                          magicdeskplus_sram_filename);
            }
        }
    } else {
        magicdeskplus_sram_enabled = 0;
        if (magicdeskplus_sram_ram != NULL) {
            lib_free(magicdeskplus_sram_ram);
            magicdeskplus_sram_ram = NULL;
        }
    }
}

static void magicdeskplus_sram_deactivate(void) {
    if (magicdeskplus_sram_ram != NULL) {
        if (magicdeskplus_sram_filename != NULL && *magicdeskplus_sram_filename != '\0') {
            if (magicdeskplus_sram_write_image) {
                util_file_save(magicdeskplus_sram_filename, magicdeskplus_sram_ram,
                              MAGICDESKPLUS_SRAM_SIZE);
            }
        }
        lib_free(magicdeskplus_sram_ram);
        magicdeskplus_sram_ram = NULL;
    }
    magicdeskplus_sram_enabled = 0;
}

/* EEPROM activate/deactivate helpers */
static void magicdeskplus_eeprom_activate(void) {
    if (magicdeskplus_eeprom_size()) {
        magicdeskplus_eeprom_enabled = 1;
        if (magicdeskplus_eeprom.data == NULL) {
            if (magicdeskplus_eeprom_filename == NULL ||
                *magicdeskplus_eeprom_filename == '\0') {
                magicdeskplus_eeprom_init(magicdeskplus_eeprom_size());
            } else if (magicdeskplus_eeprom_open_image(magicdeskplus_eeprom_filename) < 0) {
                magicdeskplus_eeprom_close_image();
                magicdeskplus_eeprom_init(magicdeskplus_eeprom_size());
            }
        }
    } else {
        magicdeskplus_eeprom_enabled = 0;
        magicdeskplus_eeprom_close_image();
    }
}

static void magicdeskplus_eeprom_deactivate(void) {
    if (magicdeskplus_eeprom_enabled) {
        if (magicdeskplus_eeprom_write_image) {
            magicdeskplus_eeprom_flush_image();
        }
        magicdeskplus_eeprom_close_image();
    }
    magicdeskplus_eeprom_enabled = 0;
}

static void magicdeskplus_io1_store(uint16_t addr, uint8_t value) {
    switch (addr & 0x03) {
        case 0:
            regval = value & (0x80 | bankmask);
            cart_romlbank_set_slotmain(value & bankmask);
            cart_set_port_game_slotmain(0);
            if (value & 0x80) {
                cart_set_port_exrom_slotmain(0);
            } else {
                cart_set_port_exrom_slotmain(1);
            }
            cart_port_config_changed_slotmain();
            break;
        case 1:
            magicdeskplus_page_reg = value;
            break;
        case 2:
            /* unused */
            break;
        case 3:
            magicdeskplus_ctrl_reg = value;
            break;
    }
    DBG(("MAGICDESKPLUS: Reg: %02x (Bank: %d of %d, %s)", regval,
        (regval & bankmask), bankmask + 1,
        (regval & 0x80) ? "disabled" : "enabled"));
}

static uint8_t magicdeskplus_io1_peek(uint16_t addr) {
    switch (addr & 0x03) {
        case 0:
            return regval;
        case 1:
            return magicdeskplus_page_reg;
        case 2:
            return 0;
        case 3:
            return magicdeskplus_ctrl_reg;
    }
    return regval;
}

static io_source_t magicdeskplus_io2_device;

static uint8_t magicdeskplus_io2_read(uint16_t addr) {
    int memory_select = (magicdeskplus_ctrl_reg >> 5) & 1;

    magicdeskplus_io2_device.io_source_valid = 0;

    if (memory_select == 1) {
        if (magicdeskplus_sram_size() &&
            magicdeskplus_sram_enabled &&
            magicdeskplus_sram_ram != NULL) {
            uint32_t bank = magicdeskplus_ctrl_reg & 1;
            uint32_t page = magicdeskplus_page_reg;
            uint32_t offset = addr & 0xff;
            magicdeskplus_io2_device.io_source_valid = 1;
            return magicdeskplus_sram_ram[(bank << 16) | (page << 8) | offset];
        }
    } else {
        if (magicdeskplus_eeprom_size() &&
            magicdeskplus_eeprom_enabled) {
            magicdeskplus_io2_device.io_source_valid = 1;
            return magicdeskplus_eeprom_read(addr);
        }
    }

  return 0;
}

static void magicdeskplus_io2_store(uint16_t addr, uint8_t value) {
    int memory_select = (magicdeskplus_ctrl_reg >> 5) & 1;

    if (memory_select == 1) {
        if (magicdeskplus_sram_size() &&
            magicdeskplus_sram_enabled &&
            magicdeskplus_sram_ram != NULL) {
            uint32_t bank = magicdeskplus_ctrl_reg & 1;
            uint32_t page = magicdeskplus_page_reg;
            uint32_t offset = addr & 0xff;
            magicdeskplus_sram_ram[(bank << 16) | (page << 8) | offset] = value;
        }
    } else {
        if (magicdeskplus_eeprom_size() &&
            magicdeskplus_eeprom_enabled) {
            magicdeskplus_eeprom_store(addr, value);
        }
    }
}

static int magicdeskplus_dump(void) {
    mon_out("Reg: %02x (Bank: %d of %d, %s)\n", regval, (regval & bankmask),
            bankmask + 1, (regval & 0x80) ? "disabled" : "enabled");
    if (magicdeskplus_sram_enabled ||
        magicdeskplus_eeprom_enabled) {
        mon_out("Page Reg: %02x, Ctrl Reg: %02x\n", magicdeskplus_page_reg,
                magicdeskplus_ctrl_reg);
        mon_out("SRAM Enabled: %d, EEPROM Enabled: %d Size: %d bytes\n",
                magicdeskplus_sram_enabled, magicdeskplus_eeprom_enabled, magicdeskplus_eeprom_size());
    }
    return 0;
}

static io_source_t magicdeskplus_device = {
    CARTRIDGE_NAME_MAGIC_DESK_PLUS,   /* name of the device */
    IO_DETACH_CART,                   /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,            /* does not use a resource for detach */
    0xde00, 0xdeff, 0xff,             /* range for the device, address is ignored, reg:$de00, mirrors:$de01-$deff */
    0,                                /* read is never valid, reg is write only */
    magicdeskplus_io1_store,          /* store function */
    NULL,                             /* NO poke function */
    NULL,                             /* read function */
    magicdeskplus_io1_peek,           /* peek function */
    magicdeskplus_dump,               /* device state information dump function */
    CARTRIDGE_MAGIC_DESK_PLUS,        /* cartridge ID */
    IO_PRIO_NORMAL,                   /* normal priority, device read needs to be checked for collisions */
    0,                                /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE                    /* NO mirroring */
};

static io_source_t magicdeskplus_io2_device = {
    CARTRIDGE_NAME_MAGIC_DESK_PLUS,   /* name of the device */
    IO_DETACH_CART,                   /* use cartridge ID to detach */
    IO_DETACH_NO_RESOURCE,
    0xdf00, 0xdfff, 0xff,             /* range: $df00-$dfff, mirror: 0xff */
    0,                                /* read validity checked dynamically */
    magicdeskplus_io2_store,          /* store function */
    NULL,
    magicdeskplus_io2_read,           /* read function */
    magicdeskplus_io2_read,           /* peek function */
    magicdeskplus_dump,
    CARTRIDGE_MAGIC_DESK_PLUS,
    IO_PRIO_NORMAL,
    0,
    IO_MIRROR_NONE
};

static io_source_list_t *magicdeskplus_list_item = NULL;
static io_source_list_t *magicdeskplus_io2_list_item = NULL;

static const export_resource_t export_res = {
    CARTRIDGE_NAME_MAGIC_DESK_PLUS, 0, 1, &magicdeskplus_device, &magicdeskplus_io2_device, CARTRIDGE_MAGIC_DESK_PLUS};

/* ---------------------------------------------------------------------*/

void magicdeskplus_config_init(void) {
    cart_config_changed_slotmain(CMODE_8KGAME, CMODE_8KGAME, CMODE_READ);
    magicdeskplus_io1_store((uint16_t)0xde00, 0);
    magicdeskplus_page_reg = 0;
    magicdeskplus_ctrl_reg = 0;
}

void magicdeskplus_config_setup(uint8_t *rawcart) {
    memcpy(roml_banks, rawcart, 0x2000 * MAXBANKS);
    cart_config_changed_slotmain(CMODE_8KGAME, CMODE_8KGAME, CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static int magicdeskplus_common_attach(void) {
    if (export_add(&export_res) < 0) {
        return -1;
    }
    magicdeskplus_list_item = io_source_register(&magicdeskplus_device);
    magicdeskplus_io2_list_item = io_source_register(&magicdeskplus_io2_device);
    magicdeskplus_enabled = 1;
    magicdeskplus_sram_activate();
    magicdeskplus_eeprom_activate();

    if (cart_getid_slotmain() == CARTRIDGE_NONE) {
        cart_set_port_exrom_slotmain(0);
        cart_set_port_game_slotmain(0);
        cart_port_config_changed_slotmain();
    }
    return 0;
}

int magicdeskplus_bin_attach(const char *filename, uint8_t *rawcart) {
    bankmask = 0x7f;
    if (util_file_load(filename, rawcart, 0x100000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        bankmask = 0x3f;
        if (util_file_load(filename, rawcart, 0x80000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
            bankmask = 0x1f;
            if (util_file_load(filename, rawcart, 0x40000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
                return -1;
            }
        }
    }
    return magicdeskplus_common_attach();
}

int magicdeskplus_crt_attach(FILE *fd, uint8_t *rawcart, uint8_t revision) {
    crt_chip_header_t chip;
    int lastbank = 0;

    if (revision > MAGICDESKPLUS_MAX_REV) {
        log_error(LOG_DEFAULT, "Invalid Magic Desk Plus revision %u.", revision);
        return -1;
    }
    while (1) {
        if (crt_read_chip_header(&chip, fd)) {
            break;
        }
        if ((chip.bank >= MAXBANKS) ||
            (chip.start != 0x8000) ||
            (chip.size != 0x2000)) {
            return -1;
        }
        if (crt_read_chip(rawcart, chip.bank << 13, &chip, fd)) {
            return -1;
        }
        if (chip.bank > lastbank) {
            lastbank = chip.bank;
        }
    }
    if (lastbank >= 128) {
        /* more than 128 banks does not work */
        return -1;
    } else if (lastbank >= 64) {
        /* min 65, max 128 banks */
        bankmask = 0x7f;
    } else if (lastbank >= 32) {
        /* min 33, max 64 banks */
        bankmask = 0x3f;
    }
    if (magicdeskplus_common_attach() < 0) {
        return -1;
    }
    set_magicdeskplus_revision(revision, NULL);
    return 0;
}

void magicdeskplus_detach(void) {
    magicdeskplus_sram_deactivate();
    magicdeskplus_eeprom_deactivate();

    export_remove(&export_res);
    io_source_unregister(magicdeskplus_list_item);
    magicdeskplus_list_item = NULL;
    io_source_unregister(magicdeskplus_io2_list_item);
    magicdeskplus_io2_list_item = NULL;
    magicdeskplus_enabled = 0;
}

void magicdeskplus_shutdown(void) {
    if (magicdeskplus_enabled) {
        magicdeskplus_detach();
    }
}

/* ---------------------------------------------------------------------*/

/* CARTMDP snapshot module format:

   type  | name       | description
   --------------------------------
   BYTE  | regval         | bank register value
   BYTE  | bankmask       | bank mask
   ARRAY | roml_banks     | 128 * 8192 bytes of ROM data
   BYTE  | revision       | hardware revision
   BYTE  | page_reg       | page register value
   BYTE  | ctrl_reg       | control register value
   DWORD | sram_size      | SRAM size (0 means none)
   ARRAY | sram_ram       | SRAM data (128k bytes)
   DWORD | eeprom_size    | EEPROM size (0 means none)
   ARRAY | eeprom data    | EEPROM data (8k or 32k)
*/

#define CART_DUMP_VER_MAJOR 0
#define CART_DUMP_VER_MINOR 1
#define SNAP_MODULE_NAME "CARTMDP"

int magicdeskplus_snapshot_write_module(snapshot_t *s) {
    snapshot_module_t *m;
    int sram_size = magicdeskplus_sram_size();
    int eeprom_size = magicdeskplus_eeprom_size();

    m = snapshot_module_create(s, SNAP_MODULE_NAME, CART_DUMP_VER_MAJOR,
                             CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    /* common stuff */
    if (0
        || (SMW_B(m, (uint8_t)regval) < 0)
        || (SMW_B(m, (uint8_t)bankmask) < 0)
        || (SMW_BA(m, roml_banks, 0x2000 * MAXBANKS) < 0)
        || (SMW_B(m, (uint8_t)magicdeskplus_revision) < 0)
        || (SMW_B(m, magicdeskplus_page_reg) < 0)
        || (SMW_B(m, magicdeskplus_ctrl_reg) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    /* SRAM */
    if (SMW_DW(m, (uint32_t)sram_size) < 0) {
        snapshot_module_close(m);
        return -1;
    }
    if (sram_size) {
        if (SMW_BA(m, magicdeskplus_sram_ram, sram_size) < 0) {
            snapshot_module_close(m);
            return -1;
        }
    }

    /* EEPROM */
    if (SMW_DW(m, (uint32_t)eeprom_size) < 0) {
        snapshot_module_close(m);
        return -1;
    }
    if (eeprom_size) {
        if (SMW_BA(m, magicdeskplus_eeprom.data, eeprom_size) < 0) {
            snapshot_module_close(m);
            return -1;
        }
    }

    snapshot_module_close(m);
    return 0;
}

int magicdeskplus_snapshot_read_module(snapshot_t *s) {
    uint8_t vmajor, vminor;
    snapshot_module_t *m;
    uint8_t page_reg;
    uint8_t ctrl_reg;
    uint8_t revision;
    uint32_t sram_size;
    uint32_t eeprom_size;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if (vmajor != CART_DUMP_VER_MAJOR || vminor != CART_DUMP_VER_MINOR) {
        snapshot_module_close(m);
        return -1;
    }

    /* common stuff */
    if (0 ||
        (SMR_B(m, &regval) < 0) ||
        (SMR_B(m, &bankmask) < 0) ||
        (SMR_BA(m, roml_banks, 0x2000 * MAXBANKS) < 0) ||
        (SMR_B(m, &revision) < 0) ||
        (SMR_B(m, &page_reg) < 0) ||
        (SMR_B(m, &ctrl_reg) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    /* SRAM */
    if (SMR_DW(m, &sram_size) < 0) {
        snapshot_module_close(m);
        return -1;
    }
    if (sram_size) {
        if (magicdeskplus_sram_ram) {
            lib_free(magicdeskplus_sram_ram);
        }
        magicdeskplus_sram_ram = lib_malloc(sram_size);
        if (SMR_BA(m, magicdeskplus_sram_ram, sram_size) < 0) {
            snapshot_module_close(m);
            lib_free(magicdeskplus_sram_ram);
            magicdeskplus_sram_ram = NULL;
            return -1;
        }
        magicdeskplus_sram_enabled = 1;
    }

    /* EEPROM */
    if (SMR_DW(m, &eeprom_size) < 0) {
        snapshot_module_close(m);
        return -1;
    }
    if (eeprom_size) {
        if (magicdeskplus_eeprom.data) {
            lib_free(magicdeskplus_eeprom.data);
        }
        magicdeskplus_eeprom.data = lib_malloc(eeprom_size);
        if (SMR_BA(m, magicdeskplus_eeprom.data, eeprom_size) < 0) {
            snapshot_module_close(m);
            lib_free(magicdeskplus_eeprom.data);
            magicdeskplus_eeprom.data = NULL;
            return -1;
        }
        magicdeskplus_eeprom_enabled = 1;
    }

    snapshot_module_close(m);

    magicdeskplus_page_reg = page_reg;
    magicdeskplus_ctrl_reg = ctrl_reg;

    if (magicdeskplus_common_attach() == -1) {
        return -1;
    }
    magicdeskplus_io1_store(0xde00, regval);
    return 0;
}

/* Resources & command-line callbacks */

static int set_magicdeskplus_sram_filename(const char *name, void *param) {

    if (magicdeskplus_sram_filename != NULL && name != NULL &&
        strcmp(name, magicdeskplus_sram_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
    }

    if (magicdeskplus_enabled) {
        magicdeskplus_sram_deactivate();
        util_string_set(&magicdeskplus_sram_filename, name);
        magicdeskplus_sram_activate();
    } else {
        util_string_set(&magicdeskplus_sram_filename, name);
    }
    return 0;
}

static int set_magicdeskplus_sram_write(int val, void *param) {
    magicdeskplus_sram_write_image = val ? 1 : 0;
    return 0;
}

static int set_magicdeskplus_eeprom_filename(const char *name, void *param) {

    if (magicdeskplus_eeprom_filename != NULL && name != NULL &&
        strcmp(name, magicdeskplus_eeprom_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
    }

    if (magicdeskplus_enabled) {
        magicdeskplus_eeprom_deactivate();
        util_string_set(&magicdeskplus_eeprom_filename, name);
        magicdeskplus_eeprom_activate();
    } else {
        util_string_set(&magicdeskplus_eeprom_filename, name);
    }
    return 0;
}

static int set_magicdeskplus_eeprom_write(int val, void *param) {
    magicdeskplus_eeprom_write_image = val ? 1 : 0;
    return 0;
}

static int set_magicdeskplus_revision(int val, void *param) {
    if ((val < 0) ||
        (val > MAGICDESKPLUS_MAX_REV)) {
        return -1;
    }

    magicdeskplus_revision = val;
    return 0;
}

/* --------------------------------------------------------------------------
 * Public SRAM save/flush API (called from c64carthooks.c)
 * -------------------------------------------------------------------------- */

/** \brief  Check if SRAM secondary image can be saved */
int magicdeskplus_can_save_sram(void) {
    return magicdeskplus_enabled &&
           magicdeskplus_sram_size() &&
           magicdeskplus_sram_ram != NULL;
}

/** \brief  Check if SRAM secondary image can be flushed */
int magicdeskplus_can_flush_sram(void) {
    if (!magicdeskplus_can_save_sram()) {
        return 0;
    }
    if (magicdeskplus_sram_filename == NULL ||
        *magicdeskplus_sram_filename == '\0') {
        return 0;
    }
    return 1;
}

/** \brief  Save SRAM contents to \a filename */
int magicdeskplus_sram_save(const char *filename) {
    if (!magicdeskplus_sram_size() ||
        magicdeskplus_sram_ram == NULL) {
        return -1;
    }
    if (filename == NULL) {
        return -1;
    }
    if (util_file_save(filename, magicdeskplus_sram_ram, MAGICDESKPLUS_SRAM_SIZE) < 0) {
        log_message(LOG_DEFAULT, "MAGICDESKPLUS: Writing SRAM image %s failed.",
                    filename);
        return -1;
    }
    log_message(LOG_DEFAULT, "MAGICDESKPLUS: Writing SRAM image %s.", filename);
    return 0;
}

/** \brief  Flush SRAM contents to the currently configured filename */
int magicdeskplus_sram_flush(void) {
    return magicdeskplus_sram_save(magicdeskplus_sram_filename);
}

/** \brief  Check if EEPROM tertiary image can be saved */
int magicdeskplus_can_save_eeprom(void) {
    return magicdeskplus_enabled &&
          magicdeskplus_eeprom_size() &&
          magicdeskplus_eeprom.data != NULL;
}

/** \brief  Check if EEPROM tertiary image can be flushed */
int magicdeskplus_can_flush_eeprom(void) {
    if (!magicdeskplus_can_save_eeprom()) {
        return 0;
    }
    if (magicdeskplus_eeprom_filename == NULL ||
        *magicdeskplus_eeprom_filename == '\0') {
        return 0;
    }
    return 1;
}

/** \brief  Save EEPROM contents to \a filename */
int magicdeskplus_eeprom_save(const char *filename) {
    int dirty;

    if (!magicdeskplus_eeprom_size() ||
        magicdeskplus_eeprom.data == NULL) {
        return -1;
    }
    if (filename == NULL) {
        return -1;
    }
    dirty = magicdeskplus_eeprom.dirty;
    if (magicdeskplus_eeprom_save_image(filename) < 0) {
        log_message(LOG_DEFAULT, "MAGICDESKPLUS: Writing EEPROM image %s failed.",
                  filename);
        return -1;
    }
    if (magicdeskplus_eeprom_filename == NULL ||
        strcmp(filename, magicdeskplus_eeprom_filename) != 0) {
        magicdeskplus_eeprom.dirty = dirty;
    }
    log_message(LOG_DEFAULT, "MAGICDESKPLUS: Writing EEPROM image %s.", filename);
    return 0;
}

/** \brief  Flush EEPROM contents to the currently configured filename */
int magicdeskplus_eeprom_flush(void) {
    return magicdeskplus_eeprom_save(magicdeskplus_eeprom_filename);
}

static const resource_string_t resources_string[] = {
    {"MagicDeskPlusSRAMImage", "", RES_EVENT_NO, NULL, &magicdeskplus_sram_filename,
     set_magicdeskplus_sram_filename, NULL},
    {"MagicDeskPlusEEPROMImage", "", RES_EVENT_NO, NULL, &magicdeskplus_eeprom_filename,
     set_magicdeskplus_eeprom_filename, NULL},
    RESOURCE_STRING_LIST_END
};

static const resource_int_t resources_int[] = {
    {"MagicDeskPlusRevision", MAGICDESKPLUS_REV_SRAM_EEPROM_32K, RES_EVENT_NO,
     NULL, &magicdeskplus_revision, set_magicdeskplus_revision, NULL},
    {"MagicDeskPlusSRAMWrite", 0, RES_EVENT_NO, NULL, &magicdeskplus_sram_write_image,
     set_magicdeskplus_sram_write, NULL},
    {"MagicDeskPlusEEPROMWrite", 0, RES_EVENT_NO, NULL,
     &magicdeskplus_eeprom_write_image, set_magicdeskplus_eeprom_write, NULL},
    RESOURCE_INT_LIST_END
};

int magicdeskplus_resources_init(void) {
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }
    return resources_register_int(resources_int);
}

void magicdeskplus_resources_shutdown(void) {
    if (magicdeskplus_sram_filename) {
      lib_free(magicdeskplus_sram_filename);
    }
    if (magicdeskplus_eeprom_filename) {
      lib_free(magicdeskplus_eeprom_filename);
    }
    magicdeskplus_sram_filename = NULL;
    magicdeskplus_eeprom_filename = NULL;
}

static const cmdline_option_t cmdline_options[] = {
    {"-magicdeskplussramimage", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS, NULL, NULL,
     "MagicDeskPlusSRAMImage", NULL, "<filename>",
     "Specify Magic Desk Plus SRAM image filename"},
    {"-magicdeskplussramwrite", SET_RESOURCE, CMDLINE_ATTRIB_NONE, NULL, NULL,
     "MagicDeskPlusSRAMWrite", (resource_value_t)1, NULL,
     "Enable saving of the Magic Desk Plus SRAM image at exit"},
    {"+magicdeskplussramwrite", SET_RESOURCE, CMDLINE_ATTRIB_NONE, NULL, NULL,
     "MagicDeskPlusSRAMWrite", (resource_value_t)0, NULL,
     "Disable saving of the Magic Desk Plus SRAM image at exit"},
    {"-magicdeskpluseepromimage", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS, NULL,
     NULL, "MagicDeskPlusEEPROMImage", NULL, "<filename>",
     "Specify Magic Desk Plus EEPROM image filename"},
    {"-magicdeskpluseepromwrite", SET_RESOURCE, CMDLINE_ATTRIB_NONE, NULL, NULL,
     "MagicDeskPlusEEPROMWrite", (resource_value_t)1, NULL,
     "Enable saving of the Magic Desk Plus EEPROM image at exit"},
    {"+magicdeskpluseepromwrite", SET_RESOURCE, CMDLINE_ATTRIB_NONE, NULL, NULL,
     "MagicDeskPlusEEPROMWrite", (resource_value_t)0, NULL,
     "Disable saving of the Magic Desk Plus EEPROM image at exit"},
    { "-magicdeskplusrevision", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MagicDeskPlusRevision", NULL,
      "<Revision>", "Set Magic Desk Plus revision (0: SRAM and EEPROM, 1: SRAM only, 2: EEPROM only)" },
    CMDLINE_LIST_END
};

int magicdeskplus_cmdline_options_init(void) {
    return cmdline_register_options(cmdline_options);
}
