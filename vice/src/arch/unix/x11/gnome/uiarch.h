/*
 * uiarch.h 
 *
 * Written by
 *  Martin Pottendorfer
 *  Oliver Schaertel
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

#ifndef _UIARCH_H
#define _UIARCH_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h>

#include <gnome.h>
#include <gtk/gtk.h>

#include "vice.h"
#include "ui.h"

typedef GtkWidget *ui_window_t;
typedef GtkSignalFunc ui_callback_t;
typedef gpointer ui_callback_data_t;
typedef KeySym ui_keysym_t;

#define UI_CALLBACK(name) \
    void name(GtkWidget *w, ui_callback_data_t event_data)

#define CHECK_MENUS (((ui_menu_cb_obj*)event_data)->status != CB_NORMAL)
#define UI_MENU_CB_PARAM (((ui_menu_cb_obj*)event_data)->value) 

extern GtkWidget *_ui_top_level;
extern GdkVisual *visual;
extern GtkWidget *canvasw;

#ifndef GNOME_MENUS
void ui_set_left_menu(GtkWidget *w);
void ui_set_right_menu(GtkWidget *w);
#else
void ui_set_left_menu(GnomeUIInfo *w);
void ui_set_right_menu(GnomeUIInfo *w);
#endif
void ui_set_drive_menu(int drvnr, GtkWidget *w);
void ui_set_tape_menu(GtkWidget *w);
void ui_set_speedmenu(GtkWidget *w);

GtkWidget *ui_create_transient_shell(GtkWidget *parent, const char *name);
void ui_popdown(GtkWidget *w);
void ui_popup(GtkWidget *w, const char *title, Boolean wait_popdown);
void ui_make_window_transient(GtkWidget *parent,GtkWidget *window);
void ui_about(gpointer data);
gint ui_hotkey_event_handler(GtkWidget *w, GdkEvent *report, gpointer gp);
void ui_block_shells(void);
void ui_unblock_shells(void);

#endif /* !defined (_UIARCH_H) */
