/** \file   settings_drive.c
 * \brief   Drive settings dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES VirtualDevice8              -vsid
 * $VICERES VirtualDevice9              -vsid
 * $VICERES VirtualDevice10             -vsid
 * $VICERES VirtualDevice11             -vsid
 * $VICERES Drive8TrueEmulation         -vsid
 * $VICERES Drive9TrueEmulation         -vsid
 * $VICERES Drive10TrueEmulation        -vsid
 * $VICERES Drive11TrueEmulation        -vsid
 * $VICERES DriveSoundEmulation         -vsid
 * $VICERES DriveSoundEmulationVolume   -vsid
 * $VICERES IECDevice8                  -vsid -xcbm5x0 -xcbm2 -xpet -xvic
 * $VICERES IECDevice9                  -vsid -xcbm5x0 -xcbm2 -xpet -xvic
 * $VICERES IECDevice10                 -vsid -xcbm5x0 -xcbm2 -xpet -xvic
 * $VICERES IECDevice11                 -vsid -xcbm5x0 -xcbm2 -xpet -xvic
 * $VICERES FileSystemDevice8           -vsid
 * $VICERES FileSystemDevice9           -vsid
 * $VICERES FileSystemDevice10          -vsid
 * $VICERES FileSystemDevice11          -vsid
 * $VICERES AttachDevice8d0Readonly     -vsid
 * $VICERES AttachDevice9d0Readonly     -vsid
 * $VICERES AttachDevice10d0Readonly    -vsid
 * $VICERES AttachDevice11d0Readonly    -vsid
 * $VICERES AttachDevice8d1Readonly     -vsid
 * $VICERES AttachDevice9d1Readonly     -vsid
 * $VICERES AttachDevice10d1Readonly    -vsid
 * $VICERES AttachDevice11d1Readonly    -vsid
 * $VICERES Drive8RTCSave               -vsid -xcbm5x0 -xcbm2 -xpet
 * $VICERES Drive9RTCSave               -vsid -xcbm5x0 -xcbm2 -xpet
 * $VICERES Drive10RTCSave              -vsid -xcbm5x0 -xcbm2 -xpet
 * $VICERES Drive11RTCSave              -vsid -xcbm5x0 -xcbm2 -xpet
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

#include "attach.h"
#include "debug_gtk3.h"
#include "drive-check.h"
#include "drive.h"
#include "drivedoswidget.h"
#include "driveextendpolicywidget.h"
#include "drivefixedsizewidget.h"
#include "driveidlemethodwidget.h"
#include "drivemodelwidget.h"
#include "driveparallelcablewidget.h"
#include "driveramwidget.h"
#include "driverpmwidget.h"
#include "driveunitwidget.h"
#include "drivewidgethelpers.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "settings_drive.h"


/** \brief  List of (virtual) file system types
 */
static const vice_gtk3_combo_entry_int_t device_types[] = {
    { "Disk images",            ATTACH_DEVICE_NONE },
    { "Host file system",       ATTACH_DEVICE_FS },
#ifdef HAVE_REALDEVICE
    { "Real device (OpenCBM)",  ATTACH_DEVICE_REAL },
#endif
    { NULL,                     -1 }
};


/* Widget references for the glue logic that updates sensitivity of those widgets
 * on drive model changes and IEC device check button toggling.
 */

/** \brief  Drive model widgets */
static GtkWidget *drive_model[NUM_DISK_UNITS];

/** \brief  TDE check buttons */
static GtkWidget *drive_tde[NUM_DISK_UNITS];

/** \brief  Virtual device check buttons */
static GtkWidget *drive_virtualdev[NUM_DISK_UNITS];

/** \brief  Read-only device check buttons */
static GtkWidget *drive_read_only[NUM_DISK_UNITS][2];

/** \brief  Real time clock save check buttons */
static GtkWidget *drive_rtc_save[NUM_DISK_UNITS];

/** \brief  IEC device check buttons */
static GtkWidget *drive_iec_device[NUM_DISK_UNITS];

/** \brief  Drive extend-policy widgets */
static GtkWidget *drive_extend[NUM_DISK_UNITS];

