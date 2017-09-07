/*
 * uiautostart.c - GTK3 autostart settings central widget
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


#include "uiautostart.h"


/*
 * Event handlers
 */

static void on_tde_changed(GtkWidget *widget, gpointer user_data)
{
    int active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    resources_set_int("AutostartHandleTrueDriveEmulation", active);
}


/** \brief  Create checkbox for the "AutostartHandleTrueDriveEmulation"
 *          resource (yes, it really is that long)
 *
 * \return  checkbox
 */
static GtkWidget *create_tde_checkbox(void)
{
    GtkWidget *check;
    int tde;

    check = gtk_check_button_new_with_label(
            "Handle True Drive Emulation on autostart");
    resources_get_int("AutostartHandleTrueDriveEmulation", &tde);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), tde);
    g_signal_connect(check, "toggled", G_CALLBACK(on_tde_changed), NULL);
    return check;
}



/** \brief  Create widget to use in the settings dialog for autostart stuff
 */
GtkWidget *uiautostart_create_central_widget(GtkWidget *parent)
{
    GtkWidget *grid;

    GtkWidget *warp;

    grid = gtk_grid_new();
    g_object_set(grid, "margin", 8, NULL);

    gtk_grid_attach(GTK_GRID(grid),
            uihelpers_create_resource_checkbox(
                "Handle True Drive Emulation on autostart",
                "AutostartHandleTrueDriveEmulation"), 0, 0, 1, 1);

    warp = uihelpers_create_resource_checkbox("Warp on autostart", "AutostartWarp");
    gtk_grid_attach(GTK_GRID(grid), warp, 0, 1, 1, 1);
    gtk_widget_show_all(grid);

    return grid;
}


