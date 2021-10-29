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
#include "machine.h"
#include "menu_common.h"
#include "menu_joystick.h"
#include "mouse.h"
#include "resources.h"
#include "uimenu.h"
#include "uipoll.h"
#include "userport_joystick.h"
#include "util.h"

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

static ui_menu_entry_t joystick_device_dyn_menu[JOYPORT_MAX_PORTS][6];
static int joystick_device_dyn_menu_init[JOYPORT_MAX_PORTS] = { 0 };

static void sdl_menu_joystick_device_free(int port)
{
    ui_menu_entry_t *entry = joystick_device_dyn_menu[port];
    int i;

    for (i = 0; entry[i].string != NULL; i++) {
        lib_free(entry[i].string);
        entry[i].string = NULL;
    }
}

static const ui_callback_t uijoystick_device_callbacks[JOYPORT_MAX_PORTS] = {
    radio_JoyDevice1_callback,
    radio_JoyDevice2_callback,
    radio_JoyDevice3_callback,
    radio_JoyDevice4_callback,
    radio_JoyDevice5_callback,
    radio_JoyDevice6_callback,
    radio_JoyDevice7_callback,
    radio_JoyDevice8_callback,
    radio_JoyDevice9_callback,
    radio_JoyDevice10_callback
};

static const char *joystick_device_dynmenu_helper(int port)
{
    int j = 0;
    ui_menu_entry_t *entry = joystick_device_dyn_menu[port];

    /* rebuild menu if it already exists. */
    if (joystick_device_dyn_menu_init[port] != 0) {
        sdl_menu_joystick_device_free(port);
    } else {
        joystick_device_dyn_menu_init[port] = 1;
    }

    if (joyport_has_mapping(port)) {
        entry[j].string = (char *)lib_strdup("None");
        entry[j].type = MENU_ENTRY_RESOURCE_RADIO;
        entry[j].callback = uijoystick_device_callbacks[port];
        entry[j].data = (ui_callback_data_t)int_to_void_ptr(JOYDEV_NONE);
        j++;
        
        entry[j].string = (char *)lib_strdup("Numpad");
        entry[j].type = MENU_ENTRY_RESOURCE_RADIO;
        entry[j].callback = uijoystick_device_callbacks[port];
        entry[j].data = (ui_callback_data_t)int_to_void_ptr(JOYDEV_NUMPAD);
        j++;

        entry[j].string = (char *)lib_strdup("Keyset 1");
        entry[j].type = MENU_ENTRY_RESOURCE_RADIO;
        entry[j].callback = uijoystick_device_callbacks[port];
        entry[j].data = (ui_callback_data_t)int_to_void_ptr(JOYDEV_KEYSET1);
        j++;

        entry[j].string = (char *)lib_strdup("Keyset 2");
        entry[j].type = MENU_ENTRY_RESOURCE_RADIO;
        entry[j].callback = uijoystick_device_callbacks[port];
        entry[j].data = (ui_callback_data_t)int_to_void_ptr(JOYDEV_KEYSET2);
        j++;

#ifdef HAVE_SDL_NUMJOYSTICKS
        entry[j].string = (char *)lib_strdup("Host joystick");
        entry[j].type = MENU_ENTRY_RESOURCE_RADIO;
        entry[j].callback = uijoystick_device_callbacks[port];
        entry[j].data = (ui_callback_data_t)int_to_void_ptr(JOYDEV_JOYSTICK);
        j++;
#endif
        entry[j].string = NULL;
        entry[j].type = 0;
        entry[j].callback = NULL;
        entry[j].data = NULL;

        return MENU_SUBMENU_STRING;
    }
    return MENU_NOT_AVAILABLE_STRING;
}

static UI_MENU_CALLBACK(Joystick1Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_1);
}

static UI_MENU_CALLBACK(Joystick2Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_2);
}

static UI_MENU_CALLBACK(Joystick3Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_3);
}

static UI_MENU_CALLBACK(Joystick4Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_4);
}

static UI_MENU_CALLBACK(Joystick5Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_5);
}

static UI_MENU_CALLBACK(Joystick6Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_6);
}

static UI_MENU_CALLBACK(Joystick7Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_7);
}

