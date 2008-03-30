/*
 * video-resources.c - Resources for the video layer
 *
 * Written by
 *  John Selck <graham@cruise.de>
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

#include "raster.h" /* Temporary */
#include "resources.h"
#include "utils.h"
#include "video-resources.h"
#include "video-color.h"
#include "video.h"
#include "videoarch.h"
#include "ui.h"
#include "utils.h"


video_resources_t video_resources =
{
    1000, 1100, 1100, 880,
    0, 0,
    0, 0,
};

static int set_color_saturation(resource_value_t v, void *param)
{
    int val;
    val = (int)v;
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.color_saturation = val;
    return video_color_update_palette();
}

static int set_color_contrast(resource_value_t v, void *param)
{
    int val;
    val = (int)v;
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.color_contrast = val;
    return video_color_update_palette();
}

static int set_color_brightness(resource_value_t v, void *param)
{
    int val;
    val = (int)v;
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.color_brightness = val;
    return video_color_update_palette();
}

static int set_color_gamma(resource_value_t v, void *param)
{
    int val;
    val = (int)v;
    if (val < 0)
        val=0;
    if (val > 2000)
        val=2000;
    video_resources.color_gamma = val;
    return video_color_update_palette();
}

static int set_ext_palette(resource_value_t v, void *param)
{
    video_resources.ext_palette = (int)v;
    return video_color_update_palette();
}

static int set_palette_file_name(resource_value_t v, void *param)
{
    util_string_set(&video_resources.palette_file_name, (char *)v);
    return video_color_update_palette();
}

#ifndef USE_GNOMEUI
/* remove this once all ports have implemented this ui function */
#define ui_update_pal_ctrls(a)
#endif

static int set_delayloop_emulation(resource_value_t v, void *param)
{
    int old = video_resources.delayloop_emulation;
    video_resources.delayloop_emulation = (int)v;

    if (video_color_update_palette() < 0) {
        video_resources.delayloop_emulation = old;
	ui_update_pal_ctrls(video_resources.delayloop_emulation);
        return -1;
    }
    ui_update_pal_ctrls(video_resources.delayloop_emulation);

    return 0;
}

static int set_pal_scanlineshade(resource_value_t v, void *param)
{
    int val;
    val = (int)v;
    if (val < 0)
        val = 0;
    if (val > 1000)
        val = 1000;
    video_resources.pal_scanlineshade = val;
    return video_color_update_palette();
}

static int set_pal_mode(resource_value_t v, void *param)
{
    video_resources.pal_mode = (int)v;
	return 0;
}

static resource_t resources[] =
{
    { "ExternalPalette", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&video_resources.ext_palette,
      set_ext_palette, NULL },
    { "PaletteFile", RES_STRING, (resource_value_t)"default",
      (resource_value_t *)&video_resources.palette_file_name,
      set_palette_file_name, NULL },
    { NULL }
};

static resource_t resources_pal[] =
{
    { "ColorSaturation", RES_INTEGER, (resource_value_t)1000,
      (resource_value_t *)&video_resources.color_saturation,
      set_color_saturation, NULL },
    { "ColorContrast", RES_INTEGER, (resource_value_t)1000,
      (resource_value_t *)&video_resources.color_contrast,
      set_color_contrast, NULL },
    { "ColorBrightness", RES_INTEGER, (resource_value_t)1000,
      (resource_value_t *)&video_resources.color_brightness,
      set_color_brightness, NULL },
    { "ColorGamma", RES_INTEGER, (resource_value_t)880,
      (resource_value_t *)&video_resources.color_gamma,
      set_color_gamma, NULL },
    { "PALEmulation", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&video_resources.delayloop_emulation,
      set_delayloop_emulation, NULL },
    { "PALScanLineShade", RES_INTEGER, (resource_value_t)667,
      (resource_value_t *)&video_resources.pal_scanlineshade,
      set_pal_scanlineshade, NULL },
    { "PALMode", RES_INTEGER, (resource_value_t)VIDEO_RESOURCE_PAL_MODE_BLUR,
      (resource_value_t *)&video_resources.pal_mode,
      set_pal_mode, NULL },
    { NULL }
};

int video_resources_init(int mode)
{
    int result = 0;

    video_resources.palette_file_name = NULL;

    switch (mode) {
      case VIDEO_RESOURCES_MONOCHROME:
        result = resources_register(resources);
        break;
      case VIDEO_RESOURCES_PAL:
      case VIDEO_RESOURCES_PAL_NOFAKE:
       result = resources_register(resources)
                | resources_register(resources_pal);
       break;
    }

    return result | video_arch_init_resources();
}

/*-----------------------------------------------------------------------*/
/* Per chip resources.  */

struct video_resource_chip_s {
    struct raster_s *raster;
    int double_scan_enabled;
};
typedef struct video_resource_chip_s video_resource_chip_t;

static video_resource_chip_t video_resource_chip;

int set_double_scan_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;

    video_resource_chip = (video_resource_chip_t *)param;

    video_resource_chip->double_scan_enabled = (int)v;

    if (video_resource_chip->raster->viewport.canvas)
        video_resource_chip->raster->viewport.canvas->videoconfig.doublescan
            = (int)v;

    raster_force_repaint(video_resource_chip->raster);

    return 0;
}

static const char *vname_chip_scan[] = { "DoubleScan", NULL };

static resource_t resources_chip_scan[] =
{
    { NULL, RES_INTEGER, (resource_value_t)1,
      (resource_value_t *)&(video_resource_chip.double_scan_enabled),
      set_double_scan_enabled, NULL },
    { NULL }
};

int video_resources_chip_init(const char *chipname, struct raster_s *raster,
                              video_chip_cap_t *video_chip_cap)
{
    unsigned int i;

    video_resource_chip.raster = raster;

    if (video_chip_cap->dscan_allowed != 0) {
        for (i = 0; vname_chip_scan[i] != NULL; i++) {
            resources_chip_scan[i].name = concat(chipname,
                                                 vname_chip_scan[i], NULL);
            resources_chip_scan[i].param = (void *)&video_resource_chip;
        }
        return resources_register(resources_chip_scan);
    }

    return 0;
}

