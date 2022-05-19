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


/** \brief  Size of the menu references array
 *
 * \note    At the time of writing (may 2022) x128 has 252 items (including
 *          items without an associated UI action), so yeah, we need a high
 *          number.
 */
#define MENU_REFERENCES_MAX 512

/** \brief  List of runtime references to menu items
 */
static ui_menu_item_ref_t menu_item_references[MENU_REFERENCES_MAX];

/** \brief  Number of items in #menu_item_references
 */
static gint menu_ref_count = 0;


/** \brief  Add menu item reference to list of runtime references
 *
 * \param[in]   decl        menu item definition
 * \param[in]   item        Gtk menu item in the UI menu structure
 * \param[in]   handler_id  ID of the 'activate' signal handler
 * \param[in]   window_id   ID of the parent window:  PRIMARY_WINDOW or
 *                          SECONDARY_WINDOW (x128 only, the VDC window)
 *
 * \return  pointer to the new item or `NULL` on failure
 * \note    Logs an error and triggers #archdep_vice_exit() when the table is
 *          full.
 *
 *
 */
static ui_menu_item_ref_t *add_menu_item_ref(ui_menu_item_t *decl,
                                             GtkWidget *item,
                                             gulong handler_id,
                                             gint window_id)
{
    if (menu_ref_count == MENU_REFERENCES_MAX) {
        log_error(LOG_ERR,
                  "%s: menu item references table is FULL",
                  __func__);
        archdep_vice_exit(1);
        return NULL;
    } else {
        ui_menu_item_ref_t *ref = NULL;

        /* If we have an item for the secondary window we look up the corresponding
         * item of the primary window */
        if (window_id == SECONDARY_WINDOW) {
            gint index;

            for (index = 0; index < menu_ref_count; index++) {
                ref = &menu_item_references[index];
                if (ref->decl == decl) {
                    ref->item[SECONDARY_WINDOW] = item;
                    ref->handler_id[SECONDARY_WINDOW] = handler_id;
                    debug_gtk3("updated item %d, action ID = %d, window ID = %d",
                               index, decl->action_id, window_id);
                    return ref;
                }
            }
            log_error(LOG_ERR,
                      "%s: failed to find corresponding primary window item for"
                      " secondary window item.",
                      __func__);
            return NULL;

        } else {
            ref = &menu_item_references[menu_ref_count];
            ref->decl = decl;
            ref->item[PRIMARY_WINDOW] = item;
            ref->item[SECONDARY_WINDOW] = NULL;
            ref->handler_id[PRIMARY_WINDOW] = handler_id;
            ref->handler_id[SECONDARY_WINDOW] = 0;
            menu_ref_count++;
            debug_gtk3("added item %d, action ID = %d, window ID = %d",
                    menu_ref_count, decl->action_id, window_id);


        }
        return ref;
    }
}


/** \brief  Check if window ID is valid
 *
 * Check if \a window_id is either #PRIMARY_WINDOW or #SECONDARY_WINDOW.
 *
 * Logs an error if \a window_id is invalid.
 *
 * \param[in]   window_id   window ID
 */
static gboolean valid_window_id(gint window_id)
{
    if (window_id != PRIMARY_WINDOW && window_id != SECONDARY_WINDOW) {
        log_error(LOG_ERR, "Invalid window ID of %d.", window_id);
        return FALSE;
    }
    return TRUE;
}


/** \brief  Get menu item references by action ID
 *
 * Look up runtime menu item reference by \a action_id.
 *
 * \param[in]   action_id   UI action ID
 *
 * \return  reference or `NULL` when not found
 *
 * \see src/arch/shared/uiactions.h for the IDs.
 */
ui_menu_item_ref_t *ui_menu_item_ref_by_action(gint action_id)
{
    if (action_id > ACTION_NONE && action_id < ACTION_ID_COUNT) {
        gint i = 0;
        for (i = 0; i < menu_ref_count; i++) {
            ui_menu_item_ref_t *ref = &menu_item_references[i];

            if (ref->decl->action_id == action_id) {
                return ref;
            }
        }
    }
    return NULL;
}


/** \brief  Get menu item reference by index
 *
 * \param[in]   index   index in the menu item references array
 *
 * \return  item or `NULL` when \a index is out of bounds */
ui_menu_item_ref_t *ui_menu_item_ref_by_index(gint index)
{
    if (index < 0 || index >= menu_ref_count) {
        return NULL;
    }
    return &menu_item_references[index];
}


