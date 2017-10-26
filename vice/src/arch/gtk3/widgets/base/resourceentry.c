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

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"

#include "resourceentry.h"


/** \brief  Handler for the "destroy" event of the entry
 *
 * Frees the heap-allocated copy of the resource name.
 *
 * \param[in]   entry       entry
 * \param[in]   user_data   extra event data (unused)
 */
static void on_entry_destroy(GtkEntry *entry, gpointer user_data)
{
    char *resource;

    resource = (char *)(g_object_get_data(G_OBJECT(entry), "ResourceName"));
    lib_free(resource);
}


/** \brief  Handler for the "changed" event of the check button
 *
 * \param[in]   entry       entry
 * \param[in]   user_data   unused
 */
static void on_entry_changed(GtkEntry *entry, gpointer user_data)
{
    const char *resource;

    resource = (const char *)g_object_get_data(G_OBJECT(entry), "ResourceName");
    debug_gtk3("setting %s to '%s'\n", resource, gtk_entry_get_text(entry));
    resources_set_string(resource, gtk_entry_get_text(entry));
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
GtkWidget *resource_entry_create(const char *resource)
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
    gtk_entry_set_text(GTK_ENTRY(entry), current);

    /* make a copy of the resource name and store the pointer in the propery
     * "ResourceName" */
    g_object_set_data(G_OBJECT(entry), "ResourceName",
            (gpointer)lib_stralloc(resource));

    g_signal_connect(entry, "changed", G_CALLBACK(on_entry_changed), NULL);
    g_signal_connect(entry, "destroy", G_CALLBACK(on_entry_destroy), NULL);

    gtk_widget_show(entry);
    return entry;
}


/** \brief  Set new \a value for \a entry
 *
 * \param[in]   entry   entry
 * \param[in]   value   new text for \a entry
 */
void resource_entry_update(GtkEntry *entry, const char *value)
{
    gtk_entry_set_text(entry, value);
}


/** \brief  Reset \a entry to its resource factory value
 *
 * \param[in]   entry   entry
 */
void resource_entry_reset(GtkEntry *entry)
{
    const char *resource;
    const char *factory;

    resource = (const char*)g_object_get_data(G_OBJECT(entry), "ResourceName");
    resources_get_default_value(resource, &factory);
    debug_gtk3("resetting %s to factory value %s\n", resource, factory);
    resource_entry_update(entry, factory);
}
