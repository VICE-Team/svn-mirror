/** \file   src/arch/gtk3/widgets/driveidlemethodwidget.c
 * \brief   Drive expansions widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]IdleMethod
 *
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
#include "drive.h"
#include "drive-check.h"

#include "driveidlemethodwidget.h"



/** \brief  Idle method (name,id) tuples
 */
static ui_radiogroup_entry_t idle_methods[] = {
    { "None", 0 },
    { "Skip cycles", 1 },
    { "Trap idle", 2 },
    { NULL, -1 }
};


/** \brief  Create widget to set the drive idle method
 *
 * \param[in]   unit    current drive unit number
 *
 * \return  GtkGrid
 */
GtkWidget *drive_idle_method_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *radio_group;
    char buffer[256];

    g_snprintf(buffer, 256, "Drive%dIdleMethod", unit);
    grid = uihelpers_create_grid_with_label("Idle method", 1);
    radio_group = resource_radiogroup_create(buffer, idle_methods,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the widget
 *
 * \param[in,out]   widget  drive idle widget
 * \param[in]       unit    drive unit number
 */
void drive_idle_method_widget_update(GtkWidget *widget, int unit)
{
    GtkWidget *radio_group;
    int value;

    radio_group = gtk_grid_get_child_at(GTK_GRID(widget), 0, 1);
    if (radio_group != NULL && GTK_IS_GRID(radio_group)) {
        resources_get_int_sprintf("Drive%dIdleMethod", &value, unit);
        resource_radiogroup_update(radio_group, value);
    }
}
