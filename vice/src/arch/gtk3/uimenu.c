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
#include "uiapi.h"
#include "uiabout.h"
#include "uistatusbar.h"
#include "util.h"

#include "uimenu.h"

typedef struct ui_accel_data_s {
    GtkWidget *widget;
    ui_menu_item_t *item;
} ui_accel_data_t;

static GtkAccelGroup *accel_group = NULL;

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
    file_item = gtk_menu_item_new_with_label("File");
    file_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), file_item);

    /* create the top-level 'Edit' menu */
    edit_item = gtk_menu_item_new_with_label("Edit");
    edit_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), edit_item);

    /* create the top-level 'Snapshot' menu */
    snap_item = gtk_menu_item_new_with_label("Snapshot");
    snapshot_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(snap_item), snapshot_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), snap_item);

    /* create the top-level 'Settings' menu */
    settings_item = gtk_menu_item_new_with_label("Settings");
    settings_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings_item), settings_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), settings_item);

#ifdef DEBUG
    /* create the top-level 'Debug' menu stub (when --enable-debug is used) */
    debug_item = gtk_menu_item_new_with_label("Debug");
    debug_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(debug_item), debug_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), debug_item);
#endif

    /* create the top-level 'Help' menu */
    help_item = gtk_menu_item_new_with_label("Help");   /* F1? */
    /* FIXME:   make 'Help' appear at the right end of the menu bar, doesn't
     *          work right now and all functions that seem to handle this are
     *          marked 'deprecated' -- compyx
     */
    help_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_item), help_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), help_item);
    gtk_widget_set_halign(GTK_WIDGET(help_item), GTK_ALIGN_END);

    main_menu_bar = bar;    /* XXX: do I need g_object_ref()/g_object_unref()
                                    for this */
    return bar;
}

/** \brief  Constructor for accelerator data */
static ui_accel_data_t *ui_accel_data_new(GtkWidget *widget, ui_menu_item_t *item)
{
    ui_accel_data_t *accel_data = lib_malloc(sizeof(ui_accel_data_t));
    accel_data->widget = widget;
    accel_data->item = item;
    return accel_data;
}

/** \brief  Destructor for accelerator data. */
static void ui_accel_data_delete(gpointer data, GClosure *closure)
{
    lib_free(data);
}

/** \brief  Callback that forwards accelerator codes.
 */
static void handle_accelerator(GtkAccelGroup *accel_group, GObject *acceleratable, guint keyval, GdkModifierType modifier, gpointer user_data)
{
    ui_accel_data_t *accel_data = (ui_accel_data_t *)user_data;
    accel_data->item->callback(accel_data->widget, accel_data->item->data);
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
        GtkWidget *submenu;

        switch (items[i].type) {
            case UI_MENU_TYPE_ITEM_ACTION:  /* fall through */
                /* normal callback item */
                /* debug_gtk3("adding menu item '%s'\n", items[i].label); */
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
            case UI_MENU_TYPE_ITEM_CHECK:
                /* check mark item */
                item = gtk_check_menu_item_new_with_mnemonic(items[i].label);
                if (items[i].callback != NULL) {
                    g_signal_connect(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            NULL);
                    /* use `data` as the resource to determine the state of
                     * the checkmark
                     */
                    if (items[i].data != NULL) {
                        int state;
                        resources_get_int((const char *)items[i].data, & state);
                        gtk_check_menu_item_set_active(
                                GTK_CHECK_MENU_ITEM(item), (gboolean)state);
                    }
                } else {
                    /* grey out */
                    gtk_widget_set_sensitive(item, FALSE);
                }
                break;

            case UI_MENU_TYPE_SEPARATOR:
                /* add a separator */
                item = gtk_separator_menu_item_new();
                break;

            case UI_MENU_TYPE_SUBMENU:
                /* add a submenu */
                submenu = gtk_menu_new();
                item = gtk_menu_item_new_with_mnemonic(items[i].label);
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
                ui_menu_add(submenu, (ui_menu_item_t *)items[i].data);
                break;

            default:
                item = NULL;
                break;
        }
        if (item != NULL) {

            if (items[i].keysym != 0 && items[i].callback != NULL) {
                GClosure *accel_closure;
                debug_gtk3("adding accelerator %d to item %s'\n",
                        items[i].keysym, items[i].label);
                /* Normally you would use gtk_widget_add_accelerator
                 * here, but that will disable the accelerators if the
                 * menu is hidden, which can be configured to happen
                 * while in fullscreen. We instead create the closure
                 * by hand, add it to the GtkAccelGroup, and update
                 * the accelerator information. */
                accel_closure = g_cclosure_new(G_CALLBACK(handle_accelerator),
                                               ui_accel_data_new(item, &items[i]),
                                               ui_accel_data_delete);
                gtk_accel_group_connect(accel_group, items[i].keysym, items[i].modifier, GTK_ACCEL_MASK, accel_closure);
                gtk_accel_label_set_accel(GTK_ACCEL_LABEL(gtk_bin_get_child(GTK_BIN(item))), items[i].keysym, items[i].modifier);
            }



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


/** \brief  Create accelerator group and add it to \a window
 *
 * \param[in,out]   window  top level window
 */
void ui_menu_init_accelerators(GtkWidget *window)
{
    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
}


