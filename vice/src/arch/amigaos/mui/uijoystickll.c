/*
 * uijoystickll.c
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

#ifndef AMIGA_OS4
#ifdef AMIGA_M68K
#define _INLINE_MUIMASTER_H
#endif
#include "mui.h"

#include "resources.h"
#include "joyll.h"
#include "uijoystickll.h"

#include <proto/lowlevel.h>

static const char *ui_joystick1_device[] = {
  "None",
  "Keypad",
  "Joy Port 0",
  "Joy Port 1",
  "Joy Port 2",
  "Joy Port 3",
  NULL
};

static const int ui_joystick1_device_values[] = {
  JOYDEV_NONE,
  JOYDEV_NUMPAD,
  JOYDEV_JOY0,
  JOYDEV_JOY1,
  JOYDEV_JOY2,
  JOYDEV_JOY3,
  -1
};

static const char *ui_joystick2_device[] = {
  "None",
  "Keypad",
  "Joy Port 0",
  "Joy Port 1",
  "Joy Port 2",
  "Joy Port 3",
  NULL
};

static const int ui_joystick2_device_values[] = {
  JOYDEV_NONE,
  JOYDEV_NUMPAD,
  JOYDEV_JOY0,
  JOYDEV_JOY1,
  JOYDEV_JOY2,
  JOYDEV_JOY3,
  -1
};

static const char *ui_joystick1_fire[] = {
  "Stop/Blue",
  "Select/Red",
  "Repeat/Yellow",
  "Shuffle/Green",
  "Forward/Charcoal",
  "Reverse/Charcoal",
  "Play-Pause/Grey",
  NULL
};

static const int ui_joystick1_fire_values[] = {
  JPF_BUTTON_BLUE,
  JPF_BUTTON_RED,
  JPF_BUTTON_YELLOW,
  JPF_BUTTON_GREEN,
  JPF_BUTTON_FORWARD,
  JPF_BUTTON_REVERSE,
  JPF_BUTTON_PLAY,
  -1
};

static const char *ui_joystick2_fire[] = {
  "Stop/Blue",
  "Select/Red",
  "Repeat/Yellow",
  "Shuffle/Green",
  "Forward/Charcoal",
  "Reverse/Charcoal",
  "Play-Pause/Grey",
  NULL
};

static const int ui_joystick2_fire_values[] = {
  JPF_BUTTON_BLUE,
  JPF_BUTTON_RED,
  JPF_BUTTON_YELLOW,
  JPF_BUTTON_GREEN,
  JPF_BUTTON_FORWARD,
  JPF_BUTTON_REVERSE,
  JPF_BUTTON_PLAY,
  -1
};

static ui_to_from_t ui_to_from_device[] = {
  { NULL, MUI_TYPE_CYCLE, "JoyDevice1", ui_joystick1_device, ui_joystick1_device_values },
  { NULL, MUI_TYPE_CYCLE, "JoyDevice2", ui_joystick2_device, ui_joystick2_device_values },
  UI_END /* mandatory */
};

static ui_to_from_t ui_to_from_fire[] = {
  { NULL, MUI_TYPE_CYCLE, "JoyFire1", ui_joystick1_fire, ui_joystick1_fire_values },
  { NULL, MUI_TYPE_CYCLE, "JoyFire2", ui_joystick2_fire, ui_joystick2_fire_values },
  UI_END /* mandatory */
};

static APTR build_gui_device(void)
{
  return GroupObject,
    CYCLE(ui_to_from_device[0].object, "Joy 1 Device", ui_joystick1_device)
    CYCLE(ui_to_from_device[1].object, "Joy 2 Device", ui_joystick2_device)
  End;
}

static APTR build_gui_fire(void)
{
  return GroupObject,
    CYCLE(ui_to_from_fire[0].object, "Joy 1 Fire", ui_joystick1_fire)
    CYCLE(ui_to_from_fire[1].object, "Joy 2 Fire", ui_joystick2_fire)
  End;
}

void ui_joystick_device_dialog(void)
{
  mui_show_dialog(build_gui_device(), "Joystick Device Selection", ui_to_from_device);
}

void ui_joystick_fire_dialog(void)
{
  mui_show_dialog(build_gui_fire(), "Joystick Fire Button Selection", ui_to_from_fire);
}

void ui_joystick_swap_joystick(void)
{
    int device1;
    int device2;

    resources_get_value("JoyDevice1",(void *)&device1);
    resources_get_value("JoyDevice2",(void *)&device2);
    resources_set_value("JoyDevice1",(resource_value_t)device2);
    resources_set_value("JoyDevice2",(resource_value_t)device1);
}
#endif
