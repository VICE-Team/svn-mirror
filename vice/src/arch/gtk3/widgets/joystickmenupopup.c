/** \file   joystickmenupopup.c
 *  \brief  Create a menu to swap joysticks
 *
 * This module contains a popup menu that can be triggered via the statusbar's
 * joysticks widget. With it, the user can swap joystick ports.
 *
 *  \author Bas Wassink <b.wassink@ziggo.nl>
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
#include "uicommands.h"

#include "joystickmenupopup.h"


/** \brief  Create joystick menu popup for the statusbar
 *
 * \return  GtkMenu
 *
 * TODO:    "gray out" items when invalid (depends on emulator and settings)
 */
GtkWidget *joystick_menu_popup_create(void)
{
    GtkWidget *menu;
    GtkWidget *item;

    menu = gtk_menu_new();

    item = gtk_menu_item_new_with_label("Swap joysticks");
    gtk_container_add(GTK_CONTAINER(menu), item);
    g_signal_connect(item, "activate",
            G_CALLBACK(ui_swap_joysticks_callback), NULL);

    item = gtk_menu_item_new_with_label("Swap userport joysticks");
    gtk_container_add(GTK_CONTAINER(menu), item);
    g_signal_connect(item, "activate",
            G_CALLBACK(ui_swap_userport_joysticks_callback), NULL);

    gtk_widget_show_all(menu);
    return menu;
}
