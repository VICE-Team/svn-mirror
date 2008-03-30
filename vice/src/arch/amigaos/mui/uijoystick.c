/*
 * uijoystick.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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

#include "mui.h"

#include "resources.h"
#include "joy.h"
#include "joyai.h"
#include "uijoystick.h"

static const char *ui_joystick[] = {
  "None",
  "Numpad + Ctrl",
  "AI/Keyset A",
  "AI/Keyset B",
  NULL
};

static const int ui_joystick_values[] = {
  JOYDEV_NONE,
  JOYDEV_NUMPAD,
  JOYDEV_KEYSET_A,
  JOYDEV_KEYSET_B,
  -1
};

static ui_to_from_t ui_to_from[] = {
  { NULL, MUI_TYPE_CYCLE, "JoyDevice1", ui_joystick, ui_joystick_values },
  { NULL, MUI_TYPE_CYCLE, "JoyDevice2", ui_joystick, ui_joystick_values },
  UI_END /* mandatory */
};

static ULONG BT0Click(struct Hook *hook, Object *obj, APTR arg)
{
  joyai_config(1);

  return 0;
}

static ULONG BT1Click(struct Hook *hook, Object *obj, APTR arg)
{
  joyai_config(2);

  return 0;
}

static APTR build_gui(void)
{
  static const struct Hook BT0Hook = { { NULL,NULL },(VOID *)BT0Click,NULL,NULL };
  static const struct Hook BT1Hook = { { NULL,NULL },(VOID *)BT1Click,NULL,NULL };

  APTR BT0, BT1;
  APTR gui = GroupObject,
    Child, GroupObject,
      MUIA_Group_Horiz, TRUE,
      Child, GroupObject,
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_FrameTitle, "Joystick in port #1",
        CYCLE(ui_to_from[0].object, "", ui_joystick)
      End,
      Child, GroupObject,
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_FrameTitle, "Joystick in port #2",
        CYCLE(ui_to_from[1].object, "", ui_joystick)
      End,
    End,
    Child, GroupObject,
      MUIA_Group_Horiz, TRUE,
      Child, BT0 = SimpleButton("Config AI/Keyset A"),
      Child, BT1 = SimpleButton("Config AI/Keyset B"),
    End,
  End;

  if (gui != NULL) {
	DoMethod(BT0, MUIM_Notify, MUIA_Pressed, FALSE,
	BT0, 2, MUIM_CallHook, &BT0Hook);

	DoMethod(BT1, MUIM_Notify, MUIA_Pressed, FALSE,
	BT1, 2, MUIM_CallHook, &BT1Hook);
  }

  return gui;
}

void ui_joystick_settings_dialog(void)
{
  mui_show_dialog(build_gui(), "Joystick Settings", ui_to_from);
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
