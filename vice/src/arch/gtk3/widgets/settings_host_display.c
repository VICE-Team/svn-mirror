/** \file   settings_host_display.c
 * \brief   Widget to control resources related to the host display
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES StartMinimized          -vsid
 * $VICERES RestoreWindowGeometry   -vsid
 * (I guess VSID could also use this?)
 *
 * Note:    RestoreWindowGeometry works fine on Linux with a display stretched
 *          over multiple monitors. No idea how this would work with separate
 *          'desktops' for each monitor, we might have to store GDK's monitor
 *          index as well.
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

#include "vice_gtk3.h"
#include "resources.h"
#include "machine.h"

#include "canvasrenderbackendwidget.h"
#include "canvasrenderfilterwidget.h"

#include "settings_misc.h"


/** \brief  Create host display settings widget
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_host_display_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;
    GtkWidget *backend_widget = canvas_render_backend_widget_create();
    GtkWidget *filter_widget = canvas_render_filter_widget_create();
    GtkWidget *minimized_widget;
    GtkWidget *restore_window_widget;


    grid = gtk_grid_new();

    if (machine_class != VICE_MACHINE_VSID) {

        minimized_widget = vice_gtk3_resource_check_button_new(
                "StartMinimized",
                "Start the emulator window minimized");

        restore_window_widget = vice_gtk3_resource_check_button_new(
                "RestoreWindowGeometry",
                "Restore emulator window(s) position and size from settings");

        gtk_grid_attach(GTK_GRID(grid), filter_widget, 0, 1, 2, 1);
        g_object_set(filter_widget, "margin-left",8, NULL);
        gtk_grid_attach(GTK_GRID(grid), backend_widget, 1, 1, 2, 1);
        g_object_set(minimized_widget, "margin-top", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), minimized_widget, 0, 2, 2, 1);
        g_object_set(restore_window_widget, "margin-top", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), restore_window_widget, 0, 3, 2, 1);
    }
    gtk_widget_show_all(grid);
    return grid;
}
