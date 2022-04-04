
/*
 * mouse_paddle.c - Paddle-like devices
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

/* Control port <--> mouse/paddles/pad connections:

   cport | paddles         | I/O
   -----------------------------
     3   | paddle X button |  I
     4   | paddle Y button |  I
     5   | paddle Y value  |  I
     9   | paddle X value  |  I

   Works on:
   - native joystick port(s) (x64/x64sc/xscpu64/x128/xcbm5x0/xvic)
   - sidcart joystick adapter port (xplus4)

   cport | koalapad     | I/O
   --------------------------
     3   | left button  |  I
     4   | right button |  I
     5   | Y-position   |  I
     9   | X-position   |  I

   Works on:
   - Native joystick port(s) (x64/x64sc/xscpu64/x128/xcbm5x0/xvic)
   - sidcart joystick adapter port (xplus4)

   cport | microflyte joystick  | I/O
   ----------------------------------
     1   | Throttle up button   |  I
     2   | Throttle down button |  I
     3   | Brake button         |  I
     4   | Flaps button         |  I
     5   | up/down pot value    |  I
     6   | Reset button         |  I
     9   | left/right pot value |  I

   Works on:
   - native joystick port(s) (x64/x64sc/xscpu64/x128/xcbm5x0/xvic)
   - sidcart joystick adapter port (xplus4)

 */

/* #define DEBUG_PADDLE */

#ifdef DEBUG_PADDLE
#define DBG(_x_)  log_debug _x_
#else
#define DBG(_x_)
#endif

#include <stdlib.h> /* abs */

#include "vice.h"

#include "cmdline.h"
#include "joyport.h"
#include "joystick.h"
#include "maincpu.h"
#include "log.h"
#include "machine.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "snapshot.h"
#include "vsyncapi.h"

#include "mouse_paddle.h"

/******************************************************************************/

/* FIXME: "private" global variables for mouse - we should get rid of most of these */

extern uint8_t mouse_digital_val;
extern int16_t mouse_x;
extern int16_t mouse_y;
extern int16_t mouse_latest_x;
extern int16_t mouse_latest_y;
extern int last_mouse_x;
extern int last_mouse_y;
extern tick_t mouse_latest_os_timestamp;

/******************************************************************************/

static int paddles_p1_input = PADDLES_INPUT_MOUSE; /* host input source for paddles in port 1 */
static int paddles_p2_input = PADDLES_INPUT_MOUSE; /* host input source for paddles in port 2 */

/* --------------------------------------------------------- */
/* Paddle support */

/* FIXME: only paddle_val[2] and paddle_val[3] is actually used by the code */
static uint8_t paddle_val[] = {
/*  x     y  */
    0x00, 0xff, /* no port */
    0x00, 0xff, /* port 1 */
    0x00, 0xff, /* port 2 */
    0x00, 0xff  /* both ports */
};

static int16_t paddle_old[] = {
    -1, -1,
    -1, -1,
    -1, -1,
    -1, -1
};

static inline uint8_t mouse_paddle_update(uint8_t paddle_v, int16_t *old_v, int16_t new_v)
{
    int16_t new_paddle = (int16_t)(paddle_v + new_v - *old_v);
    *old_v = new_v;

    if (new_paddle > 255) {
        new_paddle = 255;
    } else if (new_paddle < 0) {
        new_paddle = 0;
    }
    DBG(("mouse_paddle_update paddle:%d oldv:%d newv:%d ret:%d\n",
        paddle_v, *old_v, new_v, new_paddle));
    return (uint8_t)new_paddle;
}

/*
    note: for the expected behaviour look at testprogs/SID/paddles/readme.txt
*/

/* note: we divide mouse_x / mouse_y by two here, else paddle values will be
         changing too fast, making games unplayable */

#define PADDLE_DIV  2

