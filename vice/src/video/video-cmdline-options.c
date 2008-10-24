/*
 * video-cmdline-options.c
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

#include "cmdline.h"
#include "lib.h"
#include "translate.h"
#include "util.h"
#include "video.h"

static const char *cname_chip_size[] =
{
    "-", "dsize", "DoubleSize",
    "+", "dsize", "DoubleSize",
    NULL
};

static cmdline_option_t cmdline_options_chip_size[] =
{
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DOUBLE_SIZE,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DOUBLE_SIZE,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_scan[] =
{
    "-", "dscan", "DoubleScan",
    "+", "dscan", "DoubleScan",
    NULL
};

static cmdline_option_t cmdline_options_chip_scan[] =
{
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DOUBLE_SCAN,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DOUBLE_SCAN,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_hwscale[] =
{
    "-", "hwscale", "HwScale",
    "+", "hwscale", "HwScale",
    NULL
};

static cmdline_option_t cmdline_options_chip_hwscale[] =
{
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_HARDWARE_SCALING,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_HARDWARE_SCALING,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_scale2x[] =
{
    "-", "scale2x", "Scale2x",
    "+", "scale2x", "Scale2x",
    NULL
};

static cmdline_option_t cmdline_options_chip_scale2x[] =
{
    { NULL, SET_RESOURCE,
      0, NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_SCALE2X,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_SCALE2X,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_internal_palette[] =
{
    "-", "intpal", "ExternalPalette",
    "-", "extpal", "ExternalPalette",
    NULL
};

static cmdline_option_t cmdline_options_chip_internal_palette[] =
{
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_USE_INTERNAL_CALC_PALETTE,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_USE_EXTERNAL_FILE_PALETTE,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_palette[] =
{
    "-", "palette", "PaletteFile",
    NULL
};

static cmdline_option_t cmdline_options_chip_palette[] =
{
    { NULL, SET_RESOURCE, 1,
      NULL, NULL, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_EXTERNAL_PALETTE_NAME,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_fullscreen[] =
{
   "-", "full", "Fullscreen",
   "+", "full", "Fullscreen",
   "-", "fulldevice", "FullscreenDevice",
   "-", "fulldsize", "FullscreenDoubleSize",
   "+", "fulldsize", "FullscreenDoubleSize",
   "-", "fulldscan", "FullscreenDoubleScan",
   "+", "fulldscan", "FullscreenDoubleScan",
   NULL
};

static cmdline_option_t cmdline_options_chip_fullscreen[] =
{
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_FULLSCREEN_MODE,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_FULLSCREEN_MODE,
      NULL, NULL },
    { NULL, SET_RESOURCE, 1,
      NULL, NULL, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_DEVICE, IDCLS_SELECT_FULLSCREEN_DEVICE,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DOUBLE_SIZE_FULLSCREEN,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DOUBLE_SIZE_FULLSCREEN,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_DOUBLE_SCAN_FULLSCREEN,
      NULL, NULL },
    { NULL, SET_RESOURCE, 0,
      NULL, NULL, NULL, (void *)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_DOUBLE_SCAN_FULLSCREEN,
      NULL, NULL },
    { NULL }
};

static const char *cname_chip_fullscreen_mode[] =
{
    "-", "fullmode", "FullscreenMode",
    NULL
};

static cmdline_option_t cmdline_options_chip_fullscreen_mode[] =
{
    { NULL, SET_RESOURCE, 1,
      NULL, NULL, NULL, NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_MODE, IDCLS_SELECT_FULLSCREEN_MODE,
      NULL, NULL },
    { NULL }
};

int video_cmdline_options_chip_init(const char *chipname,
                                    video_chip_cap_t *video_chip_cap)
{
    unsigned int i, j;

    if (video_chip_cap->dsize_allowed) {
        for (i = 0; cname_chip_size[i * 3] != NULL; i++) {
            cmdline_options_chip_size[i].name
                = util_concat(cname_chip_size[i * 3], chipname,
                cname_chip_size[i * 3 + 1], NULL);
            cmdline_options_chip_size[i].resource_name
                = util_concat(chipname, cname_chip_size[i * 3 + 2], NULL);
        }

        if (cmdline_register_options(cmdline_options_chip_size) < 0)
            return -1;

        for (i = 0; cname_chip_size[i * 3] != NULL; i++) {
            lib_free((char *)cmdline_options_chip_size[i].name);
            lib_free((char *)cmdline_options_chip_size[i].resource_name);
        }
    }

    if (video_chip_cap->dscan_allowed) {
        for (i = 0; cname_chip_scan[i * 3] != NULL; i++) {
            cmdline_options_chip_scan[i].name
                = util_concat(cname_chip_scan[i * 3], chipname,
                cname_chip_scan[i * 3 + 1], NULL);
            cmdline_options_chip_scan[i].resource_name
                = util_concat(chipname, cname_chip_scan[i * 3 + 2], NULL);
        }

        if (cmdline_register_options(cmdline_options_chip_scan) < 0)
            return -1;

        for (i = 0; cname_chip_scan[i * 3] != NULL; i++) {
            lib_free((char *)cmdline_options_chip_scan[i].name);
            lib_free((char *)cmdline_options_chip_scan[i].resource_name);
        }
    }

    if (video_chip_cap->hwscale_allowed) {
        for (i = 0; cname_chip_hwscale[i * 3] != NULL; i++) {
            cmdline_options_chip_hwscale[i].name
                = util_concat(cname_chip_hwscale[i * 3], chipname,
                cname_chip_hwscale[i * 3 + 1], NULL);
            cmdline_options_chip_hwscale[i].resource_name
                = util_concat(chipname, cname_chip_hwscale[i * 3 + 2], NULL);
        }

        if (cmdline_register_options(cmdline_options_chip_hwscale) < 0)
            return -1;

        for (i = 0; cname_chip_hwscale[i * 3] != NULL; i++) {
            lib_free((char *)cmdline_options_chip_hwscale[i].name);
            lib_free((char *)cmdline_options_chip_hwscale[i].resource_name);
        }
    }

    if (video_chip_cap->scale2x_allowed) {
        for (i = 0; cname_chip_scale2x[i * 3] != NULL; i++) {
            cmdline_options_chip_scale2x[i].name
                = util_concat(cname_chip_scale2x[i * 3], chipname,
                cname_chip_scale2x[i * 3 + 1], NULL);
            cmdline_options_chip_scale2x[i].resource_name
                = util_concat(chipname, cname_chip_scale2x[i * 3 + 2], NULL);
        }

        if (cmdline_register_options(cmdline_options_chip_scale2x) < 0)
            return -1;

        for (i = 0; cname_chip_scale2x[i * 3] != NULL; i++) {
            lib_free((char *)cmdline_options_chip_scale2x[i].name);
            lib_free((char *)cmdline_options_chip_scale2x[i].resource_name);
        }
    }

    if (video_chip_cap->internal_palette_allowed) {
        for (i = 0; cname_chip_internal_palette[i * 3] != NULL; i++) {
            cmdline_options_chip_internal_palette[i].name
                = util_concat(cname_chip_internal_palette[i * 3], chipname,
                cname_chip_internal_palette[i * 3 + 1], NULL);
            cmdline_options_chip_internal_palette[i].resource_name
                = util_concat(chipname, cname_chip_internal_palette[i * 3 + 2],
                NULL);
        }

        if (cmdline_register_options(cmdline_options_chip_internal_palette)
            < 0)
            return -1;

        for (i = 0; cname_chip_internal_palette[i * 3] != NULL; i++) {
            lib_free((char *)cmdline_options_chip_internal_palette[i].name);
            lib_free((char *)cmdline_options_chip_internal_palette[i].resource_name);
        }
    }

    for (i = 0; cname_chip_palette[i * 3] != NULL; i++) {
        cmdline_options_chip_palette[i].name
            = util_concat(cname_chip_palette[i * 3], chipname,
            cname_chip_palette[i * 3 + 1], NULL);
        cmdline_options_chip_palette[i].resource_name
            = util_concat(chipname, cname_chip_palette[i * 3 + 2],
            NULL);
    }

    if (cmdline_register_options(cmdline_options_chip_palette) < 0)
        return -1;

    for (i = 0; cname_chip_palette[i * 3] != NULL; i++) {
        lib_free((char *)cmdline_options_chip_palette[i].name);
        lib_free((char *)cmdline_options_chip_palette[i].resource_name);
    }

    if (video_chip_cap->fullscreen.device_num > 0) {
        for (i = 0; cname_chip_fullscreen[i * 3] != NULL; i++) {
            cmdline_options_chip_fullscreen[i].name
                = util_concat(cname_chip_fullscreen[i * 3], chipname,
                cname_chip_fullscreen[i * 3 + 1], NULL);
            cmdline_options_chip_fullscreen[i].resource_name
                = util_concat(chipname, cname_chip_fullscreen[i * 3 + 2], NULL);
        }

        if (cmdline_register_options(cmdline_options_chip_fullscreen) < 0)
            return -1;

        for (i = 0; cname_chip_fullscreen[i * 3] != NULL; i++) {
            lib_free((char *)cmdline_options_chip_fullscreen[i].name);
            lib_free((char *)cmdline_options_chip_fullscreen[i].resource_name);
        }

        for (j = 0; j < video_chip_cap->fullscreen.device_num; j++) {
            for (i = 0; cname_chip_fullscreen_mode[i * 3] != NULL; i++) {
                cmdline_options_chip_fullscreen_mode[i].name
                    = util_concat(cname_chip_fullscreen_mode[i * 3], chipname,
                    video_chip_cap->fullscreen.device_name[j],
                    cname_chip_fullscreen_mode[i * 3 + 1], NULL);
                cmdline_options_chip_fullscreen_mode[i].resource_name
                    = util_concat(chipname,
                    video_chip_cap->fullscreen.device_name[j],
                    cname_chip_fullscreen_mode[i * 3 + 2], NULL);
            }

            if (cmdline_register_options(cmdline_options_chip_fullscreen_mode)
                < 0)
                return -1;

            for (i = 0; cname_chip_fullscreen_mode[i * 3] != NULL; i++) {
                lib_free((char *)cmdline_options_chip_fullscreen_mode[i].name);
                lib_free((char *)cmdline_options_chip_fullscreen_mode[i].resource_name);
            }
        }
    }

    return 0;
}

