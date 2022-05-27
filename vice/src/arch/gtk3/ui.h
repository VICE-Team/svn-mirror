/** \file   ui.h
 * \brief   Main Gtk3 UI code - header
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
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
 *
 */

#ifndef VICE_UI_H
#define VICE_UI_H

#include "vice.h"

#include <gtk/gtk.h>

#include "videoarch.h"
#include "palette.h"


/** \brief  Number of GtkWindow's in the ui_resources
 */
#define NUM_WINDOWS 3


/** \brief  Window indices
 */
enum {
    PRIMARY_WINDOW,     /**< primary window, all emulators */
    SECONDARY_WINDOW,   /**< secondary window, C128's VDC */
    MONITOR_WINDOW      /**< optional monitor window/terminal */
};


/** \brief  Number of drag-n-drop targets
 */
#define UI_DRAG_TARGETS_COUNT   3


/** \brief  Drag-n-drop 'target' types
 */
enum {
    DT_TEXT,        /**< simple text (text/plain) */
    DT_URI,         /**< haven't seen this one get triggered (yet) */
    DT_URI_LIST     /**< used by Windows Explorer / macOS Finder */
};


/** \brief  Encode unit and drive number for an attach/detach callback argument
 *
 * Encodes unit and drive number into a value to be used as a data argument
 * for event handlers.
 *
 * \param[in]   U   unit number (8-11)
 * \param[in]   D   drive number (0 or 1)
 */
#define UNIT_DRIVE_TO_PTR(U, D) GINT_TO_POINTER(((U) << 8) | ((D) & 0xff))


extern GtkTargetEntry ui_drag_targets[UI_DRAG_TARGETS_COUNT];


/* ------------------------------------------------------------------------- */
/* Prototypes */

void ui_set_handle_dropped_files_func(int (*func)(const char *));
void ui_set_create_window_func(void (*func)(video_canvas_t *));
void ui_set_identify_canvas_func(int (*func)(video_canvas_t *));
void ui_set_create_controls_widget_func(GtkWidget *(*func)(int));

void ui_create_main_window(video_canvas_t *canvas);
void ui_display_main_window(int index);
void ui_destroy_main_window(int index);

void ui_dispatch_events(void);
void ui_exit(void);

gboolean ui_is_fullscreen(void);
gboolean ui_is_fullscreen_from_canvas(const video_canvas_t *canvas);
void     ui_set_fullscreen_enabled(gboolean enabled);
gboolean ui_fullscreen_has_decorations(void);
void     ui_trigger_resize(void);
void     ui_update_fullscreen_decorations(void);

GtkWindow *ui_get_active_window(void);
video_canvas_t *ui_get_active_canvas(void);

/*
 * New pause 'API'
 */
int  ui_pause_active(void);
void ui_pause_enable(void);
bool ui_pause_loop_iteration(void);
void ui_pause_enter_monitor(void);
void ui_pause_disable(void);
void ui_pause_toggle(void);

/*
 * UI 'actions' for the custom hotkeys, perhaps move to uicommands.c/uiactions.c?
 */
gboolean ui_action_toggle_pause(void);
gboolean ui_action_toggle_warp(void);
gboolean ui_action_advance_frame(void);
void     ui_action_set_speed(int speed);
void     ui_action_set_fps(int fps);

gboolean ui_action_toggle_show_statusbar(void);

gboolean ui_speed_custom_toggled(GtkWidget *widget, gpointer data);
gboolean ui_fps_custom_toggled(GtkWidget *widget, gpointer data);
gboolean ui_cpu_speed_callback(GtkWidget *widget, gpointer data);
gboolean ui_fps_callback(GtkWidget *widget, gpointer data);

void ui_update_lightpen(void);
void ui_enable_crt_controls(int enabled);
void ui_enable_mixer_controls(int enabled);

GtkWidget *ui_get_window_by_index(int index);
int ui_get_window_index(GtkWidget *widget);
int ui_get_main_window_index(void);
GtkWidget *ui_get_main_window_by_index(gint index);

gboolean ui_get_autostart_on_doubleclick(void);

#endif
