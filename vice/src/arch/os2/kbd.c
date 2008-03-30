/*
 * kbd.c - Keyboard emulation. (OS/2)
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#include <os2.h>  // CCHMAXPATH

#include <string.h>

#include "kbd.h"
#include "parse.h"
#include "utils.h"
#include "cmdline.h"
#include "resources.h"

BYTE joystick_value[3];

static int keymap_index;
static char *keymapfile;//[CCHMAXPATH];

/* ------------------------------------------------------------------------ */

int kbd_init(int num, ...)
{
    keyconvmap.map[0] = xmalloc(256*sizeof(keyconv));
    keyconvmap.map[1] = xmalloc(256*sizeof(keyconv));

    load_keymap_file("os2.vkm");
    if (strcmp(keymapfile, "os2.vkm"))
        load_keymap_file(keymapfile);
    return 0;
}

static int set_keymap_index(resource_value_t v)
{
    keymap_index = (int)v;

    return 0;
}

static int set_keymap_file(resource_value_t vi, void *param)
{
    string_set(&keymapfile, (const char*) v);

    return 0;
}


static resource_t resources[] = {
    { "KeymapFile", RES_STRING, (resource_value_t) "os2.vkm",
      (resource_value_t *) &keymapfile, set_keymap_file, NULL },
    { NULL }
};

int kbd_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
     { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapFile", NULL,
      "<name>", "Specify name of keymap file" },
    { NULL },
};

int kbd_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

