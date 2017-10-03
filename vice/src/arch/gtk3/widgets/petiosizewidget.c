/** \file   src/arch/gtk3/widgets/petiosizewidget.c
 * \brief   Widget to set the PET I/O area size (PET 8296 only)
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  IOSize - I/O area size in bytes (256 or 2048)
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

#include "petiosizewidget.h"


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * Sets the IOSize resource when it has been changed.
 *
 * \param[in]   widget      radio button triggering the event
 * \param[in]   user_data   value for the resource (`int`)
 */
static void on_io_size_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("IOSize", &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && (new_val != old_val)) {
        debug_gtk3("setting IOSize to %d bytes\n", new_val);
        resources_set_int("IOSize", new_val);
    }
}


/** \brief  Create PET I/O area size widget
 *
 * \return  GtkGrid
 */
GtkWidget *pet_io_size_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *radio_256;
    GtkWidget *radio_2kb;
    GSList *group = NULL;
    int size;

    resources_get_int("IOSize", &size);

    grid = uihelpers_create_grid_with_label("I/O area size", 1);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    radio_256 = gtk_radio_button_new_with_label(group, "256 bytes");
    g_object_set(radio_256, "margin-left", 16, NULL);
    radio_2kb = gtk_radio_button_new_with_label(group, "2048 bytes");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio_256),
            GTK_RADIO_BUTTON(radio_2kb));

    gtk_grid_attach(GTK_GRID(grid), radio_256, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), radio_2kb, 1, 1, 1, 1);

    g_signal_connect(radio_256, "toggled", G_CALLBACK(on_io_size_toggled),
           GINT_TO_POINTER(256));
    g_signal_connect(radio_2kb, "toggled", G_CALLBACK(on_io_size_toggled),
            GINT_TO_POINTER(2048));

    gtk_widget_show_all(grid);
    return grid;
}
