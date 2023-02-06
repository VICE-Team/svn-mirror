/** \file   resourcecombobox.c
 * \brief   Combo boxes connected to a resource
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
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "basewidget_types.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "log.h"
#include "resourcehelpers.h"
#include "resources.h"
#include "resourcewidgetmediator.h"

#include "resourcecombobox.h"

/* Model columns for (id, value) pairs */
enum {
    COLUMN_ID,      /**< ID column (hidden) */
    COLUMN_VALUE    /**< value column (shown) */
};


/*
 * Combo box for integer resources
 *
 * Presents a combo box with strings and uses integer keys to control a resource
 */

/** \brief  Create a model for a combo box with ints as IDs
 *
 * \param[in]   list    list of options
 *
 * \return  model
 */
static GtkListStore *create_combo_int_model(const vice_gtk3_combo_entry_int_t *list)
{
    GtkListStore *model;
    int           index;

    model = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    for (index = 0; list[index].name != NULL; index++) {
        GtkTreeIter iter;
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                           &iter,
                           COLUMN_ID,    list[index].id,
                           COLUMN_VALUE, list[index].name,
                           -1);
    }
    return model;
}

/** \brief  Create a view for a combo box with ints as IDs
 *
 * \param[in]   model   model
 *
 * \return  GtkComboBox
 */
static GtkWidget *create_combo_int_view(GtkListStore *model)
{
    GtkWidget       *view;
    GtkCellRenderer *renderer;

    /* TODO: almost exact copy of create_combo_hex_view(), so refactor! */

    /* create combo box with text renderer */
    view = gtk_combo_box_new();
    gtk_combo_box_set_model(GTK_COMBO_BOX(view), GTK_TREE_MODEL(model));
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(view), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(view),
                                   renderer,
                                   "text", COLUMN_VALUE,
                                   NULL);
    return view;
}

/** \brief  Get current ID of \a combo
 *
 * \param[in]   combo   combo box
 * \param[out]  id      target of ID value
 *
 * \return  boolean
 *
 * \note    When this function returns `false`, the value in \a id is unchanged
 */
static gboolean get_combo_int_id(GtkComboBox *combo, int *id)
{
    GtkTreeIter iter;

    if (gtk_combo_box_get_active_iter(combo, &iter)) {
        GtkTreeModel *model = gtk_combo_box_get_model(combo);
        gtk_tree_model_get(model, &iter, COLUMN_ID, id, -1);
        return TRUE;
    }
    return FALSE;
}

/** \brief  Set ID of \a combo to \a id
 *
 * \param[in,out]   combo   combo box
 * \param[in]       id      ID for \a combo
 *
 * \return  boolean
 */
static gboolean set_combo_int_id(GtkComboBox *combo, int id)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    model = gtk_combo_box_get_model(combo);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            int current = 0;

            gtk_tree_model_get(model, &iter, COLUMN_ID, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(combo, &iter);
                return TRUE;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    return FALSE;
}

/** \brief  Handler for the "changed" event of the integer combo box
 *
 * Updates the resource connected to the combo box
 *
 * \param[in]   self    combo box
 * \param[in]   data    extra event data (unused)
 */
static void on_combo_int_changed(GtkComboBox *self, gpointer data)
{
    mediator_t *mediator;
    int         id = 0;

    mediator = mediator_for_widget(GTK_WIDGET(self));
    if (get_combo_int_id(self, &id)) {
        /* set resource, update state and trigger callback if present */
        if (!mediator_update_int(mediator, id)) {
            /* revert without trigger useless resource update */
            mediator_handler_block(mediator);
            set_combo_int_id(self, mediator_get_current_int(mediator));
            mediator_handler_unblock(mediator);
        }
    }
}


/** \brief  Create a combo box to control an integer resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_int_new(const char *resource,
                                            const vice_gtk3_combo_entry_int_t *entries)
{
    GtkWidget    *view;
    GtkListStore *model;
    mediator_t   *mediator;
    gulong        handler;

    model    = create_combo_int_model(entries);
    view     = create_combo_int_view(model);
    mediator = mediator_new(view, resource, G_TYPE_INT);

    /* set current ID */
    set_combo_int_id(GTK_COMBO_BOX(view), mediator_get_current_int(mediator));
    /* connect handler and register with mediator */
    handler = g_signal_connect(G_OBJECT(view),
                               "changed",
                               G_CALLBACK(on_combo_int_changed),
                               NULL);
    mediator_set_handler(mediator, handler);

    return view;
}


