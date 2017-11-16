/** \file   src/arch/gtk3/widgets/petdwwwidget.c
 * \brief   PET RAM expansion module widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  PETDWW (xpet)
 *  PETDWWfilename (xpet)
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

#include "petdwwwidget.h"

/* TODO: DWW can only be enabled when I/O size is 2KB! */

/* references to widgets to be able to toggle sensitive state depending on the
 * DWW Enable check button
 */
static GtkWidget *entry = NULL;
static GtkWidget *browse = NULL;


/** \brief  Handler for the "toggled" event of the DWW Enable check button
 *
 * Toggles sensitive state of other widgets
 *
 * \param[in]   widget      DWW check button
 * \param[in]   user_data   unused
 */
static void on_dww_toggled(GtkWidget *widget, gpointer user_data)
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

    filename = ui_open_file_dialog(widget, "Open DWW image file", NULL,
            NULL, NULL);
    if (filename != NULL) {
        GtkEntry *entry = GTK_ENTRY(user_data);
        debug_gtk3("setting PETDWWfilename to '%s'\n", filename);
        gtk_entry_set_text(entry, filename);
        g_free(filename);
    }
}


/** \brief  Create widget to control PET DWW settings
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *pet_dww_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *enable;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    /* DWW enable */
    enable = resource_check_button_create("PETDWW",
            "Enable PET DWW");
    g_signal_connect(enable, "toggled", G_CALLBACK(on_dww_toggled), NULL);
    gtk_grid_attach(GTK_GRID(grid), enable, 0, 0, 3, 1);


    /* DWW filename */
    label = gtk_label_new("DWW image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    entry = resource_entry_create("PETDWWfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked),
            (gpointer)entry);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    /* set initial sensitive state of widgets */
    on_dww_toggled(enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}
