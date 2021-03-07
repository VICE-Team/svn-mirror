/** \file   gmod3widget.c
 * \brief   Widget to control GMod3 resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES GMod3FlashWrite     x64 x64sc xscpu64 x128
 */

/*
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

#include "vice_gtk3.h"
#include "cartridge.h"
#include "machine.h"
#include "resources.h"

#include "gmod3widget.h"


/** \brief  Callback for the save-dialog response handler
 *
 * \param[in,out]   dialog      save-file dialog
 * \param[in,out]   filename    filename
 * \param[in]       data        extra data (unused)
 */
static void save_filename_callback(GtkDialog *dialog,
                                  gchar *filename,
                                  gpointer data)
{
    if (filename != NULL) {
        if (carthelpers_save_func(CARTRIDGE_GMOD3, filename) < 0) {
            vice_gtk3_message_error("Saving failed",
                    "Failed to save cartridge image '%s'",
                    filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Handler for the "clicked" event of the Save Image button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   unused
 */
static void on_save_clicked(GtkWidget *widget, gpointer user_data)
{
    /* TODO: retrieve filename of cart image */
    vice_gtk3_save_file_dialog("Save cartridge image",
                               NULL, TRUE, NULL,
                               save_filename_callback,
                               NULL);
}


/** \brief  Handler for the "clicked" event of the Flush Image button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   unused
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    if (carthelpers_flush_func(CARTRIDGE_GMOD3) < 0) {
        vice_gtk3_message_error("Flushing failed",
                    "Failed to fush cartridge image");
    }
}


/** \brief  Create widget to handle Cartridge image resources and save/flush
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cart_image_widget(void)
{
    GtkWidget *grid;
    GtkWidget *write_back;
    GtkWidget *save_button;
    GtkWidget *flush_button;

    grid = vice_gtk3_grid_new_spaced_with_label(
            -1, -1, "GMod3 Cartridge image", 3);

    write_back = vice_gtk3_resource_check_button_new("GMod3FlashWrite",
                "Safe image when changed");
    g_object_set(write_back, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), write_back, 0, 1, 1, 1);

    save_button = gtk_button_new_with_label("Safe image as ...");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked),
            NULL);
    gtk_grid_attach(GTK_GRID(grid), save_button, 1, 1, 1, 1);
    gtk_widget_set_sensitive(save_button,
            (gboolean)(carthelpers_can_save_func(CARTRIDGE_GMOD3)));

    flush_button = gtk_button_new_with_label("Flush image now");
    g_signal_connect(flush_button, "clicked", G_CALLBACK(on_flush_clicked),
            NULL);
    gtk_widget_set_sensitive(flush_button,
            (gboolean)(carthelpers_can_flush_func(CARTRIDGE_GMOD3)));
    gtk_grid_attach(GTK_GRID(grid), flush_button, 2, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control GMOD3 resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *gmod3_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), create_cart_image_widget(), 0, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
