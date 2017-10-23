/** \file   src/arch/gtk3/widgets/cbm2memorysizewidget.c
 * \brief   CBM-II memory size widget
 *
 * Controls the following resource(s):
 *  RamSize     - installed RAM in KB
 *
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

#include "basewidgets.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"

#include "cbm2memorysizewidget.h"


/** \brief  List of RAM sizes
 */
static ui_radiogroup_entry_t ram_sizes[] = {
    { "64KB",   64 },
    { "128KB",  128 },
    { "256KB",  256 },
    { "512KB",  512 },
    { "1024KB", 1024 },
    { NULL, -1 }
};


/** \brief  Create CBM-II memory size widget
 *
 * \return  GtkGrid
 */
GtkWidget *cbm2_memory_size_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = uihelpers_create_grid_with_label("RAM size", 1);
    radio_group = resource_radiogroup_create("RamSize", ram_sizes,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
