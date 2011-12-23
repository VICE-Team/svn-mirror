/*
 * mousedrv.c - Mouse handling for Unix-Systems.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/* This is a first rough implementation of mouse emulation for MS-DOS.
   A smarter and less buggy emulation is of course possible. */

/* #define DEBUG_MOUSE */

#ifdef DEBUG_MOUSE
#define DBG(x)  log_debug x
#else
#define DBG(x)
#endif

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "mouse.h"
#include "mousedrv.h"
#include "log.h"
#include "ui.h"
#include "vsyncapi.h"

#ifndef MACOSX_COCOA

int mouse_accelx = 2, mouse_accely = 2;

/* last mouse position set by gui frontend */
int mouse_x = 0, mouse_y = 0;
static unsigned long mouse_timestamp = 0;


void mousedrv_mouse_changed(void)
{
    ui_check_mouse_cursor();
}

int mousedrv_resources_init(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

int mousedrv_cmdline_options_init(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

void mousedrv_init(void)
{
}

/* ------------------------------------------------------------------------- */

void mouse_button(int bnumber, int state)
{
    if (bnumber == 0) {
        mouse_button_left(state);
    }
    if (bnumber == 2) {
        mouse_button_right(state);
    }
}

/* ------------------------------------------------------------------------- */

/*
    to avoid strange side effects two things are done here:

    - max delta is limited to MOUSE_MAX_DIFF
    - if the delta is limited, then the current position is linearly 
      interpolated towards the real position using MOUSE_MAX_DIFF for the axis
      with the largest delta
*/
#define MOUSE_MAX_DIFF  16

static int last_mouse_x = 0;
static int last_mouse_y = 0;

static void domove(void)
{
    float dx, dy, ax, ay;
    float f;

    dx = mouse_x - last_mouse_x;
    dy = mouse_y - last_mouse_y;
    ax = fabs(dx); ay = fabs(dy);

    if ((ax > MOUSE_MAX_DIFF) || (ay > MOUSE_MAX_DIFF)) {
        if (ay > ax) {
            /* do big step in Y */
            f = ay / MOUSE_MAX_DIFF;
        } else {
            /* do big step in X */
            f = ax / MOUSE_MAX_DIFF;
        }
        last_mouse_x += (dx / f);
        last_mouse_y += (dy / f);
        DBG(("mousex %8d y %8d lastx %8d y %8d dx %f dy %f f:%f dxf:%f dxy:%f",
            mouse_x, mouse_y, last_mouse_x, last_mouse_y, dx, dy, f, (dx / f), (dy / f)));
    } else {
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
    }
}

BYTE mousedrv_get_x(void)
{
    domove();
    return (BYTE)((last_mouse_x * mouse_accelx) >> 1) & 0x7e;
}

BYTE mousedrv_get_y(void)
{
    domove();
    return (BYTE)((last_mouse_y * mouse_accely) >> 1) & 0x7e;
}

/* ------------------------------------------------------------------------- */

void mouse_move(int x, int y)
{
    if (!_mouse_enabled) {
        return;
    }
    mouse_timestamp = vsyncarch_gettime();

    mouse_x = x;
    /* mouse_y = 256 - y; */
    mouse_y = ~(y - 1);
}

unsigned long mousedrv_get_timestamp(void)
{
    return mouse_timestamp;
}

#endif
