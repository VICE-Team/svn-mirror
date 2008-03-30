/*
 * ui_gp2x.c
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

#include <stdio.h>
#include "videoarch.h"
#include "minimal.h"
#include "machine.h"
#include "ui_gp2x.h"

extern unsigned short *gp2x_memregs;

int num_checkmark_menu_items;
char *last_attached_images[8];
int enabled_drives;
int w;

int drive8_status, drive8_half_track;
int drive9_status, drive9_half_track;
char *drive8_image, *drive9_image;

float emu_speed, emu_fps;

void ui_error(const char *text)
{
	fprintf(stderr, "ui_error: %s\n", text);
}

void ui_display_drive_current_image(unsigned int drive_number,
		const char *image) {
	if(drive_number==0) drive8_image=(char *)image;
	if(drive_number==1) drive9_image=(char *)image;
}

void ui_update_menus(void)
{
  /* needed */
}

void ui_display_tape_current_image()
{
  /* needed */
}

void ui_display_tape_counter()
{
  /* needed */
}

void ui_display_tape_motor_status()
{
  /* needed */
}

void ui_display_tape_control_status()
{
  /* needed */
}

void ui_set_tape_status()
{
  /* needed */
}

void ui_display_recording()
{
  /* needed */
}

void ui_display_playback()
{
  /* needed */
}

void ui_init()
{
	gp2x_init(1000, 8, 11025,16,1,60);
}

void archdep_ui_init()
{
  /* needed */
}

void ui_init_finish()
{
  /* needed */
}

void ui_enable_drive_status()
{
  /* needed */
}

void ui_extend_image_dialog()
{
  /* needed */
}

void ui_display_drive_led(int drive_number, unsigned int led_pwm1,
                          unsigned int led_pwm2) {
    int status = 0;

    if (led_pwm1 > 100)
        status |= 1;
    if (led_pwm2 > 100)
        status |= 2;

	if(drive_number==0) {
		drive8_status=status;
		if(status) {
			/* switch battery led on */
			gp2x_memregs[0x106e>>1]&=~16;
		} else {
			/* switch battery led off */
			gp2x_memregs[0x106e>>1]|=16;
		}
	}
	if(drive_number==1) drive9_status=status;
}

void ui_display_drive_track(unsigned int drive_number,
		unsigned int drive_base, 
		unsigned int half_track_number) {
	if(drive_number==0) drive8_half_track=half_track_number;
	if(drive_number==1) drive9_half_track=half_track_number;
}

void ui_resources_init()
{
  /* needed */
}

void ui_cmdline_options_init()
{
  /* needed */
}

void ui_init_finalize()
{
  /* needed */
}

void kbd_arch_keyname_to_keynum()
{
  /* needed */
}

void kbd_arch_keynum_to_keyname()
{
  /* needed */
}

void kbd_arch_init()
{
  /* needed */
}

void ui_jam_dialog()
{
  /* needed */
}

void ui_shutdown()
{
  /* needed */
}

void ui_resources_shutdown()
{
  /* needed */
}

void _ui_menu_radio_helper()
{
  /* needed */
}

void ui_check_mouse_cursor()
{
  /* needed */
}

void ui_dispatch_events()
{
  /* needed */
}

void ui_display_speed(float speed, float frame_rate, int warp_enabled)
{
	emu_speed=speed;
	emu_fps=frame_rate;
}
