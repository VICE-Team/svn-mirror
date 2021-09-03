/** \file   settings_drive.c
 * \brief   Drive settings dialog
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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
 * $VICERES AttachDevice8Readonly       -vsid
 * $VICERES AttachDevice9Readonly       -vsid
 * $VICERES AttachDevice10Readonly      -vsid
 * $VICERES AttachDevice11Readonly      -vsid
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

#include "vice.h"

#include "vice_gtk3.h"
#include "debug_gtk3.h"
#include "attach.h"
#include "drive.h"
#include "drive-check.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "drivewidgethelpers.h"
#include "driveunitwidget.h"
#include "drivemodelwidget.h"
#include "driveextendpolicywidget.h"
#include "driveidlemethodwidget.h"
#include "driveparallelcablewidget.h"
#include "driverpmwidget.h"
#include "driveramwidget.h"
#include "drivedoswidget.h"
//#include "driveoptionswidget.h"
#include "drivefixedsizewidget.h"

#include "settings_drive.h"


/** \brief  List of file system types
 */
static const vice_gtk3_combo_entry_int_t device_types[] = {
    { "Disk images", ATTACH_DEVICE_NONE },
    { "Host file system", ATTACH_DEVICE_FS },
#ifdef HAVE_REALDEVICE
    { "Real device (OpenCBM)", ATTACH_DEVICE_REAL },
#endif
    { NULL, -1 }
};


/* FIXME: this needs proper documentation */

/** \brief  Drive model widgets */
static GtkWidget *drive_model[NUM_DISK_UNITS];

#if 0
/** \brief  Drive options widgets */
static GtkWidget *drive_options[NUM_DISK_UNITS];
#endif

static GtkWidget *drive_tde[NUM_DISK_UNITS];
static GtkWidget *drive_read_only[NUM_DISK_UNITS];
static GtkWidget *drive_rtc_save[NUM_DISK_UNITS];
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

/** \brief  Drive fixed-size widgets */
static GtkWidget *drive_size[NUM_DISK_UNITS];



/** \brief  Custom callback for the IEC widget in driveoptions.c
 *
 * \param[in]   widget  IEC toggle button
 * \param[in]   unit    unit number (8-11)
 */
static void iec_callback(GtkWidget *widget, int unit)
{
    if (unit >= DRIVE_UNIT_MIN && unit <= DRIVE_UNIT_MAX) {
        gtk_widget_set_sensitive(drive_device_type[unit - DRIVE_UNIT_MIN],
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
    }
}


/** \brief  Extra event handler for the drive model changes
 *
 * \param[in]   widget      drive type radio button
 * \param[in]   user_data   unit number
 */
static void on_model_changed(GtkWidget *widget, gpointer user_data)
{
    int unit = GPOINTER_TO_INT(user_data);
    int model = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "ModelID"));
    GtkWidget *option;

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
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 1),
                drive_check_expansion2000(model));
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 2),
                drive_check_expansion4000(model));
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 3),
                drive_check_expansion6000(model));
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 4),
                drive_check_expansion8000(model));
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 5),
                drive_check_expansionA000(model));
    }

    /* DOS extensions */
    option = drive_dos[unit - DRIVE_UNIT_MIN];
    if (option != NULL) {
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 1),
                drive_check_profdos(model));
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 2),
                drive_check_stardos(model));
        gtk_widget_set_sensitive(gtk_grid_get_child_at(GTK_GRID(option), 0, 3),
                drive_check_supercard(model));
    }

    /* drive read-only widget */
    option = drive_read_only[unit - DRIVE_UNIT_MIN];
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


/** \brief  Create slider to control drive sound emulation volume
 *
 * \return  GtkScale
 */
static GtkWidget *create_drive_volume_widget(void)
{
    GtkWidget *scale;

    scale = vice_gtk3_resource_scale_int_new("DriveSoundEmulationVolume",
            GTK_ORIENTATION_HORIZONTAL, 0, 4000, 100);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(scale), GTK_POS_RIGHT);
    g_signal_connect_unlocked(scale, "format-value", G_CALLBACK(on_drive_volume_format),
            NULL);
    /* FIXME: quick fix: this kind of thing should be handled with VICE-wide
     *        CSS or similar things.
     */
    gtk_widget_set_size_request(scale, 100, -1);
    return scale;
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

    check = vice_gtk3_resource_check_button_new_sprintf(
            "IECDevice%d", "IEC-Device", unit);

    g_object_set_data(G_OBJECT(check), "UnitCallback", (gpointer)callback);
    g_signal_connect(GTK_TOGGLE_BUTTON(check), "toggled", G_CALLBACK(on_iec_toggled),
            GINT_TO_POINTER(unit));

    return check;
}


