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


/* References to various interconnected widgets */
static GtkWidget *drive_type_widget = NULL;
static GtkWidget *drive_extend_widget = NULL;
static GtkWidget *drive_expansion_widget = NULL;
static GtkWidget *drive_idle_method_widget = NULL;
static GtkWidget *drive_parallel_cable_widget = NULL;
static GtkWidget *drive_options_widget = NULL;


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
        drive_type_widget_update(drive_type_widget, unit);
    }

    if (drive_extend_widget != NULL) {
        drive_extend_policy_widget_update(drive_extend_widget, unit);
        gtk_widget_set_sensitive(drive_extend_widget,
                drive_check_extend_policy(type));
    }

    if (drive_expansion_widget != NULL) {
        drive_expansion_widget_update(drive_expansion_widget, unit);
    }

    if (drive_idle_method_widget != NULL) {
        drive_idle_method_widget_update(drive_idle_method_widget, unit);
    }

    if (drive_parallel_cable_widget != NULL) {
        drive_parallel_cable_widget_update(drive_parallel_cable_widget, unit);
    }

    if (drive_options_widget != NULL) {
        drive_options_widget_update(drive_options_widget, unit);
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


/*****************************************************************************
 *                              Public functions                             *
 *****************************************************************************/

/** \brief  Create drive settings widget for the settings dialog
 *
 * \param[in]  parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uidrivesettings_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *unit_widget;

    GtkWidget *type_40_grid;    /* wrapper to contain the drive type and the
                                   40-track extend widget */
    GtkWidget *idle_par_grid;   /* wrapper to contain the idle widget and the
                                   parallel cable widget */

    GtkWidget *tde_widget;
    GtkWidget *sound_widget;

    GtkWidget *tde_sound_wrapper;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin", 8, NULL);

    /* add TDE & sound widgets next to each other to save a bit of vertical
     * space */
    tde_sound_wrapper = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(tde_sound_wrapper), 8);

    tde_widget = uihelpers_create_resource_checkbox(
            "Enable true drive emulation", "DriveTrueEmulation");
    g_object_set(tde_widget, "margin-left", 8, NULL);
    gtk_grid_attach(GTK_GRID(tde_sound_wrapper), tde_widget, 0, 0, 1, 1);
    sound_widget = uihelpers_create_resource_checkbox(
            "Enable drive sound emulation", "DriveSoundEmulation");
    g_object_set(sound_widget, "margin-left", 8, NULL);
    gtk_grid_attach(GTK_GRID(tde_sound_wrapper), sound_widget, 1, 0, 1, 1);

    /* row 0, columns 0-2 */
    gtk_grid_attach(GTK_GRID(layout), tde_sound_wrapper, 0, 0, 3, 1);


    unit_widget = drive_unit_widget_create(8, &unit_number, unit_changed_callback);
    g_object_set(unit_widget, "margin-left", 8, NULL);
    /* row 1, column 0-2 */
    gtk_grid_attach(GTK_GRID(layout), unit_widget, 0, 1, 3, 1);

    type_40_grid = gtk_grid_new();
    /* create drive type widget, but don't connect signal handler yet, to avoid
     * reseting a drive */
    drive_type_widget = drive_type_widget_create(unit_number,
            unit_changed_callback);
    gtk_grid_attach(GTK_GRID(type_40_grid), drive_type_widget, 0, 0, 1, 1);

    drive_extend_widget = drive_extend_policy_widget_create(unit_number);
    drive_extend_policy_widget_update(drive_extend_widget, unit_number);
    gtk_grid_attach(GTK_GRID(type_40_grid), drive_extend_widget, 0, 1, 1, 1);

    /* row 2, column 0 */
    gtk_grid_attach(GTK_GRID(layout), type_40_grid, 0, 2, 1, 2);

    /* row 2, column 1 */
    drive_expansion_widget = drive_expansion_widget_create(unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_expansion_widget, 1, 2, 1, 1);

    /* row 2, column 2 */

    idle_par_grid = gtk_grid_new();
    drive_idle_method_widget = drive_idle_method_widget_create(unit_number);
    drive_idle_method_widget_update(drive_idle_method_widget, unit_number);
    gtk_grid_attach(GTK_GRID(idle_par_grid), drive_idle_method_widget,
            0, 0, 1, 1);
    drive_parallel_cable_widget = drive_parallel_cable_widget_create(
            unit_number);
    gtk_grid_attach(GTK_GRID(idle_par_grid), drive_parallel_cable_widget,
            0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), idle_par_grid, 2, 2, 1, 1);

    /* row 3, column 1 & 2 */
    drive_options_widget = drive_options_widget_create(unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_options_widget, 1, 3, 2, 1);

    /* now connect the drive type widget's signal handlers */
    drive_type_widget_connect_signals(drive_type_widget);

    drive_type_widget_set_extend_widget(drive_extend_widget);
    drive_type_widget_set_expansion_widget(drive_expansion_widget);
    drive_type_widget_set_parallel_cable_widget(drive_parallel_cable_widget);
    drive_type_widget_set_options_widget(drive_options_widget);


    g_signal_connect(layout, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(layout);
    return layout;
}