static UI_MENU_CALLBACK(Joystick8Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_8);
}

static UI_MENU_CALLBACK(Joystick9Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_9);
}

static UI_MENU_CALLBACK(Joystick10Device_dynmenu_callback)
{
    return joystick_device_dynmenu_helper(JOYPORT_10);
}

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

static ui_menu_entry_t joystick_autofire_dyn_menu[JOYPORT_MAX_PORTS + 1];
static int joystick_autofire_dyn_menu_init = 0;

static const ui_menu_entry_t *joystick_port_autofire_menus[JOYPORT_MAX_PORTS] = {
    joystick_port1_autofire_menu,
    joystick_port2_autofire_menu,
    joystick_port3_autofire_menu,
    joystick_port4_autofire_menu,
    joystick_port5_autofire_menu,
    joystick_port6_autofire_menu,
    joystick_port7_autofire_menu,
    joystick_port8_autofire_menu,
    joystick_port9_autofire_menu,
    joystick_port10_autofire_menu
};

static void sdl_menu_joystick_autofire_free(void)
{
    ui_menu_entry_t *entry = joystick_autofire_dyn_menu;
    int i;

    for (i = 0; entry[i].string != NULL; i++) {
        lib_free(entry[i].string);
        entry[i].string = NULL;
    }
}

static UI_MENU_CALLBACK(joystick_autofire_dynmenu_callback)
{
    int i;
    int j = 0;
    int mappings = 0;

    /* rebuild menu if it already exists. */
    if (joystick_autofire_dyn_menu_init != 0) {
        sdl_menu_joystick_autofire_free();
    } else {
        joystick_autofire_dyn_menu_init = 1;
    }

    for (i = 0; i < JOYPORT_MAX_PORTS; i++) {
        if (joyport_has_mapping(i)) {
            mappings++;
        }
    }

    if (mappings) {
        for (i = 0; i < JOYPORT_MAX_PORTS; i++) {
            if (joyport_has_mapping(i)) {
                joystick_autofire_dyn_menu[j].string = util_concat(joyport_get_port_name(i), " Autofire", NULL);
                joystick_autofire_dyn_menu[j].type = MENU_ENTRY_SUBMENU;
                joystick_autofire_dyn_menu[j].callback = submenu_callback;
                joystick_autofire_dyn_menu[j].data = (ui_callback_data_t)joystick_port_autofire_menus[i];
                j++;
            }
        }
        joystick_autofire_dyn_menu[j].string = NULL;
        joystick_autofire_dyn_menu[j].type = 0;
        joystick_autofire_dyn_menu[j].callback = NULL;
        joystick_autofire_dyn_menu[j].data = NULL;
        
        return MENU_SUBMENU_STRING;
    }
    return MENU_NOT_AVAILABLE_STRING;
}

static UI_MENU_CALLBACK(custom_swap_ports_callback)
{
    if (activated) {
        sdljoy_swap_ports();
    }
    return sdljoy_get_swap_ports() ? MENU_CHECKMARK_CHECKED_STRING : NULL;
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

    pin = (vice_ptr_to_int(param)) & 15;
    port = (vice_ptr_to_int(param)) >> 5;

    if (activated) {
        target = lib_msprintf("Port %i %s", port + 1, joy_pin[pin]);
        e = sdl_ui_poll_event("joystick", target, SDL_POLL_JOYSTICK | SDL_POLL_KEYBOARD, 5);
        lib_free(target);

        switch (e.type) {
            case SDL_JOYAXISMOTION:
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYHATMOTION:
                sdljoy_set_joystick(e, port, 1 << pin);
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_DELETE || e.key.keysym.sym == SDLK_BACKSPACE) {
                    sdljoy_delete_pin_mapping(port, 1 << pin);
                }
                break;
            default:
                break;
        }
    } else {
        return get_joy_pin_mapping_string(port, (1 << pin));
    }

    return NULL;
}

