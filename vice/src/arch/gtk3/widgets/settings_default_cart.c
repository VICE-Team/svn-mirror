/** \file   settings_default_cart.c
 * \brief   GTK3 default cartridge settings
 *
 * Settings UI widget to control default cartridge
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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
#include "debug_gtk3.h"
#include "cartridge.h"
#include "carthelpers.h"
#include "resources.h"
#include "machine.h"
#include "uicart.h"

#include "settings_default_cart.h"



/** \brief  Default cartridge file widget
 *
 * Read-only GtkEntry
 */
static GtkWidget *cart_file_widget = NULL;


/** \brief  Default cartridge type widget
 *
 * Read-only GtkEntry
 */
static GtkWidget *cart_type_widget = NULL;


/** \brief  Button used to set the current cartridge as default
 */
static GtkWidget *set_default_button = NULL;


static GtkWidget *attach_button = NULL;
static GtkWidget *remove_button = NULL;



/** \brief  List of cartridge types/names
 *
 * Set once in the constructor, using cartridge_get_info_list() via a carhelpers
 * function pointer.
 */
static const cartridge_info_t *cart_info_list = NULL;


/** \brief  Get cartridge name by looking up \a id in the cartridge info list
 *
 * \param[in]   id  cartridge ID
 *
 * \return  cartridge name
 */
static const char *get_cart_name_by_id(int id)
{
    int i;

    for (i = 0; cart_info_list[i].name != NULL; i++) {
        if (cart_info_list[i].crtid == id) {
            return cart_info_list[i].name;
        }
    }
    return "<Unknown cartridge type>";
}


/** \brief  Update the file widget by inspecting the CartridgeFile resource
 *
 */
static void update_cart_file_widget(void)
{
    const char *filename;

    if (resources_get_string("CartridgeFile", &filename) >= 0) {
#if 0
        gtk_label_set_text(GTK_LABEL(cart_file_widget), filename);
#else
        gtk_entry_set_text(GTK_ENTRY(cart_file_widget), filename);
#endif
    }
}


/** \brief  Update the type widget by inspecting the CartridgeType resource
 *
 */
static void update_cart_type_widget(void)
{
    if (cart_info_list != NULL) {
        int type;

        if (resources_get_int("CartridgeType", &type) >= 0) {
            const char *name = "CRT";
            if (type != 0) {
                name = get_cart_name_by_id(type);
            }
            gtk_entry_set_text(GTK_ENTRY(cart_type_widget), name);
        }
    }
}


/** \brief  Set button sensitivity based on whether there is a default cart
 */
static void update_buttons(void)
{
    /* Faulty logic: */
#if 0
    const char *filename = NULL;

    resources_get_string("CartridgeFile", &filename);
    if (filename != NULL && *filename != '\0') {
        /* default cart is set */
        gtk_widget_set_sensitive(set_default_button, FALSE);
        gtk_widget_set_sensitive(remove_button, TRUE);
    } else {
        gtk_widget_set_sensitive(set_default_button, TRUE);
        gtk_widget_set_sensitive(remove_button, FALSE);
    }
#endif
}


/** \brief  Callback triggered from the attach dialog
 */
static void attach_callback(void)
{
    debug_gtk3("Called!");

    update_cart_file_widget();
    update_cart_type_widget();
    update_buttons();
}



/** \brief  Handler for the 'clicked' event of the 'set default' button
 *
 * \param[in]   widget  button triggering the event
 * \param[iin]  data    extra event data (unused)
 */
static void on_set_default_clicked(GtkWidget *widget, gpointer data)
{
     if (carthelpers_set_default_func != NULL) {
        debug_gtk3("Setting current cart as default cart.");
        carthelpers_set_default_func();
        update_cart_file_widget();
        update_cart_type_widget();
        update_buttons();
    }
}


#if 0
/** \brief  Handler for the 'clicked' event of the 'unset default' button
 *
 * \param[in]   widget  button triggering the event
 * \param[iin]  data    extra event data (unused)
 */
static void on_unset_default_clicked(GtkWidget *widget, gpointer data)
{
    if (carthelpers_unset_default_func != NULL) {
        debug_gtk3("Unsetting default cart.");
        carthelpers_unset_default_func();
        update_cart_file_widget();
        update_cart_type_widget();
        update_buttons();
    }
}
#endif


