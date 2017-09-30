/*
 * soundbuffersizewidget.c - GTK3 sound buffer size widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SoundBufferSize - sound buffer size in milliseconds (int)
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
#include "sound.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"

#include "soundbuffersizewidget.h"


/** \brief  Minimum value for the buffer size spin button (msec)
 */
#define SPIN_MIN    25.0

/** \brief  Maximum value for the buffer size spin button (msec)
 */
#define SPIN_MAX    350.0

/** \brief  Step size of the spin button (msec) when pushing +/-
 */
#define SPIN_STEP   25.0



/** \brief  Handler for the "changed" event of the spin button
 *
 * \param[in]   widget      spin button widget
 * \param[in[   user_data   data for the event (unused)
 */
static void on_buffer_size_changed(GtkWidget *widget, gpointer user_data)
{
    gdouble msec;

    msec = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    debug_gtk3("got %d msec\n", (int)msec);
    resources_set_int("SoundBufferSize", (int)msec);
}



/** \brief  Create spin button to set the buffer size
 *
 * \return  spin button
 */
static GtkWidget *create_spinbutton(void)
{
    GtkWidget *spin;
    int msec;

    resources_get_int("SoundBufferSize", &msec);

    spin = gtk_spin_button_new_with_range(SPIN_MIN, SPIN_MAX, SPIN_STEP);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)(msec));
    g_signal_connect(spin, "changed", G_CALLBACK(on_buffer_size_changed), NULL);
    return spin;
}


/** \brief  Create a widget to set the "SoundBufferSize" resource
 *
 * \return  grid
 */
GtkWidget *sound_buffer_size_widget_create(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    g_object_set(grid, "margin", 8, NULL);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid),
            uihelpers_create_grid_label("Sound buffer size"),
            0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), create_spinbutton(), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("milliseconds"), 1, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


