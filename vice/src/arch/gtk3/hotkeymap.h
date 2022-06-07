/** \file   hotkeymap.h
 * \brief   Mapping of hotkeys to actions and menu items - header
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

#ifndef VICE_HOTKEYMAP_H
#define VICE_HOTKEYMAP_H

#include <gtk/gtk.h>
#include "uitypes.h"


/** \brief  Object mapping UI actions to hotkeys and menu items
 */
typedef struct hotkey_map_s {
    guint                   keysym;     /**< Gdk keysym */
    GdkModifierType         modifier;   /**< Gdk modifier mask */
    int                     action;     /**< UI action ID */
    GtkWidget *             item[2];    /**< runtime menu item references */
    gulong                  handler[2]; /**< event handler ID for the runtime
                                             items' activate/toggled events */
    const ui_menu_item_t *  decl;       /**< menu item declaration */
    struct hotkey_map_s *   next;       /**< next node in linked list */
    struct hotkey_map_s *   prev;       /**< previous node in linked list */
} hotkey_map_t;


void            hotkey_map_shutdown(void);
int             hotkey_map_count(void);
hotkey_map_t *  hotkey_map_get_head(void);
hotkey_map_t *  hotkey_map_get_tail(void);
hotkey_map_t *  hotkey_map_new(void);
void            hotkey_map_clear(hotkey_map_t *map);
void            hotkey_map_append(hotkey_map_t *map);
void            hotkey_map_delete(hotkey_map_t *map);
void            hotkey_map_delete_by_action(int action);
void            hotkey_map_delete_by_hotkey(guint keysym, GdkModifierType modifier);
hotkey_map_t *  hotkey_map_get_by_action(int action);
hotkey_map_t *  hotkey_map_get_by_hotkey(guint keysym, GdkModifierType modifier);
gchar *         hotkey_map_get_accel_label(const hotkey_map_t *map);
gchar *         hotkey_map_get_accel_label_for_action(int action);
GtkWidget *     hotkey_map_get_menu_item_by_hotkey_for_window(gint window_id,
                                                              guint keysym,
                                                              GdkModifierType modifier);
gboolean        hotkey_map_setup_hotkey(hotkey_map_t *map);
gboolean        hotkey_map_update_hotkey(hotkey_map_t *map,
                                         guint keysym,
                                         GdkModifierType modifier);
gboolean        hotkey_map_clear_hotkey(hotkey_map_t *map);
gboolean        hotkey_map_clear_hotkey_by_action(int action);


GtkWidget * ui_get_menu_item_by_action_for_window(gint action,
                                                      gint window_id);
void        ui_set_menu_item_accel_label(GtkWidget *item, gint action);

void        ui_set_check_menu_item_blocked(GtkWidget *item,
                                           gboolean state);
void        ui_set_check_menu_item_blocked_by_action(gint action_id,
                                                     gboolean state);
void        ui_set_check_menu_item_blocked_by_action_for_window(gint action_id,
                                                                gint window_id,
                                                                gboolean state);

void        ui_init_accelerators(GtkWidget *window);
gboolean    ui_remove_accelerator(guint keysym, GdkModifierType modifier);

void        ui_clear_hotkeys(void);

#endif
