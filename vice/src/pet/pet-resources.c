/*
 * pet-resources.c
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

#include "crtc.h"
#include "drive.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "machine.h"
#include "pet-resources.h"
#include "pet.h"
#include "petmem.h"
#include "petrom.h"
#include "pets.h"
#include "resources.h"
#include "util.h"


#define KBD_INDEX_PET_BUKS  0
#define KBD_INDEX_PET_BUKP  1
#define KBD_INDEX_PET_BGRS  2
#define KBD_INDEX_PET_BGRP  3
#define KBD_INDEX_PET_BDES  4
#define KBD_INDEX_PET_BDEP  5


/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;


static int romset_firmware[7];

static int sync_factor;

static int set_iosize(resource_value_t v, void *param)
{
    petres.IOSize = (int)v;

    mem_initialize_memory();
    return 0;
}

static int set_crtc_enabled(resource_value_t v, void *param)
{
    petres.crtc = (int)v;
    return 0;
}

static int set_superpet_enabled(resource_value_t v, void *param)
{
    if ((unsigned int) v < 2)
        petres.superpet = (unsigned int)v;
    mem_initialize_memory();
    return 0;
}

static int set_ram_9_enabled(resource_value_t v, void *param)
{
    if ((unsigned int) v < 2)
        petres.mem9 = (unsigned int)v;
    mem_initialize_memory();
    return 0;
}

static int set_ram_a_enabled(resource_value_t v, void *param)
{
    if ((unsigned int) v < 2)
        petres.memA = (unsigned int)v;
    mem_initialize_memory();
    return 0;
}

static int set_ramsize(resource_value_t v, void *param)
{
    int size = (int) v;
    int i;
    const int sizes[] = { 4, 8, 16, 32, 96, 128 };

    for (i = 0; i < 6; i++) {
        if (size <= sizes[i])
            break;
    }

    if (i > 5)
        i = 5;

    size = sizes[i];

    petres.ramSize = size;
    petres.map = 0;

    if (size == 96) {
        petres.map = 1;         /* 8096 mapping */
    } else if (size == 128) {
        petres.map = 2;         /* 8296 mapping */
    }

    petmem_check_info(&petres);
    mem_initialize_memory();

    return 0;
}

static int set_video(resource_value_t v, void *param)
{
    int col = (int)v;

    if (col != petres.video) {
        if (col == 0 || col == 40 || col == 80) {

            petres.video = col;

            petmem_check_info(&petres);

            pet_crtc_set_screen();
        }
    }
    return 0;
}

/* ROM filenames */

static int set_chargen_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&petres.chargenName, (const char *)v))
        return 0;

    return petrom_load_chargen();
}

static int set_kernal_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&petres.kernalName, (const char *)v))
        return 0;

    return petrom_load_kernal();
}

static int set_basic_rom_name(resource_value_t v, void *param)
{
/*  do we want to reload the basic even with the same name - romB can
    overload the basic ROM image and we can restore it only here ?
*/
    if (util_string_set(&petres.basicName, (const char *)v))
        return 0;

    return petrom_load_basic();
}

static int set_editor_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&petres.editorName, (const char *)v))
        return 0;

    return petrom_load_editor();
}

static int set_rom_module_9_name(resource_value_t v, void *param)
{
    if (util_string_set(&petres.mem9name, (const char *)v))
        return 0;

    return petrom_load_rom9();
}

static int set_rom_module_a_name(resource_value_t v, void *param)
{
    if (util_string_set(&petres.memAname, (const char *)v))
        return 0;

    return petrom_load_romA();
}

static int set_rom_module_b_name(resource_value_t v, void *param)
{
    if (util_string_set(&petres.memBname, (const char *)v))
        return 0;

    return petrom_load_romB();
}

/* Enable/disable patching the PET 2001 chargen ROM/kernal ROM */

