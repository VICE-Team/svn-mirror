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

GtkWidget * ui_menu_submenu_create(GtkWidget *bar, const char *label);
GtkWidget * ui_menu_add(GtkWidget *menu, ui_menu_item_t *items);
GtkWidget * ui_get_gtk_submenu_item_by_name(GtkWidget *submenu,
                                            const char *name);
GtkWidget * ui_get_gtk_submenu_item_by_hotkey(GtkWidget *submenu,
                                              GdkModifierType mask,
                                              guint keyval);
void        ui_set_gtk_check_menu_item_blocked(GtkWidget *item,
                                               gboolean state);
void        ui_set_gtk_check_menu_item_blocked_by_name(const char *name,
                                                       gboolean state);
void        ui_set_gtk_check_menu_item_blocked_by_resource(const char *name,
                                                           const char *resource);



/* FIXME: is this still even used? */
void ui_menu_init_accelerators(GtkWidget *window);

#endif
