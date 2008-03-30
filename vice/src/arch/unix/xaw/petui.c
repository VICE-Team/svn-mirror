/*
 * petui.c - Implementation of the PET-specific part of the UI.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "petui.h"
#include "uimenu.h"
#include "resources.h"
#include "uicommands.h"
#include "uisettings.h"

#ifdef XPM
#include <X11/xpm.h>
#include "c64icon.xpm"
#endif

/* ------------------------------------------------------------------------- */

UI_MENU_DEFINE_TOGGLE(DiagPin)

static ui_menu_entry_t pet_menu[] = {
    { "*PET Userport Diagnostic Pin",
      (ui_callback_t) toggle_DiagPin, NULL, NULL },
    { NULL }
};

int pet_ui_init(void)
{
#ifdef XPM
    {
        Pixmap icon_pixmap;

        /* Create the icon pixmap. */
        XpmCreatePixmapFromData(display, DefaultRootWindow(display), icon_data,
                                &icon_pixmap, NULL, NULL);
        ui_set_application_icon(icon_pixmap);
    }
#endif

    ui_set_left_menu(ui_menu_create("LeftMenu",
                                    ui_disk_commands_menu,
                                    ui_menu_separator,
                                    ui_directory_commands_menu,
                                    ui_menu_separator,
                                    ui_tool_commands_menu,
                                    ui_menu_separator,
                                    ui_help_commands_menu,
                                    ui_menu_separator,
                                    ui_run_commands_menu,
                                    ui_menu_separator,
                                    ui_exit_commands_menu,
                                    NULL));

    ui_set_right_menu(ui_menu_create("RightMenu",
                                     ui_performance_settings_menu,
                                     ui_menu_separator,
                                     ui_video_settings_menu,
                                     ui_sound_settings_menu,
                                     ui_serial_settings_menu,
                                     ui_menu_separator,
                                     pet_menu,
                                     ui_menu_separator,
                                     ui_settings_settings_menu,
                                     NULL));

    ui_update_menus();

    return 0;
}
