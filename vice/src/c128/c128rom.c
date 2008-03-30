/*
 * c128rom.c
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "c128mem.h"
#include "c128rom.h"
#include "mem.h"
#include "log.h"
#include "resources.h"
#include "sysfile.h"
#include "types.h"


#define IS_NULL(s)  (s == NULL || *s == '\0')


static log_t c128rom_log = LOG_ERR;

/* Flag: nonzero if the Kernal and BASIC ROMs have been loaded.  */
static int rom_loaded = 0;


int c128rom_kernal_checksum(void)
{
    int i, id;
    WORD sum;

    /* Check Kernal ROM.  */
    for (i = 0, sum = 0; i < C128_KERNAL_ROM_SIZE; i++)
        sum += mem_kernal_rom[i];

    id = rom_read(0xff80);

    log_message(c128rom_log, "Kernal rev #%d.", id);
    if (id == 1
        && sum != C128_KERNAL_CHECKSUM_R01
        && sum != C128_KERNAL_CHECKSUM_R01SWE
        && sum != C128_KERNAL_CHECKSUM_R01GER)
        log_error(c128rom_log, "Warning: Kernal image may be corrupted."
                  " Sum: %d.", sum);
    return 0;
}

int c128rom_load_kernal(const char *rom_name)
{
    int trapfl;

    if (!rom_loaded)
        return 0;

    /* disable traps before loading the ROM */
    resources_get_value("VirtualDevices", (resource_value_t*) &trapfl);
    resources_set_value("VirtualDevices", (resource_value_t) 1);

    if (!IS_NULL(rom_name)) {
        /* Load Kernal ROM.  */
        if (sysfile_load(rom_name,
            mem_kernal_rom, C128_KERNAL_ROM_SIZE,
            C128_KERNAL_ROM_SIZE) < 0) {
            log_error(c128rom_log, "Couldn't load kernal ROM `%s'.",
                      rom_name);
            resources_set_value("VirtualDevices", (resource_value_t) trapfl);
            return -1;
        }
    }

    c128rom_kernal_checksum();

    resources_set_value("VirtualDevices", (resource_value_t) trapfl);

    return 0;
}

int c128rom_basic_checksum(void)
{
    int i, id;
    WORD sum;

    /* Check Basic ROM.  */
    for (i = 0, sum = 0; i < C128_BASIC_ROM_SIZE; i++)
        sum += mem_basic_rom[i];

    if (sum != C128_BASIC_CHECKSUM_85 && sum != C128_BASIC_CHECKSUM_86)
        log_error(c128rom_log,
                  "Warning: Unknown Basic image.  Sum: %d ($%04X).",
                  sum, sum);

    /* Check Editor ROM.  */
    for (i = C128_BASIC_ROM_SIZE, sum = 0;
         i < C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE;
         i++)
        sum += mem_basic_rom[i];

    id = rom_read(0xff80);
    if (id == 01
        && sum != C128_EDITOR_CHECKSUM_R01
        && sum != C128_EDITOR_CHECKSUM_R01SWE
        && sum != C128_EDITOR_CHECKSUM_R01GER) {
        log_error(c128rom_log, "Warning: EDITOR image may be corrupted."
                  " Sum: %d.", sum);
        log_error(c128rom_log, "Check your Basic ROM.");
    }
    return 0;
}

int c128rom_load_basic(const char *rom_name)
{
    if (!rom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load Basic ROM.  */
        if (sysfile_load(rom_name,
            mem_basic_rom, C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE,
            C128_BASIC_ROM_SIZE + C128_EDITOR_ROM_SIZE) < 0) {
            log_error(c128rom_log, "Couldn't load basic ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return c128rom_basic_checksum();
}

int c128rom_load_chargen(const char *rom_name)
{
    if (!rom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load chargen ROM.  */
        if (sysfile_load(rom_name,
            mem_chargen_rom, C128_CHARGEN_ROM_SIZE,
            C128_CHARGEN_ROM_SIZE) < 0) {
            log_error(c128rom_log, "Couldn't load character ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int c128rom_load_kernal64(const char *rom_name)
{
    if (!rom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load C64 kernal ROM.  */
        if (sysfile_load(rom_name,
            kernal64_rom, C128_KERNAL64_ROM_SIZE,
            C128_KERNAL64_ROM_SIZE) < 0) {
            log_error(c128rom_log, "Couldn't load C64 kernal ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int c128rom_load_basic64(const char *rom_name)
{
    if (!rom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load basic ROM.  */
        if (sysfile_load(rom_name,
            basic64_rom, C128_BASIC64_ROM_SIZE,
            C128_BASIC64_ROM_SIZE) < 0) {
            log_error(c128rom_log, "Couldn't load C64 basic ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int c128rom_load_chargen64(const char *rom_name)
{
    if (!rom_loaded)
        return 0;

    if (!IS_NULL(rom_name)) {
        /* Load C64 Chargen ROM.  */
        if (sysfile_load(rom_name,
            chargen64_rom, C128_CHARGEN64_ROM_SIZE,
            C128_CHARGEN64_ROM_SIZE) < 0) {
            log_error(c128rom_log, "Couldn't load C64 character ROM `%s'.",
                      rom_name);
            return -1;
        }
    }
    return 0;
}

int mem_load(void)
{
    char *rom_name = NULL;

    if (c128rom_log == LOG_ERR)
        c128rom_log = log_open("C128MEM");

    mem_powerup();

    mem_page_zero = mem_ram;
    mem_page_one = mem_ram + 0x100;

    mem_initialize_memory();

    rom_loaded = 1;

    if (resources_get_value("KernalName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (c128rom_load_kernal(rom_name) < 0)
        return -1;

    if (resources_get_value("BasicName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (c128rom_load_basic(rom_name) < 0)
        return -1;

    if (resources_get_value("ChargenName", (resource_value_t)&rom_name) < 0)
        return -1;
    if (c128rom_load_chargen(rom_name) < 0)
        return -1;

    if (resources_get_value("Kernal64Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (c128rom_load_kernal64(rom_name) < 0)
        return -1;

    if (resources_get_value("Basic64Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (c128rom_load_basic64(rom_name) < 0)
        return -1;

    if (resources_get_value("Chargen64Name", (resource_value_t)&rom_name) < 0)
        return -1;
    if (c128rom_load_chargen64(rom_name) < 0)
        return -1;

    return 0;
}

