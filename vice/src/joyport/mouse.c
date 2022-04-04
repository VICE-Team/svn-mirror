/*
 * mouse.c - Common mouse handling
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *
 * NEOS & Amiga mouse and paddle support by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

/* Important requirements to arch specific mouse drivers for proper operation:
 * - mousedrv_get_x and mousedrv_get_y MUST return a value with at
 *   least 16 valid bits in LSB.
 * - mousedrv_get_timestamp MUST give the time stamp when the last
 *   mouse movement happened. A button press is not a movement!
*/

/* #define DEBUG_MOUSE */

#ifdef DEBUG_MOUSE
#define DBG(_x_)  log_debug _x_
#else
#define DBG(_x_)
#endif

#include <stdlib.h> /* abs */
#include <string.h> /* memset */
#include <math.h>   /* fabsf */
#include "vice.h"

#include "alarm.h"
#include "archdep.h"
#include "cmdline.h"
#include "joyport.h"
#include "joystick.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "snapshot.h"
#include "vsyncapi.h"
#include "ds1202_1302.h"

#include "mouse_1351.h"
#include "mouse_neos.h"
#include "mouse_paddle.h"
#include "mouse_quadrature.h"

/* Log descriptor.  */
#ifdef DEBUG_MOUSE
static log_t mouse_log = LOG_ERR;
#endif

/* Weird trial and error based number here :( larger causes mouse jumps. */
#define MOUSE_MAX_DIFF 63.0f

static float mouse_move_x = 0.0f;
static float mouse_move_y = 0.0f;
static tick_t mouse_timestamp = 0;

/******************************************************************************/

/* FIXME: "private" global variables for mouse - we should get rid of most of these */

int last_mouse_x = 0;
int last_mouse_y = 0;
uint8_t mouse_digital_val = 0;
int16_t mouse_x = 0;
int16_t mouse_y = 0;

/* --------------------------------------------------------- */
/* extern variables (used elsewhere in the codebase) */

int _mouse_enabled = 0;

/* Use xvic defaults, if resources get registered the factory
   default will overwrite these */
int mouse_type = MOUSE_TYPE_PADDLE;

/* --------------------------------------------------------- */
/* quadrature encoding mice support (currently experimental) */

/* FIXME: all of the following variables should get moved into mouse_quadrature.c
          and made private to it */

/* The mousedev only updates its returned coordinates at certain *
 * frequency. We try to estimate this interval by timestamping unique
 * successive readings. The estimated interval is then converted from
 * vsynchapi units to emulated cpu cycles which in turn are used to
 * clock the quardrature emulation. */
tick_t mouse_latest_os_timestamp = 0; /* in vsynchapi units */
/* The mouse coordinates returned from the latest unique mousedrv
 * reading */
int16_t mouse_latest_x = 0;
int16_t mouse_latest_y = 0;

extern CLOCK update_x_emu_iv;      /* in cpu cycle units */
extern CLOCK update_y_emu_iv;      /* in cpu cycle units */
extern CLOCK next_update_x_emu_ts; /* in cpu cycle units */
extern CLOCK next_update_y_emu_ts; /* in cpu cycle units */

extern int sx, sy;

/* the ratio between emulated cpu cycles and vsynchapi time units */
float emu_units_per_os_units;

extern uint8_t quadrature_x;
extern uint8_t quadrature_y;

extern uint8_t polled_joyval;

static const uint8_t amiga_mouse_table[4] = { 0x0, 0x1, 0x5, 0x4 };
static const uint8_t st_mouse_table[4] = { 0x0, 0x2, 0x3, 0x1 };

int update_limit = 512;

/* --------------------------------------------------------- */

/* this is called by the UI to move the mouse position */
void mouse_move(float dx, float dy)
{
    /* Capture the relative mouse movement to be processed later in mouse_poll() */
    mouse_move_x += dx;
    mouse_move_y -= dy;
    mouse_timestamp = tick_now();
    DBG(("mouse_move dx:%f dy:%f x:%f y:%f", dx, dy, mouse_move_x, mouse_move_y));
}

/* used by the individual devices to get the mouse position */
void mouse_get_int16(int16_t *x, int16_t *y)
{
    *x = (int16_t)mouse_x;
    *y = (int16_t)mouse_y;
}

