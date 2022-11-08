/*
 * lightpen.c - Lightpen/gun emulation
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

#include <stdio.h>

#if defined(HAVE_MOUSE) && defined(HAVE_LIGHTPEN)

#include "joyport.h"
#include "joystick.h"
#include "machine.h"
#include "maincpu.h"
#include "lightpen.h"
#include "resources.h"
#include "snapshot.h"


/* Control port <--> lightpen connections:

   cport | lightpen up         | I/O
   ---------------------------------
     1   | button         (up) |  I
     6   | light sensor        |  I

   Works on:
   - native joystick port 1 (x64/x64sc/xscpu64/x128/xvic)

   cport | lightpen left       | I/O
   ---------------------------------
     3   | button       (left) |  I
     6   | light sensor        |  I

   Works on:
   - native joystick port 1 (x64/x64sc/xscpu64/x128/xvic)

   cport | datel pen           | I/O
   ---------------------------------
     3   | button       (left) |  I
     6   | light sensor        |  I

   Works on:
   - native joystick port 1 (x64/x64sc/xscpu64/x128/xvic)

   cport | magnum light phaser | I/O
   ---------------------------------
     6   | light sensor        |  I
     9   | button       (potx) |  I

   Works on:
   - native joystick port 1 (x64/x64sc/xscpu64/x128/xvic)

   cport | stack light rifle   | I/O
   ---------------------------------
     3   | button       (left) |  I
     6   | light sensor        |  I

   Works on:
   - native joystick port 1 (x64/x64sc/xscpu64/x128/xvic)

   cport | inkwell lightpen    | I/O
   ---------------------------------
     3   | button 1     (left) |  I
     6   | light sensor        |  I
     9   | button 2     (potx) |  I

     not fully implemented:

   Works on:
   - native joystick port 1 (x64/x64sc/xscpu64/x128/xvic)

   cport | Gun Stick           | I/O
   ---------------------------------
     2   | light sensor (down) |  I
     6   | button       (lp)   |  I

     This gun is somewhat weird, in that it uses pin 2 (down) for the light
     sensor, and pin 6 (lp-trigger) for the trigger button. also the signal
     on pin 2 is stretched a bit by the logic in the gun.
 */

/* --------------------------------------------------------- */
/* extern variables */

int lightpen_enabled = 0;

/* --------------------------------------------------------- */
/* static variables/functions */

#define MAX_WINDOW_NUM 1

static uint8_t lightpen_value = 0;

static int lightpen_type;

static int lightpen_buttons;
static int lightpen_button_y;
static int lightpen_button_x;

/* Video chip timing callbacks for each window.
   Returns the CLOCK value of the triggering time (or 0 if off screen).
   For x128, window 1 is VICII, window 0 is VDC. Others always use window 0. */
static lightpen_timing_callback_ptr_t chip_timing_callback[MAX_WINDOW_NUM + 1];

/* Machine dependant callback for triggering the lightpen at the given CLOCK.
   x128 needs to trigger both VICII and VDC, others point this to the video chip function itself. */
static lightpen_trigger_callback_ptr_t chip_trigger_callback;

/* Lightpen/gun type */
struct lp_type_s {
    /* PEN needs button to be pressed to register, GUN doesn't */
    enum { PEN, GUN } type;
    /* Buttons: bitmask for joyport 1 pins, with 0x20 for potY and 0x40 for potX */
    uint8_t button1;
    uint8_t button2;
    /* x/y offsets to add before timing callback */
    int x_offset;
    int y_offset;
};
typedef struct lp_type_s lp_type_t;

/* note: xoffs=0; yoffs=0 gives "pixel perfect" match, use
 * testprogs/VICII/lpcoordinates to determine the offset.
 */
/* note: the following offset values should be tweaked against the original
 *       software that came with the devices - we assume it handles them
 *       correctly. it is problematic to compare against the actual lightpen/gun
 *       today, because photo transistors/diodes degrade with age and become
 *       "slower". also the optics are likely worn out/scratched, which only
 *       generates more problems.
 *
 *       new software (and cracks) should introduce a calibration feature to
 *       handle those problems.
 */