/** \brief  Return number of menu item references
 *
 * \return  number of items in #menu_item_references array
 */
gint ui_menu_item_ref_count(void)
{
    return menu_ref_count;
}


/** \brief  Get item reference by hotkey mask and keysym
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  item or `NULL` when not found
 */
ui_menu_item_ref_t *ui_menu_item_ref_by_hotkey(guint keysym,
                                               GdkModifierType modifier)
{
    gint i;

    for (i = 0; i < menu_ref_count; i++) {
        GtkAccelLabel *label;
        guint item_keysym;
        guint item_modifier;
        ui_menu_item_ref_t *ref;

        ref = &menu_item_references[i];
        label = GTK_ACCEL_LABEL(gtk_bin_get_child(GTK_BIN(ref->item)));
        gtk_accel_label_get_accel(label, &item_keysym, &item_modifier);
        if (item_keysym == keysym && item_modifier == modifier) {
            return ref;
        }
    }
    return NULL;
}


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
 * \param[in,out]   user_data       accelerator data
 */
static void handle_accelerator(GtkAccelGroup *accel_grp,
                               GObject *acceleratable,
                               guint keyval,
                               GdkModifierType modifier,
                               gpointer user_data)
{
    GtkWidget *item;
    ui_menu_item_ref_t *ref = user_data;
    ui_menu_item_t *decl = ref->decl;
    gint window_index = ui_get_main_window_index();

    item = ref->item[window_index];

    if (decl->type == UI_MENU_TYPE_ITEM_CHECK) {
        /* check items get the 'resource' member as event data */
        decl->callback(item, decl->resource);
    } else {
        /* other items get the 'data' member as event data */
        decl->callback(item, decl->data);
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
GtkWidget *ui_menu_add(GtkWidget *menu, ui_menu_item_t *items, gint window_id)
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
                group = NULL;   /* terminate radio button group */

                item = gtk_check_menu_item_new_with_mnemonic(items[i].label);
                if (items[i].callback != NULL) {
                    /* use the resource name, if present, to determine the state
                     * of the checkmark
                     */
                    if (items[i].resource != NULL) {
                        int state;

                        resources_get_int((const char *)items[i].resource, &state);
                        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
                                                       (gboolean)state);
                    }
                    /* connect signal handler AFTER setting the state, otherwise
                     * the callback gets triggered, leading to odd results */
                    if (items[i].unlocked) {
                        handler_id = g_signal_connect_unlocked(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            items[i].resource);
                    } else {
                        handler_id = g_signal_connect(
                            item,
                            "activate",
                            G_CALLBACK(items[i].callback),
                            items[i].resource);
                    }
                } else {
                    /* grey out */
                    gtk_widget_set_sensitive(item, FALSE);
                }
                break;

            case UI_MENU_TYPE_ITEM_RADIO_INT:   /* fall through */
            case UI_MENU_TYPE_ITEM_RADIO_STR:
                /* radio button item */

                item = gtk_radio_menu_item_new_with_label(group, items[i].label);
                group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));

                if (items[i].callback != NULL) {
                    /* use `data` and the resource to determine the state of
                     * the checkmark
                     */
                    if (items[i].resource != NULL) {
                        bool active = false;

                        if (items[i].type == UI_MENU_TYPE_ITEM_RADIO_INT) {
                            int res_val = 0;
                            int item_val = GPOINTER_TO_INT(items[i].data);

                            resources_get_int(items[i].resource, &res_val);
                            active = (bool)(res_val == item_val);
                        } else {
                            const char *res_val = NULL;
                            const char *item_val = items[i].data;

                            resources_get_string(items[i].resource, &res_val);
                            if (res_val != NULL && item_val != NULL) {
                                active = (bool)(strcmp(res_val, item_val) == 0);
                            } else if (res_val == NULL && item_val == NULL) {
                                active = true;
                            }
                        }
                        gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item),
                                                       active);
                    }

                    /* connect signal handler AFTER setting the state, otherwise
                     * the callback gets triggered, leading to odd results */
                    if (items[i].unlocked) {
                        handler_id = g_signal_connect_unlocked(
                            item,
                            "toggled",
                            G_CALLBACK(items[i].callback),
                            items[i].data);
                    } else {
                        handler_id = g_signal_connect(
                            item,
                            "toggled",
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
                group = NULL;   /* terminate radio button group */
                item = gtk_separator_menu_item_new();
                break;

            case UI_MENU_TYPE_SUBMENU:
                /* add a submenu */
                group = NULL;   /* terminate radio button group */
                submenu = gtk_menu_new();
                item = gtk_menu_item_new_with_mnemonic(items[i].label);
                gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), submenu);
                ui_menu_add(submenu, (ui_menu_item_t *)items[i].data, window_id);
                break;

            default:
                group = NULL;
                item = NULL;
                break;
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
            /* set resource name */
            g_object_set_data(G_OBJECT(item),
                              "ResourceName",
                              items[i].resource);

            /* add item to table of references if it triggers a UI action */
            if (items[i].action_id > ACTION_NONE) {
                add_menu_item_ref(&items[i], item, handler_id, window_id);
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


/** \brief  Set checkbox menu \a item to \a state while blocking its handler
 *
 * Set a checkbox menu item's state while blocking the 'activate' handler so
 * the handler won't recursively call itself.
 *
 * \param[in,out]   item    GtkCheckMenuItem instance
 * \param[in]       state   new state for \a item
 */
void ui_set_check_menu_item_blocked(GtkWidget *item, gboolean state)
{
    gulong handler_id = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(item), "HandlerID"));
