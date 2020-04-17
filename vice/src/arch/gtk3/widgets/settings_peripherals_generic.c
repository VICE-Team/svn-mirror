/** \file   settings_peripherals_generic.c
 * \brief   Widget to control generic peripheral settings
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES VirtualDevices          -vsid
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

#include "vice_gtk3.h"
#include "resources.h"
#include "machine.h"

#include "settings_peripherals_generic.h"


/** \brief  Create generic peripheral settings widget
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_peripherals_generic_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;
    GtkWidget *vdev_widget = NULL;

    grid = gtk_grid_new();

    if (machine_class != VICE_MACHINE_VSID) {
        vdev_widget = vice_gtk3_resource_check_button_new(
                "VirtualDevices",
                "Enable virtual devices");
        g_object_set(vdev_widget, "margin-left",8, NULL);
        gtk_grid_attach(GTK_GRID(grid), vdev_widget, 0, 1, 2, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}
