/*
 * vkeyboard_gp2x.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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

#include "uitext_gp2x.h"
#include "input_gp2x.h"
#include "prefs_gp2x.h"
#include "ui_gp2x.h"
#include <string.h>
#include "keyboard.h"

int cursor_x=15;
int cursor_y=20;
int vkeyb_x=64;
int vkeyb_y=160;

int gotkeypress=0;
int vkey_pressed=0;
int vkey_released=0;
int button_deselected=0;
int vkey_row;
int vkey_column;
int shift_gotkeypress=0;
int shift_vkey_pressed=0;
int shift_vkey_released=0;
int shift_button_deselected=0;
int shift_vkey_row;
int shift_vkey_column;

int kb_fg, kb_bg;
int cursor_fg, cursor_bg;

int initialised=0;
char **keyb;
int kb_height, kb_width;
int *keytable;
void (*ui_set_keyarr)(int);

void draw_vkeyb(unsigned char *screen) {
	int i;

	/* keyboard */
	for(i=0; keyb[i]; i++) 
        	draw_ascii_string(screen, display_width, vkeyb_x, vkeyb_y+(i*8), keyb[i], kb_fg, kb_bg);

	/* cursor */
	draw_ascii_string(screen, display_width, 8*cursor_x, 8*cursor_y, "+", cursor_fg, cursor_bg);

	if(input_down) {
		input_down=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_y+(kb_height*8)<240) {
				vkeyb_y+=8;
				cursor_y++;
			}
		} else {
			if(cursor_y<(240/8)-1) cursor_y++;
		}
	} else if(input_up) {
		input_up=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_y>0) {
				vkeyb_y-=8;
				cursor_y--;
			}
		} else {
			if(cursor_y>0) cursor_y--;
		}
	} 
	if(input_left) {
		input_left=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_x>0) {
				vkeyb_x-=8;
				cursor_x--;
			}
		} else {
			if(cursor_x>0) cursor_x--;
		}
	} else if(input_right) {
		input_right=0;
		if(input_b&&cursor_y==vkeyb_y/8) {
			if(vkeyb_x+(kb_width*8)<display_width) {
				vkeyb_x+=8;
				cursor_x++;
			}
		} else {
			if(cursor_x<(display_width/8)-1) cursor_x++;
		}
	}
	/* b=normal keypress */
	if(!input_b) button_deselected=1;
	if(!input_b && gotkeypress) {
		gotkeypress=0;
		vkey_pressed=0;
		vkey_released=1;
	} else if(input_b && button_deselected) {
		button_deselected=0;
		for(i=0; keytable[i]; i+=4) {
			if((cursor_x-(vkeyb_x/8)==keytable[i]) 
				&& (cursor_y-(vkeyb_y/8)==keytable[i+1])) {
#if 0
				vkey=keytable[i+2];
				if(shifted) vkey|=128;
#endif
				vkey_row=keytable[i+2];
				vkey_column=keytable[i+3];
				vkey_pressed=1;
				gotkeypress=1;
				break;
			}
		}
	}
	if(vkey_pressed) {
		vkey_pressed=0;
		keyboard_set_keyarr(vkey_row, vkey_column, 1);
	} else if(vkey_released) {
		vkey_released=0;
		keyboard_set_keyarr(vkey_row, vkey_column, 0);
	}

	/* y=shifted keypress */
	if(!input_y) shift_button_deselected=1;
	if(!input_y && shift_gotkeypress) {
		shift_gotkeypress=0;
		shift_vkey_pressed=0;
		shift_vkey_released=1;
	} else if(input_y && shift_button_deselected) {
		shift_button_deselected=0;
		for(i=0; keytable[i]; i+=4) {
			if((cursor_x-(vkeyb_x/8)==keytable[i]) 
				&& (cursor_y-(vkeyb_y/8)==keytable[i+1])) {
				shift_vkey_row=keytable[i+2];
				shift_vkey_column=keytable[i+3];
				shift_vkey_pressed=1;
				shift_gotkeypress=1;
				break;
			}
		}
	}
	if(shift_vkey_pressed) {
		shift_vkey_pressed=0;
		ui_set_keyarr(1);
		keyboard_set_keyarr(shift_vkey_row, shift_vkey_column, 1);
	} else if(shift_vkey_released) {
		shift_vkey_released=0;
		keyboard_set_keyarr(shift_vkey_row, shift_vkey_column, 0);
		ui_set_keyarr(0);
	}
}
