/*
 * tuimenu.h - A (very) simple text-based menu.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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
 *
 */

#ifndef _TUI_MENU_H
#define _TUI_MENU_H

#include "resources.h"

#define TUI_MENU_HOT_KEY_PREFIX		'_'

typedef enum tui_menu_item_behavior {
    TUI_MENU_BEH_CONTINUE,	/* Stay in the menu. */
    TUI_MENU_BEH_CLOSE,		/* Close the current menu. */
    TUI_MENU_BEH_RESUME		/* Leave all the menus. */
} tui_menu_item_behavior_t;

/* Menu item callback: every menu item is associated with a function of this
   type.  `is_activated' is nonzero if the menu has been activated; the
   function must return a pointer to the parameter string to be displayed on
   the right of the menu item.  `behavior' defines the behavior of the item
   after it has been activated (default is `TUI_ITEM_BEH_CONTINUE'). */
typedef char *(*tui_menu_callback_t)(int been_activated, void *callback_param);

/* Menu type. */
typedef struct tui_menu *tui_menu_t;

/* Struct to define a menu item.  */
typedef struct _tui_menu_item_def_t tui_menu_item_def_t;
struct _tui_menu_item_def_t {
    const char *label;
    const char *help_string;
    tui_menu_callback_t callback;
    void *callback_param;
    int par_string_max_len;
    tui_menu_item_behavior_t behavior;
    tui_menu_item_def_t *submenu;
    const char *submenu_title;
};

tui_menu_t tui_menu_create(const char *title, int spacing);
void tui_menu_free(tui_menu_t menu);
void tui_menu_add_item(tui_menu_t menu, const char *label, const char *help_string, tui_menu_callback_t callback, void *callback_param, int par_string_max_len, tui_menu_item_behavior_t behavior);
void tui_menu_add_submenu(tui_menu_t menu, const char *label, const char *help_string, tui_menu_t submenu, tui_menu_callback_t callback, void *callback_param, int par_string_max_len);
void tui_menu_add_separator(tui_menu_t menu);
void tui_menu_add(tui_menu_t menu, const tui_menu_item_def_t *d);
int tui_menu_handle(tui_menu_t menu);
void tui_menu_update(tui_menu_t menu);

/* ------------------------------------------------------------------------- */

#define TUI_MENU_CALLBACK(name) \
    char *name(int been_activated, void *param)

#define TUI_MENU_DEFINE_TOGGLE(resource)                                     \
    static TUI_MENU_CALLBACK(toggle_##resource##_callback)                   \
    {                                                                        \
        int value, r;                                                        \
                                                                             \
        if (been_activated)                                                  \
            r = resources_toggle(#resource, (resource_value_t *) &value);    \
        else                                                                 \
            r = resources_get_value(#resource, (resource_value_t *) &value); \
                                                                             \
        if (r < 0)                                                           \
            return "Unknown";                                                \
        else                                                                 \
            return value ? "On" : "Off";                                     \
    }

#define TUI_MENU_DEFINE_RADIO(resource)                               \
    static TUI_MENU_CALLBACK(radio_##resource##_callback)             \
    {                                                                 \
        if (been_activated)                                           \
            resources_set_value(#resource, (resource_value_t) param); \
        return NULL;                                                  \
    }

#endif
