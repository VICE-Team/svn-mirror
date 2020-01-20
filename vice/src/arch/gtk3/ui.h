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

void ui_display_paused(int flag);

int  ui_is_fullscreen(void);
void ui_trigger_resize(void);
gboolean ui_fullscreen_callback(GtkWidget *widget, gpointer user_data);
gboolean ui_fullscreen_decorations_callback(GtkWidget *widget, gpointer user_data);

GtkWindow *ui_get_active_window(void);
video_canvas_t *ui_get_active_canvas(void);

/*
 * New pause 'API'
 */
int  ui_pause_active(void);
void ui_pause_enable(void);
void ui_pause_disable(void);
void ui_pause_toggle(void);

gboolean ui_toggle_pause(void);
gboolean ui_advance_frame(void);

void ui_update_lightpen(void);

void ui_enable_crt_controls(int enabled);
void ui_enable_mixer_controls(int enabled);

GtkWidget *ui_get_window_by_index(int index);
int ui_get_window_index(GtkWidget *widget);

#if 0
#define UI_DRAG_TARGETS_COUNT   3

extern GtkTargetEntry ui_drag_targets[UI_DRAG_TARGETS_COUNT];

gboolean ui_on_drag_drop(
        GtkWidget *widget,
        GdkDragContext *context,
        gint x,
        gint y,
        guint time,
        gpointer data);

void ui_on_drag_data_received(
        GtkWidget *widget,
        GdkDragContext *context,
        int x,
        int y,
        GtkSelectionData *data,
        guint info,
        guint time);
#endif

#endif
