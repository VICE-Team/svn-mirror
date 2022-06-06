/** \file   uimenu.c
 * \brief   Native GTK3 menu handling
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  Marcus Sutton <loggedoubt@gmail.com>
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
 */

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "archdep.h"
#include "debug_gtk3.h"
#include "vice_gtk3.h"
#include "hotkeymap.h"
#include "kbd.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "ui.h"
#include "uiactions.h"
#include "uiapi.h"
#include "uiabout.h"
#include "uistatusbar.h"
#include "util.h"

#include "uimenu.h"

/** \brief  Reference to the accelerator group
 */
static GtkAccelGroup *accel_group = NULL;


/** \brief  Create an empty submenu and add it to a menu bar
 *
 * \param[in]       bar     the menu bar to add the submenu to
 * \param[in]       label   label of the submenu to create
 *
 * \return  a reference to the new submenu
*/
GtkWidget *ui_menu_submenu_create(GtkWidget *bar, const char *label)
{
    GtkWidget *submenu_item;
    GtkWidget *new_submenu;

    submenu_item = gtk_menu_item_new_with_label(label);
    new_submenu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu_item), new_submenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(bar), submenu_item);

    return new_submenu;
}


/** \brief  Handler for the 'destroy' event of a menu item
 *
 * \param[in]       item        menu item
 * \param[in,out]   unused      extra event data (unused)
 */
static void on_menu_item_destroy(GtkWidget *item, gpointer unused)
{
    GtkAccelLabel *label;
    label = GTK_ACCEL_LABEL(gtk_bin_get_child(GTK_BIN(item)));
    if (label != NULL) {
        guint keysym;
        guint mask;

        gtk_accel_label_get_accel(label, &keysym, &mask);
        gtk_accel_group_disconnect_key(accel_group, keysym, mask);
    }
}


/** \brief  Callback that forwards accelerator codes
 *
 * \param[in]       accel_grp       accelerator group (unused)
 * \param[in]       acceleratable   ? (unused)
 * \param[in]       keyval          GDK keyval (unused)
 * \param[in]       modifier        GDK key modifier(s) (unused)
 * \param[in]       action_id       UI action ID
 */
static gboolean handle_accelerator(GtkAccelGroup *accel_grp,
                               GObject *acceleratable,
                               guint keyval,
                               GdkModifierType modifier,
                               gpointer action_id)
{
    debug_gtk3("Called with action ID %d", GPOINTER_TO_INT(action_id));
    ui_action_trigger(GPOINTER_TO_INT(action_id));
    return TRUE;
}


/** \brief  Handler for the 'activate' event of a menu item
 *
 * Used for non-radio button items.
 *
 * \param[in]   item        menu item
 * \param[in]   action_id   UI action ID
 */
static void on_menu_item_activate(GtkWidget *item, gpointer action_id)
{
    debug_gtk3("Called with action ID %d", GPOINTER_TO_INT(action_id));
    ui_action_trigger(GPOINTER_TO_INT(action_id));
}


/** \brief  Handler for the 'toggled' event of a menu item
 *
 * Used for radio button items.
 *
 * \param[in]   item        menu item
 * \param[in]   action_id   UI action ID
 */
static void on_menu_item_toggled(GtkWidget *item, gpointer action_id)
{
    gint id = GPOINTER_TO_INT(action_id);
#if 0
    debug_gtk3("Called with action ID %d (%s)", id, ui_action_get_name(id));
#endif
    /* only trigger the associated action when the radio button is on */
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item))) {
#if 0
        debug_gtk3("Item is active");
#endif
        ui_action_trigger(id);
    }
}



/** \brief  Add menu \a items to \a menu
 *
 * \param[in,out]   menu        Gtk menu
 * \param[in]       items       menu items to add to \a menu
 * \param[in]       window_id   window ID (PRIMARY_WINDOW or SECONDARY_WINDOW)
 *
 * \return  \a menu
 */