/** \brief  Drive idle-mode widgets */
static GtkWidget *drive_idle[NUM_DISK_UNITS];

/** \brief  Drive parallel port widgets */
static GtkWidget *drive_parallel[NUM_DISK_UNITS];

/** \brief  Drive RPM widgets */
static GtkWidget *drive_rpm[NUM_DISK_UNITS];

/** \brief  Drive RAM widgets */
static GtkWidget *drive_ram[NUM_DISK_UNITS];

/** \brief  Drive DOS widgets */
static GtkWidget *drive_dos[NUM_DISK_UNITS];

/** \brief  Drive device type widgets */
static GtkWidget *drive_device_type[NUM_DISK_UNITS];

/** \brief  Drive device type labels */
static GtkWidget *drive_device_type_label[NUM_DISK_UNITS];

/** \brief  Drive fixed-size widgets */
static GtkWidget *drive_size[NUM_DISK_UNITS];


/*
 * Gtk event handlers and custom widget callbacks
 */

/** \brief  Custom callback for the IEC widget in driveoptions.c
 *
 * \param[in]   widget  IEC toggle button
 * \param[in]   unit    unit number (8-11)
 */
static void iec_callback(GtkWidget *widget, int unit)
{
    if (unit >= DRIVE_UNIT_MIN && unit <= DRIVE_UNIT_MAX) {
        int iecdev = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        int virtdev = 0;
        int index = unit - DRIVE_UNIT_MIN;

        if (resources_get_int_sprintf("VirtualDevice%d", &virtdev, unit) < 0) {
            virtdev = 0;
        }
        gtk_widget_set_sensitive(drive_device_type_label[index], iecdev | virtdev);
        gtk_widget_set_sensitive(drive_device_type[index], iecdev | virtdev);
    }
}


/** \brief  Extra event handler for the drive model changes
 *
 * \param[in]   widget      drive type radio button
 * \param[in]   user_data   unit number
 */
static void on_model_changed(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *option;
    int unit = GPOINTER_TO_INT(user_data);
    int model = drive_model_widget_value_combo(widget);

    option = drive_read_only[unit - DRIVE_UNIT_MIN][1];
    if (option != NULL) {
        gtk_widget_set_sensitive(option, drive_check_dual(model));
    }

    option = drive_extend[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        gtk_widget_set_sensitive(option, drive_check_extend_policy(model));
    }
    option = drive_parallel[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        gtk_widget_set_sensitive(option, drive_check_parallel_cable(model));
    }
    option = drive_idle[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        gtk_widget_set_sensitive(option, drive_check_idle_method(model));
    }

    /* RAM extensions */
    option = drive_ram[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        drive_ram_widget_update(option, model);
    }

    /* DOS extensions */
    option = drive_dos[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        drive_dos_widget_sync_combo(option);
    }
    /* RTC save widget */
    option = drive_rtc_save[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        gtk_widget_set_sensitive(option, drive_check_rtc(model));
    }
}


/** \brief  Format the drive emulation volume value
 *
 * Turns the volume scale into 0-100
 *
 * \param[in]   scale   drive volume widget
 * \param[in]   value   value of widget to format
 *
 * \return  heap-allocated string representing the current value
 *
 * \note    the string returned here appears to be deallocated by Gtk3. if
 *          I assume the code example of Gtk3 is correct (compyx)
 */
static gchar *on_drive_volume_format(GtkScale *scale, gdouble value)
{
    return g_strdup_printf("%d", (int)(value / 40));
}


/** \brief  Handler for the 'toggled' event of the IEC checkbox
 *
 * Triggers the user-provided callback function on toggle.
 *
 * \param[in]   widget  IEC checkbox
 * \param[in]   data    unit number
 */
static void on_iec_toggled(GtkWidget *widget, gpointer data)
{
    if (machine_class != VICE_MACHINE_PET &&
            machine_class != VICE_MACHINE_CBM6x0 &&
            machine_class != VICE_MACHINE_CBM5x0) {

        void (*callback)(GtkWidget *, int);
        int unit = GPOINTER_TO_INT(data);

        callback = g_object_get_data(G_OBJECT(widget), "UnitCallback");
        if (callback != NULL) {
            callback(widget, unit);
        }
    }
}