static const lp_type_t lp_type[LIGHTPEN_TYPE_NUM] = {
    /* Pen with button Up (e.g. Atari CX75) */
    { PEN, JOYPORT_UP, 0x00, 0, 0 },
    /* Pen with button Left */
    { PEN, JOYPORT_LEFT, 0x00, 0, 0 },
    /* Datel Pen */
    { PEN, JOYPORT_LEFT, 0x00, 20, -5 },
    /* Magnum Light Phaser, Cheetah Defender */
    { GUN, JOYPORT_FIRE_2, 0x00, 30, -10 },   /* tweaked against "Blaze Out" and "3-D Action Pack" */
    /* Stack Light Rifle */
    { GUN, JOYPORT_LEFT, 0x00, 20, 0 },
    /* Inkwell Lightpen */
    { GUN, JOYPORT_LEFT, JOYPORT_FIRE_2, 20, 0 },
#ifdef JOYPORT_EXPERIMENTAL_DEVICES
    /* Gun Stick */
    { GUN, JOYPORT_FIRE | JOYPORT_DOWN, 0x00, 0, 0 },
#endif
};

typedef struct lp_id_s {
    int lp;
    int id;
} lp_id_t;

static const lp_id_t lp_id[] = {
    { LIGHTPEN_TYPE_PEN_U,     JOYPORT_ID_LIGHTPEN_U },
    { LIGHTPEN_TYPE_PEN_L,     JOYPORT_ID_LIGHTPEN_L },
    { LIGHTPEN_TYPE_PEN_DATEL, JOYPORT_ID_LIGHTPEN_DATEL },
    { LIGHTPEN_TYPE_GUN_Y,     JOYPORT_ID_LIGHTGUN_Y },
    { LIGHTPEN_TYPE_GUN_L,     JOYPORT_ID_LIGHTGUN_L },
    { LIGHTPEN_TYPE_INKWELL,   JOYPORT_ID_LIGHTPEN_INKWELL },
#ifdef JOYPORT_EXPERIMENTAL_DEVICES
    { LIGHTPEN_TYPE_GUNSTICK,  JOYPORT_ID_LIGHTGUN_GUNSTICK },
#endif
    { -1,                      -1 }
};

static inline int joyport_id_to_lightpen_type(int id)
{
    int i;

    for (i = 0; lp_id[i].lp != -1; ++i) {
        if (lp_id[i].id == id) {
            return lp_id[i].lp;
        }
    }
    return -1;
}

static inline int lightpen_type_to_joyport_id(int lp)
{
    int i;

    for (i = 0; lp_id[i].lp != -1; ++i) {
        if (lp_id[i].lp == lp) {
            return lp_id[i].id;
        }
    }
    return -1;
}

static inline void lightpen_check_button_mask(uint8_t mask, int pressed)
{
    int id;
    uint8_t old_value = lightpen_value;

    if (!mask) {
        return;
    }

    if (pressed) {
        lightpen_value |= mask;
    } else {
        lightpen_value &= (uint8_t)~mask;
    }

    if (lightpen_value == old_value) {
        return;
    }

    id = lightpen_type_to_joyport_id(lightpen_type);
    if (id == -1) {
        return;
    }
    /* FIXME: we don't know the port here (is it really always port 1?, using
              JOYPORT_ID_UNKNOWN will make joyport_display_joyport search for
              the device in all available ports */
    joyport_display_joyport(JOYPORT_ID_UNKNOWN, id, (uint16_t)lightpen_value);
}

static inline void lightpen_update_buttons(int buttons)
{
    lightpen_buttons = buttons;

    /* check potx/poty */
    lightpen_button_y = ((((lp_type[lightpen_type].button1 & 0x20) == 0x20) && (buttons & LP_HOST_BUTTON_1))
                         || (((lp_type[lightpen_type].button2 & 0x20) == 0x20) && (buttons & LP_HOST_BUTTON_2)))
                        ? 1 : 0;

    lightpen_button_x = ((((lp_type[lightpen_type].button1 & 0x40) == 0x40) && (buttons & LP_HOST_BUTTON_1))
                         || (((lp_type[lightpen_type].button2 & 0x40) == 0x40) && (buttons & LP_HOST_BUTTON_2)))
                        ? 1 : 0;

    lightpen_check_button_mask((uint8_t)(lp_type[lightpen_type].button1 & 0x1f), buttons & LP_HOST_BUTTON_1);
    lightpen_check_button_mask((uint8_t)(lp_type[lightpen_type].button2 & 0x1f), buttons & LP_HOST_BUTTON_2);
}

