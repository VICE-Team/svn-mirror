/*
 * uijoystick.c
 *
 * Written by
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

#include <conio.h>
#include <stdio.h>

#include "joy.h"
#include "lib.h"
#include "grabkey.h"
#include "resources.h"
#include "tui.h"
#include "tui_backend.h"
#include "tuimenu.h"
#include "ui.h"
#include "uijoystick.h"


static TUI_MENU_CALLBACK(get_joystick_device_callback)
{
    int port = (int) param;
    char *resource = port == 1 ? "JoyDevice1" : "JoyDevice2";
    int value;

    resources_get_value(resource, (void *)&value);
    switch (value) {
      case JOYDEV_NONE:
        return "None";
      case JOYDEV_NUMPAD:
        return "Numpad + Right Ctrl";
      case JOYDEV_KEYSET1:
        return "Keyset A";
      case JOYDEV_KEYSET2:
        return "Keyset B";
      case JOYDEV_HW1:
        return "Joystick #1";
      case JOYDEV_HW2:
        return "Joystick #2";
    }

    return "Unknown";
}

static TUI_MENU_CALLBACK(set_joy_device_callback)
{
    int port = (int)param >> 8;
    char *resource = port == 1 ? "JoyDevice1" : "JoyDevice2";

    if (been_activated) {
        resources_set_value(resource, (resource_value_t)((int)param & 0xff));
        ui_update_menus();
    } else {
        int value;

        resources_get_value(resource, (void *)&value);
        if (value == ((int)param & 0xff))
            *become_default = 1;
    }

    return NULL;
}

static TUI_MENU_CALLBACK(swap_joysticks_callback)
{
    int value1, value2, tmp;

    if (been_activated) {
        resources_get_value("JoyDevice1", (void *)&value1);
        resources_get_value("JoyDevice2", (void *)&value2);

        tmp = value1;
        value1 = value2;
        value2 = tmp;

        resources_set_value("JoyDevice1", (resource_value_t)value1);
        resources_set_value("JoyDevice2", (resource_value_t)value2);

        ui_update_menus();
    }

    return NULL;
}

static tui_menu_item_def_t joy_device_1_submenu[] = {
    { "N_one",
      "No joystick device attached",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_NONE), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Numpad + Right Ctrl",
      "Use numeric keypad for movement and right Ctrl for fire",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_NUMPAD), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _A",
      "Use keyset A",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_KEYSET1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _B",
      "Use keyset B",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_KEYSET2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "PC Joystick #_1",
      "Use real PC joystick #1",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_HW1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "PC Joystick #_2",
      "Use real PC joystick #2",
      set_joy_device_callback, (void *)(0x100 | JOYDEV_HW2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static tui_menu_item_def_t joy_device_2_submenu[] = {
    { "N_one",
      "No joystick device attached",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_NONE), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "_Numpad + Right Ctrl",
      "Use numeric keypad for movement and right Ctrl for fire",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_NUMPAD), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _A",
      "Use keyset A",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_KEYSET1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Keyset _B",
      "Use keyset B",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_KEYSET2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "--" },
    { "Joystick #_1",
      "Use real joystick #1",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_HW1), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { "Joystick #_2",
      "Use real joystick #2",
      set_joy_device_callback, (void *)(0x200 | JOYDEV_HW2), 0,
      TUI_MENU_BEH_CLOSE, NULL, NULL },
    { NULL }
};

static TUI_MENU_CALLBACK(keyset_callback)
{
    int direction, number;
    int value;
    char *rname;

    number = (int)param >> 8;
    direction = (int)param & 0xff;
    rname = lib_msprintf("KeySet%d%s", number,
                         joystick_direction_to_string(direction));

    if (been_activated) {
        kbd_code_t key;
        int width = 60, height = 5;
        int x = CENTER_X(width), y = CENTER_Y(height);
        tui_area_t backing_store = NULL;
        char *msg;

        msg = lib_msprintf("Press key for %s%s (Esc for none)...",
                           direction == KEYSET_FIRE ? "" : "direction ",
                           joystick_direction_to_string(direction));
        tui_display_window(x, y, width, height, MESSAGE_BORDER, MESSAGE_BACK,
                           NULL, &backing_store);
        tui_set_attr(MESSAGE_FORE, MESSAGE_BACK, 0);
        tui_display(CENTER_X(strlen(msg)), y + 2, 0, msg);
        lib_free(msg);

        /* Do not allow Alt as we need it for hotkeys.  */
        do
            key = grab_key();
        while (key == K_LEFTALT || key == K_RIGHTALT);

        tui_area_put(backing_store, x, y);
        tui_area_free(backing_store);

        if (key == K_ESC)
            key = K_NONE;
        resources_set_value(rname, (resource_value_t)key);
    }

    resources_get_value(rname, (void *)&value);
    lib_free(rname);
    return kbd_code_to_string((kbd_code_t)value);
}

