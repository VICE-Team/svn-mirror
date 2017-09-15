/** \file   src/arch/gtk3/driveoptionswidget.c
 * \brief   Drive options widgeg
 *
 * Controls the following resource(s):
 *  IECDevice[8-11]
 *  AttachDevice[8-11]Readonly
 *  FSDevice[8-11]Dir
 *  FSDevice[8-11]ConvertP00
 *  FSDevice[8-11]SaveP00
 *  FSDevice[8-11]HideCBMFiles
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

#include "driveoptionswidget.h"


/** \brief  Unit currently having its settings changed
 */
static int unit_number = 8;



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
    return;
}

static void on_readonly_toggled(GtkWidget *widget, gpointer user_data)
{
    return;
}

static void on_p00convert_toggled(GtkWidget *widget, gpointer user_data)
{
    return;
}

static void on_p00create_toggled(GtkWidget *widget, gpointer user_data)
{
    return;
}

static void on_p00only_toggled(GtkWidget *widget, gpointer user_data)
{
    return;
}


static void on_rtc_toggled(GtkWidget *widget, gpointer user_data)
{
    return;
}



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
    g_signal_connect(check, "toggled", G_CALLBACK(on_iec_toggled), NULL);
    return check;
}


static GtkWidget *create_readonly_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Read only access");

    g_object_set(check, "margin-left", 16, NULL);
    g_signal_connect(check, "toggled", G_CALLBACK(on_readonly_toggled), NULL);
    return check;
}



static GtkWidget *create_fsdir_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *button;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("FS device dir");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    entry = gtk_entry_new();
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);

    button = gtk_button_new_with_label("Browse ...");
    gtk_grid_attach(GTK_GRID(grid), button, 2, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}



static GtkWidget *create_p00convert_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Convert P00 file names");

    g_object_set(check, "margin-left", 16, NULL);
    g_signal_connect(check, "toggled", G_CALLBACK(on_p00convert_toggled), NULL);
    return check;
}

static GtkWidget *create_p00create_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Create P00 files on save");

    g_object_set(check, "margin-left", 16, NULL);
    g_signal_connect(check, "toggled", G_CALLBACK(on_p00create_toggled), NULL);
    return check;
}


static GtkWidget *create_p00only_check_button(void)
{
    GtkWidget *check = gtk_check_button_new_with_label("Hide non-P00 files");

    g_object_set(check, "margin-left", 16, NULL);
    g_signal_connect(check, "toggled", G_CALLBACK(on_p00only_toggled), NULL);
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
    g_signal_connect(check, "toggled", G_CALLBACK(on_rtc_toggled), NULL);

    return check;
}




GtkWidget *create_drive_options_widget(int unit)
{
    GtkWidget *grid;

    unit_number = unit;

    grid = uihelpers_create_grid_with_label("Options", 1);

    iec_widget = create_iec_check_button();
    gtk_grid_attach(GTK_GRID(grid), iec_widget, 0, 1, 1, 1);

    readonly_widget = create_readonly_check_button();
    gtk_grid_attach(GTK_GRID(grid), readonly_widget, 0, 2, 1, 1);

    fsdir_widget = create_fsdir_widget();
    gtk_grid_attach(GTK_GRID(grid), fsdir_widget, 0, 3, 1, 1);

    p00convert_widget = create_p00convert_check_button();
    gtk_grid_attach(GTK_GRID(grid), p00convert_widget, 0, 4, 1, 1);

    p00create_widget = create_p00create_check_button();
    gtk_grid_attach(GTK_GRID(grid), p00create_widget, 0, 5, 1, 1);

    p00only_widget = create_p00only_check_button();
    gtk_grid_attach(GTK_GRID(grid), p00only_widget, 0, 6, 1, 1);

    rpm_widget = create_rpm_widget();
    gtk_grid_attach(GTK_GRID(grid), rpm_widget, 0, 7, 1, 1);

    rtc_widget = create_rtc_check_button();
    gtk_grid_attach(GTK_GRID(grid), rtc_widget, 0, 8, 1, 1);

    g_signal_connect(grid, "destroy", G_CALLBACK(on_destroy), NULL);

    gtk_widget_show_all(grid);
    return grid;
}


void update_drive_options_widget(GtkWidget *widget, int unit)
{
    return;
}

