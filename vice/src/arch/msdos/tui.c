/*
 * tui.c - A (very) simple text-based user interface.
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

#include "vice.h"

#include <sys/farptr.h>
#include <go32.h>
#include <strings.h>
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <pc.h>
#include <keys.h>
#include <stdarg.h>

#include <allegro.h>

#include "tui.h"
#include "patchlevel.h"
#include "utils.h"

typedef BYTE attr_t;
attr_t current_attr;

struct tui_area {
    int width, height;
    BYTE *mem;
};

struct text_info text_mode_info;

/* ------------------------------------------------------------------------- */

static attr_t make_attr(int foreground_color, int background_color, int blink)
{
    return foreground_color | (background_color << 4) | (blink ? 0x80 : 0);
}

static unsigned long screen_addr(int x, int y)
{
    return 0xb8000 + 2 * (x + y * text_mode_info.screenwidth);
}

/* ------------------------------------------------------------------------- */

void tui_init(void)
{
    set_gfx_mode(GFX_TEXT, 80, 25, 0, 0);

    /* FIXME: this should be made more flexible, to handle other screen modes
       automatically. */
    _set_screen_lines(25);
    
    gettextinfo(&text_mode_info);
    /* _setcursortype(_NOCURSOR); */
}

int tui_num_lines(void)
{
    if (text_mode_info.screenheight == 0)
	tui_init();
    return text_mode_info.screenheight;
}

int tui_num_cols(void)
{
    if (text_mode_info.screenwidth == 0)
	tui_init();
    return text_mode_info.screenwidth;
}

void tui_set_attr(int foreground_color, int background_color,
			 int blink)
{
    current_attr = make_attr(foreground_color, background_color, blink);
}

void tui_put_char(int x, int y, BYTE c)
{
    unsigned long addr = screen_addr(x, y);
    BYTE attr_byte = (BYTE)current_attr;

    _farsetsel(_dos_ds);
    _farnspokeb(addr, c);
    _farnspokeb(addr + 1, attr_byte);
}

void tui_hline(int x, int y, BYTE c, int count)
{
    unsigned long addr = screen_addr(x, y);
    BYTE attr_byte = (BYTE)current_attr;
    int i;
    
    _farsetsel(_dos_ds);
    for (i = 0; i < count; i++) {
	_farnspokeb(addr, c);
	_farnspokeb(addr + 1, attr_byte);
	addr += 2;
    }
}

void tui_vline(int x, int y, BYTE c, int count)
{
    unsigned long addr = screen_addr(x, y);
    BYTE attr_byte = (BYTE)current_attr;
    int i;
    
    _farsetsel(_dos_ds);
    for (i = 0; i < count; i++) {
	_farnspokeb(addr, c);
	_farnspokeb(addr + 1, attr_byte);
	addr += tui_num_cols();
    }
}

void tui_gotoxy(int x, int y)
{
    gotoxy(x + 1, y + 1);
}

void tui_flush_keys(void)
{
    while (kbhit())
	getkey();
}

void tui_display(int x, int y, int len, const char *format, ...)
{
    BYTE attr_byte = (BYTE)current_attr;
    unsigned long addr = screen_addr(x, y);
    static char buf[4096];
    int i, buf_len;
    va_list vl;

    va_start(vl, format);
    buf_len = vsprintf(buf, format, vl);
    if (len == 0)
	len = buf_len;
    else if (buf_len > len)
        buf_len = len;
   
    _farsetsel(_dos_ds); 
    for (i = 0; i < buf_len; i++) {
        _farnspokeb(addr, buf[i]);
        _farnspokeb(addr + 1, attr_byte);
        addr += 2;
    }
    for (; i < len; i++) {
        _farnspokeb(addr, ' ');
        _farnspokeb(addr + 1, attr_byte);
        addr += 2;
    }
}

/* ------------------------------------------------------------------------- */

void tui_area_get(tui_area_t *a, int x, int y, int width, int height)
{
    BYTE *p;
    int i, j;
	
    if (*a == NULL) {
	*a = xmalloc(sizeof (struct tui_area));
	(*a)->mem = xmalloc(2 * width * height);
    } else {
	(*a)->mem = xrealloc((*a)->mem, 2 * width * height);
    }
    (*a)->width = width;
    (*a)->height = height;

    _farsetsel(_dos_ds);

    for (p = (*a)->mem, i = 0; i < height; i++) {
	int addr = screen_addr(x, y + i);
	
	for (j = 0; j < 2 * width; j++)
	    *(p++) = _farnspeekb(addr + j);
    }
}

