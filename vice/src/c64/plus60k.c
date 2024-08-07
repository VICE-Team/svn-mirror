/*
 * plus60k.c - PLUS60K EXPANSION HACK emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

/* Introduction :
 *
 * +60K is a kind of memory expansion for C64 proposed in late '90s by Polish
 * sceners, for sceners. Basically the whole idea was to add another bank of
 * memory and provide a shared area to exchange data between the banks.
 *
 *
 * Hardware :
 *
 * +60K circuit is somewhat complicated because quite a few new ICs have to mounted
 * inside a C64 but it is not very hard to build. I will not get into details and
 * schematics because it was described quite well in disk magazines.
 *
 *
 * Software :
 *
 * - VIC address space is divided into 4 parts: $d000-$d0ff, $d100-$d1ff, $d200-$d2ff
 *   and $d300-$d3ff
 * - only $d000-$d0ff is still visible in I/O space as VIC
 * - $d100-$d1ff returns $ff on read
 * - $d200-$d3ff is unconnected and returns random values
 * - register latch for +60K is active in all $d100-$d1ff space, but programs should
 *   use $d100 only
 * - only data bit 7 is connected to the latch, but programs should use 0 as bits 0-6
 * - VIC fetches data only from bank 0 RAM (onboard)

 * +60K is controlled by a write-only register at $d100. There are only two possible
 * values that can be written there:

 * value     | $1000-$ffff RAM area
 * ---------------------------------------------
 * %0xxxxxxx | comes from onboard RAM (bank 0)
 * %1xxxxxxx | comes from additional RAM (bank 1)

 * x - reserved bit, it seems that all existing +60K-enabled programs use 0 here

 * RAM/ROM/IO is still controlled as usual by $0001. The only thing that changes is
 * where $1000-$ffff RAM comes from. The $0000-$0fff is the shared space and always
 * comes from onboard RAM.
 * It is important to say that VIC cannot see additional RAM. It still fetches data
 * from onboard RAM thus it is possible to keep gfx data in bank 0 and code with
 * sound data in bank 1.
 * The $d100 control register returns $ff on read. Although such usage is forbidden
 * I've seen at least one example of switching to bank 0 by "INC $d100" instruction
 * so it is emulated too.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c64cart.h"
#include "c64mem.h"
#include "cartio.h"
#include "cartridge.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "monitor.h"
#include "resources.h"
#include "plus60k.h"
#include "snapshot.h"
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii.h"
#include "vicii-mem.h"

/* PLUS60K registers */
static uint8_t plus60k_reg = 0;

static log_t plus60k_log = LOG_DEFAULT;

static int plus60k_activate(void);
static int plus60k_deactivate(void);

int plus60k_enabled = 0;

int plus60k_base = 0xd100;

/* Filename of the +60K image.  */
static char *plus60k_filename = NULL;

static uint8_t *plus60k_ram;

static int plus60k_dump(void)
{
    mon_out("$1000-$FFFF bank: %d\n", plus60k_reg);
    return 0;
}

static uint8_t plus60k_ff_read(uint16_t addr)
{
    return 0xff;
}

static uint8_t plus60k_peek(uint16_t addr)
{
    return plus60k_reg << 7;
}

static void plus60k_vicii_store(uint16_t addr, uint8_t value)
{
    plus60k_reg = (value & 0x80) >> 7;
}

/* When the +60K device is active and the device base is at $d040, this device is used instead of the default VICII device */
static io_source_t vicii_d000_device = {
    "VIC-II",              /* name of the device */
    IO_DETACH_NEVER,       /* chip is never involved in collisions, so no detach */
    IO_DETACH_NO_RESOURCE, /* does not use a resource for detach */
    0xd000, 0xd03f, 0x3f,  /* range for the device, regs:$d000-$d03f */
    1,                     /* read is always valid */
    vicii_store,           /* store function */
    NULL,                  /* NO poke function */
    vicii_read,            /* read function */
    vicii_peek,            /* peek function */
    vicii_dump,            /* device state information dump function */
    0,                     /* dummy (not a cartridge) */
    IO_PRIO_HIGH,          /* high priority, device is never involved in collisions */
    0,                     /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE         /* NO mirroring */
};

