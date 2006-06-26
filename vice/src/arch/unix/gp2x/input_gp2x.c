/*
 * input_gp2x.c
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

#include "minimal.h"
#include "joystick.h"
#include "kbdbuf.h"
#include "keyboard.h"
#include "joystick.h"
#include "audio_gp2x.h"
#include "prefs_gp2x.h"
#include "ui.h"
#include "resources.h"

int cur_port=2;

int input_up=0;
int input_down=0;
int input_left=0;
int input_left_last;
int input_right=0;
int input_right_last;
int input_a=0;
int input_b=0;
int input_b_last;
int input_x=0;
int input_x_last;
int input_y=0;
int input_select=0;

void gp2x_poll_input() {
	static int swap_button;
	static int stats_button;
	static int prefs_button;
	static int vkeyb_button;
#if 0
	static int choose_button;
#endif
	static int joy_time=0;
	static int a_held=0;
	static int b_held=0;
	static int x_held=0;
	static int y_held=0;

	unsigned int joy_state=gp2x_joystick_read();

	/* prefs */
	if(!(joy_state&GP2X_SELECT)) prefs_button=0;
	if(joy_state&GP2X_SELECT && !prefs_button) {
		prefs_open=~prefs_open;
#if 0
		ui_pause_emulation(1);
#endif
		prefs_button=1;
	}

	/* vkeyb */
	if(!(joy_state&GP2X_L)) vkeyb_button=0;
	if(joy_state&GP2X_L && !vkeyb_button) {
		vkeyb_open=~vkeyb_open;
		vkeyb_button=1;
	}

	if(prefs_open||vkeyb_open) {
		int rrate;
		resources_get_value("RefreshRate", (void *)&rrate);
		if(!rrate) rrate++;
		if(joy_state&(GP2X_UP|GP2X_DOWN|GP2X_LEFT|GP2X_RIGHT)) joy_time+=rrate;
		else joy_time=0;
#if 0
		if(joy_time==rrate || !(joy_time%(4+rrate)) || joy_time>35) {
#endif
		if(joy_time==rrate || (joy_time>6)) {
			if(joy_time>4) joy_time==rrate;
			input_up=joy_state&GP2X_UP;
			input_down=joy_state&GP2X_DOWN;
			input_select=joy_state&GP2X_SELECT;
			input_left=joy_state&GP2X_LEFT;
			input_right=joy_state&GP2X_RIGHT;
		}

#if 0
		/* B button */
		if(!(joy_state&(GP2X_B|GP2X_PUSH))) {
			input_b=0;
			input_b_deselected=1;
		} else if(input_b_deselected && (joy_state&(GP2X_B|GP2X_PUSH))) {
			input_b=1;
		}
#endif
		/* B button */
		if((joy_state&(GP2X_B|GP2X_PUSH))!=input_b_last) {
			input_b=joy_state&(GP2X_B|GP2X_PUSH);
			input_b_last=input_b;
		}
		/* X button */
		if((joy_state&GP2X_X)!=input_x_last) {
			input_x=joy_state&GP2X_X;
			input_x_last=input_x;
		}
		
		/* Y button */
		if(!(joy_state&GP2X_Y)) input_y=0; 
		if((joy_state&GP2X_Y) && !input_y) input_y=1;
	}

	/* stats */
	if(!(joy_state&GP2X_R)) stats_button=0;
	if(joy_state&GP2X_R && !stats_button) {
		stats_open=~stats_open;
		stats_button=1;
	}

	/* space */
	if((joy_state&GP2X_X)&&(!x_held)) {
		x_held=1;
		keyboard_set_keyarr(7,4,1);
	} else if(x_held&&(!(joy_state&GP2X_X))) {
		x_held=0;
		keyboard_set_keyarr(7,4,0);
	}

#if 0
	/* a */
	if((joy_state&GP2X_A)&&!a_held) {
		a_held=1;
		keyboard_set_keyarr(1,2,1);
	} else if(a_held&&(!(joy_state&GP2X_A))) {
		a_held=0;
		keyboard_set_keyarr(1,2,0);
	}

	if((joy_state&GP2X_Y)&&!y_held) {
		y_held=1;
		keyboard_set_keyarr(1,7,1);
	} else if(y_held&&(!(joy_state&GP2X_Y))) {
		y_held=0;
		keyboard_set_keyarr(1,7,0);
	}
#endif

	if(gp2x_joystick_read()&GP2X_VOL_UP) volume_up();
	else if(gp2x_joystick_read()&GP2X_VOL_DOWN) volume_down();

	if(prefs_open||vkeyb_open) return;

#if 0
	BYTE j=0xff;
	unsigned int joy_state=gp2x_joystick_read();
	if(joy_state&GP2X_UP) j&=0xfe;
	else if(joy_state&GP2X_DOWN) j&=0xfd;
	if(joy_state&GP2X_LEFT) j&=0xfb;
	else if(joy_state&GP2X_RIGHT) j&=0xf7;
	if(joy_state&GP2X_B) j&=0xef; /* fire */
      joystick_value[cur_port]=~j;
#endif

	BYTE j=joystick_value[cur_port];
	if(joy_state&GP2X_UP) j|=0x01;
	else j&=~0x01;
	if(joy_state&GP2X_DOWN) j|=0x02;
	else j&=~0x02;
	if(joy_state&GP2X_LEFT) j|=0x04;
	else j&=~0x04;
	if(joy_state&GP2X_RIGHT) j|=0x08;
	else j&=~0x08;
	if(joy_state&(GP2X_B|GP2X_PUSH)) j|=0x10; /* fire */
	else j&=~0x10;
	joystick_value[cur_port]=j;
}
