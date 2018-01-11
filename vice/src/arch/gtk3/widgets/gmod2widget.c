/** \file   src/arch/gtk3/widgets/gmod2widget.c
 * \brief   Widget to control GMod2 resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  GMOD2EEPROMImage (x64/x64sc/xscpu64/x128)
 *  GMOD2EEPROMRW (x64/x64sc/xscpu64/x128)
 *  GMod2FlashWrite (x64/x64sc/xscpu64/x128)
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
#include "savefiledialog.h"
#include "cartridge.h"
#include "cartimagewidget.h"
#include "carthelpers.h"

#include "gmod2widget.h"

#if 0
static int (*gmod2_save_func)(int, const char *) = NULL;
static int (*gmod2_flush_func)(int) = NULL;
#endif


/** \brief  Handler for the "clicked" event of the Save Image button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   unused
 */
static void on_save_clicked(GtkWidget *widget, gpointer user_data)
{
    /* TODO: retrieve filename of cart image */
    gchar *filename = ui_save_file_dialog(widget, "Save Cartridge image",
            NULL, TRUE, NULL);
    if (filename != NULL) {
        debug_gtk3("saving GMod2 cart image as '%s'\n", filename);
        if (carthelpers_save_func(CARTRIDGE_GMOD2, filename) < 0) {
            ui_message_error(widget, "Saving failed",
                    "Failed to save cartridge image '%s'",
                    filename);
        }
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the Flush Image button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   unused
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    if (carthelpers_flush_func(CARTRIDGE_GMOD2) < 0) {
        debug_gtk3("Flusing GMod2 cart image\n");
        ui_message_error(widget, "Flushing failed",
                    "Failed to fush cartridge image");
    }
}


/** \brief  Handler for the "clicked" event of the EEPROM browse button
 *
 * \param[in]   widget      button
 * \param[in]   user_data   text entry
 */
static void on_eeprom_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename = ui_open_file_dialog(widget, "Open EEMPROM image",
                NULL, NULL, NULL);

    if (filename != NULL) {
        debug_gtk3("Loading GMod2 EEPROM image '%s'\n", filename);
        if (resources_set_string("GMOD2EEPROMImage", filename) < 0) {
            ui_message_error(widget, "Failed to load EEPROM file",
                    "Failed load EEPROM image file '%s'",
                    filename);
        } else {
            gtk_entry_set_text(GTK_ENTRY(user_data), filename);
        }
        g_free(filename);
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

    grid = uihelpers_create_grid_with_label("GMod2 Cartridge image", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    write_back = vice_gtk3_resource_check_button_create("GMod2FlashWrite",
                "Save image when changed");
    g_object_set(write_back, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), write_back, 0, 1, 1, 1);

    save_button = gtk_button_new_with_label("Save image as ...");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked),
            NULL);
    gtk_grid_attach(GTK_GRID(grid), save_button, 1, 1, 1, 1);

    flush_button = gtk_button_new_with_label("Flush image now");
    g_signal_connect(flush_button, "clicked", G_CALLBACK(on_flush_clicked),
            NULL);
    gtk_grid_attach(GTK_GRID(grid), flush_button, 2, 1, 1, 1);

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
    GtkWidget *entry;
    GtkWidget *browse;
    GtkWidget *write_enable;

    grid = uihelpers_create_grid_with_label("GMod2 EEPROM image", 1);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("EEPROM image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);

    entry = resource_entry_full_create("GMOD2EEPROMImage");
    gtk_widget_set_hexpand(entry, TRUE);

    browse = gtk_button_new_with_label("Browse ...");
    g_signal_connect(browse, "clicked", G_CALLBACK(on_eeprom_browse_clicked),
            (gpointer)entry);

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    write_enable = vice_gtk3_resource_check_button_create("GMOD2EEPROMRW",
            "Enable writes to GMod2 EEPROM image");
    g_object_set(write_enable, "margin-left", 16, NULL);
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
GtkWidget *gmod2_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), create_cart_image_widget(), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_eeprom_image_widget(), 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