/* --------------------------------------------------------- */

/* Some prototypes are needed */
static int lightpen_write_snapshot(struct snapshot_s *s, int port);
static int lightpen_read_snapshot(struct snapshot_s *s, int port);

static int joyport_lightpen_set_enabled(int port, int id)
{
    lightpen_enabled = id ? 1 : 0;

    /* disabled */
    if (!lightpen_enabled) {
        lightpen_type = -1;
        return 0;
    }

    /* select lightpen type based on the id passed to the function */
    lightpen_type = joyport_id_to_lightpen_type(id);

    if (lightpen_type == -1) {
        return -1;
    }

    return 0;
}

static uint8_t lightpen_digital_val(int port)
{
    joyport_display_joyport(port, lightpen_type_to_joyport_id(lightpen_type), lightpen_value);

    return (uint8_t)~lightpen_value;
}

static uint8_t lightpen_read_button_y(int port)
{
    return (lightpen_enabled && lightpen_button_y) ? 0x00 : 0xff;
}

#if 0
static uint8_t lightpen_read_button_x(int port)
{
    return (lightpen_enabled && lightpen_button_x) ? 0x00 : 0xff;
}
#endif

static joyport_t lightpen_u_joyport_device = {
    "Light Pen (up trigger)",     /* name of the device */
    JOYPORT_RES_ID_MOUSE,         /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,          /* device is a lightpen */
    JOYPORT_POT_OPTIONAL,         /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,     /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTPEN,      /* device is a Light Pen */
    0,                            /* NO output bits */
    joyport_lightpen_set_enabled, /* device enable/disable function */
    lightpen_digital_val,         /* digital line read function */
    NULL,                         /* NO digital line store function */
    NULL,                         /* NO pot-x read function */
    NULL,                         /* NO pot-y read function */
    NULL,                         /* NO powerup function */
    lightpen_write_snapshot,      /* device write snapshot function */
    lightpen_read_snapshot,       /* device read snapshot function */
    NULL,                         /* NO device hook function */
    0                             /* NO device hook function mask */
};

static joyport_t lightpen_l_joyport_device = {
    "Light Pen (left trigger)",   /* name of the device */
    JOYPORT_RES_ID_MOUSE,         /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,          /* device is a lightpen */
    JOYPORT_POT_OPTIONAL,         /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,     /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTPEN,      /* device is a Light Pen */
    0,                            /* NO output bits */
    joyport_lightpen_set_enabled, /* device enable/disable function */
    lightpen_digital_val,         /* digital line read function */
    NULL,                         /* NO digital line store function */
    NULL,                         /* NO pot-x read function */
    NULL,                         /* NO pot-y read function */
    NULL,                         /* NO powerup function */
    lightpen_write_snapshot,      /* device write snapshot function */
    lightpen_read_snapshot,       /* device read snapshot function */
    NULL,                         /* NO device hook function */
    0                             /* NO device hook function mask */
};

static joyport_t lightpen_datel_joyport_device = {
    "Light Pen (Datel)",          /* name of the device */
    JOYPORT_RES_ID_MOUSE,         /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,          /* device is a lightpen */
    JOYPORT_POT_OPTIONAL,         /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,     /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTPEN,      /* device is a Light Pen */
    0,                            /* NO output bits */
    joyport_lightpen_set_enabled, /* device enable/disable function */
    lightpen_digital_val,         /* digital line read function */
    NULL,                         /* NO digital line store function */
    NULL,                         /* NO pot-x read function */
    NULL,                         /* NO pot-y read function */
    NULL,                         /* NO powerup function */
    lightpen_write_snapshot,      /* device write snapshot function */
    lightpen_read_snapshot,       /* device read snapshot function */
    NULL,                         /* NO device hook function */
    0                             /* NO device hook function mask */
};

