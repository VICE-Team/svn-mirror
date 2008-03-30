/*
 * vicii-resources.c - Resources for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "vice.h"

#include <stdio.h>

#include "archdep.h"
#include "raster-resources.h"
#include "resources.h"
#include "utils.h"
#include "vicii-color.h"
#include "vicii-resources.h"
#include "vicii.h"
#include "viciitypes.h"
#include "video.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif


vic_ii_resources_t vic_ii_resources;


static int set_new_luminances(resource_value_t v, void *param)
{
    vic_ii_resources.new_luminances = (int)v;
    return vic_ii_update_palette();
}

static int set_sprite_sprite_collisions_enabled(resource_value_t v, void *param)
{
    vic_ii_resources.sprite_sprite_collisions_enabled = (int)v;
    return 0;
}

static int set_sprite_background_collisions_enabled(resource_value_t v,
                                                    void *param)
{
    vic_ii_resources.sprite_background_collisions_enabled = (int)v;
    return 0;
}

static resource_t resources[] =
{
    { "VICIICheckSsColl", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vic_ii_resources.sprite_sprite_collisions_enabled,
      set_sprite_sprite_collisions_enabled, NULL },
    { "VICIICheckSbColl", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vic_ii_resources.sprite_background_collisions_enabled,
      set_sprite_background_collisions_enabled, NULL },
    { "VICIINewLuminances", RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&vic_ii_resources.new_luminances,
      set_new_luminances, NULL },
    { NULL }
};


#if ARCHDEP_VICII_DSIZE == 1
#ifdef USE_XF86_EXTENSIONS
static int set_fullscreen_double_size_enabled(resource_value_t v, void *param)
{
    vic_ii_resources.fullscreen_double_size_enabled = (int)v;

    /* Does not exist anymore 
    if (fullscreen_is_enabled)
        vic_ii_resize(); */
    return 0;
}
#endif
#endif

#if ARCHDEP_VICII_DSCAN == 1
#ifdef USE_XF86_EXTENSIONS
static int set_fullscreen_double_scan_enabled(resource_value_t v, void *param)
{
    vic_ii_resources.fullscreen_double_scan_enabled = (int)v;
    if (fullscreen_is_enabled)
        raster_force_repaint(&vic_ii.raster);
    return 0;
}
#endif
#endif

static resource_t resources_2x[] =
{
#if ARCHDEP_VICII_DSIZE == 1
#ifdef USE_XF86_EXTENSIONS
    { "FullscreenDoubleSize", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_ii_resources.fullscreen_double_size_enabled,
      set_fullscreen_double_size_enabled, NULL },
#endif
#endif
#if ARCHDEP_VICII_DSCAN == 1
#ifdef USE_XF86_EXTENSIONS
    { "FullscreenDoubleScan", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&vic_ii_resources.fullscreen_double_scan_enabled,
      set_fullscreen_double_scan_enabled, NULL },
#endif
#endif
    { NULL }
};


int vic_ii_resources_init(void)
{
    video_chip_cap_t *video_chip_cap;

    video_chip_cap = (video_chip_cap_t *)xmalloc(sizeof(video_chip_cap_t));

    video_chip_cap->dsize_allowed = ARCHDEP_VICII_DSIZE;
    video_chip_cap->dscan_allowed = ARCHDEP_VICII_DSCAN;
    video_chip_cap->single_mode.sizex = 1;
    video_chip_cap->single_mode.sizey = 1;
    video_chip_cap->single_mode.rmode = VIDEO_RENDER_PAL_1X1;
    video_chip_cap->double_mode.sizex = 2;
    video_chip_cap->double_mode.sizey = 2;
    video_chip_cap->double_mode.rmode = VIDEO_RENDER_PAL_2X2;

#if (ARCHDEP_VICII_DSIZE == 1) || (ARCHDEP_VICII_DSCAN == 1)
    if (resources_register(resources_2x) < 0)
        return -1;
#endif

    if (raster_resources_chip_init("VICII", &vic_ii.raster,
        video_chip_cap) < 0)
        return -1;

    return resources_register(resources);
}

