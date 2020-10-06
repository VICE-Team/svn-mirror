/** \file   settings_monitor.c
 * \brief   GTK3 monitor setting dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES KeepMonitorOpen             all
 * $VICERES RefreshOnBreak              all
 * $VICERES MonitorServer               all
 * $VICERES MonitorServerAddress        all
 * $VICERES BinaryMonitorServer         all
 * $VICERES BinaryMonitorServerAddress  all
 * $VICERES NativeMonitor               all
 * $VICERES MonitorLogEnabled           all
 * $VICERES MonitorLogFileName          all
 * $VICERES MonitorChisLines            all
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
#include "resources.h"
#include "uimon.h"

#include "settings_monitor.h"


/** \brief  Row number for the various widgets
 *
 * Using this enum makes it easy to reorder widgets and handle CHIS/no-CHIS.
 */
enum {
    ROW_NATIVE = 0,             /**< row for 'use native monitor' */
    ROW_KEEP_OPEN,              /**< row for 'keep monitor open' */
    ROW_REFRESH_ON_BREAK,       /**< row for 'Refresh display after command' */
    ROW_SERVER_ENABLE,          /**< row for 'enable monitor server' */
    ROW_SERVER_ADDRESS,         /**< row for 'monitor server address' */
    ROW_BINARY_SERVER_ENABLE,   /**< row for 'enable monitor server' */
    ROW_BINARY_SERVER_ADDRESS,  /**< row for 'monitor server address' */
    ROW_LOG_ENABLE,             /**< row for 'enable logging to a file' */
    ROW_LOG_NAME,               /**< row for 'log filename */
    ROW_SCROLL_LINES,           /**< row for 'scrollback buffer lines' */
#ifdef FEATURE_CPUMEMHISTORY
    ROW_CHIS_LINES,             /**< row for 'cpu history lines */
    ROW_CHIS_WARNING,           /**< row for warning about clearing chis */
#endif
    ROW_FONT                    /**< row for 'monitor font' */
};


/** \brief  Handler for the 'font-set' event of the font chooser
 *
 * \param[in]   button      font chooser button
 * \param[in]   user_data   extra event data (unused)
 */
static void on_font_set(GtkFontButton *button, gpointer user_data)
{
    gchar *font_desc;

    debug_gtk3("Called.");
    font_desc = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(button));
    debug_gtk3("font desc = '%s'", font_desc);
    if (font_desc != NULL) {
        if (resources_set_string("MonitorFont", font_desc) == 0) {
            /* try to 'live-update' the monitor font */
            uimon_set_font();
        }
        g_free(font_desc);
    }
}



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
    GtkWidget *refresh_on_break;
    GtkWidget *server_enable;
    GtkWidget *server_address;
    GtkWidget *label;
    GtkWidget *binary_server_enable;
    GtkWidget *binary_server_address;
    GtkWidget *binary_label;
    GtkWidget *log_enable;
    GtkWidget *log_name;
    GtkWidget *log_label;
    GtkWidget *scroll_lines;
    GtkWidget *scroll_label;
#ifdef FEATURE_CPUMEMHISTORY
    GtkWidget *chis_lines;
    GtkWidget *chis_label;
    GtkWidget *chis_warning;
#endif
    GtkWidget *font_label;
    GtkWidget *font_button;

    const char *font_name = NULL;

    debug_gtk3("Getting MonitorFont resource");
    resources_get_string("MonitorFont", &font_name);
    debug_gtk3("Font name = '%s'", font_name);

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    native = vice_gtk3_resource_check_button_new("NativeMonitor",
            "Use native monitor interface");
    keep_open = vice_gtk3_resource_check_button_new("KeepMonitorOpen",
            "Keep monitor open");
    refresh_on_break = vice_gtk3_resource_check_button_new("RefreshOnBreak",
            "Refresh display after command");

    server_enable = vice_gtk3_resource_check_button_new("MonitorServer",
            "Enable remote monitor");
    label = gtk_label_new("Server address");
    /* align with the rest, more or less */
    g_object_set(label, "margin-left", 8, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    server_address = vice_gtk3_resource_entry_full_new(
            "MonitorServerAddress");
    gtk_widget_set_hexpand(server_address, TRUE);


    binary_server_enable = vice_gtk3_resource_check_button_new("BinaryMonitorServer",
            "Enable binary remote monitor");
    binary_label = gtk_label_new("Server address");
    /* align with the rest, more or less */
    g_object_set(binary_label, "margin-left", 8, NULL);
    gtk_widget_set_halign(binary_label, GTK_ALIGN_START);
    binary_server_address = vice_gtk3_resource_entry_full_new(
            "BinaryMonitorServerAddress");
    gtk_widget_set_hexpand(binary_server_address, TRUE);

    log_enable = vice_gtk3_resource_check_button_new("MonitorLogEnabled",
            "Enable logging to a file");
    log_label = gtk_label_new("Logfile name");
    /* align with the rest, more or less */
    g_object_set(log_label, "margin-left", 8, NULL);
    gtk_widget_set_halign(log_label, GTK_ALIGN_START);
#if 0
    log_name = vice_gtk3_resource_entry_full_new(
            "MonitorLogFileName");
#else
    log_name = vice_gtk3_resource_browser_save_new(
            "MonitorLogFileName",
            "Select monitor log filename",
            NULL,
            NULL,
            NULL);
#endif
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
    chis_warning = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(chis_warning),
            "<i>Changing this setting will clear the CPU history</i>");
#endif

    /* font selection label and button */
    font_label = gtk_label_new("Monitor font");
    g_object_set(font_label, "margin-left", 8, NULL);
    gtk_widget_set_halign(font_label, GTK_ALIGN_START);

    /* create button that pops up a font selector */
    font_button = gtk_font_button_new();
    gtk_font_button_set_use_font(GTK_FONT_BUTTON(font_button), TRUE);
    if (font_name != NULL) {
        gtk_font_chooser_set_font(GTK_FONT_CHOOSER(font_button), font_name);
    }
    g_signal_connect(font_button, "font-set", G_CALLBACK(on_font_set), NULL);

    gtk_grid_attach(GTK_GRID(grid), native, 0, ROW_NATIVE, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), keep_open, 0, ROW_KEEP_OPEN, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), refresh_on_break, 0, ROW_REFRESH_ON_BREAK, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), server_enable, 0, ROW_SERVER_ENABLE, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), label, 0, ROW_SERVER_ADDRESS, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), server_address, 1, ROW_SERVER_ADDRESS, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), binary_server_enable, 0, ROW_BINARY_SERVER_ENABLE, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), binary_label, 0, ROW_BINARY_SERVER_ADDRESS, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), binary_server_address, 1, ROW_BINARY_SERVER_ADDRESS, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), log_enable, 0, ROW_LOG_ENABLE, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), log_label, 0, ROW_LOG_NAME, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), log_name, 1, ROW_LOG_NAME, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll_label, 0, ROW_SCROLL_LINES, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), scroll_lines, 1, ROW_SCROLL_LINES, 1, 1);
#ifdef FEATURE_CPUMEMHISTORY
    gtk_grid_attach(GTK_GRID(grid), chis_label, 0, ROW_CHIS_LINES, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chis_lines, 1, ROW_CHIS_LINES, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chis_warning, 1, ROW_CHIS_WARNING ,1 ,1);
#endif
    gtk_grid_attach(GTK_GRID(grid), font_label, 0, ROW_FONT, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), font_button, 1, ROW_FONT, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
