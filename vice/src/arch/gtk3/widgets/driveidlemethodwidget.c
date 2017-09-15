/** \file   src/arch/gtk3/widgets/driveidlemethodwidget.c
 * \brief   Drive expansions widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]IdleMethod
 *
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

#include <gtk/gtk.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "drive.h"
#include "drive-check.h"

#include "driveidlemethodwidget.h"


static int unit_number = 8;


static ui_text_int_pair_t idle_methods[] = {
    { "None", 0 },
    { "Skip cycles", 1 },
    { "Trap idle", 2 },
    { NULL, -1 }
};


static void on_idle_method_changed(GtkWidget *widget, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        char res_name[256];
        int value = GPOINTER_TO_INT(user_data);

        g_snprintf(res_name, 256, "Drive%dIdleMethod", unit_number);
        debug_gtk3("setting %s to %d\n", res_name, value);
        resources_set_int(res_name, value);
    }
}



GtkWidget *create_drive_idle_method_widget(int unit)
{
    GtkWidget *widget;

    unit_number = unit;

    widget = uihelpers_create_int_radiogroup_with_label(
            "Idle method",
            idle_methods,
            on_idle_method_changed,
            0);
    return widget;
}


void update_drive_idle_method_widget(GtkWidget *widget, int unit)
{
    char res_name[256];
    int value;

    unit_number = unit;

    snprintf(res_name, 256, "Drive%dIdleMethod", unit);
    resources_get_int(res_name, &value);
    uihelpers_set_radio_button_grid_by_index(widget, value);
}





