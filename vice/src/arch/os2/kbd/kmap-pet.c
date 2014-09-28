/*
 * petkmap.c -- PET keyboard resources handling.
 *
 * Written by
 *  Andre Fachat <a.fachat@physik.tu-chemnitz.de>
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

//#include "vice.h"

#include <stdio.h>

#include "resources.h"
#include "cmdline.h"
#include "kbd.h"
#include "translate.h"

/* ------------------------------------------------------------------------- */

/* keyboard-related resources.  */

#define NUM_KEYBOARD_MAPPINGS	6

/* array of resource names for keyboard - for kbd.c
 * by convention even indexes are symbol mappings, odd are positional */
static const char *my_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = {
    "KeymapBusinessUKSymFile", "KeymapBusinessUKPosFile",
    "KeymapGraphicsSymFile", "KeymapGraphicsPosFile",
    "KeymapBusinessDESymFile", "KeymapBusinessDEPosFile"
};

/* name of keymap file for symbolic and positional mappings */
static char *keymap_file_list[NUM_KEYBOARD_MAPPINGS] = { NULL, NULL, NULL, NULL, NULL, NULL };

static int set_keymap_file(int myindex, const char *name)
{
    int kmap_index;

    if (myindex >= NUM_KEYBOARD_MAPPINGS) {
        return -1;
    }

    if (resources_get_int("KeymapIndex", &kmap_index) < 0) {
        return -1;
    }

    if (util_string_set(&keymap_file_list[myindex], name)) {
        return 0;
    }

    /* reset index -> reload keymap file if this keymap is active */
    if (kmap_index == myindex) {
        resources_set_int("KeymapIndex", kmap_index);
    }
    return 0;
}

static int set_keymap_file(const char *val, void *param)
{
    int nr = vice_ptr_to_int(param);

    return set_keymap_file(nr, val);
}

static const resource_string_t resources_string[] = {
    { "KeymapBusinessUKSymFile", "busi_uk.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[0], set_keymap_file, (void *)0 },
    { "KeymapBusinessUKPosFile", "buk_pos.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[1], set_keymap_file, (void *)1 },
    { "KeymapGraphicsSymFile", "graphics.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[2], set_keymap_file, (void *)2 },
    { "KeymapGraphicsPosFile", "posg_de.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[3], set_keymap_file, (void *)3 },
    { "KeymapBusinessDESymFile", "busi_de.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[4], set_keymap_file, (void *)4 },
    { "KeymapBusinessDEPosFile", "bde_pos.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[5], set_keymap_file, (void *)5 },
    { NULL }
};

int pet_kbd_resources_init(void)
{
    keymap_res_name_list = my_keymap_res_name_list;

    if (resources_register_string(resources_string) < 0) {
        return -1;
    }

    return do_kbd_init_resources();
}

/* ------------------------------------------------------------------------- */

/* keymap command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-grsymkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapGraphicsSymFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the graphics keyboard symbolic keymap file" },
    { "-grposkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapGraphicsPosFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the graphics keyboard positional keymap file" },
    { "-buksymkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessUKSymFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the UK business keyboard symbolic keymap file" },
    { "-bukposkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessUKPosFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the UK business keyboard positional keymap file" },
    { "-bdesymkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessDESymFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the German business keyboard symbolic keymap file" },
    { "-bdeposkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapBusinessDEPosFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the German business keyboard positional keymap file" },
    { NULL }
};

int pet_kbd_cmdline_options_init(void)
{
    keymap_res_name_list = my_keymap_res_name_list;
    cmdline_register_options(cmdline_options);
    return do_kbd_init_cmdline_options();
}
