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
#include "uiabout.h"
#include "uistatusbar.h"
#include "util.h"

/* XXX: ugly hack to get ui_exit(), causes circular reference of ui.c and
 *      uimenu.c
 */
#include "ui.h"


#include "uimenu.h"


/* todo: add Doxygen docs */

static GtkWidget *main_menu_bar = NULL;
static GtkWidget *file_submenu = NULL;
static GtkWidget *edit_submenu = NULL;
static GtkWidget *snapshot_submenu = NULL;
static GtkWidget *settings_submenu = NULL;
static GtkWidget *help_submenu = NULL;


static void ui_quit_callback(GtkWidget *widget, gpointer user_data)
{
    ui_exit();
}


static void ui_about_callback(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *about;

    about = ui_about_create_dialog();
    gtk_widget_show(GTK_WIDGET(about));
}



/** \brief  Create the top menu bar with standard submenus
 *
 * \return  GtkMenuBar
 *
*/
GtkWidget *ui_menu_bar_create(void)
{
    GtkWidget *bar;

    /* file menu */
    GtkWidget *file_item;
    GtkWidget *open_item;
    GtkWidget *quit_item;

    /* edit menu */
    GtkWidget *edit_item;
    GtkWidget *copy_item;
    GtkWidget *paste_item;

    /* snapshot menu */
    GtkWidget *snap_item;
    GtkWidget *load_item;
    GtkWidget *save_item;

    /* help menu */
    GtkWidget *help_item;
    GtkWidget *about_item;

    bar = gtk_menu_bar_new();

    gtk_widget_set_hexpand(GTK_WIDGET(bar), TRUE);

    /*
     * Obviously the code here can be refactored, there's a lot duplication
     * going on -- compyx
     */

    /* create the top-level 'File' menu */
    file_item = gtk_menu_item_new_with_mnemonic("_File");
    file_submenu = gtk_menu_new();
    open_item = gtk_menu_item_new_with_mnemonic("_Open");
    quit_item = gtk_menu_item_new_with_mnemonic("_Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_submenu), open_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_submenu), quit_item);
    g_signal_connect(quit_item, "activate", G_CALLBACK(ui_quit_callback), NULL);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_item);

    /* create the top-level 'Edit' menu */
    edit_item = gtk_menu_item_new_with_mnemonic("_Edit");
    edit_submenu = gtk_menu_new();
    copy_item = gtk_menu_item_new_with_mnemonic("_Copy");
    paste_item = gtk_menu_item_new_with_mnemonic("_Paste");
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_submenu), copy_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_submenu), paste_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), edit_item);

    /* create the top-level 'Snapshot' menu */
    snap_item = gtk_menu_item_new_with_mnemonic("_Schnapps shots");
    snapshot_submenu = gtk_menu_new();
    load_item = gtk_menu_item_new_with_mnemonic("_Load");
    save_item = gtk_menu_item_new_with_mnemonic("_Save");
    gtk_menu_shell_append(GTK_MENU_SHELL(snapshot_submenu), load_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(snapshot_submenu), save_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(snap_item), snapshot_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), snap_item);

    /* create the top-level 'Help' menu */
    help_item = gtk_menu_item_new_with_mnemonic("_Help");   /* F1? */
    /* FIXME:   make 'Help' appear at the right end of the menu bar, doesn't
     *          work right now and all functions that seem to handle this are
     *          marked 'deprecated' -- compyx
     */
    gtk_widget_set_halign(GTK_WIDGET(help_item), GTK_ALIGN_END);
    help_submenu = gtk_menu_new();
    about_item = gtk_menu_item_new_with_mnemonic("_Aboot");
    gtk_menu_shell_append(GTK_MENU_SHELL(help_submenu), about_item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), help_item);

    g_signal_connect(about_item, "activate", G_CALLBACK(ui_about_callback), NULL);

    main_menu_bar = bar;    /* XXX: do I need g_object_ref()/g_object_unref()
                                    for this */
    return bar;
}

