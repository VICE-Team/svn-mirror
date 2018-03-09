/** \file   tapeportdeviceswidget.c
 * \brief   Tape port devices widget
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Datasette           (all except vsid)
 *  TapeLog             (all except vsid)
 *  TapeLogDestination  (all except vsid)
 *  CPClockF83          (all except vsid)
 *  CPClockF83Save      (all except vsid)
 *  TapeSenseDongle     (all except vsid)
 *  DTLBasicDongle      (all except vsid)
 *  Tapecart            (all except vsid)
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
#include <stdlib.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "savefiledialog.h"
#include "basedialogs.h"

#include "tapeportdeviceswidget.h"


/*
 * Reference to widgets to be able to enable/disabled them through event
 * handlers
 */
static GtkWidget *tape_log = NULL;
static GtkWidget *tape_log_dest = NULL;
static GtkWidget *tape_log_filename = NULL;
static GtkWidget *tape_log_browse = NULL;

static GtkWidget *f83_enable = NULL;
static GtkWidget *f83_rtc = NULL;

static GtkWidget *tapecart_enable = NULL;
static GtkWidget *tapecart_update = NULL;
static GtkWidget *tapecart_optimize = NULL;
static GtkWidget *tapecart_filename = NULL;
static GtkWidget *tapecart_browse = NULL;
static GtkWidget *tapecart_flush = NULL;

static int (*tapecart_flush_func)(void) = NULL;


/** \brief  Handler for the "toggled" event of the tape_log check button
 *
 * Enables/disables tape_log_dest/tape_log_file_name/tape_log_browse widgets
 *
 * \param[in]   widget      tape_log check button
 * \param[in]   user_data   unused
 */
static void on_tape_log_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(tape_log_dest, state);
    gtk_widget_set_sensitive(tape_log_filename, state);
    gtk_widget_set_sensitive(tape_log_browse, state);
}


/** \brief  Handler for the "toggled" event of the tape_log_dest check button
 *
 * Enables/disables tape_log_file_name/tape_log_browse widgets
 *
 * \param[in]   widget      tape_log check button
 * \param[in]   user_data   unused
 */
static void on_tape_log_dest_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(tape_log_filename, state);
    gtk_widget_set_sensitive(tape_log_browse, state);
}


/** \brief  Handler for the "clicked" event of the tape log browse button
 *
 * \param[in]   widget      tape log browse button
 * \param[in]   user_data   unused
 */
static void on_tape_log_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;

    /* TODO: use existing filename, if any */
    filename = vice_gtk3_save_file_dialog("Select/Create tape log file", NULL,
            TRUE, NULL);
    if (filename != NULL) {
        /* TODO: check if file is writable */
        gtk_entry_set_text(GTK_ENTRY(tape_log_filename), filename);
        g_free(filename);
    }
}


/** \brief  Handler for the "toggled" event of the CP Clock F83 check button
 *
 * \param[in]   widget      CP Clock F83 enable check button
 * \param[in]   user_data   unused
 */
static void on_f83_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gtk_widget_set_sensitive(f83_rtc,
            gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}


/** \brief  Handler for the "toggled" event of the tapecart check button
 *
 * \param[in]   widget      tapecart enable check button
 * \param[in]   user_data   unused
 */
static void on_tapecart_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
    gtk_widget_set_sensitive(tapecart_update, state);
    gtk_widget_set_sensitive(tapecart_optimize, state);
    gtk_widget_set_sensitive(tapecart_filename, state);
    gtk_widget_set_sensitive(tapecart_browse, state);
    gtk_widget_set_sensitive(tapecart_flush, state);
}


/** \brief  Handler for the "clicked" event of the tapecart browse button
 *
 * \param[in]   widget      tapecart browse button
 * \param[in]   user_data   unused
 */
static void on_tapecart_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;

    /* TODO: use existing filename, if any */
    filename = vice_gtk3_open_file_dialog("Select tapecart file", NULL,
            NULL, NULL);
    if (filename != NULL) {
        vice_gtk3_resource_entry_full_set(tapecart_filename, filename);
        g_free(filename);
    }
}


/** \brief  Handler for the 'clicked' event of the tapecart flush button
 *
 * \param[in]   widget  button (unused)
 * \param[in]   data    extra event data (unused)
 */
static void on_tapecart_flush_clicked(GtkWidget *widget, gpointer data)
{
    debug_gtk3("Attempting to flush current tapecart image\n");
    if (tapecart_flush_func == NULL) {
        debug_gtk3("Failed: please set the tapecart flush function with "
                "tapeport_devices_widget_set_tapecart_flush_func()\n");
        return;
    }
    if (tapecart_flush_func() == 0) {
        debug_gtk3("OK\n");
    } else {
        debug_gtk3("Failed\n");
    }
}


/** \brief  Create check button for the "Datasette" resource
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_datasette_widget(void)
{
    return vice_gtk3_resource_check_button_new("Datasette", "Enable Datasette");
}


/** \brief  Create check button for the "TapeSenseDongle" resource
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_tape_sense_widget(void)
{
    return vice_gtk3_resource_check_button_new("TapeSenseDongle",
            "Enable tape sense dongle");
}


/** \brief  Create check button for the "DTLBasicDongle" resource
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_dtl_basic_widget(void)
{
    return vice_gtk3_resource_check_button_new("DTLBasicDongle",
            "Enable DTL Basic dongle");
}


/** \brief  Create widget to control tape logging
 *
 * \return  GtkGrid
 */
