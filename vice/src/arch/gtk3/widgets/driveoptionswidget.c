/** \file   src/arch/gtk3/driveoptionswidget.c
 * \brief   Drive options widget
 *
 * Controls the following resource(s):
 *  IECDevice[8-11]
 *  AttachDevice[8-11]Readonly
 *  FSDevice[8-11]Dir
 *  FSDevice[8-11]ConvertP00
 *  FSDevice[8-11]SaveP00
 *  FSDevice[8-11]HideCBMFiles
 *  Drive[8-11]RPM
 *  Drive[8-11]Wobble
 *  Drive[8-11]RTCSave
 *
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
#include "drive.h"
#include "drive-check.h"
#include "machine.h"
#include "resources.h"
#include "widgethelpers.h"
#include "drivewidgethelpers.h"
#include "selectdirectorydialog.h"

#include "driveoptionswidget.h"


/** \brief  Unit currently having its settings changed
 */
static int unit_number = 8;


/** Row number of the various widgets
 */
enum {
    ROW_IEC = 1,
    ROW_READONLY,
    ROW_FSDIR,
    ROW_P00CONVERT,
    ROW_P00CREATE,
    ROW_P00ONLY,
    ROW_RPM,
    ROW_RTC
};


/*
 * References to widget, to be used in update_drive_options_widget()
 */
static GtkWidget *iec_widget = NULL;
static GtkWidget *readonly_widget = NULL;
static GtkWidget *fsdir_widget = NULL;
static GtkWidget *p00convert_widget = NULL;
static GtkWidget *p00create_widget = NULL;
static GtkWidget *p00only_widget = NULL;
static GtkWidget *rpm_widget = NULL;
static GtkWidget *rtc_widget = NULL;


static void on_iec_toggled(GtkWidget *widget, gpointer user_data)
{
    uihelpers_set_drive_resource_from_check_button(
            widget, "IECDevice%d", unit_number);
}

static void on_readonly_toggled(GtkWidget *widget, gpointer user_data)
{
    uihelpers_set_drive_resource_from_check_button(
            widget, "AttachDevice%dReadonly",
            unit_number);
}


static void on_p00convert_toggled(GtkWidget *widget, gpointer user_data)
{
    uihelpers_set_drive_resource_from_check_button(
            widget, "FSDevice%dConvertP00",
            unit_number);
}


static void on_p00create_toggled(GtkWidget *widget, gpointer user_data)
{
    uihelpers_set_drive_resource_from_check_button(
            widget, "FSDevice%dSaveP00",
            unit_number);
}

static void on_p00only_toggled(GtkWidget *widget, gpointer user_data)
{
    uihelpers_set_drive_resource_from_check_button(
            widget, "FSDevice%dHideCBMFiles",
            unit_number);
}


static void on_rtc_toggled(GtkWidget *widget, gpointer user_data)
{
    uihelpers_set_drive_resource_from_check_button(
            widget, "Drive%dRTCSave",
            unit_number);
}


/** \brief  Handler for the "changed" event of the RPM spinbox
 *
 * \param[in]   widget      RPM spin button
 * \param[in[   user_data   data for event (unused)
 */
static void on_rpm_changed(GtkWidget *widget, gpointer user_data)
{
    int value;
    char res_name[256];

    g_snprintf(res_name, 256, "Drive%dRPM", unit_number);
    value = (int)(100.0 * gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));
    debug_gtk3("setting %s to %d\n", res_name, value);
    resources_set_int(res_name, value);
}


/** \brief  Handler for the "changed" event of the Wobble spinbox
 *
 * \param[in]   widget      Wobble spin button
 * \param[in[   user_data   data for event (unused)
 */
static void on_wobble_changed(GtkWidget *widget, gpointer user_data)
{
    int value;
    char res_name[256];

    g_snprintf(res_name, 256, "Drive%dWobble", unit_number);
    value = (int)(100.0 * gtk_spin_button_get_value(GTK_SPIN_BUTTON(widget)));
    debug_gtk3("setting %s to %d\n", res_name, value);
    resources_set_int(res_name, value);
}