/** \brief  Create a combo box to control an integer resource
 *
 * Allows setting the resource name via sprintf()-syntax
 *
 * \param[in]   fmt     format string for the resource name
 * \param[in]   entries list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_int_new_sprintf(
        const char *fmt,
        const vice_gtk3_combo_entry_int_t *entries,
        ...)
{
    char    resource[256];
    va_list args;

    va_start(args, entries);
    g_vsnprintf(resource, sizeof resource, fmt, args);
    va_end(args);

    return vice_gtk3_resource_combo_int_new(resource, entries);
}


/** \brief  Set new ID for integer combo box
 *
 * Set new ID of the combo box
 *
 * \param[in,out]   widget  integer resource combo box
 * \param[in]       id      new ID for the combo box resource
 *
 * \return  TRUE if the new \a id was set
 */
gboolean vice_gtk3_resource_combo_int_set(GtkWidget *widget, int id)
{
    return set_combo_int_id(GTK_COMBO_BOX(widget), id);
}


/** \brief  Get current ID for integer combo box
 *
 * \param[in]   widget  integer resource combo box
 * \param[out]  id      object to store ID
 *
 * \return  TRUE if the ID was properly read
 */
gboolean vice_gtk3_resource_combo_int_get(GtkWidget *widget, int *id)
{
    return get_combo_int_id(GTK_COMBO_BOX(widget), id);
}


/** \brief  Reset \a widget to its factory default
 *
 * \param[in,out]   widget  integer resource combo box
 *
 * \return  TRUE if the widget was reset to its factory value
 */
gboolean vice_gtk3_resource_combo_int_factory(GtkWidget *widget)
{
    mediator_t *mediator;
    int         factory;

    mediator = mediator_for_widget(widget);
    factory  = mediator_get_factory_int(mediator);
    return set_combo_int_id(GTK_COMBO_BOX(widget), factory);
}


/** \brief  Reset \a widget to its state when it was instanciated
 *
 * \param[in,out]   widget  integer resource combo box
 *
 * \return  bool
 */
gboolean vice_gtk3_resource_combo_int_reset(GtkWidget *widget)
{
    mediator_t *mediator;
    int         initial = 0;

    mediator = mediator_for_widget(widget);
    initial  = mediator_get_initial_int(mediator);
    return vice_gtk3_resource_combo_int_set(widget, initial);
}


/** \brief  Synchronize \a widget with its resource value
 *
 * \param[in,out]   widget  integer resource combo box
 *
 * \return  bool
 */
gboolean vice_gtk3_resource_combo_int_sync(GtkWidget *widget)
{
    mediator_t *mediator;
    int         id = -1;
    int         value;
    gboolean    result = TRUE;

    mediator = mediator_for_widget(widget);
    get_combo_int_id(GTK_COMBO_BOX(widget), &id);
    value = mediator_get_resource_int(mediator);
    if (id != value) {
        mediator_handler_block(mediator);
        result = set_combo_int_id(GTK_COMBO_BOX(widget), value);
        mediator_handler_unblock(mediator);
    }
    return result;
}


/******************************************************************************
 *                      Combo box for string resources                        *
 *                                                                            *
 *      Presents a combo box with strings, and uses strings for keys.         *
 *****************************************************************************/

/** \brief  Create model for combo box with string (id,value) pairs
 *
 * Create model for combo box with column 0 as ID (string) and column 1 as
 * display value (string)).
 *
 * \param[in]   entries list of entries for the model (can be `NULL`)
 *
 * \return  new model
 */
static GtkListStore *combo_str_model_new(const vice_gtk3_combo_entry_str_t *entries)
{
    GtkListStore *model;
    int           index;

    model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    if (entries != NULL) {
        for (index = 0; entries[index].id != NULL; index++) {
            GtkTreeIter iter;

            gtk_list_store_append(model, &iter);
            gtk_list_store_set(model,
                               &iter,
                               COLUMN_ID,    entries[index].id,
                               COLUMN_VALUE, entries[index].value,
                               -1);
        }
    }
    return model;
}

/** \brief  Append row to string resource combo box model
 *
 * \param[in]   model   model
 * \param[in]   id      ID for the row
 * \param[in]   value   display value for the row
 */
static void combo_str_model_append(GtkListStore *model,
                                   const char   *id,
                                   const char   *value)
{
    if (id != NULL && *id != '\0' && value != NULL && *value != '\0') {
        GtkTreeIter iter;

        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                           &iter,
                           COLUMN_ID,    id,
                           COLUMN_VALUE, value,
                           -1);
    } else {
        log_error(LOG_ERR,
                  "%s(): cannot append row, `id` and `value` must both be"
                  " non-NULL and non-empty",
                  __func__);
    }
}

/** \brief  Create combo box for string resources and initialize with model
 *
 * \param[in]   model   model
 *
 * \return  GtkComboBox
 */
