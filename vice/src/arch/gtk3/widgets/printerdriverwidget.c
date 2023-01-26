/** \file   printerdriverwidget.c
 * \brief   Widget to control printer drivers
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Allows selecting drivers for printers \#4, \#5 and \#6.
 */

/*
 * $VICERES Printer4Driver  -vsid
 * $VICERES Printer5Driver  -vsid
 * $VICERES Printer6Driver  -vsid
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
#include <string.h>

#include "vice_gtk3.h"
#include "archdep.h"
#include "log.h"
#include "resources.h"
#include "printer.h"

#include "printerdriverwidget.h"


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * \param[in]   radio       radio button
 * \param[in]   user_data   new value for the resource (`const char *`)
 */
static void on_radio_toggled(GtkWidget *radio, gpointer user_data)
{

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {
        int device;
        const char *type;

        /* get device number from the "DeviceNumber" property of the radio
         * button */
        device = resource_widget_get_int(radio, "DeviceNumber");
        type = (const char *)user_data;
        resources_set_string_sprintf("Printer%dDriver", type, device);
    }
}

static GtkWidget *create_radio(GSList *group, const char *label, int device)
{
    GtkWidget *radio;

    radio = gtk_radio_button_new_with_label(group, label);
    g_object_set_data(G_OBJECT(radio), "DeviceNumber", GINT_TO_POINTER(device));
    return radio;
}


/** \brief  Create printer driver selection widget
 *
 * Creates a group of radio buttons to select the driver of printer # \a device.
 *
 * Uses a custom property "DeviceNumber" for the radio buttons and the widget
 * itself to pass the device number to the event handler and to allow
 * printer_driver_widget_update() to select the proper radio button index.
 *
 * Printer 4/5: [ascii, mps803, nl10, raw]
 * Printer 6  : [1520, raw]
 *
 * \param[in]   device  device number (4-6)
 *
 * \return  GtkGrid
 */
GtkWidget *printer_driver_widget_create(int device)
{
    GtkWidget  *grid;
    GtkWidget  *ascii  = NULL;
    GtkWidget  *mps803 = NULL;
    GtkWidget  *nl10   = NULL;
    GtkWidget  *raw    = NULL;
    GtkWidget  *v1520  = NULL;
    GSList     *group  = NULL;
    const char *driver;

    /* build grid */
    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "Driver", 1);
    vice_gtk3_grid_set_title_margin(grid, 8);
    /* set DeviceNumber property to allow the update function to work */
    resource_widget_set_int(grid, "DeviceNumber", device);

    if (device == 4 || device == 5) {
        /* 'normal' printers */

        /* ASCII */
        ascii = create_radio(group, "ASCII", device);
        gtk_grid_attach(GTK_GRID(grid), ascii, 0, 1, 1, 1);

        /* MPS 803 */
        mps803 = create_radio(group, "MPS-803", device);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(mps803),
                                    GTK_RADIO_BUTTON(ascii));
        gtk_grid_attach(GTK_GRID(grid), mps803, 0, 2, 1, 1);

        /* NL10 */
        nl10 = create_radio(group, "NL10", device);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(nl10),
                                    GTK_RADIO_BUTTON(mps803));
        gtk_grid_attach(GTK_GRID(grid), nl10, 0, 3, 1, 1);

        /* RAW */
        raw = create_radio(group, "RAW", device);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(raw),
                                    GTK_RADIO_BUTTON(nl10));
        gtk_grid_attach(GTK_GRID(grid), raw, 0, 4, 1, 1);
    } else if (device == 6) {
        /* plotter */

        /* Commodore VIC-1520 */
        v1520 = create_radio(group, "VIC-1520", device);
        gtk_grid_attach(GTK_GRID(grid), v1520, 0, 1, 1, 1);

        /* RAW */
        raw = create_radio(group, "RAW", device);
        gtk_radio_button_join_group(GTK_RADIO_BUTTON(raw),
                                    GTK_RADIO_BUTTON(v1520));
        gtk_grid_attach(GTK_GRID(grid), raw, 0, 2, 1, 1);
    } else {
        log_error(LOG_ERR,
                  "%s:%d:%s(): invalid device #%d\n",
                  __FILE__, __LINE__, __func__, device);
        archdep_vice_exit(1);
    }


    /* set current driver from resource */
    resources_get_string_sprintf("Printer%dDriver", &driver, device);
    printer_driver_widget_update(grid, driver);

    /* connect signal handlers */
    g_signal_connect(raw,
                     "toggled",
                     G_CALLBACK(on_radio_toggled),
                     (gpointer)"raw");

    if (device == 4 || device == 5) {
        g_signal_connect(ascii,
                         "toggled",
                         G_CALLBACK(on_radio_toggled),
                         (gpointer)"ascii");
        g_signal_connect(mps803,
                         "toggled",
                         G_CALLBACK(on_radio_toggled),
                         (gpointer)"mps803");
        g_signal_connect(nl10,
                         "toggled",
                         G_CALLBACK(on_radio_toggled),
                         (gpointer)"nl10");
    } else if (device == 6) {
        g_signal_connect(v1520,
                         "toggled",
                         G_CALLBACK(on_radio_toggled),
                         (gpointer)"1520");
    }

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Update the printer driver widget
 *
 * \param[in,out]   widget  printer driver widget
 * \param[in]       driver  driver name
 */
void printer_driver_widget_update(GtkWidget *widget, const char *driver)
{
    GtkWidget *radio;
    int index = 4;  /* RAW for 4/5 */
    int device;

    /* get device number from custom GObject property */
    device = GPOINTER_TO_INT(
            g_object_get_data(G_OBJECT(widget), "DeviceNumber"));

    /* this is a little silly, using string constants, but it works */
    if (device == 4 || device == 5) {
        if (strcmp(driver, "ascii") == 0) {
            index = 1;
        } else if (strcmp(driver, "mps803") == 0) {
            index = 2;
        } else if (strcmp(driver, "nl10") == 0) {
            index = 3;
        }
    } else if (device == 6) {
        if (strcmp(driver, "1520") == 0) {
            index = 1;
        } else {
            index = 2;  /* RAW */
        }
    } else {
        fprintf(stderr, "%s:%d:%s(): invalid printer device #%d\n",
                __FILE__, __LINE__, __func__, device);
        archdep_vice_exit(1);
    }

    /* now select the proper radio button */
    radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, index);
    if (radio != NULL && GTK_IS_RADIO_BUTTON(radio)) {
        /* set toggle button to active, this also sets the resource */
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
    }
}