/* When the +60K device is active and the device base is at $d100, this device is used instead of the default VICII device */
static io_source_t vicii_d000_full_device = {
    "VIC-II",              /* name of the device */
    IO_DETACH_NEVER,       /* chip is never involved in collisions, so no detach */
    IO_DETACH_NO_RESOURCE, /* does not use a resource for detach */
    0xd000, 0xd0ff, 0x3f,  /* range for the device, regs:$d000-$d03f, mirrors:$d040-$d0ff */
    1,                     /* read is always valid */
    vicii_store,           /* store function */
    NULL,                  /* NO poke function */
    vicii_read,            /* read function */
    vicii_peek,            /* peek function */
    vicii_dump,            /* device state information dump function */
    0,                     /* dummy (not a cartridge) */
    IO_PRIO_HIGH,          /* high priority, device and mirrors are never involved in collisions */
    0,                     /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE         /* NO mirroring */
};

static io_source_t vicii_d040_device = {
    "+60K",               /* name of the device */
    IO_DETACH_RESOURCE,   /* use resource to detach the device when involved in a read-collision */
    "PLUS60K",            /* resource to set to '0' */
    0xd040, 0xd0ff, 0x00, /* range for the device, address is ignored, reg:$d040, mirrors:$d041-$d0ff */
    1,                    /* read is always valid */
    plus60k_vicii_store,  /* store function */
    NULL,                 /* NO poke function */
    plus60k_ff_read,      /* read function */
    plus60k_peek,         /* peek function */
    plus60k_dump,         /* device state information dump function */
    CARTRIDGE_PLUS60K,    /* cartridge ID */
    IO_PRIO_NORMAL,       /* normal priority, device read needs to be checked for collisions */
    0,                    /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE        /* NO mirroring */
};

static io_source_t vicii_d100_device = {
    "+60K",               /* name of the device */
    IO_DETACH_RESOURCE,   /* use resource to detach the device when involved in a read-collision */
    "PLUS60K",            /* resource to set to '0' */
    0xd100, 0xd1ff, 0x00, /* range for the device, address is ignored, reg:$d100, mirrors:$d101-$d1ff */
    1,                    /* read is always valid */
    plus60k_vicii_store,  /* store function */
    NULL,                 /* NO poke function */
    plus60k_ff_read,      /* read function */
    plus60k_peek,         /* peek function */
    plus60k_dump,         /* device state information dump function */
    CARTRIDGE_PLUS60K,    /* cartridge ID */
    IO_PRIO_NORMAL,       /* normal priority, device read needs to be checked for collisions */
    0,                    /* insertion order, gets filled in by the registration function */
    IO_MIRROR_NONE        /* NO mirroring */
};

static io_source_list_t *vicii_d000_list_item = NULL;
static io_source_list_t *vicii_d000_full_list_item = NULL;
static io_source_list_t *vicii_d040_list_item = NULL;
static io_source_list_t *vicii_d100_list_item = NULL;

int set_plus60k_enabled(int value, int disable_reset)
{
    int val = value ? 1 : 0;

    if (val == plus60k_enabled) {
        return 0;
    }

    if (!val) {
        if (plus60k_deactivate() < 0) {
            return -1;
        }

        if (!disable_reset) {
            machine_trigger_reset(MACHINE_RESET_MODE_POWER_CYCLE);
        }
        plus60k_enabled = 0;
        return 0;
    } else {
        if (plus60k_activate() < 0) {
            return -1;
        }
        plus60k_enabled = 1;
        if (!disable_reset) {
            machine_trigger_reset(MACHINE_RESET_MODE_POWER_CYCLE);
        }
        return 0;
    }
}

static int set_plus60k_filename(const char *name, void *param)
{
    if (plus60k_filename != NULL && name != NULL && strcmp(name, plus60k_filename) == 0) {
        return 0;
    }

    if (name != NULL && *name != '\0') {
        if (util_check_filename_access(name) < 0) {
            return -1;
        }
    }

    if (plus60k_enabled) {
        plus60k_deactivate();
        util_string_set(&plus60k_filename, name);
        plus60k_activate();
    } else {
        util_string_set(&plus60k_filename, name);
    }

    return 0;
}

