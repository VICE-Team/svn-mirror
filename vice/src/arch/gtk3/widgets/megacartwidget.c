/** \file   megacartwidget.c
 * \brief   VIC-20 Mega Cart widget
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

#include "basedialogs.h"
#include "basewidgets.h"
#include "debug_gtk3.h"
#include "machine.h"
#include "openfiledialog.h"
#include "resources.h"
#include "widgethelpers.h"
#include "ui.h"

#include "megacartwidget.h"


/** \brief  Create Mega Cart settings widget
 *
 * Allows selecting an nvram image and enabling/disabling write-back.
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *mega_cart_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *write_back;
    GtkWidget *browser;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    browser = vice_gtk3_resource_browser_new(
            "MegaCartNvRAMfilename",
            NULL,
            NULL,
            "Select NvRAM image file",
            "NvRAM image file",
            NULL);
    gtk_grid_attach(GTK_GRID(grid), browser, 0, 0, 1, 1);

    write_back = vice_gtk3_resource_check_button_new(
            "MegaCartNvRAMWriteBack",
            "Enable Mega Cart NvRAM write back");
    gtk_grid_attach(GTK_GRID(grid), write_back, 0, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
