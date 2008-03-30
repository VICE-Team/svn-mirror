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

#include "raster-resources.h"
#include "resources.h"
#include "utils.h"
#include "vic-resources.h"
#include "vic.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif


vic_resources_t vic_resources;


#ifdef VIC_NEED_2X

static int set_double_size_enabled(resource_value_t v, void *param)
{
    vic_resources.double_size_enabled = (int)v;

#ifdef USE_XF86_EXTENSIONS
    if (!fullscreen_is_enabled)
#endif 
        vic_resize();

    return 0;
}

static int set_double_scan_enabled(resource_value_t v, void *param)
{
    vic_resources.double_scan_enabled = (int)v;
#ifdef USE_XF86_EXTENSIONS
    if (vic.initialized && ! fullscreen_is_enabled)
#else 
    if (vic.initialized)
#endif
        raster_enable_double_scan(&vic.raster,
                                  vic_resources.double_scan_enabled);
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

static int set_fullscreen_double_scan_enabled(resource_value_t v, void *param)
{
    vic_resources.fullscreen_double_scan_enabled = (int)v;
    if (vic.initialized && fullscreen_is_enabled)
        raster_enable_double_scan(&vic.raster,
                                  vic_resources.fullscreen_double_scan_enabled);
    return 0;
}
#endif

static resource_t resources_2x[] =
{
    { "DoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_resources.double_size_enabled,
      set_double_size_enabled, NULL },
    { "DoubleScan", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vic_resources.double_scan_enabled,
      set_double_scan_enabled, NULL },
#ifdef USE_XF86_EXTENSIONS
    { "FullscreenDoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_resources.fullscreen_double_size_enabled,
      set_fullscreen_double_size_enabled, NULL },
    { "FullscreenDoubleScan", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_resources.fullscreen_double_scan_enabled,
      set_fullscreen_double_scan_enabled, NULL },
#endif
    { NULL }
};

#endif /* VIC_NEED_2X */

int vic_resources_init(void)
{
#ifdef VIC_NEED_2X
    if (resources_register(resources_2x) < 0)
        return -1;
#endif
    if (raster_resources_chip_init("VIC", &vic.raster) < 0)
        return -1;

    return 0;
}