static int set_plus60k_base(int val, void *param)
{
    if (val == plus60k_base) {
        return 0;
    }

    switch (val) {
        case 0xd040:
        case 0xd100:
            break;
        default:
            log_message(plus60k_log, "Unknown PLUS60K base address $%X.",
                    (unsigned int)val);
            return -1;
    }

    if (plus60k_enabled) {
        plus60k_deactivate();
        plus60k_base = val;
        plus60k_activate();
    } else {
        plus60k_base = val;
    }

    return 0;
}

static const resource_string_t resources_string[] = {
    { "PLUS60Kfilename", "", RES_EVENT_NO, NULL,
      &plus60k_filename, set_plus60k_filename, NULL },
    RESOURCE_STRING_LIST_END
};

static const resource_int_t resources_int[] = {
    { "PLUS60Kbase", 0xd100, RES_EVENT_NO, NULL,
      &plus60k_base, set_plus60k_base, NULL },
    RESOURCE_INT_LIST_END
};

int plus60k_resources_init(void)
{
    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return resources_register_int(resources_int);
}

void plus60k_resources_shutdown(void)
{
    lib_free(plus60k_filename);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
    { "-plus60kimage", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "PLUS60Kfilename", NULL,
      "<Name>", "Specify name of PLUS60K image" },
    { "-plus60kbase", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "PLUS60Kbase", NULL,
      "<Base address>", "Base address of the PLUS60K expansion. (0xD040/0xD100)" },
    CMDLINE_LIST_END
};

int plus60k_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void plus60k_init(void)
{
    plus60k_log = log_open("PLUS60");
}

void plus60k_reset(void)
{
    plus60k_reg = 0;
}

static int plus60k_activate(void)
{
    plus60k_ram = lib_realloc((void *)plus60k_ram, (size_t)0xf000);

    log_message(plus60k_log, "PLUS60K expansion installed.");

    if (!util_check_null_string(plus60k_filename)) {
        if (util_file_load(plus60k_filename, plus60k_ram, (size_t)0xf000, UTIL_FILE_LOAD_RAW) < 0) {
            log_message(plus60k_log, "Reading PLUS60K image %s failed.", plus60k_filename);
            if (util_file_save(plus60k_filename, plus60k_ram, 0xf000) < 0) {
                log_message(plus60k_log, "Creating PLUS60K image %s failed.", plus60k_filename);
                return -1;
            }
            log_message(plus60k_log, "Creating PLUS60K image %s.", plus60k_filename);
        } else {
            log_message(plus60k_log, "Reading PLUS60K image %s.", plus60k_filename);
        }
    }

    plus60k_reset();

    c64io_vicii_deinit();
    if (plus60k_base == 0xd100) {
        vicii_d000_full_list_item = io_source_register(&vicii_d000_full_device);
        vicii_d100_list_item = io_source_register(&vicii_d100_device);
    } else {
        vicii_d000_list_item = io_source_register(&vicii_d000_device);
        vicii_d040_list_item = io_source_register(&vicii_d040_device);
    }
    return 0;
}

static int plus60k_deactivate(void)
{
    if (!util_check_null_string(plus60k_filename)) {
        if (util_file_save(plus60k_filename, plus60k_ram, 0xf000) < 0) {
            log_message(plus60k_log, "Writing PLUS60K image %s failed.", plus60k_filename);
            return -1;
        }
        log_message(plus60k_log, "Writing PLUS60K image %s.", plus60k_filename);
    }
    lib_free(plus60k_ram);
    plus60k_ram = NULL;

    if (vicii_d000_list_item != NULL) {
        io_source_unregister(vicii_d000_list_item);
        vicii_d000_list_item = NULL;
    }

    if (vicii_d000_full_list_item != NULL) {
        io_source_unregister(vicii_d000_full_list_item);
        vicii_d000_full_list_item = NULL;
    }

    if (vicii_d040_list_item != NULL) {
        io_source_unregister(vicii_d040_list_item);
        vicii_d040_list_item = NULL;
    }

    if (vicii_d100_list_item != NULL) {
        io_source_unregister(vicii_d100_list_item);
        vicii_d100_list_item = NULL;
    }
    c64io_vicii_reinit();

    return 0;
}

