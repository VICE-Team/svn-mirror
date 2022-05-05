/** \file   uimachinemenu.h
 * \brief   Native GTK3 menus for machine emulators (not vsid)
 *
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

#ifndef VICE_UIMACHINEMENU_H
#define VICE_UIMACHINEMENU_H

#include "vice.h"
#include <gtk/gtk.h>
#include "uitypes.h"


GtkWidget * ui_machine_menu_bar_create(gint window_id);
void        ui_machine_menu_bar_vsid_patch(GtkWidget *menu, gint window_id);

/*
 * hotkeys API
 */

/* VICE menu item iterator API 8 */

void            ui_vice_menu_iter_init(ui_vice_menu_iter_t *iter);
gboolean        ui_vice_menu_iter_next(ui_vice_menu_iter_t *iter);
gboolean        ui_vice_menu_iter_get_action_id(ui_vice_menu_iter_t *iter,
                                                int *id);
gboolean        ui_vice_menu_iter_get_type(ui_vice_menu_iter_t *iter,
                                           ui_menu_item_type_t *type);
gboolean        ui_vice_menu_iter_get_hotkey(ui_vice_menu_iter_t *iter,
                                             GdkModifierType *mask,
                                             guint *keysym);



ui_menu_item_t *ui_get_vice_menu_item_by_name(const char *name);
/* FIXME: swap arguments around */
ui_menu_item_t* ui_get_vice_menu_item_by_hotkey(GdkModifierType mask,
                                                guint keysym);
gboolean        ui_set_vice_menu_item_hotkey(ui_menu_item_t *item,
                                             const char *keyval_name,
                                             GdkModifierType modifier);
gboolean        ui_set_vice_menu_item_hotkey_by_action(int action,
                                                       const char *keyval_name,
                                                       GdkModifierType modifier);
gboolean        ui_get_vice_menu_item_hotkey_by_action(int action,
                                                       guint *keysym,
                                                       GdkModifierType *modifier);
GtkWidget *     ui_get_gtk_menu_item_by_action(int action);
GtkWidget *     ui_get_gtk_menu_item_by_action_for_window(int action, int index);

void            ui_set_gtk_menu_item_accel_label(GtkWidget *item, int action);


/* for 'live' changing of hotkeys */
GtkWidget *     ui_get_gtk_menu_item_by_hotkey(GdkModifierType mask, guint keyval);
void            ui_clear_vice_menu_item_hotkeys(void);



ui_menu_item_t *ui_get_vice_menu_item_by_action(int action);

#endif /* VICE_UIMACHINEMENU_H */
