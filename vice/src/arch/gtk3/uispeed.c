/*
 * uispeed.c - GTK3 speed control central widget for the settings dialog
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
 */


#include "vice.h"

#include <gtk/gtk.h>

#include "lib.h"
#include "resources.h"
#include "vsync.h"
#include "uihelpers.h"
#include "refreshratewidget.h"
#include "speedwidget.h"

#include "uispeed.h"



static void warp_callback(GtkWidget *widget, gpointer data)
{
    int warp = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    resources_set_int("WarpMode", warp);
}


static GtkWidget *create_warp_checkbox(void)
{
    GtkWidget *check;

    check = gtk_check_button_new_with_label("Warp mode");
    g_signal_connect(check, "toggled", G_CALLBACK(warp_callback), NULL);
    gtk_widget_show(check);
    return check;
}



GtkWidget *uispeed_create_central_widget(void)
{
    GtkWidget *layout;

    layout = gtk_grid_new();

    gtk_grid_attach(GTK_GRID(layout), create_refreshrate_widget(), 0, 0, 1, 2);
    gtk_grid_attach(GTK_GRID(layout), create_speed_widget(), 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(layout), create_warp_checkbox(), 1, 1, 1, 1);

    gtk_widget_show(layout);
    return layout;
}
