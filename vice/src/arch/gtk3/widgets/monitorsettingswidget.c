/**
 * \file    src/arch/gtk3/widgets/monitorsettingswidget.c
 * \brief   GTK3 monitor setting dialog
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  KeepMonitorOpen
 *  MonitorServer
 *  MonitorServerAddress
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
 */


#include "vice.h"

#include <gtk/gtk.h>
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"

#include "monitorsettingswidget.h"


/** \brief  Create widget to control monitor resources
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *monitor_settings_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *keep_open;
    GtkWidget *server_enable;
    GtkWidget *server_address;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);


    keep_open = resource_check_button_create("KeepMonitorOpen",
            "Keep monitor open");
    server_enable = resource_check_button_create("MonitorServer",
            "Enable remote monitor");
    label = gtk_label_new("Server address");
    /* align with the rest, more or less */
    g_object_set(label, "margin-left", 8, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    server_address = resource_entry_create("MonitorServerAddress");
    gtk_widget_set_hexpand(server_address, TRUE);
    gtk_grid_attach(GTK_GRID(grid), keep_open, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), server_enable, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), server_address, 1, 2, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}
