/*
 * kbdlayoutwidget.c - GTK3 keyboard layout widget for the settings dialog
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "ui.h"
#include "resources.h"
#include "vsync.h"
#include "widgethelpers.h"

#include "debug_gtk3.h"

#include "kbdlayoutwidget.h"



/** \brief  Keyboard layout types
 */
static ui_text_int_pair_t kbd_layouts[] = {
    { "American", 0 },
    { "British", 1 },
    { "German", 2 },
    { "Danish", 3 },
    { "Norwegian", 4 },
    { "Finnish", 5 },
    { "Italian", 6 },
    { NULL, -1 }
};


static void on_layout_changed(GtkWidget *widget, gpointer user_data)
{
    int index = GPOINTER_TO_INT(user_data);

    debug_gtk3("setting layout to %d\n", index);
    resources_set_int("KeyboardMapping", index);
}



/** \brief  Create a keyboard layout selection widget
 *
 * \return  GtkWidget
 *
 * \fixme   I'm not really satisfied with the 'select file' buttons, perhaps
 *          they should be placed next to the radio buttons?
 */
GtkWidget *create_kbdlayout_widget(void)
{
    GtkWidget *layout;
    int index = 0;

    resources_get_int("KeyboardMapping", &index);

    /* create grid with label and six radio buttons */
    layout = uihelpers_create_int_radiogroup_with_label(
            "Keyboard layout", kbd_layouts, on_layout_changed, index);

    gtk_widget_show_all(layout);
    return layout;
}