/*
    to avoid strange side effects two things are done here:

    - max delta is limited to MOUSE_MAX_DIFF
    - if the delta is limited, then the current position is linearly
      interpolated towards the real position using MOUSE_MAX_DIFF for the axis
      with the largest delta
*/
static void mouse_move_apply_limit(void)
{
    /* Limit the distance that mouse_x/y can have changed since last poll.
     * If we don't do this the mouse moment overflows and a large move
     * can result in either a move in the opposite direction, or the wrong
     * move in the right direction.
     */

    if (fabsf(mouse_move_x) >= fabsf(mouse_move_y)) {
        if (mouse_move_x > MOUSE_MAX_DIFF) {
            mouse_move_y *= MOUSE_MAX_DIFF / mouse_move_x;
            mouse_move_x = MOUSE_MAX_DIFF;
        } else if (mouse_move_x < -MOUSE_MAX_DIFF) {
            mouse_move_y *= -MOUSE_MAX_DIFF / mouse_move_x;
            mouse_move_x = -MOUSE_MAX_DIFF;
        }
    } else {
        if (mouse_move_y > MOUSE_MAX_DIFF) {
            mouse_move_x *= MOUSE_MAX_DIFF / mouse_move_y;
            mouse_move_y = MOUSE_MAX_DIFF;
        } else if (mouse_move_y < -MOUSE_MAX_DIFF) {
            mouse_move_x *= -MOUSE_MAX_DIFF / mouse_move_y;
            mouse_move_y = -MOUSE_MAX_DIFF;
        }
    }
}

/* poll the mouse, returns the digital joyport lines */

/* FIXME: at least the quadrature specific stuff should get moved out of this
          and made private to mouse_quadrature.c */
