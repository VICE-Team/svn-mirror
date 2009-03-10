/*
 * vic20-resources.c
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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
#include <string.h>

#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "util.h"
#include "vic20-resources.h"
#include "vic20mem.h"
#include "vic20rom.h"


#define KBD_INDEX_VIC20_SYM 0
#define KBD_INDEX_VIC20_POS 1


static int romset_firmware[3];

/* What sync factor between the CPU and the drive?  If equal to
   `MACHINE_SYNC_PAL', the same as PAL machines.  If equal to
   `MACHINE_SYNC_NTSC', the same as NTSC machines.  The sync factor is
   calculated as 65536 * drive_clk / clk_[main machine] */
static int sync_factor;

/* Name of the character ROM.  */
static char *chargen_rom_name = NULL;

/* Name of the BASIC ROM.  */
static char *basic_rom_name = NULL;

/* Name of the Kernal ROM.  */
static char *kernal_rom_name = NULL;

/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;

/* Flag: Do we enable the VIC-1112 IEEE488 interface?  */
int ieee488_enabled;

/* which ROMs are loaded - bits are VIC_BLK* */
int mem_rom_blocks;

/* Flag: Do we have RAM block `n'?  */
int ram_block_0_enabled;
int ram_block_1_enabled;
int ram_block_2_enabled;
int ram_block_3_enabled;
int ram_block_5_enabled;

/* ------------------------------------------------------------------------- */

static int set_chargen_rom_name(const char *val, void *param)
{
    if (util_string_set(&chargen_rom_name, val))
        return 0;

    return vic20rom_load_chargen(chargen_rom_name);
}

static int set_kernal_rom_name(const char *val, void *param)
{
    if (util_string_set(&kernal_rom_name, val))
        return 0;

    return vic20rom_load_kernal(kernal_rom_name);
}

static int set_basic_rom_name(const char *val, void *param)
{
    if (util_string_set(&basic_rom_name, val))
        return 0;

    return vic20rom_load_basic(basic_rom_name);
}

/* Ugly hack...  */
#define DEFINE_SET_BLOCK_FUNC(num)                                          \
    static int set_ram_block_##num##_enabled(int value, void *param)        \
    {                                                                       \
        ram_block_##num##_enabled = value;                                  \
        if (value) {                                                        \
            mem_rom_blocks &= (VIC_ROM_BLK##num##A | VIC_ROM_BLK##num##B);  \
            return vic20_mem_enable_ram_block(num);                         \
        } else                                                              \
            return vic20_mem_disable_ram_block(num);                        \
    }

DEFINE_SET_BLOCK_FUNC(0)
DEFINE_SET_BLOCK_FUNC(1)
DEFINE_SET_BLOCK_FUNC(2)
DEFINE_SET_BLOCK_FUNC(3)
DEFINE_SET_BLOCK_FUNC(5)

static int set_emu_id_enabled(int val, void *param)
{
    emu_id_enabled = val;
    return 0;
}

static int set_ieee488_enabled(int val, void *param)
{
    ieee488_enabled = val;

    ui_update_menus();

    return 0;
}

#if 0
/* Enable/disable the Emulator ID.  */
void mem_toggle_emu_id(int flag)
{
    emu_id_enabled = flag;
}
#endif

static int set_sync_factor(int val, void *param)
{
    int change_timing = 0;

    if (sync_factor != val)
        change_timing = 1;

    switch (val) {
      case MACHINE_SYNC_PAL:
        sync_factor = val;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_PAL);
        break;
      case MACHINE_SYNC_NTSC:
        sync_factor = val;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSC);
        break;
      default:
        return -1;
    }
    return 0;
}

static int set_romset_firmware(int val, void *param)
{
    unsigned int num = (unsigned int)(unsigned long)param;

    romset_firmware[num] = val;

    return 0;
}

static const resource_string_t resources_string[] =
{
    { "ChargenName", "chargen", RES_EVENT_NO, NULL,
      &chargen_rom_name, set_chargen_rom_name, NULL },
    { "KernalName", "kernal", RES_EVENT_NO, NULL,
      &kernal_rom_name, set_kernal_rom_name, NULL },
    { "BasicName", "basic", RES_EVENT_NO, NULL,
      &basic_rom_name, set_basic_rom_name, NULL },
#ifdef COMMON_KBD
    { "KeymapSymFile", KBD_VIC20_SYM, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[0], keyboard_set_keymap_file, (void *)0 },
    { "KeymapPosFile", KBD_VIC20_POS, RES_EVENT_NO, NULL,
      &machine_keymap_file_list[1], keyboard_set_keymap_file, (void *)1 },
#endif
    {NULL}
};

static const resource_int_t resources_int[] =
{
    { "MachineVideoStandard", MACHINE_SYNC_PAL, RES_EVENT_SAME, NULL,
      &sync_factor, set_sync_factor, NULL },
    { "RomsetChargenName", 0, RES_EVENT_NO, NULL,
      &romset_firmware[0], set_romset_firmware, (void *)0 },
    { "RomsetKernalName", 0, RES_EVENT_NO, NULL,
      &romset_firmware[1], set_romset_firmware, (void *)1 },
    { "RomsetBasicName", 0, RES_EVENT_NO, NULL,
      &romset_firmware[2], set_romset_firmware, (void *)2 },
    { "RAMBlock0", 1, RES_EVENT_SAME, NULL,
      &ram_block_0_enabled, set_ram_block_0_enabled, NULL },
    { "RAMBlock1", 1, RES_EVENT_SAME, NULL,
      &ram_block_1_enabled, set_ram_block_1_enabled, NULL },
    { "RAMBlock2", 1, RES_EVENT_SAME, NULL,
      &ram_block_2_enabled, set_ram_block_2_enabled, NULL },
    { "RAMBlock3", 1, RES_EVENT_SAME, NULL,
      &ram_block_3_enabled, set_ram_block_3_enabled, NULL },
    { "RAMBlock5", 1, RES_EVENT_SAME, NULL,
      &ram_block_5_enabled, set_ram_block_5_enabled, NULL },
    { "EmuID", 0, RES_EVENT_SAME, NULL,
      &emu_id_enabled, set_emu_id_enabled, NULL },
    { "IEEE488", 0, RES_EVENT_SAME, NULL,
      &ieee488_enabled, set_ieee488_enabled, NULL },
#ifdef COMMON_KBD
    { "KeymapIndex", KBD_INDEX_VIC20_DEFAULT, RES_EVENT_NO, NULL,
      &machine_keymap_index, keyboard_set_keymap_index, NULL },
#endif
    {NULL}
};

int vic20_resources_init(void)
{
    if (resources_register_string(resources_string) < 0)
        return -1;

    return resources_register_int(resources_int);
}

void vic20_resources_shutdown(void)
{
    lib_free(chargen_rom_name);
    lib_free(basic_rom_name);
    lib_free(kernal_rom_name);
    lib_free(machine_keymap_file_list[0]);
    lib_free(machine_keymap_file_list[1]);
}

