/** \file   settings_rs232.c
 * \brief   Widget to control various RS232 resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES Acia1Enable     x64 x64sc xscpu64 x128 xvic
 * $VICERES Acia1Dev        x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES Acia1Base       x64 x64sc xscpu64 x128 xvic
 * $VICERES Acia1Irq        x64 x64sc xscpu64 x128 xvic
 * $VICERES Acia1Mode       x64 x64sc xscpu64 x128 xvic
 * $VICERES UserportDevice  x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserBaud      x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserDev       x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserUP9600     x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserRTSInv    x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserCTSInv    x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserDSRInv    x64 x64sc xscpu64 x128 xvic
 * $VICERES RsUserDCDInv    x64 x64sc xscpu64 x128 xvic
 * $VICERES RsDevice1       x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice2       x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice3       x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice4       x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice1ip232  x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice2ip232  x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice3ip232  x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice4ip232  x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice1Baud   x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
 * $VICERES RsDevice2Baud   x64 x64sc xscpu64 x128 xvic xplus4 xcbm5x0 xcbm2
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
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "userport.h"
#include "userportrsinterfacewidget.h"
#include "settings_rs232.h"

#include "rsuser.h"


/** \brief  List of ACIA devices
 *
 * \note    Also used by the Userport RS232 Device list combo box
 */
static const vice_gtk3_combo_entry_int_t acia_devices[] = {
    { "Serial 1", 0 },
    { "Serial 2", 1 },
    { "Serial 3", 2 },
    { "Serial 4", 3 },
    { NULL, -1 }
};


/** \brief  List of base addresses for the emulated chip (x64/x64sc/xscpu64)
 */
static const vice_gtk3_radiogroup_entry_t acia_base_c64[] = {
    { "$DE00", 0xde00 },
    { "$DF00", 0xdf00 },
    { NULL, -1 }
};


/** \brief  List of base addresses for the emulated chip (x128)
 */
static const vice_gtk3_radiogroup_entry_t acia_base_c128[] = {
    { "$D700", 0xd700 },
    { "$DE00", 0xde00 },
    { "$DF00", 0xdf00 },
    { NULL, -1 }
};


/** \brief  List of base addresses for the emulated chip (xvic)
 */
static const vice_gtk3_radiogroup_entry_t acia_base_vic20[] = {
    { "$9800", 0x9800 },
    { "$9C00", 0x9c00 },
    { NULL, -1 }
};


/** \brief  List of ACIA IRQ sources (x64/x64sc/xscpu64/x128/xvic)
 */
static const vice_gtk3_radiogroup_entry_t acia_irqs[] = {
    { "None",   0 },
    { "NMI",    1 },
    { "IRQ",    2 },
    { NULL, -1 }
};


/** \brief  List of emulated RS232 modes (x64/x64sc/xscpu64/x128/xvic)
 */
static const vice_gtk3_radiogroup_entry_t acia_modes[] = {
    { "Normal",     0 },
    { "Swiftlink",  1 },
    { "Turbo232",   2 },
    { NULL, -1 }
};


/** \brief  List of user-port RS232 baud rates (x64/x64sc/xscpu64/x128/xvic)
 */
static const vice_gtk3_combo_entry_int_t rsuser_baud_rates[] = {
    { "300",    300 },
    { "600",    600 },
    { "1200",   1200 },
    { "2400",   2400 },
    { "4800",   4800 },
    { "9600",   9600 },
    { "38400",  38400},
    { "57600",  57600},
    { NULL, -1 }
};


/** \brief  List of baud rates for C64/C128/VIC-20 serial devices
 */
static const vice_gtk3_combo_entry_int_t serial_baud_rates_c64[] = {
    { "300",    300 },
    { "1200",   1200 },
    { "2400",   2400 },
    { "9600",   9600 },
    { "19200",  19200 },
    { "38400 (Swiftlink/Turbo232)", 38400 },
    { "57600 (Turbo232)",  57600 },
    { "115200 (Turbo232)", 115200 },
    { NULL, -1 }
};


/** \brief  List of baud rates for Plus4/CBM-II
 */
