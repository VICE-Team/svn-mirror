/*
 * ui.h - RISC OS GUI.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#include "ROlib.h"
#include "config.h"
#include "types.h"
#include "palette.h"
#include "uiapi.h"

typedef unsigned int ui_window_t;
typedef void (*ui_exposure_handler_t) (unsigned int width, unsigned int height);

extern int ui_init_named_app(const char *appname, const char *iconname);

extern void ui_poll(void);

extern void ui_message(const char *format,...);
extern void ui_show_text(const char *title, const char *text, int width, int height);
extern void ui_exit(void);

extern void ui_display_speed(int percent, int framerate, int warp_flag);
extern void ui_toggle_drive_status(int state);
extern void ui_display_paused(int flag);
extern void ui_display_drive_track_int(int drive_number, int track_number);

extern void ui_open_emu_window(int *b);
extern void ui_close_emu_window(int *b);

extern void ui_set_drive_leds(unsigned int led, int status);

extern void ui_toggle_truedrv_emulation(void);
extern void ui_toggle_sid_emulation(void);

extern void ui_activate_monitor(void);

extern void ui_set_sound_volume(void);



extern RO_Screen ScreenMode;

extern RO_Window *EmuWindow;
extern RO_Window *EmuPane;
extern RO_Window *ImgContWindow;
extern RO_Caret LastCaret;

extern int EmuZoom;
extern int LastPoll;
extern int LastSpeed;
extern int FrameBufferUpdate;
extern int ModeChanging;
extern int UseEigen;
extern int EmuPaused;
extern int SingleTasking;
extern int RelativeSpeed;

extern char *PetModelName;
extern char *CBM2ModelName;

extern int DriveLEDStates[4];
extern int DriveTrackNumbers[2];


#endif
