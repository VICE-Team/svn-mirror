/**
 * \file video.c
 * \brief Headless UI video stuff
 *
 * \author Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author Michael C. Martin <mcmartin@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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
#include "machine.h"
#include "resources.h"
#include "uiserver.h"
#include "videoarch.h"
#include "video.h"


/** \brief  Command line options related to generic video output
 */
static const cmdline_option_t cmdline_options[] =
{
    CMDLINE_LIST_END
};


/** \brief  Integer/boolean resources related to video output
 */
static const resource_int_t resources_int[] =
{
    RESOURCE_INT_LIST_END
};

/** \brief  Arch-sepcific function to check which chip is
 *          currently "active", or has the focus of the user.
 *
 * Note: this version always returns VIDEO_CHIP_VICII since the headless build
 * has no concept of a canvas or even an active window, and this function only
 * makes sense in a "headful" build.
 */
int video_arch_get_active_chip(void)
{
    return VIDEO_CHIP_VICII;
}

/** \brief  Arch-specific initialization for a video canvas
 *  \param[inout] canvas The canvas being initialized
 *  \sa video_canvas_create
 */
void video_arch_canvas_init(struct video_canvas_s *canvas)
{
    printf("%s\n", __func__);
}


/** \brief  Initialize command line options for generic video resouces
 *
 * \return  0 on success, < 0 on failure
 */
int video_arch_cmdline_options_init(void)
{
    printf("%s\n", __func__);

    if (machine_class != VICE_MACHINE_VSID) {
        return cmdline_register_options(cmdline_options);
    }
    return 0;
}


/** \brief  Initialize video-related resources
 *
 * \return  0 on success, < on failure
 */
int video_arch_resources_init(void)
{
    printf("%s\n", __func__);

    if (machine_class != VICE_MACHINE_VSID) {
        return resources_register_int(resources_int);
    }
    return 0;
}

/** \brief Clean up any memory held by arch-specific video resources. */
void video_arch_resources_shutdown(void)
{
    printf("%s\n", __func__);
}

/** \brief Create a new video_canvas_s.
 *  \param[inout] canvas A freshly allocated canvas object.
 *  \param[in]    width  Pointer to a width value. May be NULL if canvas
 *                       size is not yet known.
 *  \param[in]    height Pointer to a height value. May be NULL if canvas
 *                       size is not yet known.
 *  \param        mapped Unused.
 *  \return The completely initialized canvas. The window that holds
 *          it will be visible in the UI at time of return.
 */
video_canvas_t *video_canvas_create(video_canvas_t *canvas,
                                    unsigned int *width, unsigned int *height,
                                    int mapped)
{
    printf("%s\n", __func__);

    uiserver_add_screen(canvas);

    canvas->created = 1;

    return canvas;
}

/** \brief Free a previously created video canvas and all its
 *         components.
 *  \param[in] canvas The canvas to destroy.
 */
void video_canvas_destroy(struct video_canvas_s *canvas)
{
    printf("%s\n", __func__);
}

/** \brief Assign a palette to the canvas.
 * \param canvas The canvas to update the palette
 * \param palette The new palette to assign
 * \return Zero on success, nonzero on failure
 */
int video_canvas_set_palette(struct video_canvas_s *canvas,
                             struct palette_s *palette)
{
    printf("%s\n", __func__);

    canvas->palette = palette;

    return 0;
}

/** \brief Perform any frontend-specific initialization.
 *  \return 0 on success, nonzero on failure
 */
int video_init(void)
{
    printf("%s\n", __func__);

    return 0;
}

/** \brief Perform any frontend-specific uninitialization. */
void video_shutdown(void)
{
    printf("%s\n", __func__);
}

void video_canvas_new_frame_hook(struct video_canvas_s *canvas)
{
    /* printf("%s\n", __func__); */
}

void video_canvas_on_new_backbuffer(video_canvas_t *canvas)
{
    /* printf("%s\n", __func__); */

    uiserver_on_new_backbuffer(canvas);
}