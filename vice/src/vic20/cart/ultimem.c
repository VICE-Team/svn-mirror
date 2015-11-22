/*
 * ultimem.c -- UltiMem emulation.
 *
 * Written by
 *  Marko Makela <marko.makela@iki.fi>
 * Based on vic-fp.c by
 *  Marko Makela <marko.makela@iki.fi>
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
 * Based on megacart.c and finalexpansion.c by
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
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "flash040.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "resources.h"
#include "snapshot.h"
#include "translate.h"
#include "types.h"
#include "util.h"
#include "ultimem.h"
#include "vic20cart.h"
#include "vic20cartmem.h"
#include "vic20mem.h"
#include "zfile.h"

/* ------------------------------------------------------------------------- */
/*
 * Cartridge RAM (1 MiB)
 *
 * Mapping: FIXME
 *      RAM                 VIC20
 *   0x00000 - 0x01fff  ->  0xa000 - 0xbfff
 *   0x02400 - 0x02fff  ->  0x0400 - 0x0fff
 *   0x02000 - 0x07fff  ->  0x2000 - 0x7fff
 */
#define CART_RAM_SIZE (1 << 20)
static BYTE *cart_ram = NULL;

/*
 * Cartridge ROM (16 MiB)
 */
#define CART_ROM_SIZE (16 << 20)
static BYTE *cart_rom = NULL;

#define ultimem_reg0_regs_disable 0x80
#define ultimem_reg0_led 1

#define CART_CFG_DISABLE (ultimem[0] & ultimem_reg0_regs_disable)

/** Configuration registers */
static BYTE ultimem[16];
/** Used bits in ultimem[] */
static const BYTE ultimem_mask[16] = {
    ultimem_reg0_regs_disable | ultimem_reg0_led,
    0x3f, /* 00:IO3 config:IO2 config:RAM123 config */
    0xff, /* BLK5:BLK3:BLK2:BLK1 */
    0,
    0xff, 0x7, /* RAM bank lo/hi (A13..A23) */
    0xff, 0x7, /* I/O bank lo/hi (A13..A23) */
    0xff, 0x7, /* BLK1 bank lo/hi (A13..A23) */
    0xff, 0x7, /* BLK2 bank lo/hi (A13..A23) */
    0xff, 0x7, /* BLK3 bank lo/hi (A13..A23) */
    0xff, 0x7  /* BLK5 bank lo/hi (A13..A23) */
};

/** Block states */
enum blk_state_t {
    BLK_STATE_DISABLED,
    BLK_STATE_ROM,
    BLK_STATE_RAM_RO,
    BLK_STATE_RAM_RW
};

/** Get the configuration bits for RAM123, IO2 and IO3 */
#define CART_CFG_IO(x) (enum blk_state_t) (ultimem[1] >> (2 * (x - 1)) & 3)
#define CART_CFG_RAM123 CART_CFG_IO(1)

/** Get the configuration bits for BLK1,BLK2,BLK3,BLK5
@param x the BLK number (4=BLK5) */
#define CART_CFG_BLK(x) (enum blk_state_t) (ultimem[2] >> (2 * (x - 1)) & 3)

/** Get the 8KiB bank address for a block */
#define CART_ADDR(r) ((unsigned) ultimem[r + 1] << 21 | ultimem[r] << 13)
#define CART_RAM123_ADDR CART_ADDR(4)
#define CART_IO_ADDR CART_ADDR(6)
#define CART_BLK_ADDR(blk) CART_ADDR(6 + 2 * blk)

/* Cartridge States */
/** Flash state */
static flash040_context_t flash_state;

/* ------------------------------------------------------------------------- */

static int vic_um_writeback;
static char *cartfile = NULL;   /* perhaps the one in vic20cart.c could
                                   be used instead? */

static log_t um_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

/* Some prototypes are needed */
static BYTE vic_um_io_read(WORD addr);
static BYTE vic_um_io_peek(WORD addr);
static void vic_um_io_store(WORD addr, BYTE value);
static int vic_um_mon_dump(void);

static io_source_t vbi_device = {
    CARTRIDGE_VIC20_NAME_UM,
    IO_DETACH_CART,
    NULL,
    0x9800, 0x9bff, 0x3ff,
    0,
    vic_um_io_store,
    vic_um_io_read,
    vic_um_io_peek,
    vic_um_mon_dump,
    CARTRIDGE_VIC20_UM,
    0,
    0
};

static io_source_list_t *vbi_list_item = NULL;


/* ------------------------------------------------------------------------- */