static const vice_gtk3_combo_entry_int_t serial_baud_rates_plus4[] = {
    { "300",    300 },
    { "1200",   1200 },
    { "2400",   2400 },
    { "9600",   9600 },
    { "19200",  19200 },
    { NULL, -1 }
};

/** \brief  Userport baud rate widget */
static GtkWidget *rsuser_baud_widget;

/** \brief  Extra event handler for the userport RS-232 interface type changes
 *
 * \param[in]   widget      Userport RS-232 interface type combobox
 */
static void on_rsinterface_changed(GtkWidget *widget)
{
    int up9600;

    GtkWidget *baud;
    GtkWidget *rts;
    GtkWidget *cts;
    GtkWidget *dcd;
    GtkWidget *dsr;
    GtkWidget *dtr;
    GtkWidget *grid;
    int index = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));


    resources_get_int("RsUserUP9600", &up9600);
    baud = rsuser_baud_widget;
    grid = gtk_widget_get_parent(widget);
    rts = gtk_grid_get_child_at((GtkGrid*)grid, 1, 2);
    cts = gtk_grid_get_child_at((GtkGrid*)grid, 2, 2);
    dsr = gtk_grid_get_child_at((GtkGrid*)grid, 1, 3);
    dcd = gtk_grid_get_child_at((GtkGrid*)grid, 2, 3);
    dtr = gtk_grid_get_child_at((GtkGrid*)grid, 1, 4);

    if (rts != NULL && cts != NULL && dcd != NULL && dsr != NULL && dtr != NULL) {
        switch (index) {
            case USERPORT_RS_NONINVERTED:
                vice_gtk3_resource_check_button_set(rts, false);
                vice_gtk3_resource_check_button_set(cts, false);
                /*vice_gtk3_resource_check_button_set(dcd, false);*/
                vice_gtk3_resource_check_button_set(dsr, false);
                vice_gtk3_resource_check_button_set(dtr, false);
                resources_set_int("RsUserUP9600", 0);
                break;
            case USERPORT_RS_INVERTED:
                vice_gtk3_resource_check_button_set(rts, true);
                vice_gtk3_resource_check_button_set(cts, true);
                /*vice_gtk3_resource_check_button_set(dcd, true);*/
                vice_gtk3_resource_check_button_set(dsr, true);
                vice_gtk3_resource_check_button_set(dtr, true);
                resources_set_int("RsUserUP9600", 0);
                break;
            case USERPORT_RS_UP9600:
                if (baud != NULL) {
                    vice_gtk3_resource_combo_box_int_set(baud, 9600);
                }
                resources_set_int("RsUserUP9600", 1);
                break;
            default:
                resources_set_int("RsUserUP9600", 0);
                break;
        }
    }
}


/** \brief  Helper: create left-aligned, 16 px indented label
 *
 * \param[in]   text    label text
 *
 * \return  GtkLabel
 */
static GtkWidget *create_indented_label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    return label;
}

/** \brief  Create a check button to enable/disable ACIA emulation
 *
 * \return  GtkCheckButton or `NULL` when the current machine doesn't support
 *          toggling emulation
 */
static GtkWidget *create_acia_enable_widget(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:
            return vice_gtk3_resource_check_button_new("Acia1Enable",
                    "Enable ACIA RS232 interface emulation");
        default:
            return NULL;
    }
}


/** \brief  Create a group of radio buttons to set the ACIA chip address
 *
 * \return  GtkGrid with radio buttons or `NULL` when the current machine
 *          doesn't support selecting a base address
 */
static GtkWidget *create_acia_base_widget(void)
{
    const vice_gtk3_radiogroup_entry_t *entries = NULL;
    GtkWidget *widget;

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:
            entries = acia_base_c64;
            break;
        case VICE_MACHINE_C128:
            entries = acia_base_c128;
            break;
        case VICE_MACHINE_VIC20:
            entries = acia_base_vic20;
            break;
        default:
            entries = NULL;
            break;
    }

    if (entries != NULL) {
        widget = vice_gtk3_resource_radiogroup_new("Acia1Base", entries,
                GTK_ORIENTATION_HORIZONTAL);
        gtk_grid_set_column_spacing(GTK_GRID(widget), 16);
        return widget;
    } else {
        return NULL;
    }
}


