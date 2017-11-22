/** \file   src/arch/gtk3/uidebug.c
 * \brief   Debug menu dialogs
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  AutoPlaybackFrames
 *  TraceMode
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "basedialogs.h"
#include "basewidgets.h"
#include "debug_gtk3.h"
#include "drive.h"
#include "machine.h"
#include "resources.h"
#include "ui.h"
#include "vsync.h"
#include "widgethelpers.h"

#include "uidebug.h"


static ui_radiogroup_entry_t trace_modes[] = {
    { "Normal", DEBUG_NORMAL },
    { "Small", DEBUG_SMALL },
    { "History", DEBUG_HISTORY },
    { "Autoplay", DEBUG_AUTOPLAY },
    { NULL, -1 }
};


static GtkWidget *create_playback_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *spin;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    label = gtk_label_new("Auto playback frames");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    spin = resource_spin_button_int_create("AutoPlaybackFrames",
            0, 65536, 10);
    gtk_grid_attach(GTK_GRID(grid), spin, 1, 0, 1,1);

    gtk_widget_show_all(grid);
    return grid;
}


static GtkWidget *create_trace_widget(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    grid = uihelpers_create_grid_with_label("Select CPU/Drive trace mode", 1);
    group = resource_radiogroup_create("TraceMode", trace_modes,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



static GtkWidget *create_trace_mode_dialog(GtkWindow *parent)
{
    GtkWidget *dialog;
    GtkWidget *content;

    dialog = gtk_dialog_new_with_buttons("Select trace mode",
            parent, GTK_DIALOG_MODAL,
            "Close", GTK_RESPONSE_CLOSE,
            NULL, NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_trace_widget());

    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    return dialog;
}


static GtkWidget *create_playback_frames_dialog(GtkWindow *parent)
{
    GtkWidget *dialog;
    GtkWidget *content;

    dialog = gtk_dialog_new_with_buttons("Set auto playback frames",
            parent, GTK_DIALOG_MODAL,
            "Close", GTK_RESPONSE_CLOSE,
            NULL, NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_playback_widget());


    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
    return dialog;
}


void uidebug_trace_mode_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    int response;

    dialog = create_trace_mode_dialog(ui_get_active_window());
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    debug_gtk3("Got response ID %d\n", response);
    gtk_widget_destroy(dialog);
}


void uidebug_playback_frames_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    int response;

    dialog = create_playback_frames_dialog(ui_get_active_window());
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    debug_gtk3("Got response ID %d\n", response);
    gtk_widget_destroy(dialog);
}
