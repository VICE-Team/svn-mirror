/*
 * megacart.c -- VIC20 Mega-Cart emulation.
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

/* #define DEBUGCART */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "export.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "megacart.h"
#include "mem.h"
#include "monitor.h"
#include "ram.h"
#include "resources.h"
#include "snapshot.h"
#include "types.h"
#include "util.h"
#include "vic20cart.h"
#include "vic20cartmem.h"
#include "vic20mem.h"
#include "zfile.h"

#ifdef DEBUGCART
#define DBG(x) log_printf  x
#else
#define DBG(x)
#endif

/* ------------------------------------------------------------------------- */

/*
 * Cartridge RAM
 *
 * Mapping
 *      RAM                 VIC20
 *   0x0000 - 0x1fff  ->  0xa000 - 0xbfff
 *   0x2000 - 0x7fff  ->  0x2000 - 0x7fff
 *
 * (by reasoning around minimal decoding, may be different on actual HW)
 */
#define CART_RAM_SIZE 0x8000
static uint8_t *cart_ram = NULL;

/*
 * Cartridge NvRAM
 *
 * Mapping
 *      NvRAM                 VIC20
 *   0x0400 - 0x0fff  ->  0x0400 - 0x0fff
 *   0x1800 - 0x1fff  ->  0x9800 - 0x9fff
 *
 * (by reasoning around minimal decoding, may be different on actual HW)
 */
#define CART_NVRAM_SIZE 0x2000
static uint8_t *cart_nvram = NULL;

/*
 * Cartridge ROM
 *
 * Mapping
 *      ROM
 *   0x000000 - 0x0fffff  ->  Low ROM: banks 0x00-0x7f
 *   0x100000 - 0x1fffff  ->  High ROM: banks 0x00-0x7f
 *
 */
#define CART_ROM_SIZE 0x200000
static uint8_t *cart_rom = NULL;

/* Cartridge States */
static enum { BUTTON_RESET, SOFTWARE_RESET } reset_mode = BUTTON_RESET;
static int oe_flop = 0;
static int nvram_en_flop = 0;
static uint8_t bank_low_reg = 0;
static uint8_t bank_high_reg = 0;

/* Resource variables */
static char *nvram_filename = NULL;
static int nvram_writeback = 0;

static log_t megacart_log = LOG_DEFAULT;

/* ------------------------------------------------------------------------- */

/* helper pointers */
static uint8_t *cart_rom_low;
static uint8_t *cart_rom_high;

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static uint8_t megacart_io2_read(uint16_t addr);
static void megacart_io2_store(uint16_t addr, uint8_t value);
static uint8_t megacart_io3_read(uint16_t addr);
static uint8_t megacart_io3_peek(uint16_t addr);
static void megacart_io3_store(uint16_t addr, uint8_t value);
static int megacart_mon_dump(void);

static io_source_t megacart_io2_device = {
    CARTRIDGE_VIC20_NAME_MEGACART, /* name of the device */
    IO_DETACH_CART,                /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,         /* does not use a resource for detach */
    0x9800, 0x9bff, 0x3ff,         /* range for the device, regs:$9800-$9bff */
    0,                             /* read validity is determined by the device upon a read */
    megacart_io2_store,            /* store function */
    NULL,                          /* NO poke function */
    megacart_io2_read,             /* read function */
    NULL,                          /* TODO: peek function */
    megacart_mon_dump,             /* device state information dump function */
    CARTRIDGE_VIC20_MEGACART,      /* cartridge ID */
    IO_PRIO_NORMAL,                /* normal priority, device read needs to be checked for collisions */
    0,                             /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE                 /* NO mirroring */
};

static io_source_t megacart_io3_device = {
    CARTRIDGE_VIC20_NAME_MEGACART, /* name of the device */
    IO_DETACH_CART,                /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,         /* does not use a resource for detach */
    0x9c00, 0x9fff, 0x3ff,         /* range for the device, regs:$9c00-$9fff */
    0,                             /* read validity is determined by the device upon a read */
    megacart_io3_store,            /* store function */
    NULL,                          /* NO poke function */
    megacart_io3_read,             /* read function */
    megacart_io3_peek,             /* peek function */
    megacart_mon_dump,             /* device state information dump function */
    CARTRIDGE_VIC20_MEGACART,      /* cartridge ID */
    IO_PRIO_NORMAL,                /* normal priority, device read needs to be checked for collisions */
    0,                             /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE                 /* NO mirroring */
};

static io_source_list_t *megacart_io2_list_item = NULL;
static io_source_list_t *megacart_io3_list_item = NULL;