uint8_t mouse_poll(void)
{
    int16_t delta_x, delta_y;

    int16_t new_x, new_y;
    tick_t os_now, os_iv, os_iv2;
    CLOCK emu_now, emu_iv, emu_iv2;
    int diff_x, diff_y;

    DBG(("mouse_poll"));

    /* Ensure the mouse hasn't moved too far since the last poll */
    mouse_move_apply_limit();

    /* Capture an integer representation of how far the mouse has moved */
    delta_x = (int16_t)mouse_move_x;
    delta_y = (int16_t)mouse_move_y;

    /* Update the view of where the mouse is based on the accumulated delta */
    mouse_x += delta_x;
    mouse_y += delta_y;

    /* Subtract the int delta from the floating point, preserving fractional elemement */
    mouse_move_x -= delta_x;
    mouse_move_y -= delta_y;

    /* OK - on with the show, get new mouse values */
    new_x = (int16_t)mouse_x;
    new_y = (int16_t)mouse_y;

    /* range of new_x and new_y are [0,63] */
    /* fetch now for both emu and os */
    os_now = mouse_timestamp;
    emu_now = maincpu_clk;

    /* update x-wheel until we're ahead */
    while (((mouse_latest_x ^ last_mouse_x) & 0xffff) && next_update_x_emu_ts <= emu_now) {
        last_mouse_x += sx;
        next_update_x_emu_ts += update_x_emu_iv;
    }

    /* update y-wheel until we're ahead */
    while (((mouse_latest_y ^ last_mouse_y) & 0xffff) && next_update_y_emu_ts <= emu_now) {
        last_mouse_y -= sy;
        next_update_y_emu_ts += update_y_emu_iv;
    }

    /* check if the new values belong to a new mouse reading */
    if (mouse_latest_os_timestamp == 0) {
        /* only first time, init stuff */
        last_mouse_x = mouse_latest_x = new_x;
        last_mouse_y = mouse_latest_y = new_y;
        mouse_latest_os_timestamp = os_now;
    } else if (os_now != mouse_latest_os_timestamp && (new_x != mouse_latest_x || new_y != mouse_latest_y)) {
        /* yes, we have a new unique mouse coordinate reading */

        /* calculate the interval between the latest two mousedrv
         * updates in emulated cycles */
        os_iv = os_now - mouse_latest_os_timestamp;
        /* FIXME: call function only once */
        if (os_iv > tick_per_second()) {
            os_iv = tick_per_second(); /* more than a second response time?! */
        }
        emu_iv = (CLOCK)((float)os_iv * emu_units_per_os_units);
        /* FIXME: call function only once, remove cast */
        if (emu_iv > (unsigned long)machine_get_cycles_per_frame() * 2) {
            emu_iv = (CLOCK)machine_get_cycles_per_frame() * 2;   /* move in not more than 2 frames */
        }
#ifdef DEBUG_MOUSE
        log_message(mouse_log,
                    "New interval os_now %u, os_iv %u, emu_iv %lu",
                    os_now, os_iv, emu_iv);
#endif

        /* Let's set up quadrature emulation */
        diff_x = (int16_t)(new_x - last_mouse_x);
        diff_y = (int16_t)(new_y - last_mouse_y);

        if (diff_x != 0) {
            sx = diff_x >= 0 ? 1 : -1;
            /* lets calculate the interval between x-quad rotations */
            update_x_emu_iv = emu_iv / (CLOCK)abs(diff_x);
            /* and the emulated cpu cycle count when to do the first one */
            next_update_x_emu_ts = emu_now;
        } else {
            sx = 0;
            update_x_emu_iv = (CLOCK)update_limit;
        }
        if (diff_y != 0) {
            sy = diff_y >= 0 ? -1 : 1;
            /* lets calculate the interval between y-quad rotations */
            update_y_emu_iv = (emu_iv / (CLOCK)abs(diff_y));
            /* and the emulated cpu cycle count when to do the first one */
            next_update_y_emu_ts = emu_now;
        } else {
            sy = 0;
            update_y_emu_iv = (CLOCK)update_limit;
        }
        if (update_x_emu_iv < (unsigned int)update_limit) {
            if (update_x_emu_iv) {
                update_y_emu_iv = update_y_emu_iv * (CLOCK)update_limit / update_x_emu_iv;
            }
            update_x_emu_iv = (CLOCK)update_limit;
        }
        if (update_y_emu_iv < (unsigned int)update_limit) {
            if (update_y_emu_iv) {
                update_x_emu_iv = update_x_emu_iv * (CLOCK)update_limit / update_y_emu_iv;
            }
            update_y_emu_iv = (CLOCK)update_limit;
        }

#ifdef DEBUG_MOUSE
        log_message(mouse_log, "cpu %lu iv %lu,%lu old %d,%d new %d,%d",
                    emu_now, update_x_emu_iv, update_y_emu_iv,
                    mouse_latest_x, mouse_latest_y, new_x, new_y);
#endif

        /* The mouse read is probably old. Do the movement since then */
        os_iv2 = tick_now_delta(os_now);
        /* FIXME: call function only once */
        if (os_iv2 > tick_per_second()) {
            os_iv2 = tick_per_second(); /* more than a second response time?! */
        }
        emu_iv2 = (CLOCK)((float)os_iv2 * emu_units_per_os_units);
        /* FIXME: call function only once, remove cast */
        if (emu_iv2 > (unsigned long)machine_get_cycles_per_second()) {
            emu_iv2 = (CLOCK)machine_get_cycles_per_second();   /* more than a second? */
        }

        /* update x-wheel until we're ahead */
        while (((new_x ^ last_mouse_x) & 0xffff) && next_update_x_emu_ts < emu_now + emu_iv2) {
            last_mouse_x += sx;
            next_update_x_emu_ts += update_x_emu_iv;
        }

        /* update y-wheel until we're ahead */
        while (((new_y ^ last_mouse_y) & 0xffff) && next_update_y_emu_ts <= emu_now + emu_iv2) {
            last_mouse_y -= sy;
            next_update_y_emu_ts += update_y_emu_iv;
        }

        /* store the new coordinates for next time */
        mouse_latest_x = new_x;
        mouse_latest_y = new_y;
        mouse_latest_os_timestamp = os_now;
    }

    if ((quadrature_x != ((last_mouse_x >> 1) & 3)) || (quadrature_y != ((~last_mouse_y >> 1) & 3))) {
        /* keep within range */
        quadrature_x = (last_mouse_x >> 1) & 3;
        quadrature_y = (~last_mouse_y >> 1) & 3;

        switch (mouse_type) {
            case MOUSE_TYPE_AMIGA:
                polled_joyval = (uint8_t)((amiga_mouse_table[quadrature_x] << 1) | amiga_mouse_table[quadrature_y] | 0xf0);
                break;
            case MOUSE_TYPE_CX22:
                polled_joyval = (uint8_t)(((quadrature_y & 1) << 3) | ((sy > 0) << 2) | ((quadrature_x & 1) << 1) | (sx > 0) | 0xf0);
                break;
            case MOUSE_TYPE_ST:
                polled_joyval =(uint8_t)(st_mouse_table[quadrature_x] | (st_mouse_table[quadrature_y] << 2) | 0xf0);
                break;
            default:
                polled_joyval = 0xff;
        }
    }
    return polled_joyval;
}