/* read 0x0400-0x0fff */
BYTE vic_um_ram123_read(WORD addr)
{
    switch (CART_CFG_RAM123) {
    case BLK_STATE_DISABLED:
        return vic20_v_bus_last_data;
    case BLK_STATE_ROM:
        return flash040core_read(&flash_state,
                                 ((addr & 0x1fff) + CART_RAM123_ADDR) &
                                 (CART_ROM_SIZE - 1));
    case BLK_STATE_RAM_RO:
    case BLK_STATE_RAM_RW:
        return cart_ram[((addr & 0x1fff) + CART_RAM123_ADDR) &
                        (CART_RAM_SIZE - 1)];
    }
    return 0;
}

/* store 0x0400-0x0fff */
void vic_um_ram123_store(WORD addr, BYTE value)
{
    switch (CART_CFG_RAM123) {
    case BLK_STATE_DISABLED:
    case BLK_STATE_RAM_RO:
        break;
    case BLK_STATE_ROM:
        flash040core_store(&flash_state,
                           ((addr & 0x1fff) + CART_RAM123_ADDR) &
                           (CART_ROM_SIZE - 1),
                           value);
        break;
    case BLK_STATE_RAM_RW:
        cart_ram[((addr & 0x1fff) + CART_RAM123_ADDR) & (CART_RAM_SIZE - 1)] =
            value;
    }
}

/* read 0x2000-0x3fff */
BYTE vic_um_blk1_read(WORD addr)
{
    switch (CART_CFG_BLK(1)) {
    case BLK_STATE_DISABLED:
        return vic20_v_bus_last_data;
    case BLK_STATE_ROM:
        return flash040core_read(&flash_state,
                                 ((addr & 0x1fff) + CART_BLK_ADDR(1)) &
                                 (CART_ROM_SIZE - 1));
    case BLK_STATE_RAM_RO:
    case BLK_STATE_RAM_RW:
        return cart_ram[((addr & 0x1fff) + CART_BLK_ADDR(1)) &
                        (CART_RAM_SIZE - 1)];
    }
    return 0;
}

/* store 0x2000-0x3fff */
void vic_um_blk1_store(WORD addr, BYTE value)
{
    switch (CART_CFG_BLK(1)) {
    case BLK_STATE_DISABLED:
    case BLK_STATE_RAM_RO:
        break;
    case BLK_STATE_ROM:
        flash040core_store(&flash_state,
                           ((addr & 0x1fff) + CART_BLK_ADDR(1)) &
                           (CART_ROM_SIZE - 1),
                           value);
        break;
    case BLK_STATE_RAM_RW:
        cart_ram[((addr & 0x1fff) + CART_BLK_ADDR(1)) & (CART_RAM_SIZE - 1)] =
            value;
    }
}

/* read 0x4000-0x7fff */
BYTE vic_um_blk23_read(WORD addr)
{
    unsigned b = addr & 0x2000 ? 3 : 2;
    switch (CART_CFG_BLK(b)) {
    case BLK_STATE_DISABLED:
        return vic20_v_bus_last_data;
    case BLK_STATE_ROM:
        return flash040core_read(&flash_state,
                                 ((addr & 0x1fff) + CART_BLK_ADDR(b)) &
                                 (CART_ROM_SIZE - 1));
    case BLK_STATE_RAM_RO:
    case BLK_STATE_RAM_RW:
        return cart_ram[((addr & 0x1fff) + CART_BLK_ADDR(b)) &
                        (CART_RAM_SIZE - 1)];
    }
    return 0;
}

/* store 0x4000-0x7fff */
void vic_um_blk23_store(WORD addr, BYTE value)
{
    unsigned b = addr & 0x2000 ? 3 : 2;
    switch (CART_CFG_BLK(b)) {
    case BLK_STATE_DISABLED:
    case BLK_STATE_RAM_RO:
        break;
    case BLK_STATE_ROM:
        flash040core_store(&flash_state,
                           ((addr & 0x1fff) + CART_BLK_ADDR(b)) &
                           (CART_ROM_SIZE - 1),
                           value);
        break;
    case BLK_STATE_RAM_RW:
        cart_ram[((addr & 0x1fff) + CART_BLK_ADDR(b)) & (CART_RAM_SIZE - 1)] =
            value;
    }
}

/* read 0xa000-0xbfff */
BYTE vic_um_blk5_read(WORD addr)
{
    switch (CART_CFG_BLK(4)) {
    case BLK_STATE_DISABLED:
        return vic20_v_bus_last_data;
    case BLK_STATE_ROM:
        return flash040core_read(&flash_state,
                                 ((addr & 0x1fff) + CART_BLK_ADDR(4)) &
                                 (CART_ROM_SIZE - 1));
    case BLK_STATE_RAM_RO:
    case BLK_STATE_RAM_RW:
        return cart_ram[((addr & 0x1fff) + CART_BLK_ADDR(4)) &
                        (CART_RAM_SIZE - 1)];
    }
    return 0;
}