static void on_fsdir_button_clicked(GtkWidget *widget, gpointer user_data)
{
    gchar *filename;

    /* XXX: On my box, using Gtk3 3.22.11, and allowing CREATE, the first click
     *      on a directory doesn't enable the "Select" button in the dialog for
     *      some obscure reason, disallowing CREATE makes the selection work on
     *      the first click -- compyx
     */
    filename = ui_select_directory_dialog(widget, "Select directory", NULL,
            TRUE);
    debug_gtk3("got directory '%s'\n", filename ? filename : "NULL");
    if (filename != NULL) {
        GtkWidget *entry = gtk_grid_get_child_at(GTK_GRID(fsdir_widget), 1, 0);
        gtk_entry_set_text(GTK_ENTRY(entry), filename);

        g_free(filename);
    }
}


static void on_fsdir_entry_changed(GtkWidget *widget, gpointer user_data)
{
    GtkEntry *entry = GTK_ENTRY(widget);
    const char *path = gtk_entry_get_text(entry);

    debug_gtk3("setting FSDevice%dDir to '%s'\n", unit_number, path);
    resources_set_string_sprintf("FSDevice%dDir", path, unit_number);
}




/** \brief  Handler for the "destroy" event of the widget
 *
 * Sets widget references to `NULL`, this shouldn't be needed once I move
 * the g_signal_connect() calls to their proper place
 *
 * \param[in]   widget      widget triggering the event
 * \param[in]   user_data   data for the event (unused)
 */
static void on_destroy(GtkWidget *widget, gpointer user_data)
{
    iec_widget = NULL;
    readonly_widget = NULL;
    fsdir_widget = NULL;
    p00convert_widget = NULL;
    p00create_widget = NULL;
    p00only_widget = NULL;
    rpm_widget = NULL;
    rtc_widget = NULL;
}


static GtkWidget *create_iec_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Enable IEC Device");

    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


static GtkWidget *create_readonly_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Read only access");

    g_object_set(check, "margin-left", 16, NULL);
    return check;
}



static GtkWidget *create_fsdir_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *button;
    const char *value = NULL;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("FS device dir");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    /* text entry box */
    entry = gtk_entry_new();
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);
    resources_get_string_sprintf("FSDevice%dDir", &value, unit_number);
    if (value != NULL) {
        gtk_entry_set_text(GTK_ENTRY(entry), value);
    }

    /* "Browse" button */
    button = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), button, 2, 0, 1, 1);

    /* hook up signal handlers */
    g_signal_connect(entry, "changed", G_CALLBACK(on_fsdir_entry_changed),
            NULL);
    g_signal_connect(button, "clicked", G_CALLBACK(on_fsdir_button_clicked),
            NULL);

    gtk_widget_show_all(grid);
    return grid;
}



static GtkWidget *create_p00convert_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Convert P00 file names");

    g_object_set(check, "margin-left", 16, NULL);
    return check;
}

static GtkWidget *create_p00create_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Create P00 files on save");

    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


static GtkWidget *create_p00only_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Hide non-P00 files");

    g_object_set(check, "margin-left", 16, NULL);
    return check;
}


/** \brief  Create widget to control RPM and wobble settings
 *
 * The GtkSpinButton for RPM is at (0, 1) in the grid, the GtkSpinButton for
 * Wobble is at (1, 1) in the grid.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_rpm_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *spin;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Drive RPM");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    spin = gtk_spin_button_new_with_range(275.0, 325.0, 0.25);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 2);
    gtk_grid_attach(GTK_GRID(grid), spin, 1, 0, 1, 1);

    label = gtk_label_new("RPM wobble");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);

    spin = gtk_spin_button_new_with_range(0.0, 10.0, 0.1);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 2);
    gtk_grid_attach(GTK_GRID(grid), spin, 1, 1, 1, 1);



    gtk_widget_show_all(grid);

    return grid;
}



static GtkWidget *create_rtc_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Enable RTC saving");

    g_object_set(check, "margin-left", 16, NULL);

    return check;
}



static void connect_signal_handlers(void)
{
    GtkWidget *spin;

    g_signal_connect(iec_widget, "toggled",
            G_CALLBACK(on_iec_toggled), NULL);
    g_signal_connect(readonly_widget, "toggled",
            G_CALLBACK(on_readonly_toggled), NULL);
    g_signal_connect(p00convert_widget, "toggled",
            G_CALLBACK(on_p00convert_toggled), NULL);
    g_signal_connect(p00create_widget, "toggled",
            G_CALLBACK(on_p00create_toggled), NULL);
    g_signal_connect(p00only_widget, "toggled",
            G_CALLBACK(on_p00only_toggled), NULL);

    /* RPM spinbox */
    spin = gtk_grid_get_child_at(GTK_GRID(rpm_widget), 1, 0);
    g_signal_connect(spin, "value-changed",
            G_CALLBACK(on_rpm_changed), NULL);

    /* Wobble spinbox */
    spin = gtk_grid_get_child_at(GTK_GRID(rpm_widget), 1, 1);
    g_signal_connect(spin, "value-changed",
            G_CALLBACK(on_wobble_changed), NULL);


    g_signal_connect(rtc_widget, "toggled", G_CALLBACK(on_rtc_toggled), NULL);

}



