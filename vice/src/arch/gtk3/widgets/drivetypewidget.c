/** \file   src/arch/gtk3/widgets/drivetypewidget.c
 * \brief   Drive type selection dialog
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Drive[8-11]Type
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
#include "driveexpansionwidget.h"
#include "driveparallelcablewidget.h"
#include "drivewidgethelpers.h"
#include "driveoptionswidget.h"

#include "drivetypewidget.h"



/** \brief  Unit number (8-11)
 */
static int unit_number = 8;

static void(*unit_callback)(int) = NULL;

static GtkWidget *drive_extend_widget;
static GtkWidget *drive_expansion_widget;
static GtkWidget *drive_parallel_cable_widget;
static GtkWidget *drive_options_widget;



void drive_type_widget_set_extend_widget(GtkWidget *widget)
{
    drive_extend_widget = widget;
}


void drive_type_widget_set_expansion_widget(GtkWidget *widget)
{
    drive_expansion_widget = widget;
}

void drive_type_widget_set_parallel_cable_widget(GtkWidget *widget)
{
    drive_parallel_cable_widget = widget;
}

void drive_type_widget_set_options_widget(GtkWidget *widget)
{
    drive_options_widget = widget;
}



/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * \param[in]   widget      radio button
 * \param[in]   user_data   unit number
 */
static void on_radio_toggled(GtkWidget *widget, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
        int new_type = GPOINTER_TO_INT(user_data);
        int old_type = ui_get_drive_type(unit_number);

        /* prevent drive reset when switching unit number and updating the
         * drive type widget */
        if (new_type != old_type) {
            debug_gtk3("setting Drive%dType to %d\n", unit_number, new_type);
            resources_set_int_sprintf("Drive%dType", new_type, unit_number);
        }

        /* enable/disable 40-track settings widget */
        if (drive_extend_widget != NULL) {
            gtk_widget_set_sensitive(drive_extend_widget,
                    drive_check_extend_policy(new_type));
        }
        /* update expansions widget */
        if (drive_expansion_widget != NULL) {
            drive_expansion_widget_update(drive_expansion_widget, unit_number);
        }
        /* update parallel cable widget */
        if (drive_parallel_cable_widget != NULL) {
            drive_parallel_cable_widget_update(drive_parallel_cable_widget,
                    unit_number);
        }
        if (drive_options_widget != NULL) {
            drive_options_widget_update(drive_options_widget, unit_number);
        }
    }
}


/** \brief  Create a drive unit selection widget
 *
 * Creates a widget with four radio buttons, horizontally aligned, to select
 * a drive unit (8-11)
 *
 * \param[in]   unit    default drive unit
 *
 * \return  GtkGrid
 */
GtkWidget *drive_type_widget_create(int unit, void (*callback)(int))
{
    GtkWidget *grid;
    drive_type_info_t *list;
    GtkRadioButton *last = NULL;
    GSList *group = NULL;
    size_t i;

    unit_number = unit;
    unit_callback = callback;

    grid = uihelpers_create_grid_with_label("Drive type", 1);
    list = drive_get_type_info_list();
    for (i = 0; list[i].name != NULL; i++) {
        GtkWidget *radio = gtk_radio_button_new_with_label(group, list[i].name);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
        g_object_set(radio, "margin-left", 16, NULL);

        gtk_grid_attach(GTK_GRID(grid), radio, 0, i + 1, 1, 1);
        last = GTK_RADIO_BUTTON(radio);
    }

    drive_type_widget_update(grid, unit);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the drive type widget with a new unit number
 *
 * \param[in,out]   widget  drive type widget
 * \param[in]       unit    new unit number
 */
void drive_type_widget_update(GtkWidget *widget, int unit)
{
    drive_type_info_t *list;
    size_t i;
    int type = ui_get_drive_type(unit);

    unit_number = unit;

    list = drive_get_type_info_list();
    debug_gtk3("updating drive type list\n");
    for (i = 0; list[i].name != NULL; i++) {
        GtkWidget *radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 1);
        if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
            gtk_widget_set_sensitive(radio, drive_check_type(
                        (unsigned int)(list[i].id), (unsigned int)(unit - 8)));
            if (list[i].id == type) {
                /* TODO: temporary block the resource-set callback */
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
            }
        }
    }
}


/** \brief  Setup signal handlers for the drive type widget
 *
 * \param[in,out]   widget  drive type widget
 */
void drive_type_widget_connect_signals(GtkWidget *widget)
{
    drive_type_info_t *list;
    size_t i;

    list = drive_get_type_info_list();
    for (i = 0; list[i].name != NULL; i++) {
        GtkWidget *radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, i + 1);
        /* debug_gtk3("connecting handle for %s/%d\n", list[i].name, list[i].id); */
        if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
            g_signal_connect(radio, "toggled", G_CALLBACK(on_radio_toggled),
                    GINT_TO_POINTER(list[i].id));
        }
    }
}
