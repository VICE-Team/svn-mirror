/** \file   settings_host_display.c
 * \brief   Widget to control resources related to the host display
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES FullscreenDecorations   -vsid
 * $VICERES StartMinimized          -vsid
 *
 * $VICERES VSync                   -vsid
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
#include "videoarch.h"
#include "canvasrenderfilterwidget.h"
#include "settings_misc.h"

#include "settings_host_display.h"


/** \brief  Create "Enable fulllscreen decorations" widget
 *
 * Currently completely ignores C128 VDC/VCII, maybe once we can differenciate
 * between displays (ie two or more) and put the GtkWindows there, we can
 * try to handle these scenarios.
 *
 * \param[in]   index   window index (unused)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_fullscreen_decorations_widget(int index)
{
    return vice_gtk3_resource_check_button_new(
            "FullscreenDecorations",
            "Fullscreen decorations (Show menu and statusbar in fullscreen mode)");
}


/** \brief  Create synchronization method widget
 *
 * \return  GtkGrid
 */
static GtkWidget *create_sync_widget(void)
{
    GtkWidget *grid;
    GtkWidget *vsync;
    GtkWidget *header;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, 0);

    header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<b>Gtk sync method</b>");
    gtk_widget_set_halign(header, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), header, 0, 0, 1, 1);

    vsync = vice_gtk3_resource_check_button_new("VSync", "VSync");
    gtk_widget_set_sensitive(vsync, TRUE);
    gtk_grid_attach(GTK_GRID(grid), vsync, 0, 2, 1, 1);
    g_object_set(vsync, "margin-left", 16, "margin-top", 8, NULL);

    return grid;
}


/** \brief  Create host display settings widget
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_host_display_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;
    GtkWidget *filter_widget = canvas_render_filter_widget_create();
    GtkWidget *sync_widget;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    if (machine_class != VICE_MACHINE_VSID) {

        GtkWidget *decorations;
        GtkWidget *minimized_widget;
        int col = 0;

        decorations = create_fullscreen_decorations_widget(0);

        minimized_widget = vice_gtk3_resource_check_button_new(
                "StartMinimized",
                "Start the emulator window minimized");

        sync_widget = create_sync_widget();

        gtk_grid_attach(GTK_GRID(grid), filter_widget, col++, 1, 2, 1);
        g_object_set(filter_widget, "margin-left",8, NULL);

        gtk_grid_attach(GTK_GRID(grid), sync_widget, col++, 1, 2, 1);

        gtk_grid_attach(GTK_GRID(grid), decorations, 0, 2, 2, 1);
        gtk_grid_attach(GTK_GRID(grid), minimized_widget, 0, 3, 2, 1);
    }
    gtk_widget_show_all(grid);
    return grid;
}
