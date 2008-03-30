/*
 * c610-resources.c
 *
 * Written by
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "archdep.h"
#include "c610mem.h"
#include "c610tpi.h"
#include "crtc.h"
#include "drive.h"
#include "interrupt.h"
#include "kbd.h"
#include "keyboard.h"
#include "machine.h"
#include "resources.h"
#include "sid-resources.h"
#include "utils.h"
#include "vsync.h"


static int sync_factor;

static char *kernal_rom_name = NULL;
static char *chargen_name = NULL;
static char *basic_rom_name = NULL;

static char *cart_1_name = NULL;
static char *cart_2_name = NULL;
static char *cart_4_name = NULL;
static char *cart_6_name = NULL;

int cbm2_model_line = 0;
static int use_vicii = 0;

int cart08_ram = 0;
int cart1_ram = 0;
int cart2_ram = 0;
int cart4_ram = 0;
int cart6_ram = 0;
int cartC_ram = 0;

/* Flag: Do we enable the Emulator ID?  */
int emu_id_enabled;

static BYTE model_port_mask[] = { 0xc0, 0x40, 0x00 };

extern int isC500;

/* ramsize starts counting at 0x10000 if less than 512. If 512 or more,
   it starts counting at 0x00000.
   CBM2MEM module requires(!) that ramsize never gets 512-64 = 448
   (Just don't ask...)
   In a C500, that has RAM in bank 0, the actual size of RAM is 64k larger
   than ramsize when ramsize is less than 512, otherwise the same as ramsize.
*/

int ramsize;

static int set_ramsize(resource_value_t v, void *param)
{
    int rs = (int)v;

    if (rs!=64 && rs!=128 && rs!=256 && rs!=512 && rs!=1024)
        return -1;

    ramsize = rs;
    vsync_suspend_speed_eval();
    mem_initialize_memory();
    mem_powerup();
    maincpu_trigger_reset();
    return 0;
}

static int set_cbm2_model_line(resource_value_t v, void *param)
{
    int tmp = (int)v;

    if (tmp >= 0 && tmp < 3)
        cbm2_model_line = (int)v;

    set_cbm2_model_port_mask(model_port_mask[cbm2_model_line]);

    if (isC500) {
        /* VIC-II config */
    } else {
        crtc_set_screen_options(80, 25 * (cbm2_model_line ? 10 : 14));
    }

    return 0;
}

static int set_use_vicii(resource_value_t v, void *param)
{
    int tmp = (int)v;

    if (tmp < 0 || tmp > 1)
        return -1;

    use_vicii = tmp;

    /* on boot, select video chip. FIXME: change on runtime */
    if (isC500 < 1)
        isC500 = use_vicii;

    return 0;
}

static int set_chargen_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&chargen_name, (const char *)v))
        return 0;

    return mem_load_chargen(chargen_name);
}

static int set_kernal_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&kernal_rom_name, (const char *)v))
        return 0;

    return mem_load_kernal(kernal_rom_name);
}

static int set_basic_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&basic_rom_name, (const char *)v))
        return 0;

    return mem_load_basic(basic_rom_name);
}

static int set_cart1_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&cart_1_name, (const char *)v))
        return 0;

    return mem_load_cart_1(cart_1_name);
}

static int set_cart2_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&cart_2_name, (const char *)v))
        return 0;

    return mem_load_cart_2(cart_2_name);
}

static int set_cart4_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&cart_4_name, (const char *)v))
        return 0;

    return mem_load_cart_4(cart_4_name);
}

static int set_cart6_rom_name(resource_value_t v, void *param)
{
    if (util_string_set(&cart_6_name, (const char *)v))
        return 0;

    return mem_load_cart_6(cart_6_name);
    /* only does something after mem_load() */
}

static int set_cart08_ram(resource_value_t v, void *param)
{
    cart08_ram = (int)v;
    mem_initialize_memory_bank(15);
    return 0;
}

static int set_cart1_ram(resource_value_t v, void *param)
{
    cart1_ram = (int)v;
    mem_initialize_memory_bank(15);
    return 0;
}

static int set_cart2_ram(resource_value_t v, void *param)
{
    cart2_ram = (int)v;
    mem_initialize_memory_bank(15);
    return 0;
}

static int set_cart4_ram(resource_value_t v, void *param)
{
    cart4_ram = (int)v;
    mem_initialize_memory_bank(15);
    return 0;
}

static int set_cart6_ram(resource_value_t v, void *param)
{
    cart6_ram = (int)v;
    mem_initialize_memory_bank(15);
    return 0;
}