static const export_resource_t export_res = {
    CARTRIDGE_VIC20_NAME_MEGACART, 0, 0, &megacart_io2_device, &megacart_io3_device, CARTRIDGE_VIC20_MEGACART
};

/* ------------------------------------------------------------------------- */

/* read 0x0400-0x0fff (nvram 0x0400 - 0x0fff) */
uint8_t megacart_ram123_read(uint16_t addr)
{
    if (nvram_en_flop) {
        return cart_nvram[addr & 0x0fff];
    } else {
        return vic20_v_bus_last_data;
    }
}

/* store 0x0400-0x0fff (nvram 0x0400 - 0x0fff) */
void megacart_ram123_store(uint16_t addr, uint8_t value)
{
    if (nvram_en_flop) {
        cart_nvram[addr & 0x0fff] = value;
    }
}

/* read 0x2000-0x7fff */
uint8_t megacart_blk123_read(uint16_t addr)
{
    uint8_t bank_low;
    uint8_t bank_high;
    int ram_low_en;
    int ram_high_en;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;

    if (!ram_low_en) {
        return cart_rom_low[(addr & 0x1fff) | (bank_low * 0x2000)];
    } else {
        if (ram_high_en) {
            return cart_ram[addr];
        }
    }

    return vic20_cpu_last_data;
}

/* store 0x2000-0x7fff */
void megacart_blk123_store(uint16_t addr, uint8_t value)
{
    uint8_t bank_low;
    uint8_t bank_high;
    int ram_low_en;
    int ram_high_en;
    int ram_wp;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;
    ram_wp = (bank_high & 0x40) ? 0 : 1;

    if (!ram_wp && (ram_low_en && ram_high_en)) {
        cart_ram[addr] = value;
    }
}

/* read 0xa000-0xbfff */
uint8_t megacart_blk5_read(uint16_t addr)
{
    uint8_t bank_low;
    uint8_t bank_high;
    int ram_low_en;
    int ram_high_en;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;

    if (!ram_high_en) {
        return cart_rom_high[(addr & 0x1fff) | (bank_high * 0x2000)];
    } else {
        if (!ram_low_en) {
            return cart_rom_low[(addr & 0x1fff) | (bank_low * 0x2000)];
        }
    }

    return cart_ram[addr & 0x1fff];
}

/* store 0xa000-0xbfff */
void megacart_blk5_store(uint16_t addr, uint8_t value)
{
    uint8_t bank_low;
    uint8_t bank_high;
    int ram_low_en;
    int ram_high_en;
    int ram_wp;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;
    ram_wp = (bank_high & 0x40) ? 0 : 1;

    if (!ram_wp && (ram_low_en && ram_high_en)) {
        cart_ram[addr & 0x1fff] = value;
    }
}

/* read 0x9800-0x9bff (nvram 0x1800 - 0x1bff) */
static uint8_t megacart_io2_read(uint16_t addr)
{
    uint8_t value;

    if (nvram_en_flop) {
        megacart_io2_device.io_source_valid = 1;
        value = cart_nvram[0x1800 + (addr & 0x3ff)];
    } else {
        megacart_io2_device.io_source_valid = 0;
        value = vic20_cpu_last_data;
    }
    return value;
}

/* store 0x9800-0x9bff (nvram 0x1800 - 0x1bff) */
static void megacart_io2_store(uint16_t addr, uint8_t value)
{
    if (nvram_en_flop) {
        cart_nvram[0x1800 + (addr & 0x3ff)] = value;
    }
}

/* read 0x9c00-0x9fff (nvram 0x1c00 - 0x1fff) */
static uint8_t megacart_io3_read(uint16_t addr)
{
    uint8_t value;

    if (nvram_en_flop) {
        megacart_io3_device.io_source_valid = 1;
        value = cart_nvram[0x1c00 + (addr & 0x3ff)];
    } else {
        megacart_io3_device.io_source_valid = 0;
        value = vic20_cpu_last_data;
    }
    return value;
}

static uint8_t megacart_io3_peek(uint16_t addr)
{
    if ((addr & 0x3ff) == 0x080) { /* $9c80 */
        return bank_high_reg;
    }

    if ((addr & 0x3ff) == 0x100) { /* $9d00 */
        return bank_low_reg;
    }

    if (nvram_en_flop) {
        return cart_nvram[0x1c00 + (addr & 0x3ff)];
    }
    return vic20_cpu_last_data;
}

