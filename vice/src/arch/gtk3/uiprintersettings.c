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


static void on_iec_toggled(GtkRadioButton *radio, gpointer user_data)
{
    int state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio));
    int device = GPOINTER_TO_INT(user_data);

    debug_gtk3("setting IECDevice%d to %s\n", device, state ? "ON" : "OFF");
    resources_set_int_sprintf("IECDevice%d", state, device);
}


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
    int p;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);

    for (p = PRINTER_MIN; p <= PRINTER_MAX; p++) {
        gtk_grid_attach(GTK_GRID(layout),
                create_printer_widget(p),
                0, p - PRINTER_MIN + 1, 1, 1);
    }

    gtk_widget_show_all(layout);
    return layout;
}
