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
#include "lib.h"
#include "video-resources.h"
#include "video-color.h"
#include "video.h"
#include "videoarch.h"
#include "util.h"


video_resources_t video_resources =
{
    1000, 1100, 1100, 880, 500,
    0, 0, 0
};

int video_resources_init(void)
{
    return video_arch_resources_init();
}

void video_resources_shutdown(void)
{
    video_arch_resources_shutdown();
}

/*-----------------------------------------------------------------------*/
/* Per chip resources.  */

struct video_resource_chip_s {
    struct video_canvas_s **canvas;
    video_chip_cap_t *video_chip_cap;
    int double_scan_enabled;
    int double_size_enabled;
    int hwscale_enabled;
    int scale2x_enabled;
    int fullscreen_enabled;
    char *fullscreen_device;
    int fullscreen_double_size_enabled;
    int fullscreen_double_scan_enabled;
    int fullscreen_mode[FULLSCREEN_MAXDEV];
    int external_palette_enabled;
    char *external_palette_name;
};
typedef struct video_resource_chip_s video_resource_chip_t;

static void video_resources_update_ui(video_resource_chip_t *video_resource_chip);

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
    int old_doublesizex, old_doublesizey;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    if ((int)v)
        cap_render = &video_chip_cap->double_mode;
    else
        cap_render = &video_chip_cap->single_mode;

    canvas->videoconfig->rendermode = cap_render->rmode;

    old_doublesizex = canvas->videoconfig->doublesizex;
    old_doublesizey = canvas->videoconfig->doublesizey;

    if (cap_render->sizex > 1
        && (video_chip_cap->dsize_limit_width == 0
        || (canvas->draw_buffer->canvas_width > 0
        && canvas->draw_buffer->canvas_width
        <= video_chip_cap->dsize_limit_width)))
        canvas->videoconfig->doublesizex = 1;
    else
        canvas->videoconfig->doublesizex = 0;

    if (cap_render->sizey > 1
        && (video_chip_cap->dsize_limit_height == 0
        || (canvas->draw_buffer->canvas_height > 0
        && canvas->draw_buffer->canvas_height
        <= video_chip_cap->dsize_limit_height)))
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

    if ((video_resource_chip->double_size_enabled != (int)v
        || old_doublesizex != canvas->videoconfig->doublesizex
        || old_doublesizey != canvas->videoconfig->doublesizey)
        && canvas->initialized
        && canvas->viewport->update_canvas > 0) {
        video_viewport_resize(canvas);
    }

    video_resource_chip->double_size_enabled = (int)v;

    video_resources_update_ui(video_resource_chip);

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

    video_resources_update_ui(video_resource_chip);

    return 0;
}

static const char *vname_chip_scan[] = { "DoubleScan", NULL };

static resource_t resources_chip_scan[] =
{
    { NULL, RES_INTEGER, (resource_value_t)1, NULL,
      set_double_scan_enabled, NULL },
    { NULL }
};

static int set_hwscale_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->hwscale_enabled = (int)v;
    canvas->videoconfig->hwscale = (int)v;

    if (canvas->initialized) {
        video_viewport_resize(canvas);
    }

    video_resources_update_ui(video_resource_chip);

    return 0;
}

static const char *vname_chip_hwscale[] = { "HwScale", NULL };

static resource_t resources_chip_hwscale[] =
{
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_hwscale_enabled, NULL },
    { NULL }
};

static int set_scale2x_enabled(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->scale2x_enabled = (int)v;
    canvas->videoconfig->scale2x = (int)v;

    if (canvas->initialized)
        video_canvas_refresh_all(canvas);

    video_resources_update_ui(video_resource_chip);

    return 0;
}

static const char *vname_chip_scale2x[] = { "Scale2x", NULL };

static resource_t resources_chip_scale2x[] =
{
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_scale2x_enabled, NULL },
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

static int set_ext_palette(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_chip_cap_t *video_chip_cap;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    video_chip_cap = video_resource_chip->video_chip_cap;
    canvas = *(video_resource_chip->canvas);

    video_resource_chip->external_palette_enabled = (int)v;
    canvas->videoconfig->external_palette = (unsigned int)v;

    return video_color_update_palette(canvas);
}

static int set_palette_file_name(resource_value_t v, void *param)
{
    video_resource_chip_t *video_resource_chip;
    video_canvas_t *canvas;

    video_resource_chip = (video_resource_chip_t *)param;
    canvas = *(video_resource_chip->canvas);

    util_string_set(&(video_resource_chip->external_palette_name), (char *)v);
    canvas->videoconfig->external_palette_name
        = video_resource_chip->external_palette_name;

    return video_color_update_palette(canvas);
}

static const char *vname_chip_palette[] = { "PaletteFile", "ExternalPalette",
                                            NULL };

static resource_t resources_chip_palette[] =
{
    { NULL, RES_STRING, NULL, NULL,
      set_palette_file_name, NULL },
    { NULL, RES_INTEGER, (resource_value_t)0, NULL,
      set_ext_palette, NULL },
    { NULL }
};

