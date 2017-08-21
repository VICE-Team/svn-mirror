/*
 * uimenu.c - Native GTK3 menu handling
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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
 *  TODO:   add code to dynamically add submenus and menu items (not all emu
 *          have the same menu)
 *
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "debug_gtk3.h"
#include "not_implemented.h"

#include "cmdline.h"
#include "kbd.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"
#include "uiaccelerators.h"
#include "uiapi.h"
#include "uistatusbar.h"
#include "util.h"

/* XXX: ugly hack to get ui_exit(), causes circular reference of ui.c and
 *      uimenu.c
 */
#include "ui.h"


#include "uimenu.h"



static void ui_quit_callback(GtkWidget *widget, gpointer user_data)
{
    ui_exit();
}


/** \brief  Create the top menu bar with standard submenus
 *
 * \return  GtkMenuBar
 *
*/
GtkWidget *ui_menu_bar_create(void)
{
    GtkWidget *bar;

    GtkWidget *file_submenu;
    GtkWidget *file_item;
    GtkWidget *open_item;
    GtkWidget *quit_item;

    bar = gtk_menu_bar_new();

    file_item = gtk_menu_item_new_with_mnemonic("_File");
    file_submenu = gtk_menu_new();
    open_item = gtk_menu_item_new_with_mnemonic("_Open");
    quit_item = gtk_menu_item_new_with_mnemonic("_Quit");

    gtk_menu_shell_append(GTK_MENU_SHELL(file_submenu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_submenu), quit_item);
    g_signal_connect(quit_item, "activate", G_CALLBACK(ui_quit_callback), NULL);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_submenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_item);

#if 0
    menu = gtk_menu_item_new_with_label("Halp!");
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), menu);

   /* gtk_container_add(GTK_CONTAINER(menu),
            gtk_menu_item_new_with_mnemonic("fuck me!"));
    */
#endif
    return bar;
}

