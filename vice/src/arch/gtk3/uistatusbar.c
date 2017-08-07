/*
 * uistatusbar.c - Native GTK3 UI statusbar stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "not_implemented.h"

#include "types.h"
#include "uiapi.h"

void ui_display_event_time(unsigned int current, unsigned int total)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_playback(int playback_status, char *version)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_recording(int recording_status)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_statustext(const char *text, int fade_out)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_volume(int vol)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

/* TODO: status display for JOYSTICK emulation
 * 
 * NOTE: newly written GUI code should be able to handle EIGHT independent
 *       joysticks, with 3 buttons each.
 */

void ui_display_joyport(BYTE *joyport)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

/* TODO: status display for TAPE emulation
 * 
 * NOTE: newly written GUI code should be able to handle TWO independent tape
 *       drives. the PET emulation may make use of it some day.
 */

void ui_display_tape_control_status(int control)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_tape_counter(int counter)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_tape_motor_status(int motor)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_set_tape_status(int tape_status)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_tape_current_image(const char *image)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

/* TODO: status display for DRIVE emulation
 * 
 * NOTE: newly written GUI code should be able to use 4 drives, of which each
 *       can be a dual disk drive. (so it must handle 8 images total). currently
 *       the code does not make use of it yet, but it will in the future.
 */
void ui_display_drive_led(int drive_number, unsigned int pwm1, unsigned int led_pwm2)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_drive_track(unsigned int drive_number,
                            unsigned int drive_base,
                            unsigned int half_track_number)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
    NOT_IMPLEMENTED_WARN_ONLY();
}

