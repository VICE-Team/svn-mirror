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
    GtkWidget *flip_x;
    GtkWidget *flip_y;
    GtkWidget *rotate;

    grid = vice_gtk3_grid_new_spaced_with_label(8, 0, "Mirror/Rotate", 1);
    vice_gtk3_grid_set_title_margin(grid, 8);
    flip_x = vice_gtk3_resource_check_button_new_sprintf("%sFlipX",
                                                         "Flip X",
                                                         chip);
    gtk_widget_set_margin_start(flip_x, 8);

    flip_y = vice_gtk3_resource_check_button_new_sprintf("%sFlipY",
                                                         "Flip Y",
                                                         chip);
    gtk_widget_set_margin_start(flip_y, 8);

    rotate = vice_gtk3_resource_check_button_new_sprintf("%sRotate",
                                                         "Rotate 90\u00b0",
                                                         chip);
    gtk_widget_set_margin_start(rotate, 8);

    gtk_grid_attach(GTK_GRID(grid), flip_x, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), flip_y, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), rotate, 0, 3, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}