static GtkWidget *combo_str_view_new(GtkListStore *model)
{
    GtkWidget       *view;
    GtkCellRenderer *renderer;

    view = gtk_combo_box_new();
    gtk_combo_box_set_model(GTK_COMBO_BOX(view), GTK_TREE_MODEL(model));
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(view), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(view),
                                   renderer,
                                   "text", COLUMN_VALUE,
                                   NULL);
    /* set ID column so we can use gtk_combo_box_get_active_id() (only works
     * for string IDs) */
    gtk_combo_box_set_id_column(GTK_COMBO_BOX(view), COLUMN_ID);
    return view;
}

/** \brief  Handler for the 'changed' event of the string combo box
 *
 * Updates the resource connected to the combo box
 *
 * \param[in]   combo  combo box
 * \param[in]   data   extra event data (unused)
 */
static void on_combo_str_changed(GtkComboBox *self, gpointer data)
{
    mediator_t *mediator;
    const char *combo_id;

    mediator = mediator_for_widget(GTK_WIDGET(self));
    combo_id = gtk_combo_box_get_active_id(self);
    if (combo_id != NULL) {
        /* update resource and mediator state, call callback on success */
        if (!mediator_update_string(mediator, combo_id)) {
            /* failed to set resource, revert widget state */
            mediator_handler_block(mediator);
            gtk_combo_box_set_active_id(self,
                                        mediator_get_current_string(mediator));
            mediator_handler_unblock(mediator);
        }
    }
}


/** \brief  Create a combo box bound to a string resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box or `NULL`
 *
 * \return  GtkComboBox
 */
GtkWidget *vice_gtk3_resource_combo_str_new(
        const char *resource,
        const vice_gtk3_combo_entry_str_t *entries)
{
    GtkListStore *model;
    GtkWidget    *view;
    mediator_t   *mediator;
    gulong        handler;
    const char   *id = NULL;

    model    = combo_str_model_new(entries);
    view     = combo_str_view_new(model);
    mediator = mediator_new(view, resource, G_TYPE_STRING);

    /* set proper ID */
    id = mediator_get_resource_string(mediator);
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(view), id);

    /* connect signal handler and register with mediator */
    handler = g_signal_connect(G_OBJECT(view),
                               "changed",
                               G_CALLBACK(on_combo_str_changed),
                               NULL);
    mediator_set_handler(mediator, handler);

    return view;
}


/** \brief  Create a combo box to control a string resource
 *
 * \param[in]   fmt         resource name, printf-style format string
 * \param[in]   entries     list of entries for the combo box or `NULL`
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_str_new_sprintf(
        const char *fmt,
        const vice_gtk3_combo_entry_str_t *entries,
        ...)
{
    char    resource[256];
    va_list args;

    va_start(args, entries);
    g_vsnprintf(resource, sizeof resource, fmt, args);
    va_end(args);
    return vice_gtk3_resource_combo_str_new(resource, entries);
}


/** \brief  Append row to string resource combo box
 *
 * \param[in]   widget  string resource combo box
 * \param[in]   id      ID for the row
 * \param[in]   value   display value for the row
 *
 * \note    \a id and \a value must both be non-NULL and non-empty.
 */
void vice_gtk3_resource_combo_str_append(GtkWidget  *widget,
                                         const char *id,
                                         const char *value)
{
    GtkListStore *model;

    model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(widget)));
    if (model != NULL) {
        combo_str_model_append(model, id, value);
    }
}


/** \brief  Update string combo box by ID
 *
 * Set new ID of the combo box
 *
 * \param[in,out]   widget  string resource combo box
 * \param[in]       id      new ID of the combo box
 *
 * \return  TRUE if the new \a id was set
 */
gboolean vice_gtk3_resource_combo_str_set(GtkWidget *widget, const char *id)
{
    return gtk_combo_box_set_active_id(GTK_COMBO_BOX(widget), id);
}


/** \brief  Reset string combo box to its factory default
 *
 * \param[in,out]   widget  string resource combo box
 *
 * \return  TRUE if the factory default was set
 */
gboolean vice_gtk3_resource_combo_str_factory(GtkWidget *widget)
{
    mediator_t *mediator;
    const char *factory;

    mediator = mediator_for_widget(widget);
    factory  = mediator_get_factory_string(mediator);
    return gtk_combo_box_set_active_id(GTK_COMBO_BOX(widget), factory);
}


/** \brief  Reset string combo box to its original value
 *
 * Restores the state of the widget as it was when instanciated.
 *
 * \param[in,out]   widget  string resource combo box
 *
 * \return  TRUE if the combo box was reset
 */
gboolean vice_gtk3_resource_combo_str_reset(GtkWidget *widget)
{
    mediator_t *mediator;
    const char *initial;

    mediator = mediator_for_widget(widget);
    initial  = mediator_get_initial_string(mediator);
    return gtk_combo_box_set_active_id(GTK_COMBO_BOX(widget), initial);
}


