/*
 * uimenu.h - Simple and ugly cascaded pop-up menu implementation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * Support for multiple visuals and depths by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#ifndef _UIMENU_H
#define _UIMENU_H

#include "ui.h"
#include "uihotkey.h"

typedef struct _ui_menu_entry {
    char *string;
    ui_callback_t callback;
    ui_callback_data_t callback_data;
    struct _ui_menu_entry *sub_menu;
    KeySym hotkey_keysym;
    ui_hotkey_modifier_t hotkey_modifier;
} ui_menu_entry_t;

extern ui_menu_entry_t ui_menu_separator[];

extern int ui_menu_init(XtAppContext app_context, Display *display, int screen);
extern Widget ui_menu_create(const char *name, ...);
extern void ui_menu_set_sensitive(Widget w, int flag);
extern void ui_menu_set_tick(Widget w, int flag);
extern int ui_menu_any_open(void);
extern void ui_menu_update_all(void);

/* These are the callbacks for the toggle and radio menus (the ones with a
   checkmark on the left).  If call_data is NULL, they simply set/unset the
   checkmark according to the value of the corrisponding resource.  If not
   NULL, they set the value of the corresponding resource before doing so.
   For this reason, to update the checkmarks, we simply have to call all the
   callbacks with a NULL `call_data' parameter.  */

#define UI_MENU_DEFINE_TOGGLE(resource)					    \
    static UI_CALLBACK(toggle_##resource)				    \
    {									    \
        int current_value;						    \
									    \
        if (resources_get_value(#resource,				    \
                                (resource_value_t *) &current_value) < 0)   \
           return;							    \
	if (!call_data) {						    \
            printf("%s: Toggling resource `%s' %d -> %d\n",		    \
                   __FUNCTION__, #resource, current_value, !current_value); \
            resources_set_value(#resource,				    \
                                (resource_value_t) !current_value);	    \
	    ui_update_menus();						    \
	} else {							    \
	    ui_menu_set_tick(w, current_value);				    \
	}								    \
    }

#define UI_MENU_DEFINE_RADIO(resource)					     \
    static UI_CALLBACK(radio_##resource)				     \
    {									     \
        int current_value;						     \
									     \
        resources_get_value(#resource, (resource_value_t *) &current_value); \
        if (!call_data) {						     \
            if (current_value != (int) client_data) {			     \
                resources_set_value(#resource,				     \
                                    (resource_value_t) client_data);	     \
                ui_update_menus();					     \
            }								     \
        } else {							     \
            ui_menu_set_tick(w, current_value == (int) client_data);	     \
        }								     \
    }

#endif /* _UIMENU_H */
