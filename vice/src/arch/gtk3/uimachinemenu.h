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


GtkWidget * ui_machine_menu_bar_create(void);
void        ui_machine_menu_bar_vsid_patch(GtkWidget *menu);

/* hotkeys API */
ui_menu_item_t *ui_get_vice_menu_item_by_name(const char *name);
gboolean        ui_set_vice_menu_item_hotkey(ui_menu_item_t *item,
                                             const char *keyval_name,
                                             GdkModifierType modifier);
gboolean        ui_set_vice_menu_item_hotkey_by_name(const char *name,
                                                     const char *keyval_name,
                                                     GdkModifierType modifier);

GtkWidget *     ui_get_gtk_menu_item_by_name(const char *name);

void            ui_clear_vice_menu_item_hotkeys(void);

#endif /* VICE_UIMACHINEMENU_H */