/** \brief  Handler for the 'toggled' event of the Virtual Devices checkbox
 *
 * Triggers the user-provided callback function on toggle.
 *
 * \param[in]   widget  Virtual Devices checkbox
 * \param[in]   data    unit number
 */
static void on_virtual_device_toggled(GtkWidget *widget, gpointer data)
{
    void (*callback)(GtkWidget *, int);
    int unit = GPOINTER_TO_INT(data);

    callback = g_object_get_data(G_OBJECT(widget), "UnitCallback");
    if (callback != NULL) {
        callback(widget, unit);
    }
}



/*
 * Private functions
 */

/** \brief  Create left aligned GtkLabel with Pango markup
 *
 * \param[in]   text    label text with Pango markup allowed
 *
 * \return  GtkLabel
 */
static GtkWidget *create_left_aligned_label(const char *text)
{
    GtkWidget *label = gtk_label_new(NULL);

    gtk_label_set_markup(GTK_LABEL(label), text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    return label;
}


/** \brief  Create slider to control drive sound emulation volume
 *
 * \return  GtkScale
 */
static GtkWidget *create_drive_volume_widget(void)
{
    GtkWidget *scale;

    scale = vice_gtk3_resource_scale_int_new("DriveSoundEmulationVolume",
                                             GTK_ORIENTATION_HORIZONTAL,
                                             0, 4000, 100);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    g_signal_connect_unlocked(scale,
                              "format-value",
                              G_CALLBACK(on_drive_volume_format),
                              NULL);
    /* Set a minimum size so we always have enough space for the widget to
     * actually show a scale, not just the handle.
     * This was an issue when we still had the global 'Enable TDE' checkbox
     * on top of the page, that's no longer the case. Still, better safe than
     * sorry. */
    gtk_widget_set_size_request(scale, 100, -1);
    return scale;
}


/** \brief  Create checkbox to toggle IEC-Device emulation for \a unit
 *
 * \param[in]   unit        unit number (8-11)
 * \param[in]   callback    function to call on checkbutton toggle events
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_iec_check_button(int unit,
                                          void (*callback)(GtkWidget *, int))
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf("IECDevice%d",
                                                        "IEC device",
                                                        unit);
    g_object_set_data(G_OBJECT(check), "UnitCallback", (gpointer)callback);
    g_signal_connect(GTK_TOGGLE_BUTTON(check),
                     "toggled",
                     G_CALLBACK(on_iec_toggled),
                     GINT_TO_POINTER(unit));
    return check;
}


/** \brief  Create widget to control read-only mode for \a unit
 *
 * \param[in]   unit    unit number (8-11)
 * \param[in]   drive   drive number (0-1)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_readonly_check_button(int unit, int drive)
{
    GtkWidget *check;
    int drive_type;
    char label[] = "Drive 0 Read Only";

    label[6] = '0' + drive;
    check =  vice_gtk3_resource_check_button_new_sprintf("AttachDevice%dd%dReadonly",
                                                        label,
                                                        unit, drive);
    drive_type = ui_get_drive_type(unit);
    gtk_widget_set_sensitive(check, drive ? drive_check_dual(drive_type) : 1);
    return check;
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
                                                        "Save real-time clock data",
                                                        unit);
    drive_type = ui_get_drive_type(unit);
    gtk_widget_set_sensitive(check, drive_check_rtc(drive_type));
    return check;
}


/** \brief  Create widget to control IEC device type
 *
 * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_drive_device_type_widget(int unit)
{
    GtkWidget *combo;

    combo = vice_gtk3_resource_combo_box_int_new_sprintf("FileSystemDevice%d",
                                                         device_types,
                                                         unit);
    gtk_widget_set_hexpand(combo, TRUE);
    gtk_widget_show_all(combo);
    return combo;
}


/** \brief  Create per-unit True Drive Emulation check button
 *
 * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_drive_true_emulation_widget(int unit)
{
    return vice_gtk3_resource_check_button_new_sprintf("Drive%dTrueEmulation",
                                                       "True drive emulation",
                                                       unit);
}



/** \brief  Create per-unit Virtual Devices check button
 *
 * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_drive_virtual_device_widget(int unit, void (*callback)(GtkWidget *, int))
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new_sprintf("VirtualDevice%d",
                                                        "Virtual device",
                                                        unit);
    g_object_set_data(G_OBJECT(check), "UnitCallback", (gpointer)callback);
    g_signal_connect(GTK_TOGGLE_BUTTON(check),
                     "toggled",
                     G_CALLBACK(on_virtual_device_toggled),
                     GINT_TO_POINTER(unit));
    return check;
}


/** \brief  Create layout for xvic
 *
 * \param[in]   grid    main widget grid
 * \param[in]   unit    unit number
 *
 * \return  \a grid
 */
static GtkWidget *create_vic20_layout(GtkWidget *grid, int unit)
{
    GtkWidget *wrapper;
    GtkWidget *label;
    int row = 0;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */

    /* column 0 wrapper */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);

    label = create_left_aligned_label("<b>Drive model</b>");
    /* only show currently available models */
    drive_model[index] = drive_model_widget_create_combo(unit, FALSE);
    gtk_widget_set_margin_bottom(label, 16);
    gtk_widget_set_margin_bottom(drive_model[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 1, row, 1, 1);
    row++;

    /* read-only check button */
    drive_read_only[index][0] = create_readonly_check_button(unit, 0);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][0], 0, row, 2, 1);
    row++;
    drive_read_only[index][1] = create_readonly_check_button(unit, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][1], 0, row, 2, 1);
    row++;

    /* RTC save check button */
    drive_rtc_save[index] = create_rtc_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rtc_save[index], 0, row, 2, 1);
    row++;

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, row, 2, 1);
    row++;

    /* Virtual Device */
    drive_virtualdev[index] = create_drive_virtual_device_widget(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_virtualdev[index], 0, row, 2, 1);
    row++;

    /* IEC device check button */
    /* FIXME: xvic does not use the generic IEC bus code in src/iecbus/iecbus.c yet */
