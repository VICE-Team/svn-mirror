/*
 * screenshot.c - Create a screenshot.
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
#include <stdlib.h>
#include <string.h>

#include "gfxoutput.h"
#include "log.h"
#include "machine.h"
#include "palette.h"
#include "screenshot.h"
#include "utils.h"
#include "video.h"

static log_t screenshot_log = LOG_ERR;

int screenshot_init(void)
{
    /* Setup logging system.  */
    screenshot_log = log_open("Screenshot");

    return 0;
}

/*-----------------------------------------------------------------------*/

static void screenshot_line_data(screenshot_t *screenshot, BYTE *data,
                                 unsigned int line, unsigned int mode)
{
    unsigned int i;
    BYTE *line_base;
    BYTE color;

    if (line > screenshot->height) {
        log_error(screenshot_log, "Invalild line `%i' request.", line);
        return;
    }

#define BUFFER_LINE_START(i, n) ((i)->draw_buffer \
                                + (n) * (i)->draw_buffer_line_size)

    line_base = BUFFER_LINE_START(screenshot,
                                  (line + screenshot->y_offset)
                                  * screenshot->size_height);

    switch (mode) {
      case SCREENSHOT_MODE_PALETTE:
        for (i = 0; i < screenshot->width; i++)
            data[i] = screenshot->color_map[line_base[i
                      * screenshot->size_width + screenshot->x_offset]];
        break;
      case SCREENSHOT_MODE_RGB32:
        for (i = 0; i < screenshot->width; i++) {
            color = screenshot->color_map[line_base[i
                    * screenshot->size_width + screenshot->x_offset]];
            data[i * 4] = screenshot->palette->entries[color].red;
            data[i * 4 + 1] = screenshot->palette->entries[color].green;
            data[i * 4 + 2] = screenshot->palette->entries[color].blue;
            data[i * 4 + 3] = 0;
        }
        break;
      default:
        log_error(screenshot_log, "Invalid mode %i.", mode);
    }
}

/*-----------------------------------------------------------------------*/

static int screenshot_save_core(screenshot_t *screenshot, gfxoutputdrv_t *drv,
                                const char *filename)
{
    unsigned int i;

    screenshot->width  = screenshot->max_width & ~3;
    screenshot->height = screenshot->last_displayed_line
                       - screenshot->first_displayed_line;
    screenshot->y_offset = screenshot->first_displayed_line;

    screenshot->color_map = (BYTE *)xcalloc(1, 256);

    for (i = 0; i < screenshot->palette->num_entries; i++)
        screenshot->color_map[screenshot->pixel_table_sing[i]] = i;

    screenshot->convert_line = screenshot_line_data;

    if ((drv->save)(screenshot, filename) < 0) {
        log_error(screenshot_log, "Saving failed...");
        free(screenshot->color_map);
        return -1;
    }

    free(screenshot->color_map);
    return 0;
}

/*-----------------------------------------------------------------------*/

int screenshot_save(const char *drvname, const char *filename,
                    unsigned int window_number)
{
    screenshot_t screenshot;
    gfxoutputdrv_t *drv;

    if ((drv = gfxoutput_get_driver(drvname)) == NULL)
        return -1;

    /* Retrive framebuffer and screen geometry.  */
    if (machine_screenshot(&screenshot, window_number) < 0) {
        log_error(screenshot_log, "Retrieving screen geometry failed.");
        return -1;
    }

    return screenshot_save_core(&screenshot, drv, filename);
}

int screenshot_canvas_save(const char *drvname, const char *filename,
                           struct video_canvas_s *canvas)
{
    screenshot_t screenshot;
    gfxoutputdrv_t *drv;

    if ((drv = gfxoutput_get_driver(drvname)) == NULL)
        return -1;

    if (machine_canvas_screenshot(&screenshot, canvas) < 0) {
        log_error(screenshot_log, "Retrieving screen geometry failed.");
        return -1;
    }

    return screenshot_save_core(&screenshot, drv, filename);
}

