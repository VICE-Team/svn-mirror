/*
 * joy.c
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

#include "vice.h"

#ifdef AMIGA_OS4
#include "cmdline.h"
#include "resources.h"

#include "lib.h"
#include "joy.h"
#include "joyai.h"
#include "joystick.h"
#include "keyboard.h"
#include "maincpu.h"
#include "types.h"
#include "ui.h"
#include "intl.h"
#include "translate.h"

int joystick_inited = 0;

/* Joystick devices. */
static int joystick_device[2];

int joystick_port_map[2];

int joy_arch_init(void)
{
    if (joystick_inited == 0) {
        joystick_inited = 1;

        joystick_port_map[0] = 1;
        joystick_port_map[1] = 2;

        joyai_open();
    }

    return 0;
}

void joystick_close(void)
{
    joyai_close();

    joystick_inited = 0;
}

static int set_joystick_device_1(resource_value_t v, void *param)
{
    joy_arch_init();

    joystick_device[0] = (int)v;

    return 0;
}

static int set_joystick_device_2(resource_value_t v, void *param)
{
    joy_arch_init();

    joystick_device[1] = (int)v;

    return 0;
}

static const resource_t resources[] = {
    { "JoyDevice1", RES_INTEGER, (resource_value_t)JOYDEV_NONE,
      RES_EVENT_NO, NULL,
      (void *)&joystick_device[0], set_joystick_device_1, NULL },
    { "JoyDevice2", RES_INTEGER, (resource_value_t)JOYDEV_NONE,
      RES_EVENT_NO, NULL,
      (void *)&joystick_device[1], set_joystick_device_2, NULL },
    { NULL }
};

int joystick_init_resources(void)
{
    joyai_init_resources();
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice1", NULL,
      IDCLS_P_NUMBER, IDS_SET_INPUT_JOYSTICK_1 },
    { "-joydev2", SET_RESOURCE, 1, NULL, NULL,
      "JoyDevice2", NULL,
      IDCLS_P_NUMBER, IDS_SET_INPUT_JOYSTICK_1 },
    { NULL }
};

int joystick_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

/* Joystick-through-keyboard.  */

int joystick_handle_key(unsigned long kcode, int pressed)
{
    BYTE value = 0;

    /* The numpad case is handled specially because it allows users to use
       both `5' and `2' for "down".  */
    if (joystick_device[0] == JOYDEV_NUMPAD
        || joystick_device[1] == JOYDEV_NUMPAD) {

        switch (kcode) {
          case 61:               /* North-West */
            value = 5;
            break;
          case 62:               /* North */
            value = 1;
            break;
          case 63:               /* North-East */
            value = 9;
            break;
          case 47:               /* East */
            value = 8;
            break;
          case 31:               /* South-East */
            value = 10;
            break;
          case 30:               /* South */
          case 46:
            value = 2;
            break;
          case 29:               /* South-West */
            value = 6;
            break;
          case 45:               /* West */
            value = 4;
            break;
          case 15:
          case 99:
            value = 16;
            break;
          default:
            /* (make compiler happy) */
            break;
        }

        if (pressed) {
            if (joystick_device[0] == JOYDEV_NUMPAD)
                joystick_set_value_or(1, value);
            if (joystick_device[1] == JOYDEV_NUMPAD)
                joystick_set_value_or(2, value);
        } else {
            if (joystick_device[0] == JOYDEV_NUMPAD)
                joystick_set_value_and(1, (BYTE) ~value);
            if (joystick_device[1] == JOYDEV_NUMPAD)
                joystick_set_value_and(2, (BYTE) ~value);
        }
    }

    if (joystick_device[0] == JOYDEV_KEYSET_A) {
        value |= joyai_key(1, 1, kcode, pressed);
    }
    if (joystick_device[0] == JOYDEV_KEYSET_B) {
        value |= joyai_key(2, 1, kcode, pressed);
    }
    if (joystick_device[1] == JOYDEV_KEYSET_A) {
        value |= joyai_key(1, 2, kcode, pressed);
    }
    if (joystick_device[1] == JOYDEV_KEYSET_B) {
        value |= joyai_key(2, 2, kcode, pressed);
    }

    return value;
}

int joystick_update(void)
{
    if (joystick_device[0] == JOYDEV_KEYSET_A) {
        joyai_update(1, 1);
    }
    if (joystick_device[0] == JOYDEV_KEYSET_B) {
        joyai_update(2, 1);
    }
    if (joystick_device[1] == JOYDEV_KEYSET_A) {
        joyai_update(1, 2);
    }
    if (joystick_device[1] == JOYDEV_KEYSET_B) {
        joyai_update(2, 2);
    }

    return 0;
}
#endif