/** \brief  Create a group of radio buttons to set the ACIA IRQ source
 *
 * \return  GtkGrid with radio buttons or `NULL` when the current machine
 *          doesn't support selecting an IRQ
 */
static GtkWidget *create_acia_irq_widget(void)
{
    const vice_gtk3_radiogroup_entry_t *entries = NULL;
    GtkWidget *widget;

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:
            entries = acia_irqs;
            break;
        default:
            entries = NULL;
            break;
    }

    if (entries != NULL) {
        widget = vice_gtk3_resource_radiogroup_new("Acia1Irq", entries,
                GTK_ORIENTATION_HORIZONTAL);
        gtk_grid_set_column_spacing(GTK_GRID(widget), 16);
        return widget;
    } else {
        return NULL;
    }
}


/** \brief  Create a group of radio buttons to set the RS232 mode
 *
 * \return  GtkGrid with radio buttons or `NULL` when the current machine
 *          doesn't support selecting an interface
 */
static GtkWidget *create_acia_mode_widget(void)
{
    const vice_gtk3_radiogroup_entry_t *entries = NULL;
    GtkWidget *widget;

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:
            entries = acia_modes;
            break;
        default:
            entries = NULL;
            break;
    }

    if (entries != NULL) {
        widget = vice_gtk3_resource_radiogroup_new("Acia1Mode", entries,
                GTK_ORIENTATION_HORIZONTAL);
        gtk_grid_set_column_spacing(GTK_GRID(widget), 16);
        return widget;
    } else {
        return NULL;
    }
}


/** \brief  Create combo box with baud rates for serial devices
 *
 * \param[in]   resource    resource that contains the baud rate
 *
 * \return  GtkComboBox
 */
static GtkWidget *create_serial_baud_widget(const char *resource)
{
    const vice_gtk3_combo_entry_int_t *entries = NULL;

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:
            entries = serial_baud_rates_c64;
            break;
        case VICE_MACHINE_PLUS4:    /* fall through */
        case VICE_MACHINE_CBM5x0:   /* fall through */
        case VICE_MACHINE_CBM6x0:
            entries = serial_baud_rates_plus4;
            break;
        default:
            return NULL;
            break;
    }

    return vice_gtk3_resource_combo_box_int_new(resource, entries);
}


/** \brief  Create RS232 ACIA settings widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_acia_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *acia_enable_widget;
    GtkWidget *acia_device_widget;
    GtkWidget *acia_base_widget;
    GtkWidget *acia_irq_widget;
    GtkWidget *acia_mode_widget;
    int row = 1;    /* header label is always present */

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>ACIA settings</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);

    /* Acia1Enable */
    acia_enable_widget = create_acia_enable_widget();
    if (acia_enable_widget != NULL) {
        g_object_set(acia_enable_widget, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), acia_enable_widget, 0, row, 2, 1);
        row++;
    }

    /* Acia1Dev */
    label = create_indented_label("Device");
    acia_device_widget = vice_gtk3_resource_combo_box_int_new(
            "Acia1Dev", acia_devices);
    gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), acia_device_widget, 1, row, 1, 1);
    row++;

    /* Acia1Base */
    acia_base_widget = create_acia_base_widget();
    if (acia_base_widget != NULL) {
        label = create_indented_label("Base address");
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), acia_base_widget, 1, row, 1, 1);
        row++;
    }

    /* Acia1Irq */
    acia_irq_widget = create_acia_irq_widget();
    if (acia_irq_widget != NULL) {
        label = create_indented_label("IRQ");
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), acia_irq_widget, 1, row, 1, 1);
        row++;
    }

    /* Acia1Mode*/
    acia_mode_widget = create_acia_mode_widget();
    if (acia_mode_widget != NULL) {
        label = create_indented_label("Emulation mode");
        gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), acia_mode_widget, 1, row, 1, 1);
        row++;
    }

    gtk_widget_show_all(grid);
    return grid;
}



