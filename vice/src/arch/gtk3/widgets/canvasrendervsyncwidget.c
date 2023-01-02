/** \file   canvasrendervsyncwidget.c
 * \brief   Widget to select how the GL output is synced
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  groepaz <groepaz@gmx.net>
 */

/*
 * $VICERES CrtcVSync      xpet xcbm2
 * $VICERES TEDVSync       xplus4
 * $VICERES VDCVSync       x128
 * $VICERES VICVSync       xvic
 * $VICERES VICIIVSync     x64 x64sc xscpu64 xdtv64 x128 xcbm5x0
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
#include "video.h"

#include "canvasrendervsyncwidget.h"


/** \brief  Create widget to select the Gtk vsync method
 *
 * \param[in]   chip    video chip prefix
 *
 * \return  GtkGrid
 */
GtkWidget *canvas_render_vsync_widget_create(const char *chip)
{
    GtkWidget *grid;
    GtkWidget *vsync_widget;

    grid = vice_gtk3_grid_new_spaced_with_label(
            VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT, "Sync", 1);
    vsync_widget = vice_gtk3_resource_check_button_new_sprintf(
            "%sVSync", "VSync", chip);
    gtk_widget_set_margin_start(vsync_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), vsync_widget, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
