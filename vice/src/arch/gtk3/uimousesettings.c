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
    GtkWidget *todo;

    layout = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(layout), 8);
    gtk_grid_set_row_spacing(GTK_GRID(layout), 8);
    g_object_set(layout, "margin", 16, NULL);

    mouse_grab = resource_check_button_create("Mouse", "Enable mouse grab");
    mouse_save = resource_check_button_create("SmartMouseRTCSave",
            "Enable SmartMouse RTC Saving");

    gtk_grid_attach(GTK_GRID(layout), mouse_grab, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), mouse_save, 1, 0, 1, 1);

    /* add a TODO for the Windows port */
    todo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(todo),
            "<b>TODO:</b> Windows has a <i>MouseSensitivity</i> setting "
            "(x64/x64sc/xscpu64/x64dtv/x128/xvic)");
    gtk_label_set_line_wrap(GTK_LABEL(todo), TRUE);
    gtk_grid_attach(GTK_GRID(layout), todo, 0, 1, 2, 1);

    gtk_widget_show_all(layout);
    return layout;
}
