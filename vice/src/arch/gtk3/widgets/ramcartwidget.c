/** \file   src/arch/gtk3/widgets/ramcartwidget.c
 * \brief   Widget to control RamCart resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  RAMCART
 *  RAMCARTsize
 *  RAMCARTfilename
 *  RAMCARTImageWrite
 *  RAMCART_RO
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
#if 0
#include "ramcart.h"
#endif
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "cartridge.h"

#include "ramcartwidget.h"


/** \brief  List of supported RAM sizes
 */
static ui_radiogroup_entry_t ram_sizes[] = {
    { "128KB", 128 },
    { "256KB", 256 },
    { NULL, -1 }
};


/* list of widgets, used to enable/disable depending on RAMCART resource */
static GtkWidget *ramcart_enable_widget = NULL;
static GtkWidget *ramcart_size = NULL;
static GtkWidget *ramcart_readonly = NULL;
static GtkWidget *ramcart_image = NULL;

static int (*ramcart_save_handler)(int, const char *) = NULL;


/** \brief  Handler for the "toggled" event of the ramcart_enable widget
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   unused
 */
static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(ramcart_size, state);
    gtk_widget_set_sensitive(ramcart_image, state);
}


/** \brief  Handler for the "clicked" event of the "browse" button
 *
 * Select an image file for the RAMCART extension.
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_browse_clicked(GtkWidget *button, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(button,
            "Open RAMCART image file", NULL, NULL, NULL);
    if (filename != NULL) {
        GtkWidget *grid = gtk_widget_get_parent(button);
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the "save" button
 *
 * Save RAMCART image file. Uses dirname()/basename() on the RAMCARTfilename
 * resource to act as a "Save" button, but also allows changing filename/dir
 * to act as a "Save As" button.
 *
 * \param[in]   button      save button
 * \param[in]   user_data   unused
 */
static void on_save_clicked(GtkWidget *button, gpointer user_data)
{
    const char *current_filename;
    gchar *new_filename;
    gchar *fname = NULL;
    gchar *dname = NULL;

    resources_get_string("RAMCARTfilename", &current_filename);
    if (current_filename != NULL && *current_filename != '\0') {
        /* provide the current filename and path */
        fname = g_path_get_basename(current_filename);
        dname = g_path_get_dirname(current_filename);
        debug_gtk3("got dir '%s', file '%s'\n", dname, fname);
    }

    new_filename = ui_save_file_dialog(button,
            "Save RAMCART image file",
            fname, TRUE, dname);
    if (new_filename != NULL) {
        debug_gtk3("writing RAMCART file image as '%s'\n",
                new_filename);
        /* write file */
        if (ramcart_save_handler != NULL) {
            if (ramcart_save_handler(CARTRIDGE_RAMCART, new_filename) < 0) {
                /* oops */
                ui_message_error(button, "I/O error",
                        "Failed to save '%s'", new_filename);
            }
        } else {
            ui_message_error(button, "Core error",
                    "RAMCART save handler not specified");
        }
        g_free(new_filename);
    }

    if (fname != NULL) {
        g_free(fname);
    }
    if (dname != NULL) {
        g_free(dname);
    }
}


/** \brief  Create RAMCART enable check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_ramcart_enable_widget(void)
{
    return resource_check_button_create("RAMCART", "Enable RAMCART expansion");
}


static GtkWidget *create_ramcart_readonly_widget(void)
{
    return resource_check_button_create("RAMCART_RO",
            "RAMCART contents are read only");
}


/** \brief  Create radio button group to determine GEORAM RAM size
 *
 * \return  GtkGrid
 */
static GtkWidget *create_ramcart_size_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = uihelpers_create_grid_with_label("RAM Size", 1);
    radio_group = resource_radiogroup_create("RAMCARTsize", ram_sizes,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to load/save GEORAM image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_ramcart_image_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;
    GtkWidget *auto_save;
    GtkWidget *save_button;

    grid = uihelpers_create_grid_with_label("Image file", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    label = gtk_label_new("file name");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    entry = resource_entry_create("RAMCARTfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    auto_save = resource_check_button_create("RAMCARTImageWrite",
            "Write image on image detach/emulator quit");
    g_object_set(auto_save, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), auto_save, 0, 2, 2, 1);

    save_button = gtk_button_new_with_label("Save ...");
    gtk_grid_attach(GTK_GRID(grid), save_button, 2, 2, 1, 1);

    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked), NULL);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control RAM Expansion Module resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *ramcart_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    ramcart_enable_widget = create_ramcart_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_enable_widget, 0, 0, 1, 1);

    ramcart_size = create_ramcart_size_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_size, 0, 1, 1, 1);

    ramcart_image = create_ramcart_image_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_image, 1, 1, 1, 1);

    g_signal_connect(ramcart_enable_widget, "toggled", G_CALLBACK(on_enable_toggled),
            NULL);

    ramcart_readonly = create_ramcart_readonly_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_readonly, 0, 2, 2,1);

    /* enable/disable widget based on ramcart-enable (dirty trick, I know) */
    on_enable_toggled(ramcart_enable_widget, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set save function for the RAM module extension
 *
 * \param[in]   func    save function
 */
void ramcart_widget_set_save_handler(int (*func)(int, const char *))
{
    ramcart_save_handler = func;
}
