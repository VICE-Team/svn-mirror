/** \file   src/arch/gtk3/uidrivesettings.c
 * \brief   Drive settings dialog
 *
 * Controls the following resource(s):
 *  DriveTrueEmulation
 *  DriveSoundEmulation
 *
 *  (for more, see used widgets)
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"
#include "drive.h"
#include "drive-check.h"
#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"
#include "drivewidgethelpers.h"
#include "driveunitwidget.h"
#include "drivetypewidget.h"
#include "driveextendpolicywidget.h"
#include "driveexpansionwidget.h"
#include "driveidlemethodwidget.h"
#include "driveparallelcablewidget.h"
#include "driveoptionswidget.h"

#include "uidrivesettings.h"


/** \brief  Unit currently having its settings changed
 */
static int unit_number = 8;


/** \brief  Reference to the drive type widget
 *
 * Used in unit_changed_callback() to update the widget
 */
static GtkWidget *drive_type_widget = NULL;


/*
 * TODO: Update code to avoid these non-static variables
 */
GtkWidget *drive_extend_widget = NULL;
GtkWidget *drive_expansion_widget = NULL;
GtkWidget *drive_idle_method_widget = NULL;
GtkWidget *drive_parallel_cable_widget = NULL;
GtkWidget *drive_options_widget = NULL;


/*****************************************************************************
 *                          non-Gtk+ event handlers                          *
 *****************************************************************************/

/** \brief  Extra callback when the unit number has changed
 *
 * This will update the various widgets with the proper settings for the
 * selected unit
 *
 * \param[in]   unit    drive unit number (8-11)
 */
static void unit_changed_callback(int unit)
{
    int type = ui_get_drive_type(unit);

    debug_gtk3("got unit %d\n", unit);

    if (drive_type_widget != NULL) {
        update_drive_type_widget(drive_type_widget, unit);
    }

    if (drive_extend_widget != NULL) {
        update_drive_extend_policy_widget(drive_extend_widget, unit);
        gtk_widget_set_sensitive(drive_extend_widget,
                drive_check_extend_policy(type));
    }

    if (drive_expansion_widget != NULL) {
        update_drive_expansion_widget(drive_expansion_widget, unit);
    }

    if (drive_idle_method_widget != NULL) {
        update_drive_idle_method_widget(drive_idle_method_widget, unit);
    }

    if (drive_parallel_cable_widget != NULL) {
        update_drive_parallel_cable_widget(drive_parallel_cable_widget, unit);
    }

    if (drive_options_widget != NULL) {
        update_drive_options_widget(drive_options_widget, unit);
    }
}


/*****************************************************************************
 *                              Gtk+ event handlers                          *
 *****************************************************************************/



/** \brief  Handler for the "destroy" event of the widget
 *
 * Right now sets widget references to `NULL` to avoid other event handlers
 * trying to update those widgets. I should look into setting up the event
 * handlers *after* all widgets are created and their values set, which should
 * result in event handlers not being triggered while setting up the UI
 *
 * \param[in]   widget      widget triggering the event (unused)
 * \param[in]   user_data   data for event (unused)
 */
static void on_destroy(GtkWidget *widget, gpointer user_data)
{
    drive_type_widget = NULL;
    drive_extend_widget = NULL;
    drive_expansion_widget = NULL;
    drive_idle_method_widget = NULL;
    drive_parallel_cable_widget = NULL;
    drive_options_widget = NULL;
}


/** \brief  Handler for the "toggled" event of the TDE check button
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   data for event (unused)
 */
static void on_tde_toggled(GtkWidget *widget, gpointer user_data)
{
    resources_set_int("DriveTrueEmulation",
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}


/** \brief  Handler for the "toggled" event of the drive sound check button
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   data for event (unused)
 */
static void on_drive_sound_toggled(GtkWidget *widget, gpointer user_data)
{
    resources_set_int("DriveSoundEmulation",
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}



/*****************************************************************************
 *                              Private functions                            *
 *****************************************************************************/



/** \brief  Create check button to set the "DriveTrueEmulation" resource
 *
 * \return GtkCheckButton
 */
static GtkWidget *create_tde_check_button(void)
{
    GtkWidget *check;

    check = uihelpers_create_resource_checkbox("Enable true drive emulation",
            "DriveTrueEmulation");
    g_object_set(check, "margin-left", 8, NULL);
    return check;
}


/** \brief  Create check button to control
 */
static GtkWidget *create_drive_sound_check_button(void)
{
    GtkWidget *check = uihelpers_create_resource_checkbox(
            "Enable drive sound emulation", "DriveSoundEmulation");
    g_object_set(check, "margin-left", 8, NULL);
    return check;
}



/*****************************************************************************
 *                              Public functions                             *
 *****************************************************************************/

/** \brief  Create drive settings widget for the settings dialog
 *
 * \param[in]  parent  parent widget
 *
 * \return  GtkGrid
 *
 * FIXME:   this just became a bit messy, I should document the wrappers
 */
GtkWidget *uidrivesettings_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *unit_widget;

    GtkWidget *type_40_grid;
    GtkWidget *idle_par_grid;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin", 8, NULL);


    gtk_grid_attach(GTK_GRID(layout), create_tde_check_button(), 0, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(layout), create_drive_sound_check_button(),
            0, 1, 3, 1);

    unit_widget = create_drive_unit_widget(8, &unit_number, unit_changed_callback);
    g_object_set(unit_widget, "margin-left", 8, NULL);
    /* row 2, column 0-2 */
    gtk_grid_attach(GTK_GRID(layout), unit_widget, 0, 2, 3, 1);


    type_40_grid = gtk_grid_new();

    /* create drive type widget, but don't connect signal handler yet, to avoid
     * reseting a drive */
    drive_type_widget = create_drive_type_widget(unit_number,
            unit_changed_callback);
    gtk_grid_attach(GTK_GRID(type_40_grid), drive_type_widget, 0, 0, 1, 1);

    drive_extend_widget = create_drive_extend_policy_widget(unit_number);
    update_drive_extend_policy_widget(drive_extend_widget, unit_number);
    gtk_grid_attach(GTK_GRID(type_40_grid), drive_extend_widget, 0, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(layout), type_40_grid, 0, 3, 1, 2);

    /* row 3, column 1 */
    drive_expansion_widget = create_drive_expansion_widget(unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_expansion_widget, 1, 3, 1, 1);

    /* row 3, column 2 */

    idle_par_grid = gtk_grid_new();
    drive_idle_method_widget = create_drive_idle_method_widget(unit_number);
    update_drive_idle_method_widget(drive_idle_method_widget, unit_number);
    gtk_grid_attach(GTK_GRID(idle_par_grid), drive_idle_method_widget,
            0, 0, 1, 1);
    drive_parallel_cable_widget = create_drive_parallel_cable_widget(
            unit_number);
    gtk_grid_attach(GTK_GRID(idle_par_grid), drive_parallel_cable_widget,
            0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), idle_par_grid, 2, 3, 1, 1);

    /* row 4, column 1 & 2 */
    drive_options_widget = create_drive_options_widget(unit_number);
    update_drive_options_widget(drive_options_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_options_widget, 1, 4, 2, 1);

    /* now connect the drive type widget's signal handlers */
    connect_drive_type_widget_signals(drive_type_widget);

    g_signal_connect(layout, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(layout);
    return layout;
}