void tui_area_put(tui_area_t a, int x, int y)
{
    BYTE *p = a->mem;
    int i, j;

    _farsetsel(_dos_ds);

    for (i = 0; i < a->height; i++) {
	int addr = screen_addr(x, y + i);

	for (j = 0; j < 2 * a->width; j++)
	    _farnspokeb(addr + j, *(p++));
    }
}

void tui_area_free(tui_area_t a)
{
    if (a != NULL) {
	free(a->mem);
	free(a);
    }
}

void tui_clear_screen(void)
{
    int i;
	
    _setcursortype(_NOCURSOR);

    tui_set_attr(FIRST_LINE_FORE, FIRST_LINE_BACK, 0);
    
#ifndef UNSTABLE
    tui_display(0, 0, tui_num_cols(), "VICE version %s", VERSION);
#else
    tui_display(0, 0, tui_num_cols(), "VICE version %s (unstable)", VERSION);
#endif
    
    tui_set_attr(BACKPATTERN_FORE, BACKPATTERN_BACK, 0);
    for (i = 1; i < tui_num_lines() - 1; i++)
	tui_hline(0, i, BACKCHAR, tui_num_cols());
}

void tui_make_shadow(int x, int y, int width, int height)
{
    int i, j;

    _farsetsel(_dos_ds);
    for (i = 0; i < height; i++) {
	int addr = screen_addr(x, y + i) + 1;
	    
	for (j = 0; j < width; j++, addr += 2) {
	    _farnspokeb(addr, make_attr(DARKGRAY, BLACK, 0));
	}
    }
}

void tui_display_window(int x, int y, int width, int height,
			int foreground_color, int background_color,
			const char *title, tui_area_t *backing_store)
{
    int i;

    if (backing_store != NULL)
	tui_area_get(backing_store, x, y, width + 2, height + 1);

    tui_make_shadow(x + 2, y + 1, width, height);
	
    tui_set_attr(foreground_color, background_color, 0);
    tui_put_char(x, y, 0xc9);
    tui_hline(x + 1, y, 0xcd, width - 2);
    tui_put_char(x + width - 1, y, 0xbb);
    tui_put_char(x, y + height - 1, 0xc8);
    tui_hline(x + 1, y + height - 1, 0xcd, width - 2);
    tui_put_char(x + width - 1, y + height - 1, 0xbc);
    for (i = 0; i < height - 2; i++) {
	tui_put_char(x, y + i + 1, 0xba);
	tui_hline(x + 1, y + i + 1, ' ', width - 2);
	tui_put_char(x + width - 1, y + i + 1, 0xba);
    }

    if (title != NULL && *title != '\0') {
	int title_x, title_length;

	title_length = strlen(title);
	title_x = x + (width - title_length - 4) / 2;
	tui_display(title_x, y, 0, "\x10 %s \x11", title);
    }
}

/* ------------------------------------------------------------------------- */

