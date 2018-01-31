/** \file   settings_rs232t.c
 * \brief   Widget to control various RS232 resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Acia1Enable     (x64/x64sc/xscpu64/x128/xvic)
 *  Acia1Dev        (x64/x64sc/xscpu64/x128/xvic/xplus4/xcbm5x0/xcbm2)
 *  Acia1Base       (x64/x64sc/xscpu64/x128/xvic)
 *  Acia1Irq        (x64/x64sc/xscpu64/x128/xvic)
 *  Acia1Mode       (x64/x64sc/xscpu64/x128/xvic)
 *  RsUserEnable    (x64/x64sc/xscpu64/x128/xvic)
 *  RsUserBaud      (x64/x64sc/xscpu64/x128/xvic)
 *  RsUserDev       (x64/x64sc/xscpu64/x128/xvic)
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

#include "settings_rs232.h"


/** \brief  List of ACIA devices
 *
 * \note    Also used by the Userport RS232 Device list combo box
 */
static const vice_gtk3_combo_entry_int_t acia_devices[] = {
    { "Serial 1", 0 },
    { "Serial 2", 1 },
    { "Dump to file", 2 },
    { "Execute process", 3 },
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
    { "IRQ",    1 },
    { "NMI",    2 },
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
    { NULL, -1 }
};


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
            return vice_gtk3_resource_check_button_create("Acia1Enable",
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
        widget = vice_gtk3_resource_radiogroup_create("Acia1Base", entries,
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
        widget = vice_gtk3_resource_radiogroup_create("Acia1Irq", entries,
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
        widget = vice_gtk3_resource_radiogroup_create("Acia1Mode", entries,
                GTK_ORIENTATION_HORIZONTAL);
        gtk_grid_set_column_spacing(GTK_GRID(widget), 16);
        return widget;
    } else {
        return NULL;
    }
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
    acia_device_widget = vice_gtk3_resource_combo_box_int_create(
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


/** \brief  Create user-port RS233 emulation settings widget
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
    GtkWidget *rsuser_baud_widget;
    GtkWidget *rsuser_device_widget;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), "<b>Userport RS232 settings</b>");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);

    rsuser_enable_widget = vice_gtk3_resource_check_button_create(
            "RsUserEnable", "Enable Userport RS232 emulation");
    gtk_widget_set_halign(rsuser_enable_widget, GTK_ALIGN_START);
    g_object_set(rsuser_enable_widget, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), rsuser_enable_widget, 0, 1, 2, 1);

    label = create_indented_label("Baud rate");
    rsuser_baud_widget = vice_gtk3_resource_combo_box_int_create(
            "RsUserBaud", rsuser_baud_rates);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rsuser_baud_widget, 1, 2, 1, 1);

    label = create_indented_label("Device");
    rsuser_device_widget = vice_gtk3_resource_combo_box_int_create(
            "RsUserDev", acia_devices);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rsuser_device_widget, 1, 3, 1, 1);



    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create RS232 settings widget
 *
 * Invalid for PET, C64DTV and VSID
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *settings_rs232_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *userport_widget;
    int row = 1;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, 32);
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

    return grid;
}

