/** \file   src/arch/gtk3/widgets/mmc64widget.c
 * \brief   Widget to control MMC64 resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  MMC64CardImage (x64/x64sc/xscpu64/x128)
 *  MMC64EEPROMIMage (x64/x64sc/xscpu64/x128)
 *  MMC64EEPROMRW (x64/x64sc/xscpu64/x128)
 *  MMC64RescueMode (x64/x64sc/xscpu64/x128)
 *  MMC64ImageWrite (x64/x64sc/xscpu64/x128)
 *  MMC64CardRW (x64/x64sc/xscpu64/x128)
 *  MMC64SDType (x64/x64sc/xscpu64/x128)
 *  MMC64ClockPort (x64/x64sc/xscpu64/x128)
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
#include "cartimagewidget.h"
#include "cartridge.h"

#include "mmc64widget.h"


static ui_radiogroup_entry_t revisions[] = {
    { "Rev. A", 0 },
    { "Rev. B", 1 },
    { NULL, -1 }
};


/** \brief  List of memory card types
 */
static const ui_radiogroup_entry_t card_types[] = {
    { "Auto", 0 },
    { "MMC", 1 },
    { "SD", 2 },
    { "SDHC", 3 },
    { NULL, -1 }
};


static int (*eeprom_save_func)(int, const char *) = NULL;
static int (*eeprom_flush_func)(int) = NULL;

static GtkWidget *jumper_widget = NULL;
static GtkWidget *revision_widget = NULL;
static GtkWidget *clockport_widget = NULL;
static GtkWidget *eeprom_widget = NULL;
static GtkWidget *card_widget = NULL;
static GtkWidget *card_type_widget = NULL;


/** \brief  Handler for the "clicked" event of the memory card browse button
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_card_browse_clicked(GtkWidget *button, gpointer user_data)
{
    char *filename;

    filename = ui_open_file_dialog(button, "Open memory card file",
            NULL, NULL, NULL);
    if (filename != NULL) {
        GtkWidget *parent;
        GtkWidget *entry;

        parent = gtk_widget_get_parent(button);
        entry= gtk_grid_get_child_at(GTK_GRID(parent), 1, 1);
        /* trigger resource update */
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


static void on_enable_toggled(GtkWidget *check, gpointer user_data)
{
    int state;

    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));

    gtk_widget_set_sensitive(jumper_widget, state);
    gtk_widget_set_sensitive(revision_widget, state);
    gtk_widget_set_sensitive(clockport_widget, state);
    gtk_widget_set_sensitive(eeprom_widget, state);
    gtk_widget_set_sensitive(card_widget, state);
    gtk_widget_set_sensitive(card_type_widget, state);
}


static GtkWidget *create_mmc64_enable_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("MMC64", "Enable MMC64");
    g_signal_connect(check, "toggled", G_CALLBACK(on_enable_toggled), NULL);
    return check;
}


static GtkWidget *create_mmc64_jumper_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("MMC64_flashjumper",
            "Enable flash jumper");
    return check;
}


static GtkWidget *create_mmc64_revision_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *radio_group;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);

    label = gtk_label_new("Revision");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    radio_group = resource_radiogroup_create("MMC64_revision", revisions,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control EEPROM resources
 *
 * \param[in]   parent  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_eeprom_image_widget(GtkWidget *parent)
{
    return cart_image_widget_create(parent, "MMC64 EEPROM image",
            "MMC64BIOSfilename", "MMC64_bios_write",
            eeprom_save_func, eeprom_flush_func,
            CARTRIDGE_NAME_MMC64, CARTRIDGE_MMC64);
}


/** \brief  Create widget to control memory card image
 *
 * \param[in]   widget  parent widget (used for dialogs
 *
 * \return  GtkGrid
 */
static GtkWidget *create_card_image_widget(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;
    GtkWidget *card_writes;

    grid = uihelpers_create_grid_with_label("MMC64 SD/MMC Card image", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("file name");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    entry = resource_entry_create("MMC64Imagefilename");
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);

    browse = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    card_writes = resource_check_button_create("MMC64_RO",
            "Enable SD/MMC card read-only");
    g_object_set(card_writes, "margin-left", 16, "margin-top", 8, NULL);
    gtk_grid_attach(GTK_GRID(grid), card_writes, 0, 2, 3, 1);

    g_signal_connect(browse, "clicked", G_CALLBACK(on_card_browse_clicked),
            NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control memory card type
 *
 * \return  GtkGrid
 */
static GtkWidget *create_card_type_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    label = gtk_label_new("Card type");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    radio_group = resource_radiogroup_create("MMC64_sd_type", card_types,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(radio_group), 16);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


static GtkWidget *create_clockport_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("ClockPort device");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid),
            clockport_device_widget_create("MMC64ClockPort"), 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Create widget to control MMC Replay resources
 *
 * \param[in]   parent  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *mmc64_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *enable_widget;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    enable_widget = create_mmc64_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), enable_widget, 0, 0, 1, 1);

    jumper_widget = create_mmc64_jumper_widget();
    g_object_set(jumper_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), jumper_widget, 0, 1, 1, 1);

    revision_widget = create_mmc64_revision_widget();
    gtk_grid_attach(GTK_GRID(grid), revision_widget, 1, 1, 1, 1);

    clockport_widget = create_clockport_widget();
    gtk_grid_attach(GTK_GRID(grid), clockport_widget, 0, 2, 1, 1);

    eeprom_widget = create_eeprom_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), eeprom_widget, 0, 3, 3, 1);
    card_widget = create_card_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), card_widget, 0, 4, 3, 1);

    card_type_widget = create_card_type_widget();
    gtk_grid_attach(GTK_GRID(grid), card_type_widget, 0, 5, 3, 1);

    /* enable/disable widgets based on MMC64, dirty hack */
    on_enable_toggled(enable_widget, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


void mmc64_widget_set_eeprom_save_func(int (*func)(int, const char *))
{
    eeprom_save_func = func;
}


void mmc64_widget_set_eeprom_flush_func(int (*func)(int))
{
    eeprom_flush_func = func;
}
