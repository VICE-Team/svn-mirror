/*
 * ui.c - Gnome based graphical user interface.  
 *
 * Written by
 *  Martin Pottendorfer (Martin.Pottendorfer@alcatel.at
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

#include <gnome.h>

#include "vice.h"
#include "ui.h"
#include "resources.h"

/* Globals */
GtkWidget *vice_app;

/* Initialize the GUI and parse the command line. */
int ui_init(int *argc, char **argv)
{
    gnome_init(PACKAGE, VERSION, *argc, argv);
    gdk_rgb_init();

    /* Check this rgb stuff again, FIXME MP */
    gtk_widget_push_visual(gdk_rgb_get_visual());
    gtk_widget_push_colormap(gdk_rgb_get_cmap());
    vice_app = gnome_app_new(PACKAGE, PACKAGE);
    gtk_widget_pop_visual();
    gtk_widget_pop_colormap();

    return 0;
}

/* Dispatch all the pending Gtk events. */
void ui_dispatch_events(void)
{
    while (gtk_events_pending())
	gtk_main_iteration();
}
