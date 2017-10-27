/** \file   src/arch/gtk3/widgets/c64memoryexpansionhackswidget.c
 * \brief   Widget to control C64 memory expansion hacks
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  MemoryHack (x64, x6sc)
 *  C64_256Kbase (x64, x64sc)
 *  C64_256Kfilename (x64, x64sc)
 *  PLUS60Kbase (x64, x64sc)
 *  PLUS60Kfilename (x64, x64sc)
 *  PLUS256Kfilename (x64, x64sc)
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

#include "basewidgets.h"
#include "widgethelpers.h"
#include "openfiledialog.h"
#include "c64-memory-hacks.h"

#include "c64memoryexpansionhackswidget.h"


/** \brief  List of C64 memory hack devices
 */
static ui_radiogroup_entry_t mem_hack_devices[] = {
    { "None",       MEMORY_HACK_NONE },
    { "C64 256K",   MEMORY_HACK_C64_256K },
    { "+60K",       MEMORY_HACK_PLUS60K },
    { "+256K",      MEMORY_HACK_PLUS256K },
    { NULL, -1 }
};

/**\brief   List of I/O base addresses for the C64_256K memory hack
 */
static ui_radiogroup_entry_t c64_256k_base_addresses[] = {
    { "$DE00-$DE7F", 0xde00 },
    { "$DE80-$DEFF", 0xde80 },
    { "$DF00-$DF7F", 0xdf00 },
    { "$DF80-$DFFF", 0xdf80 },
    { NULL, -1 }
};

/**\brief   List of I/O base addresses for the +60K memory hack
 */
static ui_radiogroup_entry_t plus_60k_base_addresses[] = {
    { "$D040", 0xd040 },
    { "$D100", 0xd100 },
    { NULL, -1 }
};



/** \brief  Handler for the "clicked" event of the browse button for C64_256K
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_256k_image_browse_clicked(GtkWidget *button, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(button, "Select 256K image file", NULL,
            NULL, NULL);
    if (filename != NULL) {
        GtkWidget *grid = gtk_widget_get_parent(button);
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the browse button for +60K
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_plus60k_image_browse_clicked(GtkWidget *button, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(button, "Select +60K image file", NULL,
            NULL, NULL);
    if (filename != NULL) {
        GtkWidget *grid = gtk_widget_get_parent(button);
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the browse button for +256K
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_plus256k_image_browse_clicked(GtkWidget *button, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(button, "Select +256K image file", NULL,
            NULL, NULL);
    if (filename != NULL) {
        GtkWidget *grid = gtk_widget_get_parent(button);
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


/** \brief  Create widget to select the memory hacks device
 *
 * \return  GtkGrid
 */
static GtkWidget *memory_hacks_device_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    grid = uihelpers_create_grid_with_label(
            "C64 memory expansion hack device", 1);
    group = resource_radiogroup_create("MemoryHack", mem_hack_devices,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 16);
    g_object_set(group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to set the I/O base of the C64_256K expansion
 *
 * \return  GtkGrid
 */
static GtkWidget *c64_256k_base_address_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    grid = uihelpers_create_grid_with_label(
            "C64 256K base addresss", 1);
    group = resource_radiogroup_create("C64_256Kbase", c64_256k_base_addresses,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 16);
    g_object_set(group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to set the I/O base of the +60k expansion
 *
 * \return  GtkGrid
 */
static GtkWidget *plus_60k_base_address_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *group;

    grid = uihelpers_create_grid_with_label(
            "+60K base addresss", 1);
    group = resource_radiogroup_create("PLUS60Kbase", plus_60k_base_addresses,
            GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_set_column_spacing(GTK_GRID(group), 16);
    g_object_set(group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Create widget to set the C64_256K image file
 *
 * \return  GtkGrid
 */
static GtkWidget *c64_256k_image_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;

    grid = uihelpers_create_grid_with_label("C64 256K image file", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("filename");
    g_object_set(label, "margin-left", 16, NULL);
    entry = resource_entry_create("C64_256Kfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    g_signal_connect(browse, "clicked",
            G_CALLBACK(on_256k_image_browse_clicked), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to set the +60K image file
 *
 * \return  GtkGrid
 */
static GtkWidget *plus_60k_image_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;

    grid = uihelpers_create_grid_with_label("+60K image file", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("filename");
    g_object_set(label, "margin-left", 16, NULL);
    entry = resource_entry_create("PLUS60Kfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    g_signal_connect(browse, "clicked",
            G_CALLBACK(on_plus60k_image_browse_clicked), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to set the +256K image file
 *
 * \return  GtkGrid
 */
static GtkWidget *plus_256k_image_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;

    grid = uihelpers_create_grid_with_label("+256K image file", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("filename");
    g_object_set(label, "margin-left", 16, NULL);
    entry = resource_entry_create("PLUS256Kfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    g_signal_connect(browse, "clicked",
            G_CALLBACK(on_plus256k_image_browse_clicked), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget controlling C64 memory hacks
 *
 * \param[in]   parent  parent widhet, used for dialog
 *
 * \return  GtkGrid
 */
GtkWidget *c64_memory_expansion_hacks_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), memory_hacks_device_widget_create(),
            0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), c64_256k_base_address_widget_create(),
            0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), c64_256k_image_widget_create(),
            0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), plus_60k_base_address_widget_create(),
            0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), plus_60k_image_widget_create(),
            0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), plus_256k_image_widget_create(),
            0, 6, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