static int set_pet2k_enabled(resource_value_t v, void *param)
{
    int i = (((int)v) ? 1 : 0);

    if (i != petres.pet2k) {
        if (petres.pet2k)
            petrom_unpatch_2001();

        petres.pet2k = i;

        if (petres.pet2k)
            petrom_patch_2001();
    }
    return 0;
}

static int set_pet2kchar_enabled(resource_value_t v, void *param)
{
    int i = (((int)v) ? 1 : 0);

    if (i != petres.pet2kchar) {
        petres.pet2kchar = i;

        /* function reverses itself -> no reload necessary */
        petrom_convert_chargen_2k();
    }
    return 0;
}

static int set_eoiblank_enabled(resource_value_t v, void *param)
{
    int i = (((int)v) ? 1 : 0);

    petres.eoiblank = i;

    crtc_enable_hw_screen_blank(petres.eoiblank);

    return 0;
}

/* Enable/disable the Emulator ID.  */

static int set_emu_id_enabled(resource_value_t v, void *param)
{
    emu_id_enabled = (int)v;
    return 0;
}

static int set_sync_factor(resource_value_t v, void *param)
{
    int change_timing = 0;

    if (sync_factor != (int)v)
        change_timing = 1;

    switch ((int)v) {
      case MACHINE_SYNC_PAL:
        sync_factor = (int) v;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_PAL);
        break;
      case MACHINE_SYNC_NTSC:
        sync_factor = (int)v;
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSC);
        break;
      default:
        return -1;
    }
    return 0;
}

static int set_romset_firmware(resource_value_t v, void *param)
{
    unsigned int num = (unsigned int)param;

    romset_firmware[num] = (int)v;

    return 0;
}

