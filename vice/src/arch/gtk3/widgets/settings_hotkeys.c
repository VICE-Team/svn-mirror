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
#include "ui.h"
#include "uiactions.h"
#include "uiapi.h"
#include "uimachinemenu.h"
#include "uimenu.h"
#include "uitypes.h"
#include "util.h"

#include "settings_hotkeys.h"


/** \brief  Columns for the hotkeys table
 */
enum {
    COL_ACTION_NAME,    /**< action name (string) */
    COL_ACTION_DESC,    /**< action description (string) */
    COL_HOTKEY          /**< key and modifiers (string) */
};


/** \brief  Set/Unset hotkey dialog custom response IDs
 */
enum {
    RESPONSE_CLEAR  /**< clear hotkey for selected item */
};


/** \brief  Reference to the hotkeys table widget
 *
 * Used for easier event handling.
 */
static GtkWidget *hotkeys_view;


/** \brief  Label holding the hotkey string
 */
static GtkWidget *hotkey_string;

static guint hotkey_keysym;
static guint hotkey_mask;   /* modifiers mask */

static guint hotkey_keysym_old;
static guint hotkey_mask_old;   /* modifiers mask */



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


static gboolean on_key_release_event(GtkWidget *dialog,
                                     GdkEventKey *event,
                                     gpointer data)
{
    if (!event->is_modifier) {
        gchar *accel;
        gchar *escaped;
        gchar text[256];

        hotkey_keysym = event->keyval;
        hotkey_mask = event->state;

        if (hotkey_mask & GDK_CONTROL_MASK) {
            /* we don't want CTRL */
            return FALSE;
        }

        if (hotkey_keysym == GDK_KEY_Return && hotkey_mask == 0) {
            return TRUE;    /* Return means accept */
        }
#if 0
        debug_gtk3("keysym = %04x, mask = %04x.", hotkey_keysym, hotkey_mask);
#endif
        accel = gtk_accelerator_name(hotkey_keysym, hotkey_mask);
        escaped = g_markup_escape_text(accel, -1);
        g_snprintf(text, sizeof(text), "<b>%s</b>", escaped);
        gtk_label_set_markup(GTK_LABEL(hotkey_string), text);
        g_free(escaped);
        g_free(accel);
        return TRUE;
    }

    return TRUE;
}



/** \brief  Update the hotkey string of the currently selected row
 *
 * \param[in]   accel   hotkey string
 *
 * \return  TRUE on success
 */
static gboolean update_treeview_hotkey(const gchar *accel)
{
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;

    /* update entry in tree model */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(hotkeys_view));
    if (gtk_tree_selection_get_selected(selection,
                                        &model,
                                        &iter)) {
        gtk_list_store_set(GTK_LIST_STORE(model),
                          &iter,
                          COL_HOTKEY, accel,
                          -1);
        return TRUE;
    }
    return FALSE;
}



/** \brief  Handler for the 'response' event of the dialog
 *
 * \param[in]   dialog      hotkey dialog
 * \param[in]   response_id response ID
 * \param[in]   data        action name
 */
static void on_response(GtkDialog *dialog, gint response_id, gpointer data)
{
    gchar *accel;
    gchar *action = data;
    ui_menu_item_t *item_vice;
    GtkWidget *item_gtk;

    switch (response_id) {

        case GTK_RESPONSE_ACCEPT:
            debug_gtk3("Response ID %d: Accept '%s'", response_id, action);


            accel = gtk_accelerator_name(hotkey_keysym, hotkey_mask);
            debug_gtk3("Setting accelerator: %s.", accel);

            debug_gtk3("hotkey keysym: %04x, mask: %04x.",
                    hotkey_keysym, hotkey_mask);

            /* lookup item for new hotkey and remove the hotkey
             * from that action/menu item */
            /* XXX: somehow the mask gets OR'ed with $2000, which is a reserved
             *      flag, so we mask out any reserved bits:
             * TODO: Better do the masking out in the called function */
            item_vice = ui_get_vice_menu_item_by_hotkey(hotkey_mask & 0x1fff,
                                                        hotkey_keysym);
            if (item_vice != NULL) {
                debug_gtk3("Label: %s, action: %s.", item_vice->label,
                        item_vice->action_name);
                ui_menu_remove_accel_via_vice_item(item_vice);
                item_vice->keysym = 0;
                item_vice->modifier = 0;
            }


            item_vice = ui_get_vice_menu_item_by_name(action);
            item_gtk = ui_get_gtk_menu_item_by_name(action);

            /* update vice item and gtk item */
            if (item_vice != NULL && item_gtk != NULL) {
                /* remove old accelerator */
                ui_menu_remove_accel_via_vice_item(item_vice);
                /* update vice menu item */
                item_vice->keysym = hotkey_keysym;
                item_vice->modifier = hotkey_mask;
                /* now update the accelerator and closure with the updated
                 * vice menu item */
                ui_menu_set_accel_via_vice_item(item_gtk, item_vice);
                /* update treeview */
                update_treeview_hotkey(accel);
            }

            g_free(accel);
            break;

        case RESPONSE_CLEAR:
            debug_gtk3("Response ID %d: Clear '%s'", response_id, action);
            item_vice = ui_get_vice_menu_item_by_name(action);

#if 0
            debug_gtk3("item_vice = %p, item_gtk = %p.",
                    (void*)item_vice, (void*)item_gtk);
#endif
            if (item_vice != NULL) {
                debug_gtk3("Got vice item.");
                /* remove old accelerator */
                ui_menu_remove_accel_via_vice_item(item_vice);
                /* update vice menu item */
                item_vice->keysym = 0;
                item_vice->modifier = 0;
                /* update treeview */
                update_treeview_hotkey(NULL);
            }

            break;

        case GTK_RESPONSE_REJECT:
            debug_gtk3("Response ID %d: Cancel pushed.", response_id);
            break;

        default:
            debug_gtk3("Unhandled response ID: %d.", response_id);
    }

    g_free(action);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}


