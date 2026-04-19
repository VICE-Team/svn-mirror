/*
 * mouse_digital.c - Digital "joy" mouse handling
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

   cport | joy mouse    | I/O
   --------------------------
     1   | D0           |  I
     2   | D1           |  I
     3   | D2           |  I
     4   | D3           |  I

     6   | left button  |  I    ("Fire")
     7   | +5VDC        |  Power
     8   | GND          |  Ground
     9   | right button |  I    ("POTX")

   Works on:
   - native joystick port(s) (x64/x64sc/xscpu64/x128/xvic/xcbm5x0)
   - sidcart joystick adapter port (xplus4)
*/

/* #define DEBUG_DIGITAL */

#ifdef DEBUG_DIGITAL
#define DBG(_x_) log_printf  _x_
#else
#define DBG(_x_)
#endif

#include <stdlib.h> /* abs */

#include "vice.h"

#include "joyport.h"
#include "maincpu.h"
#include "log.h"
#include "mouse.h"
#include "mousedrv.h"
#include "snapshot.h"
#include "vsyncapi.h"

#include "mouse_digital.h"

/******************************************************************************/

static uint8_t mouse_digital_val = 0;

static int16_t mouse_latest_x;
static int16_t mouse_latest_y;

static CLOCK clock_vert_move = 0;
static CLOCK clock_hori_move = 0;

static int digital_buttons;

static int digital_lastx;
static int digital_lasty;
static int digital_newx;
static int digital_newy;

static int digital_state = 0xff;

/******************************************************************************/

void digital_mouse_set_machine_parameter(long clock_rate)
{
    /*digital_time_out_cycles = (CLOCK)((clock_rate / 10000) * 2);*/
}

static void digital_get_new_movement(void)
{
    int16_t newx16, newy16;

    digital_lastx = digital_newx;
    digital_lasty = digital_newy;

    mouse_get_raw_int16(&newx16, &newy16);
    digital_newx = newx16;
    digital_newy = newy16;

    /*DBG(("digital_get_new_movement %08ld %dx%d new: %dx%d last: %dx%d",
         maincpu_clk, newx16, newy16, digital_newx, digital_newy, digital_lastx ,digital_lasty)); */
}

#define MOVE_CLOCK_DELAY (((312 * 65) * 3) / 2)

#define MOVE_MIN_DIFF   8

uint8_t digital_mouse_read(void)
{
    digital_get_new_movement();

    if ((digital_newy - digital_lasty) > MOVE_MIN_DIFF) {
        DBG(("up %d,%d", digital_newy, digital_lasty));
        digital_state &= ~(1 << 0); /* up */
        clock_vert_move = maincpu_clk + MOVE_CLOCK_DELAY;
    } else if ((digital_lasty - digital_newy) > MOVE_MIN_DIFF) {
        DBG(("down %d,%d", digital_newy, digital_lasty));
        digital_state &= ~(1 << 1); /* down */
        clock_vert_move = maincpu_clk + MOVE_CLOCK_DELAY;
    }
    if ((digital_lastx - digital_newx) > MOVE_MIN_DIFF) {
        DBG(("left %d,%d", digital_newx, digital_lastx));
        digital_state &= ~(1 << 2); /* left */
        clock_hori_move = maincpu_clk + MOVE_CLOCK_DELAY;
    } else if ((digital_newx - digital_lastx) > MOVE_MIN_DIFF) {
        DBG(("right %d,%d", digital_newx, digital_lastx));
        digital_state &= ~(1 << 3); /* right */
        clock_hori_move = maincpu_clk + MOVE_CLOCK_DELAY;
    }

    if (maincpu_clk > clock_vert_move) {
        digital_state |= 0x03;
    }
    if (maincpu_clk > clock_hori_move) {
        digital_state |= 0x0c;
    }
    DBG(("digital_get_new_movement %08ld new: %dx%d last: %dx%d clock_vert_move %ld clock_hori_move %ld",
         maincpu_clk, digital_newx, digital_newy, digital_lastx ,digital_lasty, clock_vert_move, clock_hori_move));

    return digital_state;
}

static uint8_t joyport_mouse_digital_value(int port)
{
    uint8_t retval = 0xff;

    if (_mouse_enabled) {

        /* we need to poll here, else the mouse can not move if the C64 code
           does not read POTX */
        mouse_poll();

        retval = (uint8_t)((~mouse_digital_val) & digital_mouse_read());

        if (retval != (uint8_t)~mouse_digital_val) {
            joyport_display_joyport(port, JOYPORT_ID_MOUSE_DIGITAL, (uint16_t)(~retval));
        }
    }
    return retval;
}

void mouse_digital_button_right(int pressed)
{
    if (pressed) {
        digital_buttons |= JOYPORT_FIRE_2;
    } else {
        digital_buttons &= ~JOYPORT_FIRE_2;
    }
}

void mouse_digital_button_left(int pressed)
{
    if (pressed) {
        mouse_digital_val |= JOYPORT_FIRE_1;
    } else {
        mouse_digital_val &= (uint8_t)~JOYPORT_FIRE_1;
    }
}


static uint8_t joyport_mouse_digital_potx(int port)
{
    DBG(("joyport_mouse_digital_potx %02x", digital_buttons));
    return _mouse_enabled ? ((digital_buttons & JOYPORT_FIRE_2) ? 0 : 0xff) : 0xff;
}

void mouse_digital_set_enabled(int enabled)
{
    if (enabled) {
        mouse_get_raw_int16(&mouse_latest_x, &mouse_latest_y);
        digital_lastx = mouse_latest_x;
        digital_lasty = mouse_latest_y;
    }
}

