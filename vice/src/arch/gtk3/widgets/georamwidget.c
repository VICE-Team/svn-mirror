/** \file   georamwidget.c
 * \brief   Widget to control GEO-RAM resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES GEORAM              x64 x64sc xscpu64 x128
 * $VICERES GEORAMsize          x64 x64sc xscpu64 x128
 * $VICERES GEORAMfilename      x64 x64sc xscpu64 x128
 * $VICERES GEORAMImageWrite    x64 x64sc xscpu64 x128
 * $VICERES GEORAMIOSwap        xvic
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
#include "machine.h"
#include "resources.h"
#include "cartridge.h"
#include "uisettings.h"

#include "georamwidget.h"


/** \brief  List of supported RAM sizes
 */
static const vice_gtk3_radiogroup_entry_t ram_sizes[] = {
    { "512KiB",     512 },
    { "1MiB",       1024 },
    { "2MiB",       2048 },
    { "4MiB",       4096 },
    { NULL,         -1 }
};



/** \brief  Create IO-swap check button (seems to be valid for xvic only)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_georam_ioswap_widget(void)
{
    GtkWidget *check;

    check = vice_gtk3_resource_check_button_new("GEORAMIOSwap",
            "MasC=uarade I/O swap");
    return check;
}


/** \brief  Create radio button group to determine GEORAM RAM size
 *
 * \return  GtkGrid
 */
static GtkWidget *create_georam_size_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = vice_gtk3_grid_new_spaced_with_label(-1, -1, "RAM size", 1);
    radio_group = vice_gtk3_resource_radiogroup_new("GEORAMsize", ram_sizes,
            GTK_ORIENTATION_VERTICAL);
    g_object_set(radio_group, "margin-left", 16, NULL);
    gtk_grid_attach(GTK_GRID(grid), radio_group, 0, 1, 1, 1);
    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Create widget to load/save GEORAM image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_georam_image_widget(void)
{
    return cart_image_widget_create(
            NULL, "GEORAM Image",
            "GEORAMfilename", "GEORAMImageWrite",
            carthelpers_save_func, carthelpers_flush_func,
            carthelpers_can_save_func, carthelpers_can_flush_func,
            CARTRIDGE_NAME_GEORAM, CARTRIDGE_GEORAM);
}


/** \brief  Create widget to control GEORAM resources
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *georam_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *georam_enable;
    GtkWidget *georam_size;
    GtkWidget *georam_ioswap;
    GtkWidget *georam_image;

    grid = vice_gtk3_grid_new_spaced(8, 8);

    georam_enable = carthelpers_create_enable_check_button(
            CARTRIDGE_NAME_GEORAM, CARTRIDGE_GEORAM);
    gtk_grid_attach(GTK_GRID(grid), georam_enable, 0, 0, 2, 1);

    if (machine_class == VICE_MACHINE_VIC20) {
        georam_ioswap = create_georam_ioswap_widget();
        gtk_grid_attach(GTK_GRID(grid), georam_ioswap, 0, 2, 1, 1);
    }

    georam_size = create_georam_size_widget();
    gtk_grid_attach(GTK_GRID(grid), georam_size, 0, 1, 1, 1);

    georam_image = create_georam_image_widget();
    gtk_grid_attach(GTK_GRID(grid), georam_image, 1, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
