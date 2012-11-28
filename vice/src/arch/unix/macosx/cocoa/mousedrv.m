/*
 * mousedrv.m - MacVICE mouse driver
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
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

#include "cmdline.h"
#include "mousedrv.h"
#include "resources.h"
#include "translate.h"
#include "vicemachine.h"
#include "vsyncapi.h"

// mouse.c
extern int _mouse_enabled;

static float pointerX;
static float pointerY;
static unsigned long mouse_timestamp = 0;

static int  scaleX;
static int  scaleY;

static int set_scale_x(int val, void *param)
{
    if((val > 0)&&(val <= 64)) {
        scaleX = val;
    }
    return 0;
}

static int set_scale_y(int val, void *param)
{
    if((val > 0)&&(val <= 64)) {
        scaleY = val;
    }
    return 0;
}

static resource_int_t resources_int[] =
{
    { "MouseScaleX", 2, RES_EVENT_NO, NULL,
       &scaleX, set_scale_x, NULL },
    { "MouseScaleY", 2, RES_EVENT_NO, NULL,
       &scaleY, set_scale_y, NULL },
    { NULL }
 };

int mousedrv_resources_init(void)
{
    return resources_register_int(resources_int);
}

static const cmdline_option_t cmdline_options[] = {
    { "-mousescalex", SET_RESOURCE, 1,
      NULL, NULL, "MouseScaleX", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<1-8>", N_("Set scaling factor for mouse movement along X") },
    { "-mousescaley", SET_RESOURCE, 1,
      NULL, NULL, "MouseScaleY", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<1-8>", N_("Set scaling factor for mouse movement along Y") },
    { NULL }
};

int mousedrv_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

void mousedrv_init(void)
{
}

void mousedrv_mouse_changed(void)
{
    [[theVICEMachine machineNotifier] postToggleMouseNotification:_mouse_enabled];
}

// the HW polls the position
int mousedrv_get_x(void)
{
    return (int)(x * scaleX) & 0xffff;
}

int mousedrv_get_y(void)
{
    return (int)(y * scaleY) & 0xffff;
}

// FIXME: I think this should use relative movements,
// and possibly reposition the pointer when too close
// to the edge.
void mouse_move_f(float x, float y)
{
    pointerX = x;
    pointerY = y;
    mouse_timestamp = vsyncarch_gettime();
}

unsigned long mousedrv_get_timestamp(void)
{
    return mouse_timestamp;
}
