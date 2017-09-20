/*
 * uisettings.c - GTK3 settings dialog
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
 */


/* The settings_grid is supposed to become this:
 *
 * +--------------+---------------------------+
 * | treeview     |                           |
 * |  with        |                           |
 * |   settings   |    central widget,        |
 * |  more        |    depending on which     |
 * |   foo        |    item is selected in    |
 * |   bar        |    the treeview           |
 * |    whatever  |                           |
 * | burp         |                           |
 * +--------------+---------------------------+
 *
 * And this is handled by the dialog itself:
 * +------------------------------------------+
 * | load | save | load... | save... | close  |
 * +------------------------------------------+
 */


#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "resources.h"
#include "vsync.h"

#include "debug_gtk3.h"
#include "widgethelpers.h"
#include "savefiledialog.h"

#include "uispeed.h"
#include "uikeyboard.h"
#include "uisound.h"
#include "uiautostart.h"
#include "uidrivesettings.h"


#include "uisettings.h"


#define NUM_COLUMNS 2

/** \brief  Enum used for the "response" callback of the settings dialog
 *
 * All values must be positive since Gtk reserves standard responses in its
 * GtkResponse enum as negative values.
 */
enum {
    RESPONSE_LOAD = 1,  /**< "Load" -> load settings from default file */
    RESPONSE_SAVE,      /**< "Save" -> save settings from default file */
    RESPONSE_LOAD_FILE, /**< "Load ..." -> load settings via dialog */
    RESPONSE_SAVE_FILE  /**< "Save ..." -> save settings via dialog */
};


/** \brief  Main tree nodes
 */
static ui_settings_tree_node_t main_nodes[] = {
    { "Speed", uispeed_create_central_widget, NULL },
    { "Keyboard", uikeyboard_create_central_widget, NULL },
    { "Sound", uisound_create_central_widget, NULL },
    { "Autostart", uiautostart_create_central_widget, NULL },
    { "Drive settings", uidrivesettings_create_central_widget, NULL },
    { NULL, NULL, NULL }
};


/** \brief  Reference to the current 'central' widget in the settings dialog
 */
static void ui_settings_set_central_widget(GtkWidget *widget);


/** \brief  Reference to the settings dialog
 *
 * Used to show/hide the widget without rebuilding it each time. Clean up
 * with ui_settings_dialog_shutdown()
 */
static GtkWidget *settings_window = NULL;


/** \brief  Reference to the 'content area' widget of the settings dialog
 */
static GtkWidget *settings_grid = NULL;


/** \brief  Checkbox for 'Save settings on exit'
 *
 * This can probably moved inside the settings widget constructor once the
 * layout is settled using gtk_grid_get_child_at()
 */
static GtkWidget *save_on_exit = NULL;


/** \brief  Handler for the "changed" event of the tree view
 *
 * \param[in]   selection   GtkTreeSelection associated with the tree model
 * \param[in]   user_data   data for the event (unused for now)
 *
 */
static void on_tree_selection_changed(
        GtkTreeSelection *selection,
        gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gchar *name;
        GtkWidget *(*callback)(void *) = NULL;
        gtk_tree_model_get(model, &iter, 0 /* col 0 */, &name, -1);
        debug_gtk3("item '%s' clicked\n", name);
        gtk_tree_model_get(model, &iter, 1, &callback, -1);
        if (callback != NULL) {
            ui_settings_set_central_widget(callback(NULL));
        }
        g_free(name);
    }
}

#if 0
/** \brief  Handler for the 'toggled' event of the 'save on exit' checkbox
 *
 * \param[in]   widget      checkbox widget
 * \param[in]   user_data   data for the event (unused)
 */
static void on_save_on_exit_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    resources_set_int("SaveResourcesOnExit", state);
}
#endif


/** \brief  Create the 'Save on exit' checkbox
 *
 * The current position/display of the checkbox is a little lame at the moment
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_save_on_exit_checkbox(void)
{
    return uihelpers_create_resource_checkbox(
            "Save settings on exit",
            "SaveResourcesOnExit");
#if 0
    GtkWidget *check;

    check = gtk_check_button_new_with_label("Save settings on exit");
    gtk_widget_show(check);
    g_signal_connect(check, "toggled", G_CALLBACK(on_save_on_exit_toggled),
            NULL);
    return check;
#endif
}


/** \brief  Create treeview for settings side-menu
 *
 * Reads items from `main_nodes` and adds them to the tree view.
 *
 * \return  GtkTreeView
 *
 * TODO:    handle nested items, and write up somewhere how the hell I finally
 *          got the callbacks working
 */
static GtkWidget *create_treeview(void)
{
    GtkWidget *tree;
    GtkTreeStore *store;
    GtkCellRenderer *text_renderer;
    GtkTreeViewColumn *text_column;

    GtkTreeIter iter;   /* parent iter */
    size_t i;
#if 0
    GtkTreeIter child;  /* child iter */
#endif

    /* create the model */
    store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_POINTER);

    /* add root node */
    /*    gtk_tree_store_append(store, &iter, NULL); */

    for (i = 0; main_nodes[i].name != NULL; i++) {
        gtk_tree_store_append(store, &iter, NULL);
        gtk_tree_store_set(
                store, &iter,
                0, main_nodes[i].name,
                1, main_nodes[i].callback,
                -1);
    }

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));


    text_renderer = gtk_cell_renderer_text_new();
    text_column = gtk_tree_view_column_new_with_attributes(
            NULL,
            text_renderer,
            "text", 0,
            NULL);
