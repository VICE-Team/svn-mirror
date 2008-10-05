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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "vice.h"
#include "ui.h"

typedef GtkWidget *ui_window_t;
typedef GCallback ui_callback_t;
typedef gpointer ui_callback_data_t;
enum ui_keysym_s {
  KEYSYM_NONE = 0,
  KEYSYM_0 = GDK_0,
  KEYSYM_1 = GDK_1,
  KEYSYM_2 = GDK_2,
  KEYSYM_3 = GDK_3,
  KEYSYM_4 = GDK_4,
  KEYSYM_5 = GDK_5,
  KEYSYM_6 = GDK_6,
  KEYSYM_7 = GDK_7,
  KEYSYM_8 = GDK_8,
  KEYSYM_9 = GDK_9,
  KEYSYM_a = GDK_a,
  KEYSYM_b = GDK_b,
  KEYSYM_c = GDK_c,
  KEYSYM_d = GDK_d,
  KEYSYM_e = GDK_e,
  KEYSYM_f = GDK_f,
  KEYSYM_h = GDK_h,
  KEYSYM_i = GDK_i,
  KEYSYM_j = GDK_j,
  KEYSYM_k = GDK_k,
  KEYSYM_l = GDK_l,
  KEYSYM_m = GDK_m,
  KEYSYM_n = GDK_n,
  KEYSYM_N = GDK_N,
  KEYSYM_p = GDK_p,
  KEYSYM_q = GDK_q,
  KEYSYM_s = GDK_s,
  KEYSYM_t = GDK_t,
  KEYSYM_u = GDK_u,
  KEYSYM_w = GDK_w,
  KEYSYM_z = GDK_z,
  KEYSYM_F9  = GDK_F9 ,
  KEYSYM_F10 = GDK_F10,
  KEYSYM_F11 = GDK_F11,
  KEYSYM_F12 = GDK_F12,
};
typedef enum ui_keysym_s ui_keysym_t;

#define UI_CALLBACK(name) \
    void name(GtkWidget *w, ui_callback_data_t event_data)

#define CHECK_MENUS (((ui_menu_cb_obj*)event_data)->status != CB_NORMAL)
#define UI_MENU_CB_PARAM (((ui_menu_cb_obj*)event_data)->value) 

extern GtkWidget *_ui_top_level;
extern GdkVisual *visual;
extern GtkWidget *canvasw;

int ui_open_canvas_window(struct video_canvas_s *c, const char *title, 
			  int width, int heigth, int no_autorepeat);
void ui_resize_canvas_window(struct video_canvas_s *c, int height, int width);
GtkWidget *ui_create_transient_shell(GtkWidget *parent, const char *name);
void ui_popdown(GtkWidget *w);
void ui_popup(GtkWidget *w, const char *title, gboolean wait_popdown);
void ui_make_window_transient(GtkWidget *parent,GtkWidget *window);
void ui_about(gpointer data);
gint ui_hotkey_event_handler(GtkWidget *w, GdkEvent *report, gpointer gp);
void ui_block_shells(void);
void ui_unblock_shells(void);
int ui_fullscreen_statusbar(struct video_canvas_s *canvas, int enable);

#endif /* !defined (_UIARCH_H) */
