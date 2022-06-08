/** \file   hotkeymap.c
 * \brief   Mapping of hotkeys to actions and menu items
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
 */

#include "vice.h"

#include <gtk/gtk.h>
#include "lib.h"
#include "log.h"
#include "ui.h"
#include "uiactions.h"
#include "uitypes.h"
#include "vice_gtk3.h"

#include "hotkeymap.h"


/** \brief  Reference to the accelerator group
 *
 * Global accelerator group used for all accelerators in UI.
 */
static GtkAccelGroup *accel_group = NULL;

/** \brief  Mappings list head */
static hotkey_map_t *maps_head = NULL;

/** \brief  Mappings list tail */
static hotkey_map_t *maps_tail = NULL;

/** \brief  Number of registered hotkey mappings */
static int maps_count = 0;


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
static void connect_accelerator(int action,
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


/** \brief  Remove accelerator from global accelerator group
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  `TRUE` on success
 */
gboolean ui_remove_accelerator(guint keysym, GdkModifierType modifier)
{
    return gtk_accel_group_disconnect_key(accel_group, keysym, modifier);
}


/** \brief  Create accelerator group and add it to \a window
 *
 * \param[in]       window  top level window
 */
void ui_init_accelerators(GtkWidget *window)
{
    if (accel_group == NULL) {
        accel_group = gtk_accel_group_new();
    }
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
}


/** \brief  Add actions that don't have a corresponding menu item
 *
 * Since setting up the menu happens during window creation (ie early in the
 * UI init process), we add any actions that aren't already in the mappings
 * list here.
 */
void hotkey_map_add_actions(void)
{
    const ui_action_map_t *action = ui_actions_get_registered();

    while (action->action > ACTION_NONE) {
        if (!hotkey_map_get_by_action(action->action)) {
            /* action not yet registered, is it valid for the current machine? */
            if (ui_action_is_valid(action->action)) {
                hotkey_map_t *map;

                debug_gtk3("action %d (%s) not yet registered, appending.",
                           action->action, ui_action_get_name(action->action));

                map = hotkey_map_new();
                map->action = action->action;
                hotkey_map_append(map);
            }
        }
        action++;
    }
}


/** \brief  Free memory used by all hotkey maps */
void hotkey_map_shutdown(void)
{
    hotkey_map_t *node = maps_head;

    while (node != NULL) {
        hotkey_map_t *next = node->next;
        lib_free(node);
        node = next;
    }
    maps_head = NULL;
    maps_tail = NULL;
}


/** \brief  Get number of hotkey maps
 */
int hotkey_map_count(void)
{
    return maps_count;
}


/** \brief  Get hotkey map list head node
 *
 * \return  head node
 */
hotkey_map_t *hotkey_map_get_head(void)
{
    return maps_head;
}


/** \brief  Get hotkey map list tail node
 *
 * \return  tail node
 */
hotkey_map_t *hotkey_map_get_tail(void)
{
    return maps_tail;
}


/** \brief  Clear hotkeys map object
 *
 * Sets all members of \a map to 0/NULL.
 *
 * \param[in]   map hotkey map
 */
void hotkey_map_clear(hotkey_map_t *map)
{
    map->keysym = 0;
    map->modifier = 0;
    map->action = ACTION_NONE;
    map->item[PRIMARY_WINDOW] = NULL;
    map->item[SECONDARY_WINDOW] = NULL;
    map->decl = NULL;
    map->next = NULL;
    map->prev = NULL;
}


/** \brief  Allocate and initialize new hotkey map object
 *
 * \return  new hotkey map
 */
hotkey_map_t *hotkey_map_new(void)
{
    hotkey_map_t *map = lib_malloc(sizeof *map);
    hotkey_map_clear(map);
    return map;
}


/** \brief  Append hotkey map to list of maps
 *
 */
void hotkey_map_append(hotkey_map_t *map)
{
    if (maps_tail == NULL) {
        map->next = map->prev = NULL;
        maps_head = maps_tail = map;
    } else {
        maps_tail->next = map;
        map->prev = maps_tail;
        map->next = NULL;
        maps_tail = map;
    }
    maps_count++;
}


/** \brief  Delete hotkey map from the list and free it
 *
 * \param[in]   map hotkey map
 */
void hotkey_map_delete(hotkey_map_t *map)
{
    if (map->next == NULL && map->prev == NULL) {
        if (map == maps_head) {
            /* the only node in the list */
            lib_free(map);
            maps_head = NULL;
            maps_tail = NULL;
        }
        /* node isn't in the list, ignore */
        return;
    } else if (map->next == NULL) {
        /* tail node */
        map->prev->next = NULL;
        maps_tail = map->prev;
    } else if (map->prev == NULL) {
        /* head node */
        map->next->prev = NULL;
        maps_head = map->next;
    } else {
        /* inside node */
        map->prev->next = map->next;
        map->next->prev = map->prev;
    }
    lib_free(map);
    maps_count--;
}


/** \brief  Delete hotkey map for given UI action from list and free it
 *
 * Look up hotkey map for \a action and if found remove it from list and free
 * it.
 *
 * \param[in]   action  UI action ID
 */
void hotkey_map_delete_by_action(int action)
{
    hotkey_map_t *map = hotkey_map_get_by_action(action);
    if (map != NULL) {
        hotkey_map_delete(map);
    }
}


/** \brief  Delete hotkey map for given hotkey from list and free it
 *
 * Look up hotkey map for \a keysym and \a modifier and if found remove it from
 * the list and free it.
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 */
void hotkey_map_delete_by_hotkey(guint keysym, GdkModifierType modifier)
{
    hotkey_map_t *map = hotkey_map_get_by_hotkey(keysym, modifier);
    if (map != NULL) {
        hotkey_map_delete(map);
    }
}


/** \brief  Look up hotkey map by UI action ID
 *
 * \param[in]   action  UI action ID
 *
 * \return  hotkey map or `NULL` when not found
 */
hotkey_map_t *hotkey_map_get_by_action(int action)
{
    if (action <= ACTION_NONE) {
        return NULL;
    } else {
        hotkey_map_t *node;

        for (node = maps_head; node != NULL; node = node->next) {
            if (node->action == action) {
                return node;
            }
        }
    }
    return NULL;
}


/** \brief  Look up hotkey map by hotkey
 *
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  hotkey map or `NULL` when not found
 */

hotkey_map_t *hotkey_map_get_by_hotkey(guint keysym, GdkModifierType modifier)
{
    hotkey_map_t *node;

    for (node = maps_head; node != NULL; node = node->next) {
        if (node->keysym == keysym && node->modifier == modifier) {
            return node;
        }
    }
    return NULL;
}


/** \brief  Get accelerator label for hotkey in \a map
 *
 * \param[in]   map hotkey map
 *
 * \return  accelerator label or `NULL` when no hotkey defined
 *
 * \note    The value returned is allocated by Gtk and should be freed after
 *          use with g_free()
 */
gchar *hotkey_map_get_accel_label(const hotkey_map_t *map)
{
    if (map->keysym > 0) {
        return gtk_accelerator_get_label(map->keysym, map->modifier);
    }
    return NULL;
}


/** \brief  Get accelerator label for UI action, if any
 *
 * \param[in]   action  UI action ID
 *
 * \return  accelerator label or `NULL when \a action not found or no hotkey
 *          exists for the \a action
 */
gchar *hotkey_map_get_accel_label_for_action(int action)
{
    hotkey_map_t *map = hotkey_map_get_by_action(action);
    if (map != NULL) {
        return hotkey_map_get_accel_label(map);
    }
    return NULL;
}


/** \brief  Get runtime menu item for hotkey and window
 *
 * \param[in]   window_id   window ID (`PRIMARY_WINDOW` or `SECONDARY_WINDOW`)
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  menu item or `NULL` when not found
 */
GtkWidget *hotkey_map_get_menu_item_by_hotkey_for_window(gint window_id,
                                                         guint keysym,
                                                         GdkModifierType modifier)
{
    if (valid_window_id(window_id)) {
        hotkey_map_t *map = hotkey_map_get_by_hotkey(keysym, modifier);
        if (map != NULL) {
            return map->item[window_id];
        }
    }
    return NULL;
}


/** \brief  Clear accelerator from runtime menu item
 *
 * \param[in]   item    runtime menu item
 */
static void clear_menu_item_accel(GtkWidget *item)
{
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(item));
    gtk_accel_label_set_accel(GTK_ACCEL_LABEL(label), 0, 0);
}


