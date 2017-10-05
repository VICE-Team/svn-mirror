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

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "drive.h"
#include "drive-check.h"

#include "driveidlemethodwidget.h"


static int unit_number = 8;



/** \brief  Idle method (name,id) tuples
 */
static ui_radiogroup_entry_t idle_methods[] = {
    { "None", 0 },
    { "Skip cycles", 1 },
    { "Trap idle", 2 },
    { NULL, -1 }
};


/** \brief  Handler for the "toggled" event of the radio buttons
 */
static void on_idle_method_changed(GtkWidget *widget, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        int value = GPOINTER_TO_INT(user_data);

        debug_gtk3("setting Drive%dIdleMethod to %d\n", unit_number, value);
        resources_set_int_sprintf("Drive%dIdleMethod", value, unit_number);
    }
}


/** \brief  Create widget to set the drive idle method
 *
 * \param[in]   unit    current drive unit number
 *
 * \return  GtkGrid
 */
GtkWidget *drive_idle_method_widget_create(int unit)
{
    GtkWidget *widget;

    unit_number = unit;

    widget = uihelpers_radiogroup_create(
            "Idle method",
            idle_methods,
            on_idle_method_changed,
            0);
    return widget;
}


/** \brief  Update the widget
 *
 * \param[in,out]   widget  drive idle widget
 * \param[in]       unit    drive unit number
 */
void drive_idle_method_widget_update(GtkWidget *widget, int unit)
{
    int value;

    unit_number = unit;

    resources_get_int_sprintf("Drive%dIdleMethod", &value, unit);
    uihelpers_radiogroup_set_index(widget, value);
}
