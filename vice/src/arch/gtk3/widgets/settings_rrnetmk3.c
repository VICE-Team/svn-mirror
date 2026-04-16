/** \file   settings_rrnetmk3.c
 * \brief   Settings widget to control RRNet MK3 resourcs
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES RRNETMK3_flashjumper    x64 x64sc xscpu64 x128
 * $VICERES RRNETMK3_bios_write     x64 x64sc xscpu64 x128
 */


/*
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

#include "cartridge.h"
#include "vice_gtk3.h"

#include "settings_rrnetmk3.h"

/** \brief  Create widget to control RRNet Mk3 resources
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_rrnetmk3_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *flash_label;
    GtkWidget *flash_jumper;
    GtkWidget *primary;
    int row = 0;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);

    flash_label  = gtk_label_new(CARTRIDGE_NAME_RRNETMK3 " Flash jumper");
    flash_jumper = vice_gtk3_resource_switch_new("RRNETMK3_flashjumper");
    gtk_widget_set_halign(flash_label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(flash_label, FALSE);
    gtk_widget_set_halign(flash_jumper, GTK_ALIGN_START);
    gtk_widget_set_valign(flash_jumper, GTK_ALIGN_CENTER);

    gtk_grid_attach(GTK_GRID(grid), flash_label,  0, row, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), flash_jumper, 1, row, 1, 1);
    row++;

    primary = cart_image_widget_new(CARTRIDGE_RRNETMK3,        /* cart id */
                                    CARTRIDGE_NAME_RRNETMK3,   /* cart name */
                                    CART_IMAGE_PRIMARY,     /* image number */
                                    "cartridge",            /* image tag */
                                    NULL,                   /* resource name */
                                    TRUE,                   /* flush button */
                                    TRUE                    /* save button */
                                    );
    cart_image_widget_append_check(primary,
                                   "RRNETMK3_bios_write",
                                   "Save image when changed");


    gtk_grid_attach(GTK_GRID(grid), primary,   0, row, 4, 1);
    row++;

    gtk_widget_show_all(grid);
    return grid;
}
