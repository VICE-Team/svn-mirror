/*
 * ui.h - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
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

#ifndef _UI_XAW_H
#define _UI_XAW_H

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "vice.h"
#include "types.h"
#include "palette.h"

/* If this is #defined, `Alt' is handled the same as `Meta'.  On
   systems which have Meta, it's better to use Meta instead of Alt as
   a shortcut modifier (because Alt is usually used by Window
   Managers), but systems that don't have Meta (eg. GNU/Linux, HP-UX)
   would suffer then.  So it's easier to just handle Meta as Alt in
   such cases.  */
#define ALT_AS_META

/* If this is #defined, `Mode_switch' is handled the same as `Meta'.  */
/* #define MODE_SWITCH_AS_META */

typedef Widget ui_window_t;

typedef void (*ui_exposure_handler_t) (unsigned int width,
                                       unsigned int height);

typedef enum {
    UI_BUTTON_NONE, UI_BUTTON_CLOSE, UI_BUTTON_OK, UI_BUTTON_CANCEL,
    UI_BUTTON_YES, UI_BUTTON_NO, UI_BUTTON_RESET, UI_BUTTON_HARDRESET,
    UI_BUTTON_MON, UI_BUTTON_DEBUG, UI_BUTTON_CONTENTS, UI_BUTTON_AUTOSTART
} ui_button_t;

typedef enum {
    UI_JAM_RESET, UI_JAM_HARD_RESET, UI_JAM_MONITOR, UI_JAM_DEBUG
} ui_jam_action_t;

typedef XtCallbackProc ui_callback_t;
typedef XtPointer ui_callback_data_t;

#define UI_CALLBACK(name)                               \
    void name(Widget w, ui_callback_data_t client_data, \
              ui_callback_data_t call_data)

/* These resources are used only by the UI modules.  */
typedef struct {
    char *html_browser_command;
    int use_private_colormap;
    int save_resources_on_exit;
    int depth;
} _ui_resources_t;
extern _ui_resources_t _ui_resources;

extern Widget _ui_top_level;

extern Display *display;
extern int screen;
extern Visual *visual;
extern int depth;

/* ------------------------------------------------------------------------- */
#ifdef USE_VIDMODE_EXTENSION
typedef struct {
  int modeindex;
  char name[17];
} ui_bestvideomode;
#endif
/* ------------------------------------------------------------------------- */

/* This is used by `ui_enable_drive_status()'.  */
typedef enum {
    UI_DRIVE_ENABLE_NONE = 0,
    UI_DRIVE_ENABLE_0 = 1 << 0,
    UI_DRIVE_ENABLE_1 = 1 << 1,
    UI_DRIVE_ENABLE_2 = 1 << 2,
    UI_DRIVE_ENABLE_3 = 1 << 3
} ui_drive_enable_t;

/* ------------------------------------------------------------------------- */

int ui_init_resources(void);
int ui_init_cmdline_options(void);

int ui_init(int *argc, char **argv);
int ui_init_finish(void);
void ui_set_left_menu(Widget w);
void ui_set_right_menu(Widget w);
void ui_set_application_icon(Pixmap icon_pixmap);
ui_window_t ui_open_canvas_window(const char *title, int width, int height, int no_autorepeat, ui_exposure_handler_t exposure_proc, const palette_t *p, PIXEL pixel_return[]);
void ui_resize_canvas_window(ui_window_t w, int height, int width);
void ui_map_canvas_window(ui_window_t w);
void ui_unmap_canvas_window(ui_window_t w);
Window ui_canvas_get_drawable(ui_window_t w);
int ui_canvas_set_palette(ui_window_t w, const palette_t *palette,
                          PIXEL *pixel_return);
void ui_display_speed(float percent, float framerate, int warp_flag);
void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color);
void ui_display_drive_track(int drive_number, double track_number);
void ui_display_drive_led(int drive_number, int status);
void ui_display_paused(int flag);
void ui_dispatch_next_event(void);
void ui_dispatch_events(void);
void ui_error(const char *format,...);
void ui_exit(void);
ui_jam_action_t ui_jam_dialog(const char *format,...);
void ui_message(const char *format,...);
void ui_show_text(const char *title, const char *text, int width, int height);
char *ui_select_file(const char *title, char *(*read_contents_func)(const char *), int allow_autostart, const char *default_dir, const char *default_pattern, ui_button_t *button_return);
ui_button_t ui_input_string(const char *title, const char *prompt, char *buf, unsigned int buflen);
ui_button_t ui_ask_confirmation(const char *title, const char *text);
void ui_autorepeat_on(void);
void ui_autorepeat_off(void);
void ui_update_menus(void);
int ui_extend_image_dialog(void);
Widget ui_create_transient_shell(Widget parent, const char *name);
void ui_popdown(Widget w);
void ui_popup(Widget w, const char *title, Boolean wait_popdown);
void ui_pause_emulation(int flag);
int ui_emulation_is_paused(void);
void ui_create_dynamic_menues(void);
void ui_check_mouse_cursor(void);

#ifdef USE_VIDMODE_EXTENSION
int ui_set_windowmode(void);
int ui_set_fullscreenmode(void);
void ui_set_fullscreenmode_init(void);
void ui_set_mouse_timeout(void);
int ui_is_fullscreen_available();
#endif

#endif /* !defined (_UI_XAW_H) */
