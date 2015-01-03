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
#include "machine.h"
#include "maincpu.h"
#include "types.h"
#include "ui.h"
#include "intl.h"
#include "translate.h"

int joystick_inited = 0;

/* Joystick devices. */
static int joystick_device[4];

int joy_arch_init(void)
{
    if (joystick_inited == 0) {
        joystick_inited = 1;

        joystick_port_map[0] = 1;
        joystick_port_map[1] = 2;
        joystick_port_map[2] = 3;
        joystick_port_map[3] = 4;

        joyai_open();
    }

    return 0;
}

void joystick_close(void)
{
    joyai_close();

    joystick_inited = 0;
}

static int set_joystick_device(int val, void *param)
{
    int nr = vice_ptr_to_int(param);

    switch (val) {
        case JOYDEV_NONE:
        case JOYDEV_NUMPAD:
        case JOYDEV_KEYSET1:
        case JOYDEV_KEYSET2:
            break;
        default:
            return -1;
    }
    joy_arch_init();

    joystick_device[nr] = val;

    return 0;
}

static const resource_int_t joy1_resources_int[] = {
    { "JoyDevice1", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_device[0], set_joystick_device, (void *)0 },
    { NULL }
};

static const resource_int_t joy2_resources_int[] = {
    { "JoyDevice2", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_device[1], set_joystick_device, (void *)1 },
    { NULL }
};

static const resource_int_t joy3_resources_int[] = {
    { "JoyDevice3", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_device[2], set_joystick_device, (void *)2 },
    { NULL }
};

static const resource_int_t joy4_resources_int[] = {
    { "JoyDevice4", JOYDEV_NONE, RES_EVENT_NO, NULL,
      &joystick_device[3], set_joystick_device, (void *)3 },
    { NULL }
};

int joystick_arch_init_resources(void)
{
    joyai_init_resources();

    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_SCPU64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_C64DTV:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy4_resources_int);
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy4_resources_int);
            break;
        case VICE_MACHINE_CBM5x0:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy2_resources_int);
            break;
        case VICE_MACHINE_PLUS4:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy2_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy3_resources_int);
            break;
        case VICE_MACHINE_VIC20:
            if (resources_register_int(joy1_resources_int) < 0) {
                return -1;
            }
            if (resources_register_int(joy3_resources_int) < 0) {
                return -1;
            }
            return resources_register_int(joy4_resources_int);
            break;
    }
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t joydev1cmdline_options[] = {
    { "-joydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice1", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDS_SET_INPUT_JOYSTICK_1,
      NULL, NULL },
    { NULL }
};

static const cmdline_option_t joydev2cmdline_options[] = {
    { "-joydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice2", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDS_SET_INPUT_JOYSTICK_2,
      NULL, NULL },
    { NULL }
};

static const cmdline_option_t joydev3cmdline_options[] = {
    { "-extrajoydev1", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice3", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDS_SET_INPUT_EXTRA_JOYSTICK_1,
      NULL, NULL },
    { NULL }
};

static const cmdline_option_t joydev4cmdline_options[] = {
    { "-extrajoydev2", SET_RESOURCE, 1,
      NULL, NULL, "JoyDevice4", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NUMBER, IDS_SET_INPUT_EXTRA_JOYSTICK_2,
      NULL, NULL },
    { NULL }
};

int joystick_arch_cmdline_options_init(void)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:
        case VICE_MACHINE_C64SC:
        case VICE_MACHINE_SCPU64:
        case VICE_MACHINE_C128:
        case VICE_MACHINE_C64DTV:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev4cmdline_options);
            break;
        case VICE_MACHINE_PET:
        case VICE_MACHINE_CBM6x0:
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev4cmdline_options);
            break;
        case VICE_MACHINE_CBM5x0:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev2cmdline_options);
            break;
        case VICE_MACHINE_PLUS4:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev2cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev3cmdline_options);
            break;
        case VICE_MACHINE_VIC20:
            if (cmdline_register_options(joydev1cmdline_options) < 0) {
                return -1;
            }
            if (cmdline_register_options(joydev3cmdline_options) < 0) {
                return -1;
            }
            return cmdline_register_options(joydev4cmdline_options);
            break;
    }
}

