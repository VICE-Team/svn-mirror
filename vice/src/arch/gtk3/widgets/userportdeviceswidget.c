/** \file   userportdeviceswidget.c
 * \brief   Widget to select userport devices
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
 * $VICERES UserportDevice          x64 x64sc xscpu64 x128 xcbm2 xvic xpet
 * $VICERES Userport58321aSave      x64 x64sc xscpu64 x128 xcbm2 xvic xpet
 * $VICERES UserportDS1307Save      x64 x64sc xscpu64 x128 xcbm2 xvic xpet
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

#include "debug_gtk3.h"
#include "lib.h"
#include "resources.h"
#include "userport.h"
#include "vice_gtk3.h"

#include "userportdeviceswidget.h"


/** \brief  Column indexes in the useport devices model
 */
enum {
    COL_DEVICE_ID,      /**< device ID (int) */
    COL_DEVICE_NAME,    /**< device name (str) */
    COL_DEVICE_TYPE     /**< device type (int) */
};


/*
 * Used for the event handlers
 */

/** \brief  58321a save enable toggle button */
static GtkWidget *rtc_58321a_save = NULL;

/** \brief  ds1307 save enable toggle button */
static GtkWidget *rtc_ds1307_save = NULL;



/** \brief  Create widget for the "UserportRTC58321aSave" resource
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_rtc_58321a_save_widget(void)
{
    return vice_gtk3_resource_check_button_new(
            "UserportRTC58321aSave", "Enable RTC (58321a) saving");
}


/** \brief  Create widget for the "UserportRTCDS1307Save" resource
 *
 * \return  GtkCheckButton
 */
static GtkWidget *create_rtc_ds1307_save_widget(void)
{
    return vice_gtk3_resource_check_button_new(
            "UserportRTCDS1307Save", "Enable RTC (DS1307) saving");
}


/** \brief  Set the RTC checkboxes' sensitivity based on device ID
 *
 * Use userport device \a id to determine which RTC checkboxes to 'grey-out'.
 *
 * \param[in]   id  userport device ID
 */
static void set_rtc_widgets_sensitivity(int id)
{
    gtk_widget_set_sensitive(rtc_58321a_save, id == USERPORT_DEVICE_RTC_58321A);
    gtk_widget_set_sensitive(rtc_ds1307_save, id == USERPORT_DEVICE_RTC_DS1307);
}


/** \brief  Handler for the 'changed' event of the device combobox
 *
 * Sets the active userport device via the "UserportDevice" resource.
 *
 * \param[in]   combo       device combo box
 * \param[in]   user_data   extra event data (unused)
 */
static void on_device_changed(GtkComboBox *combo, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;

    model = gtk_combo_box_get_model(combo);
    if (gtk_combo_box_get_active_iter(combo, &iter)) {
        gint id;
        gchar *name;

        gtk_tree_model_get(model,
                           &iter,
                           COL_DEVICE_ID, &id,
                           COL_DEVICE_NAME, &name,
                           -1);
        debug_gtk3("Got device #%d (%s)", id, name);
        resources_set_int("UserportDevice", id);
        set_rtc_widgets_sensitivity(id);
        g_free(name);
    }
}


/** \brief  Set userport device ID
 *
 * Sets the currently selected combobox item via device ID.
 *
 * To avoid updating the related resource via the combobox' event handler, use
 * the \a blocked argument.
 *
 * \param[in]   combo   device combo box
 * \param[in]   id      device ID
 * \param[in]   blocked block 'changed' signal handler
 */
static gboolean set_device_id(GtkComboBox *combo, gint id, gboolean blocked)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    gulong handler_id;
    gboolean result = FALSE;

    /* do we need to block the 'changed' event handler? */
    if (blocked) {
        /* look up handler ID by callback */
        handler_id = g_signal_handler_find(combo,
                                           G_SIGNAL_MATCH_FUNC,
                                           0,       /* signal_id */
                                           0,       /* detail */
                                           NULL,    /* closure */
                                           on_device_changed,   /* func */
                                           NULL);
        if (handler_id > 0) {
            g_signal_handler_block(combo, handler_id);
        }
    }

    /* iterate the model until we find the device ID */
    model = gtk_combo_box_get_model(combo);
    if (gtk_tree_model_get_iter_first(model, &iter)) {
        do {
            gint current;

            gtk_tree_model_get(model, &iter, COL_DEVICE_ID, &current, -1);
            if (id == current) {
                gtk_combo_box_set_active_iter(combo, &iter);
                result = TRUE;
                break;
            }
        } while (gtk_tree_model_iter_next(model, &iter));
    }

    /* set RTC checkboxes "greyed-out" state */
    set_rtc_widgets_sensitivity(id);

    /* unblock signal, if blocked */
    if (blocked) {
        g_signal_handler_unblock(combo, handler_id);
    }

    return result;
}


/** \brief  Create combobox for the userport devices
 *
 * Create a combobox with valid userport devices for current machine.
 *
 * The model of the combobox contains device ID, name and type, of which name
 * is shown and ID is used to set the related resource.
 *
 * \return  GtkComboBox
 *
 * \todo    Try using the device type to create little headers in the combobox,
 *          grouping the devices by type. Might be overkill for some machines
 *          that only have a few userport devices, we'll see.
 */
static GtkWidget *create_device_combobox(void)
{
    GtkWidget *combo;
    GtkListStore *model;
    GtkTreeIter iter;
    GtkCellRenderer *name_renderer;
    userport_desc_t *devices;
    userport_desc_t *dev;

    /* create model for the combobox */
    model = gtk_list_store_new(3, G_TYPE_INT, G_TYPE_STRING, G_TYPE_INT);
    devices = userport_get_valid_devices(TRUE);
    for (dev = devices; dev->name != NULL; dev++) {
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model,
                           &iter,
                           COL_DEVICE_ID, dev->id,
                           COL_DEVICE_NAME, dev->name,
                           COL_DEVICE_TYPE, dev->device_type,
                           -1);
    }
    lib_free(devices);

    /* create combobox with a single cell renderer for the device name column */
    combo = gtk_combo_box_new_with_model(GTK_TREE_MODEL(model));
    name_renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo),
                               name_renderer,
                               TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo),
                                   name_renderer,
                                   "text", COL_DEVICE_NAME,
                                   NULL);
    g_signal_connect(combo, "changed", G_CALLBACK(on_device_changed), NULL);

    return combo;
}


/** \brief  Create widget to select userport devices
 *
 * \param[in]   parent  parent widget (unused)
 *
 * \return  GtkGrid
 */
GtkWidget *userport_devices_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *combo;
    int device_id;

    grid = vice_gtk3_grid_new_spaced(VICE_GTK3_DEFAULT, VICE_GTK3_DEFAULT);

    /* combobox with the userport devices */
    label = gtk_label_new("Userport device");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    combo = create_device_combobox();
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), combo, 1, 0, 1, 1);

    /* RTC 58321A save checkbox */
    rtc_58321a_save = create_rtc_58321a_save_widget();
    gtk_grid_attach(GTK_GRID(grid), rtc_58321a_save, 0, 1, 2, 1);

    /* RTC DS1307 save checkbox */
    rtc_ds1307_save = create_rtc_ds1307_save_widget();
    gtk_grid_attach(GTK_GRID(grid), rtc_ds1307_save, 0, 2, 2, 1);

    /* set the active item using the resource */
    if (resources_get_int("UserportDevice", &device_id) == 0) {
        set_device_id(GTK_COMBO_BOX(combo), device_id, TRUE);
    }

    gtk_widget_show_all(grid);
    return grid;
}
