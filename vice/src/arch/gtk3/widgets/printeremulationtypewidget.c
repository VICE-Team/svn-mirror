/** \file   src/arch/gtk3/widgets/printeremulationtypewidget.c
 * \brief   Widget to control printer emulation type
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  Printer4    - printer #4 emulation type [0..2] (`int`)
 *  Printer5    - printer #5 emulation type [0..2] (`int`)
 *  Printer6    - printer #6 emulation type [0..2] (`int`)
 *
 * \see src/printer.h for the constants.
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

#include "printeremulationtypewidget.h"


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * \param[in]   radio       radio button
 * \param[in]   user_data   new value for the resource (`int`)
 */
static void on_radio_toggled(GtkRadioButton *radio, gpointer user_data)
{

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {
        int device;
        int type;

        /* get device number from the "DeviceNumber" property of the radio
         * button */
        device = GPOINTER_TO_INT(
                g_object_get_data(G_OBJECT(radio), "DeviceNumber"));
        type = GPOINTER_TO_INT(user_data);
        debug_gtk3("setting Printer%d to %d\n", device, type);
        resources_set_int_sprintf("Printer%d", type, device);
    }
}


/** \brief  Create printer emulation type widget
 *
 * Creates a group of radio buttons to select the emulation type of printer
 * # \a device. Uses a custom property "DeviceNumber" for the radio buttons to
 * pass the device number to the event handler.
 *
 * \param[in]   device  device number (4-6)
 *
 * \return  GtkGrid
 */
GtkWidget *printer_emulation_type_widget_create(int device)
{
    GtkWidget *grid;
    GtkWidget *radio_none;
    GtkWidget *radio_fs;
    GtkWidget *radio_real;
    GSList *group = NULL;
    int type;

    /* build grid */
    grid = uihelpers_create_grid_with_label("Emulation type", 1);

    /* PRINTER_DEVICE_NONE */
    radio_none = gtk_radio_button_new_with_label(group, "None");
    g_object_set_data(G_OBJECT(radio_none), "DeviceNumber",
            GINT_TO_POINTER(device));
    g_object_set(radio_none, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_none, 0, 1, 1, 1);

    /* PRINTER_DEVICE_FS */
    radio_fs = gtk_radio_button_new_with_label(group, "File system access");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio_fs),
            GTK_RADIO_BUTTON(radio_none));
    g_object_set_data(G_OBJECT(radio_fs), "DeviceNumber",
            GINT_TO_POINTER(device));
    g_object_set(radio_fs, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_fs, 0, 2, 1, 1);

    /* PRINTER_DEVICE_REAL */
    radio_real = gtk_radio_button_new_with_label(group, "Real device access");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio_real),
            GTK_RADIO_BUTTON(radio_fs));
    g_object_set_data(G_OBJECT(radio_real), "DeviceNumber",
            GINT_TO_POINTER(device));
    g_object_set(radio_real, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_real, 0, 3, 1, 1);

    /* set current type from resource */
    resources_get_int_sprintf("Printer%d", &type, device);
    printer_emulation_type_widget_update(grid, type);

    /* connect signal handlers */
    g_signal_connect(radio_none, "toggled", G_CALLBACK(on_radio_toggled),
            GINT_TO_POINTER(PRINTER_DEVICE_NONE));
    g_signal_connect(radio_fs, "toggled", G_CALLBACK(on_radio_toggled),
            GINT_TO_POINTER(PRINTER_DEVICE_FS));
    g_signal_connect(radio_real, "toggled", G_CALLBACK(on_radio_toggled),
            GINT_TO_POINTER(PRINTER_DEVICE_REAL));


    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the printer emulation type widget
 *
 * \param[in]   widget  printer emulation type widget
 * \param[in]   type    emulation type (\see src/printer.h)
 */
void printer_emulation_type_widget_update(GtkWidget *widget, int type)
{
    GtkWidget *radio;

    radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, type + 1);
    if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
        /* set toggle button to active, this also sets the resource */
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
    }
}
