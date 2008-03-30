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
#include <sys/movedata.h>
#include <unistd.h>

#include "ui.h"

#include "cmdline.h"
#include "dos.h"
#include "machine.h"
#include "menudefs.h"
#include "mon.h"
#include "resources.h"
#include "sound.h"
#include "tui.h"
#include "tuicharset.h"
#include "tuimenu.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "vsync.h"

/* Status of keyboard LEDs.  */
static int real_kbd_led_status = -1;
static int kbd_led_status;

#define DRIVE0_LED_MSK 4
#define DRIVE1_LED_MSK 4
#define WARP_LED_MSK 1

/* ------------------------------------------------------------------------- */

/* UI-related resources and command-line options.  */

/* Flag: Use keyboard LEDs?  */
static int use_leds;

static int set_use_leds(resource_value_t v)
{
    use_leds = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "UseLeds", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &use_leds, set_use_leds },
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-leds", SET_RESOURCE, 0, NULL, NULL, "UseLeds", (resource_value_t) 1,
      NULL, "Enable usage of PC keyboard LEDs" },
    { "+leds", SET_RESOURCE, 0, NULL, NULL, "UseLeds", (resource_value_t) 0,
      NULL, "Disable usage of PC keyboard LEDs" },
    { NULL },
};

int ui_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

inline static void set_kbd_leds(int value)
{
    /* FIXME: Is this the 100% correct way to do it?  */
    if (use_leds) {
        outportb(0x60, 0xed);
        delay(1);
        outportb(0x60, value);
        delay(1);
    }
}

/* ------------------------------------------------------------------------- */

static void ui_exit(void)
{
    _setcursortype(_NORMALCURSOR);
    normvideo();
    _set_screen_lines(25);

#ifdef UNSTABLE
    cprintf("VICE version %s (unstable).\n\r", VERSION);
#else
    cprintf("VICE version %s.\n\r", VERSION);
#endif

    cprintf("\nOfficial VICE homepage: http://www.tu-chemnitz.de/~fachat/vice/vice.html\n\n\r");
}

int ui_init(int *argc, char **argv)
{
    /* This forces Ctrl-C and Ctrl-Break to be treated as normal key
       sequences.  */
    _go32_want_ctrl_break(1);

    return 0;
}

int ui_init_finish(void)
{
    tui_init();
    atexit(ui_exit);
    return 0;
}

void ui_main(char hotkey)
{
    char s[256];
    double speed_index, frame_rate;
    int old_stdin_mode = setmode(STDIN_FILENO, O_BINARY);

    sound_suspend();

    speed_index = vsync_get_avg_speed_index();
    frame_rate = vsync_get_avg_frame_rate();

    /* Get the BIOS LED status and restore it.  */
    {
        BYTE bios_leds;
        int leds = 0;

        _dosmemgetb(0x417, 1, &bios_leds);
        if (bios_leds & 16)
            leds |= 1;          /* Scroll Lock */
        if (bios_leds & 32)
            leds |= 2;          /* Num Lock */
        if (bios_leds & 64)
            leds |= 4;          /* Caps Lock */
        set_kbd_leds(leds);
    }

    enable_text();
    tui_charset_set(TUI_CHARSET_CBM_2);

    tui_clear_screen();
    tui_set_attr(FIRST_LINE_FORE, FIRST_LINE_BACK, 0);

    if (speed_index > 0.0 && frame_rate > 0.0)
	sprintf(s, "%s emulator at %d%% speed, %d fps",
		machine_name, (int)floor(speed_index), (int)floor(frame_rate));
    else
	sprintf(s, "%s emulator", machine_name);
    tui_display(tui_num_cols() - strlen(s), 0, 0, "%s", s);

    /* FIXME: This should not be necessary.  */
    tui_menu_update(ui_main_menu);

    tui_menu_handle(ui_main_menu, hotkey);

    disable_text();
    suspend_speed_eval();

    set_kbd_leds(real_kbd_led_status);

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

    /* Always hard reset.  */
    return UI_JAM_HARD_RESET;
}

void ui_show_text(const char *title, const char *text)
{
}

void ui_update_menus(void)
{
    if (ui_main_menu != NULL)
        tui_menu_update(ui_main_menu);
}

void ui_enable_drive_status(ui_drive_enable_t state)
{
    if (!(state & UI_DRIVE_ENABLE_0))
        ui_display_drive_led(0, 0);
    if (!(state & UI_DRIVE_ENABLE_1))
        ui_display_drive_led(1, 0);
}

void ui_display_drive_track(int drive_number, double track_number)
{
}

void ui_display_drive_led(int drive_number, int status)
{
    switch (drive_number) {
      case 0:
        if (status)
            kbd_led_status |= DRIVE0_LED_MSK;
        else
            kbd_led_status &= ~DRIVE0_LED_MSK;
        break;
      case 1:
        if (status)
            kbd_led_status |= DRIVE1_LED_MSK;
        else
            kbd_led_status &= ~DRIVE1_LED_MSK;
        break;
      default:
        break;
    }
}

void ui_set_warp_status(int status)
{
    if (status)
        kbd_led_status |= WARP_LED_MSK;
    else
        kbd_led_status &= ~WARP_LED_MSK;
}

int ui_extend_image_dialog(void)
{
    int ret;

    enable_text();
    tui_clear_screen();

    ret = tui_ask_confirmation("Extend disk image in drive 8 to 40 tracks?  (Y/N)");

    disable_text();

    return ret;
}

/* ------------------------------------------------------------------------- */

void ui_dispatch_events(void)
{
    /* Update keyboard LED status.  */
    if (kbd_led_status != real_kbd_led_status) {
        set_kbd_leds(kbd_led_status);
        real_kbd_led_status = kbd_led_status;
    }
}
