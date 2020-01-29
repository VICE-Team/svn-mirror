/** \file   settings_monitor.c
 * \brief   GTK3 monitor setting dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES KeepMonitorOpen         all
 * $VICERES MonitorServer           all
 * $VICERES MonitorServerAddress    all
 * $VICERES NativeMonitor           all
 * $VICERES MonitorLogEnabled       all
 * $VICERES MonitorLogFileName      all
 * $VICERES MonitorChisLines        all
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
 */


#include "vice.h"
#include <gtk/gtk.h>

#include "vice_gtk3.h"

#include "settings_monitor.h"


/** \brief  Create widget to control monitor resources
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_monitor_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *native;
    GtkWidget *keep_open;
    GtkWidget *server_enable;
    GtkWidget *server_address;
    GtkWidget *label;
    GtkWidget *log_enable;
    GtkWidget *log_name;
    GtkWidget *log_label;
    GtkWidget *scroll_lines;
    GtkWidget *scroll_label;
#ifdef FEATURE_CPUMEMHISTORY
    GtkWidget *chis_lines;
    GtkWidget *chis_label;
#endif

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    native = vice_gtk3_resource_check_button_new("NativeMonitor",
            "Use native monitor interface");
    keep_open = vice_gtk3_resource_check_button_new("KeepMonitorOpen",
            "Keep monitor open");
    server_enable = vice_gtk3_resource_check_button_new("MonitorServer",
            "Enable remote monitor");
    label = gtk_label_new("Server address");
    /* align with the rest, more or less */
    g_object_set(label, "margin-left", 8, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    server_address = vice_gtk3_resource_entry_full_new(
            "MonitorServerAddress");
    gtk_widget_set_hexpand(server_address, TRUE);
    
    log_enable = vice_gtk3_resource_check_button_new("MonitorLogEnabled",
            "Enable logging to a file");
    log_label = gtk_label_new("Logfile name");
    /* align with the rest, more or less */
    g_object_set(log_label, "margin-left", 8, NULL);
    gtk_widget_set_halign(log_label, GTK_ALIGN_START);
    log_name = vice_gtk3_resource_entry_full_new(
            "MonitorLogFileName");
    gtk_widget_set_hexpand(log_name, TRUE);

    scroll_label = gtk_label_new("Number of lines in scrollback buffer\n(-1 for no limit)");
    g_object_set(scroll_label, "margin-left", 8, NULL);
    gtk_widget_set_halign(scroll_label, GTK_ALIGN_START);
    scroll_lines = vice_gtk3_resource_spin_int_new(
            "MonitorScrollbackLines", -1, 0x0fffffff, 1);
    
#ifdef FEATURE_CPUMEMHISTORY
    chis_label = gtk_label_new("Number of lines in CPU History");
    g_object_set(chis_label, "margin-left", 8, NULL);
    gtk_widget_set_halign(chis_label, GTK_ALIGN_START);
    chis_lines = vice_gtk3_resource_spin_int_new(
            "MonitorChisLines", 10, 0x0fffffff, 1);
#endif
    
    gtk_grid_attach(GTK_GRID(grid), native, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), keep_open, 0, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), server_enable, 0, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), server_address, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), log_enable, 0, 4, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), log_label, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), log_name, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll_label, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll_lines, 1, 6, 1, 1);
#ifdef FEATURE_CPUMEMHISTORY
    gtk_grid_attach(GTK_GRID(grid), chis_label, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chis_lines, 1, 7, 1, 1);
#endif
    gtk_widget_show_all(grid);
    return grid;
}
