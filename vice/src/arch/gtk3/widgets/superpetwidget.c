/** \file   superpetwidget.c
 * \brief   Widget to control various SuperPET related resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES SuperPET        xpet
 * $VICERES CPUswitch       xpet
 * $VICERES H6809RomAName   xpet
 * $VICERES H6809RomBName   xpet
 * $VICERES H6809RomCName   xpet
 * $VICERES H6809RomDName   xpet
 * $VICERES H6809RomEName   xpet
 * $VICERES H6809RomFName   xpet
 *
 * See the widgets/aciawidget.c file for additional resources.
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
#include "aciawidget.h"
#include "resources.h"
#include "ui.h"

#include "superpetwidget.h"


/** \brief  Number of SuperPET ROMs
 */
#define SUPERPET_ROM_COUNT ('F' - 'A' + 1)


/** \brief  SuperPET enable toggle button */
GtkWidget *superpet_enable_widget = NULL;

/** \brief  ACIA widget */
static GtkWidget *acia1_widget = NULL;

/** \brief  CPU widget */
static GtkWidget *cpu_widget = NULL;

/** \brief  ROM widget */
static GtkWidget *rom_widget = NULL;

/** \brief  References to the entries for ROMS 'A' to 'F' */
static GtkWidget *rom_entry_list[SUPERPET_ROM_COUNT];


/** \brief  List of baud rates for the ACIA widget
 */
static int baud_rates[] = { 300, 1200, 2400, 9600, 19200, -1 };

/** \brief  List of CPU types
 */
static const vice_gtk3_radiogroup_entry_t cpu_types[] = {
    { "MOS 6502", 0 },
    { "Motorola 6809", 1 },
    { "Programmable", 2 },
    { NULL, -1 },
};


/** \brief  Handler for the "changed" event of the ROM text boxes
 *
 * \param[in]   widget      text entry
 * \param[in]   user_data   ROM index ('A'-'F') (`int`)
 */
static void on_superpet_rom_changed(GtkWidget *widget, gpointer user_data)
{
    int rom = GPOINTER_TO_INT(user_data);
    const char *path = gtk_entry_get_text(GTK_ENTRY(widget));

    resources_set_string_sprintf("H6809Rom%cName", path, rom);
}

/** \brief  Callback for the SuperPet $A000-$F000 ROMS
 *
 * \param[in,out]   dialog      open-file dialog
 * \param[in]       filename    ROM filename
 * \param[in]       index       ROM "index"
 */