static uint8_t mouse_get_paddle_x(int port)
{
    DBG(("mouse_get_paddle_x port:%d mouse enabled:%d mouse_x:%d mouse_y:%d\n",
         port, _mouse_enabled, mouse_x, mouse_y));

    if (port == JOYPORT_1 || (machine_class == VICE_MACHINE_PLUS4 && port == JOYPORT_6)) {
        if (paddles_p1_input == PADDLES_INPUT_JOY_AXIS) {
            return joystick_get_axis_value(port << 1);
        } else {
            if (_mouse_enabled) {
                paddle_val[2] = mouse_paddle_update(paddle_val[2], &(paddle_old[2]), (int16_t)mouse_x / PADDLE_DIV);
                return (uint8_t)(0xff - paddle_val[2]);
            }
        }
    }

    if (port == JOYPORT_2) {
        if (paddles_p2_input == PADDLES_INPUT_JOY_AXIS) {
            return joystick_get_axis_value(port << 1);
        } else {
            if (_mouse_enabled) {
                paddle_val[2] = mouse_paddle_update(paddle_val[2], &(paddle_old[2]), (int16_t)mouse_x / PADDLE_DIV);
                return (uint8_t)(0xff - paddle_val[2]);
            }
        }
    }
    return 0xff;
}

static uint8_t mouse_get_paddle_y(int port)
{
    if (port == JOYPORT_1 || (machine_class == VICE_MACHINE_PLUS4 && port == JOYPORT_6)) {
        if (paddles_p1_input == PADDLES_INPUT_JOY_AXIS) {
            return joystick_get_axis_value((port << 1) | 1);
        } else {
            if (_mouse_enabled) {
                paddle_val[3] = mouse_paddle_update(paddle_val[3], &(paddle_old[3]), (int16_t)mouse_y / PADDLE_DIV);
                return (uint8_t)(0xff - paddle_val[3]);
            }
        }
    }

    if (port == JOYPORT_2) {
        if (paddles_p2_input == PADDLES_INPUT_JOY_AXIS) {
            return joystick_get_axis_value((port << 1) | 1);
        } else {
            if (_mouse_enabled) {
                paddle_val[3] = mouse_paddle_update(paddle_val[3], &(paddle_old[3]), (int16_t)mouse_y / PADDLE_DIV);
                return (uint8_t)(0xff - paddle_val[3]);
            }
        }
    }
    return 0xff;
}

void paddles_button_left(int pressed)
{
    if (pressed) {
        mouse_digital_val |= JOYPORT_LEFT;
    } else {
        mouse_digital_val &= (uint8_t)~JOYPORT_LEFT;
    }
}

void paddles_button_right(int pressed)
{
    if (pressed) {
        mouse_digital_val |= JOYPORT_RIGHT;
    } else {
        mouse_digital_val &= (uint8_t)~JOYPORT_RIGHT;
    }
}

static uint8_t joyport_mouse_value(int port)
{
    return _mouse_enabled ? (uint8_t)~mouse_digital_val : 0xff;
}

static joyport_mapping_t paddles_mapping =  {
    "Paddle",   /* name of the device */
    NULL,       /* NO mapping of pin 0 (UP) */
    NULL,       /* NO mapping of pin 1 (DOWN) */
    NULL,       /* NO mapping of pin 2 (LEFT) */
    NULL,       /* NO mapping of pin 3 (RIGHT) */
    "Button 1", /* name for the mapping of pin 4 (FIRE-1/SNES-A) */
    "Button 2", /* name for the mapping of pin 5 (FIRE-2/SNES-B) */
    NULL,       /* NO mapping of pin 6 (FIRE-3/SNES-X) */
    NULL,       /* NO mapping of pin 7 (SNES-Y) */
    NULL,       /* NO mapping of pin 8 (SNES-LB) */
    NULL,       /* NO mapping of pin 9 (SNES-RB) */
    NULL,       /* NO mapping of pin 10 (SNES-SELECT) */
    NULL,       /* NO mapping of pin 11 (SNES-START) */
    "Paddle 1", /* name for the mapping of pot 1 (POT-X) */
    "Paddle 2"  /* name for the mapping of pot 2 (POT-Y) */
};

static uint8_t joyport_paddles_value(int port)
{
    uint16_t paddle_fire_buttons = get_joystick_value(port);

    if (port == JOYPORT_1 || (machine_class == VICE_MACHINE_PLUS4 && port == JOYPORT_6)) {
        if (paddles_p1_input == PADDLES_INPUT_JOY_AXIS) {
            return (uint8_t)~((paddle_fire_buttons & 0x30) >> 2);
        } else {
            return _mouse_enabled ? (uint8_t)~mouse_digital_val : 0xff;
        }
    }

    if (port == JOYPORT_2) {
        if (paddles_p2_input == PADDLES_INPUT_JOY_AXIS) {
            return (uint8_t)~((paddle_fire_buttons & 0x30) >> 2);
        } else {
            return _mouse_enabled ? (uint8_t)~mouse_digital_val : 0xff;
        }
    }
    return 0xff;
}