#if 0
    debug_gtk3("HandlerID = %lu.", handler_id);
#endif
    /* block signal handler */
    g_signal_handler_block(item, handler_id);
    /* update state */
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), state);
    /* unblock signal handler */
    g_signal_handler_unblock(item, handler_id);
}


/** \brief  Set checkbox menu item \a name to \a state while blocking its handler
 *
 * Set a checkbox menu item's state while blocking the 'activate' handler so
 * the handler won't recursively call itself.
 *
 * \param[in]   action_id   action ID
 * \param[in]   state       new state for \a action_id
 */
void ui_set_check_menu_item_blocked_by_action(gint action_id, gboolean state)
{
    /* update check item of primary window */
    ui_set_check_menu_item_blocked_by_action_for_window(action_id,
                                                        PRIMARY_WINDOW,
                                                        state);
    if (machine_class != VICE_MACHINE_C128) {
        return;
    }
    /* update check item of secondary window (x128 VDC) */
    ui_set_check_menu_item_blocked_by_action_for_window(action_id,
                                                        SECONDARY_WINDOW,
                                                        state);
}


/** \brief  Set checkbox menu item \a name to \a state while blocking its handler
 *
 * Set a checkbox menu item's state while blocking the 'activate' handler so
 * the handler won't recursively call itself.
 *
 * This function adds a window \a index parameter, required for x128.
 *
 * \param[in]   action_id   action ID
 * \param[in]   window_id   window ID
 * \param[in]   state       new state for \a action_id
 */
void ui_set_check_menu_item_blocked_by_action_for_window(gint action_id,
                                                         gint window_id,
                                                         gboolean state)
{
    GtkWidget *item;

    item = ui_get_menu_item_by_action_for_window(action_id, window_id);
    if (item != NULL) {
        ui_set_check_menu_item_blocked(item, state);
    }
}


/** \brief  Remove accelerator from a menu item via item ref
 *
 * Removes accelerators from menu items via \a ref from the primary and
 * secondary (x128 only) window.
 *
 * \param[in]   ref     menu item reference
 *
 * \return  `TRUE` on success
 */
gboolean ui_menu_remove_accel_via_item_ref(ui_menu_item_ref_t *ref)
{
    GtkWidget *label;

    if (ref->item[PRIMARY_WINDOW] != NULL) {
        label = gtk_bin_get_child(GTK_BIN(ref->item[PRIMARY_WINDOW]));
        gtk_accel_label_set_accel(GTK_ACCEL_LABEL(label), 0, 0);
    }
    if (ref->item[SECONDARY_WINDOW] != NULL) {
        label = gtk_bin_get_child(GTK_BIN(ref->item[SECONDARY_WINDOW]));
        gtk_accel_label_set_accel(GTK_ACCEL_LABEL(label), 0, 0);
    }
    return gtk_accel_group_disconnect_key(accel_group, ref->keysym, ref->modifier);
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


/** \brief  Set menu item accelerator via menu item reference
 *
 * Set a GtkMenuItem accelerator via an element of the menu item references
 * table.
 *
 * Sets up a closure that triggers a menu item's handler.
 * Using gtk_menu_item_add_accelerator() we lose the accelerators once we
 * hide the menu (fullscreen). With closures in an accelerator group we can
 * still trigger the handlers, even in fullscreen.
 *
 * \note    We can't use ref->item[] here since we also want to be able to
 *          set an accelerator for popup menu items.
 *
 *
 * \param[in]   item_gtk3   Gtk menu item
 * \param[in]   ref         menu item reference
 */
void ui_menu_set_accel_via_item_ref(GtkWidget *item,
                                    ui_menu_item_ref_t *ref)
{
    GtkWidget *child;
    GClosure *closure;

    closure = g_cclosure_new(G_CALLBACK(handle_accelerator), (gpointer)ref, NULL);

    if (ref->decl->unlocked) {
        gtk_accel_group_connect(accel_group,
                                ref->keysym,
                                ref->modifier,
                                GTK_ACCEL_MASK,
                                closure);
    } else {
        vice_locking_gtk_accel_group_connect(accel_group,
                                             ref->keysym,
                                             ref->modifier,
                                             GTK_ACCEL_MASK,
                                             closure);
    }

    child = gtk_bin_get_child(GTK_BIN(item));
    gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child),
                              ref->keysym,
                              ref->modifier);
}