int video_resources_chip_init(const char *chipname,
                              struct video_canvas_s **canvas,
                              video_chip_cap_t *video_chip_cap)
{
    unsigned int i;
    video_resource_chip_t *resource_chip;

    resource_chip
        = (video_resource_chip_t *)lib_calloc(1, sizeof(video_resource_chip_t));
    (*canvas)->video_resource_chip = resource_chip;

    video_render_initconfig((*canvas)->videoconfig);
    (*canvas)->videoconfig->cap = video_chip_cap;

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

        lib_free((char *)(resources_chip_scan[0].name));
    }

    if (video_chip_cap->hwscale_allowed != 0) {
        resources_chip_hwscale[0].name
            = util_concat(chipname, vname_chip_hwscale[0], NULL);
        resources_chip_hwscale[0].value_ptr
            = (resource_value_t *)&(resource_chip->hwscale_enabled);
        resources_chip_hwscale[0].param = (void *)resource_chip;
        if (resources_register(resources_chip_hwscale) < 0)
            return -1;

        lib_free((char *)(resources_chip_hwscale[0].name));
    }

    if (video_chip_cap->scale2x_allowed != 0) {
        resources_chip_scale2x[0].name
            = util_concat(chipname, vname_chip_scale2x[0], NULL);
        resources_chip_scale2x[0].value_ptr
            = (resource_value_t *)&(resource_chip->scale2x_enabled);
        resources_chip_scale2x[0].param = (void *)resource_chip;
        if (resources_register(resources_chip_scale2x) < 0)
            return -1;

        lib_free((char *)(resources_chip_scale2x[0].name));
    }

    if (video_chip_cap->dsize_allowed != 0) {
        resources_chip_size[0].name
            = util_concat(chipname, vname_chip_size[0], NULL);
        resources_chip_size[0].factory_value
            = (resource_value_t)video_chip_cap->dsize_default;
        resources_chip_size[0].value_ptr
            = (resource_value_t *)&(resource_chip->double_size_enabled);
        resources_chip_size[0].param = (void *)resource_chip;
        if (resources_register(resources_chip_size) < 0)
            return -1;

        lib_free((char *)(resources_chip_size[0].name));
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
            = (resource_value_t)(video_chip_cap->fullscreen.device_name[0]);
        resources_chip_fullscreen[3].value_ptr
            = (resource_value_t *)&(resource_chip->fullscreen_device);
        resources_chip_fullscreen[3].param = (void *)resource_chip;

        if (resources_register(resources_chip_fullscreen) < 0)
            return -1;

        lib_free((char *)(resources_chip_fullscreen[0].name));
        lib_free((char *)(resources_chip_fullscreen[1].name));
        lib_free((char *)(resources_chip_fullscreen[2].name));
        lib_free((char *)(resources_chip_fullscreen[3].name));

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

            lib_free((char *)(resources_chip_fullscreen_mode[0].name));
        }
    }

    resources_chip_palette[0].name
        = util_concat(chipname, vname_chip_palette[0], NULL);
    resources_chip_palette[0].factory_value
        = (resource_value_t)video_chip_cap->external_palette_name;
    resources_chip_palette[0].value_ptr
        = (resource_value_t *)&(resource_chip->external_palette_name);
    resources_chip_palette[0].param = (void *)resource_chip;

    if (video_chip_cap->internal_palette_allowed != 0) {
        resources_chip_palette[1].name
            = util_concat(chipname, vname_chip_palette[1], NULL);
        resources_chip_palette[1].value_ptr
            = (resource_value_t *)&(resource_chip->external_palette_enabled);
        resources_chip_palette[1].param = (void *)resource_chip;
    } else {
        resources_chip_palette[1].name = NULL;
        resource_chip->external_palette_enabled = 1;
        (*canvas)->videoconfig->external_palette = 1;
    }

    if (resources_register(resources_chip_palette) < 0)
        return -1;

    lib_free((char *)(resources_chip_palette[0].name));
    if (video_chip_cap->internal_palette_allowed != 0)
        lib_free((char *)(resources_chip_palette[1].name));

    return 0;
}

void video_resources_chip_shutdown(struct video_canvas_s *canvas)
{
    lib_free(canvas->video_resource_chip->external_palette_name);
    lib_free(canvas->video_resource_chip);
}

void video_resources_update_ui(video_resource_chip_t *video_resource_chip)
{
    int pal_enabled = 0;
    int ui_doublescan_enabled, ui_scale2x_enabled;

    if (video_resource_chip->video_chip_cap->palemulation_allowed)
        resources_get_value("PALEmulation", &pal_enabled);

    if (video_resource_chip->double_size_enabled != 0) {
        if (pal_enabled) {
            ui_doublescan_enabled = 1;
            ui_scale2x_enabled = 0;
        } else if (video_resource_chip->scale2x_enabled != 0) {
            ui_doublescan_enabled = 0;
            ui_scale2x_enabled = 1;
        } else {
            ui_doublescan_enabled = 1;
            ui_scale2x_enabled = 1;
        }
    } else {
        ui_doublescan_enabled = 0;
        ui_scale2x_enabled = 0;
    }
/*
    ui_enable_chip resources(ui_doublescan_enabled, ui_scale2x_enabled);
*/
}

