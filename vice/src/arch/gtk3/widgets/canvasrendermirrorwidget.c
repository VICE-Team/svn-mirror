/** \file   canvasrendermirrorwidget.c
 * \brief   Widget to select how the GL output is mirrored / rotated
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  groepaz <groepaz@gmx.net>
 */

/*
 * $VICERES CrtcFlipX      xpet xcbm2
 * $VICERES TEDFlipX       xplus4
 * $VICERES VDCFlipX       x128
 * $VICERES VICFlipX       xvic
 * $VICERES VICIIFlipX     x64 x64sc xscpu64 xdtv64 x128 xcbm5x0
 *
 * $VICERES CrtcFlipY      xpet xcbm2
 * $VICERES TEDFlipY       xplus4
 * $VICERES VDCFlipY       x128
 * $VICERES VICFlipY       xvic
 * $VICERES VICIIFlipY     x64 x64sc xscpu64 xdtv64 x128 xcbm5x0
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

#include "canvasrendermirrorwidget.h"


/** \brief  Create widget to select the Gtk render filter method
 *
 * \param[in]   chip    video chip prefix
 *
 * \return  GtkGrid
 */
GtkWidget *canvas_render_mirror_widget_create(const char *chip)
{
    GtkWidget *grid;
    GtkWidget *flipx_widget;
    GtkWidget *flipy_widget;
    GtkWidget *rot_widget;

    grid = vice_gtk3_grid_new_spaced_with_label(
            VICE_GTK3_DEFAULT, 0, "Mirror/Rotate", 1);
    flipx_widget = vice_gtk3_resource_check_button_new_sprintf(
            "%sFlipX", "Mirror X", chip);
    gtk_widget_set_margin_start(flipx_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), flipx_widget, 0, 1, 1, 1);

    flipy_widget = vice_gtk3_resource_check_button_new_sprintf(
            "%sFlipY", "Mirror Y", chip);
    gtk_widget_set_margin_start(flipy_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), flipy_widget, 0, 2, 1, 1);

    rot_widget = vice_gtk3_resource_check_button_new_sprintf(
            "%sRotate", "Rotate 90\u00b0", chip);
    gtk_widget_set_margin_start(rot_widget, 16);
    gtk_grid_attach(GTK_GRID(grid), rot_widget, 0, 3, 1, 1);


    gtk_widget_show_all(grid);
    return grid;
}