static UI_MENU_CALLBACK(custom_joymap_axis_callback)
{
    char *target = NULL;
    SDL_Event e;
    int pot, port;

    pot = (vice_ptr_to_int(param)) & 15;
    port = (vice_ptr_to_int(param)) >> 5;

    if (activated) {
        target = lib_msprintf("Port %i %s", port + 1, joy_pot[pot]);
        e = sdl_ui_poll_event("joystick", target, SDL_POLL_JOYSTICK | SDL_POLL_KEYBOARD, 5);
        lib_free(target);

        switch (e.type) {
            case SDL_JOYAXISMOTION:
                sdljoy_set_joystick_axis(e, port, pot);
                resources_set_int_sprintf("PaddlesInput%d", PADDLES_INPUT_JOY_AXIS, port + 1);
                break;
            case SDL_MOUSEMOTION:
                sdljoy_delete_pot_mapping(port, pot);
                resources_set_int_sprintf("PaddlesInput%d", PADDLES_INPUT_MOUSE, port + 1);
                break;
            case SDL_KEYDOWN:
                if (e.key.keysym.sym == SDLK_DELETE || e.key.keysym.sym == SDLK_BACKSPACE) {
                    sdljoy_delete_pot_mapping(port, pot);
                }
                break;
            default:
                break;
        }
    } else {
        return get_joy_pot_mapping_string(port, pot);
    }

    return NULL;
}

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

static ui_menu_entry_t joystick_mapping_dyn_menu[JOYPORT_MAX_PORTS][JOYPORT_MAX_PINS + JOYPORT_MAX_POTS + 1];
static int joystick_mapping_dyn_menu_init[JOYPORT_MAX_PORTS] = { 0 };

static void sdl_menu_joystick_mapping_free(int port)
{
    ui_menu_entry_t *entry = joystick_mapping_dyn_menu[port];
    int i;

    for (i = 0; entry[i].string != NULL; i++) {
        lib_free(entry[i].string);
        entry[i].string = NULL;
    }
}

static const char *joystick_mapping_dynmenu_helper(int port)
{
    joyport_map_desc_t *mappings = NULL;

    ui_menu_entry_t *entry = joystick_mapping_dyn_menu[port];
    int i;
    int j = 0;

    /* rebuild menu if it already exists. */
    if (joystick_mapping_dyn_menu_init[port] != 0) {
        sdl_menu_joystick_mapping_free(port);
    } else {
        joystick_mapping_dyn_menu_init[port] = 1;
    }

    if (joyport_port_is_active(port)) {
        mappings = joyport_get_mapping(port);
        if (mappings != NULL) {
            if (mappings->pinmap != NULL) {
                for (i = 0; mappings->pinmap[i].name; i++) {
                    entry[j].string = (char *)lib_strdup(mappings->pinmap[i].name);
                    entry[j].type = MENU_ENTRY_DIALOG;
                    entry[j].callback = custom_joymap_callback;
                    entry[j].data = (ui_callback_data_t)int_to_void_ptr((mappings->pinmap[i].pin | (port << 5)));
                    j++;
                }
            }
            if (mappings->potmap != NULL) {
                for (i = 0; mappings->potmap[i].name; i++) {
                    entry[j].string = (char *)lib_strdup(mappings->potmap[i].name);
                    entry[j].type = MENU_ENTRY_DIALOG;
                    entry[j].callback = custom_joymap_axis_callback;
                    entry[j].data = (ui_callback_data_t)int_to_void_ptr((mappings->potmap[i].pin | (port << 5)));
                    j++;
                }
            }
        }
        entry[j].string = NULL;
        entry[j].type = 0;
        entry[j].callback = NULL;
        entry[j].data = NULL;

        return MENU_SUBMENU_STRING;
    }
    return MENU_NOT_AVAILABLE_STRING;
}

static UI_MENU_CALLBACK(Joystick1Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_1);
}

static UI_MENU_CALLBACK(Joystick2Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_2);
}

static UI_MENU_CALLBACK(Joystick3Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_3);
}

static UI_MENU_CALLBACK(Joystick4Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_4);
}

static UI_MENU_CALLBACK(Joystick5Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_5);
}

static UI_MENU_CALLBACK(Joystick6Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_6);
}

static UI_MENU_CALLBACK(Joystick7Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_7);
}

static UI_MENU_CALLBACK(Joystick8Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_8);
}

