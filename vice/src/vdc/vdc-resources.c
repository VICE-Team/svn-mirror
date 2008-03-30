/*
 * vdc-resources.c - Resources for the MOS 8563 (VDC) emulation.
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
#include "vdc-resources.h"
#include "vdctypes.h"
#include "video.h"


vdc_resources_t vdc_resources;
static video_chip_cap_t video_chip_cap;


static int set_64kb_expansion(int val, void *param)
{
    vdc_resources.vdc_64kb_expansion = val;

    vdc.vdc_address_mask = vdc_resources.vdc_64kb_expansion
                           ? 0xffff : 0x3fff;
    return 0;
}

static int set_vdc_revision(int val, void *param)
{
    unsigned int revision;

    revision = (unsigned int)val;

    if (revision > 2)
        return -1;

    vdc.revision = revision;

    return 0;
}

static const resource_int_t resources_int[] =
{
    { "VDC64KB", 1, RES_EVENT_SAME, NULL,
      &vdc_resources.vdc_64kb_expansion, set_64kb_expansion, NULL },
    { "VDCRevision", 2, RES_EVENT_SAME, NULL,
      (int *)&vdc.revision, set_vdc_revision, NULL },
    { NULL }
};

int vdc_resources_init(void)
{
    video_chip_cap.dsize_allowed = ARCHDEP_VDC_DSIZE;
    video_chip_cap.dsize_default = ARCHDEP_VDC_DSIZE;
    video_chip_cap.dsize_limit_width = 0;
    video_chip_cap.dsize_limit_height = 0;
    video_chip_cap.dscan_allowed = ARCHDEP_VDC_DSCAN;
    video_chip_cap.hwscale_allowed = ARCHDEP_VDC_HWSCALE;
    video_chip_cap.scale2x_allowed = 0;
    video_chip_cap.internal_palette_allowed = 0;
    video_chip_cap.external_palette_name = "vdc_deft";
    video_chip_cap.palemulation_allowed = 0;
    video_chip_cap.single_mode.sizex = 1;
    video_chip_cap.single_mode.sizey = 1;
    video_chip_cap.single_mode.rmode = VIDEO_RENDER_RGB_1X1;
    video_chip_cap.double_mode.sizex = 1;
    video_chip_cap.double_mode.sizey = 2;
    video_chip_cap.double_mode.rmode = VIDEO_RENDER_RGB_1X2;

    fullscreen_capability(&(video_chip_cap.fullscreen));

    vdc.video_chip_cap = &video_chip_cap;

    if (raster_resources_chip_init("VDC", &vdc.raster, &video_chip_cap) < 0)
        return -1;

    return resources_register_int(resources_int);
}

