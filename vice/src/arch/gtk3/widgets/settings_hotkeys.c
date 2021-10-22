/** \file   settings_hotkeys.c
 * \brief   Hotkeys settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#include "archdep.h"
#include "lib.h"
#include "hotkeys.h"
#include "resources.h"
#include "uiactions.h"
#include "uiapi.h"
#include "util.h"

#include "settings_hotkeys.h"


/** \brief  Columns for the hotkeys table
 */
enum {
    COL_ACTION_NAME,    /**< action name (string) */
    COL_ACTION_DESC,    /**< action description (string) */
    COL_HOTKEY          /**< key and modifiers (string) */
};


/** \brief  Reference to the hotkeys table widget
 *
 * Used for easier event handling.
 */
static GtkWidget *hotkeys_view;



/** \brief  Callback for the 'Export' button
 *
 * Save current hotkeys to \a filename and close \a dialog.
 *
 * \param[in]   dialog      Save dialog
 * \param[in]   filename    filename (`NULL` == cancel)
 * \param[in]   data        extra data (unused)
 */
static void export_callback(GtkDialog *dialog, gchar *filename, gpointer data)
{
    if (filename != NULL) {
        if (ui_hotkeys_export(filename)) {
            debug_gtk3("OK, '%s' was written succesfully.", filename);
        } else {
            ui_error("Failed to write '%s' to filesystem.", filename);
        }
    } else {
        debug_gtk3("Canceled.");
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Handler for the 'clicked' event of the export button
 *
 * \param[in]   button  widget triggering the event
 * \param[in]   data    extra event data (unused)
 */
static void on_export_clicked(GtkWidget *button, gpointer data)
{
    GtkWidget *dialog;
    const char *path = NULL;
    char *fname = NULL;
    char *dname = NULL;

    /* split path into basename and directory component */
    if ((resources_get_string("HotkeyFile", &path)) == 0 && path != NULL) {
        util_fname_split(path, &dname, &fname);
    }

    dialog = vice_gtk3_save_file_dialog("Save current hotkeys to file",
                                        fname,
                                        TRUE,
                                        dname,
                                        export_callback,
                                        NULL);
    gtk_widget_show(dialog);

    /* clean up */
    if (fname != NULL) {
        lib_free(fname);
    }
    if (dname != NULL) {
        lib_free(dname);
    }
}


/** \brief  Create widget to select the user-defined hotkeys file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_browse_widget(void)
{
    GtkWidget *widget;
    const char * const patterns[] = { "*.vhk", NULL };

    widget = vice_gtk3_resource_browser_new("HotkeyFile",
                                            patterns,
                                            "VICE hotkeys",
                                            "Select VICE hotkeys file",
                                            "Custom hotkeys file:",
                                            NULL);
    return widget;
}


/** \brief  Create model for the hotkeys table
 *
 * \return  new list store
 */
static GtkListStore *create_hotkeys_model(void)
{
    GtkListStore *model;
    ui_action_info_t *list;
    const ui_action_info_t *action;

    model = gtk_list_store_new(3,
                               G_TYPE_STRING,   /* action name */
                               G_TYPE_STRING,   /* action description */
                               G_TYPE_STRING    /* hotkey as string */
                               );

    list = ui_action_get_info_list();
    for (action = list; action->name != NULL; action++) {
        GtkTreeIter iter;
        char *hotkey;

        hotkey = ui_hotkeys_get_hotkey_string_for_action(action->name);

        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                           &iter,
                           COL_ACTION_NAME, action->name,
                           COL_ACTION_DESC, action->desc,
                           COL_HOTKEY, hotkey,
                           -1);
        if (hotkey != NULL) {
            lib_free(hotkey);
        }
    }
    lib_free(list);

    return model;
}


/** \brief  Create the table view of the hotkeys
 *
 * Create a table with 'action', 'description' and 'hotkey' columns.
 *
 * \return  GtkTreeView
 */
static GtkWidget *create_hotkeys_view(void)
{
    GtkWidget *view;
    GtkListStore *model;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;

    model = create_hotkeys_model();
    view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));

    /* name */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("action",
                                                      renderer,
                                                      "text", COL_ACTION_NAME,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* description */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("description",
                                                      renderer,
                                                      "text", COL_ACTION_DESC,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* hotkey */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("hotkey",
                                                      renderer,
                                                      "text", COL_HOTKEY,
                                                      NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    return view;
}



/** \brief  Create main widget
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_hotkeys_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *scroll;
    GtkWidget *browse;
    GtkWidget *export;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    /* create view, pack into scrolled window and add to grid */
    hotkeys_view = create_hotkeys_view();
    scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                   GTK_POLICY_NEVER,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scroll, TRUE);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_container_add(GTK_CONTAINER(scroll), hotkeys_view);
    gtk_widget_show_all(scroll);
    gtk_grid_attach(GTK_GRID(grid), scroll, 0, 0, 1, 1);

    browse = create_browse_widget();
    gtk_grid_attach(GTK_GRID(grid), browse, 0, 1, 1, 1);

    export = gtk_button_new_with_label("Save current hotkeys to file");
    g_signal_connect(export, "clicked", G_CALLBACK(on_export_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), export, 0, 2, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}
