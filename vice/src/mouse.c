/*
 * mouse.c - Common mouse handling
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "vice.h"

#include "alarm.h"
#include "cmdline.h"
#include "joystick.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "translate.h"

/* --------------------------------------------------------- */
/* extern variables */

int _mouse_enabled = 0;
int mouse_port = 1;
int mouse_type;


/* --------------------------------------------------------- */
/* NEOS mouse */

#define NEOS_RESET_CLK 100
struct alarm_s *neosmouse_alarm;

static int neos_and_amiga_buttons;
static int neos_prev;

static BYTE neos_x;
static BYTE neos_y;
static BYTE neos_lastx;
static BYTE neos_lasty;

enum {
    NEOS_IDLE = 0,
    NEOS_XH,
    NEOS_XL,
    NEOS_YH,
    NEOS_YL,
    NEOS_DONE
} neos_state = NEOS_IDLE;

static void neos_get_new_movement(void)
{
    BYTE new_x, new_y;
    
    new_x = mousedrv_get_x();
    new_y = mousedrv_get_y();
    neos_x = new_x - neos_lastx;
    if (new_x < neos_lastx)
    {
        if (neos_lastx > 0x6f && new_x < 0x10)
        {
            neos_x += 0x80;
        }
    }
    else if (new_x > neos_lastx)
    {
        if(neos_lastx < 0x10 && new_x > 0x6f)
        {
            neos_x += 0x80;
        }
    }
    neos_lastx = new_x;

    neos_y = new_y - neos_lasty;
    if (new_y < neos_lasty)
    {
        if (neos_lasty > 0x6f && new_y < 0x10)
        {
            neos_y += 0x80;
        }
    }
    else if (new_y > neos_lasty)
    {
        if (neos_lasty < 0x10 && new_y > 0x6f)
        {
            neos_y += 0x80;
        }
    }
    neos_lasty = new_y;

    neos_x = (neos_x & 0x80) ? (neos_x / 2) | 0x80 : neos_x / 2;
    neos_y = (neos_y & 0x80) ? (neos_y / 2) | 0x80 : neos_y / 2;
    neos_x = -neos_x;
}

void neos_mouse_store(BYTE val) 
{
    switch (neos_state) {
        case NEOS_IDLE:
            if (((val & 16) ^ (neos_prev & 16)) && ((val & 16)==0)) {
                ++neos_state;
                neos_get_new_movement();
            }
            break;
        case NEOS_XH:
            if (((val & 16) ^ (neos_prev & 16)) && ((val & 16)!=0)) {
                ++neos_state;
            }
            break;
        case NEOS_XL:
            if (((val & 16) ^ (neos_prev & 16)) && ((val & 16)==0)) {
                ++neos_state;
            }
            break;
        case NEOS_YH:
            if (((val & 16) ^ (neos_prev & 16)) && ((val & 16)!=0)) {
                ++neos_state;
                alarm_set(neosmouse_alarm, maincpu_clk + NEOS_RESET_CLK);
            }
            break;
        case NEOS_YL:
            ++neos_state;
            break;
        case NEOS_DONE:
        default:
            break;
    }
    neos_prev = val;
}

BYTE neos_mouse_read(void)
{
    switch (neos_state)
    {
        case NEOS_XH:
            return ((neos_x >> 4) & 0xf) | 0xf0;
            break;
        case NEOS_XL:
            return (neos_x & 0xf) | 0xf0;
            break;
        case NEOS_YH:
            return ((neos_y >> 4) & 0xf) | 0xf0;
            break;
        case NEOS_YL:
            return (neos_y & 0xf) | 0xf0;
            break;
        case NEOS_IDLE:
        case NEOS_DONE:
        default:
            return 0xff;
            break;
    }
}


static void neosmouse_alarm_handler(CLOCK offset, void *data)
{
    alarm_unset(neosmouse_alarm);
    neos_state = NEOS_IDLE;
}


/* --------------------------------------------------------- */
/* Amiga mouse support (currently experimental) */

static const BYTE amiga_mouse_table[4] = { 0x0, 0x1, 0x5, 0x4 };

/* the method below results in alot of overflows */
#if 0
BYTE amiga_mouse_read(void)
{
    BYTE new_x, new_y;
    
    new_x = mousedrv_get_x()/2;
    new_y = (-mousedrv_get_y())/2;

    return (amiga_mouse_table[new_x & 3] << 1) | amiga_mouse_table[new_y & 
3] | 0xf0;
}
#endif

/* the alternate method below doesn't keep track of the speed of
   the mouse movements, just the direction.
 */

