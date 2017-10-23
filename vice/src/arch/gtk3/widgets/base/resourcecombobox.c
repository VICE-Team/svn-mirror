/** \file   src/arch/gtk3/widgets/base/resourcecombobox.c
 * \brief   Combo boxes connected to a resource
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
 *
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"
#include "basewidget_types.h"

#include "resourcecombobox.h"


/*****************************************************************************
 *                      Combo box for integer resources                      *
 ****************************************************************************/

/** \brief  Handler for the "destroy" event of the integer combo box
 *
 * Frees the heap-allocated copy of the resource name
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_int_destroy(GtkComboBoxText *combo, gpointer user_data)
{
    char *resource = (char *)g_object_get_data(G_OBJECT(combo), "ResourceName");
    lib_free(resource);
}


/** \brief  Handler for the "changed" event of the integer combo box
 *
 * Updates the resource connected to the combo box
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_int_changed(GtkComboBoxText *combo, gpointer user_data)
{
    const char *id_str;
    int id_val;
    char *endptr;
    const char *resource;

    resource = (const char *)g_object_get_data(G_OBJECT(combo), "ResourceName");
    id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    id_val = strtol(id_str, &endptr, 10);
    if (*endptr == '\0') {
        debug_gtk3("setting %s to %d\n", resource, id_val);
        resources_set_int(resource, id_val);
    }
}


/** \brief  Create a combo box to control an integer resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *resource_combo_box_int_create(const char *resource,
                                         const ui_combo_entry_int_t *entries)
{
    GtkWidget *combo;
    int index;
    int current;

    combo = gtk_combo_box_text_new();

    /* store a heap-allocated copy of the resource name in the object */
    g_object_set_data(G_OBJECT(combo), "ResourceName",
            (gpointer)lib_stralloc(resource));

    /* get current value of resource */
    resources_get_int(resource, &current);

    /* add entries */
    for (index = 0; entries[index].name != NULL; index++) {
        char id_str[80];    /* enough space for a 256-bit int decimal digits */
        g_snprintf(id_str, 80, "%d", entries[index].id);

        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                id_str, entries[index].name);
        if (current == entries[index].id) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), index);
        }
    }

    /* connect signal handlers */
    g_signal_connect(combo, "changed", G_CALLBACK(on_combo_int_changed), NULL);
    g_signal_connect(combo, "destroy", G_CALLBACK(on_combo_int_destroy), NULL);

    gtk_widget_show(combo);
    return combo;
}


/** \brief  Create combo box for integer \a resource with a \a label
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 * \param[in]   label       text for the label
 *
 * \return  GtkGrid
 */
GtkWidget *resource_combo_box_int_create_with_label(
        const char *resource,
        const ui_combo_entry_int_t *entries,
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

    combo = resource_combo_box_int_create(resource, entries);
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
void resource_combo_box_int_update(GtkWidget *widget, int id)
{
    GtkWidget *combo;
    char id_str[80];

    g_snprintf(id_str, 80, "%d", id);

    if (GTK_IS_GRID(widget)) {
        /* widget is a grid with label & combo */
        combo = gtk_grid_get_child_at(GTK_GRID(widget), 1, 0);
    } else {
        combo = widget;
    }
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(combo), id_str);
}


/** \brief  Reset integer combo box to its factory default
 *
 * \param[in,out]   widget  integer combo box
 */
void resource_combo_box_int_reset(GtkWidget *widget)
{
    const char *resource;
    int value;

    resource = (const char*)g_object_get_data(G_OBJECT(widget), "ResourceName");
    resources_get_default_value(resource, &value);
    debug_gtk3("resetting %s to factory value %d\n", resource, value);
    resource_combo_box_int_update(widget, value);
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
static void on_combo_str_destroy(GtkComboBoxText *combo, gpointer user_data)
{
    char *resource = (char *)g_object_get_data(G_OBJECT(combo), "ResourceName");
    lib_free(resource);
}


/** \brief  Handler for the "changed" event of the string combo box
 *
 * Updates the resource connected to the combo box
 *
 * \param[im]   combo       combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_combo_str_changed(GtkComboBoxText *combo, gpointer user_data)
{
    const char *id_str;
    const char *resource;

    resource = (const char *)g_object_get_data(G_OBJECT(combo), "ResourceName");
    id_str = gtk_combo_box_get_active_id(GTK_COMBO_BOX(combo));
    debug_gtk3("setting %s to '%s'\n", resource, id_str);
    resources_set_string(resource, id_str);
}


/** \brief  Create a combo box to control a string resource
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 *
 * \return  GtkComboBoxText
 */
GtkWidget *resource_combo_box_str_create(const char *resource,
                                         const ui_combo_entry_str_t *entries)
{
    GtkWidget *combo;
    int index;
    const char *current;

    combo = gtk_combo_box_text_new();

    /* store a heap-allocated copy of the resource name in the object */
    g_object_set_data(G_OBJECT(combo), "ResourceName",
            (gpointer)lib_stralloc(resource));

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


/** \brief  Create combo box for string \a resource with a \a label
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the combo box
 * \param[in]   label       text for the label
 *
 * \return  GtkGrid
 */
GtkWidget *resource_combo_box_str_create_with_label(
        const char *resource,
        const ui_combo_entry_str_t *entries,
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

    combo = resource_combo_box_str_create(resource, entries);
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
void resource_combo_box_str_update(GtkWidget *widget, const char *id)
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
void resource_combo_box_str_reset(GtkWidget *widget)
{
    const char *resource;
    const char *value;

    resource = (const char*)g_object_get_data(G_OBJECT(widget), "ResourceName");
    resources_get_default_value(resource, &value);
    debug_gtk3("resetting %s to factory value '%s'\n", resource, value);
    resource_combo_box_str_update(widget, value);
}