/** \brief  Handler for the 'toggled' event of the userport enable checkbox
 *
 * Set userport device to USERPORT_DEVICE_RS232 or USERPORT_DEVICE_NONE.
 *
 * \param[in]   self    userport enable check button
 * \param[in]   data    extra event data (unused)
 */
static void on_userport_enable_toggled(GtkWidget *self, gpointer data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self))) {
        /* set global userport device to rs232 modem */
        resources_set_int("UserportDevice", USERPORT_DEVICE_RS232_MODEM);
    } else {
        /* set global userport device to none */
        resources_set_int("UserportDevice", USERPORT_DEVICE_NONE);
    }
}


/** \brief  Create toggle button to enable the userport rs232 device
 *
 * Set userport device to USERPORT_DEVICE_RS232 or USERPORT_DEVICE_NONE.
 *
 * The old resource "RsUserEnable" has been removed in the new userport system
 * and we now set/unset the global "UserportDevice" resource.
 *
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_userport_enable_widget(void)
{
    GtkWidget *check;
    int device;

    if (resources_get_int("UserportDevice", &device) < 0) {
        device = USERPORT_DEVICE_NONE;
    }

    check = gtk_check_button_new_with_label("Enable userport RS232 emulation");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check),
                                 device == USERPORT_DEVICE_RS232_MODEM);
    g_signal_connect(check,
                     "toggled",
                     G_CALLBACK(on_userport_enable_toggled),
                     NULL);
    return check;
}


/** \brief  Create user-port RS232 emulation settings widget
 *
 * \note    C64,C128 and VIC-20 only
 *
 * \return  GtkGrid
 */
static GtkWidget *create_userport_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *rsuser_enable_widget;
    GtkWidget *rsuser_device_widget;
    GtkWidget *rsuser_rsinterface_widget;
    int up9600;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Userport RS232 settings</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 4, 1);

    rsuser_enable_widget = create_userport_enable_widget();
    gtk_widget_set_halign(rsuser_enable_widget, GTK_ALIGN_START);
    g_object_set(rsuser_enable_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), rsuser_enable_widget, 0, 1, 4, 1);

    /* RS-232 Interface Widget */
    rsuser_rsinterface_widget = userport_rsinterface_widget_create();
    gtk_grid_attach(GTK_GRID(grid), rsuser_rsinterface_widget, 0, 2, 4, 1);

    label = create_indented_label("Device");
    rsuser_device_widget = vice_gtk3_resource_combo_box_int_new(
            "RsUserDev", acia_devices);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rsuser_device_widget, 1, 3, 1, 1);
    label = create_indented_label("Baud");
    rsuser_baud_widget = vice_gtk3_resource_combo_box_int_new(
            "RsUserBaud", rsuser_baud_rates);
    gtk_grid_attach(GTK_GRID(grid), label, 2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rsuser_baud_widget, 3, 3, 1, 1);
    resources_get_int("RsUserUP9600", &up9600);

    gtk_widget_show_all(grid);
    userport_rsinterface_widget_add_callback((GtkGrid*)rsuser_rsinterface_widget, on_rsinterface_changed);
    return grid;
}


/** \brief  Create RS232 devices widget
 *
 * XXX: only supports Unix, Windows appears do things differently.
 *
 * \return  GtkGrid
 */