/** \brief  Synchronize widget with its resource
 *
 * Updates the widget state to what its resource currently is.
 *
 * \param[in,out]   widget  string resource combo box
 *
 * \return  TRUE if the widget was synchronized with its resource
 */
gboolean vice_gtk3_resource_combo_str_sync(GtkWidget *widget)
{
    mediator_t *mediator;
    const char *res_val;
    const char *combo_id;
    gboolean    result = TRUE;

    mediator = mediator_for_widget(widget);
    res_val  = mediator_get_resource_string(mediator);
    combo_id = gtk_combo_box_get_active_id(GTK_COMBO_BOX(widget));
    if (g_strcmp0(res_val, combo_id) != 0) {
        mediator_handler_block(mediator);
        result = gtk_combo_box_set_active_id(GTK_COMBO_BOX(widget), res_val);
        mediator_handler_unblock(mediator);
    }
    return result;
}



/*
 * GtkComboBoxText with free text entry
 *
 * Simple combo box with a text entry that allows entering a string not in the
 * drop down list.
 */

static gboolean combo_with_entry_fix_index(GtkWidget *widget, const char *value);


/** \brief  Handler for the 'destroy' event of the combo box with entry
 *
 * \param[in]   widget  resource combo box
 * \param[in]   unused  extra event data (unused)
 */
static void on_combo_with_entry_destroy(GtkWidget *widget, gpointer unused)
{
    resource_widget_free_string(widget, "ResourceOrig");
}


/** \brief  Handler for the 'changed' event of the combo box with entry
 *
 * Sets the resource to the new value, if it's a value in the drop down list.
 *
 * If the new value doesn't match an entry in the list we depend on the
 * 'focus-out' event to set the resource since each character entered triggers
 * this handler, which would result in setting the resource a lot with
 * (presumably) incomplete text.
 *
 * XXX: Also of note is the fact that manually entering a string that matches
 *      one of the values in the drop down list doesn't result in a valid
 *      index, -1 is still returned.
 *
 * \param[in]   widget  resource combo box
 * \param[in]   unused  extra event data (unused)
 */
static void on_combo_with_entry_changed(GtkWidget *widget, gpointer unused)
{
    gchar *text;
    gint index;

    text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));
    index = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
#if 0
    debug_gtk3("Index = %d, text = '%s'", index, text);
#endif

    if (index >= 0) {
        /* an item from the list was selected, we can accept that */
        const char *resource = resource_widget_get_resource_name(widget);
        resources_set_string(resource, text);
        combo_with_entry_fix_index(widget, text);
    }

    g_free(text);
}


/** \brief  Handler for the 'focus-out' event of the combo box' text entry
 *
 * To avoid setting the resource on each key press, we wait for the focus-out
 * event and then update the resource.
 *
 * \note    We pass \a combo since gtk_widget_get_parent(entry) doesn't return
 *          the combo box, although gtk_bin_get_child(combo) does return the
 *          entry :)
 *
 * \param[in]   entry   entry widget of the combo box
 * \param[in]   unused  event data (unused)
 * \param[in]   combo   parent combo box
 *
 * \return  `FALSE` to propagate events further, Gtk 3.24.31 complains when we
 *          return `TRUE` here (3.24.24 is fine with it)
 */
static gboolean on_combo_with_entry_focus_out_event(GtkEntry *entry,
                                                    GdkEvent *unused,
                                                    gpointer combo)
{
    const char *resource;
    const char *text;

    resource = resource_widget_get_resource_name(GTK_WIDGET(combo));
    text = gtk_entry_get_text(entry);
#if 0
    debug_gtk3("focus-out: setting \"%s\" to '%s'", resource, text);
#endif
    resources_set_string(resource, text);
    combo_with_entry_fix_index(GTK_WIDGET(combo), text);

    return FALSE;   /* always let the event propagate */
}


/** \brief  Handler for the 'key-press' event of the combo box' text entry
 *
 * If the user presses Return or Enter we store the value in the resource.
 *
 * \param[in]   entry   text entry
 * \param[in]   event   event data
 * \param[in]   combo   combo box
 *
 * \return  `FALSE` to propagate event further
 */
static gboolean on_combo_with_entry_key_press_event(GtkEntry *entry,
                                                    GdkEvent *event,
                                                    gpointer combo)
{
    GdkEventKey *keyev = (GdkEventKey *)event;

    if (keyev->type == GDK_KEY_PRESS &&
            (keyev->keyval == GDK_KEY_Return ||
             keyev->keyval == GDK_KEY_KP_Enter)) {
        const char *resource;
        const char *text;

        resource =  resource_widget_get_resource_name(GTK_WIDGET(combo));
        text = gtk_entry_get_text(entry);
        resources_set_string(resource, text);
        combo_with_entry_fix_index(GTK_WIDGET(combo), text);
    }
    return FALSE;
}