static int joyport_mouse_enable(int port, int joyportid)
{
    int mt;

    mousedrv_mouse_changed();

    /* FIXME: clean up the following */
    mouse_get_int16(&mouse_latest_x, &mouse_latest_y);
    last_mouse_x = mouse_latest_x;
    last_mouse_y = mouse_latest_y;
    mouse_latest_os_timestamp = 0;

    if (joyportid == JOYPORT_ID_NONE) {
        mouse_type = -1;
        return 0;
    }

    mt = mouse_id_to_type(joyportid);

    if (mt == -1) {
        return -1;
    }

    if (mt == mouse_type) {
        return 0;
    }

    mouse_type = mt;
    return 0;
}

static int joyport_paddles_enable(int port, int val)
{
    if (val) {
        joyport_set_mapping(&paddles_mapping, port);
    } else {
        joyport_clear_mapping(port);
    }
    return joyport_mouse_enable(port, val);
}

/* Some prototypes are needed */
static int paddles_write_snapshot(struct snapshot_s *s, int port);
static int paddles_read_snapshot(struct snapshot_s *s, int port);

joyport_t paddles_joyport_device = {
    "Paddles",                /* name of the device */
    JOYPORT_RES_ID_NONE,      /* device normally uses the mouse for input,
                                 but it can be mapped to a joystick axis too,
                                 therefor it is flagged as not using the mouse */
    JOYPORT_IS_NOT_LIGHTPEN,  /* device is NOT a lightpen */
    JOYPORT_POT_REQUIRED,     /* device uses the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE, /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_PADDLES,   /* device is a Paddle */
    0,                        /* NO output bits */
    joyport_paddles_enable,   /* device enable function */
    joyport_paddles_value,    /* digital line read function */
    NULL,                     /* NO digital line store function */
    mouse_get_paddle_x,       /* pot-x read function */
    mouse_get_paddle_y,       /* pot-y read function */
    NULL,                     /* NO powerup function */
    paddles_write_snapshot,   /* device write snapshot function */
    paddles_read_snapshot,    /* device read snapshot function */
    NULL,                     /* NO device hook function */
    0                         /* NO device hook function mask */
};

static int set_paddles_p1_input(int val, void *param)
{
    paddles_p1_input = val ? PADDLES_INPUT_JOY_AXIS : PADDLES_INPUT_MOUSE;

    return 0;
}

static int set_paddles_p2_input(int val, void *param)
{
    paddles_p2_input = val ? PADDLES_INPUT_JOY_AXIS : PADDLES_INPUT_MOUSE;

    return 0;
}


static const resource_int_t resources_extra_int[] = {
    { "PaddlesInput1", PADDLES_INPUT_MOUSE, RES_EVENT_SAME, NULL,
      &paddles_p1_input, set_paddles_p1_input, NULL },
    { "PaddlesInput2", PADDLES_INPUT_MOUSE, RES_EVENT_SAME, NULL,
      &paddles_p2_input, set_paddles_p2_input, NULL },
    RESOURCE_INT_LIST_END
};

int paddles_resources_init(void)
{
    return resources_register_int(resources_extra_int);
}

static const cmdline_option_t cmdline_extra_option[] =
{
    { "-paddles1inputmouse", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "PaddlesInput1", (void *)PADDLES_INPUT_MOUSE,
      NULL, "Use host mouse as input for paddles in port 1." },
    { "-paddles1inputjoyaxis", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "PaddlesInput1", (void *)PADDLES_INPUT_JOY_AXIS,
      NULL, "Use host joystick axis as input for paddles in port 1." },
    { "-paddles2inputmouse", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "PaddlesInput2", (void *)PADDLES_INPUT_MOUSE,
      NULL, "Use host mouse as input for paddles in port 2." },
    { "-paddles2inputjoyaxis", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "PaddlesInput2", (void *)PADDLES_INPUT_JOY_AXIS,
      NULL, "Use host joystick axis as input for paddles in port 2." },
    CMDLINE_LIST_END
};

