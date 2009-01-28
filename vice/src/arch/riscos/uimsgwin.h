/*
 * uimsgwin.h - all windows for displaying / editing text.
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

#ifndef VICE_UIMSGWIN_H
#define VICE_UIMSGWIN_H


struct wimp_msg_desc_s;

struct text_window_s;

typedef enum {
  msg_win_monitor,
  msg_win_license,
  msg_win_warranty,
  msg_win_contrib,
  msg_win_log,
  msg_win_NUMBER
} message_window_e;


/* return columns and rows of a message string */
void ui_message_get_dimensions(const char *msg, int *cols, int *rows);

/* Concerning single windows */
int  ui_message_window_open(message_window_e mwin, const char *title, const char *message, int cols, int rows);
int  ui_message_window_is_open(message_window_e mwin);
int  ui_message_window_busy(message_window_e mwin, int busy);
int  ui_message_window_is_busy(message_window_e mwin);
int  ui_message_window_close(message_window_e mwin);
int  ui_message_window_close_request(message_window_e mwin);
int  ui_message_window_destroy(message_window_e mwin);
const char *ui_message_window_get_last_command(message_window_e mwin);

/* All windows */
void ui_message_init(void);
void ui_message_exit(void);
message_window_e ui_message_window_for_handle(int handle);
struct text_window_s *ui_message_get_text_window(message_window_e mwin);
int  ui_message_need_null_event(void);
int  ui_message_process_event(int event, int *wimpblock);

void ui_message_init_messages(struct wimp_msg_desc_s *msg);
void ui_message_other_clicked(void);

#endif
