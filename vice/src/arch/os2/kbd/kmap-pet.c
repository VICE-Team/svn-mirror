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

//#include "vice.h"
//#include "types.h"
//#include "utils.h"
#include "resources.h"
#include "cmdline.h"
#include "kbd.h"

/* ------------------------------------------------------------------------- */

/* keyboard-related resources.  */

#define	NUM_KEYBOARD_MAPPINGS	6

/* array of resource names for keyboard - for kbd.c
 * by convention even indexes are symbol mappings, odd are positional */
static const char *my_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = {
	"KeymapBusinessUKSymFile", "KeymapBusinessUKPosFile",
	"KeymapGraphicsSymFile", "KeymapGraphicsPosFile",
	"KeymapBusinessDESymFile", "KeymapBusinessDEPosFile"
};

/* name of keymap file for symbolic and positional mappings */
static char *keymap_file_list[NUM_KEYBOARD_MAPPINGS] = {
	NULL, NULL,
	NULL, NULL,
	NULL, NULL
};

static int set_keymap_file(int myindex, const char *name)
{
    long kmap_index;

    if (myindex >= NUM_KEYBOARD_MAPPINGS) {
	return -1;
    }

    if (resources_get_value("KeymapIndex", (void *)&kmap_index) < 0)
	return -1;

    if (util_string_set(&keymap_file_list[myindex], name))
        return 0;

    /* reset index -> reload keymap file if this keymap is active */
    if(kmap_index == myindex) {
      resources_set_value("KeymapIndex", (resource_value_t) kmap_index);
    }
    return 0;
}

static int set_keymap_buk_sym_file(resource_value_t v, void *param)
{
    return set_keymap_file(0, (const char *) v);
}

static int set_keymap_buk_pos_file(resource_value_t v, void *param)
{
    return set_keymap_file(1, (const char *) v);
}

static int set_keymap_gr_sym_file(resource_value_t v, void *param)
{
    return set_keymap_file(2, (const char *) v);
}

static int set_keymap_gr_pos_file(resource_value_t v, void *param)
{
    return set_keymap_file(3, (const char *) v);
}

static int set_keymap_bde_sym_file(resource_value_t v, void *param)
{
    return set_keymap_file(4, (const char *) v);
}

static int set_keymap_bde_pos_file(resource_value_t v, void *param)
{
    return set_keymap_file(5, (const char *) v);
}

static const resource_t resources[] = {
    { "KeymapBusinessUKSymFile", RES_STRING, (resource_value_t) "busi_uk.vkm",
      (void *) &keymap_file_list[0], set_keymap_buk_sym_file, NULL },
    { "KeymapBusinessUKPosFile", RES_STRING, (resource_value_t) "buk_pos.vkm",
      (void *) &keymap_file_list[1], set_keymap_buk_pos_file, NULL },
    { "KeymapGraphicsSymFile", RES_STRING, (resource_value_t) "graphics.vkm",
      (void *) &keymap_file_list[2], set_keymap_gr_sym_file, NULL },
    { "KeymapGraphicsPosFile", RES_STRING, (resource_value_t) "posg_de.vkm",
      (void *) &keymap_file_list[3], set_keymap_gr_pos_file, NULL },
    { "KeymapBusinessDESymFile", RES_STRING, (resource_value_t) "busi_de.vkm",
      (void *) &keymap_file_list[4], set_keymap_bde_sym_file, NULL },
    { "KeymapBusinessDEPosFile", RES_STRING, (resource_value_t) "bde_pos.vkm",
      (void *) &keymap_file_list[5], set_keymap_bde_pos_file, NULL },
    NULL
};

int pet_kbd_resources_init(void)
{
    keymap_res_name_list = my_keymap_res_name_list;
    resources_register(resources);
    return do_kbd_init_resources();
}

/* ------------------------------------------------------------------------- */

/* keymap command-line options.  */

static const cmdline_option_t cmdline_options[] = {
    { "-grsymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapGraphicsSymFile",
	NULL, "<name>",
	"Specify name of graphics keyboard symbolic keymap file" },
    { "-grposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapGraphicsPosFile",
	NULL, "<name>",
	"Specify name of graphics keyboard positional keymap file" },
    { "-buksymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessUKSymFile",
	NULL, "<name>",
	"Specify name of UK business keyboard symbolic keymap file" },
    { "-bukposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessUKPosFile",
	NULL, "<name>",
	"Specify name of UK business keyboard positional keymap file" },
    { "-bdesymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessDESymFile",
	NULL, "<name>",
	"Specify name of German business keyboard symbolic keymap file" },
    { "-bdeposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessDEPosFile",
        NULL, "<name>",
        "Specify name of German business keyboard positional keymap file" },
    NULL
};

int pet_kbd_cmdline_options_init(void)
{
    keymap_res_name_list = my_keymap_res_name_list;
    cmdline_register_options(cmdline_options);
    return do_kbd_init_cmdline_options();
}
