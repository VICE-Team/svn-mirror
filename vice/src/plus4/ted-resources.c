/*
 * ted-resources.c - Resources for the TED emulation.
 *
 * Written by
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
#include "fullscreen.h"
#include "raster-resources.h"
#include "resources.h"
#include "ted-resources.h"
#include "tedtypes.h"
#include "utils.h"
#include "video.h"


int ted_resources_init(void)
{
    video_chip_cap_t *video_chip_cap;

    video_chip_cap = (video_chip_cap_t *)xmalloc(sizeof(video_chip_cap_t));

    video_chip_cap->dsize_allowed = ARCHDEP_TED_DSIZE;
    video_chip_cap->dscan_allowed = ARCHDEP_TED_DSCAN;
    video_chip_cap->single_mode.sizex = 1;
    video_chip_cap->single_mode.sizey = 1;
    video_chip_cap->single_mode.rmode = VIDEO_RENDER_PAL_1X1;
    video_chip_cap->double_mode.sizex = 2;
    video_chip_cap->double_mode.sizey = 2;
    video_chip_cap->double_mode.rmode = VIDEO_RENDER_PAL_2X2;

    fullscreen_capability(&video_chip_cap->fullscreen);

    if (raster_resources_chip_init("TED", &ted.raster, video_chip_cap) < 0)
        return -1;

    return 0;
}