static BYTE old_x = 0;
static BYTE old_y = 0;
static BYTE x_count = 0;
static BYTE y_count = 0;

BYTE amiga_mouse_read(void)
{
    BYTE new_x, new_y;
    signed char dir_x, dir_y;
    
    /* get the new mouse values */
    new_x = mousedrv_get_x();
    new_y = (-mousedrv_get_y());

    /* find out the x direction */
    if (new_x == old_x)
    {
        /* no direction, 0 */
        dir_x = 0;
    }
    else
    {
        if (new_x > old_x)
        {
            if ((new_x - old_x) < (old_x + 256 - new_x))
            {
                /* right, +1 */
                dir_x = 1;
            }
            else
            {
                /* left underflow, -1 */
                dir_x = -1;
            }
        }
        else
        {
            if ((old_x - new_x) < (new_x + 256 - old_x))
            {
                /* left, -1 */
                dir_x = -1;
            }
            else
            {
                /* right overflow, +1 */
                dir_x = 1;
            }
        }
    }

    /* find out the y direction */
    if (new_y == old_y)
    {
        dir_y = 0;
    }
    else
    {
        if (new_y > old_y)
        {
            if ((new_y - old_y) < (old_y + 256 - new_y))
            {
                dir_y = 1;
            }
            else
            {
                dir_y = -1;
            }
        }
        else
        {
            if ((old_y - new_y) < (new_y + 256 - old_y))
            {
                dir_y = -1;
            }
            else
            {
                dir_y = 1;
            }
        }
    }

    /* store new values as old values */
    old_x = new_x;
    old_y = new_y;

    /* add x direction to x counter */
    x_count += dir_x;

    /* add y direction to y counter */
    y_count += dir_y;

    return (amiga_mouse_table[x_count & 3] << 1) | amiga_mouse_table[y_count & 3] | 0xf0;
}


/* --------------------------------------------------------- */
/* Paddle support */

static BYTE paddle_val[] = { 
/*  x     y  */
    0x00, 0xff, /* no port */
    0x00, 0xff, /* port 1 */
    0x00, 0xff, /* port 2 */
    0x00, 0xff  /* both ports */
};

static BYTE paddle_old[] = { 
    0xff, 0xff, 
    0xff, 0xff, 
    0xff, 0xff, 
    0xff, 0xff
};

static BYTE paddle_port = 1;

static inline BYTE mouse_paddle_update(BYTE paddle_v, BYTE *old_v, BYTE new_v)
{
    BYTE diff = new_v - *old_v;
    BYTE new_paddle;

    if (new_v < *old_v) {
        if (*old_v > 0x6f && new_v < 0x10) {
            diff += 0x80;
        }
    } else if (new_v > *old_v) {
        if(*old_v < 0x10 && new_v > 0x6f) {
            diff += 0x80;
        }
    }

    new_paddle = paddle_v + diff;

    *old_v = new_v;

    if (((paddle_v & 0x80) ^ (new_paddle & 0x80)) && ((new_paddle & 0x80) == (diff & 0x80))) {
        new_paddle = (paddle_v & 0x80)?0xff:0;
    }

    return new_paddle;
}

static BYTE mouse_get_paddle_x(void)
{
    int i = (paddle_port << 1);

    if (paddle_port == mouse_port) {
        paddle_val[i] = mouse_paddle_update(paddle_val[i], &(paddle_old[i]), mousedrv_get_x());
    }

    return 0xff-paddle_val[i];
}

static BYTE mouse_get_paddle_y(void)
{
    int i = (paddle_port << 1) + 1;

    if (paddle_port == mouse_port) {
        paddle_val[i] = mouse_paddle_update(paddle_val[i], &(paddle_old[i]), mousedrv_get_y());
    }

    return paddle_val[i];
}

void mouse_set_paddle_port(int port)
{
    paddle_port = port & 3;
}

/* --------------------------------------------------------- */
/* Resources & cmdline */

static int set_mouse_enabled(int val, void *param)
{
    _mouse_enabled = val;
    mousedrv_mouse_changed();
    return 0;
}

static int set_mouse_port(int val, void *param)
{
    if (val < 1 || val > 2)
        return -1;

    mouse_port = val;

    return 0;
}

static int set_mouse_type(int val, void *param)
{
    if (!((val >= 0) && (val <= 3))) {
        return -1;
    }

    mouse_type = val;

    return 0;
}

static const resource_int_t resources_int[] = {
    { "Mouse", 0, RES_EVENT_SAME, NULL,
      &_mouse_enabled, set_mouse_enabled, NULL },
    { NULL }
};