/* store 0x9c00-0x9fff (nvram 0x1c00 - 0x1fff) */
static void megacart_io3_store(uint16_t addr, uint8_t value)
{
    if (nvram_en_flop) {
        cart_nvram[0x1c00 + (addr & 0x3ff)] = value;
    }

    if ((addr & 0x180) == 0x080) { /* $9c80 */
        bank_high_reg = value;
    }

    if ((addr & 0x180) == 0x100) { /* $9d00 */
        bank_low_reg = value;
    }

    if ((addr & 0x180) == 0x180) { /* $9d80 */
        nvram_en_flop = (value & 0x1) ? 0 : 1;
        bank_high_reg = value;
        bank_low_reg = value;
    }

    if ((addr & 0x200) == 0x200) { /* $9e00 */
        /* perform reset */
        reset_mode = SOFTWARE_RESET;
        machine_trigger_reset(MACHINE_RESET_MODE_RESET_CPU);
    }
}

/* ------------------------------------------------------------------------- */

static int zfile_load(const char *filename, uint8_t *dest, size_t size)
{
    FILE *fd;
    off_t len;

    fd = zfile_fopen(filename, MODE_READ);
    if (!fd) {
        return -1;
    }
    len = archdep_file_size(fd);
    if (len < 0 || (size_t)len != size) {
        zfile_fclose(fd);
        return -1;
    }
    if (fread(dest, size, 1, fd) < 1) {
        zfile_fclose(fd);
        return -1;
    }
    zfile_fclose(fd);
    return 0;
}

static int try_nvram_load(const char *filename)
{
    DBG(("cart_nvram: %p filename: %s", cart_nvram, filename ? filename : "NULL"));
    if (cart_nvram && filename && *filename != '\0') {
        if (zfile_load(filename, cart_nvram, (size_t)CART_NVRAM_SIZE) < 0) {
            log_message(megacart_log, "Failed to read NvRAM image `%s'!", filename);
            return -1;
        } else {
            log_message(megacart_log, "Read NvRAM image `%s'.", filename);
            return 1;
        }
    }
    return 0;
}

static int try_nvram_save(const char *filename)
{
    int ret = 0;
    if (cart_nvram && filename && *filename != '\0') {
        FILE *fd;
        fd = fopen(filename, "wb");
        if (fd) {
            if (fwrite(cart_nvram, (size_t)CART_NVRAM_SIZE, 1, fd) > 0) {
                log_message(megacart_log, "Wrote back NvRAM image `%s'.", filename);
            } else {
                ret = -1;
            }
            fclose(fd);
        } else {
            ret = -1;
        }
        if (ret == -1) {
            log_message(megacart_log, "Failed to write back NvRAM image `%s'!", filename);
        }
    }

    return ret;
}


int megacart_save_nvram(const char *filename)
{
    return try_nvram_save(filename);
}

int megacart_flush_nvram(void)
{
    /* try to write back NvRAM contents if cartridge is not from a snapshot */
    if (!cartridge_is_from_snapshot) {
        return try_nvram_save(nvram_filename);
    }
    return -1;
}

int megacart_can_flush_nvram(void)
{
    return (nvram_filename != NULL) && !cartridge_is_from_snapshot;
}

/* ------------------------------------------------------------------------- */

/* FIXME: this still needs to be tweaked to match the hardware */
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

/* FIXME: this still needs to be tweaked to match the hardware */
static RAMINITPARAM nvramparam = {
    .start_value = 255,
    .value_invert = 2,
    .value_offset = 1,

    .pattern_invert = 0x100,
    .pattern_invert_value = 255,

    .random_start = 0,
    .random_repeat = 0,
    .random_chance = 0,
};

static void allocate_rom_ram(void)
{
    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    if (!cart_nvram) {
        cart_nvram = lib_malloc(CART_NVRAM_SIZE);
    }
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }
}

static void clear_ram(void)
{
    if (cart_ram) {
        DBG(("clear_ram: cart_ram"));
        ram_init_with_pattern(cart_ram, CART_RAM_SIZE, &ramparam);
    }
    if (cart_nvram) {
        DBG(("clear_ram: cart_nvram"));
        ram_init_with_pattern(cart_nvram, CART_NVRAM_SIZE, &nvramparam);
    }
}

void megacart_init(void)
{
    if (megacart_log == LOG_DEFAULT) {
        megacart_log = log_open(CARTRIDGE_VIC20_NAME_MEGACART);
    }
}

void megacart_reset(void)
{
    if (reset_mode == SOFTWARE_RESET) {
        oe_flop = !oe_flop;
    } else {
        oe_flop = 0;
    }
    reset_mode = BUTTON_RESET;
}

void megacart_powerup(void)
{
    reset_mode = BUTTON_RESET;
    oe_flop = 0;
    nvram_en_flop = 1;
}

