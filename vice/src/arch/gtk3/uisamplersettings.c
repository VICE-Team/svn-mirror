/** \file   src/arch/gtk3/uisamplersettings.c
 * \brief   Widget to control sampler settings
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  None
 *
 *  (see included widgets for more resources)
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
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"
#include "machine.h"
#include "sampler.h"
#include "basedialogs.h"

#include "uisamplersettings.h"


static sampler_device_t *(*devices_getter)(void) = NULL;


static void on_browse_clicked(GtkWidget *widget, gpointer user_data)
{
    ui_message_info(widget, "Select sampler input",
            "There is no resource for <i>SamplerInput</i> or whatever you"
            " want to call it.\n\n"
            "<b>This not normal method!</b>");
}


static GtkWidget *create_device_widget(void)
{
    GtkWidget *combo;
    sampler_device_t *devices;
    const char *current;
    int i;

    resources_get_string("SamplerDevice", &current);

    combo = gtk_combo_box_text_new();
    if (devices_getter != NULL) {
        devices = devices_getter();
    } else {
        return combo;
    }
    for (i = 0; devices[i].name != NULL; i++) {
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo),
                devices[i].name, devices[i].name);
        if (strcmp(devices[i].name, current) == 0) {
            gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
        }
    }
    return combo;
}


static GtkWidget *create_gain_widget(void)
{
    GtkWidget *scale;
    GtkWidget *label;
    int value;
    int i;

    label = gtk_label_new("Sampler gain");
    g_object_set(label, "margin-left",16, NULL);

    scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
            0.0, 200.0, 25.0);
    gtk_scale_set_digits(GTK_SCALE(scale), 0);

    /* add tick marks */
    for (i = 0; i < 200; i += 25) {
        gtk_scale_add_mark(GTK_SCALE(scale), (gdouble)i, GTK_POS_BOTTOM, NULL);
    }


    if (resources_get_int("SamplerGain", &value) >= 0) {
        gtk_range_set_value(GTK_RANGE(scale), (gdouble)value);
    } else {
        gtk_range_set_value(GTK_RANGE(scale), 100.0);
    }

    gtk_widget_show_all(scale);
    return scale;
}


static GtkWidget *create_input_entry(void)
{
    GtkWidget *entry;

    entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), "I haz no resource");
    return entry;
}


static GtkWidget *create_input_button(void)
{
    GtkWidget *button;

    button = gtk_button_new_with_label("Browse ...");
    g_signal_connect(button, "clicked", G_CALLBACK(on_browse_clicked), NULL);
    return button;
}


void uisamplersettings_set_devices_getter(sampler_device_t *(func)(void))
{
    devices_getter = func;
}


GtkWidget *uisamplersettings_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *entry;

    grid = uihelpers_create_grid_with_label("Sampler settings", 3);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    /* sampler device */
    label = gtk_label_new("Sampler device");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_device_widget(), 1, 1, 2, 1);

    label = gtk_label_new("Sampler gain");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_gain_widget(), 1, 2, 2, 1);

    gtk_widget_set_halign(label, GTK_ALIGN_START);
    label = gtk_label_new("Sampler media file");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    entry = create_input_entry();
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), create_input_button(), 2, 3, 1, 1);



    gtk_widget_show_all(grid);
    return grid;
}

