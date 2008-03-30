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

#include "resources.h"
#include "utils.h"
#include "video-resources.h"
#include "video-color.h"
#include "video.h"
#include "videoarch.h"
#include "utils.h"


video_resources_t video_resources =
{
    1000, 1100, 1100, 880,
    0, 0,
    0, 0,
};

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

static const resource_t resources[] =
{
    { "ExternalPalette", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&video_resources.ext_palette,
      set_ext_palette, NULL },
    { "PaletteFile", RES_STRING, (resource_value_t)"default",
      (resource_value_t *)&video_resources.palette_file_name,
      set_palette_file_name, NULL },
    { NULL }
};

int video_resources_init(void)
{
    return resources_register(resources) | video_arch_init_resources();
}

/*-----------------------------------------------------------------------*/
/* Per chip resources.  */

struct video_resource_chip_s {
    struct video_canvas_s **canvas;
    video_chip_cap_t *video_chip_cap;
    int double_scan_enabled;
    int double_size_enabled;
    int fullscreen_enabled;
    char *fullscreen_device;
    int fullscreen_double_size_enabled;
    int fullscreen_double_scan_enabled;
    int fullscreen_mode[FULLSCREEN_MAXDEV];
};
typedef struct video_resource_chip_s video_resource_chip_t;

struct video_resource_chip_mode_s {
    video_resource_chip_t *resource_chip;
    unsigned int device;
};
typedef struct video_resource_chip_mode_s video_resource_chip_mode_t;

static int set_double_size_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    cap_render_t *cap_render;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    if ((int)v)
        cap_render = &video_chip_cap->double_mode;
    else
        cap_render = &video_chip_cap->single_mode;

    canvas->videoconfig->rendermode = cap_render->rmode;

    if (cap_render->sizex > 1
        && (video_chip_cap->dsize_limit_width == 0
        || canvas->draw_buffer->canvas_width
        <= video_chip_cap->dsize_limit_width))
        canvas->videoconfig->doublesizex = 1;
    else
        canvas->videoconfig->doublesizex = 0;

    if (cap_render->sizey > 1
        && (video_chip_cap->dsize_limit_height == 0
        || canvas->draw_buffer->canvas_height
        <= video_chip_cap->dsize_limit_height))
        canvas->videoconfig->doublesizey = 1;
    else
        canvas->videoconfig->doublesizey = 0;

    /* FIXME: Kludge needed until kind of render and dimensions are
       separated from `rendermode' (which is overloaded currently). */
    if (canvas->videoconfig->rendermode == VIDEO_RENDER_RGB_2X2) {
        if (canvas->videoconfig->doublesizex == 0)
            canvas->videoconfig->rendermode = VIDEO_RENDER_RGB_1X2;
        if (canvas->videoconfig->doublesizex == 0
            && canvas->videoconfig->doublesizey == 0)
            canvas->videoconfig->rendermode = VIDEO_RENDER_RGB_1X1;
    }

    if (video_resource_chip->double_size_enabled != (int)v
        && canvas->initialized
        && canvas->viewport->update_canvas > 0) {
        video_viewport_resize(canvas);
    }

    video_resource_chip->double_size_enabled = (int)v;

    return 0;
}

static const char *vname_chip_size[] = { "DoubleSize", NULL };

static resource_t resources_chip_size[] =
{
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_double_size_enabled, NULL },
    { NULL }
};

static int set_double_scan_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->double_scan_enabled = (int)v;
    canvas->videoconfig->doublescan = (int)v;

    if (canvas->initialized)
        video_canvas_refresh_all(canvas);

    return 0;
}

static const char *vname_chip_scan[] = { "DoubleScan", NULL };

static resource_t resources_chip_scan[] =
{
    { NULL, RES_INTEGER, (resource_value_t)1, NULL,
      set_double_scan_enabled, NULL },
    { NULL }
};

static int set_fullscreen_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->fullscreen_enabled = (int)v;

    if (canvas->initialized)
        return (video_chip_cap->fullscreen.enable)(canvas, (int)v);

    return 0;
}

static int set_fullscreen_double_size_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->fullscreen_double_size_enabled = (int)v;

    return (video_chip_cap->fullscreen.double_size)(canvas, (int)v);
}

static int set_fullscreen_double_scan_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->fullscreen_double_scan_enabled = (int)v;

    return (video_chip_cap->fullscreen.double_scan)(canvas, (int)v);
}

static int set_fullscreen_device(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    if (util_string_set(&video_resource_chip->fullscreen_device,
        (const char *)v))
        return 0;

    return (video_chip_cap->fullscreen.device)(canvas, (const char *)v);
}

static const char *vname_chip_fullscreen[] = {
    "Fullscreen", "FullscreenDoubleSize", "FullscreenDoubleScan",
    "FullscreenDevice", NULL
};

static resource_t resources_chip_fullscreen[] =
{
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_fullscreen_enabled, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_fullscreen_double_size_enabled, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_fullscreen_double_scan_enabled, NULL },
    { NULL, RES_STRING, (resource_value_t)NULL, NULL,
      set_fullscreen_device, NULL },
    { NULL }
};