/*    obj_column = gtk_tree_view_column_new_with_attributes(
            NULL,
            NULL,
            "text", 0,
            NULL);
*/
    /*    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), obj_column); */
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), text_column);
    return tree;
}


/** \brief  Set the 'central'/action widget for the settings dialog
 *
 * Destroys the old 'central' widget and sets the new one.
 *
 *  \param[in,out]  widget  widget to use as the new 'central' widget
 */
static void ui_settings_set_central_widget(GtkWidget *widget)
{
    GtkWidget *child;

    child = gtk_grid_get_child_at(GTK_GRID(settings_grid), 1, 0);
    if (child != NULL) {
        gtk_widget_destroy(child);
    }
    gtk_grid_attach(GTK_GRID(settings_grid), widget, 1, 0, 1, 1);
}


/** \brief  Create the 'content widget' of the settings dialog
 *
 * This creates the widget in the dialog used to display the treeview and room
 * for the widget connected to that tree's currently selected item.
 *
 * \param[in]   widget  parent widget
 *
 * \return  GtkGrid (as a GtkWidget)
 */
static GtkWidget *create_content_widget(GtkWidget *widget)
{
    GtkWidget *tree;
    GtkTreeSelection *selection;
    int soe_state;      /* save-on-exit state */


    debug_gtk3("called\n");

    settings_grid = gtk_grid_new();
    tree = create_treeview();
    g_print("tree created\n");

    gtk_grid_attach(GTK_GRID(settings_grid), tree, 0, 0, 1, 1);

    /* TODO: remember the previously selected setting/widget and set it here */
    ui_settings_set_central_widget(uispeed_create_central_widget(widget));

    save_on_exit = create_save_on_exit_checkbox();
    soe_state= resources_get_int("SaveResourcesOnExit", &soe_state);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(save_on_exit), soe_state);
    gtk_grid_attach(GTK_GRID(settings_grid), save_on_exit, 0, 2, 2, 1);


    gtk_widget_show(settings_grid);
    gtk_widget_show(tree);

    gtk_widget_set_size_request(tree, 150, 500);
    gtk_widget_set_size_request(settings_grid, 600, 550);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(selection), "changed",
            G_CALLBACK(on_tree_selection_changed), NULL);


    return settings_grid;
}


/** \brief  Properly destroy the settings window if required
 */
void ui_settings_dialog_shutdown(void)
{
    if (settings_window != NULL && GTK_IS_WIDGET(settings_window)) {
        gtk_widget_destroy(settings_window);
    }
}



/** \brief  Handler for the "response" event of the settings dialog
 *
 * This determines what to do based on the 'reponse ID' emitted by the dialog.
 *
 * \param[in]   widget      widget triggering the event (button pushed)
 * \param[in]   user_data   response ID (`gint`)
 */
static void response_callback(GtkWidget *widget, gpointer user_data)
{
    gint response_id = GPOINTER_TO_INT(user_data);
    gchar *filename;

    debug_gtk3("got response ID %d\n", response_id);

    switch (response_id) {
        case GTK_RESPONSE_DELETE_EVENT:
            gtk_widget_destroy(widget);
            break;

        case RESPONSE_SAVE_FILE:
            filename = ui_save_file_dialog(widget, "Save settings as ...",
                    NULL, TRUE);
            debug_gtk3("got save filename '%s'\n", filename ? filename : "NULL");
            g_free(filename);
            break;


        default:
            break;
    }
}


/** \brief  Callback to create the main settings dialog from the menu
 *
 * \param[in]   widget      (direct) parent widget, the menu item
 * \param[in]   user_data   data for the event (unused)
 *
 * \note    The appearance of minimize/maximize buttons seems to depend on which
 *          Window Manager is active:
 *
 *          On MATE (marco, a Metacity fork) both buttons are hidden.
 *          On KDE (KWin) the maximize button is still visible but inactive
 *          On OpenBox both min/max are visible with only minimize working
 */
void ui_settings_dialog_create(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkWidget *content;

    dialog = gtk_dialog_new_with_buttons(
            "Settings",
            GTK_WINDOW(gtk_widget_get_toplevel(widget)),
            GTK_DIALOG_MODAL,
            "Load", RESPONSE_LOAD,
            "Save", RESPONSE_SAVE,
            "Load file ...", RESPONSE_LOAD_FILE,
            "Save file ...", RESPONSE_SAVE_FILE,
            "Close", GTK_RESPONSE_DELETE_EVENT,
            NULL);

    content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(content), create_content_widget(dialog));

    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    g_signal_connect(dialog, "response", G_CALLBACK(response_callback), NULL);
    gtk_widget_show_all(dialog);
}