static const resource_t resources[] = {
    { "MachineVideoStandard", RES_INTEGER, (resource_value_t)MACHINE_SYNC_PAL,
      (void *)&sync_factor, set_sync_factor, NULL },
    { "RamSize", RES_INTEGER, (resource_value_t)32,
      (void *)&petres.ramSize, set_ramsize, NULL },
    { "IOSize", RES_INTEGER, (resource_value_t)0x800,
      (void *)&petres.IOSize, set_iosize, NULL },
    { "Crtc", RES_INTEGER, (resource_value_t)1,
      (void *)&petres.crtc, set_crtc_enabled, NULL },
    { "VideoSize", RES_INTEGER, (resource_value_t)1,
      (void *)&petres.video, set_video, NULL },
    { "Ram9", RES_INTEGER, (resource_value_t)0,
      (void *)&petres.mem9, set_ram_9_enabled, NULL },
    { "RamA", RES_INTEGER, (resource_value_t)0,
      (void *)&petres.memA, set_ram_a_enabled, NULL },
    { "SuperPET", RES_INTEGER, (resource_value_t)0,
      (void *)&petres.superpet, set_superpet_enabled, NULL },
    { "Basic1", RES_INTEGER, (resource_value_t)1,
      (void *)&petres.pet2k, set_pet2k_enabled, NULL },
    { "Basic1Chars", RES_INTEGER, (resource_value_t)0,
      (void *)&petres.pet2kchar, set_pet2kchar_enabled, NULL },
    { "EoiBlank", RES_INTEGER, (resource_value_t)0,
      (void *)&petres.eoiblank, set_eoiblank_enabled, NULL },
    { "ChargenName", RES_STRING, (resource_value_t)"chargen",
      (void *)&petres.chargenName, set_chargen_rom_name, NULL },
    { "RomsetChargenName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[0], set_romset_firmware, (void *)0 },
    { "KernalName", RES_STRING, (resource_value_t)PET_KERNAL4NAME,
      (void *)&petres.kernalName, set_kernal_rom_name, NULL },
    { "RomsetKernalName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[1], set_romset_firmware, (void *)1 },
    { "EditorName", RES_STRING, (resource_value_t)PET_EDITOR4B80NAME,
      (void *)&petres.editorName, set_editor_rom_name, NULL },
    { "RomsetEditorName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[2], set_romset_firmware, (void *)2 },
    { "BasicName", RES_STRING, (resource_value_t)PET_BASIC4NAME,
      (void *)&petres.basicName, set_basic_rom_name, NULL },
    { "RomsetBasicName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[3], set_romset_firmware, (void *)3 },
    { "RomModule9Name", RES_STRING, (resource_value_t)"",
      (void *)&petres.mem9name, set_rom_module_9_name, NULL },
    { "RomsetRomModule9Name", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[4], set_romset_firmware, (void *)4 },
    { "RomModuleAName", RES_STRING, (resource_value_t)"",
      (void *)&petres.memAname, set_rom_module_a_name, NULL },
    { "RomsetRomModuleAName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[5], set_romset_firmware, (void *)5 },
    { "RomModuleBName", RES_STRING, (resource_value_t)"",
      (void *)&petres.memBname, set_rom_module_b_name, NULL },
    { "RomsetRomModuleBName", RES_INTEGER, (resource_value_t)0,
      (void *)&romset_firmware[6], set_romset_firmware, (void *)6 },
    { "EmuID", RES_INTEGER, (resource_value_t)0,
      (void *)&emu_id_enabled, set_emu_id_enabled, NULL },
#ifdef COMMON_KBD
    { "KeymapIndex", RES_INTEGER, (resource_value_t)KBD_INDEX_PET_BUKS,
      (void *)&machine_keymap_index, keyboard_set_keymap_index, NULL },
    { "KeymapBusinessUKSymFile", RES_STRING, (resource_value_t)KBD_PET_SYM_UK,
      (void *)&machine_keymap_file_list[0],
      keyboard_set_keymap_file, (void *)0 },
    { "KeymapBusinessUKPosFile", RES_STRING, (resource_value_t)KBD_PET_POS_UK,
      (void *)&machine_keymap_file_list[1],
      keyboard_set_keymap_file, (void *)1 },
    { "KeymapGraphicsSymFile", RES_STRING, (resource_value_t)KBD_PET_SYM_GR,
      (void *)&machine_keymap_file_list[2],
      keyboard_set_keymap_file, (void *)2 },
    { "KeymapGraphicsPosFile", RES_STRING, (resource_value_t)KBD_PET_POS_GR,
      (void *)&machine_keymap_file_list[3],
      keyboard_set_keymap_file, (void *)3 },
    { "KeymapBusinessDESymFile", RES_STRING, (resource_value_t)KBD_PET_SYM_DE,
      (void *)&machine_keymap_file_list[4],
      keyboard_set_keymap_file, (void *)4 },
    { "KeymapBusinessDEPosFile", RES_STRING, (resource_value_t)KBD_PET_POS_DE,
      (void *)&machine_keymap_file_list[5],
      keyboard_set_keymap_file, (void *)5 },
#endif
    { NULL }
};

int pet_resources_init(void)
{
    petres.chargenName = NULL;
    petres.kernalName = NULL;
    petres.editorName = NULL;
    petres.basicName = NULL;
    petres.memBname = NULL;
    petres.memAname = NULL;
    petres.mem9name = NULL;

    return resources_register(resources);
}

void pet_resources_shutdown(void)
{
    lib_free(machine_keymap_file_list[0]);
    lib_free(machine_keymap_file_list[1]);
    lib_free(machine_keymap_file_list[2]);
    lib_free(machine_keymap_file_list[3]);
    lib_free(machine_keymap_file_list[4]);
    lib_free(machine_keymap_file_list[5]);
    lib_free(petres.chargenName);
    lib_free(petres.kernalName);
    lib_free(petres.editorName);
    lib_free(petres.basicName);
    lib_free(petres.memBname);
    lib_free(petres.memAname);
    lib_free(petres.mem9name);
}

