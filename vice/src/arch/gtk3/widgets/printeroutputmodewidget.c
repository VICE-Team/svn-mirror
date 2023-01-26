/** \file   printeroutputmodewidget.c
 * \brief   Widget to control printer output mode settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES Printer4Output  -vsid
 * $VICERES Printer5Output  -vsid
 * $VICERES Printer6Output  -vsid
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

#include "resources.h"
#include "vice_gtk3.h"

#include "printeroutputmodewidget.h"


/** \brief  Handler for the 'destroy' event of \a widget
 *
 * Frees memory used by the copy of the resource name.
 *
 * \param[in,out]   widget      widget
 * \param[in]       user_data   extra event data (unused)
 */
static void on_widget_destroy(GtkWidget *widget, gpointer user_data)
{
    resource_widget_free_resource_name(widget);
}

/** \brief  Handler for the 'toggled' event of the radio buttons
 *
 * \param[in]   radio   radio button
 * \param[in]   mode    new output mode
 */
static void on_radio_toggled(GtkWidget *radio, gpointer mode)
{
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio))) {

        GtkWidget  *parent;
        const char *new_val;
        const char *old_val = NULL;
        const char *resource;

        parent   = gtk_widget_get_parent(radio);
        resource = resource_widget_get_resource_name(parent);
        new_val  = (const char *)mode;
        resources_get_string(resource, &old_val);

        if (g_strcmp0(new_val, old_val) != 0) {
            resources_set_string(resource, new_val);
        }
    }
}


/** \brief  Create widget to control Printer[device]TextDevice resource
 *
 * \param[in]   device  device number
 *
 * \return  GtkGrid
 */
GtkWidget *printer_output_mode_widget_create(int device)
{
    GtkWidget  *grid;
    GtkWidget  *text;
    GtkWidget  *gfx;
    GSList     *group = NULL;
    const char *value = NULL;
    char        resource[256];

    /* can't use the resource base widgets here, since for some reason this
     * resource is a string with two possible values: "text" and "graphics"
     */

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "Output mode", 1);
    vice_gtk3_grid_set_title_margin(grid, 8);

    g_snprintf(resource, sizeof resource , "Printer%dOutput", device);
    resource_widget_set_resource_name(grid, resource);

    text = gtk_radio_button_new_with_label(group, "Text");
    gfx  = gtk_radio_button_new_with_label(group, "Graphics");
    gtk_radio_button_join_group(GTK_RADIO_BUTTON(gfx),
                                GTK_RADIO_BUTTON(text));

    resources_get_string(resource, &value);
    if (g_strcmp0(value, "text") == 0) {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(text), TRUE);
    } else {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gfx), TRUE);
    }

    gtk_grid_attach(GTK_GRID(grid), text, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gfx,  0, 2, 1, 1);

    g_signal_connect(text,
                     "toggled",
                     G_CALLBACK(on_radio_toggled),
                     (gpointer)"text");
    g_signal_connect(gfx,
                     "toggled",
                     G_CALLBACK(on_radio_toggled),
                     (gpointer)"graphics");
    g_signal_connect_unlocked(grid,
                              "destroy",
                              G_CALLBACK(on_widget_destroy),
                              NULL);

    gtk_widget_show_all(grid);
    return grid;
}