#if 0
    drive_iec_device[index] = create_iec_check_button(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_iec_device[index], 0, row, 2, 1);
    row++;
#endif

    /* IEC device type combo box */
    drive_device_type_label[index] = create_left_aligned_label("IEC device type");
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type_label[index], 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type[index], 1, row, 1, 1);
    row++;

    /* Drive RAM */
    drive_ram[index] = drive_ram_widget_create(unit);
    gtk_widget_set_margin_top(drive_ram[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_ram[index], 0, row, 2, 1);

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 1);


    /* column 1 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);
    row = 0;    /* row in wrapper */

    /* wrapper row 0: image extend policy for track 36+ */
    label = create_left_aligned_label("40-track policy");
    drive_extend[index] = drive_extend_policy_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_extend[index], 1, row, 1, 1);
    row++;

    /* wrapper row 1: idle method */
    label = create_left_aligned_label("Idle method");
    drive_idle[index] = drive_idle_method_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_idle[index], 1, row, 1, 1);
    row++;

    /* wrapper row 2, RPM settings */
    drive_rpm[index] = drive_rpm_widget_create(unit);
    gtk_widget_set_margin_top(drive_rpm[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rpm[index], 0, row, 2, 1);

    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 1, 0, 1, 1);

    drive_model_widget_add_callback(drive_model[index],
                                    on_model_changed,
                                    GINT_TO_POINTER(unit));
    return grid;
}


/** \brief  Create layout for x64, x64sc, xscpu64 and x128
 *
 * \param[in]   grid    main widget grid
 * \param[in]   unit    unit number
 *
 * \return  \a grid
 */
