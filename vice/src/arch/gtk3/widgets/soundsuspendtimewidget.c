/*
 * soundsuspendtimewidget.c - GTK3 sound suspend time widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SoundSuspendTime - time (seconds) to suspend sound playback to allow the
 *                     emulation to catch up
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

#include "soundsuspendtimewidget.h"


/** \brief  Minimum value for the buffer size spin button (sec)
 */
#define SPIN_MIN    0.0

/** \brief  Maximum value for the buffer size spin button (sec)
 */
#define SPIN_MAX    10.0

/** \brief  Step size of the spin button (sec) when pushing +/-
 */
#define SPIN_STEP   1.0



/** \brief  Handler for the "changed" event of the spin button
 *
 * \param[in]   widget      spin button widget
 * \param[in[   user_data   data for the event (unused)
 */
static void on_suspend_time_changed(GtkWidget *widget, gpointer user_data)
{
    gdouble sec;

    sec = gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget));
    debug_gtk3("got %d sec\n", (int)sec);
    resources_set_int("SoundSuspendTime", (int)sec);
}



/** \brief  Create spin button to set the buffer size
 *
 * \return  spin button
 */
static GtkWidget *create_spinbutton(void)
{
    GtkWidget *spin;
    int sec;

    resources_get_int("SoundSuspendTime", &sec);

    spin = gtk_spin_button_new_with_range(SPIN_MIN, SPIN_MAX, SPIN_STEP);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)(sec));
    g_signal_connect(spin, "changed", G_CALLBACK(on_suspend_time_changed), NULL);
    return spin;
}


/** \brief  Create a widget to set the "SoundSuspendTime" resource
 *
 * \return  grid
 */
GtkWidget *sound_suspend_time_widget_create(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    g_object_set(grid, "margin", 8, NULL);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid),
            uihelpers_create_grid_label("Sound suspend time"),
            0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), create_spinbutton(), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("seconds"), 1, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
