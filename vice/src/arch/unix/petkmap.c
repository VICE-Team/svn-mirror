/*
 * petkmap.c -- PET keyboard resources handling.
 *
 * Written by
 *  Andre Fachat (a.fachat@physik.tu-chemnitz.de)
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

#include "vice.h"
#include "types.h"
#include "utils.h"
#include "resources.h"
#include "cmdline.h"
#include "kbd.h"

/* ------------------------------------------------------------------------- */

/* keyboard-related resources.  */

/* array of resource names for keyboard - for kbd.c
 * by convention even indexes are symbol mappings, odd are positional */
static char *my_keymap_res_name_list[] = {
	"KeymapBusinessUKSymFile", "KeymapBusinessUKPosFile",
	"KeymapGraphicsSymFile", "KeymapGraphicsPosFile"
};

/* name of keymap file for symbolic and positional mappings */
static char *keymap_file_list[] = {
	NULL, NULL,
	NULL, NULL
};

static int set_keymap_file(int myindex, const char *name)
{
    int index;

    if (keymap_file_list[myindex] != NULL && name != NULL
        && strcmp(name, keymap_file_list[myindex]) == 0)
        return 0;

    if(resources_get_value("KeymapIndex", (resource_value_t*) &index) < 0)
	return -1;

    string_set(&keymap_file_list[myindex], name);

    /* reset index -> reload keymap file if this keymap is active */
    if(index == myindex) {
      resources_set_value("KeymapIndex", (resource_value_t) index);
    }
    return 0;
}

static int set_keymap_gr_sym_file(resource_value_t v)
{
    return set_keymap_file(0, (const char *) v);
}

static int set_keymap_gr_pos_file(resource_value_t v)
{
    return set_keymap_file(1, (const char *) v);
}

static int set_keymap_buk_sym_file(resource_value_t v)
{
    return set_keymap_file(2, (const char *) v);
}

static int set_keymap_buk_pos_file(resource_value_t v)
{
    return set_keymap_file(3, (const char *) v);
}

static resource_t resources[] = {
    { "KeymapGraphicsSymFile", RES_STRING, (resource_value_t) "graphics.vkm",
      (resource_value_t *) &keymap_file_list[0], set_keymap_gr_sym_file },
    { "KeymapGraphicsPosFile", RES_STRING, (resource_value_t) "posg_de.vkm",
      (resource_value_t *) &keymap_file_list[1], set_keymap_gr_pos_file },
    { "KeymapBusinessUKSymFile", RES_STRING, (resource_value_t) "busi_uk.vkm",
      (resource_value_t *) &keymap_file_list[2], set_keymap_buk_sym_file },
    { "KeymapBusinessUKPosFile", RES_STRING, (resource_value_t) "buk_pos.vkm",
      (resource_value_t *) &keymap_file_list[3], set_keymap_buk_pos_file },
    { NULL }
};

int pet_kbd_init_resources(void)
{
    keymap_res_name_list = my_keymap_res_name_list;
    resources_register(resources);
    return do_kbd_init_resources();
}

/* ------------------------------------------------------------------------- */

/* keymap command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-grsymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapGraphicsSymFile",
	NULL, "<name>",
	"Specify name of graphics keyboard symbolic keymap file" },
    { "-grposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapGraphicsPosFile",
	NULL, "<name>",
	"Specify name of graphics keyboard positional keymap file" },
    { "-buksymkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessUKSymFile",
	NULL, "<name>",
	"Specify name of business UK symbolic keymap file" },
    { "-bukposkeymap", SET_RESOURCE, 1, NULL, NULL, "KeymapBusinessUKPosFile",
	NULL, "<name>",
	"Specify name of business UK positional keymap file" },
    { NULL }
};

int pet_kbd_init_cmdline_options(void)
{
    keymap_res_name_list = my_keymap_res_name_list;
    cmdline_register_options(cmdline_options);
    return do_kbd_init_cmdline_options();
}
