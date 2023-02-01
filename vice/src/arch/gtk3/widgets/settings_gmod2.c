/** \file   settings_gmod2widget.c
 * \brief   Settings widget to control GMod2 resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES GMOD2EEPROMImage    x64 x64sc xscpu64 x128
 * $VICERES GMOD2EEPROMRW       x64 x64sc xscpu64 x128
 * $VICERES GMod2FlashWrite     x64 x64sc xscpu64 x128
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
#include "resources.h"
#include "vice_gtk3.h"

#include "settings_gmod2.h"


/** \brief  Text entry used for the EEPROM filename
 */
static GtkWidget *eeprom_entry;


/** \brief  Callback for the open-file dialog
 *
 * \param[in]   dialog      open-file dialog
 * \param[in]   filename    filename or NULL on cancel
 * \param[in]   data        extra data (unused)
 */
static void save_filename_callback(GtkDialog *dialog,
                                   gchar     *filename,
                                   gpointer   data)
{
    if (filename != NULL) {
        if (cartridge_save_image(CARTRIDGE_GMOD2, filename) < 0) {
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
    GtkWidget *dialog;

    dialog = vice_gtk3_save_file_dialog("Save cartridge image",
                                        NULL,
                                        TRUE,
                                        NULL,
                                        save_filename_callback,
                                        NULL);
    gtk_widget_show(dialog);
}

/** \brief  Handler for the "clicked" event of the Flush Image button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   unused
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    if (cartridge_flush_image(CARTRIDGE_GMOD2) < 0) {
        vice_gtk3_message_error("Flushing failed",
                                "Failed to fush cartridge image");
    }
}

/** \brief  Callback for the EEPROM file selection dialog
 *
 * \param[in,out]   dialog      file chooser dialog
 * \param[in,out]   filename    name of the EEPROM file
 * \param[in]       data        extra data (unused)
 */
static void eeprom_filename_callback(GtkDialog *dialog,
                                     gchar *filename,
                                     gpointer data)
{
    if (filename != NULL) {
        if (resources_set_string("GMOD2EEPROMImage", filename) < 0) {
            vice_gtk3_message_error("Failed to load EEPROM file",
                                    "Failed to load EEPROM image file '%s'",
                                    filename);
        } else {
            gtk_entry_set_text(GTK_ENTRY(eeprom_entry), filename);
        }
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}



/** \brief  Handler for the "clicked" event of the EEPROM browse button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry
 */
static void on_eeprom_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;

    dialog = vice_gtk3_open_file_dialog("Open EEMPROM image",
                                        NULL,
                                        NULL,
                                        NULL,
                                        eeprom_filename_callback,
                                        NULL);
    gtk_widget_show(dialog);
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
    GtkWidget *box;
    gboolean   can_save;
    gboolean   can_flush;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "GMod2 Cartridge image", 2);

    write_back = vice_gtk3_resource_check_button_new("GMod2FlashWrite",
                                                     "Save image when changed");
    gtk_widget_set_margin_start(write_back, 8);

    save_button = gtk_button_new_with_label("Save image as ...");
    g_signal_connect(save_button,
                     "clicked",
                     G_CALLBACK(on_save_clicked),
                     NULL);
    flush_button = gtk_button_new_with_label("Save image");
    g_signal_connect(flush_button,
                     "clicked",
                     G_CALLBACK(on_flush_clicked),
                     NULL);

    box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_halign(box, GTK_ALIGN_END);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_box_set_spacing(GTK_BOX(box), 8);
    gtk_box_pack_start(GTK_BOX(box), save_button,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), flush_button, FALSE, FALSE, 0);

    gtk_grid_attach(GTK_GRID(grid), write_back, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), box,        1, 1, 1, 1);

    can_save  = (gboolean)cartridge_can_save_image(CARTRIDGE_EASYFLASH);
    can_flush = (gboolean)cartridge_can_flush_image(CARTRIDGE_EASYFLASH);
    gtk_widget_set_sensitive(save_button,  can_save);
    gtk_widget_set_sensitive(flush_button, can_flush);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to control GMod2 EEPROM
 *
 * \return  GtkGrid
 */
static GtkWidget *create_eeprom_image_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *browse;
    GtkWidget *write_enable;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 8, "GMod2 EEPROM image", 1);

    label = gtk_label_new("EEPROM image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label, 8);

    eeprom_entry = vice_gtk3_resource_entry_full_new("GMOD2EEPROMImage");
    gtk_widget_set_hexpand(eeprom_entry, TRUE);

    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse,
                     "clicked",
                     G_CALLBACK(on_eeprom_browse_clicked),
                     NULL);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), eeprom_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    write_enable = vice_gtk3_resource_check_button_new("GMOD2EEPROMRW",
            "Enable writes to GMod2 EEPROM image");
    gtk_widget_set_margin_start(write_enable, 8);
    gtk_grid_attach(GTK_GRID(grid), write_enable, 0, 2, 3, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control GMOD2 resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *settings_gmod2_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = vice_gtk3_grid_new_spaced(8, 32);

    gtk_grid_attach(GTK_GRID(grid), create_cart_image_widget(),   0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_eeprom_image_widget(), 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
