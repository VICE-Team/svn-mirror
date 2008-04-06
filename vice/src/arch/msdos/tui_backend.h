/*
 * tui_backend.h - MS-DOS backend for the text-based user interface.
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

#ifndef _TUI_BACKEND_H
#define _TUI_BACKEND_H

#include <conio.h>
#include <stdarg.h>

#include "tuifs.h"
#include "types.h"

#define BACKCHAR		' '
#define BACKPATTERN_FORE	LIGHTGRAY
#define BACKPATTERN_BACK	BLUE
#define FIRST_LINE_FORE		BLACK
#define FIRST_LINE_BACK		LIGHTGRAY
#define MENU_FORE		BLACK
#define MENU_BACK		LIGHTGRAY
#define MENU_BORDER		WHITE
#define MENU_TITLE		RED
#define MENU_HOTKEY		RED
#define MENU_HIGHLIGHT		CYAN
#define ERROR_FORE		YELLOW
#define ERROR_BACK		RED
#define ERROR_BORDER		YELLOW
#define MESSAGE_FORE		BLACK
#define MESSAGE_BACK		LIGHTGRAY
#define MESSAGE_BORDER		WHITE
#define FIELD_BACK		CYAN
#define FIELD_FORE		BLACK
#define REQUESTER_FORE		BLACK
#define REQUESTER_BACK		LIGHTGRAY
#define REQUESTER_BORDER	WHITE

typedef struct tui_area *tui_area_t;

void tui_init(void);
int tui_num_lines(void);
int tui_num_cols(void);
void tui_set_attr(int foreground_color, int background_color, int blink);
void tui_put_char(int x, int y, BYTE c);
void tui_clear_screen(void);
void tui_hline(int x, int y, BYTE c, int count);
void tui_vline(int x, int y, BYTE c, int count);
void tui_display_window(int x, int y, int width, int height, int fore,
                        int back, const char *title, tui_area_t *backing_store);
int tui_input_string(const char *title, const char *prompt, char *buf,
                     int buflen);
void tui_beep(void);
void tui_gotoxy(int x, int y);
void tui_flush_keys(void);
void tui_display(int x, int y, int len, const char *format, ...);
void tui_area_get(tui_area_t *a, int x, int y, int width, int height);
void tui_area_put(tui_area_t a, int x, int y);
void tui_area_free(tui_area_t a);
void tui_make_shadow(int x, int y, int width, int height);


#endif /* _TUI_BACKEND_H */
