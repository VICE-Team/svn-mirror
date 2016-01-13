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
#include "joystick.h"
#include "resources.h"
#include "uiapi.h"
#include "uimenu.h"
#include "vsync.h"
#include "uijoystickkeys.h"
#include "userport_joystick.h"

static UI_CALLBACK(set_joystick_device_1)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice1", vice_ptr_to_int(UI_MENU_CB_PARAM));
        ui_update_menus();
    } else {
        resources_get_int("JoyDevice1", &tmp);
        ui_menu_set_tick(w, tmp == vice_ptr_to_int(UI_MENU_CB_PARAM));
    }
}

static UI_CALLBACK(set_joystick_device_2)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice2", vice_ptr_to_int(UI_MENU_CB_PARAM));
        ui_update_menus();
    } else {
        resources_get_int("JoyDevice2", &tmp);
        ui_menu_set_tick(w, tmp == vice_ptr_to_int(UI_MENU_CB_PARAM));
    }
}

static UI_CALLBACK(set_joystick_device_3)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice3", vice_ptr_to_int(UI_MENU_CB_PARAM));
        ui_update_menus();
    } else {
        resources_get_int("JoyDevice3", &tmp);
        ui_menu_set_tick(w, tmp == vice_ptr_to_int(UI_MENU_CB_PARAM));
    }
}

static UI_CALLBACK(set_joystick_device_4)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice4", vice_ptr_to_int(UI_MENU_CB_PARAM));
        ui_update_menus();
    } else {
        resources_get_int("JoyDevice4", &tmp);
        ui_menu_set_tick(w, tmp == vice_ptr_to_int(UI_MENU_CB_PARAM));
    }
}

static UI_CALLBACK(set_joystick_device_5)
{
    int tmp;

    if (!CHECK_MENUS) {
        resources_set_int("JoyDevice5", vice_ptr_to_int(UI_MENU_CB_PARAM));
        ui_update_menus();
    } else {
        resources_get_int("JoyDevice5", &tmp);
        ui_menu_set_tick(w, tmp == vice_ptr_to_int(UI_MENU_CB_PARAM));
    }
}

static UI_CALLBACK(swap_joystick_ports)
{
    int tmp1, tmp2;

    if (w != NULL) {
        vsync_suspend_speed_eval();
    }
    resources_get_int("JoyDevice1", &tmp1);
    resources_get_int("JoyDevice2", &tmp2);
    resources_set_int("JoyDevice1", tmp2);
    resources_set_int("JoyDevice2", tmp1);
    ui_update_menus();
}

static UI_CALLBACK(swap_userport_joystick_ports)
{
    int tmp3, tmp4;

    if (w != NULL) {
        vsync_suspend_speed_eval();
    }
    resources_get_int("JoyDevice3", &tmp3);
    resources_get_int("JoyDevice4", &tmp4);
    resources_set_int("JoyDevice3", tmp4);
    resources_set_int("JoyDevice4", tmp3);
    ui_update_menus();
}

UI_MENU_DEFINE_TOGGLE(KeySetEnable)
UI_MENU_DEFINE_TOGGLE(JoyOpposite)
UI_MENU_DEFINE_TOGGLE(UserportJoy)
UI_MENU_DEFINE_TOGGLE(SIDCartJoy)
UI_MENU_DEFINE_RADIO(UserportJoyType)

