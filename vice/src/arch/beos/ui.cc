/*
 * ui.c - BeOS user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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

#include <Alert.h>
#include <Application.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <View.h>
#include <Window.h>
#include <stdio.h>
#include <stdlib.h>


extern "C" {
#include "attach.h"
#include "autostart.h"
#include "archdep.h"
#include "cmdline.h"
#include "constants.h"
#include "datasette.h"
#include "drive.h"
#include "fliplist.h"
#include "imagecontents.h"
#include "info.h"
//#include "interrupt.h" 
#include "kbd.h"
#include "keyboard.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
//#include "mon.h"
#include "mos6510.h"
#include "mouse.h"
#include "resources.h"
#include "tape.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "version.h"
#include "vsync.h"

/* ------------------------------------------------------------------------ */

/* UI-related resources.  */

static resource_t resources[] = {
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */
static cmdline_option_t cmdline_options[] = {
    { NULL }
};

int ui_init_cmdline_options(void)
{
    return 0;
}


/* Exit.  */
void ui_exit(void)
{
}

/* ------------------------------------------------------------------------ */
/* Initialize the UI before setting all the resource values.  */

int ui_init(int *argc, char **argv)
{
    return 0;
}

/* Initialize the UI after setting all the resource values.  */
int ui_init_finish(void)
{
	atexit(ui_exit);
    return 0;
}

/* ------------------------------------------------------------------------- */


/* Report an error to the user (`printf()' style).  */
void ui_error(const char *format, ...)
{
	BAlert *messagebox;
    char tmp[1024];
    va_list args;

    va_start(args, format);
    vsprintf(tmp, format, args);
    va_end(args);
    messagebox = new BAlert("error", tmp, "OK", NULL, NULL, 
    	B_WIDTH_AS_USUAL, B_STOP_ALERT);
	messagebox->Go();
}

/* Report an error to the user (one string).  */
void ui_error_string(const char *text)
{
}

/* Report a message to the user (`printf()' style).  */
void ui_message(const char *format,...)
{
	BAlert *messagebox;
    char tmp[1024];
    va_list args;

    va_start(args, format);
    vsprintf(tmp, format, args);
    va_end(args);
    messagebox = new BAlert("info", tmp, "OK", NULL, NULL, 
    	B_WIDTH_AS_USUAL, B_INFO_ALERT);
    messagebox->Go();

}

/* Handle the "CPU JAM" case.  */
ui_jam_action_t ui_jam_dialog(const char *format,...)
{
	ui_error("CPU jam. Resetting the machine...");
    return UI_JAM_HARD_RESET;
}

/* Handle the "Do you want to extend the disk image to 40-track format"?
   dialog.  */
int ui_extend_image_dialog(void)
{
}

void ui_update_menus(void)
{
}

void ui_cmdline_show_help(void)
{
}


/* ------------------------------------------------------------------------- */

/* Dispay the current emulation speed.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
}

void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
}

/* Toggle displaying of the drive track.  */
/* drive_base is either 8 or 0 depending on unit or drive display.
   Dual drives display drive 0: and 1: instead of unit 8: and 9: */
void ui_display_drive_track(int drivenum, int drive_base, double track_number)
{
}

/* Toggle displaying of the drive LED.  */
void ui_display_drive_led(int drivenum, int status)
{
}

/* display current image */
void ui_display_drive_current_image(unsigned int drivenum, const char *image)
{
    /* just a dummy so far */
}

/* tape-status on*/
void ui_set_tape_status(int tape_status)
{
}

void ui_display_tape_motor_status(int motor)
{   
}

void ui_display_tape_control_status(int control)
{
}

void ui_display_tape_counter(int counter)
{
}

/* Toggle displaying of paused state.  */
void ui_display_paused(int flag)
{
}

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

static void save_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
}

static void load_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
}

}
