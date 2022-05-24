/** \file   uimenu.h
 * \brief   Native GTK3 menu handling - header
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
 *
 */

#ifndef VICE_UIMENU_H
#define VICE_UIMENU_H

#include "vice.h"
#include <gtk/gtk.h>
#include <stdbool.h>

#include "uitypes.h"

/*
 * Public functions
 */

ui_menu_item_ref_t *ui_menu_item_ref_by_action(gint action_id);
ui_menu_item_ref_t *ui_menu_item_ref_by_hotkey(guint keysym,
                                               GdkModifierType modifier);
ui_menu_item_ref_t *ui_menu_item_ref_by_index (gint index);
gint                ui_menu_item_ref_count    (void);


GtkWidget * ui_menu_submenu_create(GtkWidget *bar, const char *label);

GtkWidget * ui_menu_add(GtkWidget *menu, ui_menu_item_t *items, gint window_id);

void        ui_set_check_menu_item_blocked(GtkWidget *item,
                                           gboolean state);
void        ui_set_check_menu_item_blocked_by_action(gint action_id,
                                                     gboolean state);
void        ui_set_check_menu_item_blocked_by_action_for_window(gint action_id,
                                                                gint window_id,
                                                                gboolean state);

void        ui_menu_init_accelerators(GtkWidget *window);

gboolean    ui_menu_remove_accel(guint keysym, GdkModifierType modifier);
gboolean    ui_menu_remove_accel_via_action(gint action_id);
gboolean    ui_menu_remove_accel_via_item_ref(ui_menu_item_ref_t *ref);
void        ui_menu_update_accel_via_item_ref(ui_menu_item_ref_t *ref,
                                              guint keysym,
                                              GdkModifierType modifier);
void        ui_menu_set_accel_via_item_ref(GtkWidget *item,
                                           ui_menu_item_ref_t *ref);

gboolean ui_set_menu_item_hotkey_by_action(gint action_id,
                                           guint keysym,
                                           GdkModifierType modifier);

ui_menu_item_t *ui_get_menu_decl_by_hotkey(guint keysym,
                                           GdkModifierType modifier);
ui_menu_item_t *ui_get_menu_decl_by_action(gint action_id);

gboolean ui_get_menu_item_hotkey_by_action(gint action_id,
                                           guint *keysym,
                                           GdkModifierType *modifier);
GtkWidget *ui_get_menu_item_by_action_for_window(gint action_id,
                                                 gint window_id);
GtkWidget *ui_get_menu_item_by_action(gint action_id);

void ui_set_menu_item_accel_label(GtkWidget *item, gint action_id);

GtkWidget *ui_get_menu_item_by_hotkey_for_window(gint window_id,
                                                 guint keysym,
                                                 GdkModifierType modifier);


void ui_clear_menu_hotkeys(void);

#endif