/** \brief  Fix combo box index to match value if its present in the list
 *
 * For some reason setting a string in the entry of the combo box that matches
 * a string in the drop down list does not update the list index to match the
 * string, so we brute force that here.
 *
 * \param[in]   widget  combo box with entry
 * \param[in]   value   value to try to match to string in the drop down list
 *
 * \return  `TRUE` when \a value was found in the drop down list
 */
static gboolean combo_with_entry_fix_index(GtkWidget *widget, const char *value)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gulong handler_id;
    int index = 0;
    gboolean found = FALSE;

    /* make sure we don't trigger the 'changed' event handler */
    handler_id = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(widget),
                                                     "ChangedHandlerID"));
    /* when this function is called from the constructor the handler isn't
     * connected yet so handler_id will be 0, which isn't valid for the
     * block/unblock functions */
    if (handler_id > 0) {
        g_signal_handler_block(G_OBJECT(widget), handler_id);
    }

#if 0
    debug_gtk3("Iterating model for value '%s'", value);
#endif
    model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget));
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model), &iter)) {
        do {
            char *text = NULL;

            gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, 0, &text, -1);
#if 0
            debug_gtk3("Checking '%s'", text);
#endif
            if (text != NULL && strcmp(text, value) == 0) {
#if 0
                debug_gtk3("Found index %d", index);
#endif
                gtk_combo_box_set_active(GTK_COMBO_BOX(widget), index);
                g_free(text);
                found = TRUE;
                break;
            }
            g_free(text);
            index++;
        } while (gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &iter));
    }

    if (handler_id > 0) {
        g_signal_handler_unblock(G_OBJECT(widget), handler_id);
    }
    return found;
}


/** \brief  Create combo box with text entry
 *
 * Create a combo box where the user can enter a string not present in the
 * list of \a values.
 *
 * \note    This combo box only works for string-type resources and doesn't
 *          support (key,value) pairs.
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     NULL-terminated list of strings
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_box_with_entry(const char *resource,
                                                   const char **values)

{
    GtkWidget *combo;
    GtkWidget *entry;
    const char *orig = NULL;
    gulong handler_id;

    combo = gtk_combo_box_text_new_with_entry();
    /* keep copy of resource name */
    resource_widget_set_resource_name(combo, resource);
    /* keep copy of resource value at instanciation */
    resources_get_string(resource, &orig);
    resource_widget_set_string(combo, "ResourceOrig", orig);

    /* add entries to the combo box */
    while (*values != NULL) {
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, *values);
        values++;
    }
    /* set the resource's value */
    entry = gtk_bin_get_child(GTK_BIN(combo));
    gtk_entry_set_text(GTK_ENTRY(entry), orig);
    /* fix up index */
    combo_with_entry_fix_index(combo, orig);

    /* now connect the event handlers */
    g_signal_connect_unlocked(combo,
                              "destroy",
                              G_CALLBACK(on_combo_with_entry_destroy),
                              NULL);
    handler_id = g_signal_connect(combo,
                                  "changed",
                                  G_CALLBACK(on_combo_with_entry_changed),
                                  NULL);
    g_object_set_data(G_OBJECT(combo),
                      "ChangedHandlerID",
                      GULONG_TO_POINTER(handler_id));
    /* focus-out must be connected to the child */
    g_signal_connect(entry,
                     "focus-out-event",
                     G_CALLBACK(on_combo_with_entry_focus_out_event),
                     (gpointer)combo);
    /* return/kp-enter to accept text entry */
    g_signal_connect(entry,
                     "key-press-event",
                     G_CALLBACK(on_combo_with_entry_key_press_event),
                     (gpointer)combo);

    return combo;
}


/** \brief  Set new value for the combo box, also setting the resource
 *
 * \param[in]   widget  combo box with entry
 * \param[in]   value   new value for the combo box
 *
 * \return  `TRUE` when setting the resource succeeded
 */
gboolean vice_gtk3_resource_combo_box_with_entry_set(GtkWidget *widget,
                                                     const char *value)
{
    GtkWidget *entry;
    const char *resource;

    entry = gtk_bin_get_child(GTK_BIN(widget));
    resource = resource_widget_get_resource_name(widget);

    gtk_entry_set_text(GTK_ENTRY(entry), value);
    combo_with_entry_fix_index(widget, value);
    return resources_set_string(resource, value) == 0 ? TRUE : FALSE;
}


/** \brief  Get text of the widget
 *
 * \param[in]   widget  combo box
 * \param[out]  value   location to store pointer to text
 *
 * \note    The data returned in \a value must be freed with g_free()
 *
 * \return  TRUE if text isn't `NULL`
 */
gboolean vice_gtk3_resource_combo_box_with_entry_get(GtkWidget *widget,
                                                     gchar **value)
{
    gchar *text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(widget));

    *value = text;
    return text != NULL ? TRUE : FALSE;
}


