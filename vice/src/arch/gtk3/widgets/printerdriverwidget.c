/** \file   printerdriverwidget.c
 * \brief   Widget to control printer drivers
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Allows selecting drivers for printers \#4, \#5, \#6 and userport printer.
 */

/*
 * $VICERES Printer4Driver          -vsid
 * $VICERES Printer5Driver          -vsid
 * $VICERES Printer6Driver          -vsid
 * $VICERES PrinterUserportDriver   x64 x64sc xscpu64 x128 xvic xpet xcbm2
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

#include "log.h"
#include "resources.h"
#include "vice_gtk3.h"

#include "printerdriverwidget.h"


/** \brief  Radio buttons for a device */
typedef struct driver_radio_s {
    const char *label;  /**< radio button label */
    const char *value;  /**< resource value */
} driver_radio_t;

/** \brief  Resource and resource data for a device */
typedef struct driver_resource_s {
    const char           *name;     /**< resource name */
    const driver_radio_t *drivers;  /**< driver radio buttons for device */
} driver_resource_t;


/** \brief  Drivers for printer 4 & 5 */
static const driver_radio_t printer_drivers[] = {
    { "ASCII",                "ascii" },
    { "Commodore 2022",       "2022" },
    { "Commodore 4023",       "4023" },
    { "Commodore 8023",       "8023" },
    { "MPS-801",              "mps801" },
    { "MPS-802",              "mps802" },
    { "MPS-803",              "mps803" },
    { "NL10",                 "nl10" },
    { "RAW",                  "raw" },
    { NULL,                   NULL }
};

/** \brief  Drivers for plotter 6 */
static const driver_radio_t plotter_drivers[] = {
    { "VIC-1520",   "1520" },
    { "RAW",        "raw" },
    { NULL,         NULL }
};

/** \brief  Drivers for printer 4 & 5 */
static const driver_radio_t userport_drivers[] = {
    { "ASCII",      "ascii" },
    { "NL10",       "nl10" },
    { "RAW",        "raw" },
    { NULL,         NULL }
};

/** \brief  Resources and values per device
 *
 * Note: index 0 is device 3/userport, index 1-3 are printer 4, 5 and plotter 6
 */
static const driver_resource_t driver_resources[] = {
    { "PrinterUserportDriver",  userport_drivers },
    { "Printer4Driver",         printer_drivers },
    { "Printer5Driver",         printer_drivers },
    { "Printer6Driver",         plotter_drivers }
};


/** \brief  Handler for the "toggled" event of the radio buttons
 *
 * \param[in]   radio       radio button
 * \param[in]   user_data   new value for the resource (`const char *`)
 */
static void on_radio_toggled(GtkWidget *radio, gpointer user_data)
{

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {
        GtkWidget  *parent;
        const char *resource;
        const char *driver;

        parent   = gtk_widget_get_parent(radio);
        resource = resource_widget_get_resource_name(parent);
        driver   = g_object_get_data(G_OBJECT(radio), "Driver");
#if 0
        debug_gtk3("Setting resource '%s' to '%s'", resource, driver);
#endif
        resources_set_string(resource, driver);
    }
}


/** \brief  Create printer driver selection widget
 *
 * Creates a group of radio buttons to select the driver of printer # \a device.
 *
 * Uses a custom property "Driver" for the radio buttons and stores the
 * resource name for \a device as the "ResourceName" property.
 *
 * Printer 4/5  : [ascii, mps803, nl10, raw]
 * Printer 6    : [1520, raw]
 * Userport (3) : [ascii, nl10, raw]
 *
 * \param[in]   device  device number (4-6) or 3 for userport
 *
 * \return  GtkGrid
 */
GtkWidget *printer_driver_widget_create(int device)
{
    GtkWidget            *grid;
    GtkWidget            *last;
    GSList               *group;
    const driver_radio_t *drivers;
    const char           *current = NULL;
    int                   index;
    int                   drv;
    int                   row;

    /* sanity check */
    if (device < 3 || device > 6) {
        log_error(LOG_ERR,
                  "%s(): invalid device number %d, valid device numbers are"
                  " 4-6 or 3 for userport",
                  __func__, device);
        return NULL;
    }
    index = device - 3;


    /* build grid */
    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "Driver", 1);
    vice_gtk3_grid_set_title_margin(grid, 8);

    /* get current resource value */
    resources_get_string(driver_resources[index].name, &current);

    /* set resource name to allow the update function to work */
    resource_widget_set_resource_name(grid, driver_resources[index].name);

    /* create radio buttons */
    drivers = driver_resources[index].drivers;
    last    = NULL;
    group   = NULL;
    row     = 1;
    for (drv = 0; drivers[drv].label != NULL; drv++) {
        GtkWidget *radio = gtk_radio_button_new_with_label(group,
                                                           drivers[drv].label);

        /* No need to use g_strdup() since the string is always available.
         * We could use the signal handler's `data` argument to pass this
         * value, but we also need the value for the update() function to work
         */
        g_object_set_data(G_OBJECT(radio), "Driver", (gpointer)drivers[drv].value);

        if (last != NULL) {
            gtk_radio_button_join_group(GTK_RADIO_BUTTON(radio),
                                        GTK_RADIO_BUTTON(last));
        }
        if (g_strcmp0(current, drivers[drv].value) == 0) {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
        }

        g_signal_connect(G_OBJECT(radio),
                         "toggled",
                         G_CALLBACK(on_radio_toggled),
                         NULL);

        gtk_grid_attach(GTK_GRID(grid), radio, 0, row, 1, 1);
        row++;
        last = radio;
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
    int row = 1;

    while (TRUE) {
        GtkWidget *radio = gtk_grid_get_child_at(GTK_GRID(widget), 0, row);

        if (radio == NULL) {
            break;  /* end of grid, give up */
        } else if (GTK_IS_RADIO_BUTTON(radio)) {
            const char *value = g_object_get_data(G_OBJECT(radio), "Driver");
            if (g_strcmp0(driver, value) == 0) {
                /* this also triggers the signal handler, updating the resource */
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio), TRUE);
                break;
            }
        }
        row++;
    }
}
