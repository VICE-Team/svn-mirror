/*
 * ui.h - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _UI_XAW_H
#define _UI_XAW_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "vice.h"

#include "types.h"
#include "uiapi.h"

/* If this is #defined, `Mode_switch' is handled the same as `Meta'.  */
/* #define MODE_SWITCH_AS_META */

typedef Widget ui_window_t;
typedef XtCallbackProc ui_callback_t;
typedef XtPointer ui_callback_data_t;

#define UI_CALLBACK(name)                               \
    void name(Widget w, ui_callback_data_t client_data, \
              ui_callback_data_t call_data)
/* fill callback variables */
#define CHECK_MENUS call_data
#define UI_MENU_CB_PARAM client_data

extern Widget _ui_top_level;
extern Display *display;
extern Visual *visual;

/* ------------------------------------------------------------------------- */
/* Prototypes */
extern void ui_set_left_menu(Widget w);
extern void ui_set_right_menu(Widget w);
extern void ui_set_drive8_menu(Widget w);
extern void ui_set_drive9_menu(Widget w);
extern void ui_set_tape_menu(Widget w);
extern Widget ui_create_shell(Widget parent, const char *name,
			      WidgetClass class);
extern Widget ui_create_transient_shell(Widget parent, const char *name);
extern void ui_popdown(Widget w);
extern void ui_popup(Widget w, const char *title, Boolean wait_popdown);
extern void ui_about (Widget w, ui_callback_data_t cd, ui_callback_data_t cl);

#ifdef USE_VIDMODE_EXTENSION
typedef struct {
  int modeindex;
  char name[17];
} ui_bestvideomode;

extern int ui_set_windowmode(void);
extern int ui_set_fullscreenmode(void);
extern void ui_set_fullscreenmode_init(void);
extern void ui_set_mouse_timeout(void);
extern int ui_is_fullscreen_available(void);
#endif /* !USE_VIDMODE_EXTENSION */

#endif /* !defined (_UI_XAW_H) */