void megacart_config_setup(uint8_t *rawcart)
{
}

int megacart_crt_attach(FILE *fd, uint8_t *rawcart)
{
    crt_chip_header_t chip;
    int idx = 0;

    allocate_rom_ram();
    clear_ram();

    for (idx = 0; idx < 256; idx++) {
        if (crt_read_chip_header(&chip, fd)) {
            goto exiterror;
        }

        DBG(("chip %d at %02x len %02x", idx, chip.start, chip.size));
        if (chip.size != 0x2000) {
            goto exiterror;
        }

        if (crt_read_chip(&cart_rom[0x2000 * idx], 0, &chip, fd)) {
            goto exiterror;
        }
    }

    if (export_add(&export_res) < 0) {
        goto exiterror;
    }

    try_nvram_load(nvram_filename);

    cart_rom_low = cart_rom;
    cart_rom_high = cart_rom + 0x100000;

    mem_cart_blocks = VIC_CART_RAM123 |
                      VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
                      VIC_CART_IO2 | VIC_CART_IO3;
    mem_initialize_memory();

    megacart_io2_list_item = io_source_register(&megacart_io2_device);
    megacart_io3_list_item = io_source_register(&megacart_io3_device);

    return CARTRIDGE_VIC20_MEGACART;

exiterror:
    megacart_detach();
    return -1;
}

int megacart_bin_attach(const char *filename)
{
    allocate_rom_ram();
    clear_ram();

    if (zfile_load(filename, cart_rom, (size_t)CART_ROM_SIZE) < 0) {
        megacart_detach();
        return -1;
    }

    if (export_add(&export_res) < 0) {
        return -1;
    }

    try_nvram_load(nvram_filename);

    cart_rom_low = cart_rom;
    cart_rom_high = cart_rom + 0x100000;

    mem_cart_blocks = VIC_CART_RAM123 |
                      VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
                      VIC_CART_IO2 | VIC_CART_IO3;
    mem_initialize_memory();

    megacart_io2_list_item = io_source_register(&megacart_io2_device);
    megacart_io3_list_item = io_source_register(&megacart_io3_device);

    return 0;
}

void megacart_detach(void)
{
    /* try to write back NvRAM contents if write back is enabled
       and cartridge is not from a snapshot */
    if (nvram_writeback && !cartridge_is_from_snapshot) {
        try_nvram_save(nvram_filename);
    }

    mem_cart_blocks = 0;
    mem_initialize_memory();
    lib_free(cart_ram);
    lib_free(cart_nvram);
    lib_free(cart_rom);
    cart_ram = NULL;
    cart_nvram = NULL;
    cart_rom = NULL;

    export_remove(&export_res);

    if (megacart_io2_list_item != NULL) {
        io_source_unregister(megacart_io2_list_item);
        megacart_io2_list_item = NULL;
    }

    if (megacart_io3_list_item != NULL) {
        io_source_unregister(megacart_io3_list_item);
        megacart_io3_list_item = NULL;
    }
}

/* ------------------------------------------------------------------------- */

static int set_nvram_filename(const char *name, void *param)
{
    if (nvram_filename && name && strcmp(name, nvram_filename) == 0) {
        return 0;
    }

    /* try to write back NvRAM contents to the old file if write back is enabled
       and NvRAM wasn't from a snapshot */
    if (nvram_writeback && !cartridge_is_from_snapshot) {
        try_nvram_save(nvram_filename);
    }

    util_string_set(&nvram_filename, name);

    try_nvram_load(nvram_filename);
    return 0;
}

static const resource_string_t resources_string[] = {
    { "MegaCartNvRAMfilename", "", RES_EVENT_NO, NULL,
      &nvram_filename, set_nvram_filename, NULL },
    RESOURCE_STRING_LIST_END
};

static int set_nvram_writeback(int val, void *param)
{
    nvram_writeback = val ? 1 : 0;

    return 0;
}

static const resource_int_t resources_int[] = {
    { "MegaCartNvRAMWriteBack", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &nvram_writeback, set_nvram_writeback, NULL },
    RESOURCE_INT_LIST_END
};

int megacart_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return resources_register_int(resources_int);
}

void megacart_resources_shutdown(void)
{
    lib_free(nvram_filename);
    nvram_filename = NULL;
}

