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
#include "raster-resources.h"
#include "video.h"


static video_chip_cap_t video_chip_cap;


int crtc_resources_init(void)
{
    video_chip_cap.dsize_allowed = ARCHDEP_CRTC_DSIZE;
    video_chip_cap.dsize_default = 0;
    video_chip_cap.dsize_limit_width = 400;
    video_chip_cap.dsize_limit_height = 350;
    video_chip_cap.dscan_allowed = ARCHDEP_CRTC_DSCAN;
    video_chip_cap.hwscale_allowed = ARCHDEP_VDC_HWSCALE;
    video_chip_cap.scale2x_allowed = ARCHDEP_CRTC_DSIZE;
    video_chip_cap.internal_palette_allowed = 0;
    video_chip_cap.external_palette_name = "green";
    video_chip_cap.palemulation_allowed = 0;
    video_chip_cap.single_mode.sizex = 1;
    video_chip_cap.single_mode.sizey = 1;
    video_chip_cap.single_mode.rmode = VIDEO_RENDER_RGB_1X1;
    video_chip_cap.double_mode.sizex = 2;
    video_chip_cap.double_mode.sizey = 2;
    video_chip_cap.double_mode.rmode = VIDEO_RENDER_RGB_2X2;

    fullscreen_capability(&(video_chip_cap.fullscreen));

    if (raster_resources_chip_init("Crtc", &crtc.raster, &video_chip_cap) < 0)
        return -1;

    crtc.video_chip_cap = &video_chip_cap;

    return 0;
}