/** \brief  Create widget to control read-only mode for \a unit
 *
  * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_readonly_check_button(int unit)
{
    return vice_gtk3_resource_check_button_new_sprintf("AttachDevice%dReadonly",
            "Read Only", unit);
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
            "RTC Save", unit);

    drive_type = ui_get_drive_type(unit);
    gtk_widget_set_sensitive(check,
            drive_type == DRIVE_TYPE_2000 || drive_type == DRIVE_TYPE_4000);
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
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *combo;

    grid = vice_gtk3_grid_new_spaced(16, 0);

    label = gtk_label_new("IEC-Device type");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    combo = vice_gtk3_resource_combo_box_int_new_sprintf(
            "FileSystemDevice%d", device_types, unit);
    gtk_widget_set_hexpand(combo, TRUE);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create per-unit True Drive Emulation check button
 *
 * \param[in]   unit    unit number (8-11)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_drive_true_emulation_widget(int unit)
{
    return vice_gtk3_resource_check_button_new_sprintf(
            "Drive%dTrueEmulation",
            "Enable True Drive Emulation",
            unit);
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
    GtkWidget *iec_wrapper;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */

    /* row 0 & 1, column 0 */
    wrapper = vice_gtk3_grid_new_spaced(32, 8);

    drive_model[unit - DRIVE_UNIT_MIN] = drive_model_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_model[unit - DRIVE_UNIT_MIN], 0, 0, 1, 1);

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, 1, 1, 1);
    g_object_set(drive_tde[index], "margin-top", 16, NULL);

    /* read-only check button */
    drive_read_only[index] = create_readonly_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index], 0, 2, 1, 1);

    /* RTC save check button */
    drive_rtc_save[index] = create_rtc_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rtc_save[index], 0, 3, 1, 1);

    /* wrapper for IEC check button and IEC device type combo box */
    iec_wrapper = gtk_grid_new();
    /* IEC device check button */
#if 0
    /* FIXME: xvic does not use the generic IEC bus code in src/iecbus/iecbus.c yet */
    drive_iec_device[index] = create_iec_check_button(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(iec_wrapper), drive_iec_device[index], 0, 0, 1, 1);
#endif
    /* IEC device type combo box */
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(iec_wrapper), drive_device_type[index], 0, 1, 1, 1);
    g_object_set(drive_device_type[index], "margin-left", 16, NULL);
    /* add iec-wrapper to parent wrapper */
    gtk_grid_attach(GTK_GRID(wrapper), iec_wrapper, 0, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 2);

    /* row 0, column 1 */
    drive_ram[unit - DRIVE_UNIT_MIN] = drive_ram_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_ram[unit - DRIVE_UNIT_MIN], 1, 0, 1, 1);

    /* row 1, column 1 */
    /*    drive_dos = drive_dos_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_dos, 1, 1, 1, 1);
    */

    /* row 0 & 1, column 2 */
    wrapper = vice_gtk3_grid_new_spaced(0, 16);
    drive_extend[unit - DRIVE_UNIT_MIN] = drive_extend_policy_widget_create(unit);
    drive_idle[unit - DRIVE_UNIT_MIN] = drive_idle_method_widget_create(unit);
    /*    drive_parallel = drive_parallel_cable_widget_create(unit); */
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_extend[unit - DRIVE_UNIT_MIN], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_idle[unit - DRIVE_UNIT_MIN], 0, 1, 1, 1);
    /*    gtk_grid_attach(GTK_GRID(wrapper), drive_parallel, 0, 2, 1, 1); */
    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 2, 0, 1, 2);

    /* row 2, column 0 */
    drive_rpm[unit - DRIVE_UNIT_MIN] = drive_rpm_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_rpm[unit - DRIVE_UNIT_MIN], 1, 1, 2, 1);

    drive_model_widget_add_callback(drive_model[unit - DRIVE_UNIT_MIN],
            on_model_changed, GINT_TO_POINTER(unit));
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
    GtkWidget *iec_wrapper;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */

    /* row 0 & 1, column 0 */

    wrapper = vice_gtk3_grid_new_spaced(32, 8);

    drive_model[index] = drive_model_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 0, 0, 1, 1);

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, 1, 1, 1);
    g_object_set(drive_tde[index], "margin-top", 16, NULL);

    /* read-only check button */
    drive_read_only[index] = create_readonly_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index], 0, 2, 1, 1);

    /* RTC save check button */
    drive_rtc_save[index] = create_rtc_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rtc_save[index], 0, 3, 1, 1);

    /* wrapper for IEC check button and IEC device type combo box */
    iec_wrapper = gtk_grid_new();
    /* IEC device check button */
    drive_iec_device[index] = create_iec_check_button(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(iec_wrapper), drive_iec_device[index], 0, 0, 1, 1);
    /* IEC device type combo box */
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(iec_wrapper), drive_device_type[index], 0, 1, 1, 1);
    g_object_set(drive_device_type[index], "margin-left", 16, NULL);
    /* add iec-wrapper to parent wrapper */
    gtk_grid_attach(GTK_GRID(wrapper), iec_wrapper, 0, 4, 1, 1);

    drive_size[index] = drive_fixed_size_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_size[index], 0, 5, 1, 1);

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 2);


    /* row 0, column 1 */
    drive_ram[unit - DRIVE_UNIT_MIN] = drive_ram_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_ram[unit - DRIVE_UNIT_MIN], 1, 0, 1, 1);

    /* row 1, column 1 */
    drive_dos[unit - DRIVE_UNIT_MIN] = drive_dos_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_dos[unit - DRIVE_UNIT_MIN], 1, 1, 1, 1);

    /* row 0 & 1, column 2 */
    wrapper = vice_gtk3_grid_new_spaced(16, 16);
    drive_extend[unit - DRIVE_UNIT_MIN] = drive_extend_policy_widget_create(unit);
    drive_idle[unit - DRIVE_UNIT_MIN] = drive_idle_method_widget_create(unit);
    drive_parallel[unit - DRIVE_UNIT_MIN] = drive_parallel_cable_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_extend[unit - DRIVE_UNIT_MIN], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_idle[unit - DRIVE_UNIT_MIN], 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_parallel[unit - DRIVE_UNIT_MIN], 0, 2, 1, 1);
    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 2, 0, 1, 2);

    /* row 2, column 1 & 2 */
    drive_rpm[unit - DRIVE_UNIT_MIN] = drive_rpm_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_rpm[unit - DRIVE_UNIT_MIN], 1, 2, 2, 1);

    drive_model_widget_add_callback(
            drive_model[unit - DRIVE_UNIT_MIN],
            on_model_changed,
            GINT_TO_POINTER(unit));
    return grid;
}


