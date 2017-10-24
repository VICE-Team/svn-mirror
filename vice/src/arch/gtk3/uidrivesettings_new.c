/** \file   src/arch/gtk3/uidrivesettings_new.c
 * \brief   Drive settings dialog, new version
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  DriveTrueEmulation
 *  DriveSoundEmulation
 *
 *  (for more, see used widgets)
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

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "not_implemented.h"
#include "drive.h"
#include "drive-check.h"
#include "machine.h"
#include "resources.h"
#include "resourcecheckbutton.h"
#include "widgethelpers.h"
#include "drivewidgethelpers.h"
#include "driveunitwidget.h"
#if 0
#include "drivetypewidget.h"
#else
#include "drivetypewidget_new.h"
#endif
#include "driveextendpolicywidget.h"
#include "driveexpansionwidget.h"
#include "driveidlemethodwidget.h"
#include "driveparallelcablewidget.h"
#if 0
#include "driveoptionswidget.h"
#endif
#include "driverpmwidget.h"


#include "uidrivesettings_new.h"

/* position in a stack child widget of the drive type widget */
#define CHILD_DRIVE_TYPE_XPOS   0
#define CHILD_DRIVE_TYPE_YPOS   0

/* position in a stack child widget of the drive extend policy widget */
#define CHILD_DRIVE_EXTEND_XPOS 2
#define CHILD_DRIVE_EXTEND_YPOS 1

/* position in a stack child widget of the drive expansions widget */
#define CHILD_DRIVE_EXPAND_XPOS 1
#define CHILD_DRIVE_EXPAND_YPOS 0

/* position in a stack child widget of the drive idle method widget */
#define CHILD_DRIVE_IDLE_XPOS 2
#define CHILD_DRIVE_IDLE_YPOS 0

/* position in a stack child widget of the drive parallel cable widget */
#define CHILD_DRIVE_PARALLEL_XPOS 1
#define CHILD_DRIVE_PARALLEL_YPOS 2

#define CHILD_DRIVE_RPM_XPOS 0
#define CHILD_DRIVE_RPM_YPOS 2



/** \brief  Callback for changes in the drive type widget
 *
 * XXX: I could use gtk_widget_get_parent() on the \a widget argument to get
 *      the stack child widget, but perhaps I'll need to wrap widgets into
 *      more GtkGrid's to get the layout right, and that would make a simple
 *      parent lookup fail (it would return the wrapper grid). Then again if
 *      I do use wrappers, this code will still fail.
 *      What did I get myself into, just to avoid globals and to make the
 *      glue logic simpler?     -- compyx
 *
 * \param[in,out]   widget  drive type widget
 * \param[in,out]   data    the child widget of the GtkStack
 */
static void stack_child_drive_type_callback(GtkWidget *widget, gpointer data)
{
    GtkWidget *drive_extend;
    GtkWidget *drive_expand;
    GtkWidget *drive_parallel;
    int unit;
    int type;

    debug_gtk3("called:\n");

    unit = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "UnitNumber"));
    resources_get_int_sprintf("Drive%dType", &type, unit);

    /* determine if extend-policy is valid for current unit number and its
     * drive type */
    drive_extend = gtk_grid_get_child_at(GTK_GRID(data),
            CHILD_DRIVE_EXTEND_XPOS, CHILD_DRIVE_EXTEND_YPOS);
    if (drive_extend != NULL) {
        gtk_widget_set_sensitive(drive_extend,
                drive_check_extend_policy(type));
    }

    /* determine which expansions are valid for current unit number and its
     * drive type */
    drive_expand = gtk_grid_get_child_at(GTK_GRID(data),
            CHILD_DRIVE_EXPAND_XPOS, CHILD_DRIVE_EXPAND_YPOS);
    if (drive_expand != NULL) {
        drive_expansion_widget_update(drive_expand);
    }

    /* determine which parallel cables are valid for current unit number and
     * its drive type */
    drive_parallel = gtk_grid_get_child_at(GTK_GRID(data),
            CHILD_DRIVE_PARALLEL_XPOS, CHILD_DRIVE_PARALLEL_YPOS);
    if (drive_parallel != NULL) {
        drive_parallel_cable_widget_update(drive_parallel);
    }


}



/** \brief  Create a composite widget with settings for drive \a unit
 *
 * \param[in]   unit    unit number
 *
 * \return  GtkGrid
 */
static GtkWidget *create_stack_child_widget(int unit)
{
    GtkWidget *grid;
    GtkWidget *drive_type;
    GtkWidget *drive_extend;
    GtkWidget *drive_expand;
    GtkWidget *drive_idle;
    GtkWidget *drive_parallel;
    GtkWidget *drive_rpm;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    g_object_set(grid, "margin-left", 8, NULL);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    drive_type = drive_type_widget_create(unit);
    drive_type_widget_add_callback(drive_type, stack_child_drive_type_callback,
            (gpointer)(grid));
    gtk_grid_attach(GTK_GRID(grid), drive_type,
            CHILD_DRIVE_TYPE_XPOS, CHILD_DRIVE_TYPE_YPOS, 1, 3);

    drive_extend = drive_extend_policy_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_extend,
            CHILD_DRIVE_EXTEND_XPOS, CHILD_DRIVE_EXTEND_YPOS, 1, 1);

    drive_expand= drive_expansion_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_expand,
            CHILD_DRIVE_EXPAND_XPOS, CHILD_DRIVE_EXPAND_YPOS, 1, 2);

    drive_idle = drive_idle_method_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_idle,
            CHILD_DRIVE_IDLE_XPOS, CHILD_DRIVE_IDLE_YPOS, 1, 1);

    drive_parallel = drive_parallel_cable_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_parallel,
            CHILD_DRIVE_PARALLEL_XPOS, CHILD_DRIVE_PARALLEL_YPOS, 1, 1);

    drive_rpm = drive_rpm_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_rpm,
            CHILD_DRIVE_RPM_XPOS, CHILD_DRIVE_RPM_YPOS, 1, 1);

    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Help, I'm forced to do Gtk3 stuff!</b>:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 4, 1);



    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create main drive settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uidrivesettings_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *wrapper;
    GtkWidget *tde;
    GtkWidget *sound;
    GtkWidget *stack;
    GtkWidget *switcher;
    int unit;

    /* three column wide grid */
    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 16);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 16);

    wrapper = gtk_grid_new();
    g_object_set(wrapper, "margin-left", 16, NULL);
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 16);
    tde = resource_check_button_create("DriveTrueEmulation",
            "True drive emulation");
    gtk_grid_attach(GTK_GRID(wrapper), tde, 0, 0, 1, 1);
    sound = resource_check_button_create("DriveSoundEmulation",
            "Drive sound emulation");
    gtk_grid_attach(GTK_GRID(wrapper), sound, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(layout), wrapper, 0, 0, 2, 1);


    stack = gtk_stack_new();
    for (unit = 8; unit < 12; unit++) {
        char title[256];

        g_snprintf(title, 256, "Drive %d", unit);
        gtk_stack_add_titled(GTK_STACK(stack),
                create_stack_child_widget(unit),
                title, title);
    }
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 1000);

    switcher = gtk_stack_switcher_new();
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(switcher, TRUE);
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),
            GTK_STACK(stack));

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_grid_attach(GTK_GRID(layout), switcher, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(layout), stack, 0, 2, 3, 1);


    gtk_widget_show_all(layout);
    return layout;
}