static joyport_t magnum_light_phaser_joyport_device = {
    "Light Gun (Magnum Light Phaser)", /* name of the device */
    JOYPORT_RES_ID_MOUSE,              /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,               /* device is a lightpen */
    JOYPORT_POT_REQUIRED,              /* device uses the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,          /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTGUN,           /* device is a Light Gun */
    0,                                 /* NO output bits */
    joyport_lightpen_set_enabled,      /* device enable/disable function */
    lightpen_digital_val,              /* digital line read function */
    NULL,                              /* NO digital line store function */
    NULL,                              /* NO pot-x read function */
    lightpen_read_button_y,            /* pot-y read function */
    NULL,                              /* NO powerup function */
    lightpen_write_snapshot,           /* device write snapshot function */
    lightpen_read_snapshot,            /* device read snapshot function */
    NULL,                              /* NO device hook function */
    0                                  /* NO device hook function mask */
};

static joyport_t stack_light_rifle_joyport_device = {
    "Light Gun (Stack Light Rifle)", /* name of the device */
    JOYPORT_RES_ID_MOUSE,            /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,             /* device is a lightpen */
    JOYPORT_POT_OPTIONAL,            /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,        /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTGUN,         /* device is a Light Gun */
    0,                               /* NO output bits */
    joyport_lightpen_set_enabled,    /* device enable/disable function */
    lightpen_digital_val,            /* digital line read function */
    NULL,                            /* NO digital line store function */
    NULL,                            /* NO pot-x read function */
    NULL,                            /* NO pot-y read function */
    NULL,                            /* NO powerup function */
    lightpen_write_snapshot,         /* device write snapshot function */
    lightpen_read_snapshot,          /* device read snapshot function */
    NULL,                            /* NO device hook function */
    0                                /* NO device hook function mask */
};

static joyport_t inkwell_lightpen_joyport_device = {
    "Light Pen (Inkwell)",        /* name of the device */
    JOYPORT_RES_ID_MOUSE,         /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,          /* device is a lightpen */
    JOYPORT_POT_REQUIRED,         /* device uses the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,     /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTPEN,      /* device is a Light Pen */
    0,                            /* NO output bits */
    joyport_lightpen_set_enabled, /* device enable/disable function */
    lightpen_digital_val,         /* digital line read function */
    NULL,                         /* NO digital line store function */
    NULL,                         /* NO pot-x read function */
    lightpen_read_button_y,       /* pot-y read function */
    NULL,                         /* NO powerup function */
    lightpen_write_snapshot,      /* device write snapshot function */
    lightpen_read_snapshot,       /* device read snapshot function */
    NULL,                         /* NO device hook function */
    0                             /* NO device hook function mask */
};

#ifdef JOYPORT_EXPERIMENTAL_DEVICES
static joyport_t gun_stick_joyport_device = {
    "Light Gun (Gun Stick)",      /* name of the device */
    JOYPORT_RES_ID_MOUSE,         /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_LIGHTPEN,          /* device is a lightpen */
    JOYPORT_POT_OPTIONAL,         /* device does NOT use the potentiometer lines */
    JOYSTICK_ADAPTER_ID_NONE,     /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_LIGHTGUN,      /* device is a Light Gun */
    0,                            /* NO output bits */
    joyport_lightpen_set_enabled, /* device enable/disable function */
    lightpen_digital_val,         /* digital line read function */
    NULL,                         /* NO digital line store function */
    NULL,                         /* NO pot-x read function */
    NULL,                         /* NO pot-y read function */
    NULL,                         /* NO powerup function */
    lightpen_write_snapshot,      /* device write snapshot function */
    lightpen_read_snapshot,       /* device read snapshot function */
    NULL,                         /* NO device hook function */
    0                             /* NO device hook function mask */
};
#endif

