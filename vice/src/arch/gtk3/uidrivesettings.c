/** \file   src/arch/gtk3/uidrivesettings.c
 * \brief   Drive settings dialog
 *
 * Controls the following resource(s):
 * XXX: replace
 *  AutostartDelay                    - delay in seconds (0-1000) (integer)
 *  AutostartDelayRandom              - add random delay to autostart (boolean)
 *  AutostartPrgMode                  - autostart mode for PRG/P00 files:
 *                                      VirtualFS (0), InjectIntoRam (1),
 *                                      Disk (2) (integer)
 *  AutostartPrgDiskImage             - disk image to use in case of
 *                                      AutostartPrgMode disk (string)
 *  AutostartRunWithColon             - add ':' after RUN (boolean)
 *  AutostartBasicLoad                - load PRG into BASIC (LOAD"*",8) (boolean)
 *  AutostartWarp                     - use warp during autostart (boolean)
 *  AutostartHandleTrueDriveEmulation - use True Drive Emulation during
 *                                      autostart (boolean)
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

#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"
#include "driveunitwidget.h"
#include "drivetypewidget.h"

#include "uidrivesettings.h"


/** \brief  Unit currently having its settings changed
 */
static int unit_number = 8;

static GtkWidget *drive_type_widget = NULL;


/** \brief  Extra callback when the unit number has changed
 *
 * This will update the various widgets with the proper settings for the
 * selected unit
 *
 * \param[in]   unit    drive unit number (8-11)
 */
static void unit_changed_callback(int unit)
{
    debug_gtk3("got unit %d\n", unit);
    update_drive_type_widget(drive_type_widget, unit);
}


GtkWidget *uidrivesettings_create_central_widget(GtkWidget *parent)
{
    GtkWidget *layout;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin", 8, NULL);

    /* row 0, column 0 & 1 */
    gtk_grid_attach(
            GTK_GRID(layout),
            create_drive_unit_widget(8, &unit_number, unit_changed_callback),
            0, 0, 2, 1);

    /* row 1, column 0 */
    drive_type_widget = create_drive_type_widget(unit_number);
    update_drive_type_widget(drive_type_widget, unit_number);
    gtk_grid_attach(GTK_GRID(layout), drive_type_widget, 0, 1, 1, 1);

    gtk_widget_show_all(layout);
    return layout;
}