/* store 0xa000-0xbfff */
void vic_um_blk5_store(WORD addr, BYTE value)
{
    switch (CART_CFG_BLK(4)) {
    case BLK_STATE_DISABLED:
    case BLK_STATE_RAM_RO:
        break;
    case BLK_STATE_ROM:
        flash040core_store(&flash_state,
                           ((addr & 0x1fff) + CART_BLK_ADDR(4)) &
                           (CART_ROM_SIZE - 1),
                           value);
        break;
    case BLK_STATE_RAM_RW:
        cart_ram[((addr & 0x1fff) + CART_BLK_ADDR(4)) & (CART_RAM_SIZE - 1)] =
            value;
    }
}

/* read 0x9800-0x9fff */
static BYTE vic_um_io_read(WORD addr)
{
    unsigned io;

    vbi_device.io_source_valid = 0;

    if (CART_CFG_DISABLE) {
        return vic20_cpu_last_data;
    } else if ((addr & 0x7f0) == 0x3f0) {
        vbi_device.io_source_valid = 1;
        return ultimem[addr & 0xf];
    }

    io = addr & 0x200 ? 3 : 2;

    switch (CART_CFG_IO(io)) {
    case BLK_STATE_DISABLED:
        break;
    case BLK_STATE_ROM:
        vbi_device.io_source_valid = 1;
        return flash040core_read(&flash_state,
                                 ((addr & 0x1fff) + CART_IO_ADDR) &
                                 (CART_ROM_SIZE - 1));
    case BLK_STATE_RAM_RO:
    case BLK_STATE_RAM_RW:
        vbi_device.io_source_valid = 1;
        return cart_ram[((addr & 0x1fff) + CART_IO_ADDR) &
                        (CART_RAM_SIZE - 1)];
    }

    return vic20_v_bus_last_data;
}

static BYTE vic_um_io_peek(WORD addr)
{
    return ultimem[addr & 0xf];
}

/* store 0x9800-0x9fff */
static void vic_um_io_store(WORD addr, BYTE value)
{
    unsigned io;
    if (CART_CFG_DISABLE) {
        /* ignore */
        return;
    } else if ((addr & 0x7f0) == 0x3f0) {
        addr &= 0xf;
        ultimem[addr] = ultimem_mask[addr] & value;
        return;
    }

    io = addr & 0x200 ? 3 : 2;

    switch (CART_CFG_IO(io)) {
    case BLK_STATE_DISABLED:
    case BLK_STATE_RAM_RO:
        break;
    case BLK_STATE_ROM:
        flash040core_store(&flash_state,
                           ((addr & 0x1fff) + CART_IO_ADDR) &
                           (CART_ROM_SIZE - 1),
                           value);
        break;
    case BLK_STATE_RAM_RW:
        cart_ram[((addr & 0x1fff) + CART_IO_ADDR) & (CART_RAM_SIZE - 1)] =
            value;
    }
}

/* ------------------------------------------------------------------------- */

void vic_um_init(void)
{
    if (um_log == LOG_ERR) {
        um_log = log_open(CARTRIDGE_VIC20_NAME_UM);
    }
}

void vic_um_reset(void)
{
    flash040core_reset(&flash_state);
    memset(ultimem, 0, sizeof ultimem);
    ultimem[2] = 0x40;
}

void vic_um_config_setup(BYTE *rawcart)
{
}

