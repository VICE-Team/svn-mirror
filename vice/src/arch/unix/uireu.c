/*
 * uireu.c
 *
 * Written by
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
#include <stdlib.h>

#include "resources.h"
#include "uimenu.h"
#include "utils.h"
#include "vsync.h"


UI_MENU_DEFINE_TOGGLE(REU)
UI_MENU_DEFINE_RADIO(REUsize)

UI_CALLBACK(set_reu_image_name)
{
    char *resname = (char *)UI_MENU_CB_PARAM;
    char *title;
    ui_button_t button;
    char *value;
    char *new_value;
    int len;

    vsync_suspend_speed_eval();
    title = stralloc(_("REU image name"));

    resources_get_value(resname, (resource_value_t *)&value);

    if (value == NULL)
        value = "";

    len = strlen(value) * 2;
    if (len < 255)
        len = 255;

    new_value = xmalloc(len + 1);
    strcpy(new_value, value);

    button = ui_input_string(title, _("Name:"), new_value, len);
    free(title);

    if (button == UI_BUTTON_OK)
        resources_set_value(resname, (resource_value_t)new_value);

    free(new_value);
}

static ui_menu_entry_t reu_size_submenu[] = {
    { "*128KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)128, NULL },
    { "*256KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)256, NULL },
    { "*512KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)512, NULL },
    { "*1024KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)1024, NULL },
    { "*2048KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)2048, NULL },
    { "*4096KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)4096, NULL },
    { "*8192KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)8192, NULL },
    { "*16384KB", (ui_callback_t)radio_REUsize,
      (ui_callback_data_t)16384, NULL },
    { NULL }
};

ui_menu_entry_t reu_submenu[] = {
    { N_("*Enable REU"),
      (ui_callback_t)toggle_REU, NULL, NULL },
    { N_("REU size"),
      NULL, NULL, reu_size_submenu },
    { N_("REU image name..."),
      (ui_callback_t)set_reu_image_name,
      (ui_callback_data_t)"REUfilename", NULL },
    { NULL }
};

