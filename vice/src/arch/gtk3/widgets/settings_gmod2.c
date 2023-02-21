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

    dialog = vice_gtk3_save_file_dialog("Save " CARTRIDGE_NAME_GMOD2 " cartridge image",
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
        vice_gtk3_message_error(CARTRIDGE_NAME_GMOD2 "Error",
                                "Failed to flush " CARTRIDGE_NAME_GMOD2
                                " cartridge image.");
    }
}

/** \brief  Create left-aligned label with Pango markup
 *
 * \param[in]   text    label text (uses Pango markup)
 *
 * \return  GtkLabel
 */
static GtkWidget *label_helper(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}


/** \brief  Create widget to handle Cartridge image resources and save/flush
 *
 * \return  GtkGrid
 */
static GtkWidget *create_cart_image_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *write_back;
    GtkWidget *save_button;
    GtkWidget *flush_button;
    GtkWidget *box;
    gboolean   can_save;
    gboolean   can_flush;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>" CARTRIDGE_NAME_GMOD2 " cartridge image</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 2, 1);
    row++;

    write_back = vice_gtk3_resource_check_button_new("GMod2FlashWrite",
                                                     "Save image when changed");

    save_button = gtk_button_new_with_label("Save image as ..");
    g_signal_connect(save_button,
                     "clicked",
                     G_CALLBACK(on_save_clicked),
                     NULL);
    flush_button = gtk_button_new_with_label("Flush image");
    g_signal_connect(flush_button,
                     "clicked",
                     G_CALLBACK(on_flush_clicked),
                     NULL);

    box = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_halign(box, GTK_ALIGN_END);
    gtk_widget_set_hexpand(box, TRUE);
    gtk_box_set_spacing(GTK_BOX(box), 8);
    gtk_box_pack_start(GTK_BOX(box), save_button,  FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), flush_button, FALSE, FALSE, 0);

    gtk_grid_attach(GTK_GRID(grid), write_back, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), box,        1, row, 1, 1);

    can_save  = (gboolean)cartridge_can_save_image(CARTRIDGE_GMOD2);
    can_flush = (gboolean)cartridge_can_flush_image(CARTRIDGE_GMOD2);
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
    GtkWidget *eeprom;
    GtkWidget *write_enable;
    int        row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = label_helper("<b>" CARTRIDGE_NAME_GMOD2 " EEPROM image</b>");
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 2, 1);
    row++;

    label = label_helper("EEPROM image file");
    eeprom = vice_gtk3_resource_filechooser_new("GMOD2EEPROMImage",
                                                GTK_FILE_CHOOSER_ACTION_SAVE);
    vice_gtk3_resource_filechooser_set_custom_title(eeprom,
                                                    "Select or create "
                                                    CARTRIDGE_NAME_GMOD2
                                                    " EEPROM image file");
    gtk_widget_set_hexpand(eeprom, TRUE);
    gtk_grid_attach(GTK_GRID(grid), label,  0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), eeprom, 1, row, 1, 1);
    row++;

    write_enable = vice_gtk3_resource_check_button_new("GMOD2EEPROMRW",
            "Enable writes to " CARTRIDGE_NAME_GMOD2 " EEPROM image");
    gtk_grid_attach(GTK_GRID(grid), write_enable, 0, row, 2, 1);

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

    gtk_grid_attach(GTK_GRID(grid), create_eeprom_image_widget(), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_cart_image_widget(),   0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
