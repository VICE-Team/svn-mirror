/*
 * uireu.c - REU UI interface for MS-DOS.
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

#include "resources.h"
#include "tui.h"
#include "tuimenu.h"
#include "uireu.h"


static tui_menu_t ui_reu_submenu;


TUI_MENU_DEFINE_TOGGLE(REU)
TUI_MENU_DEFINE_RADIO(REUsize)


static TUI_MENU_CALLBACK(reu_size_submenu_callback)
{
    int value;
    static char s[100];

    resources_get_value("REUsize", (void *)&value);
    sprintf(s, "%dKB",value);
    return s;
}

static tui_menu_item_def_t reu_size_submenu[] = {
    { "_128KB", NULL, radio_REUsize_callback,
      (void *)128, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_256KB", NULL, radio_REUsize_callback,
      (void *)256, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_512KB", NULL, radio_REUsize_callback,
      (void *)512, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "102_4KB", NULL, radio_REUsize_callback,
      (void *)1024, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "2_048KB", NULL, radio_REUsize_callback,
      (void *)2048, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "40_96KB", NULL, radio_REUsize_callback,
      (void *)4096, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "_8192KB", NULL, radio_REUsize_callback,
      (void *)8192, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "1_6384KB", NULL, radio_REUsize_callback,
      (void *)16384, 7, TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(reu_image_file_callback)
{
    char s[256], *v;

    if (been_activated) {

        *s = '\0';

        if (tui_input_string("Change REU image name",
                             "New image name:", s, 255) == -1)
            return NULL;

        if (*s == '\0')
            return NULL;

        resources_set_value("REUfilename", (resource_value_t)s);
    }

    resources_get_value("REUfilename", (void *)&v);
    return v;
}

static tui_menu_item_def_t reu_menu_items[] = {
    { "_Enable REU:", "Emulate RAM Expansion Unit",
      toggle_REU_callback, NULL, 3,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "REU _size:", "Select the size of the REU",
      reu_size_submenu_callback, NULL, 7,
      TUI_MENU_BEH_CONTINUE, reu_size_submenu,
      "REU size" },
    { "REU _image file:", "Select the REU image file",
      reu_image_file_callback, NULL, 20,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { NULL }
};

void uireu_init(struct tui_menu *io_submenu)
{
    ui_reu_submenu = tui_menu_create("REU settings", 1);

    tui_menu_add(ui_reu_submenu, reu_menu_items);

    tui_menu_add_submenu(io_submenu, "_REU settings...",
                         "REU settings",
                         ui_reu_submenu, NULL, 0,
                         TUI_MENU_BEH_CONTINUE);

    /*tui_menu_add(io_submenu, reu_menu_items);*/
}