static UI_MENU_CALLBACK(Joystick9Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_9);
}

static UI_MENU_CALLBACK(Joystick10Mapping_dynmenu_callback)
{
    return joystick_mapping_dynmenu_helper(JOYPORT_10);
}

static ui_menu_entry_t joystick_host_mapping_dyn_menu[JOYPORT_MAX_PORTS + 1];
static int joystick_host_mapping_dyn_menu_init = 0;

static const ui_callback_t uijoystick_host_mapping_callbacks[JOYPORT_MAX_PORTS] = {
    Joystick1Mapping_dynmenu_callback,
    Joystick2Mapping_dynmenu_callback,
    Joystick3Mapping_dynmenu_callback,
    Joystick4Mapping_dynmenu_callback,
    Joystick5Mapping_dynmenu_callback,
    Joystick6Mapping_dynmenu_callback,
    Joystick7Mapping_dynmenu_callback,
    Joystick8Mapping_dynmenu_callback,
    Joystick9Mapping_dynmenu_callback,
    Joystick10Mapping_dynmenu_callback
};

static void sdl_menu_joystick_host_mapping_free(void)
{
    ui_menu_entry_t *entry = joystick_host_mapping_dyn_menu;
    int i;

    for (i = 0; entry[i].string != NULL; i++) {
        lib_free(entry[i].string);
        entry[i].string = NULL;
    }
}

static UI_MENU_CALLBACK(joystick_host_mapping_dynmenu_callback)
{
    int i;
    int j = 0;
    int mappings = 0;

    /* rebuild menu if it already exists. */
    if (joystick_host_mapping_dyn_menu_init != 0) {
        sdl_menu_joystick_host_mapping_free();
    } else {
        joystick_host_mapping_dyn_menu_init = 1;
    }

    for (i = 0; i < JOYPORT_MAX_PORTS; i++) {
        if (joyport_has_mapping(i)) {
            mappings++;
        }
    }

    if (mappings) {
        for (i = 0; i < JOYPORT_MAX_PORTS; i++) {
            if (joyport_has_mapping(i)) {
                joystick_host_mapping_dyn_menu[j].string = util_concat("Host -> ", joyport_get_port_name(i), NULL);
                joystick_host_mapping_dyn_menu[j].type = MENU_ENTRY_SUBMENU;
                joystick_host_mapping_dyn_menu[j].callback = uijoystick_host_mapping_callbacks[i];
                joystick_host_mapping_dyn_menu[j].data = (ui_callback_data_t)joystick_mapping_dyn_menu[i];
                j++;
            }
        }
        joystick_host_mapping_dyn_menu[j].string = NULL;
        joystick_host_mapping_dyn_menu[j].type = 0;
        joystick_host_mapping_dyn_menu[j].callback = NULL;
        joystick_host_mapping_dyn_menu[j].data = NULL;
        
        return MENU_SUBMENU_STRING;
    }
    return MENU_NOT_AVAILABLE_STRING;
}
#endif

