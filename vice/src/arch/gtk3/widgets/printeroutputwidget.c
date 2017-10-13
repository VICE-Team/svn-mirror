/** \file   src/arch/gtk3/widgets/printeroutputwidget.c
 * \brief   Widget to control printer output settings
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "printer.h"

#include "printeroutputwidget.h"



static void on_text_device_toggled(GtkRadioButton *radio, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {
        int device = GPOINTER_TO_INT(
                g_object_get_data(G_OBJECT(radio), "DeviceNumber"));
        int value = GPOINTER_TO_INT(user_data);

        debug_gtk3("setting Printer%dTextDevice to %d\n", device, value);
        resources_set_int_sprintf("Printer%dTextDevice", value, device);
    }
}



static void on_output_mode_toggled(GtkRadioButton *radio, gpointer user_data)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {
        const char *mode = (const char *)user_data;
        int device = GPOINTER_TO_INT(
                g_object_get_data(G_OBJECT(radio), "DeviceNumber"));
        debug_gtk3("setting Printer%dOutput to '%s'\n", device, mode);
        resources_set_string_sprintf("Printer%dOutput", mode, device);
    }
}


/** \brief  Create text output device selection widget for \a device
 *
 * This controls the Printer[4-6]TextDevice resource, which in turn depends on
 * the PrintTextDevice[1-3] resource. (Yep, very clear)
 *
 * \param[in]   device  device number (4-6)
 *
 * \return  GtkGrid
 */
static GtkWidget *create_text_device_widget(int device)
{
    GtkWidget *grid;
    const char *labels[] = { "#1 (file dump)", "#2 (exec)", "#3 (exec)" };
    size_t index;
    GSList *group = NULL;
    GtkRadioButton *last = NULL;
    int text_dingus;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    resources_get_int_sprintf("Printer%dTextDevice", &text_dingus, device);

    for (index = 0; index < sizeof labels / sizeof labels[0]; index++) {
        GtkWidget *radio;

        radio = gtk_radio_button_new_with_label(group, labels[index]);
        g_object_set_data(G_OBJECT(radio), "DeviceNumber",
                GINT_TO_POINTER(device));
        if (last != NULL) {
            gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio), last);
        } else {
            /* indent first radio button */
            g_object_set(radio, "margin-left", 16, NULL);
        }

        if (text_dingus == index) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        }

        g_signal_connect(radio, "toggled", G_CALLBACK(on_text_device_toggled),
                GINT_TO_POINTER(index));

        gtk_grid_attach(GTK_GRID(grid), radio, index, 0, 1, 1);
        last = GTK_RADIO_BUTTON(radio);
    }


    gtk_widget_show_all(grid);
    return grid;
}



static GtkWidget *create_output_mode_widget(int device)
{
    GtkWidget *grid;
#if 0
    GtkWidget *label;
#endif
    GtkWidget *radio_text;
    GtkWidget *radio_gfx;
    GSList *group = NULL;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

#if 0
    label = gtk_label_new("Mode");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
#endif

    /* change column number (+1) if removing the above #ifdef's */

    radio_text = gtk_radio_button_new_with_label(group, "Text");
    g_object_set_data(G_OBJECT(radio_text), "DeviceNumber",
            GINT_TO_POINTER(device));
    g_object_set(radio_text, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_text, 0, 0, 1, 1);

    radio_gfx = gtk_radio_button_new_with_label(group, "Graphics");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio_gfx),
            GTK_RADIO_BUTTON(radio_text));
    g_object_set_data(G_OBJECT(radio_gfx), "DeviceNumber",
            GINT_TO_POINTER(device));
    gtk_grid_attach(GTK_GRID(grid), radio_gfx, 1, 0, 1, 1);


    g_signal_connect(radio_text, "toggled", G_CALLBACK(on_output_mode_toggled),
            (gpointer)"text");
    g_signal_connect(radio_gfx, "toggled", G_CALLBACK(on_output_mode_toggled),
            (gpointer)"graphics");

    gtk_widget_show_all(grid);
    return grid;
}




/** \brief  Create printer output settings widget
 *
 * \param[in]   device  device number [4..6]
 *
 * \return  GtkGrid
 */
GtkWidget *printer_output_widget_create(int device)
{
    GtkWidget *grid;


    grid = uihelpers_create_grid_with_label("Output", 3);

    gtk_grid_attach(GTK_GRID(grid), create_output_mode_widget(device),
            0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_text_device_widget(device),
            0, 2, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