/** \brief  Create drive options widget for \a unit
 *
 * \param[in]   unit    drive unit number
 *
 * \return GtkGrid
 *
 * TODO:    move signal handler setup further down to avoid calling resource
 *          setters triggering those callbacks
 */
GtkWidget *create_drive_options_widget(int unit)
{
    GtkWidget *grid;

    unit_number = unit;

    grid = uihelpers_create_grid_with_label("Options", 1);

    iec_widget = create_iec_check_button();
    gtk_grid_attach(GTK_GRID(grid), iec_widget, 0, ROW_IEC, 1, 1);

    readonly_widget = create_readonly_check_button();
    gtk_grid_attach(GTK_GRID(grid), readonly_widget, 0, ROW_READONLY, 1, 1);

    fsdir_widget = create_fsdir_widget();
    gtk_grid_attach(GTK_GRID(grid), fsdir_widget, 0, ROW_FSDIR, 1, 1);

    p00convert_widget = create_p00convert_check_button();
    gtk_grid_attach(GTK_GRID(grid), p00convert_widget, 0, ROW_P00CONVERT, 1, 1);

    p00create_widget = create_p00create_check_button();
    gtk_grid_attach(GTK_GRID(grid), p00create_widget, 0, ROW_P00CREATE, 1, 1);

    p00only_widget = create_p00only_check_button();
    gtk_grid_attach(GTK_GRID(grid), p00only_widget, 0, ROW_P00ONLY, 1, 1);

    rpm_widget = create_rpm_widget();
    gtk_grid_attach(GTK_GRID(grid), rpm_widget, 0, ROW_RPM, 1, 1);

    rtc_widget = create_rtc_check_button();
    gtk_grid_attach(GTK_GRID(grid), rtc_widget, 0, ROW_RTC, 1, 1);

    /* set current values */
    update_drive_options_widget(grid, unit);

    /* now set up signals */
    connect_signal_handlers();

    g_signal_connect(grid, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the options widget using the resources for \a unit
 *
 * \param[in,out]   widget  drive options widget
 * \param[in]       unit    drive unit number (8-11)
 */
void update_drive_options_widget(GtkWidget *widget, int unit)
{
    unit_number = unit;

    int value;
    char res_name[256];
    GtkWidget *spin;

    uihelpers_get_drive_resource_from_check_button(
            iec_widget, "IECDevice%d", unit);
    uihelpers_get_drive_resource_from_check_button(
            readonly_widget, "AttachDevice%dReadonly", unit);

    /* RPM */
    g_snprintf(res_name, 256, "Drive%dRPM", unit);
    resources_get_int(res_name, &value);
    spin = gtk_grid_get_child_at(GTK_GRID(rpm_widget), 1, 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)(value / 100));

    /* Wobble */
    g_snprintf(res_name, 256, "Drive%dWobble", unit);
    resources_get_int(res_name, &value);
    spin = gtk_grid_get_child_at(GTK_GRID(rpm_widget), 1, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)(value / 100));

    /* RTC */
    if (drive_check_rtc(ui_get_drive_type(unit))) {
        /* supported */
        int state;

        resources_get_int_sprintf("Drive%dRTCSave", &state, unit);
        gtk_widget_set_sensitive(rtc_widget, TRUE);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rtc_widget), state);
    } else {
        gtk_widget_set_sensitive(rtc_widget, FALSE);
    }
}