static void browse_superpet_rom_filename_callback(GtkDialog *dialog,
                                                  gchar     *filename,
                                                  gpointer   index)
{
    int rom_index = GPOINTER_TO_INT(index);

    if (filename != NULL) {
        GtkWidget *entry = rom_entry_list[rom_index];
        /* update text entry, forcing update of the related resource */
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

/** \brief  Handler for the "clicked" event of the ROM browse buttons
 *
 * \param[in]   widget      button
 * \param[in]   index       ROM index ('A'-'F') (`int`)
 */
static void on_superpet_rom_browse_clicked(GtkWidget *widget, gpointer index)
{
    int rom = GPOINTER_TO_INT(index);
    char title[256];

    g_snprintf(title, sizeof title, "Select $%cXXX ROM", rom);

    vice_gtk3_open_file_dialog(title,
                               NULL,
                               NULL,
                               NULL,
                               browse_superpet_rom_filename_callback,
                               GINT_TO_POINTER(rom - 'A'));
}

/** \brief  User-defined callback function
 *
 * Set with pet_superpet_widget_set_superpet_enable_callback()
 */
static void (*user_callback_enable)(int);


/** \brief  Callback for the check button widget
 *
 * \param[in]   widget   check button widget
 * \param[in]   enabled  enabled or not
 */
static void superpet_enable_callback(GtkWidget *widget, int enabled)
{
    if (user_callback_enable != NULL) {
        user_callback_enable(enabled);
    }
}


/** \brief  Create check button for the SuperPET resource
 *
 * Create a grid with a check button and some informative text under it.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_superpet_enable_widget(void)
{
    GtkWidget *grid;
    GtkWidget *check;
    GtkWidget *label;

    grid  = gtk_grid_new();
    check = vice_gtk3_resource_check_button_new("SuperPET", "I/O Enable");
    label = gtk_label_new("(disables x96)");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_margin_start(label, 8);
    gtk_grid_attach(GTK_GRID(grid), check, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_widget_show_all(grid);

    vice_gtk3_resource_check_button_add_callback(check, superpet_enable_callback);

    return grid;
}


/** \brief  Set function to trigger on SuperPET enable checkbox toggle
 *
 * \param[in]   func    callback function
 */
void pet_superpet_widget_set_superpet_enable_callback(void (*func)(int))
{
    user_callback_enable = func;
}


/** \brief  Synchronize \a widget with its resource
 *
 * \param[in,out]   widget  SuperPET I/O enabled widget
 */
void pet_superpet_enable_widget_sync(GtkWidget *widget)
{
    GtkWidget *check;

    check = gtk_grid_get_child_at(GTK_GRID(widget), 0, 0);
    vice_gtk3_resource_check_button_sync(check);
}

/** \brief  Create SuperPET CPU selection widget
 *
 * Select between 'MOS 6502, 'Motorola 6809' or 'programmable'
 *
 * \return  GtkGrid
 */
static GtkWidget *create_superpet_cpu_widget(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "CPU type", 1);
    vice_gtk3_grid_set_title_margin(grid, 8);

    group = vice_gtk3_resource_radiogroup_new("CPUswitch",
                                              cpu_types,
                                              GTK_ORIENTATION_VERTICAL);
    gtk_widget_set_margin_start(group, 8);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}

/** \brief  Create widget to select SuperCPU ROMs at $A000-$FFFF
 *
 * \return  GtkGrid
 */
static GtkWidget *create_superpet_rom_widget(void)
{
    GtkWidget *grid;
    int bank;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "6809 ROMs", 3);
    vice_gtk3_grid_set_title_margin(grid, 8);

    for (bank = 0; bank < SUPERPET_ROM_COUNT; bank++) {
        GtkWidget  *label;
        GtkWidget  *entry;
        GtkWidget  *browse;
        gchar       buffer[64];
        const char *path;

        /* assumes ASCII, should be safe, except for old IBM main frames */
        g_snprintf(buffer, sizeof buffer, "$%cxxx", bank + 'A');
        label = gtk_label_new(buffer);
        gtk_widget_set_margin_start(label, 8);

        entry = gtk_entry_new();
        gtk_widget_set_hexpand(entry, TRUE);
        resources_get_string_sprintf("H6809rom%cName", &path, bank + 'A');
        gtk_entry_set_text(GTK_ENTRY(entry), path);

        browse = gtk_button_new_with_label("Browse");

        gtk_grid_attach(GTK_GRID(grid), label, 0, bank + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), entry, 1, bank + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), browse, 2, bank + 1, 1, 1);

        /* hook up event handlers */
        g_signal_connect(entry,
                         "changed",
                         G_CALLBACK(on_superpet_rom_changed),
                         GINT_TO_POINTER(bank + 'A'));
        g_signal_connect(browse,
                         "clicked",
                         G_CALLBACK(on_superpet_rom_browse_clicked),
                         GINT_TO_POINTER(bank + 'A'));

        rom_entry_list[bank] = entry;
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create a SuperPET-specific widget to be used in the PET model widget
 *
 * \return  GtkGrid
 */
GtkWidget *superpet_widget_create(void)
{
    GtkWidget *grid;

    grid = vice_gtk3_grid_new_spaced(16, 0);

    acia1_widget = acia_widget_create(baud_rates);
    gtk_grid_attach(GTK_GRID(grid), acia1_widget, 0, 2, 2, 1);

    cpu_widget = create_superpet_cpu_widget();
    gtk_widget_set_margin_top(cpu_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), cpu_widget, 0, 3, 1, 1);

    superpet_enable_widget = create_superpet_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), superpet_enable_widget, 0, 4, 1, 1);

    rom_widget = create_superpet_rom_widget();
    gtk_widget_set_margin_top(rom_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), rom_widget, 1, 3, 1, 2);

    gtk_widget_show_all(grid);
    return grid;
}
