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
 *  TODO:   add code to dynamically add submenus and menu items (not all emus
 *          have the same menu)
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

#include "uimenu.h"


/*
 * The following are translation unit local so we can create functions like
 * add_to_file_menu() or even functions that alter the top bar itself.
 */


/** \brief  Main menu bar widget
 *
 * Contains the submenus on the menu main bar
 *
 * This one lives until ui_exit() or thereabouts
 */
static GtkWidget *main_menu_bar = NULL;


/** \brief  File submenu
 */
static GtkWidget *file_submenu = NULL;


/** \brief  Edit submenu (do we need this?)
 */
static GtkWidget *edit_submenu = NULL;


/** \brief  Snapshot submenu
 */
static GtkWidget *snapshot_submenu = NULL;


/** \brief  Settings submenu
 *
 * This might contain a single 'button' to pop up the new treeview-based
 * settings widgets, or even removed and the 'button' added to the File menu
 */
static GtkWidget *settings_submenu = NULL;


#ifdef DEBUG
/** \brief  Debug submenu, only available when --enable-debug was specified
 */
static GtkWidget *debug_submenu = NULL;
#endif


/** \brief  Help submenu
 */
static GtkWidget *help_submenu = NULL;


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

    /* edit menu */
    GtkWidget *edit_item;

    /* snapshot menu */
    GtkWidget *snap_item;

    /* settings menu */
    GtkWidget *settings_item;

#ifdef DEBUG
    /* debug menu */
    GtkWidget *debug_item;
#endif

    /* help menu */
    GtkWidget *help_item;


    /* create the top menu bar */
    bar = gtk_menu_bar_new();

    gtk_widget_set_hexpand(GTK_WIDGET(bar), TRUE);  /* doesn't appear to work */

    /*
     * Obviously the code here can be refactored, there's a lot duplication
     * going on -- compyx
     */

    /* create the top-level 'File' menu */
    file_item = gtk_menu_item_new_with_mnemonic("_File");
    file_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_item);

    /* create the top-level 'Edit' menu */
    edit_item = gtk_menu_item_new_with_mnemonic("_Edit");
    edit_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), edit_item);

    /* create the top-level 'Snapshot' menu */
    snap_item = gtk_menu_item_new_with_mnemonic("_Snapshot");
    snapshot_submenu = gtk_menu_new();
#if 0
    load_item = gtk_menu_item_new_with_mnemonic("_Load");
    save_item = gtk_menu_item_new_with_mnemonic("_Save");
    gtk_menu_shell_append(GTK_MENU_SHELL(snapshot_submenu), load_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(snapshot_submenu), save_item);
#endif
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(snap_item), snapshot_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), snap_item);

    /* create the top-level 'Settings' menu */
    settings_item = gtk_menu_item_new_with_mnemonic("_Settings");
    settings_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_item), settings_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), settings_item);

#ifdef DEBUG
    /* create the top-level 'Debug' menu stub (when --enable-debug is used) */
    debug_item = gtk_menu_item_new_with_mnemonic("_Debug");
    debug_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(debug_item), debug_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), debug_item);
#endif

    /* create the top-level 'Help' menu */
    help_item = gtk_menu_item_new_with_mnemonic("_Help");   /* F1? */
    /* FIXME:   make 'Help' appear at the right end of the menu bar, doesn't
     *          work right now and all functions that seem to handle this are
     *          marked 'deprecated' -- compyx
     */
    gtk_widget_set_halign(GTK_WIDGET(help_item), GTK_ALIGN_END);
    help_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), help_item);

    main_menu_bar = bar;    /* XXX: do I need g_object_ref()/g_object_unref()
                                    for this */
    return bar;
}


/** \brief  Add menu \a items to \a menu
 *
 * \param[in,out]   menu    Gtk menu
 * \param[in]       items   menu items to add to \a menu
 *
 * \return  \a menu
 */
GtkWidget *ui_menu_add(GtkWidget *menu, ui_menu_item_t *items)
{
    size_t i = 0;
    while (items[i].label != NULL || items[i].type >= 0) {
        GtkWidget *item = NULL;
        switch (items[i].type) {
            case UI_MENU_TYPE_ITEM_ACTION:  /* fall through */
            case UI_MENU_TYPE_ITEM_CHECK:   /* XXX: for now */
                /* normal callback item */
                item = gtk_menu_item_new_with_mnemonic(items[i].label);
                if (items[i].callback != NULL) {
                    g_signal_connect(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            (gpointer)(items[i].data));
                } else {
                    /* no callback: 'grey-out'/'ghost' the item */
                    gtk_widget_set_sensitive(item, FALSE);
                }
                break;
            case UI_MENU_TYPE_SEPARATOR:
                item = gtk_separator_menu_item_new();
                break;
            default:
                item = NULL;
                break;
        }
        if (item != NULL) {
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
        }
        i++;
    }
    return menu;
}


/** \brief  Add menu \a items to the 'File' menu
 *
 * \param[in]       items   menu items to add to the 'File' menu
 *
 * \return  'File' menu reference
 */
GtkWidget *ui_menu_file_add(ui_menu_item_t *items)
{
    return ui_menu_add(file_submenu, items);
}

/** \brief  Add menu \a items to the 'Edit' menu
 *
 * \param[in]       items   menu items to add to the 'File' menu
 *
 * \return  'Edit' menu reference
 */
GtkWidget *ui_menu_edit_add(ui_menu_item_t *items)
{
    return ui_menu_add(edit_submenu, items);
}


GtkWidget *ui_menu_settings_add(ui_menu_item_t *items)
{
    return ui_menu_add(settings_submenu, items);
}


/** \brief  Add menu \a items to the 'Snapshot' menu
 *
 * \param[in]       items   menu items to add to the 'Snapshot' menu
 *
 * \return  'Help' menu reference
 */

GtkWidget *ui_menu_snapshot_add(ui_menu_item_t *items)
{
    return ui_menu_add(snapshot_submenu, items);
}

/** \brief  Add menu \a items to the 'Help' menu
 *
 * \param[in]       items   menu items to add to the 'Help' menu
 *
 * \return  'Help' menu reference
 */

GtkWidget *ui_menu_help_add(ui_menu_item_t *items)
{
    return ui_menu_add(help_submenu, items);
}


#ifdef DEBUG
/** \brief  Add menu \a items to the 'Debug' menu
 *
 * \param[in]       items   menu items to add to the 'Debug' menu
 *
 * \return  'Debug' menu reference
 */
GtkWidget *ui_menu_debug_add(ui_menu_item_t *items)
{
    return ui_menu_add(debug_submenu, items);
}
#endif