int paddles_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_extra_option);
}

/* PADDLES snapshot module format:

   type  | name               | description
   ----------------------------------------
   BYTE  | digital value      | digital pins return value
   BYTE  | paddle value 2     | paddle value 2
   BYTE  | paddle value 3     | paddle value 3
   BYTE  | old paddle value 2 | old paddle value 2
   BYTE  | old paddle value 3 | old paddle value 3
 */

static int write_mouse_digital_val_snapshot(snapshot_module_t *m)
{
    return SMW_B(m, mouse_digital_val);
}

static int read_mouse_digital_val_snapshot(snapshot_module_t *m)
{
    return SMR_B(m, &mouse_digital_val);
}

static int write_paddle_val_snapshot(snapshot_module_t *m)
{
    if (0
        || SMW_B(m, paddle_val[2]) < 0
        || SMW_B(m, paddle_val[3]) < 0
        || SMW_W(m, (uint16_t)paddle_old[2]) < 0
        || SMW_W(m, (uint16_t)paddle_old[3]) < 0) {
        return -1;
    }
    return 0;
}

static int read_paddle_val_snapshot(snapshot_module_t *m)
{
    uint16_t paddle_old2;
    uint16_t paddle_old3;

    if (0
        || SMR_B(m, &paddle_val[2]) < 0
        || SMR_B(m, &paddle_val[3]) < 0
        || SMR_W(m, &paddle_old2) < 0
        || SMR_W(m, &paddle_old3) < 0) {
        return -1;
    }
    paddle_old[2] = (int16_t)paddle_old2;
    paddle_old[3] = (int16_t)paddle_old3;

    return 0;
}

static const char paddles_snap_module_name[] = "PADDLES";
#define PADDLES_VER_MAJOR   0
#define PADDLES_VER_MINOR   0