const ui_menu_entry_t joystick_menu[] = {
    { "Native joystick port 1",
      MENU_ENTRY_SUBMENU,
      Joystick1Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[0] },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      Joystick2Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[1] },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      Joystick5Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[4] },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      Joystick6Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[5] },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      Joystick7Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[6] },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      Joystick8Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[7] },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      Joystick9Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[8] },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      Joystick10Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[9] },
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
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
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
      Joystick1Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[0] },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      Joystick2Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[1] },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      Joystick5Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[4] },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      Joystick6Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[5] },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      Joystick7Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[6] },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      Joystick8Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[7] },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      Joystick9Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[8] },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      Joystick10Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[9] },
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
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
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
      Joystick1Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[0] },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      Joystick2Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[1] },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      Joystick5Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[4] },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      Joystick6Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[5] },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      Joystick7Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[6] },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      Joystick8Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[7] },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      Joystick9Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[8] },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      Joystick10Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[9] },
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
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
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
      Joystick1Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[0] },
    { "Native joystick port 2",
      MENU_ENTRY_SUBMENU,
      Joystick2Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[1] },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      Joystick5Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[4] },
    { "SID cartridge joystick port",
      MENU_ENTRY_SUBMENU,
      Joystick6Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[5] },
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
    { "SID Cart Joystick",
      MENU_ENTRY_RESOURCE_TOGGLE,
      toggle_SIDCartJoy_callback,
      NULL },
    SDL_MENU_ITEM_SEPARATOR,
    { "Autofire options",
      MENU_ENTRY_SUBMENU,
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
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
      Joystick1Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[0] },
    { "Joystick adapter port 1",
      MENU_ENTRY_SUBMENU,
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
    { "Joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      Joystick5Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[4] },
    { "Joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      Joystick6Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[5] },
    { "Joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      Joystick7Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[6] },
    { "Joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      Joystick8Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[7] },
    { "Joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      Joystick9Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[8] },
    { "Joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      Joystick10Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[9] },
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
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
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
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Userport joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
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
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
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
      Joystick3Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[2] },
    { "Userport joystick adapter port 2",
      MENU_ENTRY_SUBMENU,
      Joystick4Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[3] },
    { "Userport joystick adapter port 3",
      MENU_ENTRY_SUBMENU,
      Joystick5Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[4] },
    { "Userport joystick adapter port 4",
      MENU_ENTRY_SUBMENU,
      Joystick6Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[5] },
    { "Userport joystick adapter port 5",
      MENU_ENTRY_SUBMENU,
      Joystick7Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[6] },
    { "Userport joystick adapter port 6",
      MENU_ENTRY_SUBMENU,
      Joystick8Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[7] },
    { "Userport joystick adapter port 7",
      MENU_ENTRY_SUBMENU,
      Joystick9Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[8] },
    { "Userport joystick adapter port 8",
      MENU_ENTRY_SUBMENU,
      Joystick10Device_dynmenu_callback,
      (ui_callback_data_t)joystick_device_dyn_menu[9] },
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
      joystick_autofire_dynmenu_callback,
      (ui_callback_data_t)joystick_autofire_dyn_menu },
#ifdef HAVE_SDL_NUMJOYSTICKS
    { "Host joystick mapping",
      MENU_ENTRY_SUBMENU,
      joystick_host_mapping_dynmenu_callback,
      (ui_callback_data_t)joystick_host_mapping_dyn_menu },
    { "Extra joystick options",
      MENU_ENTRY_SUBMENU,
      submenu_callback,
      (ui_callback_data_t)define_joy_misc_menu },
#endif
    SDL_MENU_LIST_END
};

void uijoystick_menu_create(int p1, int p2, int p3_p5, int p6, int p7_p10)
{
#if 0
    int i, j = 0;
    int port_ids[] = { p1, p2, p3_p5, p3_p5, p3_p5, p6, p7_p10, p7_p10, p7_p10, p7_p10 };

    joyport_menu[j].string = "Save BBRTC data when changed";
    joyport_menu[j].type = MENU_ENTRY_RESOURCE_TOGGLE;
    joyport_menu[j].callback = toggle_BBRTCSave_callback;
    joyport_menu[j].data = NULL;
    ++j;

    for (i = 0; i < JOYPORT_MAX_PORTS; i++) {
        if (port_ids[i] != 0) {
            joyport_menu[j].string = (char *)joyport_get_port_name(i);
            joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
            joyport_menu[j].callback = uijoyport_callbacks[i];
            joyport_menu[j].data = (ui_callback_data_t)joyport_dyn_menu[i];
            ++j;
        }
    }

    joyport_menu[j].string = NULL;
    joyport_menu[j].type = MENU_ENTRY_TEXT;
    joyport_menu[j].callback = NULL;
    joyport_menu[j].data = NULL;
#endif
}

void uijoystick_menu_shutdown(void)
{
    int i;

    for (i = 0; i < JOYPORT_MAX_PORTS; i++) {
        if (joystick_mapping_dyn_menu_init[i]) {
            sdl_menu_joystick_mapping_free(i);
        }
        if (joystick_device_dyn_menu_init[i]) {
            sdl_menu_joystick_device_free(i);
        }
    }
    if (joystick_autofire_dyn_menu_init) {
        sdl_menu_joystick_autofire_free();
    }
    if (joystick_host_mapping_dyn_menu_init) {
        sdl_menu_joystick_host_mapping_free();
    }
}