void mouse_init(void)
{
    /* FIXME: some of these can perhaps be moved into individual devices */
    emu_units_per_os_units = (float)(machine_get_cycles_per_second() / tick_per_second());
    update_limit = (int)(machine_get_cycles_per_frame() / 31 / 2);
#ifdef DEBUG_MOUSE
    mouse_log = log_open("Mouse");
    log_message(mouse_log, "cpu cycles / time unit %.5f",
                emu_units_per_os_units);
#endif

    mouse_amiga_st_init();
    mouse_neos_init();

    mousedrv_init();
}

void mouse_shutdown(void)
{
    smart_mouse_shutdown();
}

/*--------------------------------------------------------------------------*/
/* Main API */

/* FIXME: some devices do not use mouse_digital_val for (some) digital lines,
          that makes the joystick lines indicators in the UIs not work */

static void mouse_button_left(int pressed)
{
    uint8_t old_val = mouse_digital_val;

    switch (mouse_type) {
        case MOUSE_TYPE_1351:
        case MOUSE_TYPE_SMART:
        case MOUSE_TYPE_MICROMYS:
            mouse_1351_button_left(pressed);
            break;
        case MOUSE_TYPE_KOALAPAD:
        case MOUSE_TYPE_PADDLE:
            paddles_button_left(pressed);
            break;
        case MOUSE_TYPE_NEOS:
            mouse_neos_button_left(pressed);
            break;
        case MOUSE_TYPE_AMIGA:
        case MOUSE_TYPE_ST:
            mouse_amiga_st_button_left(pressed);
            break;
        default:
            break;
    }

    if (old_val == mouse_digital_val || mouse_type == -1) {
        return;
    }
    joyport_display_joyport(mouse_type_to_id(mouse_type), (uint16_t)mouse_digital_val);
}

static void mouse_button_right(int pressed)
{
    uint8_t old_val = mouse_digital_val;

    switch (mouse_type) {
        case MOUSE_TYPE_1351:
        case MOUSE_TYPE_SMART:
        case MOUSE_TYPE_MICROMYS:
            mouse_1351_button_right(pressed);
            break;
        case MOUSE_TYPE_KOALAPAD:
        case MOUSE_TYPE_PADDLE:
            paddles_button_right(pressed);
            break;
        case MOUSE_TYPE_NEOS:
            mouse_neos_button_right(pressed);
            break;
        case MOUSE_TYPE_AMIGA:
        case MOUSE_TYPE_ST:
            mouse_amiga_st_button_right(pressed);
            break;
        default:
            break;
    }
    if (old_val == mouse_digital_val || mouse_type == -1) {
        return;
    }
    joyport_display_joyport(mouse_type_to_id(mouse_type), (uint16_t)mouse_digital_val);
}

static void mouse_button_middle(int pressed)
{
    uint8_t old_val = mouse_digital_val;

    switch (mouse_type) {
        case MOUSE_TYPE_MICROMYS:
            micromys_mouse_button_middle(pressed);
            break;
        case MOUSE_TYPE_AMIGA:
        case MOUSE_TYPE_ST:
            mouse_amiga_st_button_right(pressed);
            break;
        default:
            break;
    }
    if (old_val == mouse_digital_val || mouse_type == -1) {
        return;
    }
    joyport_display_joyport(mouse_type_to_id(mouse_type), (uint16_t)mouse_digital_val);
}

static void mouse_button_up(int pressed)
{
    switch (mouse_type) {
        case MOUSE_TYPE_MICROMYS:
            micromys_mouse_button_up(pressed);
            break;
        default:
            break;
    }
}

static void mouse_button_down(int pressed)
{
    switch (mouse_type) {
        case MOUSE_TYPE_MICROMYS:
            micromys_mouse_button_down(pressed);
            break;
        default:
            break;
    }
}

/*--------------------------------------------------------------------------*/

void mouse_set_machine_parameter(long clock_rate)
{
    neos_mouse_set_machine_parameter(clock_rate);
}

/*--------------------------------------------------------------------------*/

typedef struct mt_id_s {
    int mt;
    int id;
} mt_id_t;

