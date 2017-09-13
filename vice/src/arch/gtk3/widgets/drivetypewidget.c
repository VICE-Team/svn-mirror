/** \file   src/arch/gtk3/widgets/drivetypewidget.c
 * \brief   Drive type selection dialog
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]Type
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



#include "drivetypewidget.h"


static int unit_number = 8;



static void on_radio_toggled(GtkWidget *widget, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        gchar buffer[64];
        int type = GPOINTER_TO_INT(user_data);

        g_snprintf(buffer, 64, "Drive%dType", unit_number);
        debug_gtk3("setting %s to %d\n", buffer, type);
        resources_set_int(buffer, type);
    }
}



GtkWidget *create_drive_type_widget(int unit)
{
    GtkWidget *grid;
    drive_type_info_t *list;
    GtkRadioButton *last = NULL;
    GSList *group = NULL;
    size_t i;

    unit_number = unit;

    grid = uihelpers_create_grid_with_label("Drive type", 1);
    list = drive_get_type_info_list();
    for (i = 0; list[i].name != NULL; i++) {
        GtkWidget *radio = gtk_radio_button_new_with_label(group, list[i].name);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
        g_object_set(radio, "margin-left", 16, NULL);

#if 0
        if (callback != NULL) {
            g_signal_connect(radio, "toggled", G_CALLBACK(callback),
                    GINT_TO_POINTER(data[i].value));
        }
#endif
#if 0
        gtk_widget_set_sensitive(radio,
                drive_check_type((unsigned int)(list[i].id),
                    (unsigned int)(unit - 8)));
#endif
        g_signal_connect(radio, "toggled", G_CALLBACK(on_radio_toggled),
                GINT_TO_POINTER(list[i].id));

        gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 1, 1, 1);
        gtk_widget_show(radio);
        last = GTK_RADIO_BUTTON(radio);
    }

    gtk_widget_show_all(grid);
    return grid;
}


void update_drive_type_widget(GtkWidget *widget, int unit)
{
    drive_type_info_t *list;
    size_t i;
    gchar res_name[64];
    int type;

    unit_number = unit;

    g_snprintf(res_name, 64, "Drive%dType", unit);
    resources_get_int(res_name, &type);

    list = drive_get_type_info_list();

    for (i = 0; list[i].name != NULL; i++) {
        GtkWidget *radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 1);
        if (widget != NULL && GTK_IS_RADIO_BUTTON(radio)) {
            gtk_widget_set_sensitive(radio, drive_check_type(
                        (unsigned int)(list[i].id), (unsigned int)(unit - 8)));
            if (list[i].id == type) {
                /* TODO: temporary block the resource-set callback */
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
            }
        }
    }
}


