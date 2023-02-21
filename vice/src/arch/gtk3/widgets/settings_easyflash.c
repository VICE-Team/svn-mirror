/** \file   settings_easyflash.c
 * \brief   Settings widget to control Easy Flash resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES EasyFlashJumper         x64 x64sc xscpu64 x128
 * $VICERES EasyFlashWriteCRT       x64 x64sc xscpu64 x128
 * $VICERES EasyFlashOptimizeCRT    x64 x64sc xscpu64 x128
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

#include "cartridge.h"
#include "vice_gtk3.h"

#include "settings_easyflash.h"


/** \brief  Callback for the save-dialog
 *
 * \param[in,out]   dialog      save-file dialog
 * \param[in,out]   filename    path to file to save
 * \param[in]       data        extra data (unused)
 */
static void save_filename_callback(GtkDialog *dialog,
                                   gchar     *filename,
                                   gpointer   data)
{
    if (filename != NULL) {
        if (cartridge_save_image(CARTRIDGE_EASYFLASH, filename) < 0) {
            vice_gtk3_message_error(CARTRIDGE_NAME_EASYFLASH " Error",
                                    "Failed to save " CARTRIDGE_NAME_EASYFLASH
                                    " image as '%s'.",
                                    filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

/** \brief  Handler for the 'clicked' event of the "Save As" button
 *
 * \param[in]   widget      button (unused)
 * \param[in]   user_data   extra event data (unused)
 */
static void on_save_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;

    dialog = vice_gtk3_save_file_dialog("Save " CARTRIDGE_NAME_EASYFLASH
                                        " image as",
                                        NULL,
                                        TRUE,
                                        NULL,
                                        save_filename_callback,
                                        NULL);
    gtk_widget_show_all(dialog);
}

/** \brief  Handler for the 'clicked' event of the "Flush now" button
 *
 * \param[in]   widget      button (unused)
 * \param[in]   user_data   extra event data (unused)
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    if (cartridge_flush_image(CARTRIDGE_EASYFLASH) < 0) {
        vice_gtk3_message_error(CARTRIDGE_NAME_EASYFLASH "Error",
                                "Failed to flush the "
                                CARTRIDGE_NAME_EASYFLASH " image.");
    }
}


/** \brief  Create Easy Flash widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_easyflash_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *jumper;
    GtkWidget *jumper_label;
    GtkWidget *write_crt;
    GtkWidget *optimize_crt;
    GtkWidget *save_button;
    GtkWidget *flush_button;
    GtkWidget *button_box;
    gboolean   can_save;
    gboolean   can_flush;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);

    jumper = vice_gtk3_resource_switch_new("EasyFlashJumper");
    gtk_widget_set_hexpand(jumper, TRUE);
    gtk_widget_set_halign(jumper, GTK_ALIGN_START);
    jumper_label = gtk_label_new(CARTRIDGE_NAME_EASYFLASH " jumper");
    gtk_widget_set_halign(jumper_label, GTK_ALIGN_START);

    write_crt = vice_gtk3_resource_check_button_new("EasyFlashWriteCRT",
                                                    "Save image when changed");
    gtk_widget_set_margin_top(write_crt, 8);
    optimize_crt = vice_gtk3_resource_check_button_new("EasyFlashOptimizeCRT",
                                                       "Optimize image when saving");

    /* Save image as... */
    save_button = gtk_button_new_with_label("Save image as ..");
    g_signal_connect(G_OBJECT(save_button),
                     "clicked",
                     G_CALLBACK(on_save_clicked),
                     NULL);

    /* Flush image now */
    flush_button = gtk_button_new_with_label("Flush image");
    g_signal_connect(G_OBJECT(flush_button),
                     "clicked",
                     G_CALLBACK(on_flush_clicked),
                     NULL);

    /* pack buttons in a button box for homogeneous sizes */
    button_box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_set_spacing(GTK_BOX(button_box), 8);
    gtk_widget_set_margin_top(button_box, 16);
    gtk_box_pack_start(GTK_BOX(button_box), save_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_box), flush_button, FALSE, FALSE, 0);
    gtk_widget_set_hexpand(button_box, TRUE);
    gtk_widget_set_halign(button_box, GTK_ALIGN_END);

    gtk_grid_attach(GTK_GRID(grid), jumper_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), jumper,       1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), write_crt,    0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), optimize_crt, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), button_box,   0, 3, 2, 1);

    can_save  = (gboolean)cartridge_can_save_image(CARTRIDGE_EASYFLASH);
    can_flush = (gboolean)cartridge_can_flush_image(CARTRIDGE_EASYFLASH);
    gtk_widget_set_sensitive(save_button,  can_save);
    gtk_widget_set_sensitive(flush_button, can_flush);

    gtk_widget_show_all(grid);
    return grid;
}
