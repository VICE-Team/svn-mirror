/** \file   settings_host_display.c
 * \brief   Widget to control resources related to the host display
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES FullscreenDecorations   -vsid
 * $VICERES StartMinimized          -vsid
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


/** \brief  Create host display settings widget
 *
 * \param[in]   widget  parent widget (used for dialogs)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_host_display_widget_create(GtkWidget *widget)
{
    GtkWidget *grid;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    if (machine_class != VICE_MACHINE_VSID) {

        GtkWidget *decorations;
        GtkWidget *minimized_widget;

        decorations = create_fullscreen_decorations_widget(0);

        minimized_widget = vice_gtk3_resource_check_button_new(
                "StartMinimized",
                "Start the emulator window minimized");

        gtk_grid_attach(GTK_GRID(grid), decorations, 0, 2, 2, 1);
        gtk_grid_attach(GTK_GRID(grid), minimized_widget, 0, 3, 2, 1);
    }
    gtk_widget_show_all(grid);
    return grid;
}
