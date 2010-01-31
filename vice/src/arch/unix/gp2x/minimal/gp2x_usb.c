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

#include "gp2xsys.h"
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
extern unsigned int cur_portusb1;
extern unsigned int cur_portusb2;

int input_left_last;
int input_right_last;
unsigned int input_a_last;
unsigned int input_b_last;
unsigned int input_x_last;
extern unsigned char keybuffer[64];

void gp2x_poll_usb_input(void)
{
    static int stats_button;
    static int prefs_button;
    static int vkeyb_button;
    static int joy_time = 0;
    static unsigned int a_held = 0;
    static unsigned int x_held = 0;
    unsigned int i;
    unsigned int j;

    unsigned int joy_state = gp2x_joystick_read(0);

    //usb joysticks
    if (gp2x_usbjoys > 0) {
        joy_state = gp2x_joystick_read(1);

        j = joystick_value[cur_portusb1];
        if (joy_state & GP2X_UP) {
            j |= 0x01;
        } else {
            j &= ~0x01;
        }
        if (joy_state & GP2X_DOWN) {
            j |= 0x02;
        } else {
            j &= ~0x02;
        }
        if (joy_state & GP2X_LEFT) {
            j |= 0x04;
        } else {
            j &= ~0x04;
        }
        if (joy_state & GP2X_RIGHT) {
            j |= 0x08;
        } else {
            j &= ~0x08;
        }
        if (joy_state & (GP2X_B | GP2X_PUSH)) {
            j |= 0x10;
        } else {
            j &= ~0x10;
        }
        joystick_value[cur_portusb1] = j;

        if (gp2x_usbjoys > 1) {
            joy_state = gp2x_joystick_read(2);

            j = joystick_value[cur_portusb2];
            if (joy_state & GP2X_UP) {
                j |= 0x01;
            } else {
                j &= ~0x01;
            }
            if (joy_state & GP2X_DOWN) {
                j |= 0x02;
            } else {
                j &= ~0x02;
            }
            if (joy_state & GP2X_LEFT) {
                j |= 0x04;
            } else {
                j &= ~0x04;
            }
            if (joy_state & GP2X_RIGHT) {
                j |= 0x08;
            } else {
                j &= ~0x08;
            }
            if (joy_state & (GP2X_B | GP2X_PUSH)) {
                j |= 0x10;
            } else {
                j &= ~0x10;
            }
            joystick_value[cur_portusb2]=j;
        }
    }

    //usb keyboard
    for (i = 0; i < gp2x_keyboard_readext(); ++i) {
        int keycode = keybuffer[i];

        if (keycode & 0x80) {
            keycode &= 0x7f; // Remove release bit
            keyboard_key_released((signed long)keycode);
        } else {
            keyboard_key_pressed((signed long)keycode);
        }
    }
}

void gp2x_handle_usb_cursor_pos(int cursor_pos, int val)
{
    if (gp2x_usbjoys > 0) {
        if (cursor_pos == USBJOYSTICK1) {
            cur_portusb1 = val;
        }
    } else if (gp2x_usbjoys > 1) {
        if (cursor_pos == USBJOYSTICK2) {
            cur_portusb2 = val;
        }
    }
}

void gp2x_draw_usb_strings(unsigned char *screen, int width)
{
    /* usb joystick 1 port */
    if (gp2x_usbjoys > 0) {
        if (cur_portusb1 == 1) {
	      draw_ascii_string(screen, width, MENU_X + (8 * 24), MENU_Y + (8 * USBJOYSTICK1), "1", menu_fg, menu_bg);
        } else {
            draw_ascii_string(screen, width, MENU_X + (8 * 24), MENU_Y + (8 * USBJOYSTICK1), "2", menu_fg, menu_bg);
        }

    } else {
        draw_ascii_string(screen, width, MENU_X + (8 * 24), MENU_Y + (8 * USBJOYSTICK1), "none", menu_fg, menu_bg);
    }

    /* usb joystick 2 port */
    if (gp2x_usbjoys > 1) {
        if (cur_portusb2 == 1) {
            draw_ascii_string(screen, width, MENU_X + (8 * 24), MENU_Y + (8 * USBJOYSTICK2), "1", menu_fg, menu_bg);
        } else {
            draw_ascii_string(screen, width, MENU_X + (8 * 24), MENU_Y + (8 * USBJOYSTICK2), "2", menu_fg, menu_bg);
        }
    } else {
        draw_ascii_string(screen, width, MENU_X + (8 * 24), MENU_Y + (8 * USBJOYSTICK2), "none", menu_fg, menu_bg);
    }
}
