/**
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
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "basewidget_types.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "log.h"
#include "resourcehelpers.h"
#include "resources.h"

#include "resourcecombobox.h"


/*****************************************************************************
 *                      Combo box for integer resources                      *
 ****************************************************************************/

/** \brief  Create a model for a combo box with int's as ID's
 *
 * \param[in]   list    list of options
 *
 * \return  model
 */
static GtkListStore *create_combo_int_model(const vice_gtk3_combo_entry_int_t *list)
{
    GtkListStore *model;
    GtkTreeIter iter;
    int i;

    model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
    if (list == NULL) {
        return model;
    }
    for (i = 0; list[i].name != NULL; i++) {
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter,
                0, list[i].name,    /* item name */
                1, list[i].id,      /* item ID */
                -1);
    }
    return model;
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
static bool get_combo_int_id(GtkComboBox *combo, int *id)
{
    GtkTreeModel *model;
    GtkTreeIter iter;


    if (gtk_combo_box_get_active(combo) >= 0) {
        model = gtk_combo_box_get_model(combo);
        if (gtk_combo_box_get_active_iter(combo, &iter)) {
            gtk_tree_model_get(model, &iter, 1, id, -1);
            return true;
        }
    }
    return false;
}


/** \brief  Set ID of \a combo to \a id
 *
 * \param[in,out]   combo   combo box
 * \param[in]       id      ID for \a combo
 *
 * \return  boolean
 */
static bool set_combo_int_id(GtkComboBox *combo, int id)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(combo);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            int current;

            gtk_tree_model_get(model, &iter, 1, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(combo, &iter);
                return true;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }
    debug_gtk3("ID %d not found\n", id);
    return false;
}


/** \brief  Handler for the "destroy" event of the integer combo box
 *
 * Frees the heap-allocated copy of the resource name
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_int_destroy(GtkWidget *combo, gpointer user_data)
{
    resource_widget_free_resource_name(combo);
}


/** \brief  Handler for the "changed" event of the integer combo box
 *
 * Updates the resource connected to the combo box
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_int_changed(GtkComboBox *combo, gpointer user_data)
{
    int id;
    const char *resource;

    resource = resource_widget_get_resource_name(GTK_WIDGET(combo));
    if (get_combo_int_id(combo, &id)) {
        debug_gtk3("setting %s to %d\n", resource, id);
        if (resources_set_int(resource, id) < 0) {
            log_error(LOG_ERR, "failed to set resource '%s' to %d\n",
                    resource, id);
        }
    } else {
        log_error(LOG_ERR, "failed to get ID for resource '%s'\n", resource);
    }
}


/** \brief  Create a combo box to control an integer resource
 *
 * \param[in]   combo   combo box
 * \param[in]   entries list of entries for the combo box
 *
 * \return  GtkComboBox
 */
static GtkWidget *resource_combo_box_int_create_helper(
        GtkWidget *combo,
        const vice_gtk3_combo_entry_int_t *entries)
{
    GtkListStore *model;
    GtkCellRenderer *renderer;
    const char *resource;
    int current;

    /* setup combo box with model and renderers */
    model = create_combo_int_model(entries);
    gtk_combo_box_set_model(GTK_COMBO_BOX(combo), GTK_TREE_MODEL(model));
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), renderer,
            "text", 0, NULL);

    /* set current ID */
    resource = resource_widget_get_resource_name(combo);
    if (resources_get_int(resource, &current) < 0) {
        /* couldn't read resource */
        log_error(LOG_ERR,
                "failed to get value for resource %s, "
                "reverting to the first entry\n",
                resource);
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    } else if (!set_combo_int_id(GTK_COMBO_BOX(combo), current)) {
        /* failed to set ID, revert to first entry */
        log_error(LOG_ERR,
                "failed to set ID to %d, reverting to the first entry\n",
                current);
        gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
    }

    /* connect signal handlers */
    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_int_changed), NULL);
    g_signal_connect(combo, "destroy", G_CALLBACK(on_combo_int_destroy), NULL);

    gtk_widget_show(combo);
    return combo;
}


/** \brief  Create a combo box to control an integer resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_box_int_create(const char *resource,
                                         const vice_gtk3_combo_entry_int_t *entries)
{
    GtkWidget * combo = gtk_combo_box_new();

    /* store a heap-allocated copy of the resource name in the object */
    resource_widget_set_resource_name(combo, resource);

    return resource_combo_box_int_create_helper(combo, entries);
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
GtkWidget *vice_gtk3_resource_combo_box_int_create_sprintf(
        const char *fmt,
        const vice_gtk3_combo_entry_int_t *entries,
        ...)
{
    GtkWidget *combo;
    char *resource;
    va_list args;

    combo = gtk_combo_box_new();

    va_start(args, entries);
    resource = lib_mvsprintf(fmt, args);
    g_object_set_data(G_OBJECT(combo), "ResourceName", (gpointer)resource);
    va_end(args);

    return resource_combo_box_int_create_helper(combo, entries);
}



/** \brief  Create combo box for integer \a resource with a \a label
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 * \param[in]   label       text for the label
 *
 * \return  GtkGrid
 */
GtkWidget *vice_gtk3_resource_combo_box_int_create_with_label(
        const char *resource,
        const vice_gtk3_combo_entry_int_t *entries,
        const char *label)
{
    GtkWidget *grid;
    GtkWidget *lbl;
    GtkWidget *combo;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    lbl = gtk_label_new(label);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), lbl, 0, 0, 1, 1);

    combo = vice_gtk3_resource_combo_box_int_create(resource, entries);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}