static int set_cartC_ram(resource_value_t v, void *param)
{
    cartC_ram = (int)v;
    mem_initialize_memory_bank(15);
    return 0;
}

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
        drive_set_pal_sync_factor();
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_PAL);
        break;
      case MACHINE_SYNC_NTSC:
        sync_factor = (int)v;
        drive_set_ntsc_sync_factor();
        if (change_timing)
            machine_change_timing(MACHINE_SYNC_NTSC);
        break;
      default:
        if ((int)v > 0) {
            sync_factor = (int)v;
            drive_set_sync_factor((unsigned int)v);
        } else {
            return -1;
        }
    }
    return 0;
}


static resource_t resources[] = {
    { "MachineVideoStandard", RES_INTEGER, (resource_value_t)MACHINE_SYNC_PAL,
      (resource_value_t *)&sync_factor,
      set_sync_factor, NULL },
    { "RamSize", RES_INTEGER, (resource_value_t)128,
      (resource_value_t *)&ramsize,
      set_ramsize, NULL },
    { "ChargenName", RES_STRING, (resource_value_t)CBM2_CHARGEN600,
      (resource_value_t *)&chargen_name,
      set_chargen_rom_name, NULL },
    { "KernalName", RES_STRING, (resource_value_t)"kernal",
      (resource_value_t *)&kernal_rom_name,
      set_kernal_rom_name, NULL },
    { "BasicName", RES_STRING, (resource_value_t)CBM2_BASIC128,
      (resource_value_t *)&basic_rom_name,
      set_basic_rom_name, NULL },
    { "Cart1Name", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&cart_1_name,
      set_cart1_rom_name, NULL },
    { "Cart2Name", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&cart_2_name,
      set_cart2_rom_name, NULL },
    { "Cart4Name", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&cart_4_name,
      set_cart4_rom_name, NULL },
    { "Cart6Name", RES_STRING, (resource_value_t)"",
      (resource_value_t *)&cart_6_name,
      set_cart6_rom_name, NULL },
    { "Ram08", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&cart08_ram,
      set_cart08_ram, NULL },
    { "Ram1", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&cart1_ram,
      set_cart1_ram, NULL },
    { "Ram2", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&cart2_ram,
      set_cart2_ram, NULL },
    { "Ram4", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&cart4_ram,
      set_cart4_ram, NULL },
    { "Ram6", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&cart6_ram,
      set_cart6_ram, NULL },
    { "RamC", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&cartC_ram,
      set_cartC_ram, NULL },
    { "UseVicII", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&use_vicii,
      set_use_vicii, NULL },
    { "ModelLine", RES_INTEGER, (resource_value_t)2,
      (resource_value_t *)&cbm2_model_line,
      set_cbm2_model_line, NULL },
    { "EmuID", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&emu_id_enabled,
      set_emu_id_enabled, NULL },
#ifdef COMMON_KBD
    { "KeymapIndex", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&machine_keymap_index,
      keyboard_set_keymap_index, NULL },
    { "KeymapBusinessUKSymFile", RES_STRING,
      (resource_value_t)KBD_PET_BUKS,
      (resource_value_t *)&machine_keymap_file_list[0],
      keyboard_set_keymap_file, (void *)0 },
    { "KeymapBusinessUKPosFile", RES_STRING,
      (resource_value_t)KBD_PET_BUKP,
      (resource_value_t *)&machine_keymap_file_list[1],
      keyboard_set_keymap_file, (void *)1 },
    { "KeymapGraphicsSymFile", RES_STRING,
      (resource_value_t)KBD_PET_BGRS,
      (resource_value_t *)&machine_keymap_file_list[2],
      keyboard_set_keymap_file, (void *)2 },
    { "KeymapGraphicsPosFile", RES_STRING,
      (resource_value_t)KBD_PET_BGRP,
      (resource_value_t *)&machine_keymap_file_list[3],
      keyboard_set_keymap_file, (void *)3 },
    { "KeymapBusinessDESymFile", RES_STRING,
      (resource_value_t)KBD_PET_BDES,
      (resource_value_t *)&machine_keymap_file_list[4],
      keyboard_set_keymap_file, (void *)4 },
    { "KeymapBusinessDEPosFile", RES_STRING,
      (resource_value_t)KBD_PET_BDEP,
      (resource_value_t *)&machine_keymap_file_list[5],
      keyboard_set_keymap_file, (void *)5 },
#endif
    { "SidStereoAddressStart", RES_INTEGER, (resource_value_t)0xda20,
      (resource_value_t *)&sid_stereo_address_start,
      sid_set_sid_stereo_address, NULL },
    { NULL }
};

int c610_resources_init(void)
{
    return resources_register(resources);
}