static ui_menu_entry_t userport_joystick_type_c64_submenu[] = {
    { N_("CGA userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_CGA, NULL },
    { N_("PET userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_PET, NULL },
    { N_("Hummer userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_HUMMER, NULL },
    { N_("OEM userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_OEM, NULL },
    { N_("HIT userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_HIT, NULL },
    { N_("Kingsoft userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_KINGSOFT, NULL },
    { N_("Starbyte userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_STARBYTE, NULL },
    { NULL }
};

static ui_menu_entry_t userport_joystick_type_submenu[] = {
    { N_("CGA userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_CGA, NULL },
    { N_("PET userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_PET, NULL },
    { N_("Hummer userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_HUMMER, NULL },
    { N_("OEM userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_OEM, NULL },
    { NULL }
};

static ui_menu_entry_t plus4_userport_joystick_type_submenu[] = {
    { N_("PET userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_PET, NULL },
    { N_("Hummer userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_HUMMER, NULL },
    { N_("OEM userport joy adapter"), UI_MENU_TYPE_TICK, (ui_callback_t)radio_UserportJoyType,
      (ui_callback_data_t)USERPORT_JOYSTICK_OEM, NULL },
    { NULL }
};

static ui_menu_entry_t set_joystick_device_1_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("Numpad"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("Keyset A"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("Keyset B"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("Analog Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("Analog Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("Analog Joystick 2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("Analog Joystick 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("Analog Joystick 4"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("Analog Joystick 5"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("Digital Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("Digital Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("USB Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("USB Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_1,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t set_joystick_device_2_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("Numpad"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("Keyset A"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("Keyset B"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("Analog Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("Analog Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("Analog Joystick 2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("Analog Joystick 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("Analog Joystick 4"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("Analog Joystick 5"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("Digital Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("Digital Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("USB Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("USB Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_2,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif /* HAS_JOYSTICK */
    { NULL }
};

static ui_menu_entry_t set_joystick_device_3_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("Numpad"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("Keyset A"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("Keyset B"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("Analog Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("Analog Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("Analog Joystick 2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("Analog Joystick 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("Analog Joystick 4"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("Analog Joystick 5"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("Digital Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("Digital Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("USB Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("USB Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_3,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif
    { NULL }
};

static ui_menu_entry_t set_joystick_device_4_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("Numpad"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("Keyset A"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("Keyset B"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("Analog Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("Analog Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("Analog Joystick 2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("Analog Joystick 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("Analog Joystick 4"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("Analog Joystick 5"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("Digital Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("Digital Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("USB Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("USB Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_4,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif /* HAS_JOYSTICK */
    { NULL }
};

static ui_menu_entry_t set_joystick_device_5_submenu[] = {
    { N_("None"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_NONE, NULL },
    { N_("Numpad"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_NUMPAD, NULL },
    { N_("Keyset A"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_KEYSET1, NULL },
    { N_("Keyset B"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_KEYSET2, NULL },
#ifdef HAS_JOYSTICK
    { N_("Analog Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_ANALOG_0, NULL },
    { N_("Analog Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_ANALOG_1, NULL },
    { N_("Analog Joystick 2"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_ANALOG_2, NULL },
    { N_("Analog Joystick 3"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_ANALOG_3, NULL },
    { N_("Analog Joystick 4"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_ANALOG_4, NULL },
    { N_("Analog Joystick 5"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_ANALOG_5, NULL },
#ifdef HAS_DIGITAL_JOYSTICK
    { N_("Digital Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_DIGITAL_0, NULL },
    { N_("Digital Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_DIGITAL_1, NULL },
#endif
#ifdef HAS_USB_JOYSTICK
    { N_("USB Joystick 0"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_USB_0, NULL },
    { N_("USB Joystick 1"), UI_MENU_TYPE_TICK,
      (ui_callback_t)set_joystick_device_5,
      (ui_callback_data_t)JOYDEV_USB_1, NULL },
#endif
#endif /* HAS_JOYSTICK */
    { NULL }
};

static ui_menu_entry_t joystick_settings_c64_submenu[] = {
    { N_("Joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_1_submenu },
    { N_("Joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_2_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef USE_GNOMEUI
    { N_("Define keysets"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)ui_keyset_dialog },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { N_("Allow opposite directions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_JoyOpposite, NULL, NULL },
    { N_("Allow keyset joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeySetEnable, NULL, NULL, KEYSYM_J, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { N_("Swap joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_joystick_ports, NULL, NULL, KEYSYM_j, UI_HOTMOD_META },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport joystick adapter"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportJoy, NULL, NULL },
    { N_("Userport joystick adapter type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, userport_joystick_type_c64_submenu },
    { N_("Userport joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_3_submenu },
    { N_("Userport joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_4_submenu },
    { N_("Swap userport joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_userport_joystick_ports, NULL, NULL, KEYSYM_u, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { NULL }
};

static ui_menu_entry_t joystick_settings_c64dtv_submenu[] = {
    { N_("Joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_1_submenu },
    { N_("Joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_2_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef USE_GNOMEUI
    { N_("Define keysets"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)ui_keyset_dialog },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { N_("Allow opposite directions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_JoyOpposite, NULL, NULL },
    { N_("Allow keyset joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeySetEnable, NULL, NULL, KEYSYM_J, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { N_("Swap joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_joystick_ports, NULL, NULL, KEYSYM_j, UI_HOTMOD_META },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport joystick adapter"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportJoy, NULL, NULL },
    { N_("Userport joystick"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_3_submenu },
    { NULL }
};

static ui_menu_entry_t joystick_settings_cbm5x0_submenu[] = {
    { N_("Joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_1_submenu },
    { N_("Joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_2_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef USE_GNOMEUI
    { N_("Define keysets"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)ui_keyset_dialog },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { N_("Allow opposite directions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_JoyOpposite, NULL, NULL },
    { N_("Allow keyset joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeySetEnable, NULL, NULL, KEYSYM_J, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { N_("Swap joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_joystick_ports, NULL, NULL, KEYSYM_j, UI_HOTMOD_META },
    { NULL }
};

static ui_menu_entry_t joystick_settings_pet_submenu[] = {
#ifdef USE_GNOMEUI
    { N_("Define keysets"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)ui_keyset_dialog },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { N_("Allow opposite directions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_JoyOpposite, NULL, NULL },
    { N_("Allow keyset joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeySetEnable, NULL, NULL, KEYSYM_J, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport joystick adapter"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportJoy, NULL, NULL },
    { N_("Userport joystick adapter type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, userport_joystick_type_submenu },
    { N_("Userport joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_3_submenu },
    { N_("Userport joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_4_submenu },
    { N_("Swap userport joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_userport_joystick_ports, NULL, NULL, KEYSYM_u, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { NULL }
};

static ui_menu_entry_t joystick_settings_vic20_submenu[] = {
    { N_("Joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_1_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef USE_GNOMEUI
    { N_("Define keysets"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)ui_keyset_dialog },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { N_("Allow opposite directions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_JoyOpposite, NULL, NULL },
    { N_("Allow keyset joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeySetEnable, NULL, NULL, KEYSYM_J, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport joystick adapter"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportJoy, NULL, NULL },
    { N_("Userport joystick adapter type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, userport_joystick_type_submenu },
    { N_("Userport joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_3_submenu },
    { N_("Userport joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_4_submenu },
    { N_("Swap userport joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_userport_joystick_ports, NULL, NULL, KEYSYM_u, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { NULL }
};

static ui_menu_entry_t joystick_settings_plus4_submenu[] = {
    { N_("Joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_1_submenu },
    { N_("Joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_2_submenu },
    { "--", UI_MENU_TYPE_SEPARATOR },
#ifdef USE_GNOMEUI
    { N_("Define keysets"), UI_MENU_TYPE_DOTS,
      (ui_callback_t)ui_keyset_dialog },
    { "--", UI_MENU_TYPE_SEPARATOR },
#endif
    { N_("Allow opposite directions"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_JoyOpposite, NULL, NULL },
    { N_("Allow keyset joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_KeySetEnable, NULL, NULL, KEYSYM_J, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { N_("Swap joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_joystick_ports, NULL, NULL, KEYSYM_j, UI_HOTMOD_META },
    { "--", UI_MENU_TYPE_SEPARATOR },
    { N_("Userport joystick adapter"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_UserportJoy, NULL, NULL },
    { N_("Userport joystick adapter type"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, userport_joystick_type_submenu },
    { N_("Userport joystick #1"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_3_submenu },
    { N_("Userport joystick #2"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_4_submenu },
    { N_("Swap userport joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_userport_joystick_ports, NULL, NULL, KEYSYM_u, UI_HOTMOD_META | UI_HOTMOD_SHIFT },
    { N_("SIDcart joystick"), UI_MENU_TYPE_TICK,
      (ui_callback_t)toggle_SIDCartJoy, NULL, NULL },
    { N_("Joystick in SIDcart control port"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, set_joystick_device_5_submenu },
    { NULL }
};

ui_menu_entry_t joystick_options_submenu[] = {
    { N_("Swap joysticks"), UI_MENU_TYPE_NORMAL,
      (ui_callback_t)swap_joystick_ports, NULL, NULL, KEYSYM_j, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t joystick_settings_c64_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joystick_settings_c64_submenu },
    { NULL }
};

ui_menu_entry_t joystick_settings_c64dtv_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joystick_settings_c64dtv_submenu },
    { NULL }
};

ui_menu_entry_t joystick_settings_cbm5x0_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joystick_settings_cbm5x0_submenu },
    { NULL }
};

ui_menu_entry_t joystick_settings_pet_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joystick_settings_pet_submenu },
    { NULL }
};

ui_menu_entry_t joystick_settings_vic20_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joystick_settings_vic20_submenu },
    { NULL }
};

ui_menu_entry_t joystick_settings_plus4_menu[] = {
    { N_("Joystick settings"), UI_MENU_TYPE_NORMAL,
      NULL, NULL, joystick_settings_plus4_submenu },
    { NULL }
};
