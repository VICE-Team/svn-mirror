/*
 * crtc-resources.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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
#include "crtc-resources.h"
#include "crtctypes.h"
#include "fullscreen.h"
#include "lib.h"
#include "raster-resources.h"
#include "resources.h"
#include "utils.h"
#include "video.h"


crtc_resources_t crtc_resources;
static video_chip_cap_t video_chip_cap;


static int set_palette_file_name(resource_value_t v, void *param)
{
    util_string_set(&crtc_resources.palette_file_name, (char *)v);
    if (crtc.initialized)
        return crtc_load_palette(crtc_resources.palette_file_name);

    return 0;
}

static const resource_t resources[] =
{
    { "CrtcPaletteFile", RES_STRING, (resource_value_t)"green",
      (void *)&crtc_resources.palette_file_name, set_palette_file_name, NULL },
    { NULL }
};

int crtc_resources_init(void)
{
    video_chip_cap.dsize_allowed = ARCHDEP_CRTC_DSIZE;
    video_chip_cap.dsize_default = 0;
    video_chip_cap.dsize_limit_width = 400;
    video_chip_cap.dsize_limit_height = 350;
    video_chip_cap.dscan_allowed = ARCHDEP_CRTC_DSCAN;
    video_chip_cap.scale2x_allowed = ARCHDEP_CRTC_DSIZE;
    video_chip_cap.internal_palette_allowed = 0;
    video_chip_cap.single_mode.sizex = 1;
    video_chip_cap.single_mode.sizey = 1;
    video_chip_cap.single_mode.rmode = VIDEO_RENDER_RGB_1X1;
    video_chip_cap.double_mode.sizex = 2;
    video_chip_cap.double_mode.sizey = 2;
    video_chip_cap.double_mode.rmode = VIDEO_RENDER_RGB_2X2;

    fullscreen_capability(&(video_chip_cap.fullscreen));

    if (raster_resources_chip_init("Crtc", &crtc.raster, &video_chip_cap) < 0)
        return -1;

    crtc_resources.palette_file_name = NULL;
    crtc.video_chip_cap = &video_chip_cap;

    return resources_register(resources);
}

