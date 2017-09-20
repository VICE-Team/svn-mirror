/** \file   src/arch/gtk3/widgets/driveextendpolicywidget.c
 * \brief   Drive 40-track extend policy widget
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]ExtendPolicy
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
#include "drivewidgethelpers.h"

#include "driveextendpolicywidget.h"


/** \brief  List of (name,id) tuples for the radio buttons
 */
static ui_text_int_pair_t policies[] = {
    { "Never extend", 0 },
    { "Ask on extend", 1 },
    { "Extend on access", 2 },
    { NULL, -1 }
};


/** \brief  Unit number
 */
static int unit_number = 8;


/** \brief  Handler for the "toggle" event of the radio buttons
 *
 * Sets the 40-track expansion policy.
 *
 * \param[in]   widget      radio button
 * \param[in]   user_data   policy (int)
 */
static void on_radio_toggle(GtkWidget *widget, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        int policy = GPOINTER_TO_INT(user_data);

        debug_gtk3("setting Drive%dExtendImagePolicy to %d\n", unit_number, policy);
        resources_set_int_sprintf("Drive%dExtendImagePolicy", policy, unit_number);
    }
}


/** \brief  Create 40-track extend policy widget
 *
 * \param[in]   unit    drive unit number (8-11)
 *
 * \return  GtkGrid
 */
GtkWidget *create_drive_extend_policy_widget(int unit)
{
    GtkWidget *grid;

    unit_number = 8;

    grid = uihelpers_create_int_radiogroup_with_label(
            "40-track policy",
            policies,
            on_radio_toggle,
            0); /* update_drive_extend_policy_widget() should be called right
                   after this function, so this is fine*/

    return grid;
}


/** \brief  Update the widget with data from \a unit
 *
 * \param[in]   unit    drive unit number (8-11)
 */
void update_drive_extend_policy_widget(GtkWidget *widget, int unit)
{
    GtkWidget *radio;
    int policy;
    int drive_type = ui_get_drive_type(unit);

    unit_number = unit;

    resources_get_int_sprintf("Drive%dExtendImagePolicy", &policy, unit);

    radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, policy + 1);
    if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
    }

    /* determine if this widget is valid for the current drive type */
    gtk_widget_set_sensitive(widget,
            drive_check_extend_policy(drive_type));
}
