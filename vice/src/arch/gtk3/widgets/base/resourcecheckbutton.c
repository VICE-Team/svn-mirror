/** \file   src/arch/gtk3/widgets/base/resourcecheckbutton.c
 * \brief   Check button connected to a resource
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

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"

#include "resourcecheckbutton.h"


/** \brief  Handler for the "destroy" event of the check button
 *
 * Frees the heap-allocated copy of the resource name.
 *
 * \param[in]   check       check button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_check_button_destroy(GtkWidget *check, gpointer user_data)
{
    char *resource;

    resource = (char *)(g_object_get_data(G_OBJECT(check), "ResourceName"));
    lib_free(resource);
}


/** \brief  Handler for the "toggled" event of the check button
 *
 * \param[in]   check       check button
 * \param[in]   user_data   resource name
 */
static void on_check_button_toggled(GtkWidget *check, gpointer user_data)
{
    const char *resource;
    int state;
    int current;

    resource = (const char *)g_object_get_data(G_OBJECT(check), "ResourceName");
    state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
    resources_get_int(resource, &current);

    /* make sure we don't update a resource when the UI happens to be out of
     * sync for some reason */
    if (state != current) {
        debug_gtk3("setting %s to %s\n", resource, state ? "True": "False");
        resources_set_int(resource, state ? 1 : 0);
    }
}


/** \brief  Create check button to toggle \a resource
 *
 * Creates a check button to toggle \a resource. Makes a heap-allocated copy
 * of the resource name so initializing this widget with a constructed/temporary
 * resource name works as well.
 *
 * \param[in]   resource    resource name
 * \param[in]   label       label of the check button
 *
 * \note    The resource name is stored in the "ResourceName" property.
 *
 * \return  new check button
 */
GtkWidget *resource_check_button_create(const char *resource,
                                        const char *label)
{
    GtkWidget *check;
    int state;

    /* get current resource value */
    resources_get_int(resource, &state);

    check = gtk_check_button_new_with_label(label);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
            state ? TRUE : FALSE);

    /* make a copy of the resource name and store the pointer in the propery
     * "ResourceName" */
    g_object_set_data(G_OBJECT(check), "ResourceName",
            (gpointer)lib_stralloc(resource));

    g_signal_connect(check, "toggled", G_CALLBACK(on_check_button_toggled),
            (gpointer)resource);
    g_signal_connect(check, "destroy", G_CALLBACK(on_check_button_destroy),
            NULL);

    gtk_widget_show(check);
    return check;
}


void resource_check_button_update(GtkWidget *check, gboolean value)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), value);
}


void resource_check_button_reset(GtkWidget *check)
{
    const char *resource;
    int value;

    resource = (const char*)g_object_get_data(G_OBJECT(check), "ResourceName");
    resources_get_default_value(resource, &value);
    debug_gtk3("resetting %s to factory value %s\n",
            resource, value ? "True" : "False");
    resource_check_button_update(check, (gboolean)value);
}