static GtkWidget *create_c64_layout(GtkWidget *grid, int unit)
{
    GtkWidget *wrapper;
    GtkWidget *label;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */
    int row = 0;

    /* column 0 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);

    label = create_left_aligned_label("<b>Drive model</b>");
    /* only show currently available models */
    drive_model[index] = drive_model_widget_create_combo(unit, FALSE);
    gtk_widget_set_margin_bottom(label, 16);
    gtk_widget_set_margin_bottom(drive_model[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 1, row, 1, 1);
    row++;

    /* read-only check button */
    drive_read_only[index][0] = create_readonly_check_button(unit, 0);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][0], 0, row, 2, 1);
    row++;
    drive_read_only[index][1] = create_readonly_check_button(unit, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][1], 0, row, 2, 1);
    row++;

    /* RTC save check button */
    drive_rtc_save[index] = create_rtc_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rtc_save[index], 0, row, 2, 1);
    row++;

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, row, 2, 1);
    row++;

    /* Virtual Device */
    drive_virtualdev[index] = create_drive_virtual_device_widget(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_virtualdev[index], 0, row, 2, 1);
    row++;

    /* IEC device check button */
    drive_iec_device[index] = create_iec_check_button(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_iec_device[index], 0, row, 2, 1);
    row++;

    /* IEC device type combo box */
    drive_device_type_label[index] = create_left_aligned_label("IEC device type");
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type_label[index], 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type[index], 1, row, 1, 1);
    row++;

    /* Fixed HD size */
    label = create_left_aligned_label("CMD-HD size");
    drive_size[index] = drive_fixed_size_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_size[index], 1, row, 1, 1);
    row++;

    /* Drive RAM */
    drive_ram[index] = drive_ram_widget_create(unit);
    gtk_widget_set_margin_top(drive_ram[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_ram[index], 0, row, 2, 1);

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 1);


    /* column 1 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);
    row = 0;    /* row in wrapper */

    /* wrapper row 0: DOS expansion */
    label = create_left_aligned_label("DOS expansion");
    drive_dos[index] = drive_dos_widget_create_combo(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_dos[index], 1, row, 1, 1);
    row++;

    /* wrapper row 1: image extend policy for track 36+ */
    label = create_left_aligned_label("40-track policy");
    drive_extend[index] = drive_extend_policy_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_extend[index], 1, row, 1, 1);
    row++;

    /* wrapper row 2: idle method */
    label = create_left_aligned_label("Idle method");
    drive_idle[index] = drive_idle_method_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_idle[index], 1, row, 1, 1);
    row++;

    /* wrapper row 3: parallel cable */
    label = create_left_aligned_label("Parallel cable");
    drive_parallel[index] = drive_parallel_cable_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_parallel[index], 1, row, 1, 1);
    row++;

    /* wrapper row 4: RPM settings */
    drive_rpm[index] = drive_rpm_widget_create(unit);
    gtk_widget_set_margin_top(drive_rpm[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rpm[index], 0, row, 2, 1);

    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 1, 0, 1, 1);

    drive_model_widget_add_callback(drive_model[index],
                                    on_model_changed,
                                    GINT_TO_POINTER(unit));
    return grid;
}


/** \brief  Create layout for xplus4 and c64dtv
 *
 * \param[in]   grid    main widget grid
 * \param[in]   unit    unit number
 *
 * \return  \a grid
 */
static GtkWidget *create_plus4_layout(GtkWidget *grid, int unit)
{
    GtkWidget *wrapper;
    GtkWidget *label;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */
    int row = 0;

    /* column 0 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);

    label = create_left_aligned_label("<b>Drive model</b>");
    /* only show currently available models */
    drive_model[index] = drive_model_widget_create_combo(unit, FALSE);
    gtk_widget_set_margin_bottom(label, 16);
    gtk_widget_set_margin_bottom(drive_model[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 1, row, 1, 1);
    row++;

    /* read-only check button */
    drive_read_only[index][0] = create_readonly_check_button(unit, 0);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][0], 0, row, 2, 1);
    row++;
    drive_read_only[index][1] = create_readonly_check_button(unit, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][1], 0, row, 2, 1);
    row++;

    /* RTC save check button */
    drive_rtc_save[index] = create_rtc_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rtc_save[index], 0, row, 2, 1);
    row++;

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, row, 2, 1);
    row++;

    /* Virtual Device */
    drive_virtualdev[index] = create_drive_virtual_device_widget(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_virtualdev[index], 0, row, 2, 1);
    row++;

    /* IEC device check button */
    drive_iec_device[index] = create_iec_check_button(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_iec_device[index], 0, row, 2, 1);
    row++;

    /* IEC device type combo box */
    drive_device_type_label[index] = create_left_aligned_label("IEC device type");
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type_label[index], 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type[index], 1, row, 1, 1);
    row++;

    /* Drive RAM */
    drive_ram[index] = drive_ram_widget_create(unit);
    gtk_widget_set_margin_top(drive_ram[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_ram[index], 0, row, 2, 1);

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 1);


    /* column 1 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);
    row = 0;    /* row in wrapper */

    /* wrapper row 0: image extend policy for track 36+ */
    label = create_left_aligned_label("40-track policy");
    drive_extend[index] = drive_extend_policy_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_extend[index], 1, row, 1, 1);
    row++;

    /* wrapper row 1: idle method */
    label = create_left_aligned_label("Idle method");
    drive_idle[index] = drive_idle_method_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_idle[index], 1, row, 1, 1);
    row++;

    /* wrapper row 2: parallel cable */
    /* XXX: vice.texi mentions parallel support for Plus4, and the resource
     *      is there, but the implementation is missing, the files
     *      src/drive/iec/plus4exp/ contain stubs.
     */