static int _tui_input_string(int x, int y, char *buf, int max_length,
			     int displayed_length, int foreground_color,
			     int background_color)
{
    int cursor_pos = 0;
    int scroll_offset = 0;
    int string_length = strlen(buf);
    int need_redisplay = 1;

    tui_set_attr(foreground_color, background_color, 0);
    _setcursortype(_SOLIDCURSOR);
    while (1) {
	int key;
	
	if (need_redisplay) {
	    tui_display(x, y, displayed_length, "%s", buf + scroll_offset);
	    need_redisplay = 0;
	}
	tui_gotoxy(x + cursor_pos - scroll_offset, y);
	key = getkey();
	switch (key) {
	  case K_Left:
	    if (cursor_pos > 0) {
		cursor_pos--;
		if (cursor_pos < scroll_offset) {
		    scroll_offset = cursor_pos;
		    need_redisplay = 1;
		}
	    }
	    break;
	  case K_Right:
	    if (cursor_pos < string_length) {
		cursor_pos++;
		if (cursor_pos >= (scroll_offset + displayed_length)) {
		    scroll_offset = cursor_pos - displayed_length + 1;
		    need_redisplay = 1;
		}
	    }
	    break;
	  case K_Home:
	    cursor_pos = 0;
	    scroll_offset = 0;
	    need_redisplay = 1;
	    break;
	  case K_End:
	    cursor_pos = string_length;
	    scroll_offset = cursor_pos - displayed_length + 1;
	    if (scroll_offset < 0)
		scroll_offset = 0;
	    need_redisplay = 1;
	    break;
	  case K_BackSpace:
	    if (cursor_pos > 0 && cursor_pos <= string_length) {
		memmove(buf + cursor_pos - 1, buf + cursor_pos,
			string_length - cursor_pos + 1);
		string_length--;
		cursor_pos--;
		if (cursor_pos < scroll_offset)
		    scroll_offset = cursor_pos;
		need_redisplay = 1;
	    }
	    break;
	  case K_Delete:
	    if (cursor_pos < string_length) {
		memmove(buf + cursor_pos, buf + cursor_pos + 1,
			string_length - cursor_pos);
		string_length--;
		need_redisplay = 1;
	    }
	    break;
	  case K_Return:
	    _setcursortype(_NOCURSOR);
	    return 0;
	  case K_Escape:
	    _setcursortype(_NOCURSOR);
	    return -1;
	  default:
	    if (key <= 0xff
		&& isprint((char) key)
		&& string_length < max_length) {
		memmove(buf + cursor_pos + 1, buf + cursor_pos,
			string_length - cursor_pos + 1);
		buf[cursor_pos] = (char) key;
		cursor_pos++;
		string_length++;
		if (cursor_pos - scroll_offset >= displayed_length)
		    scroll_offset = cursor_pos - displayed_length + 1;
		need_redisplay = 1;
	    }
	    break;
	}
    }
}

int tui_input_string(const char *title, const char *prompt, char *buf,
		     int buflen)
{
    int field_width, x, y, width, height, retval;
    tui_area_t backing_store = NULL;

    if (buflen + 1 < text_mode_info.screenwidth - 12)
	field_width = buflen + 1;
    else
	field_width = text_mode_info.screenwidth - 12;

    width = field_width + 4;
    height = 4;
    x = CENTER_X(width);
    y = CENTER_Y(height);

    tui_display_window(x, y, width, height, REQUESTER_BORDER, REQUESTER_BACK,
		       title, &backing_store);
    tui_set_attr(REQUESTER_FORE, REQUESTER_BACK, 0);
    tui_display(x + 2, y + 1, field_width, prompt);
    
    retval = _tui_input_string(x + 2, y + 2, buf, buflen, field_width,
			       FIELD_FORE, FIELD_BACK);

    tui_area_put(backing_store, x, y);
    tui_area_free(backing_store);

    return retval;
}

/* ------------------------------------------------------------------------- */

void tui_error(const char *format,...)
{
    int x, y, width, height;
    char str[1024];
    int str_length;
    va_list ap;
    tui_area_t backing_store = NULL;
	
    va_start(ap, format);
    vsprintf(str, format, ap);
    str_length = strlen(str);
    if (str_length > text_mode_info.screenwidth - 10) {
	str_length = text_mode_info.screenwidth - 10;
	str[str_length] = 0;
    }
    x = CENTER_X(str_length + 6);
    y = CENTER_Y(5);
    width = str_length + 6;
    height = 5;

    tui_display_window(x, y, width, height, ERROR_BORDER, ERROR_BACK, "Error!",
		       &backing_store);
    tui_set_attr(ERROR_FORE, ERROR_BACK, 0);
    tui_display(CENTER_X(str_length), y + 2, 0, str);
    getkey();

    tui_area_put(backing_store, x, y);
    tui_area_free(backing_store);
}

void tui_message(const char *format,...)
{
    int x, y, width, height;
    char str[1024];
    int str_length;
    va_list ap;
    tui_area_t backing_store = NULL;
	
    va_start(ap, format);
    vsprintf(str, format, ap);
    str_length = strlen(str);
    if (str_length > text_mode_info.screenwidth - 10) {
	str_length = text_mode_info.screenwidth - 10;
	str[str_length] = 0;
    }
    x = CENTER_X(str_length + 6);
    y = CENTER_Y(5);
    width = str_length + 6;
    height = 5;

    tui_display_window(x, y, width, height, MESSAGE_BORDER, MESSAGE_BACK,
		       NULL, &backing_store);
    tui_set_attr(MESSAGE_FORE, MESSAGE_BACK, 0);
    tui_display(CENTER_X(str_length), y + 2, 0, str);
    getkey();

    tui_area_put(backing_store, x, y);
    tui_area_free(backing_store);
}