static int set_fullscreen_mode(resource_value_t v, void *param)
{
    video_resource_chip_mode_t *video_resource_chip_mode;
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    video_canvas_t *canvas;

    unsigned device;

    video_resource_chip_mode = (video_resource_chip_mode_t *)param;
    video_resource_chip = video_resource_chip_mode->resource_chip;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    device = video_resource_chip_mode->device;

    video_resource_chip->fullscreen_mode[device] = (int)v;

    return (video_chip_cap->fullscreen.mode[device])(canvas, (int)v);
}

static const char *vname_chip_fullscreen_mode[] = { "FullscreenMode", NULL };

static resource_t resources_chip_fullscreen_mode[] =
{
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_fullscreen_mode, NULL },
    { NULL }
};

int video_resources_chip_init(const char *chipname,
                              struct video_canvas_s **canvas,
                              video_chip_cap_t *video_chip_cap)
{
    unsigned int i;
    video_resource_chip_t *resource_chip;

    resource_chip
        = (video_resource_chip_t *)xcalloc(1, sizeof(video_resource_chip_t));

    video_render_initconfig((*canvas)->videoconfig);

    /* Set single size render as default.  */
    (*canvas)->videoconfig->rendermode = video_chip_cap->single_mode.rmode;
    (*canvas)->videoconfig->doublesizex
        = video_chip_cap->single_mode.sizex > 1 ? 1 : 0;
    (*canvas)->videoconfig->doublesizey
        = video_chip_cap->single_mode.sizey > 1 ? 1 : 0;

    resource_chip->canvas = canvas;
    resource_chip->video_chip_cap = video_chip_cap;

    if (video_chip_cap->dscan_allowed != 0) {
        resources_chip_scan[0].name
            = util_concat(chipname, vname_chip_scan[0], NULL);
        resources_chip_scan[0].value_ptr
            = (resource_value_t *)&(resource_chip->double_scan_enabled);
        resources_chip_scan[0].param = (void *)resource_chip;
        if (resources_register(resources_chip_scan) < 0)
            return -1;
    }

    if (video_chip_cap->dsize_allowed != 0) {
        resources_chip_size[0].name
            = util_concat(chipname, vname_chip_size[0], NULL);
        resources_chip_size[0].factory_value
            = (const resource_value_t)video_chip_cap->dsize_default;
        resources_chip_size[0].value_ptr
            = (resource_value_t *)&(resource_chip->double_size_enabled);
        resources_chip_size[0].param = (void *)resource_chip;
        if (resources_register(resources_chip_size) < 0)
            return -1;
    }

    if (video_chip_cap->fullscreen.device_num > 0) {
        video_resource_chip_mode_t *resource_chip_mode;

        resources_chip_fullscreen[0].name
            = util_concat(chipname, vname_chip_fullscreen[0], NULL);
        resources_chip_fullscreen[0].value_ptr
            = (resource_value_t *)&(resource_chip->fullscreen_enabled);
        resources_chip_fullscreen[0].param = (void *)resource_chip;

        resources_chip_fullscreen[1].name
            = util_concat(chipname, vname_chip_fullscreen[1], NULL);
        resources_chip_fullscreen[1].value_ptr
            = (resource_value_t *)&(resource_chip->fullscreen_double_size_enabled);
        resources_chip_fullscreen[1].param = (void *)resource_chip;

        resources_chip_fullscreen[2].name
            = util_concat(chipname, vname_chip_fullscreen[2], NULL);
        resources_chip_fullscreen[2].value_ptr
            = (resource_value_t *)&(resource_chip->fullscreen_double_scan_enabled);
        resources_chip_fullscreen[2].param = (void *)resource_chip;

        resources_chip_fullscreen[3].name
            = util_concat(chipname, vname_chip_fullscreen[3], NULL);
        resources_chip_fullscreen[3].factory_value
            = (const resource_value_t)(video_chip_cap->fullscreen.device_name[0]);
        resources_chip_fullscreen[3].value_ptr
            = (resource_value_t *)&(resource_chip->fullscreen_device);
        resources_chip_fullscreen[3].param = (void *)resource_chip;

        if (resources_register(resources_chip_fullscreen) < 0)
            return -1;

        for (i = 0; i < video_chip_cap->fullscreen.device_num; i++) {
            resource_chip_mode = (video_resource_chip_mode_t *)lib_malloc(
                                 sizeof(video_resource_chip_mode_t));
            resource_chip_mode->resource_chip = resource_chip;
            resource_chip_mode->device = i;

            resources_chip_fullscreen_mode[0].name
                = util_concat(chipname,
                    video_chip_cap->fullscreen.device_name[i],
                    vname_chip_fullscreen_mode[0], NULL);
            resources_chip_fullscreen_mode[0].value_ptr
                = (resource_value_t *)&(resource_chip->fullscreen_mode[i]);
            resources_chip_fullscreen_mode[0].param
                = (void *)resource_chip_mode;

            if (resources_register(resources_chip_fullscreen_mode) < 0)
                return -1;
        }
    }

    return 0;
}