#if 0
/** \brief  Set hotkey for item with a specific action ID and window ID
 *
 * Look up menu item for \a action_id and \a window_id and set up an accelerator
 * with a GClosure to trigger the callback in fullscreen mode as well.
 *
 * \param[in]   action_id   action ID
 * \param[in]   window_id   window ID (#PRIMARY_WINDOW or #SECONDARY_WINDOW)
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  `TRUE` on success
 */
gboolean ui_set_menu_item_hotkey_by_action_for_window(gint action_id,
                                                      gint window_id,
                                                      guint keysym,
                                                      GdkModifierType modifier)
{
    ui_menu_item_ref_t *ref;
    GtkWidget *child;
    GClosure *accel_closure;

    debug_gtk3("setting action %d for window %d, keysym %u, mods %u",
                action_id, window_id, keysym, modifier);

    ref = ui_menu_item_ref_by_action(action_id, window_id);
    if (ref == NULL) {
        debug_gtk3("failed to find item.");
        return FALSE;
    }

    debug_gtk3("removing old accelerator from group");
    ui_menu_remove_accel(keysym, modifier);
    ref->keysym = keysym;
    ref->modifier = modifier;

    debug_gtk3("Setting new accelerator");

    accel_closure = g_cclosure_new(G_CALLBACK(handle_accelerator),
                                   (gpointer)ref,
                                   NULL);

    if (ref->decl->unlocked) {
        gtk_accel_group_connect(accel_group,
                                keysym,
                                modifier,
                                GTK_ACCEL_MASK,
                                accel_closure);
    } else {
        vice_locking_gtk_accel_group_connect(accel_group,
                                             keysym,
                                             modifier,
                                             GTK_ACCEL_MASK,
                                             accel_closure);
    }

    child = gtk_bin_get_child(GTK_BIN(ref->item));
    gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), keysym, modifier);

    return TRUE;
}
#endif


/** \brief  Set hotkey for menu item with specific UI action ID
 *
 * Sets the hotkey for both windows in case of x128.
 *
 * \param[in]   action_id   action ID
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  `TRUE` on success
 */
gboolean ui_set_menu_item_hotkey_by_action(gint action_id,
                                           guint keysym,
                                           GdkModifierType modifier)
{
    GtkWidget *child;
    GClosure *accel_closure;
    ui_menu_item_ref_t *ref;

    debug_gtk3("setting action %d: keysym %u, mods %u",
                action_id, keysym, modifier);

    ref = ui_menu_item_ref_by_action(action_id);
    if (ref == NULL) {
        debug_gtk3("failed to find item.");
        return FALSE;
    }

    debug_gtk3("removing old accelerator from group");
    ui_menu_remove_accel(keysym, modifier);
    /* set new accel */
    ref->keysym = keysym;
    ref->modifier = modifier;

    debug_gtk3("Setting new accelerator");
    accel_closure = g_cclosure_new(G_CALLBACK(handle_accelerator),
                                   (gpointer)ref,
                                   NULL);
    if (ref->decl->unlocked) {
        gtk_accel_group_connect(accel_group,
                                keysym,
                                modifier,
                                GTK_ACCEL_MASK,
                                accel_closure);
    } else {
        vice_locking_gtk_accel_group_connect(accel_group,
                                             keysym,
                                             modifier,
                                             GTK_ACCEL_MASK,
                                             accel_closure);
    }

    child = gtk_bin_get_child(GTK_BIN(ref->item[PRIMARY_WINDOW]));
    gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), keysym, modifier);
    if (ref->item[SECONDARY_WINDOW] != NULL) {
        /* add accelerator to the VDC window  */
        child = gtk_bin_get_child(GTK_BIN(ref->item[SECONDARY_WINDOW]));
        gtk_accel_label_set_accel(GTK_ACCEL_LABEL(child), keysym, modifier);
    }

    return TRUE;
}