/* ------------------------------------------------------------------------- */

/* Joystick-through-keyboard.  */

int joystick_handle_key(unsigned long kcode, int pressed)
{
    BYTE value = 0;

    /* The numpad case is handled specially because it allows users to use
       both `5' and `2' for "down".  */
    if (joystick_device[0] == JOYDEV_NUMPAD ||
        joystick_device[1] == JOYDEV_NUMPAD ||
        joystick_device[2] == JOYDEV_NUMPAD ||
        joystick_device[3] == JOYDEV_NUMPAD) {

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
            if (joystick_device[0] == JOYDEV_NUMPAD) {
                joystick_set_value_or(1, value);
            }

            if (joystick_device[1] == JOYDEV_NUMPAD) {
                joystick_set_value_or(2, value);
            }

            if (joystick_device[2] == JOYDEV_NUMPAD) {
                joystick_set_value_or(3, value);
            }

            if (joystick_device[3] == JOYDEV_NUMPAD) {
                joystick_set_value_or(4, value);
            }
        } else {
            if (joystick_device[0] == JOYDEV_NUMPAD) {
                joystick_set_value_and(1, (BYTE) ~value);
            }
            if (joystick_device[1] == JOYDEV_NUMPAD) {
                joystick_set_value_and(2, (BYTE) ~value);
            }
            if (joystick_device[2] == JOYDEV_NUMPAD) {
                joystick_set_value_and(3, (BYTE) ~value);
            }
            if (joystick_device[3] == JOYDEV_NUMPAD) {
                joystick_set_value_and(4, (BYTE) ~value);
            }
        }
    }

    if (joystick_device[0] == JOYDEV_KEYSET1) {
        value |= joyai_key(1, 1, kcode, pressed);
    }
    if (joystick_device[0] == JOYDEV_KEYSET2) {
        value |= joyai_key(2, 1, kcode, pressed);
    }
    if (joystick_device[1] == JOYDEV_KEYSET1) {
        value |= joyai_key(1, 2, kcode, pressed);
    }
    if (joystick_device[1] == JOYDEV_KEYSET2) {
        value |= joyai_key(2, 2, kcode, pressed);
    }
    if (joystick_device[2] == JOYDEV_KEYSET1) {
        value |= joyai_key(1, 3, kcode, pressed);
    }
    if (joystick_device[2] == JOYDEV_KEYSET2) {
        value |= joyai_key(2, 3, kcode, pressed);
    }
    if (joystick_device[3] == JOYDEV_KEYSET1) {
        value |= joyai_key(1, 4, kcode, pressed);
    }
    if (joystick_device[3] == JOYDEV_KEYSET2) {
        value |= joyai_key(2, 4, kcode, pressed);
    }

    return value;
}

int joystick_update(void)
{
    if (joystick_device[0] == JOYDEV_KEYSET1) {
        joyai_update(1, 1);
    }
    if (joystick_device[0] == JOYDEV_KEYSET2) {
        joyai_update(2, 1);
    }
    if (joystick_device[1] == JOYDEV_KEYSET1) {
        joyai_update(1, 2);
    }
    if (joystick_device[1] == JOYDEV_KEYSET2) {
        joyai_update(2, 2);
    }
    if (joystick_device[2] == JOYDEV_KEYSET1) {
        joyai_update(1, 3);
    }
    if (joystick_device[2] == JOYDEV_KEYSET2) {
        joyai_update(2, 3);
    }
    if (joystick_device[3] == JOYDEV_KEYSET1) {
        joyai_update(1, 4);
    }
    if (joystick_device[3] == JOYDEV_KEYSET2) {
        joyai_update(2, 4);
    }

    return 0;
}
#endif
