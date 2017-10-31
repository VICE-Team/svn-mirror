/** \file   src/arch/gtk3/widgets/expertwidget.c
 * \brief   Widget to control Expert Cartridge resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  ExpertCartridgeEnabled (x64/x64sc/xscpu64/x128)
 *  ExpertCartridgeMode (x64/x64sc/xscpu64/x128)
 *  Expertfilename (x64/x64sc/xscpu64/x128)
 *  ExpertImageWrite (x64/x64sc/xscpu64/x128)
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

#include "expertwidget.h"


/** \brief  List of 'modes' for the Expert Cartridge
 */
static ui_radiogroup_entry_t mode_list[] = {
    { "Off", 0 },
    { "Programmable", 1 },
    { "On", 2 },
    { NULL, -1 }
};


/* list of widgets, used to enable/disable depending on Expert Cartridge resource */
static GtkWidget *expert_enable_widget = NULL;
static GtkWidget *expert_image = NULL;
static GtkWidget *expert_mode = NULL;

static int (*expert_save_func)(int, const char *) = NULL;


/** \brief  Handler for the "toggled" event of the expert_enable widget
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   unused
 */
static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(expert_image, state);
    gtk_widget_set_sensitive(expert_mode, state);
}


/** \brief  Handler for the "clicked" event of the "browse" button
 *
 * Select an image file for the Expert Cartridge extension.
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_browse_clicked(GtkWidget *button, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(button,
            "Open Expert Cartridge image file", NULL, NULL, NULL);
    if (filename != NULL) {
        GtkWidget *grid = gtk_widget_get_parent(button);
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the "save" button
 *
 * Save Expert Cartridge image file. Uses dirname()/basename() on the Expert Cartridgefilename
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

    resources_get_string("Expert Cartridgefilename", &current_filename);
    if (current_filename != NULL && *current_filename != '\0') {
        /* provide the current filename and path */
        fname = g_path_get_basename(current_filename);
        dname = g_path_get_dirname(current_filename);
        debug_gtk3("got dir '%s', file '%s'\n", dname, fname);
    }

    new_filename = ui_save_file_dialog(button,
            "Save Expert Cartridge image file",
            fname, TRUE, dname);
    if (new_filename != NULL) {
        debug_gtk3("writing Expert Cartridge file image as '%s'\n", new_filename);
        /* write file */
        if (expert_save_func != NULL) {
            if (expert_save_func(CARTRIDGE_EXPERT, new_filename) < 0) {
                /* oops */
                ui_message_error(button, "I/O error",
                        "Failed to save '%s'", new_filename);
            }
        } else {
            ui_message_error(button, "Core error",
                    "Expert Cartridge save handler not specified");
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


/** \brief  Create Expert Cartridge enable check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_expert_enable_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("ExpertCartridgeEnabled",
            "Enable Expert Cartridge");
    return check;
}


/** \brief  Create Expert Cartridge mode widget
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_expert_mode_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = uihelpers_create_grid_with_label("Cartridge mode", 3);

    radio_group = resource_radiogroup_create("ExpertCartridgeMode",
            mode_list, GTK_ORIENTATION_HORIZONTAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_set_column_spacing(GTK_GRID(radio_group), 16);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to load/save Expert Cartridge image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_expert_image_widget(void)
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
    entry = resource_entry_create("Expertfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    auto_save = resource_check_button_create("ExpertImageWrite",
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


/** \brief  Create widget to control Expert Cartridge resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *expert_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    expert_enable_widget = create_expert_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), expert_enable_widget, 0, 0, 1, 1);

    /* TODO: add widget for MODE */
    expert_mode = create_expert_mode_widget();
    gtk_grid_attach(GTK_GRID(grid), expert_mode, 0, 1, 1, 1);

    expert_image = create_expert_image_widget();
    gtk_grid_attach(GTK_GRID(grid), expert_image, 0, 2, 1, 1);

    g_signal_connect(expert_enable_widget, "toggled", G_CALLBACK(on_enable_toggled),
            NULL);

    /* enable/disable widget based on expert-enable (dirty trick, I know) */
    on_enable_toggled(expert_enable_widget, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set save function for the Expert Cartridge extension
 *
 * \param[in]   func    save function
 */
void expert_widget_set_save_handler(int (*func)(int, const char *))
{
    expert_save_func = func;
}
