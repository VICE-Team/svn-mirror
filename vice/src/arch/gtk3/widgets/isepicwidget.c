/** \file   src/arch/gtk3/widgets/isepicwidget.c
 * \brief   Widget to control ISEPIC resources
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Controls the following resource(s):
 *  IsepicCartridgeEnabled
 *  Isepicfilename
 *  IsepicSwitch
 *  IsepicImageWrite
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
#include "cartimagewidget.h"
#include "cartridge.h"

#include "isepicwidget.h"

/* list of widgets, used to enable/disable depending on ISEPIC resource */
static GtkWidget *isepic_enable_widget = NULL;
static GtkWidget *isepic_image = NULL;
static GtkWidget *isepic_switch = NULL;

static int (*isepic_save_func)(int, const char *) = NULL;
static int (*isepic_flush_func)(int) = NULL;


/** \brief  Handler for the "toggled" event of the isepic_enable widget
 *
 * \param[in]   widget      check button
 * \param[in]   user_data   unused
 */
static void on_enable_toggled(GtkWidget *widget, gpointer user_data)
{
    gboolean state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));

    gtk_widget_set_sensitive(isepic_image, state);
    gtk_widget_set_sensitive(isepic_switch, state);
}


/** \brief  Create ISEPIC enable check button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_isepic_enable_widget(void)
{
    GtkWidget *check;

    check = resource_check_button_create("IsepicCartridgeEnabled",
            "Enable ISEPIC");
    return check;
}


/** \brief  Handler for the "state-set" event of the "IsepicSwitch" resource
 *
 * \param[in]   widget      switch widget
 * \param[in]   state       new state of \a widget
 * \param[in]   user_data   unused
 *
 * \return  FALSE
 */
static gboolean on_isepic_switch_state_set(GtkWidget *widget, gboolean state,
        gpointer user_data)
{
    debug_gtk3("setting IsepicSwitch to %s\n", state ? "ON" : "OFF");
    resources_set_int("IsepicSwitch", state);
    return FALSE;
}


/** \brief  Create ISEPIC switch button
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_isepic_switch_widget(void)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *button;
    int state;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    g_object_set(grid, "margin-left", 16, NULL);

    label = gtk_label_new("Isepic switch");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);

    button = gtk_switch_new();
    resources_get_int("IsepicSwitch", &state);
    gtk_switch_set_active(GTK_SWITCH(button), state);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 0, 1, 1);
    gtk_widget_show_all(grid);

    g_signal_connect(button, "state-set", G_CALLBACK(on_isepic_switch_state_set),
            NULL);
    return grid;
}


/** \brief  Create widget to load/save ISEPIC image file
 *
 * \return  GtkGrid
 */
static GtkWidget *create_isepic_image_widget(GtkWidget *parent)
{
    return cart_image_widget_create(parent,
            "Isepicfilename", "IsepicImageWrite",
            isepic_save_func, isepic_flush_func,
            CARTRIDGE_NAME_ISEPIC, CARTRIDGE_ISEPIC);
}


/** \brief  Create widget to control ISEPIC resources
 *
 * \param[in]   parent  parent widget, used for dialogs
 *
 * \return  GtkGrid
 */
GtkWidget *isepic_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;

    grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 8);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 8);

    isepic_enable_widget = create_isepic_enable_widget();
    gtk_grid_attach(GTK_GRID(grid), isepic_enable_widget, 0, 0, 1, 1);

    isepic_switch = create_isepic_switch_widget();
    gtk_grid_attach(GTK_GRID(grid), isepic_switch, 0, 1, 1, 1);

    isepic_image = create_isepic_image_widget(parent);
    gtk_grid_attach(GTK_GRID(grid), isepic_image, 0, 2, 1, 1);

    g_signal_connect(isepic_enable_widget, "toggled", G_CALLBACK(on_enable_toggled),
            NULL);

    /* enable/disable widget based on isepic-enable (dirty trick, I know) */
    on_enable_toggled(isepic_enable_widget, NULL);

    gtk_widget_show_all(grid);
    return grid;
}


/** \brief  Set save function for the ISEPIC extension
 *
 * \param[in]   func    save function
 */
void isepic_widget_set_save_handler(int (*func)(int, const char *))
{
    isepic_save_func = func;
}


/** \brief  Set flush function for the ISEPIC extension
 *
 * \param[in]   func    flush function
 */
void isepic_widget_set_flush_handler(int (*func)(int))
{
    isepic_flush_func = func;
}