/** \brief  Set the combo box to the resource's factory value
 *
 * This sets the combo box to the factory value of the connected resource, to
 * reset the combo box to the value the resource had when the combo box was
 * created use vice_gtk3_resource_combo_box_with_entry_reset().
 *
 * \note    This also sets the resource to its factory value.
 *
 * \param[in]   widget  combo box with entry
 *
 * \return  `TRUE` on success
 */
gboolean vice_gtk3_resource_combo_box_with_entry_factory(GtkWidget *widget)
{
    const char *resource;
    const char *factory;

    resource = resource_widget_get_resource_name(widget);
    if (resources_get_default_value(resource, &factory) == 0) {
        return vice_gtk3_resource_combo_box_with_entry_set(widget, factory);
    }
    return FALSE;
}


/** \brief  Reset the combo box to its value at the combo box' creation
 *
 * This sets the combo box to the value of the connected resource at the time
 * the combo box was created, to reset the resource to factory, use
 * vice_gtk3_resource_combo_box_with_entry_factory().
 *
 * \note    This also sets the resource to its original value.
 *
 * \param[in]   widget  combo box with entry
 *
 * \return  `TRUE` on success
 */
gboolean vice_gtk3_resource_combo_box_with_entry_reset(GtkWidget *widget)
{
    const char *orig = resource_widget_get_string(widget, "ResourceOrig");

    return vice_gtk3_resource_combo_box_with_entry_set(widget, orig);
}


/** \brief  Synchronize the combo box with its resource value
 *
 * \param[in]   widget  combo box with entry
 *
 * \return  `TRUE` on success
 */
gboolean vice_gtk3_resource_combo_box_with_entry_sync(GtkWidget *widget)
{
    GtkWidget *entry;
    const char *resource;
    const char *value;
    gulong handler_id;

    resource = resource_widget_get_resource_name(widget);
    if (resources_get_string(resource, &value) != 0) {
        return FALSE;
    }
    /* make sure we don't trigger the changed event handler */
    handler_id = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(widget),
                                                     "ChangedHandlerID"));
    g_signal_handler_block(G_OBJECT(widget), handler_id);
    entry = gtk_bin_get_child(GTK_BIN(widget));
    gtk_entry_set_text(GTK_ENTRY(entry), value);
    combo_with_entry_fix_index(widget, value);
    g_signal_handler_unblock(G_OBJECT(widget), handler_id);
    return TRUE;
}


/******************************************************************************
 *    GtkComboBox for integer resources, displaying values in hexadecimal     *
 *                                                                            *
 * Displays a list, or range, of integer values in hexadecimal notation,      *
 * presenting the values as unsigned 16-bit values in the form '$hhhh'.       *
 *****************************************************************************/

/* model column indexes */
enum {
    COL_HEX_ID,     /**< value */
    COL_HEX_STR     /**< displayed string */
};

/* forward declarations */
static bool get_hex_id(GtkComboBox *self, int *id);
static bool set_hex_id(GtkComboBox *self, int id);


/** \brief  Block the 'changed' signal handler of a hex resource combo box
 *
 * \param[in]   self    hex resource combo box
 */
static void combo_hex_handler_block(GtkComboBox *self)
{
    gulong handler = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(self),
                                                         "ChangedHandlerID"));
    g_signal_handler_block(G_OBJECT(self), handler);
}

/** \brief  Unblock the 'changed' signal handler of a hex resource combo box
 *
 * \param[in]   self    hex resource combo box
 */
static void combo_hex_handler_unblock(GtkComboBox *self)
{
    gulong handler = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(self),
                                                         "ChangedHandlerID"));
    g_signal_handler_unblock(G_OBJECT(self), handler);
}

/** \brief  Handler for the 'changed' event of a hex resource combo box
 *
 * Attempt to set the resource of \a self to the new value, revert to previous
 * value if fail.
 *
 * \param[in]   self    hex resource combo box
 * \param[in]   data    extra event data (unused)
 */
static void on_combo_hex_changed(GtkComboBox *self, gpointer data)
{
    int id = 0;

    if (get_hex_id(self, &id)) {
        const char *resource;

        resource = resource_widget_get_resource_name(GTK_WIDGET(self));
        debug_gtk3("Setting %s to $%04x", resource, (unsigned int)id);
        if (resources_set_int(resource, id) < 0) {
            /* failed, restore previous value */
            int previous = resource_widget_get_int(GTK_WIDGET(self),
                                                   "PreviousID");
            debug_gtk3("Failed: restoring combo to previous value $%04x",
                       (unsigned int)previous);
            combo_hex_handler_block(self);
            set_hex_id(self, previous);
            combo_hex_handler_unblock(self);
        } else {
            /* success, update previous ID */
            resource_widget_set_int(GTK_WIDGET(self), "PreviousID", id);
        }
    }
}