static GtkWidget *create_rs232_devices_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *ser1_file_widget;
    GtkWidget *ser1_baud_widget;
    GtkWidget *ser1_ip232_widget;
    GtkWidget *ser2_file_widget;
    GtkWidget *ser2_baud_widget;
    GtkWidget *ser2_ip232_widget;
    GtkWidget *ser3_file_widget;
    GtkWidget *ser3_baud_widget;
    GtkWidget *ser3_ip232_widget;
    GtkWidget *ser4_file_widget;
    GtkWidget *ser4_baud_widget;
    GtkWidget *ser4_ip232_widget;
    /* ttyu[0-3] are supposedly set up on FreeBSD */
    const char *patterns_ttys[] = { "ttyS*", "ttyu*", NULL };

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>RS232 devices</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);

    label = create_indented_label("Serial 1");
    ser1_file_widget = vice_gtk3_resource_browser_new(
            "RsDevice1", patterns_ttys, "Serial ports",
            "Select serial port", NULL, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser1_file_widget, 1, 1, 1, 1);
    label = gtk_label_new("Baud");
    ser1_baud_widget = create_serial_baud_widget("RsDevice1Baud");
    gtk_grid_attach(GTK_GRID(grid), label, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser1_baud_widget, 3, 1, 1, 1);
    ser1_ip232_widget = vice_gtk3_resource_check_button_new(
            "RsDevice1ip232", "IP232");
    gtk_grid_attach(GTK_GRID(grid), ser1_ip232_widget, 4, 1, 1, 1);

    label = create_indented_label("Serial 2");
    ser2_file_widget = vice_gtk3_resource_browser_new(
            "RsDevice2", patterns_ttys, "Serial ports",
            "Select serial port", NULL, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser2_file_widget, 1, 2, 1, 1);
    label = gtk_label_new("Baud");
    ser2_baud_widget = create_serial_baud_widget("RsDevice2Baud");
    gtk_grid_attach(GTK_GRID(grid), label, 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser2_baud_widget, 3, 2, 1, 1);
    ser2_ip232_widget = vice_gtk3_resource_check_button_new(
            "RsDevice2ip232", "IP232");
    gtk_grid_attach(GTK_GRID(grid), ser2_ip232_widget, 4, 2, 1, 1);

    label = create_indented_label("Serial 3");
    ser3_file_widget = vice_gtk3_resource_browser_new(
            "RsDevice3", patterns_ttys, "Serial ports",
            "Select serial port", NULL, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser3_file_widget, 1, 3, 1, 1);
    label = gtk_label_new("Baud");
    ser3_baud_widget = create_serial_baud_widget("RsDevice3Baud");
    gtk_grid_attach(GTK_GRID(grid), label, 2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser3_baud_widget, 3, 3, 1, 1);
    ser3_ip232_widget = vice_gtk3_resource_check_button_new(
            "RsDevice3ip232", "IP232");
    gtk_grid_attach(GTK_GRID(grid), ser3_ip232_widget, 4, 3, 1, 1);

    label = create_indented_label("Serial 4");
    ser4_file_widget = vice_gtk3_resource_browser_new(
            "RsDevice4", patterns_ttys, "Serial ports",
            "Select serial port", NULL, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser4_file_widget, 1, 4, 1, 1);
    label = gtk_label_new("Baud");
    ser4_baud_widget = create_serial_baud_widget("RsDevice4Baud");
    gtk_grid_attach(GTK_GRID(grid), label, 2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ser4_baud_widget, 3, 4, 1, 1);
    ser4_ip232_widget = vice_gtk3_resource_check_button_new(
            "RsDevice4ip232", "IP232");
    gtk_grid_attach(GTK_GRID(grid), ser4_ip232_widget, 4, 4, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create RS232 settings widget
 *
 * Invalid for PET, C64DTV and VSID
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_rs232_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *userport_widget;
    int row = 1;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, 32);

    /* guard against accidental use on unsupported machines */
    if (machine_class == VICE_MACHINE_C64DTV
            || machine_class == VICE_MACHINE_PET
            || machine_class == VICE_MACHINE_VSID) {
        char *text;
        GtkWidget *label;

        text = lib_msprintf(
                "<b>Error</b>: RS232 not supported for <b>%s</b>, please fix "
                "the code that calls this code!",
                machine_name);
        label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(label), text);
        gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
        gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
        lib_free(text);
        gtk_widget_show_all(grid);
        return grid;
    }

    gtk_grid_attach(GTK_GRID(grid), create_acia_widget(), 0, 0, 1, 1);
    gtk_widget_show_all(grid);

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:    /* fall through */
        case VICE_MACHINE_SCPU64:   /* fall through */
        case VICE_MACHINE_C128:     /* fall through */
        case VICE_MACHINE_VIC20:
            userport_widget = create_userport_widget();
            gtk_grid_attach(GTK_GRID(grid), userport_widget, 0, row, 1, 1);
            row++;
            break;
        default:
            break;
    }

    gtk_grid_attach(GTK_GRID(grid), create_rs232_devices_widget(), 0, row, 1, 1);

    return grid;
}
