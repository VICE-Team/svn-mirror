/*
 * ui.h - RISC OS GUI.
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#ifndef _UI_RO_H
#define _UI_RO_H

#include "vice.h"

#include "resources.h"
#include "types.h"
#include "uiapi.h"


struct conf_iconid_s;
struct conf_item_s;
struct wimp_msg_desc_s;
struct disp_desc_s;

typedef unsigned int ui_window_t;

/* Function type for setting a value */
typedef int (*set_var_function)(const char *name, resource_value_t val);

extern int ui_init_named_app(const char *appname, const char *iconname);

extern void ui_poll(int dopoll);
extern int  ui_poll_core(int *block);

extern void ui_message(const char *format,...);
extern void ui_show_text(const char *title, const char *text, int width, int height);
extern void ui_exit(void);

extern void ui_display_speed(int percent, int framerate, int warp_flag);
extern void ui_toggle_drive_status(int state);
extern void ui_display_paused(int flag);
extern void ui_display_drive_track_int(int drive_number, int track_number);
extern void ui_display_drive_dir(int number, const char *dir_name);

extern void ui_open_emu_window(RO_Window *win, int *b);
extern void ui_close_emu_window(RO_Window *win, int *b);
extern void ui_show_emu_scale(void);

extern void ui_set_drive_leds(unsigned int led, int status);

extern void ui_toggle_truedrv_emulation(void);
extern void ui_toggle_sid_emulation(void);

extern void ui_activate_monitor(void);

extern void ui_set_sound_volume(void);

extern int  ui_flip_iterate_and_attach(int dir);

extern int  ui_make_last_screenshot(void);
extern int  ui_save_last_snapshot(void);
extern void ui_trigger_snapshot_load(void);

extern void ui_set_icons_grey(RO_Window *win, const struct conf_iconid_s *desc, int state);


extern struct wimp_msg_desc_s *ui_emulator_init_prologue(const char *icon);
extern int  ui_emulator_init_epilogue(struct wimp_msg_desc_s *msg);
extern int  ui_load_template(const char *tempname, RO_Window **wptr, struct wimp_msg_desc_s *msg);
extern void ui_create_emulator_menu(int *block);
extern int  ui_open_centered_or_raise_block(RO_Window *win, int *block);
extern void ui_setup_menu_display(const struct disp_desc_s *dd);
extern void ui_set_menu_display_core(const struct disp_desc_s *dd, set_var_function func, int number);
extern void ui_setup_config_item(struct conf_item_s *ci);
extern void ui_update_rom_names(void);

extern RO_Screen ScreenMode;

extern RO_Window *EmuWindow;
extern RO_Window *EmuPane;
extern RO_Window *ImgContWindow;
extern RO_Window *MessageWindow;
extern RO_Window *InfoWindow;
extern RO_Window *CreateDiscWindow;

extern RO_Caret LastCaret;

extern int EmuZoom;
extern int FrameBufferUpdate;
extern int ModeChanging;
extern int UseEigen;
extern int EmuPaused;
extern int SingleTasking;
extern int CycleBasedSound;

extern int DriveLEDStates[4];
extern int DriveTrackNumbers[2];

extern struct ui_machine_callback_s ViceMachineCallbacks;

#endif
