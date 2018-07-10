/** \file   resourcewidgetmanager.c
 * \brief   Module to manage resource widgets
 *
 * This module allows one to register resource-bound widgets to the manager,
 * so the manager can then be invoked to either reset all widgets to their
 * state when they were registered, or reset the widgets to the
 * resource's default state.
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
 */

#include "vice.h"

#include <gtk/gtk.h>

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "lib.h"
#include "log.h"
#include "resourcehelpers.h"

#include "resourcewidgetmanager.h"


/** \brief  Initial number of entries for widgets allocated
 *
 * Seems like a decent number, but should one register another widget, the
 * list is simply doubled in size.
 */
#define INITIAL_ENTRIES 64


static void resource_widget_entry_init(resource_widget_entry_t *entry)
{
    entry->widget = NULL;
    entry->resource = NULL;
    entry->reset = NULL;
    entry->factory = NULL;
    entry->sync = NULL;
}


static void resource_widget_entry_cleanup(resource_widget_entry_t *entry)
{
    if (entry->resource != NULL) {
        lib_free(entry->resource);
    }
}


void vice_resource_widget_manager_init(resource_widget_manager_t *manager)
{
    size_t i;

    manager->widget_list = lib_malloc(sizeof *(manager->widget_list)
            * INITIAL_ENTRIES);
    manager->widget_num = 0;
    manager->widget_max = INITIAL_ENTRIES;
    for (i = 0; i < INITIAL_ENTRIES; i++) {
        manager->widget_list[i] = NULL;
    }
}


/** \brief  Clean up resources used by \a manager's members
 *
 * Doesn't free \a manager itself
 *
 * \param[in,out]   manager resource widget manager instance
 */
void vice_resource_widget_manager_exit(resource_widget_manager_t *manager)
{
    size_t i;

    for (i = 0; i < manager->widget_num; i++) {
        resource_widget_entry_cleanup(manager->widget_list[i]);
        lib_free(manager->widget_list[i]);
    }
    lib_free(manager->widget_list);
}


/** \brief  Add a widget to the resource widget manager
 *
 */
void vice_resource_widget_manager_add_widget(
        resource_widget_manager_t *manager,
        GtkWidget *widget,
        const char *resource,
        gboolean (*reset)(GtkWidget *),
        gboolean (*factory)(GtkWidget *),
        gboolean (*sync)(GtkWidget *))
{
    resource_widget_entry_t *entry;

    /* do we need to resize the list? */
    if (manager->widget_max == manager->widget_num) {
        /* resize widget array */
        manager->widget_list = lib_realloc(manager->widget_list,
            sizeof *(manager->widget_list) * manager->widget_max * 2);
        manager->widget_max *= 2;
    }

    /* create new entry */
    entry = lib_malloc(sizeof *entry);
    resource_widget_entry_init(entry);
    entry->widget = widget;
    if (resource != NULL) {
        entry->resource = lib_stralloc(resource);
    }
    entry->reset = reset;
    entry->factory = factory;
    entry->sync = sync;

    /* store entry */
    manager->widget_list[manager->widget_num++] = entry;
}


void vice_resource_widget_manager_dump(resource_widget_manager_t *manager)
{
    size_t i;

    debug_gtk3("Resource Widget Manager: registered resources:");
    for (i = 0; i < manager->widget_num; i++) {
        resource_widget_entry_t *entry = manager->widget_list[i];

        if (entry->resource == NULL) {
            debug_gtk3("    standard widget: '%s'",
                    resource_widget_get_resource_name(entry->widget));
        } else {
            debug_gtk3("    custom widget  : '%s'",
                    entry->resource);
        }
    }
}


gboolean vice_resource_widget_manager_reset(resource_widget_manager_t *manager)
{
    size_t i;

    printf("Resource Widget Manager: registered resources:\n");
    for (i = 0; i < manager->widget_num; i++) {
        resource_widget_entry_t *entry = manager->widget_list[i];
        const char *resource;

        if (entry->resource == NULL) {
            resource = resource_widget_get_resource_name(entry->widget);
        } else {
            resource = entry->resource;
        }
        debug_gtk3("resetting resource '%s'", resource);
        /* custom reset func? */
        if (entry->reset != NULL) {
            debug_gtk3("calling custom reset function");
            entry->reset(entry->widget);
        } else {
            gboolean (*reset)(GtkWidget *) = NULL;
            debug_gtk3("calling default reset function");
            reset = g_object_get_data(G_OBJECT(entry->widget), "MethodReset");
            if (reset != NULL) {
                reset(entry->widget);
            } else {
                debug_gtk3("failed to find the reset method of the widget");
                return FALSE;
            }
        }
    }
    return TRUE;
}


