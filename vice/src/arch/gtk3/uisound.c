/*
 * uisound.c - GTK3 sound settings central widget for the settings dialog
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

#include "sounddriverwidget.h"

#include "uisound.h"


/** \brief  Handler for the "toggled" event of the 'sound enabled checkbox
 *
 * Sets the "Sound" resource and toggles the 'sensitive' state of the the inner
 * grid containing widgets
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   reference to the inner grid
 */
static void on_sound_enabled_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    resources_set_int("Sound", state);
    gtk_widget_set_sensitive(GTK_WIDGET(user_data), state);
}


/** \brief  Create the 'inner' grid, the one containing all the widgets
 *
 * \return  grid
 */
static GtkWidget *create_inner_grid(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();

    gtk_grid_attach(GTK_GRID(grid), create_sound_driver_widget(), 0, 0, 1, 1);

    return grid;
}



/** \brief  Create sound settings widget for use as a 'central' settings widget
 *
 * \param[in]   widget  parent widget
 *
 * \return  grid with sound settings widgets
 */
GtkWidget *uisound_create_central_widget(GtkWidget *widget)
{
    GtkWidget *outer;
    GtkWidget *inner;
    GtkWidget *enabled_check;
    int enabled_state;

    debug_gtk3("called\n");

    resources_get_int("Sound", &enabled_state);

    /* outer grid: contains the checkbox and an 'inner' grid for the widgets */
    outer = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(outer), 8);

    /* add checkbox for 'sound enabled' */
    enabled_check = gtk_check_button_new_with_label("Enable sound playback");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(enabled_check), enabled_state);
    gtk_grid_attach(GTK_GRID(outer), enabled_check, 0, 0, 1, 1);

    /* inner grid: contains widgets and can be enabled/disabled depending on
     * the state of the 'sound enabled' checkbox */
    inner = create_inner_grid();
    gtk_grid_set_column_spacing(GTK_GRID(inner), 8);
    g_object_set(inner, "margin", 8, NULL);
    gtk_widget_set_sensitive(inner, enabled_state); /* set enabled state */

    /* connect signal handler for the 'sound enabled' checkbox */
    g_signal_connect(enabled_check, "toggled",
            G_CALLBACK(on_sound_enabled_toggled), (gpointer)(inner));
    gtk_grid_attach(GTK_GRID(outer), inner, 0, 1, 1, 1);

    gtk_widget_show_all(outer);
    return outer;
}