/** \brief  Clear hotkey from map and its associated actions and or items
 *
 * \param[in]   map hotkey map
 *
 * \return  `TRUE` on success
 */
gboolean hotkey_map_clear_hotkey(hotkey_map_t *map)
{
    gboolean result;

    if (map->keysym == 0) {
        /* No hotkey to remove */
        return FALSE;
    }
    if (map->item[PRIMARY_WINDOW] != NULL) {
        clear_menu_item_accel(map->item[PRIMARY_WINDOW]);
    }
    if (map->item[SECONDARY_WINDOW] != NULL) {
        clear_menu_item_accel(map->item[SECONDARY_WINDOW]);
    }
    result = ui_remove_accelerator(map->keysym, map->modifier);
    map->keysym = 0;
    map->modifier = 0;
    return result;
}

/** \brief  Clear hotkey from map and its associated actions and or items,
 *          using action ID to look up the map
 *
 * \param[in]   action  UI action ID
 *
 * \return  `TRUE` on success
 */
gboolean hotkey_map_clear_hotkey_by_action(int action)
{
    hotkey_map_t *map = hotkey_map_get_by_action(action);

    if (map != NULL) {
        return hotkey_map_clear_hotkey(map);
    } else {
        return FALSE;
    }
}


/** \brief  Set accelerator for menu item
 *
 * \param[in]   item    runtime menu item
 * \param[in]   map     hotkey map
 *
 * \note    This only sets the \a item's accelerator, it does not connect any
 *          handler, the handler is set up during menu creation in uimenu.c.
 */