/** \brief  Get menu item declaration for hotkey
 *
 * \param[in]   window_id   window ID
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  pointer to menu item or `NULL` when not found
 */
ui_menu_item_t *ui_get_menu_decl_by_hotkey(guint keysym,
                                           GdkModifierType modifier)
{
    ui_menu_item_ref_t *ref = ui_menu_item_ref_by_hotkey(keysym, modifier);
    return ref != NULL ? ref->decl : NULL;
}


/** \brief  Clear hotkeys of all the menu items
 *
 * Iterates all menu items removes accelerators the items.
 */
void ui_clear_menu_hotkeys(void)
{
    gint i = 0;

    for (i = 0; i < menu_ref_count; i++) {
        ui_menu_item_ref_t *ref = &menu_item_references[i];

        ui_menu_remove_accel_via_item_ref(ref);
        ref->modifier = 0;
        ref->keysym = 0;
    }
}


/** \brief  Get hotkey for action of the given window
 *
 * \param[in]   action_id   action ID
 * \param[out]  keysym      Gdk keysym
 * \param[out]  modifier    Gdk modifier mask
 *
 * \return  TRUE on success
 */
gboolean ui_get_menu_item_hotkey_by_action(gint action_id,
                                           guint *keysym,
                                           GdkModifierType *modifier)
{

    ui_menu_item_ref_t *ref = ui_menu_item_ref_by_action(action_id);

    if (ref != NULL) {
        *keysym = ref->keysym;
        *modifier = ref->modifier;
        return TRUE;
    } else {
        *keysym = 0;
        *modifier = 0;
        return FALSE;
    }
}


/** \brief  Get menu item declaration for action ID
 *
 * \param[in]   id  action ID
 *
 * \return  pointer to menu item declaration or `NULL` when not found
 */
ui_menu_item_t *ui_get_menu_decl_by_action(gint action_id)
{
    ui_menu_item_ref_t *ref = ui_menu_item_ref_by_action(action_id);
    return ref != NULL ? ref->decl : NULL;
}


/** \brief  Get menu item from the Gtk menu bar by action and window index
 *
 * Try to look up a menu item for \a action_id in a window.
 *
 * \param[in]   action_id   menu action ID
 * \param[in]   window_id   window ID
 *
 * \return  GtkMenuItem reference or `NULL` when not found
 */
GtkWidget *ui_get_menu_item_by_action_for_window(gint action_id,
                                                 gint window_id)
{

    if (valid_window_id(window_id)) {
        ui_menu_item_ref_t *ref = ui_menu_item_ref_by_action(action_id);
        if (ref != NULL) {
            return ref->item[window_id];
        }
    }
    return NULL;
}


/** \brief  Look up menu item by hotkey (modifiers + keysym)
 *
 * \param[in]   modifier    Gdk modifier mask
 * \param[in]   keysym      Gdk keysym
 * \param[in]   window_id   window ID
 *
 * \return  item or `NULL` when not found
 */
GtkWidget *ui_get_menu_item_by_hotkey_for_window(gint window_id,
                                                 guint keysym,
                                                 GdkModifierType modifier)
{
    if (valid_window_id(window_id)) {
        ui_menu_item_ref_t *ref = ui_menu_item_ref_by_hotkey(keysym, modifier);
        if (ref != NULL) {
            return ref->item[window_id];
        }
    }
    return NULL;
}


/** \brief  Set accelator label according to the related main menu item
 *
 * Doesn't actually add an active accelerator, just the formatted label, the
 * actual keypress is handled by the related main menu item.
 *
 * Used to set popup menu item accelerators that trigger an action also present
 * as a main menu item.
 *
 * \param[in]   item        popup menu item
 * \param[in]   action_id   UI action ID
 *
 * \see     uiactions.h for action names
 */
void ui_set_menu_item_accel_label(GtkWidget *item, gint action_id)
{
    GtkWidget *accel_label;
    guint keysym;
    GdkModifierType modifier;

    accel_label = gtk_bin_get_child(GTK_BIN(item));
    if (ui_get_menu_item_hotkey_by_action(action_id, &keysym, &modifier)) {
        gtk_accel_label_set_accel(GTK_ACCEL_LABEL(accel_label), keysym, modifier);
    }
}
