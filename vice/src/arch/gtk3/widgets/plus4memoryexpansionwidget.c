/** \file   plus4memoryexpansionwidget.c
 * \brief   Plus4 memory expansion widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MemoryHack  xplus4
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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "resourceradiogroup.h"
#include "plus4memhacks.h"

#include "plus4memoryexpansionwidget.h"


/** \brief  List of memory expansion hacks
 *
 * This list is used to build the radio button group
 */
static const vice_gtk3_radiogroup_entry_t expansions[] = {
    { "None",               MEMORY_HACK_NONE },
    { "256KiB (CSORY)",     MEMORY_HACK_C256K },
    { "256KiB (HANNES)",    MEMORY_HACK_H256K },
    { "1024KiB (HANNES)",   MEMORY_HACK_H1024K },
    { "4096KiB (HANNES)",   MEMORY_HACK_H4096K },
    { NULL, -1 }
};


/** \brief  Reference to the radiogroup widget
 */
static GtkWidget *memory_exp_widget = NULL;


/** \brief  Create Plus/4 memory expansion widget
 *
 * \return  GtkGrid
 */
GtkWidget *plus4_memory_expansion_widget_create(void)
{
    GtkWidget *grid;

    grid = uihelpers_create_grid_with_label("Memory expansion hack", 1);
    memory_exp_widget = vice_gtk3_resource_radiogroup_new(
            "MemoryHack",
            expansions,
            GTK_ORIENTATION_VERTICAL);
    gtk_grid_attach(GTK_GRID(grid), memory_exp_widget, 0, 1, 1, 1);
    g_object_set(memory_exp_widget, "margin-left", 16, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set user-defined callback
 *
 * This callback will be called with RAM-size and hack-type arguments
 *
 * \param[in]   callback    function to trigger
 */
void plus4_memory_expansion_widget_add_callback(void (*cb)(GtkWidget *, int))
{
    vice_gtk3_resource_radiogroup_add_callback(memory_exp_widget, cb);
}


/** \brief  Synchronize the widget with its resource
 *
 * No need for passing in the widget reference, there shall be only one.
 *
 * \return  bool
 */
gboolean plus4_memory_expansion_widget_sync(void)
{
    return vice_gtk3_resource_radiogroup_sync(memory_exp_widget);
}
