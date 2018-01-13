/** \file   src/arch/gtk3/widgets/base/resourceentry.c
 * \brief   Text entry connected to a resource
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
#include <string.h>

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"
#include "resourcehelpers.h"

#include "resourceentry.h"


/** \brief  Handler for the "destroy" event of the entry
 *
 * Frees the heap-allocated copy of the resource name.
 *
 * \param[in]   entry       entry
 * \param[in]   user_data   extra event data (unused)
 */
static void on_entry_destroy(GtkWidget *entry, gpointer user_data)
{
    resource_widget_free_resource_name(entry);
}


/** \brief  Handler for the "changed" event of the check button
 *
 * \param[in]   entry       entry
 * \param[in]   user_data   unused
 */
static void on_entry_changed(GtkWidget *entry, gpointer user_data)
{
    const char *resource;

    resource = resource_widget_get_resource_name(entry);
    debug_gtk3("setting %s to '%s'\n", resource,
            gtk_entry_get_text(GTK_ENTRY(entry)));
    resources_set_string(resource, gtk_entry_get_text(GTK_ENTRY(entry)));
}


/** \brief  Create entry to control a string resource
 *
 * Creates a text entry to update \a resource. Makes a heap-allocated copy
 * of the resource name so initializing this widget with a constructed/temporary
 * resource name works as well.
 *
 * \param[in]   resource    resource name
 *
 * \note    The resource name is stored in the "ResourceName" property.
 *
 * \return  new entry
 */
GtkWidget *vice_gtk3_resource_entry_create(const char *resource)
{
    GtkWidget *entry;
    const char *current;

    /* get current resource value */
    if (resources_get_string(resource, &current) < 0) {
        /* invalid resource, set text to NULL */
        debug_gtk3("warning: failed to get resource '%s'\n", resource);
        current = NULL;
    }

    entry = gtk_entry_new();
    if (current != NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry), current);
    }

    /* make a copy of the resource name and store the pointer in the propery
     * "ResourceName" */
    resource_widget_set_resource_name(entry, resource);

    g_signal_connect(entry, "changed", G_CALLBACK(on_entry_changed), NULL);
    g_signal_connect(entry, "destroy", G_CALLBACK(on_entry_destroy), NULL);

    gtk_widget_show(entry);
    return entry;
}


/** \brief  Set new \a value for \a entry
 *
 * \param[in]   entry   entry
 * \param[in]   new     new text for \a entry
 */
void vice_gtk3_resource_entry_update(GtkWidget *entry, const char *new)
{
    gtk_entry_set_text(GTK_ENTRY(entry), new);
}


/** \brief  Reset \a entry to its resource factory value
 *
 * \param[in]   entry   entry
 */
void vice_gtk3_resource_entry_reset(GtkWidget *entry)
{
    const char *resource;
    const char *factory;

    resource = resource_widget_get_resource_name(entry);
    resources_get_default_value(resource, &factory);
    debug_gtk3("resetting %s to factory value %s\n", resource, factory);
    vice_gtk3_resource_entry_update(entry, factory);
}


/*****************************************************************************
 *          Resource entry box that only responds to 'full' changes          *
 ****************************************************************************/

/** \brief  Handler for the "destroy" event of the full entry box
 *
 * \param[in,out]   entry   full resource entry box
 * \param[in]       data    ununsed
 */
static void on_resource_entry_full_destroy(GtkEntry *entry, gpointer data)
{
    char *tmp;

    tmp = g_object_get_data(G_OBJECT(entry), "ResourceName");
    if (tmp != NULL) {
        lib_free(tmp);
    }
    tmp = g_object_get_data(G_OBJECT(entry), "ResourceOrig");
    if (tmp != NULL) {
        lib_free(tmp);
    }
}


/** \brief  Update resource when it differs from the \a entry's value
 *
 * \param[in,out]   entry   full resource entry box
 */
static void resource_entry_full_update_resource(GtkEntry *entry)
{
    const char *res_name;
    const char *res_val;
    const char *entry_text;

    res_name = resource_widget_get_resource_name(GTK_WIDGET(entry));
    if (resources_get_string(res_name, &res_val) < 0) {
        return;
    }
    entry_text = gtk_entry_get_text(entry);
    debug_gtk3("res_name: %s res_val: %s entry_text: %s\n",
            res_name, res_val, entry_text);
    if ((res_val == NULL) || (strcmp(entry_text, res_val) != 0)) {
        resources_set_string(res_name, entry_text);
        debug_gtk3("set res_name: %s entry_text: %s\n", res_name, entry_text);
    }
}


