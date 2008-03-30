/*
 * uijoystick2.c
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "joy.h"
#include "resources.h"
#include "uimenu.h"
#include "vsync.h"


static UI_CALLBACK(set_joystick_device_1)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_value("JoyDevice1", (resource_value_t)UI_MENU_CB_PARAM);
        ui_update_menus();
    } else {
        resources_get_value("JoyDevice1", (void *)&tmp);
        ui_menu_set_tick(w, tmp == (int)UI_MENU_CB_PARAM);
    }
}

static UI_CALLBACK(set_joystick_device_2)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_value("JoyDevice2", (resource_value_t)UI_MENU_CB_PARAM);
        ui_update_menus();
    } else {
        resources_get_value("JoyDevice2", (void *)&tmp);
        ui_menu_set_tick(w, tmp == (int)UI_MENU_CB_PARAM);
    }
}

static UI_CALLBACK(swap_joystick_ports)
{
    int tmp1, tmp2;

    if (w != NULL)
        vsync_suspend_speed_eval();
    resources_get_value("JoyDevice1", (void *)&tmp1);
    resources_get_value("JoyDevice2", (void *)&tmp2);
    resources_set_value("JoyDevice1", (resource_value_t)tmp2);
    resources_set_value("JoyDevice2", (resource_value_t)tmp1);
    ui_update_menus();
}

static ui_menu_entry_t set_joystick_device_1_submenu[] = {
    { N_("*None"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("*Numpad"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("*Custom Keys"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { N_("*Analog Joystick 0"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("*Analog Joystick 1"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("*Analog Joystick 2"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("*Analog Joystick 3"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("*Analog Joystick 4"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("*Analog Joystick 5"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("*Digital Joystick 0"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("*Digital Joystick 1"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("*USB Joystick 0"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("*USB Joystick 1"),
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t set_joystick_device_2_submenu[] = {
    { N_("*None"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("*Numpad"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("*Custom Keys"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_CUSTOM_KEYS, NULL },
#ifdef HAS_JOYSTICK
    { N_("*Analog Joystick 0"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("*Analog Joystick 1"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("*Analog Joystick 2"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("*Analog Joystick 3"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("*Analog Joystick 4"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("*Analog Joystick 5"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("*Digital Joystick 0"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("*Digital Joystick 1"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("*USB Joystick 0"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("*USB Joystick 1"),
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif /* HAS_JOYSTICK */
    { NULL }
};


ui_menu_entry_t joystick_settings_submenu[] = {
    { N_("Joystick device in port 1"),
      NULL, NULL, set_joystick_device_1_submenu },
    { N_("Joystick device in port 2"),
      NULL, NULL, set_joystick_device_2_submenu },
    { "--" },
    { N_("Swap joystick ports"),
      (ui_callback_t)swap_joystick_ports, NULL, NULL, XK_j, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t joystick_options_submenu[] = {
    { N_("Swap joystick ports"),
      (ui_callback_t)swap_joystick_ports, NULL, NULL, XK_j, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t joystick_settings_menu[] = {
    { N_("Joystick settings"),
      NULL, NULL, joystick_settings_submenu },
    { NULL }
};

