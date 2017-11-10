/** \file   src/arch/gtk3/widgets/ieee488widget.c
 * \brief   IEEE-488 adapter widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  IEEE488
 *  IEEE488Image
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

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "openfiledialog.h"

#include "ieee488widget.h"


static GtkWidget *entry = NULL;
static GtkWidget *browse = NULL;



static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(entry, state);
    gtk_widget_set_sensitive(browse, state);
}


/** \brief  Handler for the "clicked" event of the browse button
 *
 * Activates a file-open dialog and stores the file name in the GtkEntry passed
 * in \a user_data if valid, triggering a resource update.
 *
 * \param[in]   widget      button
 * \param[in]   user_data   entry to store filename in
 */
static void on_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(widget, "Open IEEE-488 image", NULL, NULL,
            NULL);
    if (filename != NULL) {
        GtkEntry *entry = GTK_ENTRY(user_data);
        debug_gtk3("setting IEEE488Image to '%s'\n", filename);
        gtk_entry_set_text(entry, filename);
        g_free(filename);
    }
}


/** \brief  Create widget to control IEEE-488 adapter
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *ieee488_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *enable;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    enable = resource_check_button_create("IEEE488",
            "Enable IEEE-488 interface");
    gtk_grid_attach(GTK_GRID(grid), enable, 0, 0, 3, 1);

    label = gtk_label_new("IEEE-488 ROM");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    entry = resource_entry_create("IEEE488Image");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked),
            (gpointer)entry);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    g_signal_connect(enable, "toggled", G_CALLBACK(on_enable_toggled), NULL);

    /*hack*/
    on_enable_toggled(enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}