static int lightpen_joyport_register(void)
{
    if (joyport_device_register(JOYPORT_ID_LIGHTPEN_U, &lightpen_u_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_LIGHTPEN_L, &lightpen_l_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_LIGHTPEN_DATEL, &lightpen_datel_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_LIGHTGUN_Y, &magnum_light_phaser_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_LIGHTGUN_L, &stack_light_rifle_joyport_device) < 0) {
        return -1;
    }
#ifdef JOYPORT_EXPERIMENTAL_DEVICES
    if (joyport_device_register(JOYPORT_ID_LIGHTGUN_GUNSTICK, &gun_stick_joyport_device) < 0) {
        return -1;
    }
#endif
    return joyport_device_register(JOYPORT_ID_LIGHTPEN_INKWELL, &inkwell_lightpen_joyport_device);
}

/* --------------------------------------------------------- */

int lightpen_resources_init(void)
{
    if (lightpen_joyport_register() < 0) {
        return -1;
    }
    return 0;
}

/* --------------------------------------------------------- */
/* Main API */

void lightpen_init(void)
{
    int i;

    for (i = 0; i < (MAX_WINDOW_NUM + 1); ++i) {
        chip_timing_callback[i] = NULL;
    }

    chip_trigger_callback = NULL;
}

int lightpen_register_timing_callback(lightpen_timing_callback_ptr_t timing_callback, int window)
{
    if ((window < 0) || (window > MAX_WINDOW_NUM)) {
        return -1;
    }

    chip_timing_callback[window] = timing_callback;
    return 0;
}

int lightpen_register_trigger_callback(lightpen_trigger_callback_ptr_t trigger_callback)
{
    chip_trigger_callback = trigger_callback;
    return 0;
}

/* Update lightpen coordinates and button status. Called at the end of each frame.
   For x128, window 1 is VICII, window 0 is VDC. Others always use window 0.
   x and y are the canvas coordinates; double size, and offsets are removed in the arch side.
   Negative values of x and/or y can be used to indicate that the pointer is off the (emulated) screen. */
void lightpen_update(int window, int x, int y, int buttons)
{
    CLOCK pulse_time;

    if ((window < 0) || (window > MAX_WINDOW_NUM)) {
        return;
    }

    if ((!lightpen_enabled) || (chip_timing_callback[window] == NULL) || (chip_trigger_callback == NULL)) {
        return;
    }

    lightpen_update_buttons(buttons);

    if ((x < 0) || (y < 0)) {
        return;
    }

    x += lp_type[lightpen_type].x_offset;
    y += lp_type[lightpen_type].y_offset;

    if ((x < 0) || (y < 0)) {
        return;
    }

    if ((lp_type[lightpen_type].type == PEN) && !(buttons & LP_HOST_BUTTON_1)) {
        return;
    }

    pulse_time = chip_timing_callback[window](x, y);

    if (pulse_time > 0) {
        chip_trigger_callback(pulse_time);
    }
}

/* --------------------------------------------------------- */

/* LIGHTPEN snapshot module format:

   type  | name     | description
   ------------------------------
   BYTE  | value    | lightpen return value
   BYTE  | type     | lightpen type
   DWORD | buttons  | buttons state
   DWORD | button y | button Y state
   DWORD | button x | button X state
 */

static const char snap_module_name[] = "LIGHTPEN";
#define SNAP_MAJOR   0
#define SNAP_MINOR   0

static int lightpen_write_snapshot(struct snapshot_s *s, int port)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_B(m, lightpen_value) < 0
        || SMW_B(m, (uint8_t)lightpen_type) < 0
        || SMW_DW(m, (uint32_t)lightpen_buttons) < 0
        || SMW_DW(m, (uint32_t)lightpen_button_y) < 0
        || SMW_DW(m, (uint32_t)lightpen_button_x) < 0) {
        snapshot_module_close(m);
        return -1;
    }

    return snapshot_module_close(m);
}

static int lightpen_read_snapshot(struct snapshot_s *s, int port)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept versions higher than current */
    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_B(m, &lightpen_value) < 0
        || SMR_B_INT(m, &lightpen_type) < 0
        || SMR_DW_INT(m, &lightpen_buttons) < 0
        || SMR_DW_INT(m, &lightpen_button_y) < 0
        || SMR_DW_INT(m, &lightpen_button_x) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}
#endif
