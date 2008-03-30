/*
 * c64-snapshot.c -- C64 snapshot handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "c64-resources.h"
#include "c64-snapshot.h"
#include "c64cart.h"
#include "c64mem.h"
#include "c64pla.h"
#include "c64rom.h"
#include "log.h"
#include "mem.h"
#include "resources.h"
#include "reu.h"
#include "snapshot.h"
#include "types.h"
#include "ui.h"

#ifdef HAVE_RS232
#include "c64acia.h"
#endif

static log_t c64_snapshot_log = LOG_ERR;

#define SNAP_ROM_MAJOR 0
#define SNAP_ROM_MINOR 0
static const char snap_rom_module_name[] = "C64ROM";

static int c64_snapshot_write_rom_module(snapshot_t *s)
{
    snapshot_module_t *m;
    int trapfl;

    /* Main memory module.  */

    m = snapshot_module_create(s, snap_rom_module_name,
                               SNAP_ROM_MAJOR, SNAP_ROM_MINOR);
    if (m == NULL)
        return -1;

    /* disable traps before saving the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*)&trapfl);
    resources_set_value("VirtualDevices", (resource_value_t)1);

    if (SMW_BA(m, mem_kernal_rom, C64_KERNAL_ROM_SIZE) < 0
        || SMW_BA(m, mem_basic_rom, C64_BASIC_ROM_SIZE) < 0
        || SMW_BA(m, mem_chargen_rom, C64_CHARGEN_ROM_SIZE) < 0)
        goto fail;

    /* FIXME: save cartridge ROM (& RAM?) areas:
       first write out the configuration, i.e.
       - type of cartridge (banking scheme type)
       - state of cartridge (active/which bank, ...)
       then the ROM/RAM arrays:
       - cartridge ROM areas
       - cartridge RAM areas  */

    ui_update_menus();

    if (snapshot_module_close(m) < 0)
        goto fail;

    resources_set_value("VirtualDevices", (resource_value_t)trapfl);

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);

    resources_set_value("VirtualDevices", (resource_value_t)trapfl);

    return -1;
}

static int c64_snapshot_read_rom_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;
    int trapfl;

    /* Main memory module.  */

    m = snapshot_module_open(s, snap_rom_module_name,
                             &major_version, &minor_version);
    if (m == NULL) {
        /* this module is optional */
        /* FIXME: reset all cartridge stuff to standard C64 behaviour */
        return 0;
    }

    if (major_version > SNAP_ROM_MAJOR || minor_version > SNAP_ROM_MINOR) {
        log_error(c64_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_ROM_MAJOR, SNAP_ROM_MINOR);
        snapshot_module_close(m);
        return -1;
    }

    /* disable traps before loading the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*)&trapfl);
    resources_set_value("VirtualDevices", (resource_value_t)1);

    if (SMR_BA(m, mem_kernal_rom, C64_KERNAL_ROM_SIZE) < 0
        || SMR_BA(m, mem_basic_rom, C64_BASIC_ROM_SIZE) < 0
        || SMR_BA(m, mem_chargen_rom, C64_CHARGEN_ROM_SIZE) < 0)
        goto fail;

    /* FIXME: read cartridge ROM (& RAM?) areas:
       first read out the configuration, i.e.
       - type of cartridge (banking scheme type)
       - state of cartridge (active/which bank, ...)
       then the ROM/RAM arrays:
       - cartridge ROM areas
       - cartridge RAM areas
    */

    if (snapshot_module_close(m) < 0)
        goto fail;

    c64rom_get_kernal_checksum();
    c64rom_get_basic_checksum();
    /* enable traps again when necessary */
    resources_set_value("VirtualDevices", (resource_value_t)trapfl);


    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    resources_set_value("VirtualDevices", (resource_value_t)trapfl);
    return -1;
}


#define SNAP_MAJOR 0
#define SNAP_MINOR 0
static const char snap_mem_module_name[] = "C64MEM";

int c64_snapshot_write_module(snapshot_t *s, int save_roms)
{
    snapshot_module_t *m;

    /* Main memory module.  */

    m = snapshot_module_create(s, snap_mem_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL)
        return -1;

    if (SMW_B(m, pport.data) < 0
        || SMW_B(m, pport.dir) < 0
        || SMW_B(m, export.exrom) < 0
        || SMW_B(m, export.game) < 0
        || SMW_BA(m, mem_ram, C64_RAM_SIZE) < 0)
        goto fail;

    if (snapshot_module_close(m) < 0)
        goto fail;
    m = NULL;

    if (save_roms && c64_snapshot_write_rom_module(s) < 0)
        goto fail;

    /* REU module.  */
    if (reu_enabled && reu_write_snapshot_module(s) < 0)
        goto fail;

#ifdef HAVE_RS232
    /* ACIA module.  */
    if (acia_de_enabled && acia1_snapshot_write_module(s) < 0)
        goto fail;
#endif

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

int c64_snapshot_read_module(snapshot_t *s)
{
    BYTE major_version, minor_version;
    snapshot_module_t *m;

    /* Main memory module.  */

    m = snapshot_module_open(s, snap_mem_module_name,
                             &major_version, &minor_version);
    if (m == NULL)
        return -1;

    if (major_version > SNAP_MAJOR || minor_version > SNAP_MINOR) {
        log_error(c64_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    if (SMR_B(m, &pport.data) < 0
        || SMR_B(m, &pport.dir) < 0
        || SMR_B(m, &export.exrom) < 0
        || SMR_B(m, &export.game) < 0
        || SMR_BA(m, mem_ram, C64_RAM_SIZE) < 0)
        goto fail;

    mem_pla_config_changed();

    if (snapshot_module_close(m) < 0)
        goto fail;
    m = NULL;

    if (c64_snapshot_read_rom_module(s) < 0)
        goto fail;

    /* REU module.  */
    if (reu_read_snapshot_module(s) < 0) {
        reu_enabled = 0;
    } else {
        reu_enabled = 1;
    }

#ifdef HAVE_RS232
    /* ACIA module.  */
    if (acia1_snapshot_read_module(s) < 0) {
        acia_de_enabled = 0;
    } else {
        /* FIXME: Why do we need to do so???  */
        acia1_reset();          /* Clear interrupts.  */
        acia_de_enabled = 1;
    }
#endif

    ui_update_menus();

    return 0;

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

