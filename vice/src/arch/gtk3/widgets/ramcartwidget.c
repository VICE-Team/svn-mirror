/** \file   src/arch/gtk3/widgets/ramcartwidget.c
 * \brief   Widget to control RamCart resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  RAMCART
 *  RAMCARTsize
 *  RAMCARTfilename
 *  RAMCARTImageWrite
 *  RAMCART_RO
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

#include "ramcartwidget.h"


/** \brief  List of supported RAM sizes
 */
static ui_radiogroup_entry_t ram_sizes[] = {
    { "128KB", 128 },
    { "256KB", 256 },
    { NULL, -1 }
};


/* list of widgets, used to enable/disable depending on RAMCART resource */
static GtkWidget *ramcart_enable_widget = NULL;
static GtkWidget *ramcart_size = NULL;
static GtkWidget *ramcart_readonly = NULL;
static GtkWidget *ramcart_image = NULL;

static int (*ramcart_save_func)(int, const char *) = NULL;
static int (*ramcart_flush_func)(int) = NULL;


/** \brief  Handler for the "toggled" event of the ramcart_enable widget
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   unused
 */
static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(ramcart_size, state);
    gtk_widget_set_sensitive(ramcart_image, state);
}


/** \brief  Create RAMCART enable check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_ramcart_enable_widget(void)
{
    return resource_check_button_create("RAMCART", "Enable RAMCART expansion");
}


static GtkWidget *create_ramcart_readonly_widget(void)
{
    return resource_check_button_create("RAMCART_RO",
            "RAMCART contents are read only");
}


/** \brief  Create radio button group to determine GEORAM RAM size
 *
 * \return  GtkGrid
 */
static GtkWidget *create_ramcart_size_widget(void)
{
    GtkWidget *grid;
    GtkWidget *radio_group;

    grid = uihelpers_create_grid_with_label("RAM Size", 1);
    radio_group = resource_radiogroup_create("RAMCARTsize", ram_sizes,
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
static GtkWidget *create_ramcart_image_widget(GtkWidget *parent)
{
    return cart_image_widget_create(parent,
            "RAMCARTfilename", "RAMCARTImageWrite",
            ramcart_save_func, ramcart_flush_func,
            CARTRIDGE_NAME_RAMCART, CARTRIDGE_RAMCART);

}


/** \brief  Create widget to control RAM Expansion Module resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *ramcart_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    ramcart_enable_widget = create_ramcart_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_enable_widget, 0, 0, 1, 1);

    ramcart_size = create_ramcart_size_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_size, 0, 1, 1, 1);

    ramcart_image = create_ramcart_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), ramcart_image, 1, 1, 1, 1);

    g_signal_connect(ramcart_enable_widget, "toggled", G_CALLBACK(on_enable_toggled),
            NULL);

    ramcart_readonly = create_ramcart_readonly_widget();
    gtk_grid_attach(GTK_GRID(grid), ramcart_readonly, 0, 2, 2,1);

    /* enable/disable widget based on ramcart-enable (dirty trick, I know) */
    on_enable_toggled(ramcart_enable_widget, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set save function for the RAMCART extension
 *
 * \param[in]   func    save function
 */
void ramcart_widget_set_save_handler(int (*func)(int, const char *))
{
    ramcart_save_func = func;
}


/** \brief  Set flush function for the RAMCART extension
 *
 * \param[in]   func    save function
 */
void ramcart_widget_set_flush_handler(int (*func)(int))
{
    ramcart_flush_func = func;
}
