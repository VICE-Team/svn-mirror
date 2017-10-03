/** \file   src/arch/gtk3/widgets/superpetwidget.c
 * \brief   Widget to control various SuperPET related resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  SuperPET - Enable/disable SuperPET I/O and 6809 CPU
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"

#include "aciawidget.h"

#include "superpetwidget.h"


static GtkWidget *superpet_enable_widget = NULL;
static GtkWidget *acia1_widget = NULL;
static GtkWidget *cpu_widget = NULL;
static GtkWidget *rom_widget = NULL;
static GtkWidget *ram_9xxx_widget = NULL;
static GtkWidget *ram_axxx_widget = NULL;


static int baud_rates[] = { 300, 1200, 2400, 9600, 19200, -1 };

static ui_text_int_pair_t cpu_types[] = {
    { "MOS 6502", 0 },
    { "Motorola 6809", 1 },
    { "Programmable", 2 },
    { NULL, -1 },
};


/** \brief  Handler for the "toggled" event of the SuperPET check button
 *
 * \param[in]   widget      SuperPET check button
 * \param[in]   user_data   extra data (unused)
 */
static void on_superpet_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("SuperPET", &old_val);
    new_val = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    if (old_val != new_val) {
        debug_gtk3("setting SuperPET to %s\n", new_val ? "ON" : "OFF");
        resources_set_int("SuperPET", new_val);
    }
}


static void on_superpet_cpu_type_changed(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("CPUswitch", &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))
            && new_val != old_val) {
        debug_gtk3("setting CPUswitch to %d\n", new_val);
        resources_set_int("CPUswitch", new_val);
    }
}


/** \brief  Create check button for the SuperPET resource
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_superpet_enable_widget(void)
{
    GtkWidget *check;
    int enabled;

    check = gtk_check_button_new_with_label("I/O Enable (disables 8x96");
    resources_get_int("SuperPet", &enabled);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), enabled);
    g_signal_connect(check, "toggled", G_CALLBACK(on_superpet_enable_toggled),
            NULL);
    gtk_widget_show(check);
    return check;
}


static GtkWidget *create_superpet_cpu_widget(void)
{
    GtkWidget *grid;

    int cpu;

    resources_get_int("CPUswitch", &cpu);

    grid = uihelpers_create_int_radiogroup_with_label("CPU type",
            cpu_types,
            on_superpet_cpu_type_changed,
            cpu);

    gtk_widget_show_all(grid);
    return grid;
}


static GtkWidget *create_superpet_rom_widget(void)
{
    GtkWidget *grid;

    grid = uihelpers_create_grid_with_label("6809 ROMs", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    for (int bank = 0; bank < 6; bank++) {

        GtkWidget *label;
        GtkWidget *entry;
        GtkWidget *browse;
        gchar buffer[64];
        const char *path;

        /* XXX: assumes ASCII, which should be safe, except for old IBM
         *      main frames */
        g_snprintf(buffer, 64, "$%cxxx", bank + 'A');
        label = gtk_label_new(buffer);
        g_object_set(label, "margin-left", 16, NULL);

        entry = gtk_entry_new();
        gtk_widget_set_hexpand(entry, TRUE);
        resources_get_string_sprintf("H6809rom%cName", &path, bank + 'A');
        gtk_entry_set_text(GTK_ENTRY(entry), path);

        browse = gtk_button_new_with_label("Browse ...");

        gtk_grid_attach(GTK_GRID(grid), label, 0, bank + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), entry, 1, bank + 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), browse, 2, bank + 1, 1, 1);

        /* TODO: hook up event handlers */
    }

    gtk_widget_show_all(grid);
    return grid;
}



static void on_ram_9xxx_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("Ram9", &old_val);
    new_val = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    if (new_val != old_val) {
        debug_gtk3("setting Ram9 to %s\n", new_val ? "ON" : "OFF");
        resources_set_int("Ram9", new_val);
    }
}


static void on_ram_axxx_toggled(GtkWidget *widget, gpointer user_data)
{
    int old_val;
    int new_val;

    resources_get_int("Ram9", &old_val);
    new_val = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    if (new_val != old_val) {
        debug_gtk3("setting RamA to %s\n", new_val ? "ON" : "OFF");
        resources_set_int("RamA", new_val);
    }
}


static GtkWidget *create_superpet_9xxx_ram_widget(void)
{
    GtkWidget *check;
    int enabled;

    resources_get_int("Ram9", &enabled);
    check = gtk_check_button_new_with_label("$9xxx as RAM");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), enabled);
    g_signal_connect(check, "toggled", G_CALLBACK(on_ram_9xxx_toggled), NULL);
    gtk_widget_show(check);
    return check;
}


static GtkWidget *create_superpet_axxx_ram_widget(void)
{
    GtkWidget *check;
    int enabled;

    resources_get_int("Ram9", &enabled);
    check = gtk_check_button_new_with_label("$Axxx as RAM");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), enabled);
    g_signal_connect(check, "toggled", G_CALLBACK(on_ram_axxx_toggled), NULL);
    gtk_widget_show(check);
    return check;
}


/** \brief  Create a SuperPET-specific widget to be used in the PET model widget
 *
 * \return  GtkGrid
 */
GtkWidget *superpet_widget_create(void)
{
    GtkWidget *grid;

    grid = uihelpers_create_grid_with_label("SuperPET model settings", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    superpet_enable_widget = create_superpet_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), superpet_enable_widget, 0, 1, 3, 1);

    acia1_widget = acia_widget_create(baud_rates);
    gtk_grid_attach(GTK_GRID(grid), acia1_widget, 0, 2, 3, 1);

    cpu_widget = create_superpet_cpu_widget();
    gtk_grid_attach(GTK_GRID(grid), cpu_widget, 0, 3, 1, 3);

    rom_widget = create_superpet_rom_widget();
    gtk_grid_attach(GTK_GRID(grid), rom_widget, 1 ,3 , 2, 3);

    ram_9xxx_widget = create_superpet_9xxx_ram_widget();
    g_object_set(ram_9xxx_widget, "margin-left", 8, NULL);
    gtk_widget_set_vexpand(ram_9xxx_widget, TRUE);
    gtk_widget_set_valign(ram_9xxx_widget, GTK_ALIGN_END);
    ram_axxx_widget = create_superpet_axxx_ram_widget();
    g_object_set(ram_axxx_widget, "margin-left", 8, NULL);
    gtk_widget_set_valign(ram_axxx_widget, GTK_ALIGN_END);

    gtk_grid_attach(GTK_GRID(grid), ram_9xxx_widget, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ram_axxx_widget, 0, 5, 1, 1);



    gtk_widget_show_all(grid);
    return grid;
}

