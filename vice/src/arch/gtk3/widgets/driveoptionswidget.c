/** \file   driveoptionswidget.c
 * \brief   Drive options widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES IECDevice8              -vsid
 * $VICERES IECDevice9              -vsid
 * $VICERES IECDevice10             -vsid
 * $VICERES IECDevice11             -vsid
 * $VICERES AttachDevice8Readonly   -vsid
 * $VICERES AttachDevice9Readonly   -vsid
 * $VICERES AttachDevice10Readonly  -vsid
 * $VICERES AttachDevice11Readonly  -vsid
 * $VICERES Drive8RTCSave           -vsid -xcbm5x0 -xcbm2 -xpet
 * $VICERES Drive9RTCSave           -vsid -xcbm5x0 -xcbm2 -xpet
 * $VICERES Drive10RTCSave          -vsid -xcbm5x0 -xcbm2 -xpet
 * $VICERES Drive11RTCSave          -vsid -xcbm5x0 -xcbm2 -xpet
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

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "basewidgets.h"
#include "debug_gtk3.h"
#include "drive-check.h"
#include "drive.h"
#include "drivewidgethelpers.h"
#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"

#include "driveoptionswidget.h"


static void on_iec_toggled(GtkWidget *widget, gpointer data)
{
    int unit = GPOINTER_TO_INT(data);
#ifdef HAVE_DEBUG_GTK3UI
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
#endif
    void (*callback)(GtkWidget *, int);

    debug_gtk3("State: %s, unit: %d",
            state ? "ON" : "OFF", unit);
    callback = g_object_get_data(G_OBJECT(widget), "ExtraCallback");
    if (callback != NULL) {
        debug_gtk3("Triggering custom callback:");
        callback(widget, unit);
    }
}


/** \brief  Create checkbox to toggle IEC-Device emulation for \a unit
 *
 * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_iec_check_button(int unit,
                                          void (*callback)(GtkWidget *, int))
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf(
            "IECDevice%d", "IEC-Device", unit);

    g_object_set_data(G_OBJECT(check), "ExtraCallback", (gpointer)callback);
    g_signal_connect(GTK_TOGGLE_BUTTON(check), "toggled", G_CALLBACK(on_iec_toggled),
            GINT_TO_POINTER(unit));

    return check;
}


/** \brief  Create widget to control read-only mode for \a unit
 *
  * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_readonly_check_button(int unit)
{
    return vice_gtk3_resource_check_button_new_sprintf("AttachDevice%dReadonly",
            "Read Only", unit);
}


/** \brief  Create widget to control Real time clock emulation for \a unit
 *
  * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_rtc_check_button(int unit)
{
    GtkWidget *check;
    int drive_type;

    check = vice_gtk3_resource_check_button_new_sprintf("Drive%dRTCSave",
            "RTC Save", unit);

    drive_type = ui_get_drive_type(unit);
    gtk_widget_set_sensitive(check,
            drive_type == DRIVE_TYPE_2000 || drive_type == DRIVE_TYPE_4000);
    return check;
}



/** \brief  Create drive options widget for \a unit
 *
 * \param[in]   unit        drive unit number
 * \param[in]   callback    custom callback
 *
 * \return GtkGrid
 */
GtkWidget *drive_options_widget_create(int unit,
                                       void (*iec_callback)(GtkWidget *, int))
{
    GtkWidget *grid;
    GtkWidget *iec_widget = NULL;
    GtkWidget *readonly_widget;
    GtkWidget *rtc_widget = NULL;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));
    g_object_set(grid, "margin-left", 16, NULL);

    readonly_widget = create_readonly_check_button(unit);
    gtk_grid_attach(GTK_GRID(grid), readonly_widget, 0, 0, 1, 1);

    switch (machine_class) {
        /* these machines have IEC */
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_C64DTV:   /* fall through */
        case VICE_MACHINE_VIC20:    /* fall through */
        case VICE_MACHINE_PLUS4:
            iec_widget = create_iec_check_button(unit, iec_callback);
            gtk_grid_attach(GTK_GRID(grid), iec_widget, 1, 0, 1, 1);
            rtc_widget = create_rtc_check_button(unit);
            gtk_grid_attach(GTK_GRID(grid), rtc_widget, 2, 0, 1, 1);
           break;
        default:
            break;
    }
    gtk_widget_show_all(grid);
    return grid;
}