static GtkWidget *create_tape_log_widget(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);

    tape_log = vice_gtk3_resource_check_button_new("TapeLog", "Enable tape log device");
    gtk_grid_attach(GTK_GRID(grid), tape_log, 0, 0, 3, 1);

    tape_log_dest = vice_gtk3_resource_check_button_new("TapeLogDestination",
            "Save to user file");
    g_object_set(tape_log_dest, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), tape_log_dest, 0, 1, 1, 1);

    tape_log_filename = vice_gtk3_resource_entry_full_new("TapeLogFilename");
    gtk_widget_set_hexpand(tape_log_filename, TRUE);
    gtk_grid_attach(GTK_GRID(grid), tape_log_filename, 1, 1, 1, 1);

    tape_log_browse = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), tape_log_browse, 2, 1, 1, 1);

    g_signal_connect(tape_log_browse, "clicked",
            G_CALLBACK(on_tape_log_browse_clicked), NULL);

    g_signal_connect(tape_log_dest, "toggled",
            G_CALLBACK(on_tape_log_dest_toggled), NULL);

    g_signal_connect(tape_log, "toggled",
            G_CALLBACK(on_tape_log_toggled), NULL);


    on_tape_log_dest_toggled(tape_log_dest, NULL);
    on_tape_log_toggled(tape_log, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to handler the CP Clock F83 resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_f83_widget(void)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);

    f83_enable = vice_gtk3_resource_check_button_new("CPCLockF83",
            "Enable CP Clock F83");
    gtk_grid_attach(GTK_GRID(grid), f83_enable, 0, 0, 1, 1);

    f83_rtc = vice_gtk3_resource_check_button_new("CPClockF83Save",
            "Save RTC data when changed");
    g_object_set(f83_rtc, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), f83_rtc, 0, 1, 1, 1);

    g_signal_connect(f83_enable, "toggled", G_CALLBACK(on_f83_enable_toggled),
            NULL);

    on_f83_enable_toggled(f83_enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to handle the tapecart resources
 *
 * \return  GtkGrid
 */
static GtkWidget *create_tapecart_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);

    tapecart_enable = vice_gtk3_resource_check_button_new("TapecartEnabled",
            "Enable tapecart");
    gtk_grid_attach(GTK_GRID(grid), tapecart_enable, 0, 0, 1, 1);

    tapecart_update = vice_gtk3_resource_check_button_new(
            "TapecartUpdateTCRT", "Save TCRT data when changed");
    g_object_set(tapecart_update, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), tapecart_update, 0, 1, 1, 1);

    tapecart_optimize = vice_gtk3_resource_check_button_new(
            "TapecartOptimizeTCRT", "Optimize TCRT data when changed");
    g_object_set(tapecart_optimize, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), tapecart_optimize, 0, 2, 1, 1);

    label = gtk_label_new("TCRT Filename:");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);

    tapecart_filename = vice_gtk3_resource_entry_full_new(
            "TapecartTCRTFilename");
    g_object_set(tapecart_filename, "margin-left", 16, NULL);
    gtk_widget_set_hexpand(tapecart_filename, TRUE);
    gtk_grid_attach(GTK_GRID(grid), tapecart_filename, 0, 4, 1, 1);

    tapecart_browse = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), tapecart_browse, 1, 4, 1, 1);

    tapecart_flush = gtk_button_new_with_label("Flush image");
    gtk_grid_attach(GTK_GRID(grid), tapecart_flush, 1, 5, 1, 1);

    g_signal_connect(tapecart_enable, "toggled",
            G_CALLBACK(on_tapecart_enable_toggled), NULL);
    g_signal_connect(tapecart_browse, "clicked",
            G_CALLBACK(on_tapecart_browse_clicked), NULL);
    g_signal_connect(tapecart_flush, "clicked",
            G_CALLBACK(on_tapecart_flush_clicked), NULL);

    on_tapecart_enable_toggled(tapecart_enable, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to select/control tape port devices
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *tapeport_devices_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 16);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    gtk_grid_attach(GTK_GRID(grid), create_datasette_widget(), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_tape_sense_widget(), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_dtl_basic_widget(), 0, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), create_tape_log_widget(), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_f83_widget(), 0, 4, 1, 1);
    if (machine_class == VICE_MACHINE_C64
            || machine_class == VICE_MACHINE_C64SC
            || machine_class == VICE_MACHINE_C128) {

        gtk_grid_attach(GTK_GRID(grid), create_tapecart_widget(), 0, 5, 1, 1);
        gtk_widget_show_all(grid);
    }
    return grid;
}


/** \brief  Set the tapecart flush function
 *
 * This is required to work around vsid not linking against tapecart.
 *
 * \param[in]   func    tapecart flush function
 */
void tapeport_devices_widget_set_tapecart_flush_func(int (*func)(void))
{
    tapecart_flush_func = func;
}
