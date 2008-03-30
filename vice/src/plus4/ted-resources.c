/*
 * ted-resources.c - Resources for the TED emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "resources.h"
#include "ted-resources.h"
#include "ted.h"
#include "tedtypes.h"
#include "utils.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif


#ifdef __MSDOS__
#define DEFAULT_VideoCache_VALUE 0
#else
#define DEFAULT_VideoCache_VALUE 1
#endif


ted_resources_t ted_resources;

static int set_video_cache_enabled(resource_value_t v, void *param)
{
    ted_resources.video_cache_enabled = (int)v;
    if (ted.initialized)
        raster_enable_cache(&ted.raster,
                            ted_resources.video_cache_enabled);

    return 0;
}

static resource_t resources[] =
{
    { "VideoCache", RES_INTEGER, (resource_value_t)DEFAULT_VideoCache_VALUE,
      (resource_value_t *)&ted_resources.video_cache_enabled,
      set_video_cache_enabled, NULL },
    { NULL }
};


#ifdef VIC_II_NEED_2X

static int set_double_size_enabled(resource_value_t v, void *param)
{
    ted_resources.double_size_enabled = (int)v;

#ifdef USE_XF86_EXTENSIONS
    if (!fullscreen_is_enabled)
#endif
        ted_resize();

    return 0;
}

static int set_double_scan_enabled (resource_value_t v, void *param)
{
    ted_resources.double_scan_enabled = (int)v;
#ifdef USE_XF86_EXTENSIONS
    if (ted.initialized && !fullscreen_is_enabled)
#else
    if (ted.initialized)
#endif
        raster_enable_double_scan(&ted.raster,
                                  ted_resources.double_scan_enabled);

    return 0;
}

#ifdef USE_XF86_EXTENSIONS
static int set_fullscreen_double_size_enabled(resource_value_t v, void *param)
{
    ted_resources.fullscreen_double_size_enabled = (int)v;
    if (fullscreen_is_enabled)
        ted_resize();
    return 0;
}

static int
set_fullscreen_double_scan_enabled(resource_value_t v, void *param)
{
    ted_resources.fullscreen_double_scan_enabled = (int)v;
    if (fullscreen_is_enabled && ted.initialized)
        raster_enable_double_scan(&ted.raster,
                                  ted_resources.fullscreen_double_scan_enabled);
    return 0;
}
#endif

static resource_t resources_2x[] =
{
    { "DoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&ted_resources.double_size_enabled,
      set_double_size_enabled, NULL },
    { "DoubleScan", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&ted_resources.double_scan_enabled,
      set_double_scan_enabled, NULL },
#ifdef USE_XF86_EXTENSIONS
    { "FullscreenDoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&ted_resources.fullscreen_double_size_enabled,
      set_fullscreen_double_size_enabled, NULL },
    { "FullscreenDoubleScan", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&ted_resources.fullscreen_double_scan_enabled,
      set_fullscreen_double_scan_enabled, NULL },
#endif
    { NULL }
};

#endif /* VIC_II_NEED_2X */


int ted_resources_init(void)
{
#ifdef VIC_II_NEED_2X
    if (resources_register(resources_2x) < 0)
        return -1;
#endif

    return resources_register(resources);
}