static const resource_int_t resources_extra_int[] = {
    { "Mousetype", MOUSE_TYPE_1351, RES_EVENT_SAME, NULL,
      &mouse_type, set_mouse_type, NULL },
    { "Mouseport", 1, RES_EVENT_SAME, NULL,
      &mouse_port, set_mouse_port, NULL },
    { NULL }
};

int mouse_resources_init(void)
{
    if (resources_register_int(resources_int) < 0)
        return -1;

    /* FIXME ugly kludge to remove port and type setting from xvic */
    if (machine_class != VICE_MACHINE_VIC20) {
        if (resources_register_int(resources_extra_int) < 0) {
            return -1;
        }
    }

    return mousedrv_resources_init();
}

static const cmdline_option_t cmdline_options[] = {
    { "-mouse", SET_RESOURCE, 0,
      NULL, NULL, "Mouse", (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_MOUSE_GRAB,
      NULL, NULL },
    { "+mouse", SET_RESOURCE, 0,
      NULL, NULL, "Mouse", (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_MOUSE_GRAB,
      NULL, NULL },
    { NULL }
};

static const cmdline_option_t cmdline_extra_option[] = {
    { "-mousetype", SET_RESOURCE, 1,
      NULL, NULL, "Mousetype", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SELECT_MOUSE_TYPE,
      NULL, NULL },
    { "-mouseport", SET_RESOURCE, 1,
      NULL, NULL, "Mouseport", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_VALUE, IDCLS_SELECT_MOUSE_JOY_PORT,
      NULL, NULL },
    { NULL }
};

int mouse_cmdline_options_init(void)
{
    if (cmdline_register_options(cmdline_options) < 0)
        return -1;

    /* FIXME ugly kludge to remove port and type setting from xvic */
    if (machine_class != VICE_MACHINE_VIC20) {
        if (cmdline_register_options(cmdline_extra_option) < 0) {
            return -1;
        }
    }

    return mousedrv_cmdline_options_init();
}

void mouse_init(void)
{
    /* FIXME ugly kludge to set the correct port and type setting for xvic */
    if (machine_class == VICE_MACHINE_VIC20) {
        set_mouse_port(1, NULL);
        set_mouse_type(MOUSE_TYPE_PADDLE, NULL);
    }

    neos_and_amiga_buttons = 0;
    neos_prev = 0xff;
    neosmouse_alarm = alarm_new(maincpu_alarm_context, "NEOSMOUSEAlarm", neosmouse_alarm_handler, NULL);
    mousedrv_init();
}

/* --------------------------------------------------------- */
/* Main API */

void mouse_button_left(int pressed)
{
    BYTE joypin = ((mouse_type == MOUSE_TYPE_PADDLE) ? 4 : 16);

    if (pressed) {
        joystick_set_value_or(mouse_port, joypin);
    } else {
        joystick_set_value_and(mouse_port, ~joypin);
    }
}

void mouse_button_right(int pressed)
{
    switch (mouse_type) {
        case MOUSE_TYPE_1351:
            if (pressed) {
                joystick_set_value_or(mouse_port, 1);
            } else {
                joystick_set_value_and(mouse_port, ~1);
            }
            break;
        case MOUSE_TYPE_PADDLE:
            if (pressed) {
                joystick_set_value_or(mouse_port, 8);
            } else {
                joystick_set_value_and(mouse_port, ~8);
            }
            break;
        case MOUSE_TYPE_NEOS:
        case MOUSE_TYPE_AMIGA:
            if (pressed) {
                neos_and_amiga_buttons |= 1;
            } else {
                neos_and_amiga_buttons &= ~1;
            }
            break;
        default:
            break;
    }
}

BYTE mouse_get_x(void)
{
    switch (mouse_type) {
        case MOUSE_TYPE_1351:
            return mousedrv_get_x();
            break;
        case MOUSE_TYPE_PADDLE:
            return mouse_get_paddle_x();
            break;
        case MOUSE_TYPE_NEOS:
        case MOUSE_TYPE_AMIGA:
            return (neos_and_amiga_buttons & 1) ? 0xff : 0;
            break;
        default:
            break;
    }
    return 0xff;
}

BYTE mouse_get_y(void)
{
    switch (mouse_type) {
        case MOUSE_TYPE_1351:
            return mousedrv_get_y();
            break;
        case MOUSE_TYPE_PADDLE:
            return mouse_get_paddle_y();
            break;
        case MOUSE_TYPE_NEOS:
        case MOUSE_TYPE_AMIGA:
        default:
            break;
    }
    return 0xff;
}
