/*
 * menu_joystick.c - Joystick menu for SDL UI.
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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
#include "types.h"

#include "vice_sdl.h"

#include "joy.h"
#include "joystick.h"
#include "kbd.h"
#include "lib.h"
#include "menu_common.h"
#include "menu_joystick.h"
#include "mouse.h"
#include "resources.h"
#include "uimenu.h"
#include "uipoll.h"
#include "userport_joystick.h"

UI_MENU_DEFINE_RADIO(JoyDevice1)
UI_MENU_DEFINE_RADIO(JoyDevice2)
UI_MENU_DEFINE_RADIO(JoyDevice3)
UI_MENU_DEFINE_RADIO(JoyDevice4)
UI_MENU_DEFINE_RADIO(JoyDevice5)
UI_MENU_DEFINE_RADIO(JoyDevice6)
UI_MENU_DEFINE_RADIO(JoyDevice7)
UI_MENU_DEFINE_RADIO(JoyDevice8)
UI_MENU_DEFINE_RADIO(JoyDevice9)
UI_MENU_DEFINE_RADIO(JoyDevice10)

#define VICE_SDL_JOYSTICK_DEVICE_MENU(port)                              \
    static const ui_menu_entry_t joystick_port##port##_device_menu[] = { \
        { "None",                                                        \
          MENU_ENTRY_RESOURCE_RADIO,                                     \
          radio_JoyDevice##port##_callback,                              \
          (ui_callback_data_t)JOYDEV_NONE },                             \
        { "Numpad",                                                      \
          MENU_ENTRY_RESOURCE_RADIO,                                     \
          radio_JoyDevice##port##_callback,                              \
          (ui_callback_data_t)JOYDEV_NUMPAD },                           \
        { "Keyset 1",                                                    \
          MENU_ENTRY_RESOURCE_RADIO,                                     \
          radio_JoyDevice##port##_callback,                              \
          (ui_callback_data_t)JOYDEV_KEYSET1 },                          \
        { "Keyset 2",                                                    \
          MENU_ENTRY_RESOURCE_RADIO,                                     \
          radio_JoyDevice##port##_callback,                              \
          (ui_callback_data_t)JOYDEV_KEYSET2 },                          \
        { "Joystick",                                                    \
          MENU_ENTRY_RESOURCE_RADIO,                                     \
          radio_JoyDevice##port##_callback,                              \
          (ui_callback_data_t)JOYDEV_JOYSTICK },                         \
        SDL_MENU_LIST_END                                                \
    };

VICE_SDL_JOYSTICK_DEVICE_MENU(1)
VICE_SDL_JOYSTICK_DEVICE_MENU(2)
VICE_SDL_JOYSTICK_DEVICE_MENU(3)
VICE_SDL_JOYSTICK_DEVICE_MENU(4)
VICE_SDL_JOYSTICK_DEVICE_MENU(5)
VICE_SDL_JOYSTICK_DEVICE_MENU(6)
VICE_SDL_JOYSTICK_DEVICE_MENU(7)
VICE_SDL_JOYSTICK_DEVICE_MENU(8)
VICE_SDL_JOYSTICK_DEVICE_MENU(9)
VICE_SDL_JOYSTICK_DEVICE_MENU(10)

UI_MENU_DEFINE_TOGGLE(KeySetEnable)
UI_MENU_DEFINE_TOGGLE(JoyOpposite)

UI_MENU_DEFINE_TOGGLE(JoyStick1AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick2AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick3AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick4AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick5AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick6AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick7AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick8AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick9AutoFire)
UI_MENU_DEFINE_TOGGLE(JoyStick10AutoFire)

UI_MENU_DEFINE_SLIDER(JoyStick1AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick2AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick3AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick4AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick5AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick6AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick7AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick8AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick9AutoFireSpeed, 1, 255)
UI_MENU_DEFINE_SLIDER(JoyStick10AutoFireSpeed, 1, 255)

UI_MENU_DEFINE_RADIO(JoyStick1AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick2AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick3AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick4AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick5AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick6AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick7AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick8AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick9AutoFireMode)
UI_MENU_DEFINE_RADIO(JoyStick10AutoFireMode)

#define VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(port)                              \
    static const ui_menu_entry_t joystick_port##port##_autofire_mode_menu[] = { \
        { "Autofire button press",                                              \
          MENU_ENTRY_RESOURCE_RADIO,                                            \
          radio_JoyStick##port##AutoFireMode_callback,                          \
          (ui_callback_data_t)JOYSTICK_AUTOFIRE_MODE_PRESS },                   \
        { "Permanent autofire",                                                 \
          MENU_ENTRY_RESOURCE_RADIO,                                            \
          radio_JoyStick##port##AutoFireMode_callback,                          \
          (ui_callback_data_t)JOYSTICK_AUTOFIRE_MODE_PERMANENT },               \
        SDL_MENU_LIST_END                                                       \
    };

VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(1)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(2)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(3)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(4)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(5)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(6)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(7)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(8)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(9)
VICE_SDL_JOYSTICK_AUTOFIRE_MODE_MENU(10)

#define VICE_SDL_JOYSTICK_AUTOFIRE_MENU(port)                              \
    static const ui_menu_entry_t joystick_port##port##_autofire_menu[] = { \
        { "Enable autofire",                                               \
          MENU_ENTRY_RESOURCE_TOGGLE,                                      \
          toggle_JoyStick##port##AutoFire_callback,                        \
          NULL },                                                          \
        { "Autofire mode",                                                 \
          MENU_ENTRY_SUBMENU,                                              \
          submenu_radio_callback,                                          \
          (ui_callback_data_t)joystick_port##port##_autofire_mode_menu },  \
        { "Autofire speed",                                                \
          MENU_ENTRY_RESOURCE_INT,                                         \
          slider_JoyStick##port##AutoFireSpeed_callback,                   \
          (ui_callback_data_t)"Set autofire speed (1 - 255)" },            \
        SDL_MENU_LIST_END                                                  \
    };

VICE_SDL_JOYSTICK_AUTOFIRE_MENU(1)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(2)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(3)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(4)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(5)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(6)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(7)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(8)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(9)
VICE_SDL_JOYSTICK_AUTOFIRE_MENU(10)

static const ui_menu_entry_t joystick_autofire_10_menu[] = {
    { "Native joystick 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port1_autofire_menu },
    { "Native joystick 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port2_autofire_menu },
    { "Joystick adapter port 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port3_autofire_menu },
    { "Joystick adapter port 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port4_autofire_menu },
    { "Joystick adapter port 3 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port5_autofire_menu },
    { "Joystick adapter port 4 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port6_autofire_menu },
    { "Joystick adapter port 5 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port7_autofire_menu },
    { "Joystick adapter port 6 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port8_autofire_menu },
    { "Joystick adapter port 7 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port9_autofire_menu },
    { "Joystick adapter port 8 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port10_autofire_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_autofire_plus4_menu[] = {
    { "Native joystick 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port1_autofire_menu },
    { "Native joystick 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port2_autofire_menu },
    { "Joystick adapter port 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port3_autofire_menu },
    { "Joystick adapter port 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port4_autofire_menu },
    { "Joystick adapter port 4 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port5_autofire_menu },
    { "SID Cartridge joystick port autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port6_autofire_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_autofire_vic20_menu[] = {
    { "Native joystick autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port1_autofire_menu },
    { "Joystick adapter port 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port3_autofire_menu },
    { "Joystick adapter port 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port4_autofire_menu },
    { "Joystick adapter port 3 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port5_autofire_menu },
    { "Joystick adapter port 4 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port6_autofire_menu },
    { "Joystick adapter port 5 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port7_autofire_menu },
    { "Joystick adapter port 6 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port8_autofire_menu },
    { "Joystick adapter port 7 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port9_autofire_menu },
    { "Joystick adapter port 8 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port10_autofire_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_autofire_userport_menu[] = {
    { "Userport joystick 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port3_autofire_menu },
    { "Userport joystick 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port4_autofire_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_autofire_userport8_menu[] = {
    { "Userport joystick 1 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port3_autofire_menu },
    { "Userport joystick 2 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port4_autofire_menu },
    { "Userport joystick 3 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port5_autofire_menu },
    { "Userport joystick 4 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port6_autofire_menu },
    { "Userport joystick 5 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port7_autofire_menu },
    { "Userport joystick 6 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port8_autofire_menu },
    { "Userport joystick 7 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port9_autofire_menu },
    { "Userport joystick 8 autofire",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_port10_autofire_menu },
    SDL_MENU_LIST_END
};

static UI_MENU_CALLBACK(custom_swap_ports_callback)
{
    if (activated) {
        sdljoy_swap_ports();
    }
    return sdljoy_get_swap_ports() ? MENU_CHECKMARK_CHECKED_STRING : NULL;
}

static UI_MENU_CALLBACK(custom_swap_userport_joystick_ports)
{
    if (activated) {
        sdljoy_swap_userport_ports();
    }
    return sdljoy_get_swap_userport_ports() ? MENU_CHECKMARK_CHECKED_STRING : NULL;
}

static UI_MENU_CALLBACK(custom_keyset_callback)
{
    SDL_Event e;
    int previous;

    if (resources_get_int((const char *)param, &previous)) {
        return sdl_menu_text_unknown;
    }

    if (activated) {
        e = sdl_ui_poll_event("key", (const char *)param, SDL_POLL_KEYBOARD | SDL_POLL_MODIFIER, 5);

        if (e.type == SDL_KEYDOWN) {
            resources_set_int((const char *)param, (int)SDL2x_to_SDL1x_Keys(e.key.keysym.sym));
        }
    } else {
        return SDL_GetKeyName(SDL1x_to_SDL2x_Keys(previous));
    }
    return NULL;
}

static const ui_menu_entry_t define_keyset_menu[] = {
    { "Keyset 1 Up",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1North" },
    { "Keyset 1 Down",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1South" },
    { "Keyset 1 Left",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1West" },
    { "Keyset 1 Right",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1East" },
    { "Keyset 1 Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire" },
    { "Keyset 1 2nd Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire2" },
    { "Keyset 1 3rd Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire3" },
    { "Keyset 1 4th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire4" },
    { "Keyset 1 5th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire5" },
    { "Keyset 1 6th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire6" },
    { "Keyset 1 7th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire7" },
    { "Keyset 1 8th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet1Fire8" },
    SDL_MENU_ITEM_SEPARATOR,
    { "Keyset 2 Up",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2North" },
    { "Keyset 2 Down",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2South" },
    { "Keyset 2 Left",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2West" },
    { "Keyset 2 Right",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2East" },
    { "Keyset 2 Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire" },
    { "Keyset 2 2nd Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire2" },
    { "Keyset 2 3rd Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire3" },
    { "Keyset 2 4th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire4" },
    { "Keyset 2 5th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire5" },
    { "Keyset 2 6th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire6" },
    { "Keyset 2 7th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire7" },
    { "Keyset 2 8th Fire",
      MENU_ENTRY_DIALOG,
      custom_keyset_callback,
      (ui_callback_data_t)"KeySet2Fire8" },
    SDL_MENU_LIST_END
};

#ifdef HAVE_SDL_NUMJOYSTICKS
static const char *joy_pin[] = {
    "Up",
    "Down",
    "Left",
    "Right",
    "Fire (or SNES-A)",
    "Fire 2 (or SNES-B)",
    "Fire 3 (or SNES-X)",
    "Fire 4 (SNES-Y)",
    "Fire 5 (SNES-LB)",
    "Fire 6 (SNES-RB)",
    "Fire 7 (SNES-SELECT)",
    "Fire 8 (SNES-START)"
};

static const char *joy_pot[] = {
    "Pot-X",
    "Pot-Y"
};

static UI_MENU_CALLBACK(custom_joymap_callback)
{
    char *target = NULL;
    SDL_Event e;
    int pin, port;

    if (activated) {
        pin = (vice_ptr_to_int(param)) & 15;
        port = (vice_ptr_to_int(param)) >> 5;

        target = lib_msprintf("Port %i %s", port + 1, joy_pin[pin]);
        e = sdl_ui_poll_event("joystick", target, SDL_POLL_JOYSTICK, 5);
        lib_free(target);

        switch (e.type) {
            case SDL_JOYAXISMOTION:
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYHATMOTION:
                sdljoy_set_joystick(e, port, (1 << pin));
                break;
            default:
                break;
        }
    }

    return NULL;
}

static UI_MENU_CALLBACK(custom_joymap_axis_callback)
{
    char *target = NULL;
    SDL_Event e;
    int pot, port;

    if (activated) {
        pot = (vice_ptr_to_int(param)) & 15;
        port = (vice_ptr_to_int(param)) >> 5;

        target = lib_msprintf("Port %i %s", port + 1, joy_pot[pot]);
        e = sdl_ui_poll_event("joystick", target, SDL_POLL_JOYSTICK, 5);
        lib_free(target);

        switch (e.type) {
            case SDL_JOYAXISMOTION:
                sdljoy_set_joystick_axis(e, port, pot);
                resources_set_int_sprintf("PaddlesInput%d", PADDLES_INPUT_JOY_AXIS, port + 1);
                break;
            case SDL_MOUSEMOTION:
                resources_set_int_sprintf("PaddlesInput%d", PADDLES_INPUT_MOUSE, port + 1);
                break;
            default:
                break;
        }
    }

    return NULL;
}

#define VICE_SDL_JOYSTICK_MAPPING_POT_MENU(port)                       \
    static const ui_menu_entry_t define_joy ## port ## _pot_menu[] = { \
        { "Up",                                                        \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(0 | ((port - 1) << 5)) },               \
        { "Down",                                                      \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(1 | ((port - 1) << 5)) },               \
        { "Left",                                                      \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(2 | ((port - 1) << 5)) },               \
        { "Right",                                                     \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(3 | ((port - 1) << 5)) },               \
        { "Fire (or SNES-A)",                                          \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(4 | ((port - 1) << 5)) },               \
        { "Fire 2 (or SNES-B)",                                        \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(5 | ((port - 1) << 5)) },               \
        { "Fire 3 (or SNES-X)",                                        \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(6 | ((port - 1) << 5)) },               \
        { "Fire 4 (SNES-Y)",                                           \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(7 | ((port - 1) << 5)) },               \
        { "Fire 5 (SNES-LB)",                                          \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(8 | ((port - 1) << 5)) },               \
        { "Fire 6 (SNES-RB)",                                          \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(9 | ((port - 1) << 5)) },               \
        { "Fire 7 (SNES-SELECT)",                                      \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(10 | ((port - 1) << 5)) },              \
        { "Fire 8 (SNES-START)",                                       \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_callback,                                      \
          (ui_callback_data_t)(11 | ((port - 1) << 5)) },              \
        { "Pot-X",                                                     \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_axis_callback,                                 \
          (ui_callback_data_t)(0 | ((port - 1) << 5)) },               \
        { "Pot-Y",                                                     \
          MENU_ENTRY_DIALOG,                                           \
          custom_joymap_axis_callback,                                 \
          (ui_callback_data_t)(1 | ((port - 1) << 5)) },               \
        SDL_MENU_LIST_END                                              \
    };

#define VICE_SDL_JOYSTICK_MAPPING_MENU(port)                       \
    static const ui_menu_entry_t define_joy ## port ## _menu[] = { \
        { "Up",                                                    \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(0 | ((port - 1) << 5)) },           \
        { "Down",                                                  \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(1 | ((port - 1) << 5)) },           \
        { "Left",                                                  \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(2 | ((port - 1) << 5)) },           \
        { "Right",                                                 \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(3 | ((port - 1) << 5)) },           \
        { "Fire (or SNES-A)",                                      \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(4 | ((port - 1) << 5)) },           \
        { "Fire 2 (or SNES-B)",                                    \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(5 | ((port - 1) << 5)) },           \
        { "Fire 3 (or SNES-X)",                                    \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(6 | ((port - 1) << 5)) },           \
        { "Fire 4 (SNES-Y)",                                       \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(7 | ((port - 1) << 5)) },           \
        { "Fire 5 (SNES-LB)",                                      \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(8 | ((port - 1) << 5)) },           \
        { "Fire 6 (SNES-RB)",                                      \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(9 | ((port - 1) << 5)) },           \
        { "Fire 7 (SNES-SELECT)",                                  \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(10 | ((port - 1) << 5)) },          \
        { "Fire 8 (SNES-START)",                                   \
          MENU_ENTRY_DIALOG,                                       \
          custom_joymap_callback,                                  \
          (ui_callback_data_t)(11 | ((port - 1) << 5)) },          \
        SDL_MENU_LIST_END                                          \
    };

VICE_SDL_JOYSTICK_MAPPING_MENU(1)
VICE_SDL_JOYSTICK_MAPPING_MENU(2)
VICE_SDL_JOYSTICK_MAPPING_MENU(3)
VICE_SDL_JOYSTICK_MAPPING_MENU(4)
VICE_SDL_JOYSTICK_MAPPING_MENU(5)
VICE_SDL_JOYSTICK_MAPPING_MENU(6)
VICE_SDL_JOYSTICK_MAPPING_MENU(7)
VICE_SDL_JOYSTICK_MAPPING_MENU(8)
VICE_SDL_JOYSTICK_MAPPING_MENU(9)
VICE_SDL_JOYSTICK_MAPPING_MENU(10)

VICE_SDL_JOYSTICK_MAPPING_POT_MENU(1)
VICE_SDL_JOYSTICK_MAPPING_POT_MENU(2)
VICE_SDL_JOYSTICK_MAPPING_POT_MENU(6)

static UI_MENU_CALLBACK(custom_joy_misc_callback)
{
    char *target = NULL;
    SDL_Event e;

    if (activated) {
        e = sdl_ui_poll_event("joystick", (vice_ptr_to_int(param)) ? "Map" : "Menu activate", SDL_POLL_JOYSTICK, 5);
        lib_free(target);

        switch (e.type) {
            case SDL_JOYAXISMOTION:
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYHATMOTION:
                sdljoy_set_extra(e, vice_ptr_to_int(param));
                break;
            default:
                break;
        }
    }

    return NULL;
}

UI_MENU_DEFINE_SLIDER(JoyThreshold, 0, 32767)
UI_MENU_DEFINE_SLIDER(JoyFuzz, 0, 32767)

static const ui_menu_entry_t define_joy_misc_menu[] = {
    { "Menu activate",
      MENU_ENTRY_DIALOG,
      custom_joy_misc_callback,
      (ui_callback_data_t)0 },
    { "Map",
      MENU_ENTRY_DIALOG,
      custom_joy_misc_callback,
      (ui_callback_data_t)1 },
    SDL_MENU_ITEM_SEPARATOR,
    { "Threshold",
      MENU_ENTRY_RESOURCE_INT,
      slider_JoyThreshold_callback,
      (ui_callback_data_t)"Set joystick threshold (0 - 32767)" },
    { "Fuzz",
      MENU_ENTRY_RESOURCE_INT,
      slider_JoyFuzz_callback,
      (ui_callback_data_t)"Set joystick fuzz (0 - 32767)" },
    SDL_MENU_LIST_END
};
#endif

#ifdef HAVE_SDL_NUMJOYSTICKS
static const ui_menu_entry_t joystick_host_mapping_10_menu[] = {
    { "Host joy to native port 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy1_pot_menu },
    { "Host joy to native port 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy2_pot_menu },
    { "Host joy to joy adapter port 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy3_menu },
    { "Host joy to joy adapter port 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy4_menu },
    { "Host joy to joy adapter port 3 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy5_menu },
    { "Host joy to joy adapter port 4 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy6_menu },
    { "Host joy to joy adapter port 5 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy7_menu },
    { "Host joy to joy adapter port 6 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy8_menu },
    { "Host joy to joy adapter port 7 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy9_menu },
    { "Host joy to joy adapter port 8 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy10_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_host_mapping_plus4_menu[] = {
    { "Host joy to native port 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy1_menu },
    { "Host joy to native port 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy2_menu },
    { "Host joy to joy adapter port 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy3_menu },
    { "Host joy to joy adapter port 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy4_menu },
    { "Host joy to joy adapter port 3 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy5_menu },
    { "Host joy to SID Cartridge joy mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy6_pot_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_host_mapping_vic20_menu[] = {
    { "Host joy to native joy mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy1_pot_menu },
    { "Host joy to joy adapter port 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy3_menu },
    { "Host joy to joy adapter port 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy4_menu },
    { "Host joy to joy adapter port 3 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy5_menu },
    { "Host joy to joy adapter port 4 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy6_menu },
    { "Host joy to joy adapter port 5 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy7_menu },
    { "Host joy to joy adapter port 6 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy8_menu },
    { "Host joy to joy adapter port 7 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy9_menu },
    { "Host joy to joy adapter port 8 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy10_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_host_mapping_userport_menu[] = {
    { "Userport joystick 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy3_menu },
    { "Userport joystick 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy4_menu },
    SDL_MENU_LIST_END
};

static const ui_menu_entry_t joystick_host_mapping_userport8_menu[] = {
    { "Userport joystick 1 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy3_menu },
    { "Userport joystick 2 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy4_menu },
    { "Userport joystick 3 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy5_menu },
    { "Userport joystick 4 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy6_menu },
    { "Userport joystick 5 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy7_menu },
    { "Userport joystick 6 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy8_menu },
    { "Userport joystick 7 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy9_menu },
    { "Userport joystick 8 mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy10_menu },
    SDL_MENU_LIST_END
};
#endif

const ui_menu_entry_t joystick_menu[] = {
    { "Native joystick port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port1_device_menu },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port2_device_menu },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port5_device_menu },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port6_device_menu },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port7_device_menu },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port8_device_menu },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port9_device_menu },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port10_device_menu },
    { "Swap native joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_ports_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_10_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_10_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

const ui_menu_entry_t joystick_c64_menu[] = {
    { "Native joystick port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port1_device_menu },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port2_device_menu },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port5_device_menu },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port6_device_menu },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port7_device_menu },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port8_device_menu },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port9_device_menu },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port10_device_menu },
    { "Swap native joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_ports_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Swap userport joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_userport_joystick_ports,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_10_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_10_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

const ui_menu_entry_t joystick_c64dtv_menu[] = {
    { "Native joystick port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port1_device_menu },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port2_device_menu },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port5_device_menu },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port6_device_menu },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port7_device_menu },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port8_device_menu },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port9_device_menu },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port10_device_menu },
    { "Swap native joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_ports_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_10_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_10_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

UI_MENU_DEFINE_TOGGLE(SIDCartJoy)

const ui_menu_entry_t joystick_plus4_menu[] = {
    { "Native joystick port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port1_device_menu },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port2_device_menu },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port5_device_menu },
    { "SID cartridge joystick port",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port6_device_menu },
    { "Swap native joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_ports_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Swap userport joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_userport_joystick_ports,
      NULL },
    { "SID Cart Joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SIDCartJoy_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_plus4_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_plus4_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

const ui_menu_entry_t joystick_vic20_menu[] = {
    { "Native joystick port",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port1_device_menu },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port5_device_menu },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port6_device_menu },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port7_device_menu },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port8_device_menu },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port9_device_menu },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port10_device_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Swap userport joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_userport_joystick_ports,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_vic20_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_vic20_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

const ui_menu_entry_t joystick_userport_only_menu[] = {
    { "Userport joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Userport joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Swap userport joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_userport_joystick_ports,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_userport_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_userport_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

const ui_menu_entry_t joystick_userport_cbm2_menu[] = {
    { "Userport joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port3_device_menu },
    { "Userport joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port4_device_menu },
    { "Userport joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port5_device_menu },
    { "Userport joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port6_device_menu },
    { "Userport joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port7_device_menu },
    { "Userport joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port8_device_menu },
    { "Userport joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port9_device_menu },
    { "Userport joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      submenu_radio_callback,
      (ui_callback_data_t)joystick_port10_device_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Allow opposite directions",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_JoyOpposite_callback,
      NULL },
    { "Allow keyset joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_KeySetEnable_callback,
      NULL },
    { "Define keysets",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_keyset_menu },
    SDL_MENU_ITEM_SEPARATOR,
    { "Swap userport joystick ports",
      MENU_ENTRY_OTHER_TOGGLE,
      custom_swap_userport_joystick_ports,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_autofire_userport8_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)joystick_host_mapping_userport8_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};
