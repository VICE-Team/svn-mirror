/** \file   settings_megacart.c
 * \brief   Settings widget controlling VIC-20 Mega Cart resources
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES MegaCartNvRAMfilename   xvic
 * $VICERES MegaCartNvRAMWriteBack  xvic
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

#include "cartridge.h"
#include "vice_gtk3.h"

#include "settings_megacart.h"


/** \brief  Create Mega Cart settings widget
 *
 * Allows selecting an nvram image and enabling/disabling write-back.
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_megacart_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *write_back;
    GtkWidget *label;
    GtkWidget *chooser;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    label = gtk_label_new("NvRAM image file");
    gtk_widget_set_halign(label, GTK_ALIGN_START);

    chooser = vice_gtk3_resource_filechooser_new("MegaCartNvRAMfilename",
                                                GTK_FILE_CHOOSER_ACTION_SAVE);
    vice_gtk3_resource_filechooser_set_custom_title(chooser,
                                                    "Select " CARTRIDGE_VIC20_NAME_MEGACART
                                                    " NvRAM image file");
    gtk_grid_attach(GTK_GRID(grid), label,   0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), chooser, 1, 0, 1, 1);

    write_back = vice_gtk3_resource_check_button_new("MegaCartNvRAMWriteBack",
                                                     "Enable "
                                                     CARTRIDGE_VIC20_NAME_MEGACART
                                                     " NvRAM write back");
    gtk_grid_attach(GTK_GRID(grid), write_back, 0, 1, 2, 1);

    gtk_widget_show_all(grid);
    return grid;
}
