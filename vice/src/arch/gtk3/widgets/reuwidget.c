/** \file   src/arch/gtk3/widgets/reuwidget.c
 * \brief   Widget to control RAM Expansion Module resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  REU
 *  REUsize
 *  REUfilename
 *  REUImageWrite
 *  REUIOSwap (xvic)
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

#include "vice.h"
#include <gtk/gtk.h>

#include "machine.h"
#include "resources.h"
#include "debug_gtk3.h"
#include "basewidgets.h"
#include "widgethelpers.h"
#include "basedialogs.h"
#include "openfiledialog.h"
#include "savefiledialog.h"
#include "cartridge.h"
#include "cartimagewidget.h"
#include "carthelpers.h"

#include "reuwidget.h"


/** \brief  List of supported RAM sizes
 */
static ui_radiogroup_entry_t ram_sizes[] = {
    { "128KB", 128 },
    { "256KB", 256 },
    { "512KB", 512 },
    { "1MB", 1024 },
    { "2MB", 2048 },
    { "4MB", 4096 },
    { "8MB", 8192 },
    { "16MB", 16384 },
    { NULL, -1 }
};


/* list of widgets, used to enable/disable depending on GEORAM resource */

/** \brief  Create REU enable check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_reu_enable_widget(void)
{
    return resource_check_button_create("REU", "Enable RAM Expansion Module");
}


/** \brief  Create IO-swap check button (seems to be valid for xvic only)
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_reu_ioswap_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("REUIOSwap", "MasC=uarade I/O swap");
    return check;
}


/** \brief  Create radio button group to determine GEORAM RAM size
 *
 * \return  GtkGrid
 */
static GtkWidget *create_reu_size_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = uihelpers_create_grid_with_label("RAM Size", 1);
    radio_group = resource_radiogroup_create("REUsize", ram_sizes,
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
static GtkWidget *create_reu_image_widget(GtkWidget *parent)
{
    return cart_image_widget_create(parent, "REU image",
            "REUfilename", "REUImageWrite",
            carthelpers_save_func, carthelpers_flush_func,
            CARTRIDGE_NAME_REU, CARTRIDGE_REU);
}


/** \brief  Create widget to control RAM Expansion Module resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *reu_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *reu_enable;
    GtkWidget *reu_size;
    GtkWidget *reu_ioswap;
    GtkWidget *reu_image;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    reu_enable = create_reu_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), reu_enable, 0, 0, 1, 1);

    if (machine_class == VICE_MACHINE_VIC20) {
        reu_ioswap = create_reu_ioswap_widget();
        gtk_grid_attach(GTK_GRID(grid), reu_ioswap, 0, 1, 1, 1);
    }

    reu_size = create_reu_size_widget();
    gtk_grid_attach(GTK_GRID(grid), reu_size, 0, 1, 1, 1);

    reu_image = create_reu_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), reu_image, 1, 1, 1, 1);

    gtk_widget_show_all(grid);
    return grid;
}