static int paddles_write_snapshot(struct snapshot_s *s, int port)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, paddles_snap_module_name, PADDLES_VER_MAJOR, PADDLES_VER_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (write_mouse_digital_val_snapshot(m) < 0) {
        goto fail;
    }

    if (write_paddle_val_snapshot(m) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

static int paddles_read_snapshot(struct snapshot_s *s, int port)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, paddles_snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, PADDLES_VER_MAJOR, PADDLES_VER_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (read_mouse_digital_val_snapshot(m) < 0) {
        goto fail;
    }

    if (read_paddle_val_snapshot(m) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

/*****************************************************************************/


static uint8_t joyport_koalapad_pot_x(int port)
{
    return _mouse_enabled ? (uint8_t)(255 - mouse_get_paddle_x(port)) : 0xff;
}

/* Some prototypes are needed */
static int koalapad_write_snapshot(struct snapshot_s *s, int port);
static int koalapad_read_snapshot(struct snapshot_s *s, int port);

joyport_t koalapad_joyport_device = {
    "KoalaPad",                 /* name of the device */
    JOYPORT_RES_ID_MOUSE,       /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,    /* device is NOT a lightpen */
    JOYPORT_POT_REQUIRED,       /* device uses the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,   /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_DRAWING_PAD, /* device is a Drawing Tablet */
    0,                          /* NO output bits */
    joyport_mouse_enable,       /* device enable function */
    joyport_mouse_value,        /* digital line read function */
    NULL,                       /* NO digital line store function */
    joyport_koalapad_pot_x,     /* pot-x read function */
    mouse_get_paddle_y,         /* pot-y read function */
    NULL,                       /* NO powerup function */
    koalapad_write_snapshot,    /* device write snapshot function */
    koalapad_read_snapshot,     /* device read snapshot function */
    NULL,                       /* NO device hook function */
    0                           /* NO device hook function mask */
};

/* KOALAPAD snapshot module format:

   type   | name               | description
   -------------------------------------
   BYTE   | digital value      | digital pins return value
   BYTE   | paddle value 2     | paddle value 2
   BYTE   | paddle value 3     | paddle value 3
   WORD   | old paddle value 2 | old paddle value 2
   WORD   | old paddle value 3 | old paddle value 3
 */

static const char koalapad_snap_module_name[] = "KOALAPAD";
#define KOALAPAD_VER_MAJOR   0
#define KOALAPAD_VER_MINOR   0

static int koalapad_write_snapshot(struct snapshot_s *s, int port)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, koalapad_snap_module_name, KOALAPAD_VER_MAJOR, KOALAPAD_VER_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (write_mouse_digital_val_snapshot(m) < 0) {
        goto fail;
    }

    if (write_paddle_val_snapshot(m) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

static int koalapad_read_snapshot(struct snapshot_s *s, int port)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, koalapad_snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, KOALAPAD_VER_MAJOR, KOALAPAD_VER_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (read_mouse_digital_val_snapshot(m) < 0) {
        goto fail;
    }

    if (read_paddle_val_snapshot(m) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

/*****************************************************************************/

static joyport_mapping_t mf_mapping =  {
    "Microflyte Joystick", /* name of the device on the port */
    "Throttle Up",         /* name for the mapping of pin 0 (UP) */
    "Throttle Down",       /* name for the mapping of pin 1 (DOWN) */
    "Brake",               /* name for the mapping of pin 2 (LEFT) */
    "Flaps",               /* name for the mapping of pin 3 (RIGHT) */
    "Reset",               /* name for the mapping of pin 4 (FIRE-1/SNES-A) */
    NULL,                  /* NO mapping of pin 5 (FIRE-2/SNES-B) */
    NULL,                  /* NO mapping of pin 6 (FIRE-3/SNES-X) */
    NULL,                  /* NO mapping of pin 7 (SNES-Y) */
    NULL,                  /* NO mapping of pin 8 (SNES-LB) */
    NULL,                  /* NO mapping of pin 9 (SNES-RB) */
    NULL,                  /* NO mapping of pin 10 (SNES-SELECT) */
    NULL,                  /* NO mapping of pin 11 (SNES-START) */
    "Up/Down",             /* name for the mapping of pot 1 (POT-X) */
    "Left/Right"           /* name for the mapping of pot 2 (POT-Y) */
};

static int joyport_mf_enable(int port, int val)
{
    if (val) {
        joyport_set_mapping(&mf_mapping, port);
    } else {
        joyport_clear_mapping(port);
    }
    return joyport_mouse_enable(port, val);
}

static uint8_t joyport_mf_joystick_value(int port)
{
    uint16_t mf_fire_buttons = get_joystick_value(port);

    if (port == JOYPORT_1 || (machine_class == VICE_MACHINE_PLUS4 && port == JOYPORT_6)) {
        if (paddles_p1_input == PADDLES_INPUT_JOY_AXIS) {
            return (uint8_t)(~mf_fire_buttons);
        } else {
            return _mouse_enabled ? (uint8_t)~mouse_digital_val : 0xff;
        }
    }

    if (port == JOYPORT_2) {
        if (paddles_p2_input == PADDLES_INPUT_JOY_AXIS) {
            return (uint8_t)(~mf_fire_buttons);
        } else {
            return _mouse_enabled ? (uint8_t)~mouse_digital_val : 0xff;
        }
    }
    return 0xff;
}

joyport_t mf_joystick_joyport_device = {
    "Microflyte Joystick",     /* name of the device */
    JOYPORT_RES_ID_NONE,       /* device normally uses the mouse for input,
                                 but it can be mapped to a joystick axis too,
                                 therefor it is flagged as not using the mouse */
    JOYPORT_IS_NOT_LIGHTPEN,   /* device is NOT a lightpen */
    JOYPORT_POT_REQUIRED,      /* device uses the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,  /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_PADDLES,    /* device is a Paddle */
    0,                         /* NO output bits */
    joyport_mf_enable,         /* device enable function */
    joyport_mf_joystick_value, /* digital line read function */
    NULL,                      /* NO digital line store function */
    mouse_get_paddle_x,        /* pot-x read function */
    mouse_get_paddle_y,        /* pot-y read function */
    NULL,                      /* NO powerup function */
    paddles_write_snapshot,    /* device write snapshot function */
    paddles_read_snapshot,     /* device read snapshot function */
    NULL,                      /* NO device hook function */
    0                          /* NO device hook function mask */
};

