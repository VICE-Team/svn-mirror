/*
 * vic-resources.c - Resource handling for the VIC-I emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "archdep.h"
#include "raster-resources.h"
#include "resources.h"
#include "utils.h"
#include "vic-resources.h"
#include "vic.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif


vic_resources_t vic_resources;


#if ARCHDEP_VIC_DSIZE == 1
static int set_double_size_enabled(resource_value_t v, void *param)
{
    vic_resources.double_size_enabled = (int)v;

#ifdef USE_XF86_EXTENSIONS
    if (!fullscreen_is_enabled)
#endif 
        vic_resize();

    return 0;
}

#ifdef USE_XF86_EXTENSIONS
static int set_fullscreen_double_size_enabled(resource_value_t v, void *param)
{
    vic_resources.fullscreen_double_size_enabled = (int)v;
    if (fullscreen_is_enabled)
        vic_resize();
    return 0;
}
#endif
#endif

#if ARCHDEP_VIC_DSCAN == 1
#ifdef USE_XF86_EXTENSIONS
static int set_fullscreen_double_scan_enabled(resource_value_t v, void *param)
{
    vic_resources.fullscreen_double_scan_enabled = (int)v;
    if (fullscreen_is_enabled)
        raster_force_repaint(&vic.raster);
    return 0;
}
#endif
#endif

static resource_t resources_2x[] =
{
#if ARCHDEP_VIC_DSIZE == 1
    { "DoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_resources.double_size_enabled,
      set_double_size_enabled, NULL },
#ifdef USE_XF86_EXTENSIONS
    { "FullscreenDoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_resources.fullscreen_double_size_enabled,
      set_fullscreen_double_size_enabled, NULL },
#endif
#endif
#if ARCHDEP_VIC_DSCAN == 1
#ifdef USE_XF86_EXTENSIONS
    { "FullscreenDoubleScan", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_resources.fullscreen_double_scan_enabled,
      set_fullscreen_double_scan_enabled, NULL },
#endif
#endif
    { NULL }
};

int vic_resources_init(void)
{
#if (ARCHDEP_VIC_DSIZE == 1) || (ARCHDEP_VIC_DSCAN == 1)
    if (resources_register(resources_2x) < 0)
        return -1;
#endif
    if (raster_resources_chip_init("VIC", &vic.raster,
        ARCHDEP_VIC_DSIZE, ARCHDEP_VIC_DSCAN) < 0)
        return -1;

    return 0;
}