static const cmdline_option_t cmdline_options[] =
{
    { "-mcnvramfile", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "MegaCartNvRAMfilename", NULL,
      "<Name>", "Set Mega-Cart NvRAM filename" },
    { "-mcnvramwriteback", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "MegaCartNvRAMWriteBack", (resource_value_t)1,
      NULL, "Enable Mega-Cart NvRAM writeback" },
    { "+mcnvramwriteback", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "MegaCartNvRAMWriteBack", (resource_value_t)0,
      NULL, "Disable Mega-Cart NvRAM writeback" },
    CMDLINE_LIST_END
};

int megacart_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

#define VIC20CART_DUMP_VER_MAJOR   2
#define VIC20CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "MEGACART"

int megacart_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME, VIC20CART_DUMP_VER_MAJOR, VIC20CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_B(m, bank_low_reg) < 0)
        || (SMW_B(m, bank_high_reg) < 0)
        || (SMW_B(m, (uint8_t)oe_flop) < 0)
        || (SMW_B(m, (uint8_t)nvram_en_flop) < 0)
        || (SMW_BA(m, cart_ram, CART_RAM_SIZE) < 0)
        || (SMW_BA(m, cart_rom, CART_ROM_SIZE) < 0)
        || (SMW_BA(m, cart_nvram, CART_NVRAM_SIZE) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);
    return 0;
}

int megacart_snapshot_read_module(snapshot_t *s)
{
    uint8_t vmajor, vminor;
    snapshot_module_t *m;

    m = snapshot_module_open(s, SNAP_MODULE_NAME, &vmajor, &vminor);
    if (m == NULL) {
        return -1;
    }

    if (vmajor != VIC20CART_DUMP_VER_MAJOR) {
        snapshot_module_close(m);
        return -1;
    }

    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    if (!cart_nvram) {
        cart_nvram = lib_malloc(CART_NVRAM_SIZE);
    }
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }

    if (0
        || (SMR_B(m, &bank_low_reg) < 0)
        || (SMR_B(m, &bank_high_reg) < 0)
        || (SMR_B_INT(m, &oe_flop) < 0)
        || (SMR_B_INT(m, &nvram_en_flop) < 0)
        || (SMR_BA(m, cart_ram, CART_RAM_SIZE) < 0)
        || (SMR_BA(m, cart_rom, CART_ROM_SIZE) < 0)
        || (SMR_BA(m, cart_nvram, CART_NVRAM_SIZE) < 0)) {
        snapshot_module_close(m);
        lib_free(cart_ram);
        lib_free(cart_nvram);
        lib_free(cart_rom);
        cart_ram = NULL;
        cart_nvram = NULL;
        cart_rom = NULL;
        return -1;
    }

    snapshot_module_close(m);

    cart_rom_low = cart_rom;
    cart_rom_high = cart_rom + 0x100000;

    reset_mode = BUTTON_RESET;

    mem_cart_blocks = VIC_CART_RAM123 |
                      VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
                      VIC_CART_IO2 | VIC_CART_IO3;
    mem_initialize_memory();

    return 0;
}

/* ------------------------------------------------------------------------- */

static int megacart_mon_dump(void)
{
    uint8_t bank_low;
    uint8_t bank_high;
    int ram_low_en;
    int ram_high_en;
    int ram_wp;

    /* get bank registers */
    bank_low = (oe_flop) ? bank_low_reg : 0x7f;
    bank_high = (oe_flop) ? bank_high_reg : 0x7f;

    /* determine flags from bank registers. */
    ram_low_en = (bank_low & 0x80) ? 1 : 0;
    ram_high_en = (bank_high & 0x80) ? 1 : 0;
    ram_wp = (bank_high & 0x40) ? 0 : 1;

    mon_out("Registers: Bank low $%02x, high $%02x\n", bank_low_reg, bank_high_reg);
    mon_out("NvRAM flop: %i, OE flop: %i\n", nvram_en_flop, oe_flop);
    mon_out("RAM123: %s\n", nvram_en_flop ? "NvRAM" : "off");

    mon_out("BLKn: ");
    if (!ram_low_en) {
        mon_out("ROM bank $%02x (offset $%06x)\n",
                bank_low, bank_low * 0x2000U);
    } else {
        if (ram_high_en) {
            mon_out("RAM %s\n", ram_wp ? "(write protected)" : "");
        } else {
            mon_out("off\n");
        }
    }

    mon_out("BLK5: ");
    if (!ram_high_en) {
        mon_out("ROM bank $%02x (offset $%06x)\n", bank_high, bank_high * 0x2000U + 0x100000U);
    } else {
        if (!ram_low_en) {
            mon_out("ROM bank $%02x (offset $%06x)\n", bank_low, bank_low * 0x2000U);
        } else {
            mon_out("RAM %s\n", ram_wp ? "(write protected)" : "");
        }
    }

    return 0;
}
