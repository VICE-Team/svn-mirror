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

#include "log.h"
#include "parse.h"      // load_keymap_file
#include "util.h"       // util_string_set
#include "cmdline.h"    // cmdline_register_options
#include "resources.h"  // resources_register
#include "translate.h"
#include "keyboard.h"

static char *keymapfile;

/* ------------------------------------------------------------------------ */

int kbd_init(void)
{
    //
    // FIXME: This is a stupid workaround to make sure,
    // that the logging system is initialized when
    // the keymap is loaded
    //

    const char *name;

    resources_get_string("KeymapFile", &name);
    resources_set_string("KeymapFile", name);

    return 0;
}

static int set_keymap_file(const char *val, void *param)
{
    if (load_keymap_file(val) == -1) {
        log_message(LOG_DEFAULT, "kbd.c: Error loading keymapfile `%s'.", keymapfile);
    } else {
        util_string_set(&keymapfile, val);
    }

    return 0;
}

static const resource_string_t resources_string[] = {
    { "KeymapFile", "os2.vkm", RES_EVENT_NO, NULL,
      &keymapfile, set_keymap_file, NULL },
    { NULL }
};

int kbd_resources_init(void)
{
    return resources_register_string(resources_string);
}

static const cmdline_option_t cmdline_options[] = {
    { "-keymap", SET_RESOURCE, 1,
      NULL, NULL, "KeymapFile", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<name>", "Specify the name of the keymap file" },
    { NULL }
};

int kbd_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages/actual detection
*/
int kbd_arch_get_host_mapping(void)
{
    return KBD_MAPPING_US;
}
