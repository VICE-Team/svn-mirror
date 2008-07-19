/*
 * video-resources-pal.c - PAL resources for the video layer
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

#include "resources.h"
#include "ui.h"
#include "video-resources.h"
#include "video.h"

extern struct video_canvas_s *video_current_canvas;

static int set_color_saturation(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.color_saturation = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_color_contrast(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.color_contrast = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_color_brightness(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.color_brightness = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_color_gamma(int val, void *param)
{
    if (val < 0)
        val=0;
    if (val > 2000)
        val=2000;
    video_resources.color_gamma = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_color_tint(int val, void *param)
{
    if (val < 0)
        val=0;
    if (val > 2000)
        val=2000;
    video_resources.color_tint = val;
    return video_color_update_palette(video_current_canvas);
}

#ifndef USE_GNOMEUI
/* remove this once all ports have implemented this ui function */
#define ui_update_pal_ctrls(a)
#endif

static int set_delayloop_emulation(int val, void *param)
{
    int old = video_resources.delayloop_emulation;
    video_resources.delayloop_emulation = val;

    if (video_color_update_palette(video_current_canvas) < 0) {
        video_resources.delayloop_emulation = old;
        ui_update_pal_ctrls(video_resources.delayloop_emulation);
        return -1;
    }
    ui_update_pal_ctrls(video_resources.delayloop_emulation);

#ifdef WIN32
    video_resources_check_win32_newpal();
#endif

    return 0;
}

static int set_pal_scanlineshade(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 1000)
        val = 1000;
    video_resources.pal_scanlineshade = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_pal_oddlinesphase(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.pal_oddlines_phase = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_pal_oddlinesoffset(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 2000)
        val = 2000;
    video_resources.pal_oddlines_offset = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_pal_blur(int val, void *param)
{
    if (val < 0)
        val = 0;
    if (val > 1000)
        val = 1000;
    video_resources.pal_blur = val;
    return video_color_update_palette(video_current_canvas);
}

static int set_pal_mode(int val, void *param)
{
    video_resources.pal_mode = val;

#ifdef WIN32
    if (val == 2)
        video_resources_check_win32_newpal();
#endif

    return 0;
}

static const resource_int_t resources_int[] =
{
    { "ColorSaturation", 1000, RES_EVENT_NO, NULL,
      &video_resources.color_saturation, set_color_saturation, NULL },
    { "ColorContrast", 1000, RES_EVENT_NO, NULL,
      &video_resources.color_contrast, set_color_contrast, NULL },
    { "ColorBrightness", 1000, RES_EVENT_NO, NULL,
      &video_resources.color_brightness, set_color_brightness, NULL },
    { "ColorGamma", 880, RES_EVENT_NO, NULL,
      &video_resources.color_gamma, set_color_gamma, NULL },
    { "ColorTint", 1000, RES_EVENT_NO, NULL,
      &video_resources.color_tint, set_color_tint, NULL },
    { "PALEmulation", 0, RES_EVENT_NO, NULL,
      &video_resources.delayloop_emulation, set_delayloop_emulation, NULL },
    { "PALScanLineShade", 667, RES_EVENT_NO, NULL,
      &video_resources.pal_scanlineshade, set_pal_scanlineshade, NULL },
    { "PALBlur", 500, RES_EVENT_NO, NULL,
      &video_resources.pal_blur, set_pal_blur, NULL },
    { "PALMode", VIDEO_RESOURCE_PAL_MODE_FAST, RES_EVENT_NO, NULL,
      &video_resources.pal_mode, set_pal_mode, NULL },
    { "PALOddLinePhase", 1250, RES_EVENT_NO, NULL,
      &video_resources.pal_oddlines_phase, set_pal_oddlinesphase, NULL },
    { "PALOddLineOffset", 750, RES_EVENT_NO, NULL,
      &video_resources.pal_oddlines_offset, set_pal_oddlinesoffset, NULL },
    { NULL }
};

int video_resources_pal_init(void)
{
    return resources_register_int(resources_int);
}

