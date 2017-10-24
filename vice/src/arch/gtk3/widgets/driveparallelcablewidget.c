/** \file   src/arch/gtk3/widgets/driveparallelcablewidget.c
 * \brief   Drive parallel cable widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]ParallelCable
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
#include "machine.h"
#include "drivewidgethelpers.h"

#include "driveparallelcablewidget.h"


/** \brief  List of possible parallel cables
 */
static ui_radiogroup_entry_t parallel_cables[] = {
    { "None", 0 },
    { "Standard", 1 },
    { "Professional DOS", 2 },
    { "Formel 64", 3 },
    { NULL, -1 }
};


/** \brief  Create drive parallel cable widget
 *
 * \param[in]   unit    drive unit
 *
 * \return  GtkGrid
 */
GtkWidget *drive_parallel_cable_widget_create(int unit)
{
    GtkWidget *grid;
    GtkWidget *radio_group;
    char resource[256];

    grid = uihelpers_create_grid_with_label("Parallel cable", 1);
    g_snprintf(resource, 256, "Drive%dParallelCable", unit);
    debug_gtk3("setting UnitNumber property to %d\n", unit);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    radio_group = resource_radiogroup_create(resource, parallel_cables,
            GTK_ORIENTATION_VERTICAL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);

    drive_parallel_cable_widget_update(grid);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the widget
 *
 * Enables/disable both the widget and its children depending on the drive type
 * and the machine class.
 *
 * \param[in,out]   widget  drive parallel cable widget
 */
void drive_parallel_cable_widget_update(GtkWidget *widget)
{
    int cable_type = 0; /* cable type, if set to < 0 causes the cable type
                           to revert to None (used for machines/drives that
                           don't support drive parallel cables */
    GtkWidget *radio_group;
    int drive_type;
    GtkWidget *radio;
    int enabled;
    int count;
    int i;
    int unit;

    unit = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "UnitNumber"));

    debug_gtk3("called with unit #%d\n", unit);

    /* determine if parallel cables are supported by the currently selected
     * drive type */
    drive_type = ui_get_drive_type(unit);
    gtk_widget_set_sensitive(widget, drive_check_parallel_cable(drive_type));

    /* determine if the parallel cable is supported by the machine */
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:
            /* all four types supported */
            enabled = TRUE;
            count = 4;
            break;
        case VICE_MACHINE_PLUS4:
            /* only the first two supported */
            enabled = TRUE;
            count = 2;
            break;
        default:
            /* none supported */
            enabled = FALSE;
            count = 4;
            cable_type = -1;
            break;
    }

    radio_group = gtk_grid_get_child_at(GTK_GRID(widget), 0, 1);
    for (i = 0; i < 4; i++) {
        radio = gtk_grid_get_child_at(GTK_GRID(radio_group), 0, i);
        if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
            if (i == count) {
                enabled = !enabled;
            }
            gtk_widget_set_sensitive(radio, enabled);
        }
    }

    if (cable_type >= 0) {
        resources_get_int_sprintf("Drive%dParallelCable", &cable_type, unit);
    } else {
        cable_type = 0; /* unsupported, set to 0 */
    }
    resource_radiogroup_update(radio_group, cable_type);
}