/** \brief  Create content widget for the hotkey dialog
 *
 * \param[in]   action  hotkey action name
 * \param[in]   hotkey  hotkey string
 *
 * \return  GtkGrid
 */
static GtkWidget *create_content_widget(const gchar *action, const gchar *hotkey)
{
    GtkWidget *grid;
    GtkWidget *instructions;
    GtkWidget *hotkey_label;
    gchar text[1024];
    gchar *escaped;

    grid = vice_gtk3_grid_new_spaced(16, 32);
    g_object_set(grid, "margin-left", 16, "margin-right", 16, NULL);

    instructions = gtk_label_new(NULL);
    g_snprintf(text,
               sizeof(text),
               "Press a key(combination) to set the new hotkey"
               " for '<b>%s</b> and press Accept.\n"
               "Press Clear to clear the hotkey and press Cancel to cancel.",
               action);
    gtk_label_set_markup(GTK_LABEL(instructions), text);

    hotkey_label = gtk_label_new("Current hotkey:");
    gtk_widget_set_halign(hotkey_label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(hotkey_label, FALSE);
    gtk_grid_attach(GTK_GRID(grid), hotkey_label, 0, 1, 1, 1);

    escaped = g_markup_escape_text(hotkey, -1);
    g_snprintf(text, sizeof(text), "<b>%s</b>", escaped);
    g_free(escaped);
    hotkey_string = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(hotkey_string), text);
    gtk_widget_set_halign(hotkey_string, GTK_ALIGN_START);
    gtk_widget_set_hexpand(hotkey_string, TRUE);
    gtk_grid_attach(GTK_GRID(grid), hotkey_string, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), instructions, 0, 0, 2, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Handler for the 'row-activated' event of the treeview
 *
 * Handler triggered by double-clicked or pressing Return on a tree row.
 *
 * \param[in]   view    tree view
 * \param[in]   path    tree path to activated row
 * \param[in]   column  activated column
 * \param[in]   data    extra event data (unused)
 */
static void on_row_activated(GtkTreeView *view,
                             GtkTreePath *path,
                             GtkTreeViewColumn *column,
                             gpointer data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_tree_view_get_model(view);
    if (gtk_tree_model_get_iter(model, &iter, path)) {

        GtkWidget *dialog;
        GtkWidget *content_area;
        GtkWidget *content_widget;
        gchar *action = NULL;
        gchar *hotkey = NULL;
        guint keysym = 0;
        GdkModifierType mask = 0;

        /* get current hotkey info */
        gtk_tree_model_get(model, &iter,
                           COL_ACTION_NAME, &action,
                           COL_HOTKEY, &hotkey,
                           -1);
        if (hotkey != NULL) {
            gtk_accelerator_parse(hotkey, &keysym, &mask);
            hotkey_keysym = keysym;
            hotkey_keysym_old = keysym;
            hotkey_mask = mask;
            hotkey_mask_old = mask;

        }
        debug_gtk3("Row clicked: '%s' -> %s (mask: %04x, keysym: %04x).",
                   action,
                   hotkey != NULL ? hotkey : NULL,
                   mask,
                   keysym);

        dialog = gtk_dialog_new_with_buttons("Set/Unset hotkey",
                                             ui_get_active_window(),
                                             GTK_DIALOG_MODAL,
                                             "Accept", GTK_RESPONSE_ACCEPT,
                                             "Clear", RESPONSE_CLEAR,
                                             "Cancel", GTK_RESPONSE_REJECT,
                                             NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        content_widget = create_content_widget(action, hotkey);

        gtk_box_pack_start(GTK_BOX(content_area), content_widget, TRUE, TRUE, 16);

        g_signal_connect(dialog,
                         "response",
                         G_CALLBACK(on_response),
                         (gpointer)action);
        g_signal_connect(dialog,
                         "key-release-event",
                         G_CALLBACK(on_key_release_event),
                         NULL);
        gtk_widget_show_all(dialog);

        // g_free(action);
        g_free(hotkey);
    }
}



/** \brief  Create the table view of the hotkeys
 *
 * Create a table with 'action', 'description' and 'hotkey' columns.
 * Columns are resizable and the table is sortable by clicking the column
 * headers.
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
    column = gtk_tree_view_column_new_with_attributes("Action",
                                                      renderer,
                                                      "text", COL_ACTION_NAME,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_ACTION_NAME);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* description */
    renderer = gtk_cell_renderer_text_new();
    /* g_object_set(G_OBJECT(renderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL); */
    column = gtk_tree_view_column_new_with_attributes("Description",
                                                      renderer,
                                                      "text", COL_ACTION_DESC,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_ACTION_DESC);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    /* hotkey */
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Hotkey",
                                                      renderer,
                                                      "text", COL_HOTKEY,
                                                      NULL);
    gtk_tree_view_column_set_sort_column_id(column, COL_HOTKEY);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(view), column);

    g_signal_connect(view, "row-activated", G_CALLBACK(on_row_activated), NULL);

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