/** \brief  Get current ID of hex resource combo box
 *
 * \param[in]   self    hex resource combo box
 * \param[out]  id      current ID
 *
 * \return  `true` on success (active selection in \a self)
 */
static bool get_hex_id(GtkComboBox *self, int *id)
{
    GtkTreeIter iter;

    if (gtk_combo_box_get_active_iter(self, &iter)) {
        gtk_tree_model_get(gtk_combo_box_get_model(self),
                           &iter,
                           COL_HEX_ID, id,
                           -1);
        return true;
    }
    *id = -1;
    return false;
}

/** \brief  Set ID of hex resource combo box
 *
 * \param[in]   self    hex resource combo box
 * \param[out]  id      new ID
 *
 * \return  `true` on success (\a id was valid for \a self)
 */
static bool set_hex_id(GtkComboBox *self, int id)
{
    GtkTreeModel *model;
    GtkTreeIter   iter;

    model = gtk_combo_box_get_model(self);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            int current = 0;

            gtk_tree_model_get(model, &iter, COL_HEX_ID, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(self, &iter);
                return true;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    return false;
}

/** \brief  Create empty model
 *
 * Create empty model with one integer column and one string column.
 *
 * \return  empty model
 */
static GtkListStore *create_hex_model_base(void)
{
    return gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
}

/** \brief  Add value to model
 *
 * Add \a value as integer ID and as a hex string (prefixed with '$') to be
 * displayed.
 *
 * \param[in]   model   hex resource combo model
 * \param[in]   value   value for \a model
 */
static void add_hex_model_value(GtkListStore *model, int value)
{
    GtkTreeIter iter;
    char        hexstr[64];

    g_snprintf(hexstr, sizeof hexstr, "$%04x", (unsigned int)value);
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model,
                       &iter,
                       COL_HEX_ID, value,
                       COL_HEX_STR, hexstr,
                       -1);
}

/** \brief  Create model from list of integer values
 *
 * Create hex combo model from values in \a list.
 *
 * \param[in]   list    values, terminated with <0
 *
 * \return  populated model
 */
static GtkListStore *create_hex_model_list(const int *list)
{
    GtkListStore *model;
    int           index = 0;

    model = create_hex_model_base();
    while (list[index] >= 0) {
        add_hex_model_value(model, list[index]);
        index++;
    }
    return model;
}

/** \brief  Create model from range of integer values
 *
 * Create hex combo model with values starting with \a lower up to, but excluding,
 * \a upper, incrementing with \a step.
 *
 * \param[in]   list    values, terminated with <0
 *
 * \return  populated model
 */
static GtkListStore *create_hex_model_range(int lower,
                                            int upper,
                                            int step)
{
    GtkListStore *model;
    int           value;

    model = create_hex_model_base();
    for (value = lower; value < upper; value += step) {
        add_hex_model_value(model, value);
    }
    return model;
}

/** \brief  Create view for a hex resource combo
 *
 * Create combo box for \a model.
 *
 * \param[in]   model   model for the combo box
 *
 * \return  GtkComboBox
 */
static GtkWidget *create_hex_view(GtkListStore *model)
{
    GtkWidget       *view;
    GtkCellRenderer *renderer;

    /* create combo box with text renderer */
    view = gtk_combo_box_new();
    gtk_combo_box_set_model(GTK_COMBO_BOX(view), GTK_TREE_MODEL(model));
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(view), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(view),
                                   renderer,
                                   "text", COL_HEX_STR,
                                   NULL);
    return view;
}

/** \brief  Helper to create hex resource combo box
 *
 * \param[in]   resource    resource name
 * \param[in]   model       model for the combo box
 *
 * \return  GtkComboBox
 */
static GtkWidget *combo_hex_helper(const char   *resource,
                                   GtkListStore *model)
{
    GtkWidget    *combo;
    gulong        handler_id;
    int           id = 0;

    combo = create_hex_view(model);

    /* set current value, resource name and "previous" value for recovery
     * from errors setting the resource */
    resources_get_int(resource, &id);
    debug_gtk3("resource %s = $%04x", resource, (unsigned int)id);
    set_hex_id(GTK_COMBO_BOX(combo), id);
    resource_widget_set_resource_name(combo, resource);
    resource_widget_set_int(combo, "ResourceOrig", id);
    resource_widget_set_int(combo, "PreviousID", id);

    /* set up signal handlers */
    handler_id = g_signal_connect(G_OBJECT(combo),
                                  "changed",
                                  G_CALLBACK(on_combo_hex_changed),
                                  NULL);
    g_object_set_data(G_OBJECT(combo),
                      "ChangedHandlerID",
                      GULONG_TO_POINTER(handler_id));
    return combo;
}