/** \brief  Create layout for xplus4, vic20, dtv
 *
 * \param[in]   grid    main widget grid
 * \param[in]   unit    unit number
 *
 * \return  \a grid
 */
static GtkWidget *create_plus4_layout(GtkWidget *grid, int unit)
{
    GtkWidget *wrapper;
    GtkWidget *iec_wrapper;
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */

    /* row 0 & 1, column 0 */
    wrapper = vice_gtk3_grid_new_spaced(32, 8);

    drive_model[index] = drive_model_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 0, 0, 1, 1);

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, 1, 1, 1);
    g_object_set(drive_tde[index], "margin-top", 16, NULL);

    /* read-only check button */
    drive_read_only[index] = create_readonly_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index], 0, 2, 1, 1);

    /* RTC save check button */
    drive_rtc_save[index] = create_rtc_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_rtc_save[index], 0, 3, 1, 1);

    /* wrapper for IEC check button and IEC device type combo box */
    iec_wrapper = gtk_grid_new();
    /* IEC device check button */
    drive_iec_device[index] = create_iec_check_button(unit, iec_callback);
    gtk_grid_attach(GTK_GRID(iec_wrapper), drive_iec_device[index], 0, 0, 1, 1);
    /* IEC device type combo box */
    drive_device_type[index] = create_drive_device_type_widget(unit);
    gtk_grid_attach(GTK_GRID(iec_wrapper), drive_device_type[index], 0, 1, 1, 1);
    g_object_set(drive_device_type[index], "margin-left", 16, NULL);
    /* add iec-wrapper to parent wrapper */
    gtk_grid_attach(GTK_GRID(wrapper), iec_wrapper, 0, 4, 1, 1);

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 2);    /* row 0 & 1, column 0 */


    /* row 0 & 1, column 2 */
    wrapper = vice_gtk3_grid_new_spaced(0, 16);
    drive_extend[unit - DRIVE_UNIT_MIN] = drive_extend_policy_widget_create(unit);
    drive_idle[unit - DRIVE_UNIT_MIN]  = drive_idle_method_widget_create(unit);
    /* XXX: vice.texi mentions parallel support for Plus4, and the resource
     *      is there, but the implementation is missing, the files
     *      src/drive/iec/plus4exp/ contain stubs.
     */
    drive_parallel[unit - DRIVE_UNIT_MIN] = drive_parallel_cable_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_extend[unit - DRIVE_UNIT_MIN], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_idle[unit - DRIVE_UNIT_MIN], 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_parallel[unit - DRIVE_UNIT_MIN], 0, 2, 1, 1);

    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 2, 0, 1, 2);

    /* row 1, column 1 & 2 */
    drive_rpm[unit - DRIVE_UNIT_MIN] = drive_rpm_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_rpm[unit - DRIVE_UNIT_MIN], 1, 2, 2, 1);

    drive_model_widget_add_callback(
            drive_model[unit - DRIVE_UNIT_MIN],
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
    int index = unit - DRIVE_UNIT_MIN;  /* index in widget arrays */

    /* row 0 & 1, column 0 */
    wrapper = vice_gtk3_grid_new_spaced(32, 8);

    drive_model[index] = drive_model_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_model[index], 0, 0, 1, 1);

    /* true drive emulation check button */
    drive_tde[index] = create_drive_true_emulation_widget(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_tde[index], 0, 1, 1, 1);
    g_object_set(drive_tde[index], "margin-top", 16, NULL);

    /* read-only check button */
    drive_read_only[index] = create_readonly_check_button(unit);
    gtk_grid_attach(GTK_GRID(wrapper), drive_read_only[index], 0, 2, 1, 1);

    /* add wrapper to main grid */
    gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 0, 1, 2);    /* row 0 & 1, column 0 */


    /* row 0 & 1, column 2 */
    wrapper = vice_gtk3_grid_new_spaced(0, 16);
    drive_extend[unit - DRIVE_UNIT_MIN] = drive_extend_policy_widget_create(unit);
    drive_idle[unit - DRIVE_UNIT_MIN]  = drive_idle_method_widget_create(unit);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_extend[unit - DRIVE_UNIT_MIN], 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper),
            drive_idle[unit - DRIVE_UNIT_MIN], 0, 1, 1, 1);

    gtk_widget_show_all(wrapper);
    gtk_grid_attach(GTK_GRID(grid), wrapper, 1, 0, 1, 2);

    /* row 2, column 1 */
    drive_rpm[unit - DRIVE_UNIT_MIN] = drive_rpm_widget_create(unit);
    gtk_grid_attach(GTK_GRID(grid), drive_rpm[unit - DRIVE_UNIT_MIN], 1, 2, 1, 1);

    drive_model_widget_add_callback(
            drive_model[unit - DRIVE_UNIT_MIN],
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
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    g_object_set(grid, "margin-left", 8, NULL);
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
#if 0
    GtkWidget *tde;
#endif
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
    g_object_set(wrapper, "margin-left", 16, NULL);
    gtk_grid_set_column_spacing(GTK_GRID(wrapper), 16);
#if 0
    tde = vice_gtk3_resource_check_button_new("DriveTrueEmulation",
            "True drive emulation");
    gtk_grid_attach(GTK_GRID(wrapper), tde, 0, 0, 1, 1);
#endif
    sound = vice_gtk3_resource_check_button_new("DriveSoundEmulation",
            "Drive sound emulation");
    gtk_grid_attach(GTK_GRID(wrapper), sound, 0, 0, 1, 1);

    volume_wrapper = gtk_grid_new();
    label = gtk_label_new("Drive volume:");
    volume = create_drive_volume_widget();
    g_object_set(volume, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(volume_wrapper), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(volume_wrapper), volume, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(wrapper), volume_wrapper, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(layout), wrapper, 0, 0, 1, 1);

    stack = gtk_stack_new();
    for (unit = DRIVE_UNIT_MIN; unit <= DRIVE_UNIT_MAX; unit++) {
        char title[256];

        g_snprintf(title, 256, "Drive %d", unit);
        gtk_stack_add_titled(GTK_STACK(stack),
                create_stack_child_widget(unit),
                title, title);
    }
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT_RIGHT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 1000);

    switcher = gtk_stack_switcher_new();
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(switcher, TRUE);
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher),
            GTK_STACK(stack));

    gtk_widget_show_all(stack);
    gtk_widget_show_all(switcher);

    gtk_grid_attach(GTK_GRID(layout), switcher, 0, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(layout), stack, 0, 2, 3, 1);

    /* set sensitivity of the filesystem-type comboboxes, depending on the
     * IECDevice resource (not for VIC20 and PET/CBM-II)
     */
    if (machine_class != VICE_MACHINE_VIC20 &&
        machine_class != VICE_MACHINE_PET &&
        machine_class != VICE_MACHINE_CBM5x0 &&
        machine_class != VICE_MACHINE_CBM6x0) {

        for (unit = DRIVE_UNIT_MIN; unit <= DRIVE_UNIT_MAX; unit++) {
            int state = 0;

            if (resources_get_int_sprintf("IECDevice%d", &state, unit) < 0) {
                state = 0;
            }
            /* try to set sensitive, regardless of if the widget actually
             * exists, this helps with debugging since Gtk will print a warning
             * on the console.
             */
            gtk_widget_set_sensitive(drive_device_type[unit - DRIVE_UNIT_MIN], state);
        }
    }
    gtk_widget_show_all(layout);
    return layout;
}
