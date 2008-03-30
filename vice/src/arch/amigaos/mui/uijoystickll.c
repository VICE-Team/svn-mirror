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
#include "intl.h"
#include "translate.h"

#include <proto/lowlevel.h>

static int ui_joystick_device_translate[] = {
  IDS_NONE,
  IDS_KEYPAD,
  IDS_JOY_PORT_0,
  IDS_JOY_PORT_1,
  IDS_JOY_PORT_2,
  IDS_JOY_PORT_3,
  0
};

static char *ui_joystick_device[countof(ui_joystick_device_translate)];

static const int ui_joystick_device_values[] = {
  JOYDEV_NONE,
  JOYDEV_NUMPAD,
  JOYDEV_JOY0,
  JOYDEV_JOY1,
  JOYDEV_JOY2,
  JOYDEV_JOY3,
  -1
};

static int ui_joystick_fire_translate[] = {
  IDS_STOP_BLUE,
  IDS_SELECT_RED,
  IDS_REPEAT_YELLOW,
  IDS_SHUFFLE_GREEN,
  IDS_FORWARD_CHARCOAL,
  IDS_REVERSE_CHARCOAL,
  IDS_PLAY_PAUSE_GREY,
  0
};

static char *ui_joystick_fire[countof(ui_joystick_fire_translate)];

static const int ui_joystick_fire_values[] = {
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
  { NULL, MUI_TYPE_CYCLE, "JoyDevice1", ui_joystick_device, ui_joystick_device_values },
  { NULL, MUI_TYPE_CYCLE, "JoyDevice2", ui_joystick_device, ui_joystick_device_values },
  UI_END /* mandatory */
};

static ui_to_from_t ui_to_from_fire[] = {
  { NULL, MUI_TYPE_CYCLE, "JoyFire1", ui_joystick_fire, ui_joystick_fire_values },
  { NULL, MUI_TYPE_CYCLE, "JoyFire2", ui_joystick_fire, ui_joystick_fire_values },
  UI_END /* mandatory */
};

static APTR build_gui_device(void)
{
  return GroupObject,
    CYCLE(ui_to_from_device[0].object, translate_text(IDS_JOY_1_DEVICE), ui_joystick_device)
    CYCLE(ui_to_from_device[1].object, translate_text(IDS_JOY_2_DEVICE), ui_joystick_device)
  End;
}

static APTR build_gui_fire(void)
{
  return GroupObject,
    CYCLE(ui_to_from_fire[0].object, translate_text(IDS_JOY_1_FIRE), ui_joystick_fire)
    CYCLE(ui_to_from_fire[1].object, translate_text(IDS_JOY_2_FIRE), ui_joystick_fire)
  End;
}

void ui_joystick_device_dialog(void)
{
  intl_convert_mui_table(ui_joystick_device_translate, ui_joystick_device);
  mui_show_dialog(build_gui_device(), translate_text(IDMS_JOYSTICK_DEVICE_SELECT), ui_to_from_device);
}

void ui_joystick_fire_dialog(void)
{
  intl_convert_mui_table(ui_joystick_fire_translate, ui_joystick_fire);
  mui_show_dialog(build_gui_fire(), translate_text(IDMS_JOYSTICK_FIRE_SELECT), ui_to_from_fire);
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
