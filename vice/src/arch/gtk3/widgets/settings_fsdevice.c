/** \file   settings_fsdevice.c
 * \brief   File system device settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES FileSystemDevice8           -vsid
 * $VICERES FileSystemDevice9           -vsid
 * $VICERES FileSystemDevice10          -vsid
 * $VICERES FileSystemDevice11          -vsid
 * $VICERES FSDeviceLongNames           -vsid
 * $VICERES FSDeviceOverwrite           -vsid
 *
 *  (for more, see used widgets)
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

#include "vice.h"

#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "drive.h"
#include "resources.h"
#include "drivefsdevicewidget.h"

#include "settings_fsdevice.h"


/** \brief  References to the stack widgets
 */
static GtkWidget *fsdevice_widgets[NUM_DISK_UNITS];


/** \brief  Create a stack child widget for \a unit
 *
 * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkGrid
 */
GtkWidget *create_stack_child_widget(int unit)
{
    GtkWidget *layout;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 16);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 16);

    fsdevice_widgets[unit - DRIVE_UNIT_MIN] = drive_fsdevice_widget_create(unit);
    gtk_grid_attach(GTK_GRID(layout), fsdevice_widgets[unit - DRIVE_UNIT_MIN],
            0, 0, 1, 1);
    gtk_widget_set_hexpand(fsdevice_widgets[unit - DRIVE_UNIT_MIN], TRUE);

    gtk_widget_show_all(layout);
    return layout;
}



/** \brief  Create FS Device settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_fsdevice_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *long_names;
    GtkWidget *overwrite;
    GtkWidget *stack;
    GtkWidget *switcher;
    int unit;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 16);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 16);

    long_names = vice_gtk3_resource_check_button_new(
            "FSDeviceLongNames",
            "Allow filenames longer than 16 characters");
    gtk_grid_attach(GTK_GRID(layout), long_names, 0, 0, 1, 1);

    overwrite = vice_gtk3_resource_check_button_new(
            "FSDeviceOverwrite",
            "Always overwrite files without error");
    gtk_grid_attach(GTK_GRID(layout), overwrite, 0, 1, 1, 1);

    /* create 'tabs' for each fsdevice drive */
    stack = gtk_stack_new();
    for (unit = DRIVE_UNIT_MIN; unit <= DRIVE_UNIT_MAX; unit++) {
        gchar title[256];

        g_snprintf(title, sizeof(title), "Drive %d", unit);
        gtk_stack_add_titled(GTK_STACK(stack),
                create_stack_child_widget(unit),
                title, title);
    }
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 500);

    switcher = gtk_stack_switcher_new();
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(switcher, TRUE);
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),
            GTK_STACK(stack));

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_grid_attach(GTK_GRID(layout), switcher, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), stack, 0, 3, 1, 1);


    gtk_widget_show_all(layout);
    return layout;
}