/** \brief  Update integer combo box by ID
 *
 * Set new ID of the combo box
 *
 * \param[in,out]   combo   combo box
 * \param[in]       id      new ID of the combo box
 */
void vice_gtk3_resource_combo_box_int_update(GtkWidget *widget, int id)
{
    GtkWidget *combo;

    if (GTK_IS_GRID(widget)) {
        /* widget is a grid with label & combo */
        combo = gtk_grid_get_child_at(GTK_GRID(widget), 1, 0);
    } else {
        combo = widget;
    }
    if (GTK_IS_COMBO_BOX(combo)) {
        set_combo_int_id(GTK_COMBO_BOX(combo), id);
    }
}


/** \brief  Reset integer combo box to its factory default
 *
 * \param[in,out]   widget  integer combo box
 */
void vice_gtk3_resource_combo_box_int_reset(GtkWidget *widget)
{
    const char *resource;
    int value;

    resource = resource_widget_get_resource_name(widget);
    resources_get_default_value(resource, &value);
    debug_gtk3("resetting %s to factory value %d\n", resource, value);
    vice_gtk3_resource_combo_box_int_update(widget, value);
}


/*****************************************************************************
 *                      Combo box for string resources                       *
 ****************************************************************************/

/** \brief  Handler for the "destroy" event of the combo box
 *
 * Frees the heap-allocated copy of the resource name
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_str_destroy(GtkWidget *combo, gpointer user_data)
{
    resource_widget_free_resource_name(combo);
}


/** \brief  Handler for the "changed" event of the string combo box
 *
 * Updates the resource connected to the combo box
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_str_changed(GtkWidget *combo, gpointer user_data)
{
    const char *id_str;
    const char *resource;

    resource = resource_widget_get_resource_name(combo);
    id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    debug_gtk3("setting %s to '%s'\n", resource, id_str);
    resources_set_string(resource, id_str);
}


/** \brief  Create a combo box to control a string resource
 *
 * \param[in]   combo   combo box
 * \param[in]   entries list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
static GtkWidget *resource_combo_box_str_create_helper(
        GtkWidget *combo,
        const vice_gtk3_combo_entry_str_t *entries)
{
    int index;
    const char *current;
    const char *resource;

    resource = resource_widget_get_resource_name(combo);

    /* store a heap-allocated copy of the resource name in the object */
    resource_widget_set_resource_name(combo, resource);

    /* get current value of resource */
    resources_get_string(resource, &current);

    /* add entries */
    for (index = 0; entries[index].name != NULL; index++) {
        const char *id_str;

        if (entries[index].id == NULL) {
            id_str = entries[index].name;
        } else {
            id_str = entries[index].id;
        }

        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                id_str, entries[index].name);
        if (strcmp(current, id_str) == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), index);
        }
    }

    /* connect signal handlers */
    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_str_changed), NULL);
    g_signal_connect(combo, "destroy", G_CALLBACK(on_combo_str_destroy), NULL);

    gtk_widget_show(combo);
    return combo;
}


/** \brief  Create a combo box to control a string resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_box_str_create(const char *resource,
                                         const vice_gtk3_combo_entry_str_t *entries)
{
    GtkWidget *combo;

    combo = gtk_combo_box_text_new();

    /* store a heap-allocated copy of the resource name in the object */
    resource_widget_set_resource_name(combo, resource);

    return resource_combo_box_str_create_helper(combo, entries);
}


/** \brief  Create a combo box to control a string resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *vice_gtk3_resource_combo_box_str_create_sprintf(
        const char *fmt,
        const vice_gtk3_combo_entry_str_t *entries,
        ...)
{
    GtkWidget *combo;
    char *resource;
    va_list args;

    combo = gtk_combo_box_text_new();

    va_start(args, entries);
    resource = lib_mvsprintf(fmt, args);
    g_object_set_data(G_OBJECT(combo), "ResourceName", (gpointer)resource);
    va_end(args);

    return resource_combo_box_str_create_helper(combo, entries);
}


/** \brief  Create combo box for string \a resource with a \a label
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 * \param[in]   label       text for the label
 *
 * \return  GtkGrid
 */
GtkWidget *vice_gtk3_resource_combo_box_str_create_with_label(
        const char *resource,
        const vice_gtk3_combo_entry_str_t *entries,
        const char *label)
{
    GtkWidget *grid;
    GtkWidget *lbl;
    GtkWidget *combo;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    lbl = gtk_label_new(label);
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), lbl, 0, 0, 1, 1);

    combo = vice_gtk3_resource_combo_box_str_create(resource, entries);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update string combo box by ID
 *
 * Set new ID of the combo box
 *
 * \param[in,out]   combo   combo box
 * \param[in]       id      new ID of the combo box
 */
void vice_gtk3_resource_combo_box_str_update(GtkWidget *widget, const char *id)
{
    GtkWidget *combo;

    if (GTK_IS_GRID(widget)) {
        combo = gtk_grid_get_child_at(GTK_GRID(widget), 1, 0);
    } else {
        combo = widget;
    }
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(combo), id);
}


/** \brief  Reset string combo box to its factory default
 *
 * \param[in,out]   widget  string combo box
 */
void vice_gtk3_resource_combo_box_str_reset(GtkWidget *widget)
{
    const char *resource;
    const char *value;

    resource = resource_widget_get_resource_name(widget);
    resources_get_default_value(resource, &value);
    debug_gtk3("resetting %s to factory value '%s'\n", resource, value);
    vice_gtk3_resource_combo_box_str_update(widget, value);
}