#if 0
    if (machine_class != VICE_MACHINE_C64DTV) {

        label = create_left_aligned_label("Parallel cable");
        drive_parallel[index] = drive_parallel_cable_widget_create(unit);
        gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(wrapper), drive_parallel[index], 1, row, 1, 1);
        row++;
    }
#endif

    /* wrapper row 2/3: RPM settings */
    drive_rpm[index] = drive_rpm_widget_create(unit);
    gtk_widget_set_margin_top(drive_rpm[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rpm[index], 0, row, 2, 1);

    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 1, 0, 1, 1);

    drive_model_widget_add_callback(drive_model[index],
                                    on_model_changed,
                                    GINT_TO_POINTER(unit));
    return grid;
}


/** \brief  Create layout for xcbm2, xcbm5x0, pet
 *
 * \param[in]   grid    main widget grid
 * \param[in]   unit    unit number
 *
 * \return  \a grid
 */
static GtkWidget *create_pet_layout(GtkWidget *grid, int unit)
{
    GtkWidget *wrapper;
    GtkWidget *label;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */
    int row = 0;

    /* column 0 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);

    label = create_left_aligned_label("<b>Drive model</b>");
    /* only show currently available models */
    drive_model[index] = drive_model_widget_create_combo(unit, FALSE);
    gtk_widget_set_margin_bottom(label, 16);
    gtk_widget_set_margin_bottom(drive_model[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 1, row, 1, 1);
    row++;

    /* read-only check button */
    drive_read_only[index][0] = create_readonly_check_button(unit, 0);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][0], 0, row, 2, 1);
    row++;
    drive_read_only[index][1] = create_readonly_check_button(unit, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index][1], 0, row, 2, 1);
    row++;

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, row, 2, 1);
    row++;

    /* Virtual Device */
    drive_virtualdev[index] = create_drive_virtual_device_widget(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(wrapper), drive_virtualdev[index], 0, row, 2, 1);
    row++;

    /* IEEE/Virtual device type combo box */
    drive_device_type_label[index] = create_left_aligned_label("IEEE device type");
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type_label[index], 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_device_type[index], 1, row, 1, 1);
    row++;

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 1);


    /* column 1 */
    wrapper = vice_gtk3_grid_new_spaced(8, 0);
    row = 0;

    /* wrapper row 0: image extend policy for track 36+ */
    label = create_left_aligned_label("40-track policy");
    drive_extend[index] = drive_extend_policy_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_extend[index], 1, row, 1, 1);
    row++;

    /* wrapper row 1: idle method */
    label = create_left_aligned_label("Idle method");
    drive_idle[index] = drive_idle_method_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), drive_idle[index], 1, row, 1, 1);
    row++;

    /* wrapper row 2, RPM settings */
    drive_rpm[index] = drive_rpm_widget_create(unit);
    gtk_widget_set_margin_top(drive_rpm[index], 16);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rpm[index], 0, row, 2, 1);

    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 1, 0, 1, 1);

    drive_model_widget_add_callback(drive_model[index],
                                    on_model_changed,
                                    GINT_TO_POINTER(unit));
    return grid;
}


