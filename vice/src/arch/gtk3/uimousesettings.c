/** \file   src/arch/gtk3/uimousesettings.c
 * \brief   Mouse settings widget
 *
 * Controls the following resource(s):
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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

#include "debug_gtk3.h"
#include "resources.h"
#include "resourcecheckbutton.h"

#include "uimousesettings.h"


static GtkWidget *create_sensitivity_widget(void)
{
    GtkWidget * grid;
    GtkWidget * scale;
    GtkWidget * label;
    int         current;

    if (resources_get_int("MouseSensitivity", &current) < 0) {
        current = 20;
    }

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    g_object_set(grid, "margin", 16, NULL);

    label = gtk_label_new("Mouse sensitivity");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 40.0, 1.0);
    gtk_range_set_value(GTK_RANGE(scale), (gdouble)current);
    gtk_widget_set_hexpand(scale, TRUE);
    gtk_grid_attach(GTK_GRID(grid), scale, 1, 0, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create mouse settings widget
 *
 * \param[in]   parent  parent widget
 *
 * \return  GtkGrid
 */
GtkWidget *uimousesettings_widget_create(GtkWidget *parent)
{
    GtkWidget *layout;
    GtkWidget *mouse_grab;
    GtkWidget *mouse_save;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin", 16, NULL);

    mouse_grab = resource_check_button_create("Mouse", "Enable mouse grab");
    mouse_save = resource_check_button_create("SmartMouseRTCSave",
            "Enable SmartMouse RTC Saving");

    gtk_grid_attach(GTK_GRID(layout), mouse_grab, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), mouse_save, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(layout), create_sensitivity_widget(), 0, 1, 2, 1);

    gtk_widget_show_all(layout);
    return layout;
}