static const mt_id_t mt_id[] = {
    { MOUSE_TYPE_PADDLE,   JOYPORT_ID_PADDLES },
    { MOUSE_TYPE_MF_JOY,   JOYPORT_ID_MF_JOYSTICK },
    { MOUSE_TYPE_1351,     JOYPORT_ID_MOUSE_1351 },
    { MOUSE_TYPE_NEOS,     JOYPORT_ID_MOUSE_NEOS },
    { MOUSE_TYPE_AMIGA,    JOYPORT_ID_MOUSE_AMIGA },
    { MOUSE_TYPE_CX22,     JOYPORT_ID_MOUSE_CX22 },
    { MOUSE_TYPE_ST,       JOYPORT_ID_MOUSE_ST },
    { MOUSE_TYPE_SMART,    JOYPORT_ID_MOUSE_SMART },
    { MOUSE_TYPE_MICROMYS, JOYPORT_ID_MOUSE_MICROMYS },
    { MOUSE_TYPE_KOALAPAD, JOYPORT_ID_KOALAPAD },
    { -1,                  -1 }
};

int mouse_id_to_type(int id)
{
    int i;

    for (i = 0; mt_id[i].mt != -1; ++i) {
        if (mt_id[i].id == id) {
            return mt_id[i].mt;
        }
    }
    return -1;
}

int mouse_type_to_id(int mt)
{
    int i;

    for (i = 0; mt_id[i].mt != -1; ++i) {
        if (mt_id[i].mt == mt) {
            return mt_id[i].id;
        }
    }
    return -1;
}

/*--------------------------------------------------------------------------*/

static int mouse_joyport_register(void)
{
    DBG(("mouse_joyport_register\n"));
    if (joyport_device_register(JOYPORT_ID_PADDLES, &paddles_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MF_JOYSTICK, &mf_joystick_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_1351, &mouse_1351_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_NEOS, &mouse_neos_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_AMIGA, &mouse_amiga_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_CX22, &mouse_cx22_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_ST, &mouse_st_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_SMART, &mouse_smart_joyport_device) < 0) {
        return -1;
    }
    if (joyport_device_register(JOYPORT_ID_MOUSE_MICROMYS, &mouse_micromys_joyport_device) < 0) {
        return -1;
    }
    return joyport_device_register(JOYPORT_ID_KOALAPAD, &koalapad_joyport_device);
}

/* --------------------------------------------------------- */
/* Resources & cmdline */

static int set_mouse_enabled(int val, void *param)
{
    if (_mouse_enabled == val) {
        return 0;
    }

    _mouse_enabled = val ? 1 : 0;

    mousedrv_mouse_changed();

    /* FIXME: these should move into individual devices */
    mouse_get_int16(&mouse_latest_x, &mouse_latest_y);
    last_mouse_x = mouse_latest_x;
    last_mouse_y = mouse_latest_y;
    mouse_latest_os_timestamp = 0;

    mouse_neos_set_enabled(_mouse_enabled);

    if (mouse_type != -1) {
        joyport_display_joyport(mouse_type_to_id(mouse_type), 0);
    }
    return 0;
}

static const resource_int_t resources_int[] = {
    { "Mouse", ARCHDEP_MOUSE_ENABLE_DEFAULT, RES_EVENT_SAME, NULL,
      &_mouse_enabled, set_mouse_enabled, NULL },
    RESOURCE_INT_LIST_END
};

static const mouse_func_t mouse_funcs =
{
    mouse_button_left,
    mouse_button_right,
    mouse_button_middle,
    mouse_button_up,
    mouse_button_down
};

int mouse_resources_init(void)
{
    DBG(("mouse_resources_init\n"));
    if (mouse_joyport_register() < 0) {
        return -1;
    }
    if (resources_register_int(resources_int) < 0) {
        return -1;
    }

    if (paddles_resources_init() < 0) {
        return -1;
    }

    if (smart_mouse_resources_init() < 0) {
        return -1;
    }

    return mousedrv_resources_init(&mouse_funcs);
}

static const cmdline_option_t cmdline_options[] =
{
    { "-mouse", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "Mouse", (void *)1,
      NULL, "Enable mouse grab" },
    { "+mouse", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "Mouse", (void *)0,
      NULL, "Disable mouse grab" },
    CMDLINE_LIST_END
};

int mouse_cmdline_options_init(void)
{
    if (cmdline_register_options(cmdline_options) < 0) {
        return -1;
    }

    if (paddles_cmdline_options_init() < 0) {
        return -1;
    }

    if (smart_mouse_cmdline_options_init() < 0) {
        return -1;
    }

    return mousedrv_cmdline_options_init();
}