/** \brief  Callback for the attach dialog
 *
 * \param[in]   widget  parent widget
 * \param[in]   data    extra even data (unused)
 */
static void on_attach_clicked(GtkWidget *widget, gpointer data)
{
    ui_cart_default_attach(widget, attach_callback);
}



/** \brief  Handler for the 'clicked' event of the 'Remove' button
 *
 * Unsets the current default cart.
 *
 * \param[in]   widget  button triggering the event
 * \param[in]   data    extra event data (unused)
 */
static void on_remove_clicked(GtkWidget *widget, gpointer data)
{
    if (carthelpers_unset_default_func != NULL) {
        carthelpers_unset_default_func();
        update_cart_file_widget();
        update_cart_type_widget();
        update_buttons();
    }
}



static GtkWidget *create_cart_file_widget(void)
{
    GtkWidget *widget;
#if 0
    widget = gtk_label_new(NULL);
#endif
    widget = gtk_entry_new();
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_sensitive(widget, FALSE);
    g_object_set(widget, "editable", FALSE, NULL);
    return widget;
}


static GtkWidget *create_cart_type_widget(void)
{
    GtkWidget *widget;

    widget = gtk_entry_new();
    gtk_widget_set_hexpand(widget, TRUE);
    gtk_widget_set_sensitive(widget, FALSE);
    g_object_set(widget, "editable", FALSE, NULL);
    return widget;
}


/** \brief  Create a default cart settings widget for the settings UI
 *
 * \param[in]   parent  parent widget (ignored)
 *
 * \return  GtkGrid
 */
GtkWidget *settings_default_cart_widget_create(GtkWidget *parent)
{
    GtkWidget *grid;
    GtkWidget *label;

    grid = vice_gtk3_grid_new_spaced_with_label(
            8,
            VICE_GTK3_DEFAULT,
            "Default cartridge",
            4);

    if (cart_info_list == NULL) {
        if (carthelpers_info_list_func != NULL) {
            cart_info_list = carthelpers_info_list_func();
        }
    }

    /* add some spacing between the title and the widgets */
    label = gtk_grid_get_child_at(GTK_GRID(grid), 0, 0);
    g_object_set(label, "margin-bottom", 16, NULL);

    /* resource CartridgeFile */
    label = gtk_label_new("File");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    g_object_set(label,
            "margin-left", 16,
            "margin-right", 16,
            NULL);
#if 0
    cart_file_widget = gtk_entry_new();
    g_object_set(cart_file_widget, "editable", FALSE, NULL);
    gtk_widget_set_hexpand(cart_file_widget, TRUE);
#endif
    cart_file_widget = create_cart_file_widget();
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cart_file_widget, 1, 1, 1, 1);
    update_cart_file_widget();

    attach_button = gtk_button_new_with_label("Attach");
    g_signal_connect(attach_button, "clicked",
            G_CALLBACK(on_attach_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), attach_button, 2, 1, 1, 1);

    remove_button = gtk_button_new_with_label("Remove");
    g_signal_connect(remove_button, "clicked",
            G_CALLBACK(on_remove_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), remove_button, 3, 1, 1, 1);


    /* resource CartridgeType */
    if (cart_info_list != NULL) {
        label = gtk_label_new("Type");
        g_object_set(label,
                "margin-left", 16,
                "margin-right", 16,
                NULL);
        gtk_widget_set_halign(label, GTK_ALIGN_START);
        cart_type_widget = create_cart_type_widget();
        gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), cart_type_widget, 1, 2, 1, 1);
        update_cart_type_widget();
    }


    /* buttons */
    set_default_button = gtk_button_new_with_label("Set cartridge as default");
    gtk_widget_set_hexpand(set_default_button, FALSE);
    gtk_widget_set_halign(set_default_button, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), set_default_button, 0, 3, 2, 1);
    /* make some space */
    g_object_set(set_default_button,
            "margin-left", 16,
            "margin-top", 32,
            NULL);

    /* set sensitivity of buttons (ie grey-out or not) */
    update_buttons();

    /* connect signal handlers */
    g_signal_connect(set_default_button, "clicked",
            G_CALLBACK(on_set_default_clicked), NULL);

    gtk_widget_show_all(grid);
    return grid;
}

