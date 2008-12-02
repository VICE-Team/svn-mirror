/*
 * input_gp2x.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
 *  Mustafa 'GnoStiC' Tufan <mtufan@gmail.com>
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

#include "minimal.h"
#include "joystick.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "vkeyboard_gp2x.h"
#include "joystick.h"
#include "audio_gp2x.h"
#include "prefs_gp2x.h"
#include "ui.h"
#include "resources.h"
#include "keycodes.h"

//usbjoystick
unsigned int cur_portusb1=2;
unsigned int cur_portusb2=2;

//gp2x joystick
unsigned int cur_port=2;

unsigned int input_up=0;
unsigned int input_down=0;
unsigned int input_left=0;
unsigned int input_right=0;
unsigned int input_a=0;
unsigned int input_b=0;
unsigned int input_x=0;
unsigned int input_y=0;
unsigned int input_select=0;
unsigned int input_start=0;

int input_left_last;
int input_right_last;
unsigned int input_a_last;
unsigned int input_b_last;
unsigned int input_x_last;
extern unsigned char keybuffer[64];
int pos = 25;

void gp2x_poll_input() {
	static int stats_button;
	static int prefs_button;
	static int vkeyb_button;
	static int joy_time=0;
	static unsigned int a_held = 0;
	static unsigned int x_held = 0;
	unsigned int i;

	unsigned int joy_state=gp2x_joystick_read(0);

	/* start */
	if (joy_state&GP2X_START) input_start=1;

	/* prefs */
	if (!(joy_state&GP2X_SELECT)) prefs_button=0;
	if (joy_state&GP2X_SELECT && !prefs_button) {
		prefs_open=~prefs_open;
		prefs_button=1;
	}

	/* vkeyb */
	if (!(joy_state&GP2X_L)) vkeyb_button=0;
	if (joy_state&GP2X_L && !vkeyb_button) {
		vkeyb_open=~vkeyb_open;
		vkeyb_button=1;
	}

	if (prefs_open||vkeyb_open) {
		int rrate;
		resources_get_int("RefreshRate", &rrate);
		if (!rrate) rrate++;
		if (joy_state&(GP2X_UP|GP2X_DOWN|GP2X_LEFT|GP2X_RIGHT)) joy_time+=rrate;
		else joy_time=0;
		if (joy_time==rrate || (joy_time>6)) {
			if (joy_time>4) joy_time=rrate;
			input_up=joy_state&GP2X_UP;
			input_down=joy_state&GP2X_DOWN;
			input_select=joy_state&GP2X_SELECT;
			input_left=joy_state&GP2X_LEFT;
			input_right=joy_state&GP2X_RIGHT;
		}

		/* B button */
		if ((joy_state&(GP2X_B|GP2X_PUSH))!=input_b_last) {
			input_b=joy_state&(GP2X_B|GP2X_PUSH);
			input_b_last=input_b;
		}
		/* A button */
		if ((joy_state&GP2X_A)!=input_a_last) {
			input_a=joy_state&GP2X_A;
			input_a_last=input_a;
		}
		/* X button */
		if ((joy_state&GP2X_X)!=input_x_last) {
			input_x=joy_state&GP2X_X;
			input_x_last=input_x;
		}
		/* Y button */
		if (!(joy_state&GP2X_Y)) input_y=0; 
		if ((joy_state&GP2X_Y) && !input_y) input_y=1;
	}

	/* stats */
	if (!(joy_state&GP2X_R)) stats_button=0;
	if (joy_state&GP2X_R && !stats_button) {
		stats_open=~stats_open;
		stats_button=1;
	}

#if 0
	/* space */
	if ((joy_state&GP2X_X)&&(!x_held)) {
		x_held=1;
		keyboard_set_keyarr(7,4,1);
	} else if (x_held&&(!(joy_state&GP2X_X))) {
		x_held=0;
		keyboard_set_keyarr(7,4,0);
	}