static void set_menu_item_accel_via_map(GtkWidget *item, hotkey_map_t *map)
{
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(item));
    gtk_accel_label_set_accel(GTK_ACCEL_LABEL(label), map->keysym, map->modifier);
}


/** \brief  Set up handler for the hotkey and menu item accelerators if needed
 *
 * Sets up a closure to trigger a UI action for the hotkey and sets the menu
 * item(s) accelerators if there are any menu items associated with the action.
 *
 * \param[in]   map hotkey map
 *
 * \return  `TRUE` on success
 */
gboolean hotkey_map_setup_hotkey(hotkey_map_t *map)
{
    gboolean result = FALSE;

    /* disconnect accelerator for hotkey, if any */
    ui_remove_accelerator(map->keysym, map->modifier);

    /* setup gclosure to handle accelerator */
    connect_accelerator(map->action,
                        map->keysym,
                        map->modifier,
                        map->decl != NULL ? map->decl->unlocked : false);

    /* set accelerator label for primary window */
    if (map->item[PRIMARY_WINDOW] != NULL) {
        set_menu_item_accel_via_map(map->item[PRIMARY_WINDOW], map);
        result = TRUE;
    }
    /* set accelerator label for secondary window */
    if (map->item[SECONDARY_WINDOW] != NULL) {
        set_menu_item_accel_via_map(map->item[SECONDARY_WINDOW], map);
    }
    return result;
}


/** \brief  Remove old hotkey and set new hotkey
 *
 * \param[in]   map         hotkey map
 * \param[in]   keysym      Gdk keysym
 * \param[in]   modifier    Gdk modifier mask
 *
 * \return  `TRUE` on success
 */
gboolean hotkey_map_update_hotkey(hotkey_map_t *map,
                                  guint keysym,
                                  GdkModifierType modifier)
{
    /* remove old accelerator */
    ui_remove_accelerator(map->keysym, map->modifier);

    /* set new accelerator */
    map->keysym = keysym;
    map->modifier = modifier;

    return hotkey_map_setup_hotkey(map);
}


/** \brief  Clear all hotkeys
 *
 * This removes all registered hotkeys, those for menu items and those without
 * menu items.
 */