/** \brief  Create a composite widget with settings for drive \a unit
 *
 * \param[in]   unit    unit number
 *
 * \return  GtkGrid
 */
static GtkWidget *create_stack_child_widget(int unit)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 32);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_widget_set_margin_start(grid, 8);
    g_object_set_data(G_OBJECT(grid), "UnitNumber", GINT_TO_POINTER(unit));

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:
            create_c64_layout(grid, unit);
            break;
        case VICE_MACHINE_VIC20:
            create_vic20_layout(grid, unit);
            break;
        case VICE_MACHINE_PLUS4:    /* fall through */
        case VICE_MACHINE_C64DTV:
            create_plus4_layout(grid, unit);
            break;
        case VICE_MACHINE_PET:      /* fall through */
        case VICE_MACHINE_CBM5x0:   /* fall through */
        case VICE_MACHINE_CBM6x0:
            create_pet_layout(grid, unit);
            break;
        default:
            break;
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create main drive settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_drive_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *wrapper;
    GtkWidget *sound;
    GtkWidget *stack;
    GtkWidget *switcher;
    GtkWidget *volume;
    GtkWidget *label;
    GtkWidget *volume_wrapper;
    int unit;

    /* three column wide grid */
    layout = vice_gtk3_grid_new_spaced(16, 16);

    wrapper = gtk_grid_new();
    gtk_widget_set_margin_start(wrapper, 16);
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 16);

    sound = vice_gtk3_resource_check_button_new("DriveSoundEmulation",
                                                "Drive sound emulation");
    gtk_grid_attach(GTK_GRID(wrapper), sound, 0, 0, 1, 1);

    volume_wrapper = gtk_grid_new();
    label = gtk_label_new("Drive volume:");
    volume = create_drive_volume_widget();
    gtk_widget_set_margin_start(volume, 16);
    gtk_grid_attach(GTK_GRID(volume_wrapper), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(volume_wrapper), volume, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), volume_wrapper, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), wrapper, 0, 0, 1, 1);

    stack = gtk_stack_new();
    for (unit = DRIVE_UNIT_MIN; unit <= DRIVE_UNIT_MAX; unit++) {
        char title[256];

        g_snprintf(title, sizeof(title), "Drive %d", unit);
        gtk_stack_add_titled(GTK_STACK(stack),
                             create_stack_child_widget(unit),
                             title,
                             title);
    }
    gtk_stack_set_transition_type(GTK_STACK(stack),
                                  GTK_STACK_TRANSITION_TYPE_NONE);

    switcher = gtk_stack_switcher_new();
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(switcher, TRUE);
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), GTK_STACK(stack));

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_grid_attach(GTK_GRID(layout), switcher, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(layout), stack, 0, 2, 3, 1);

    /* set sensitivity of the filesystem-type comboboxes, depending on the
     * IECDevice (not for VIC20 and PET/CBM-II) and VirtualDevice resource
     */
    for (unit = DRIVE_UNIT_MIN; unit <= DRIVE_UNIT_MAX; unit++) {
        int iecdev = 0;
        int virtdev = 0;
        int index = unit - DRIVE_UNIT_MIN;  /* index in the widget arrays */

        /* FIXME: xvic doesn't use IEDevice (yet), uses its own iecbus code */
        if (machine_class != VICE_MACHINE_VIC20 &&
            machine_class != VICE_MACHINE_PET &&
            machine_class != VICE_MACHINE_CBM5x0 &&
            machine_class != VICE_MACHINE_CBM6x0) {
            if (resources_get_int_sprintf("IECDevice%d", &iecdev, unit) < 0) {
                iecdev = 0;
            }
        }
        if (resources_get_int_sprintf("VirtualDevice%d", &virtdev, unit) < 0) {
            virtdev = 0;
        }
        /* try to set sensitive, regardless of if the widget actually
            * exists, this helps with debugging since Gtk will print a warning
            * on the console.
            */
        gtk_widget_set_sensitive(drive_device_type_label[index], iecdev | virtdev);
        gtk_widget_set_sensitive(drive_device_type[index], iecdev | virtdev);
    }
    gtk_widget_show_all(layout);
    return layout;
}