/******************************************************************************
 *                      Hex resource combo box - Public API                   *
 *****************************************************************************/

/** \brief  Create resource-bound combo box showing hex literals
 *
 * Create combo box bound to an integer resource that shows a hex string in
 * the form '$hhhh' for each value in \a list.
 *
 * \param[in]   resource    resource name
 * \param[in]   list        list of values, terminate with <0
 *
 * \return  GtkComboBox
 */
GtkWidget *vice_gtk3_resource_combo_hex_list_new(const char *resource,
                                                 const int  *list)
{
    return combo_hex_helper(resource, create_hex_model_list(list));
}


/** \brief  Create resource-bound combo box showing hex literals
 *
 * Create combo box bound to an integer resource that shows a hex string in
 * the form '$hhhh' for each value in range \a lower to \a upper (exclusive).
 *
 * \param[in]   resource    resource name
 * \param[in]   lower       range starting value (inclusive)
 * \param[in]   upper       range ending value (exclusive)
 * \param[in]   step        increment of value
 *
 * \return  GtkComboBox
 */
GtkWidget *vice_gtk3_resource_combo_hex_range_new(const char *resource,
                                                  int         lower,
                                                  int         upper,
                                                  int         step)
{
    if (upper <= lower) {
        log_error(LOG_ERR, "%s(): invalid range %d-%d", __func__, lower, upper);
        return NULL;
    }
    if (step < 1) {
        log_error(LOG_ERR, "%s(): invalid step value %d", __func__, step);
        return NULL;
    }
    return combo_hex_helper(resource, create_hex_model_range(lower, upper, step));
}


/** \brief  Set hex resource combo box ID, triggering resource update
 *
 * \param[in]   combo   hex resource combo box
 * \param[in]   id      new ID for \a combo
 *
 * \return  `TRUE` if the combo box ID and resource value could be set
 */
gboolean vice_gtk3_resource_combo_hex_set(GtkWidget *combo, int id)
{
    return set_hex_id(GTK_COMBO_BOX(combo), id) ? TRUE : FALSE;
}


/** \brief  Set hex resource combo box ID, without triggering resource update
 *
 * \param[in]   combo   hex resource combo box
 * \param[in]   id      new ID for \a combo
 *
 * \return  `TRUE` if the combo box ID could be set
 */
gboolean vice_gtk3_resource_combo_hex_set_blocked(GtkWidget *combo, int id)
{
    bool result;

    combo_hex_handler_block(GTK_COMBO_BOX(combo));
    result = set_hex_id(GTK_COMBO_BOX(combo), id);
    combo_hex_handler_unblock(GTK_COMBO_BOX(combo));
    return result ? TRUE : FALSE;
}


/** \brief  Get hex resource combo box ID
 *
 * \param[in]   combo   hex resource combo box
 * \param[out]  id      current ID of \a combo (set to -1 on failure)
 *
 * \return  `TRUE` if the combo box had a valid selection
 */
gboolean vice_gtk3_resource_combo_hex_get(GtkWidget *combo, int *id)
{
    return get_hex_id(GTK_COMBO_BOX(combo), id) ? TRUE : FALSE;
}


/** \brief  Reset hex combo box to the resource factory value
 *
 * Set combo box and the bound resource to the resource factory value.
 *
 * \param[in]   combo
 *
 * \return  `TRUE` if the combo box and the resource could be set
 */
gboolean vice_gtk3_resource_combo_hex_factory(GtkWidget *combo)
{
    const char *resource;
    int         factory = -1;

    resource = resource_widget_get_resource_name(combo);
    if (resources_get_default_value(resource, &factory) < 0) {
        return FALSE;
    }
    return vice_gtk3_resource_combo_hex_set(combo, factory);
}


/** \brief  Reset hex combo box to its value on instanciation
 *
 * Reset \a combo to the value the resource had when the \a combo was created.
 *
 * \param[in]   combo
 *
 * \return  `TRUE` if the combo box and the resource could be set
 */
gboolean vice_gtk3_resource_combo_hex_reset(GtkWidget *combo)
{
    int orig = resource_widget_get_int(combo, "ResourceOrig");
    return vice_gtk3_resource_combo_hex_set(combo, orig);
}


/** \brief  Synchronized hex combo box with its resource
 *
 * Set \a combo to the current value of its resource, without triggering a
 * resource-set operation.
 *
 * \return  `TRUE` if the combo box could be synchronized with its resource
 */
gboolean vice_gtk3_resource_combo_hex_sync(GtkWidget *combo)
{
    const char *resource;
    int         current = -1;

    resource = resource_widget_get_resource_name(combo);
    if (resources_get_int(resource, &current) < 0) {
        return FALSE;
    }
    return vice_gtk3_resource_combo_hex_set_blocked(combo, current);
}
