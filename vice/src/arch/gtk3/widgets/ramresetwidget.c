/** \file   src/arch/gtk3/widgets/ramresetwidgetc.c
 * \brief   Control the RAM reset pattern settings
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  RAMInitStartValue       - value for the first RAM address
 *  RamInitValueInvert      - ???
 *  RAMInitPatternInvert    - ???
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
#include <math.h>

#include "widgethelpers.h"
#include "debug_gtk3.h"
#include "resources.h"

#include "ramresetwidget.h"


/** \brief  List of powers of two used for the widgets
 */
static const int powers_of_two[] = {
    0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, -1
};


/* for some reason GtkSpinButton refuses displaying/parsing hex values, I even
 * followed the demo code at GitHub, still no dice
 */
#if 0

static gint on_start_value_input(GtkSpinButton *spin, gdouble *new_value)
{
    const gchar *text;
    char *endptr;
    gdouble res;

    text = gtk_entry_get_text(GTK_ENTRY(spin));
    res = strtol(text, &endptr, 16);
    *new_value = res;
    if (*endptr != '\0') {
        return GTK_INPUT_ERROR;
    }
    return TRUE;
}



static gboolean on_start_value_output(GtkSpinButton *spin)
{
    GtkAdjustment *adjustment;
    gchar *text;
    gint value;

    adjustment = gtk_spin_button_get_adjustment(spin);
    value = (gint)gtk_adjustment_get_value(adjustment);

    printf("SPIN VALUE = %d\n", value);
    text = g_strdup_printf("0x%.02X", value);
    if (strcmp(text, gtk_entry_get_text(GTK_ENTRY(spin))) == 0) {
        gtk_entry_set_text(GTK_ENTRY(spin), text);
    }
    g_free(text);

    return TRUE;
}
#endif


/** \brief  Handler for the "changed" event of the start value widget
 *
 * \param[in]   spin_button spin button triggering the event
 */
static void on_start_value_changed(GtkSpinButton *spin_button)
{
    int value = (int)gtk_spin_button_get_value(spin_button);
    debug_gtk3("setting RAMInitStartValue to %d\n", value);
    resources_set_int("RAMInitStartValue", value);
}


/** \brief  Create a spin button controlling the "RAMInitStartValue" resource
 *
 * \return  GtkSpinButton
 */
static GtkWidget *create_start_value_widget(void)
{
    GtkWidget *spin;
    int value;
#if 0
    GtkAdjustment *adjustment;

    adjustment = gtk_adjustment_new(0.0, 0.0, 255.0, 1.0, 16.0, 0.0);
    spin = gtk_spin_button_new(adjustment, 1.0, 0);
#endif
    spin = gtk_spin_button_new_with_range(0.0, 255.0, 1.0);
    gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spin), 0);
    resources_get_int("RAMInitStartValue", &value);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin), (gdouble)value);
#if 0
    g_signal_connect(spin, "input", G_CALLBACK(on_start_value_input), NULL);
    g_signal_connect(spin, "output", G_CALLBACK(on_start_value_output), NULL);
#endif

    g_signal_connect(spin, "changed", G_CALLBACK(on_start_value_changed), NULL);

    return spin;
}


/** \brief  Create widget to control RAM init settings
 *
 * \return  GtkGrid
 */
GtkWidget *ram_reset_widget_create(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *start_value_widget;
    GtkWidget *value_invert_widget;
    GtkWidget *pattern_invert_widget;

    grid = uihelpers_create_grid_with_label("RAM reset pattern", 2);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("Value of first byte");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    start_value_widget = create_start_value_widget();
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), start_value_widget, 1, 1, 1, 1);

    label = gtk_label_new("Length of constant values");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    value_invert_widget = uihelpers_create_int_combo_box(powers_of_two,
            "RAMInitValueInvert");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), value_invert_widget, 1, 2, 1, 1);

    label = gtk_label_new("Length of constant patterns");
    g_object_set(label, "margin-left", 16, NULL);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    pattern_invert_widget = uihelpers_create_int_combo_box(powers_of_two,
            "RAMInitPatternInvert");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pattern_invert_widget, 1, 3, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create central widget for ui_settings.c for the RAM init settings
 *
 * Placeholder, seems to me the RAM init widget can be combined with other
 * widgets into a more complete uisettings sub-widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *create_ram_reset_central_widget(GtkWidget *parent)
{
    return ram_reset_widget_create();
}