void ui_clear_hotkeys(void)
{
    hotkey_map_t *node;

    for (node = maps_head; node != NULL; node = node->next) {
        if (node->keysym != 0) {
            hotkey_map_clear_hotkey(node);
        }
    }
}



/** \brief  Get runtime menu item for UI action and main window
 *
 * \param[in]   action      UI action ID
 * \param[in]   window_id   window ID (`PRIMARY_WINDOW` or `SECONDARY_WINDOW`)
 *
 * \return  menu item or `NULL` when not found
 */
GtkWidget *ui_get_menu_item_by_action_for_window(gint action, gint window_id)
{
    if (valid_window_id(window_id)) {
        hotkey_map_t *map = hotkey_map_get_by_action(action);
        if (map != NULL) {
            return map->item[window_id];
        }
    }
    return NULL;
}


/** \brief  Set accelerator label to the hotkey associated with UI action
 *
 * Look up \a action and if found set the accelerator label of \a item to the
 * hotkey defined for \a action, if the action is not found the accelerator
 * label will be cleared.
 * This allows context menus to display an accelerator next to an item. Only
 * sets the label of a menu item, no action handler is set for the item.
 *
 * \param[in]   item    runtime menu item
 * \param[in]   action  UI action ID
 */
void ui_set_menu_item_accel_label(GtkWidget *item, gint action)
{
    const hotkey_map_t *map;
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(item));

    map = hotkey_map_get_by_action(action);
    if (map != NULL) {
        gtk_accel_label_set_accel(GTK_ACCEL_LABEL(label),
                                  map->keysym,
                                  map->modifier);
    } else {
        gtk_accel_label_set_accel(GTK_ACCEL_LABEL(label), 0, 0);
    }
}


/** \brief  Check/uncheck check menu item while blocking its handler
 *
 * Set a checkbox menu item's state while blocking the 'activate' handler so
 * the handler won't recursively call itself.
 *
 * \param[in]   item    GtkCheckMenuItem instance
 * \param[in]   checked new check button state
 */
void ui_set_check_menu_item_blocked(GtkWidget *item, gboolean checked)
{
    gulong handler_id = GPOINTER_TO_ULONG(g_object_get_data(G_OBJECT(item),
                                                            "HandlerID"));
    g_signal_handler_block(item, handler_id);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), checked);
    g_signal_handler_unblock(item, handler_id);
}


/** \brief  Check/uncheck check menu item(s) mapped to UI action
 *
 * Set menu item or items (x128) that are registered for UI \a action to
 * \a checked.
 *
 * \note    This will usually suffice for most items, but in cases where x128
 *          can have different states for a menu item per window (status bar
 *          enable/disable, fullscreen, fulllscreen-decorations) the function
 *          ui_set_check_menu_item_blocked_by_action_for_window() is available.
 *
 * \param[in]   action  UI action ID
 * \param[in]   checked new checked state for menu item(s)
 */
void ui_set_check_menu_item_blocked_by_action(gint action, gboolean checked)
{
    hotkey_map_t *map = hotkey_map_get_by_action(action);
    if (map != NULL) {
        if (map->item[PRIMARY_WINDOW] != NULL) {
            ui_set_check_menu_item_blocked(map->item[PRIMARY_WINDOW], checked);
        }
        if (map->item[SECONDARY_WINDOW] != NULL) {
            ui_set_check_menu_item_blocked(map->item[SECONDARY_WINDOW], checked);
        }
    }
}

/** \brief  Check/uncheck check menu item mapped to UI action for a window
 *
 * Set menu item for a main window that is registered for UI \a action to
 * \a checked.
 *
 * \param[in]   action      UI action ID
 * \param[in]   window_id   window ID (`PRIMARY_WINDOW` or `SECONDARY_WINDOW`)
 * \param[in]   checked     new checked state for menu item(s)
 */
void ui_set_check_menu_item_blocked_by_action_for_window(gint action,
                                                         gint window_id,
                                                         gboolean checked)
{
    if (valid_window_id(window_id)) {
        hotkey_map_t *map = hotkey_map_get_by_action(action);
        if (map != NULL) {
            ui_set_check_menu_item_blocked(map->item[window_id], checked);
        }
    }
}



