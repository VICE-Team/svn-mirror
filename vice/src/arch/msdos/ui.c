/*
 * ui.c - A (very) simple user interface for MS-DOS.
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

#include <fcntl.h>
#include <go32.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>

#if 0
#include <sys/farptr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <go32.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <pc.h>
#include <allegro.h>
#include <math.h>
#include <io.h>
#endif

#include "ui.h"

#include "machine.h"
#include "menudefs.h"
#include "mon.h"
#include "tui.h"
#include "tuimenu.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "vsync.h"

/* ------------------------------------------------------------------------- */

static int log_fd = -1;

static void enable_log(int new)
{
    char *path = concat(boot_path, "/", "vice.log", NULL);

    printf("Writing log to `%s'\n", path);

    if (new)
	log_fd = open(path, O_TEXT | O_WRONLY | O_CREAT | O_TRUNC, 0666);
    else
	log_fd = open(path, O_TEXT | O_WRONLY | O_APPEND);
    if (log_fd == -1) {
	perror(path);
	exit(-1);
    }

    free(path);

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    dup2(log_fd, STDOUT_FILENO);
    dup2(log_fd, STDERR_FILENO);
    close(log_fd);
}

static void disable_log(void)
{
    if (log_fd != -1) {
	close(log_fd);
	freopen("CON", "wt", stdout);
	freopen("CON", "wt", stderr);
    }
}

/* ------------------------------------------------------------------------- */

static void mon_trap(ADDRESS addr)
{
    int old_mode;

    disable_log();
    enable_text();
    clrscr();
    _set_screen_lines(43);
    _setcursortype(_SOLIDCURSOR);
    old_mode = setmode(STDIN_FILENO, O_TEXT);

    mon(addr);

    setmode(STDIN_FILENO, old_mode);
    enable_log(0);
    disable_text();
}

/* ------------------------------------------------------------------------- */

static void ui_exit(void)
{
    _setcursortype(_NORMALCURSOR);
    normvideo();
}

int ui_init(int *argc, char **argv)
{
    /* This forces Ctrl-C and Ctrl-Break to be treated as normal key
       sequences. */
    _go32_want_ctrl_break(1);

    return 0;
}

int ui_init_finish(void)
{
    enable_log(1);
    tui_init();
    atexit(ui_exit);
    return 0;
}

void ui_main(ADDRESS addr)
{
    char s[256];
    double speed_index, frame_rate;
    int old_stdin_mode = setmode(STDIN_FILENO, O_BINARY);

    speed_index = vsync_get_avg_speed_index();
    frame_rate = vsync_get_avg_frame_rate();

    if (speed_index > 0.0 && frame_rate > 0.0)
	sprintf(s, "%s emulator at %d%% speed, %d fps",
		machine_name, (int)floor(speed_index), (int)floor(frame_rate));
    else
	sprintf(s, "%s emulator", machine_name);

    enable_text();

    tui_clear_screen();
    tui_set_attr(FIRST_LINE_FORE, FIRST_LINE_BACK, 0);
    tui_display(tui_num_cols() - strlen(s), 0, 0, "%s", s);

    tui_menu_update(ui_main_menu);
    tui_menu_handle(ui_main_menu);

#if 0
    /* FIXME: This is an ugly kludge.  I really think this should be handled
       transparently, i.e. without messing with the resource values, in the
       sound driver.  */
    if (app_resources.sound) {
	/* With the current version of the sound driver, we must always
	   force 100% speed (no automatic sound speed adjustment is
	   implemented for MS-DOS).  */
	app_resources.speed = 100;
    }
#endif

    disable_text();
    suspend_speed_eval();

    setmode(STDIN_FILENO, old_stdin_mode);
}

void ui_error(const char *format,...)
{
    char tmp[1024];
    va_list ap;

    enable_text();
    tui_clear_screen();

    va_start(ap, format);
    vsprintf(tmp, format, ap);
    tui_error(tmp);

    disable_text();
}

ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    char tmp[1024];
    va_list ap;

    enable_text();
    tui_clear_screen();

    va_start(ap, format);
    vsprintf(tmp, format, ap);
    tui_error(tmp);

    disable_text();

    /* Always reset. */
    return 0;
}

void ui_show_text(const char *title, const char *text)
{
    /* Needs to be done... */
}

void ui_update_menus(void)
{
    if (ui_main_menu != NULL)
        tui_menu_update(ui_main_menu);
}

void ui_autorepeat_on(void)
{
}

void ui_autorepeat_off(void)
{
}

void ui_toggle_drive_status(int state)
{
}

void ui_display_drive_track(double track_number)
{
}

void ui_display_drive_led(int status)
{
}

int ui_extend_image_dialog(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

int ui_init_resources(void)
{
    return 0;
}

int ui_init_cmdline_options(void)
{
    return 0;
}
