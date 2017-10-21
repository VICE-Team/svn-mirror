/** \file   src/arch/gtk3/widgets/base/resourceradiogroup.c
 * \brief   Group of radio buttons controlling a resource
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

#include "basewidget_types.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"

#include "resourceradiogroup.h"


/** \brief  Handler for the "destroy" event of the widget
 *
 * Frees the heap-allocated copy of the resource name
 *
 * \param[in]   widget      radiogroup widget
 * \param[in]   user_data   extra event data (unused)
 */
static void on_radiogroup_destroy(GtkWidget *widget, gpointer user_data)
{
    char *resource;

    resource = (char *)g_object_get_data(G_OBJECT(widget), "ResourceName");
    lib_free(resource);
}


/** \brief  Handler for the "toggled" event of a radio button
 *
 * \param[in]   radio       radio button triggering the event
 * \param[in]   user_data   new value for the radio group (`int`)
 */
static void on_radio_toggled(GtkWidget *radio, gpointer user_data)
{
    GtkWidget *parent;
    int old_val;
    int new_val;
    char *resource;
    void (*callback)(int);

    /* parent widget (grid) contains the "ResourceName" property */
    parent = gtk_widget_get_parent(radio);
    resource = (char *)g_object_get_data(G_OBJECT(parent), "ResourceName");
    /* get new and old values */
    resources_get_int(resource, &old_val);
    new_val = GPOINTER_TO_INT(user_data);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))
            && (old_val != new_val)) {
        debug_gtk3("setting %s to %d\n", resource, new_val);
        resources_set_int(resource, new_val);

        callback = g_object_get_data(G_OBJECT(parent), "ExtraCallback");
        if (callback != NULL) {
            callback(new_val);
        }
    }
}


/** \brief  Create a radiogroup widget
 *
 * A radiogroup widget is a composite widget consisting of a GtkGrid with
 * a number of GtkRadioButton's, laid out according to \a orientation
 *
 * Three GObject properties are set: "ResourceName", a heap-allocated copy
 * of the \a resource argument, "Entries", a reference to the \a entries and
 * "Orientation", the \a orientation argument. These are all required to make
 * the event handlers and resource_radiogroup_update() work properly.
 *
 * \param[in]   resource    resource name
 * \param[in]   entries     list of entries for the group
 * \param[in]   orientation layout direction of the radio buttons
 *
 * \return  GtkGrid
 */
GtkWidget *resource_radiogroup_create(const char *resource,
                                      const ui_radiogroup_entry_t *entries,
                                      GtkOrientation orientation)
{
    GtkWidget *grid;
    GtkRadioButton *last = NULL;
    GSList *group = NULL;
    int i;
    int current;

    grid = gtk_grid_new();

    resources_get_int(resource, &current);

    /* store a copy of the resource name in the object */
    g_object_set_data(G_OBJECT(grid), "ResourceName",
            (gpointer)lib_stralloc(resource));
    /* store a reference to the entries in the object */
    g_object_set_data(G_OBJECT(grid), "Entries", (gpointer)entries);
    /* store the orientation in the object */
    g_object_set_data(G_OBJECT(grid), "Orientation",
            GINT_TO_POINTER(orientation));

    /* create radio buttons */
    for (i = 0; entries[i].name != NULL; i++) {
        GtkWidget *radio;

        radio = gtk_radio_button_new_with_label(group, entries[i].name);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);

        if (current == entries[i].id) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        }
        g_signal_connect(radio, "toggled", G_CALLBACK(on_radio_toggled),
                GINT_TO_POINTER(entries[i].id));

        if (orientation == GTK_ORIENTATION_HORIZONTAL) {
            gtk_grid_attach(GTK_GRID(grid), radio, i, 0, 1, 1);
        } else {
            gtk_grid_attach(GTK_GRID(grid), radio, 0, i, 1, 1);
        }

        last = GTK_RADIO_BUTTON(radio);
    }

    g_signal_connect(grid, "destroy", G_CALLBACK(on_radiogroup_destroy), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update \a widget with \a id
 *
 * \param[in,out]   widget  radiogroup widget
 * \param[in]       id      new value for widget
 */
void resource_radiogroup_update(GtkWidget *widget, int id)
{
    int orientation;
    int index;
    ui_radiogroup_entry_t *entries;
    GtkWidget *radio;

    orientation = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget),
                "Orientation"));
    entries = (ui_radiogroup_entry_t *)(g_object_get_data(G_OBJECT(widget),
                "Entries"));

    for (index = 0; entries[index].name != NULL; index++) {
        if (entries[index].id == id) {
            if (orientation == GTK_ORIENTATION_VERTICAL) {
                radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, index);
            } else {
                radio = gtk_grid_get_child_at(GTK_GRID(widget), index, 0);
            }
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
            break;
        }
    }
}


/** \brief  Add an extra callback to \a widget
 *
 * This callback should allow widgets interacting with other widgets without
 * usingany global references.
 *
 * The widget returned is the actual radio group GtkGrid, the integer value is
 * the curent IID of the radiogroup's currently selected radio button.
 *
 * \param[in,out]   widget      radiogroup widget
 * \param[in]       callback    function to call when the radiogroup selection
 *                              changes
 */
void resource_radiogroup_add_callback(GtkWidget *widget,
                                      void (*callback)(GtkWidget *, int))
{
    g_object_set_data(G_OBJECT(widget), "ExtraCallback", (gpointer)callback);
}