#define DEFINE_KEYSET_MENU(num)                                         \
    static tui_menu_item_def_t keyset_##num##_submenu[] = {             \
        { "_1: South West:",                                            \
          "Specify key for diagonal down-left direction",               \
          keyset_callback, (void *)((num << 8) | KEYSET_SW), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_2: South:",                                                 \
          "Specify key for the down direction",                         \
          keyset_callback, (void *)((num << 8) | KEYSET_S), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_3: South East:",                                            \
          "Specify key for the diagonal down-right direction",          \
          keyset_callback, (void *)((num << 8) | KEYSET_SE), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_4: West:",                                                  \
          "Specify key for the left direction",                         \
          keyset_callback, (void *)((num << 8) | KEYSET_W), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_6: East:",                                                  \
          "Specify key for the right direction",                        \
          keyset_callback, (void *)((num << 8) | KEYSET_E), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_7: North West:",                                            \
          "Specify key for the diagonal up-left direction",             \
          keyset_callback, (void *)((num << 8) | KEYSET_NW), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_8: North:",                                                 \
          "Specify key for the up direction",                           \
          keyset_callback, (void *)((num << 8) | KEYSET_N), 12,         \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_9: North East:",                                            \
          "Specify key for the diagonal up-right direction",            \
          keyset_callback, (void *)((num << 8) | KEYSET_NE), 12,        \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { "_0: Fire",                                                   \
          "Specify key for the fire button",                            \
          keyset_callback, (void *)((num << 8) | KEYSET_FIRE), 12,      \
          TUI_MENU_BEH_CONTINUE, NULL, NULL },                          \
        { NULL }                                                        \
    };

DEFINE_KEYSET_MENU(1)
DEFINE_KEYSET_MENU(2)

static tui_menu_item_def_t single_joystick_submenu[] = {
    { "Joystick _Device:",
      "Specify device for joystick emulation",
      get_joystick_device_callback, (void *)1, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_1_submenu, "Joystick" },
    { "--" },
    { "Configure Keyset _A...",
      "Configure keyboard set A for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_1_submenu, "Keyset A" },
    { "Configure Keyset _B...",
      "Configure keyboard set B for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_2_submenu, "Keyset B" },
    { NULL }
};

static tui_menu_item_def_t double_joystick_submenu[] = {
    { "_Swap",
      "Swap joystick ports",
      swap_joysticks_callback, NULL, 0,
      TUI_MENU_BEH_CONTINUE, NULL, NULL },
    { "--" },
    { "Port #_1:",
      "Specify device for emulation of joystick in port #1",
      get_joystick_device_callback, (void *)1, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_1_submenu, "Joystick #1" },
    { "Port #_2:",
      "Specify device for emulation of joystick in port #2",
      get_joystick_device_callback, (void *)2, 19,
      TUI_MENU_BEH_CONTINUE, joy_device_2_submenu, "Joystick #2" },
    { "--" },
    { "Configure Keyset _A...",
      "Configure keyboard set A for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_1_submenu, "Keyset A" },
    { "Configure Keyset _B...",
      "Configure keyboard set B for joystick emulation",
      NULL, NULL, 0,
      TUI_MENU_BEH_CONTINUE, keyset_2_submenu, "Keyset B" },
    { NULL }
};

void uijoystick_init(struct tui_menu *parent_submenu,
                     unsigned int number_joysticks)
{
    tui_menu_t ui_joystick_settings_submenu;

    if (number_joysticks > 0) {
        ui_joystick_settings_submenu = tui_menu_create("Joystick Settings", 1);
        tui_menu_add_submenu(parent_submenu, "_Joystick Settings...",
                             "Joystick settings",
                             ui_joystick_settings_submenu, NULL, 0,
                             TUI_MENU_BEH_CONTINUE);
        if (number_joysticks == 2)
            tui_menu_add(ui_joystick_settings_submenu,
                         double_joystick_submenu);
        else                    /* Just one joystick.  */
            tui_menu_add(ui_joystick_settings_submenu,
                         single_joystick_submenu);
    }
}