void plus60k_shutdown(void)
{
    if (plus60k_enabled) {
        plus60k_deactivate();
    }
}

/* ------------------------------------------------------------------------- */

static void plus60k_memory_store(uint16_t addr, uint8_t value)
{
    plus60k_ram[addr - 0x1000] = value;
}

static void vicii_mem_vbank_store_wrapper(uint16_t addr, uint8_t value)
{
    vicii_mem_vbank_store(addr, value);
}

static void vicii_mem_vbank_39xx_store_wrapper(uint16_t addr, uint8_t value)
{
    vicii_mem_vbank_39xx_store(addr, value);
}

static void vicii_mem_vbank_3fxx_store_wrapper(uint16_t addr, uint8_t value)
{
    vicii_mem_vbank_3fxx_store(addr, value);
}

static void ram_hi_store_wrapper(uint16_t addr, uint8_t value)
{
    ram_hi_store(addr, value);
}

static store_func_ptr_t plus60k_mem_write_tab[] = {
    vicii_mem_vbank_store_wrapper,
    plus60k_memory_store,
    vicii_mem_vbank_39xx_store_wrapper,
    plus60k_memory_store,
    vicii_mem_vbank_3fxx_store_wrapper,
    plus60k_memory_store,
    ram_hi_store_wrapper,
    plus60k_memory_store
};

void plus60k_vicii_mem_vbank_store(uint16_t addr, uint8_t value)
{
    plus60k_mem_write_tab[plus60k_reg](addr, value);
}

void plus60k_vicii_mem_vbank_39xx_store(uint16_t addr, uint8_t value)
{
    plus60k_mem_write_tab[plus60k_reg + 2](addr, value);
}

void plus60k_vicii_mem_vbank_3fxx_store(uint16_t addr, uint8_t value)
{
    plus60k_mem_write_tab[plus60k_reg + 4](addr, value);
}

void plus60k_ram_hi_store(uint16_t addr, uint8_t value)
{
    plus60k_mem_write_tab[plus60k_reg + 6](addr, value);
}

uint8_t plus60k_ram_read(uint16_t addr)
{
    if (plus60k_enabled && addr >= 0x1000 && plus60k_reg == 1) {
        return plus60k_ram[addr - 0x1000];
    } else {
        return mem_ram[addr];
    }
}

void plus60k_ram_store(uint16_t addr, uint8_t value)
{
    if (plus60k_enabled && addr >= 0x1000 && plus60k_reg == 1) {
        plus60k_ram[addr - 0x1000] = value;
    } else {
        mem_ram[addr] = value;
    }
}

void plus60k_ram_inject(uint16_t addr, uint8_t value)
{
    plus60k_ram_store(addr, value);
}

/* ------------------------------------------------------------------------- */

/* PLUS60K snapshot module format:

   type  | name     | description
   --------------------------------------
   WORD  | base     | base address of register
   BYTE  | register | register
   ARRAY | RAM      | 61440 BYTES of RAM data

   Note: for some reason this snapshot module started at 0.1, so there never was a 0.0
 */

static char snap_module_name[] = "PLUS60K";
#define SNAP_MAJOR   0
#define SNAP_MINOR   1

int plus60k_snapshot_write(struct snapshot_s *s)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_W (m, (uint16_t)plus60k_base) < 0
        || SMW_B (m, plus60k_reg) < 0
        || SMW_BA(m, plus60k_ram, 0xf000) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    return snapshot_module_close(m);
}

int plus60k_snapshot_read(struct snapshot_s *s)
{
    snapshot_module_t *m;
    uint8_t vmajor, vminor;

    m = snapshot_module_open(s, snap_module_name, &vmajor, &vminor);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(vmajor, vminor, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (SMR_W_INT(m, &plus60k_base) < 0) {
        goto fail;
    }

    /* enable plus60k, without reset */
    set_plus60k_enabled(1, 1);

    if (0
        || SMR_B(m, &plus60k_reg) < 0
        || SMR_BA(m, plus60k_ram, 0xf000) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);

    /* disable plus60k, without reset */
    set_plus60k_enabled(0, 1);

    return -1;
}
