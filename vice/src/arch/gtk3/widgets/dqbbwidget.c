/** \file   src/arch/gtk3/widgets/dqbb.c
 * \brief   Widget to control Double Quick Brown Box resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  DQBB (x64/x64sc/xscpu64/x128)
 *  DQBBfilename (x64/x64sc/xscpu64/x128)
 *  DQBBImageWrite (x64/x64sc/xscpu64/x128)
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

#include "dqbbwidget.h"


/* list of widgets, used to enable/disable depending on Double Quick Brown Box
 * resources */
static GtkWidget *dqbb_enable_widget = NULL;
static GtkWidget *dqbb_image = NULL;

static int (*dqbb_save_func)(int, const char *) = NULL;
static int (*dqbb_flush_func)(int) = NULL;


/** \brief  Handler for the "toggled" event of the dqbb_enable widget
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   unused
 */
static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(dqbb_image, state);
}


/** \brief  Handler for the "clicked" event of the "browse" button
 *
 * Select an image file for the Double Quick Brown Box extension.
 *
 * \param[in]   button      browse button
 * \param[in]   user_data   unused
 */
static void on_browse_clicked(GtkWidget *button, gpointer user_data)
{
    gchar *filename;

    filename = ui_open_file_dialog(button,
            "Open Double Quick Brown Box image file", NULL, NULL, NULL);
    if (filename != NULL) {
        GtkWidget *grid = gtk_widget_get_parent(button);
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(grid), 1, 1);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "clicked" event of the "save" button
 *
 * Save Double Quick Brown Box image file. Uses dirname()/basename() on the
 * Double Quick Brown Boxfilename resource to act as a "Save" button, but also
 * allows changing filename/dir to act as a "Save As" button.
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

    resources_get_string("Double Quick Brown Boxfilename", &current_filename);
    if (current_filename != NULL && *current_filename != '\0') {
        /* provide the current filename and path */
        fname = g_path_get_basename(current_filename);
        dname = g_path_get_dirname(current_filename);
        debug_gtk3("got dir '%s', file '%s'\n", dname, fname);
    }

    new_filename = ui_save_file_dialog(button,
            "Save Double Quick Brown Box image file",
            fname, TRUE, dname);
    if (new_filename != NULL) {
        debug_gtk3("writing Double Quick Brown Box file image as '%s'\n",
                new_filename);
        /* write file */
        if (dqbb_save_func != NULL) {
            if (dqbb_save_func(CARTRIDGE_DQBB, new_filename) < 0) {
                /* oops */
                ui_message_error(button, "I/O error",
                        "Failed to save '%s'", new_filename);
            }
        } else {
            ui_message_error(button, "Core error",
                    "Double Quick Brown Box save handler not specified");
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


/** \brief  Handler for the "clicked" event of the "Flush image" button
 *
 * \param[in]   widget      button triggering the event
 * \param[in]   user_data   unused
 */
static void on_flush_clicked(GtkWidget *widget, gpointer user_data)
{
    if (dqbb_flush_func != NULL) {
        if (dqbb_flush_func(CARTRIDGE_DQBB) < 0) {
            ui_message_error(widget, "I/O error", "Failed to flush image");
        }
    } else {
        ui_message_error(widget, "Core error",
                "Double Quick Brown Bow flush handler not specified");
    }
}



/** \brief  Create Double Quick Brown Box enable check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_dqbb_enable_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("DQBB",
            "Enable Double Quick Brown Box");
    return check;
}


/** \brief  Create widget to load/save Double Quick Brown Box image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_dqbb_image_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *browse;
    GtkWidget *auto_save;
    GtkWidget *save_button;
    GtkWidget *flush_button;

    grid = uihelpers_create_grid_with_label("Image file", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    label = gtk_label_new("file name");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    entry = resource_entry_create("DQBBfilename");
    gtk_widget_set_hexpand(entry, TRUE);
    browse = gtk_button_new_with_label("Browse ...");

    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), browse, 2, 1, 1, 1);

    auto_save = resource_check_button_create("DQBBImageWrite",
            "Write image on image detach/emulator quit");
    g_object_set(auto_save, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), auto_save, 0, 2, 2, 1);

    save_button = gtk_button_new_with_label("Save ...");
    gtk_grid_attach(GTK_GRID(grid), save_button, 2, 2, 1, 1);

    flush_button = gtk_button_new_with_label("Flush image");
    gtk_grid_attach(GTK_GRID(grid), flush_button, 2, 3, 1, 1);

    g_signal_connect(browse, "clicked", G_CALLBACK(on_browse_clicked), NULL);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), NULL);
    g_signal_connect(flush_button, "clicked", G_CALLBACK(on_flush_clicked),
            NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control Double Quick Brown Box resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *dqbb_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    dqbb_enable_widget = create_dqbb_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), dqbb_enable_widget, 0, 0, 1, 1);

    dqbb_image = create_dqbb_image_widget();
    gtk_grid_attach(GTK_GRID(grid), dqbb_image, 0, 1, 1, 1);

    g_signal_connect(dqbb_enable_widget, "toggled", G_CALLBACK(on_enable_toggled),
            NULL);

    /* enable/disable widget based on dqbb-enable (dirty trick, I know) */
    on_enable_toggled(dqbb_enable_widget, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set save function for the Double Quick Brown Box extension
 *
 * \param[in]   func    save function
 */
void dqbb_widget_set_save_handler(int (*func)(int, const char *))
{
    dqbb_save_func = func;
}


/** \brief  Set flush function for the Double Quick Brown Box extension
 *
 * \param[in]   func    flush function
 */
void dqbb_widget_set_flush_handler(int (*func)(int))
{
    dqbb_flush_func = func;
}