GtkWidget *ui_menu_add(GtkWidget *menu, const ui_menu_item_t *items, gint window_id)
{
    size_t i = 0;
    GSList *group = NULL;

    while (items[i].label != NULL || items[i].type >= 0) {
        GtkWidget *item = NULL;
        GtkWidget *submenu;
        gulong handler_id = 0;

        switch (items[i].type) {
            case UI_MENU_TYPE_ITEM_ACTION:
                /* normal callback item */
                group = NULL;   /* terminate radio button group */
                item = gtk_menu_item_new_with_mnemonic(items[i].label);
                break;

            case UI_MENU_TYPE_ITEM_CHECK:
                /* check mark item */
                group = NULL;   /* terminate radio button group */
                item = gtk_check_menu_item_new_with_mnemonic(items[i].label);
                break;

            case UI_MENU_TYPE_ITEM_RADIO_INT:   /* fall through */
            case UI_MENU_TYPE_ITEM_RADIO_STR:
                /* radio button item */
                item = gtk_radio_menu_item_new_with_label(group, items[i].label);
                group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
                break;

            case UI_MENU_TYPE_SEPARATOR:
                /* add a separator */
                group = NULL;   /* terminate radio button group */
                item = gtk_separator_menu_item_new();
                break;

            case UI_MENU_TYPE_SUBMENU:
                /* add a submenu */
                group = NULL;   /* terminate radio button group */
                submenu = gtk_menu_new();
                item = gtk_menu_item_new_with_mnemonic(items[i].label);
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
                ui_menu_add(submenu, items[i].submenu, window_id);
                break;

            default:
                group = NULL;
                item = NULL;
                break;
        }

        if (items[i].action_id > ACTION_NONE) {
            /* radio buttons use the "toggled" event */
            if (items[i].type == UI_MENU_TYPE_ITEM_RADIO_INT ||
                    items[i].type == UI_MENU_TYPE_ITEM_RADIO_STR) {
                if (items[i].unlocked) {
                    handler_id = g_signal_connect_unlocked(
                            item, "toggled",
                            G_CALLBACK(on_menu_item_toggled),
                            GINT_TO_POINTER(items[i].action_id));
                } else {
                    handler_id = g_signal_connect(
                            item, "toggled",
                            G_CALLBACK(on_menu_item_toggled),
                            GINT_TO_POINTER(items[i].action_id));
                }
            } else {
                if (items[i].unlocked) {
                    handler_id = g_signal_connect_unlocked(
                            item, "activate",
                            G_CALLBACK(on_menu_item_activate),
                            GINT_TO_POINTER(items[i].action_id));
                } else {
                    handler_id = g_signal_connect(
                            item, "activate",
                            G_CALLBACK(on_menu_item_activate),
                            GINT_TO_POINTER(items[i].action_id));
                }
            }
        }

        if (item != NULL) {
            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

            /* destroy handler to remove an accelerator from its accelerator
             * group, should we decide to add/remove menu items during runtime.
             */
            g_signal_connect_unlocked(item,
                                      "destroy",
                                      G_CALLBACK(on_menu_item_destroy),
                                      NULL);

            /* set signal handler ID of the 'activate' signal which we later
             * have to use to toggle the checkbox from the callback while
             * temporarily blocking the signal handler to avoid recursively
             * triggering the callback.
             */
            g_object_set_data(G_OBJECT(item),
                              "HandlerID",
                              GULONG_TO_POINTER(handler_id));

            /* set action name */
            g_object_set_data(G_OBJECT(item),
                              "ActionID",
                              GINT_TO_POINTER(items[i].action_id));

            /* add item to table of references if it triggers a UI action */
            if (items[i].action_id > ACTION_NONE) {

                hotkey_map_t *map;

                /* add to hotkey maps or update */
                if (window_id == PRIMARY_WINDOW) {
                    map = hotkey_map_new();
                    map->action = items[i].action_id;
                    map->decl = &items[i];
                    hotkey_map_append(map);
                } else {
                    map = hotkey_map_get_by_action(items[i].action_id);
                    if (map == NULL) {
                        /* this shouldn't happen! */
                        debug_gtk3("Failed to locate hotkey mapping object"
                                   "for action %d (%s).",
                                   items[i].action_id,
                                   ui_action_get_name(items[i].action_id));
                    }
                }
                if (map != NULL) {
                    map->item[window_id] = item;
                    map->handler[window_id] = handler_id;
                }
            }
        }
        i++;
    }
    return menu;
}


/** \brief  Create accelerator group and add it to \a window
 *
 * \param[in]       window  top level window
 */
void ui_menu_init_accelerators(GtkWidget *window)
{
    if (accel_group == NULL) {
        accel_group = gtk_accel_group_new();
    }
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
}


/** \brief  Remove accelerator from global accelerator group
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  `TRUE` on success
 */
gboolean ui_menu_remove_accel(guint keysym, GdkModifierType modifier)
{
    return gtk_accel_group_disconnect_key(accel_group, keysym, modifier);
}


/** \brief  Set up a closure to trigger UI action for a hotkey
 *
 * Create a closure to trigger UI \a action for \a keysym and \a modifier.
 * This way hotkeys will work in fullscreen and also when there's no menu item
 * associated with \a action.
 *
 * \param[in]   action      UI action ID
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 * \param[in]   unlocked    connect accelator non-lockeding
 */
void ui_menu_connect_accelerator(int action,
                                 guint keysym,
                                 GdkModifierType modifier,
                                 bool unlocked)
{
    GClosure *closure = g_cclosure_new(G_CALLBACK(handle_accelerator),
                                       GINT_TO_POINTER(action),
                                       NULL);

    if (unlocked) {
        gtk_accel_group_connect(accel_group,
                                keysym,
                                modifier,
                                GTK_ACCEL_MASK,
                                closure);
    } else {
        vice_locking_gtk_accel_group_connect(accel_group,
                                             keysym,
                                             modifier,
                                             GTK_ACCEL_MASK,
                                             closure);
    }
}