static int joyport_mouse_digital_set_enabled(int port, int joyportid)
{
    int mt;

    mouse_reset();
    digital_lastx = mouse_latest_x;
    digital_lasty = mouse_latest_y;

    if (joyportid == JOYPORT_ID_NONE) {
        /* disabled, set mouse type to invalid/none */
        mouse_type = -1;
        return 0;
    }

    /* convert joyport ID to mouse type */
    mt = mouse_id_to_type(joyportid);

    if (mt == -1) {
        return -1;
    }

    DBG(("joyport_mouse_digital_set_enabled mt:%d,mouse_type:%d",mt,mouse_type));
    if (mt == mouse_type) {
        return 0;
    }

    mouse_type = mt;

    return 0;
}


/* Some prototypes are needed */
int mouse_digital_write_snapshot(struct snapshot_s *s, int port);
int mouse_digital_read_snapshot(struct snapshot_s *s, int port);

static joyport_t mouse_digital_joyport_device = {
    "Mouse (Joystick Mouse)",       /* name of the device */
    JOYPORT_RES_ID_MOUSE,           /* device uses the mouse for input, only 1 mouse type device can be active at the same time */
    JOYPORT_IS_NOT_LIGHTPEN,        /* device is NOT a lightpen */
    JOYPORT_POT_OPTIONAL,           /* device uses the potentiometer line for the right button, but could work without it */
    JOYPORT_5VDC_REQUIRED,          /* device NEEDS +5VDC to work */
    JOYSTICK_ADAPTER_ID_NONE,       /* device is NOT a joystick adapter */
    JOYPORT_DEVICE_MOUSE,           /* device is a Mouse */
    0x10,                           /* bit 4 is an output bit */
    joyport_mouse_digital_set_enabled, /* device enable/disable function */
    joyport_mouse_digital_value,    /* digital line read function */
    NULL,                           /* digital line store function */
    joyport_mouse_digital_potx,     /* pot-x read function */
    NULL,                           /* NO pot-y read function */
    NULL,                           /* NO powerup function */
    mouse_digital_write_snapshot,   /* device write snapshot function */
    mouse_digital_read_snapshot,    /* device read snapshot function */
    NULL,                           /* NO device hook function */
    0                               /* NO device hook function mask */
};

int mouse_digital_register(void)
{
    DBG(("mouse_digital_register"));
    return joyport_device_register(JOYPORT_ID_MOUSE_DIGITAL, &mouse_digital_joyport_device);
}


void mouse_digital_init(void)
{
    DBG(("mouse_digital_init"));
    digital_buttons = 0;
    clock_vert_move = 0;
    clock_hori_move = 0;
}

/* --------------------------------------------------------- */

/* MOUSE_DIGITAL snapshot module format:

   type  | name                 | description
   ------------------------------------------
   BYTE  | digital value        | digital pins return value
   DWORD | buttons              | buttons state
   BYTE  | new x                | X
   BYTE  | new y                | Y
   BYTE  | last x               | last X
   BYTE  | last y               | last Y
   DWORD | digital state        | state
   DWORD | clock_vert_move      | last trigger clock (Vertical)
   DWORD | clock_hori_move      | last trigger clock (Horizontal)
 */

static const char mouse_digital_snap_module_name[] = "MOUSE_DIGITAL";
#define MOUSE_DIGITAL_VER_MAJOR   1
#define MOUSE_DIGITAL_VER_MINOR   0

int mouse_digital_write_snapshot(struct snapshot_s *s, int port)
{
    snapshot_module_t *m;

    m = snapshot_module_create(s, mouse_digital_snap_module_name, MOUSE_DIGITAL_VER_MAJOR, MOUSE_DIGITAL_VER_MINOR);

    if (m == NULL) {
        return -1;
    }

    if (write_mouse_common_snapshot(m) < 0) {
        goto fail;
    }

    if (0
        || SMW_B(m, mouse_digital_val) < 0
        || SMW_DW(m, (uint32_t)digital_buttons) < 0
        || SMW_DW(m, digital_newx) < 0
        || SMW_DW(m, digital_newy) < 0
        || SMW_DW(m, digital_lastx) < 0
        || SMW_DW(m, digital_lasty) < 0
        || SMW_DW(m, (uint32_t)digital_state) < 0
        || SMW_DW(m, (uint32_t)clock_vert_move) < 0
        || SMW_DW(m, (uint32_t)clock_hori_move) < 0
        ) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

int mouse_digital_read_snapshot(struct snapshot_s *s, int port)
{
    uint8_t major_version, minor_version;
    snapshot_module_t *m;
    uint32_t tmpc1;
    uint32_t tmpc2;
    int tmp_digital_state;

    m = snapshot_module_open(s, mouse_digital_snap_module_name, &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    /* Do not accept higher versions than current */
    if (snapshot_version_is_bigger(major_version, minor_version, MOUSE_DIGITAL_VER_MAJOR, MOUSE_DIGITAL_VER_MINOR)) {
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (read_mouse_common_snapshot(m) < 0) {
        goto fail;
    }

    if (0
        || SMR_B(m, &mouse_digital_val) < 0
        || SMR_DW_INT(m, &digital_buttons) < 0
        || SMR_DW_INT(m, &digital_newx) < 0
        || SMR_DW_INT(m, &digital_newy) < 0
        || SMR_DW_INT(m, &digital_lastx) < 0
        || SMR_DW_INT(m, &digital_lasty) < 0
        || SMR_DW_INT(m, &tmp_digital_state) < 0
        || SMR_DW(m, &tmpc1) < 0
        || SMR_DW(m, &tmpc2) < 0
    ) {
        goto fail;
    }

    clock_vert_move = (CLOCK)tmpc1;
    clock_hori_move = (CLOCK)tmpc2;
    digital_state = tmp_digital_state;

    return snapshot_module_close(m);

fail:
    snapshot_module_close(m);
    return -1;
}

