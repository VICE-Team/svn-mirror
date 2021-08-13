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

#include "debug_gtk3.h"

#include "kbd.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uiapi.h"
#include "uiabout.h"
#include "uistatusbar.h"
#include "util.h"
#include "uimachinemenu.h"

#include "uimenu.h"


/** \brief  Size of the menu references array
 */
#define MENU_REFERENCES_MAX     256


/** \brief  Menu accelerator object
 */
typedef struct ui_accel_data_s {
    GtkWidget *widget;      /**< widget connected to the accelerator */
    ui_menu_item_t *item;   /**< menu item connected to the accelerator */
} ui_accel_data_t;


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


/** \brief  Constructor for accelerator data
 *
 * \param[in]   widget  widget for the accelerator data
 * \param[in]   item    menu item for the accelerator data
 *
 * \return  heap-allocated accelerator data (owned by VICE)
 */
static ui_accel_data_t *ui_accel_data_new(GtkWidget *widget, ui_menu_item_t *item)
{
    ui_accel_data_t *accel_data = lib_malloc(sizeof(ui_accel_data_t));
    accel_data->widget = widget;
    accel_data->item = item;
    return accel_data;
}


/** \brief  Destructor for accelerator data
 *
 * FIXME:   this doesn't get triggered
 *
 * \param[in,out]   data    accelerator data
 * \param[in]       closure closure (unused)
 */
static void ui_accel_data_delete(gpointer data, GClosure *closure)
{
    debug_gtk3("Holy Shit, this got triggered!!\n");
#if 0
    lib_free(data);
#endif
}


/** \brief  Handler for the 'destroy' event of a menu item
 *
 * This 'hack' is needed since the 'finalize' callback of the GClosures we use
 * for accelerators doesn't get called, which means the accelerator data doesn't
 * get cleaned up, which means we leak memory.
 *
 * \param[in]       item        menu item
 * \param[in,out]   accel_data  accelator data (optional)
 */
static void on_menu_item_destroy(GtkWidget *item, gpointer accel_data)
{
    if (accel_data != NULL) {
        lib_free(accel_data);
    }
}


/** \brief  Callback that forwards accelerator codes
 *
 * \param[in]       accel_grp       accelerator group (unused)
 * \param[in]       acceleratable   ? (unused)
 * \param[in]       keyval          GDK keyval (unused)
 * \param[in]       modifier        GDK key modifier(s) (unused)
 * \param[in,out]   user_data       accelerator data
 */