static int zfile_load(const char *filename, BYTE *dest, size_t size)
{
    FILE *fd;

    fd = zfile_fopen(filename, MODE_READ);
    if (!fd) {
        return -1;
    }
    if (util_file_length(fd) != size) {
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

int vic_um_bin_attach(const char *filename)
{
    if (!cart_ram) {
        cart_ram = lib_malloc(CART_RAM_SIZE);
    }
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }

    util_string_set(&cartfile, filename);
    if (zfile_load(filename, cart_rom, (size_t)CART_ROM_SIZE) < 0) {
        vic_um_detach();
        return -1;
    }

    flash040core_init(&flash_state, maincpu_alarm_context, FLASH040_TYPE_010, cart_rom);

    mem_cart_blocks = VIC_CART_RAM123 |
                      VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
                      VIC_CART_IO2 | VIC_CART_IO3;
    mem_initialize_memory();

    vbi_list_item = io_source_register(&vbi_device);

    return 0;
}

void vic_um_detach(void)
{
    /* try to write back cartridge contents if write back is enabled
       and cartridge wasn't from a snapshot */
    if (vic_um_writeback && !cartridge_is_from_snapshot) {
        if (flash_state.flash_dirty) {
            int n;
            FILE *fd;

            n = 0;
            log_message(um_log, "Flash dirty, trying to write back...");
            fd = fopen(cartfile, "wb");
            if (fd) {
                n = fwrite(flash_state.flash_data, (size_t)CART_ROM_SIZE, 1, fd);
                fclose(fd);
            }
            if (n < 1) {
                log_message(um_log, "Failed to write back image `%s'!",
                            cartfile);
            } else {
                log_message(um_log, "Wrote back image `%s'.",
                            cartfile);
            }
        } else {
            log_message(um_log, "Flash clean, skipping write back.");
        }
    }

    mem_cart_blocks = 0;
    mem_initialize_memory();
    lib_free(cart_ram);
    lib_free(cart_rom);
    lib_free(cartfile);
    cart_ram = NULL;
    cart_rom = NULL;
    cartfile = NULL;

    if (vbi_list_item != NULL) {
        io_source_unregister(vbi_list_item);
        vbi_list_item = NULL;
    }
}

/* ------------------------------------------------------------------------- */

static int set_vic_um_writeback(int val, void *param)
{
    vic_um_writeback = val ? 1 : 0;

    return 0;
}

static const resource_int_t resources_int[] = {
    { "UltiMemWriteBack", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &vic_um_writeback, set_vic_um_writeback, NULL },
    { NULL }
};

int vic_um_resources_init(void)
{
    return resources_register_int(resources_int);
}

void vic_um_resources_shutdown(void)
{
}

static const cmdline_option_t cmdline_options[] =
{
    { "-umwriteback", SET_RESOURCE, 0,
      NULL, NULL, "UltiMemWriteBack", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_VIC_UM_ROM_WRITE,
      NULL, NULL },
    { "+umwriteback", SET_RESOURCE, 0,
      NULL, NULL, "UltiMemWriteBack", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_VIC_UM_ROM_WRITE,
      NULL, NULL },
    { NULL }
};

int vic_um_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

#define VIC20CART_DUMP_VER_MAJOR   2
#define VIC20CART_DUMP_VER_MINOR   0
#define SNAP_MODULE_NAME  "ULTIMEM"
#define FLASH_SNAP_MODULE_NAME  "FLASH040"

int vic_um_snapshot_write_module(snapshot_t *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, SNAP_MODULE_NAME,
                               VIC20CART_DUMP_VER_MAJOR,
                               VIC20CART_DUMP_VER_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || (SMW_BA(m, ultimem, sizeof ultimem) < 0)
        || (SMW_BA(m, cart_ram, CART_RAM_SIZE) < 0)
        || (SMW_BA(m, cart_rom, CART_ROM_SIZE) < 0)) {
        snapshot_module_close(m);
        return -1;
    }

    snapshot_module_close(m);

    if ((flash040core_snapshot_write_module(s, &flash_state, FLASH_SNAP_MODULE_NAME) < 0)) {
        return -1;
    }

    return 0;
}

int vic_um_snapshot_read_module(snapshot_t *s)
{
    BYTE vmajor, vminor;
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
    if (!cart_rom) {
        cart_rom = lib_malloc(CART_ROM_SIZE);
    }

    flash040core_init(&flash_state, maincpu_alarm_context, FLASH040_TYPE_010, cart_rom);

    if (0
        || (SMR_BA(m, ultimem, sizeof ultimem) < 0)
        || (SMR_BA(m, cart_ram, CART_RAM_SIZE) < 0)
        || (SMR_BA(m, cart_rom, CART_ROM_SIZE) < 0)) {
        snapshot_module_close(m);
        lib_free(cart_ram);
        lib_free(cart_rom);
        cart_ram = NULL;
        cart_rom = NULL;
        return -1;
    }

    snapshot_module_close(m);

    if ((flash040core_snapshot_read_module(s, &flash_state, FLASH_SNAP_MODULE_NAME) < 0)) {
        flash040core_shutdown(&flash_state);
        lib_free(cart_ram);
        lib_free(cart_rom);
        cart_ram = NULL;
        cart_rom = NULL;
        return -1;
    }

    mem_cart_blocks = VIC_CART_RAM123 |
                      VIC_CART_BLK1 | VIC_CART_BLK2 | VIC_CART_BLK3 | VIC_CART_BLK5 |
                      VIC_CART_IO2 | VIC_CART_IO3;
    mem_initialize_memory();

    return 0;
}

/* ------------------------------------------------------------------------- */

static int vic_um_mon_dump(void)
{
    mon_out("I/O2 %sabled\n", CART_CFG_DISABLE ? "dis" : "en");
    return 0;
}
