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

unsigned char *c64_icon_data=NULL;
unsigned char *c128_icon_data=NULL;
unsigned char *vic20_icon_data=NULL;
unsigned char *pet_icon_data=NULL;
unsigned char *plus4_icon_data=NULL;
unsigned char *cbm2_icon_data=NULL;
int num_checkmark_menu_items;
char *last_attached_images[8];
int enabled_drives;
int w;

int drive8_status, drive8_half_track;
int drive9_status, drive9_half_track;
char *drive8_image, *drive9_image;

float emu_speed, emu_fps;

int machine_type=UNKNOWN;

void ui_error(const char *text) {
	fprintf(stderr, "ui_error: %s\n", text);
}

void ui_display_drive_current_image(unsigned int drive_number,
		const char *image) {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
	if(drive_number==0) drive8_image=(char *)image;
	if(drive_number==1) drive9_image=(char *)image;
}

void ui_display_tape_current_image() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_update_menus() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void uicolor_convert_color_table() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void uicolor_alloc_color() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void uicolor_free_color() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_tape_counter() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_tape_motor_status() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_tape_control_status() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_tape_status() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_recording() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_playback() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_init() {
#if 0
	fprintf(stderr, "calling %s\n", __func__);
#endif
	gp2x_init(1000, 8, 11025,16,1,60);

	if(!strcmp(machine_name, "C64")) machine_type=C64;
	else if(!strcmp(machine_name, "C128")) machine_type=C128;
	else if(!strcmp(machine_name, "CBM-II")) machine_type=CBM2;
	else if(!strcmp(machine_name, "PET")) machine_type=PET;
	else if(!strcmp(machine_name, "PLUS4")) machine_type=PLUS4;
	else if(!strcmp(machine_name, "VIC20")) machine_type=VIC20;
}

void archdep_ui_init() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_init_finish() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_statustext() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_enable_drive_status() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_extend_image_dialog() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_drive_led(int drive_number, unsigned int led_pwm1,
                          unsigned int led_pwm2) {
    int status = 0;

    if (led_pwm1 > 100)
        status |= 1;
    if (led_pwm2 > 100)
        status |= 2;

#if 0
	fprintf(stderr, "drive led %d=%d\n", drive_number, status);
#endif

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
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
	if(drive_number==0) drive8_half_track=half_track_number;
	if(drive_number==1) drive9_half_track=half_track_number;
}

void ui_resources_init() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_cmdline_options_init() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_init_finalize() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void kbd_arch_keyname_to_keynum() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void kbd_arch_keynum_to_keyname() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void kbd_arch_init() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_jam_dialog() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_shutdown() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_resources_shutdown() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_menu_create() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_menu_update_all() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_left_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_right_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_topmenu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_tape_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_select_file() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_empty_disk_dialog() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_paused() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_screenshot_dialog() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void uiscreenshot_shutdown() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_speedmenu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void _ui_menu_radio_helper() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void _ui_menu_string_radio_helper() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void _ui_menu_toggle_helper() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_check_mouse_cursor() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_dispatch_next_event() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_cartridge_dialog() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_input_string() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_exit() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_vsid_setpsid() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_dispatch_events() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_message() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_show_text() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_display_speed(float speed, float frame_rate, int warp_enabled) {
#if 0
	fprintf(stderr, "%f%%, %ffps\n", speed, frame_rate);
#endif
	emu_speed=speed;
	emu_fps=frame_rate;
}

void ui_vsid_setsync() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_vsid_setauthor() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_vsid_setcopyright() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_vsid_setmodel() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_vsid_settune() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_vsid_setirq() {
	fprintf(stderr, "calling undefined function %s\n", __func__);
}

void ui_set_application_icon() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_autorepeat_on() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_restore_mouse() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_snapshot_dialog() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_destroy_drive8_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_drive8_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_destroy_drive9_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_set_drive9_menu() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}

void ui_about() {
#if 0
	fprintf(stderr, "calling undefined function %s\n", __func__);
#endif
}
