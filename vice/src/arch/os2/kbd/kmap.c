/*
 * kmap.c -- C64/C128/VIC20 keyboard resources handling.
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

#include <stdio.h>

#include "resources.h"
#include "cmdline.h"
#include "kbd.h"
#include "translate.h"

#include "kbd/parse.h"

/* ------------------------------------------------------------------------- */

/* keyboard-related resources.  */

#define NUM_KEYBOARD_MAPPINGS 2

/* array of resource names for keyboard - for kbd.c
 * by convention even indexes are symbol mappings, odd are positional */
static const char *my_keymap_res_name_list[NUM_KEYBOARD_MAPPINGS] = { "KeymapSymFile", "KeymapPosFile" };

/* name of keymap file for symbolic and positional mappings */
static char *keymap_file_list[NUM_KEYBOARD_MAPPINGS] = { NULL, NULL };

static int set_keymap_file(int myindex, const char *name)
{
    int kindex;

    if (myindex >= NUM_KEYBOARD_MAPPINGS) {
        return -1;
    }

    if (resources_get_int("KeymapIndex", &kindex) < 0) {
        return -1;
    }

    if (util_string_set(&keymap_file_list[myindex], name)) {
        return 0;
    }

    /* reset kindex -> reload keymap file if this keymap is active */
    if (kindex == myindex) {
        resources_set_int("KeymapIndex", kindex);
    }

    return 0;
}

static int set_keymap_file(const char *val, void *param)
{
    int nr = vice_ptr_to_int(param);

    return set_keymap_file(nr, val);
}

static const resource_string_t resources_string[] = {
    { "KeymapSymFile", "default.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[0], set_keymap_file, (void *)0 },
    { "KeymapPosFile", "position.vkm", RES_EVENT_NO, NULL,
      &keymap_file_list[1], set_keymap_file, (void *)1 },
    { NULL }
};

int kbd_resources_init(void)
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
    { "-symkeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapSymFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify name of symbolic keymap file" },
    { "-poskeymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapPosFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify name of positional keymap file" },
    { NULL }
};

int kbd_cmdline_options_init(void)
{
    keymap_res_name_list = my_keymap_res_name_list;
    cmdline_register_options(cmdline_options);
    return do_kbd_init_cmdline_options();
}