#endif

	/* A */
	if (mapped_key_a) {
	if ((joy_state & GP2X_A) && (!a_held)) {
		a_held = 1;
		keyboard_set_keyarr (mapped_key_a_row,mapped_key_a_col,1);
	} else if (a_held && (!(joy_state & GP2X_A))) {
		a_held = 0;
		keyboard_set_keyarr (mapped_key_a_row,mapped_key_a_col,0);
	}
	}

	/* X */
	if (mapped_key_x) {
	if ((joy_state & GP2X_X) && (!x_held)) {
		x_held = 1;
		keyboard_set_keyarr (mapped_key_x_row,mapped_key_x_col,1);
	} else if (x_held && (!(joy_state & GP2X_X))) {
		x_held = 0;
		keyboard_set_keyarr (mapped_key_x_row,mapped_key_x_col,0);
	}
	}

//volume
	if (gp2x_joystick_read(0) & GP2X_VOL_UP) volume_up();
	else if (gp2x_joystick_read(0) & GP2X_VOL_DOWN) volume_down();

	if (prefs_open||vkeyb_open) return;

//gp2x joystick
	BYTE j=joystick_value[cur_port];
	if (joy_state&GP2X_UP) j|=0x01;
	else j&=~0x01;
	if (joy_state&GP2X_DOWN) j|=0x02;
	else j&=~0x02;
	if (joy_state&GP2X_LEFT) j|=0x04;
	else j&=~0x04;
	if (joy_state&GP2X_RIGHT) j|=0x08;
	else j&=~0x08;
	if (joy_state&(GP2X_B|GP2X_PUSH)) j|=0x10; /* fire */
	else j&=~0x10;
	joystick_value[cur_port]=j;

//mapped
	if (mapped_up)    { if (joy_state&mapped_up)    { j|=0x01; joystick_value[cur_port]=j; }}
	if (mapped_down)  { if (joy_state&mapped_down)  { j|=0x02; joystick_value[cur_port]=j; }}
	if (mapped_left)  { if (joy_state&mapped_left)  { j|=0x04; joystick_value[cur_port]=j; }}
	if (mapped_right) { if (joy_state&mapped_right) { j|=0x08; joystick_value[cur_port]=j; }}

//usb joysticks
	if (gp2x_usbjoys > 0) {
		joy_state = gp2x_joystick_read(1);

		j=joystick_value[cur_portusb1];
		if (joy_state&GP2X_UP) j|=0x01;
		else j&=~0x01;
		if (joy_state&GP2X_DOWN) j|=0x02;
		else j&=~0x02;
		if (joy_state&GP2X_LEFT) j|=0x04;
		else j&=~0x04;
		if (joy_state&GP2X_RIGHT) j|=0x08;
		else j&=~0x08;
		if (joy_state&(GP2X_B|GP2X_PUSH)) j|=0x10; // fire
		else j&=~0x10;
		joystick_value[cur_portusb1]=j;

		if (gp2x_usbjoys > 1) {
			joy_state = gp2x_joystick_read(2);

			j=joystick_value[cur_portusb2];
			if (joy_state&GP2X_UP) j|=0x01;
			else j&=~0x01;
			if (joy_state&GP2X_DOWN) j|=0x02;
			else j&=~0x02;
			if (joy_state&GP2X_LEFT) j|=0x04;
			else j&=~0x04;
			if (joy_state&GP2X_RIGHT) j|=0x08;
			else j&=~0x08;
			if (joy_state&(GP2X_B|GP2X_PUSH)) j|=0x10; // fire
			else j&=~0x10;
			joystick_value[cur_portusb2]=j;
		}
	}

//usb keyboard
	for (i=0; i<gp2x_keyboard_readext(); ++i) {
		int keycode = keybuffer[i];
		if (keycode & 0x80) {
        		keycode &= 0x7f; // Remove release bit
                	keyboard_key_released((signed long)keycode);
		} else { 
                	keyboard_key_pressed((signed long)keycode);
		}
	}
}

void kbd_initialize_numpad_joykeys(int* joykeys) { }

signed long kbd_arch_keyname_to_keynum(char *keyname) {
	return (signed long)atoi(keyname);
}

const char *kbd_arch_keynum_to_keyname(signed long keynum) {
	static char keyname[20];

	memset(keyname, 0, 20);
	sprintf(keyname, "%li", keynum);
	return keyname;
}

void kbd_arch_init() {
	keyboard_clear_keymatrix();
}
