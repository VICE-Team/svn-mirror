/** \file   settings_misc.c
 * \brief   Widget to control resources that are hard to place properly
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES VirtualDevices      -vsid
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

#include "canvasrenderbackendwidget.h"
#include "canvasrenderfilterwidget.h"
#include "cwdwidget.h"
#include "jamactionwidget.h"

#include "settings_misc.h"


static void on_fuck_apple(GtkWidget *widget, gpointer data)
{
    debug_gtk3("got triggered!");
    vice_gtk3_message_info("You're a fucking idiot",
            "Since you actually clicked on this, "
            "you're either a complete idiot or you have a sense of humor");
    abort();

}

/** \brief  Create miscellaneous settings widget
 *
 * Basically a widget to contain (hopefully temporarily) widgets controlling
 * resources that can't (yet) be placed in a more logical location
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_misc_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;
    GtkWidget *cwd_widget = NULL;
    GtkWidget *vdev_widget = NULL;
    GtkWidget *jam_widget = jam_action_widget_create();
    GtkWidget *backend_widget = canvas_render_backend_widget_create();
    GtkWidget *filter_widget = canvas_render_filter_widget_create();
    GtkWidget *apple;

    grid = gtk_grid_new();

    if (machine_class != VICE_MACHINE_VSID) {
        vdev_widget = vice_gtk3_resource_check_button_new(
                "VirtualDevices",
                "Enable virtual devices");
        cwd_widget = cwd_widget_create();

        gtk_grid_attach(GTK_GRID(grid), cwd_widget, 0, 1, 2, 1);
        g_object_set(vdev_widget, "margin-left",8, NULL);
        gtk_grid_attach(GTK_GRID(grid), vdev_widget, 0, 2, 2, 1);
        gtk_grid_attach(GTK_GRID(grid), jam_widget, 0, 3, 2, 1);
        gtk_grid_attach(GTK_GRID(grid), filter_widget, 0, 4, 2, 1);
        g_object_set(filter_widget, "margin-left",8, NULL);
        gtk_grid_attach(GTK_GRID(grid), backend_widget, 1, 4, 2, 1);
        g_object_set(backend_widget, "margin-left",8, NULL);

        apple = gtk_check_button_new_with_label("Enable Apple monitor stand emulation");
        g_object_set(apple, "margin-top", 16, NULL);
        gtk_grid_attach(GTK_GRID(grid), apple, 0, 5, 1, 1);
        g_signal_connect(apple, "toggled", G_CALLBACK(on_fuck_apple), NULL);

    } else {
         gtk_grid_attach(GTK_GRID(grid), jam_widget, 0, 0, 1, 1);
    }

    gtk_widget_show_all(grid);
    return grid;
}
