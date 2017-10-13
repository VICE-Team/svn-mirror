/** \file   src/arch/gtk3/uiprintersettings.c
 * \brief   Widget to control printer settings
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
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

#include <gtk/gtk.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "printer.h"

/* widgets */
#include "printeremulationtypewidget.h"
#include "printerdriverwidget.h"
#include "printeroutputwidget.h"

#include "uiprintersettings.h"


#define PRINTER_NUM 4   /**< number of printer devices supported */
#define PRINTER_MIN 4   /**< lowest device number for a printer */
#define PRINTER_MAX 7   /**< highest device number for a printer */


/** \brief  Handler for the "toggled" event of the Printer7 checkbox
 *
 * Switches between `PRINTER_DEVICE_NONE` and `PRINTER_DEVICE_REAL`.
 *
 * \param[in]   check       check button
 * \param[in]   user_data   extra data (unused)
 */
static void on_real_device7_toggled(GtkCheckButton *check, gpointer user_data)
{
    int state;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check))) {
        state = PRINTER_DEVICE_REAL;
    } else {
        state = PRINTER_DEVICE_NONE;
    }
    debug_gtk3("setting Printer7 to '%s'\n", state ? "REAL" : "NONE");
    resources_set_int("Printer7", state);
}



/** \brief  Handler for the "toggled" event of the IEC checkboxes
 *
 * \param[in]   check       IEC checkbox
 * \param[in]   user_data   device number (`int`)
 */
static void on_iec_toggled(GtkCheckButton *check, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check));
    int device = GPOINTER_TO_INT(user_data);

    debug_gtk3("setting IECDevice%d to %s\n", device, state ? "ON" : "OFF");
    resources_set_int_sprintf("IECDevice%d", state, device);
}


/** \brief  Create IEC device emulation widget for \a device
 *
 * \param[in]   device  printer device
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_iec_widget(int device)
{
    GtkWidget *check;
    int value;

    resources_get_int_sprintf("IECDevice%d", &value, device);

    check = gtk_check_button_new_with_label("Enable IEC device");
    g_object_set(check, "margin-left", 16, NULL);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), value);

    g_signal_connect(check, "toggled", G_CALLBACK(on_iec_toggled),
            GINT_TO_POINTER(device));
    return check;
}


/** \brief  Create checkbox to switch between NONE/REAL emu mode for Printer7
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_real_device7_checkbox(void)
{
    GtkWidget *check;
    int value;

    check = gtk_check_button_new_with_label("Real device access");
    resources_get_int("Printer7", &value);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check), value);
    g_signal_connect(check, "toggled", G_CALLBACK(on_real_device7_toggled),
            NULL);
    return check;
}



/** \brief  Create a widget for the settings of printer # \a device
 *
 * Creates a widget for \a device to control its resource. The widget for
 * device #7 is different/simpler.
 *
 * \param[in]   device  device number (4-7)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_printer_widget(int device)
{
    GtkWidget *grid;
    char title[256];

    g_snprintf(title, 256, "Printer #%d settings", device);

    grid = uihelpers_create_grid_with_label(title, 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    if (device == 4 || device == 5 || device == 6) {
        /* device 4,5,6 are 'normal' printers */
        GtkWidget *wrapper;

        wrapper = gtk_grid_new();

        gtk_grid_attach(GTK_GRID(wrapper),
                printer_emulation_type_widget_create(device), 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(wrapper),
                create_iec_widget(device), 0, 1, 1, 1);

        gtk_grid_attach(GTK_GRID(grid), wrapper, 0, 1, 1, 1);



        gtk_grid_attach(GTK_GRID(grid),
                printer_driver_widget_create(device), 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid),
                printer_output_widget_create(device), 2, 1, 1, 1);
    } else if (device == 7) {
        /* device 7 is 'special' */
        GtkWidget *iec_widget = create_iec_widget(device);

        g_object_set(iec_widget, "margin-left", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), iec_widget, 0, 1, 1, 1);

        gtk_grid_attach(GTK_GRID(grid), create_real_device7_checkbox(),
                1, 1, 1,1);
    }

    gtk_widget_show_all(grid);
    return grid;
}


static GtkWidget *create_userport_widget(void)
{
    GtkWidget *grid;

    grid = uihelpers_create_grid_with_label("Userport printer settings", 3);
    gtk_widget_show_all(grid);
    return grid;
}



static GtkWidget *create_printer_text_devices_widget(void)
{
    GtkWidget *grid;
    int i;

    grid = uihelpers_create_grid_with_label("Printer text output devices", 6);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    for (i = 0; i < 3; i++) {
        GtkWidget *label;
        GtkWidget *entry;
        const char *text;
        gchar title[64];

        g_snprintf(title, 64, "#%d", i + 1);

        label= gtk_label_new(title);
        if (i == 0) {
            g_object_set(label, "margin-left", 16, NULL);
        }
        gtk_grid_attach(GTK_GRID(grid), label, i * 2, 1, 1, 1);
        entry = gtk_entry_new();
        resources_get_string_sprintf("PrinterTextDevice%d", &text, i + 1);
        gtk_entry_set_text(GTK_ENTRY(entry), text);
        gtk_grid_attach(GTK_GRID(grid), entry, i * 2 + 1, 1, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to control printer settings
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uiprintersettings_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *stack;
    GtkWidget *stack_switcher;
    GtkWidget *layout_printers;
    GtkWidget *layout_userport;
    int p;

    layout = gtk_grid_new();

    stack = gtk_stack_new();
    gtk_stack_set_transition_type(GTK_STACK(stack),
            GTK_STACK_TRANSITION_TYPE_SLIDE_LEFT);
    gtk_stack_set_transition_duration(GTK_STACK(stack), 1000);

    layout_printers = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout_printers), 8);

    for (p = PRINTER_MIN; p <= PRINTER_MAX; p++) {
        gtk_grid_attach(GTK_GRID(layout_printers),
                create_printer_widget(p),
                0, p - PRINTER_MIN + 1, 1, 1);
    }
    gtk_widget_show_all(layout_printers);

    layout_userport = create_userport_widget();

    gtk_stack_add_titled(GTK_STACK(stack), layout_printers, "printers",
            "Standard printers");
    gtk_stack_add_titled(GTK_STACK(stack), layout_userport, "userport",
            "Userport printer");

    stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher),
            GTK_STACK(stack));
    gtk_widget_set_halign(stack_switcher, GTK_ALIGN_CENTER);

    gtk_widget_show_all(stack);
    gtk_widget_show_all(stack_switcher);

    gtk_grid_attach(GTK_GRID(layout), stack_switcher, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), stack, 0, 1, 1, 1);

    gtk_orientable_set_orientation(GTK_ORIENTABLE(stack_switcher),
            GTK_ORIENTATION_HORIZONTAL);

    gtk_grid_attach(GTK_GRID(layout), create_printer_text_devices_widget(),
            0, 2, 1, 1);

    gtk_widget_show_all(layout);
    return layout;
}