static void handle_accelerator(GtkAccelGroup *accel_grp,
                               GObject *acceleratable,
                               guint keyval,
                               GdkModifierType modifier,
                               gpointer user_data)
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
        ui_accel_data_t *accel_data = NULL;
        gulong handler_id = 0;

        switch (items[i].type) {
            case UI_MENU_TYPE_ITEM_ACTION:  /* fall through */
                /* normal callback item */
                item = gtk_menu_item_new_with_mnemonic(items[i].label);
                if (items[i].callback != NULL) {
                    if (items[i].unlocked) {
                        g_signal_connect_unlocked(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            (gpointer)(items[i].data));
                    } else {
                        g_signal_connect(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            (gpointer)(items[i].data));
                    }
                } else {
                    /* no callback: 'grey-out'/'ghost' the item */
                    gtk_widget_set_sensitive(item, FALSE);
                }
                break;
            case UI_MENU_TYPE_ITEM_CHECK:
                /* check mark item */
                item = gtk_check_menu_item_new_with_mnemonic(items[i].label);
                if (items[i].callback != NULL) {
                   /* use `data` as the resource to determine the state of
                     * the checkmark
                     */
                    if (items[i].data != NULL) {
                        int state;
                        resources_get_int((const char *)items[i].data, & state);
                        gtk_check_menu_item_set_active(
                                GTK_CHECK_MENU_ITEM(item), (gboolean)state);
                    }
                    /* connect signal handler AFTER setting the state, otherwise
                     * the callback gets triggered, leading to odd results */
                    if (items[i].unlocked) {
                        handler_id = g_signal_connect_unlocked(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            items[i].data);
                    } else {
                        handler_id = g_signal_connect(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            items[i].data);
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

                /* Normally you would use gtk_widget_add_accelerator
                 * here, but that will disable the accelerators if the
                 * menu is hidden, which can be configured to happen
                 * while in fullscreen. We instead create the closure
                 * by hand, add it to the GtkAccelGroup, and update
                 * the accelerator information. */
                accel_data = ui_accel_data_new(item, &items[i]);
                accel_closure = g_cclosure_new(G_CALLBACK(handle_accelerator),
                                               accel_data,
                                               ui_accel_data_delete);
                if (items[i].unlocked) {
                    gtk_accel_group_connect(accel_group, items[i].keysym, items[i].modifier, GTK_ACCEL_MASK, accel_closure);
                } else {
                    vice_locking_gtk_accel_group_connect(accel_group, items[i].keysym, items[i].modifier, GTK_ACCEL_MASK, accel_closure);
                }
                gtk_accel_label_set_accel(GTK_ACCEL_LABEL(gtk_bin_get_child(GTK_BIN(item))), items[i].keysym, items[i].modifier);
            }

            gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
            /* the closure's callback doesn't trigger due to mysterious reasons,
             * so we use the menu item to free the accelerator's data
             */
            g_signal_connect_unlocked(item,
                                      "destroy",
                                      G_CALLBACK(on_menu_item_destroy),
                                      accel_data);

            /* set signal handler ID of the 'activate' signal which we later
             * have to use to toggle the checkbox from the callback while
             * temporarily blocking the signal handler to avoid recursively
             * triggering the callback.
             */
            g_object_set_data(G_OBJECT(item),
                              "HandlerID",
                              (gpointer)handler_id);

            /* set action name */
            g_object_set_data(G_OBJECT(item),
                              "ActionName",
                              items[i].action_name);
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
    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
}


/** \brief  Recursively look up \a name in \a submenu
 *
 * \param[in]   submenu GtkMenuItem
 * \param[in]   name    item action name
 *
 * \return  GtkMenuItem or `NULL` when not found
 */
GtkWidget *ui_get_gtk_submenu_item_by_name(GtkWidget *submenu, const char *name)
{
    GList *node = gtk_container_get_children(GTK_CONTAINER(submenu));

    debug_gtk3("Iterating children of submenu.");
    while (node != NULL) {
        GtkWidget *item = node->data;
        if (GTK_IS_CONTAINER(item)) {
            const char *action_name = g_object_get_data(G_OBJECT(item),
                                                    "ActionName");
            if (action_name != NULL) {
                debug_gtk3("Checking action-name '%s' against '%s'.",
                        action_name, name);

                if (strcmp(action_name, name) == 0) {
                    debug_gtk3("FOUND");
                    return GTK_WIDGET(item);
                }
            } else {
                item = ui_get_gtk_submenu_item_by_name(item, name);
                if (item != NULL) {
                    return item;
                }
            }
        }
        node = node->next;
    }

    return NULL;
}


/** \brief  Set checkbox menu \a item to \a state while blocking its handler
 *
 * Set a checkbox menu item's state while blocking the 'activate' handler so
 * the handler won't recursively itself.
 *
 * \param[in,out]   item    GtkCheckMenuItem instance
 * \param[in]       state   new state for \a item
 *
 * \return  TRUE
 */
void ui_set_gtk_check_menu_item_blocked(GtkWidget *item, gboolean state)
{
    gulong handler_id = (gulong)g_object_get_data(G_OBJECT(item), "HandlerID");
    debug_gtk3("HandlerID = %lu.", handler_id);

    /* block signal handler */
    g_signal_handler_block(item, handler_id);
    /* update state */
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), state);
    /* unblock signal handler */
    g_signal_handler_unblock(item, handler_id);
}


void ui_set_gtk_check_menu_item_blocked_by_name(const char *name, gboolean state)
{
    GtkWidget *item;

    item = ui_get_gtk_menu_item_by_name(name);
    if (item != NULL) {
        ui_set_gtk_check_menu_item_blocked(item, state);
    }
}