/** \brief  Handler for the "focus-out" event
 *
 * \param[in]   entry   entry box
 * \param[in]   event   event object
 * \param[in]   data    unused
 *
 * \return  TRUE
 */
static gboolean on_focus_out_event(
        GtkEntry *entry,
        GdkEvent *event,
        gpointer data)
{
    resource_entry_full_update_resource(entry);
    return TRUE;
}


/** \brief  Handler for the "on-key-press" event
 *
 * \param[in]   entry   entry box
 * \param[in]   event   event object
 * \param[in]   data    unused
 *
 * \return  TRUE if Enter was pushed, FALSE otherwise (makes the pushed key
 *          propagate to the entry_
 */
static gboolean on_key_press_event(
        GtkEntry *entry,
        GdkEvent *event,
        gpointer data)
{
    GdkEventKey *keyev = (GdkEventKey *)event;

    if (keyev->type == GDK_KEY_PRESS && keyev->keyval == GDK_KEY_Return) {
        resource_entry_full_update_resource(entry);
        return TRUE;
    }
    return FALSE;
}


/** \brief  Create resource entry box that only reacts to 'full' entries
 *
 * Creates a resource-connected entry box that only updates the resource when
 * the either the widget looses focus (due to Tab or mouse click somewhere else
 * in the UI) or when the user presses 'Enter'. This behaviour differs from the
 * other resource entry which updates its resource on every key press.
 *
 * \param[in]   resource    resource name
 *
 * \return  GtkEntry
 */
GtkWidget *vice_gtk3_resource_entry_full_create(const char *resource)
{
    GtkWidget *entry;
    const char *current;
    char *orig = NULL;

    entry = gtk_entry_new();
    /* make a copy of the resource name and store the pointer in the propery
     * "ResourceName" */
    resource_widget_set_resource_name(entry, resource);

    /* set current value */
    if (resources_get_string(resource, &current) < 0) {
        current = NULL;
    }

    /* store current resource value, so it can be restored via
     * resource_entry_full_reset() */
    if (current != NULL) {
        orig = lib_stralloc(current);
    } else {
        orig = lib_stralloc("");
    }
    g_object_set_data(G_OBJECT(entry), "ResourceOrig", orig);

    if (current != NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry), current);
    }

    g_signal_connect(entry, "destroy",
            G_CALLBACK(on_resource_entry_full_destroy), NULL);
    g_signal_connect(entry, "focus-out-event",
            G_CALLBACK(on_focus_out_event), NULL);
    g_signal_connect(entry, "key-press-event",
            G_CALLBACK(on_key_press_event), NULL);

    return entry;
}


/** \brief  Reset the widget to the original resource value
 *
 * Resets the widget and the connect resource to the value the resource
 * contained when the widget was created.
 *
 * \param[in,out]   entry   resource entry box
 */
void vice_gtk3_resource_entry_full_reset(GtkWidget *entry)
{
    const char *res_name;
    const char *orig;

    orig = resource_widget_get_string(entry, "ResourceOrig");
    res_name = resource_widget_get_resource_name(entry);
    resources_set_string(res_name, orig);
    gtk_entry_set_text(GTK_ENTRY(entry), orig);
}


/** \brief  Update \a entry with text \a new
 *
 * Sets \a new as the new text for \a entry and also updates the connected
 * resource.
 *
 * \param[in,out]   entry   entry box
 * \param[in]       new     new string for \a entry
 */
void vice_gtk3_resource_entry_full_update(GtkWidget *entry, const char *new)
{
    const char *res_name;

    if (new == NULL) {
        new = "";
    }

    res_name = resource_widget_get_resource_name(entry);
    if (resources_set_string(res_name, new) < 0) {
        debug_gtk3("failed to set resource %s to '%s'\n", res_name, new);
    } else {
        gtk_entry_set_text(GTK_ENTRY(entry), new);
    }
}
