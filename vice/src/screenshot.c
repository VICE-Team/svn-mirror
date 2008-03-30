/*
 * screenshot.c - Create a screenshot.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "machine.h"
#include "palette.h"
#include "screenshot.h"
#include "utils.h"
#include "video.h"

struct screendrv_list_s {
    struct screendrv_s *drv;
    struct screendrv_list_s *next;
};
typedef struct screendrv_list_s screendrv_list_t;

static screendrv_list_t *screendrv_list;
static int screendrv_list_count = 0;

static log_t screenshot_log = LOG_ERR;
static screendrv_list_t *screendrv_list_iter = NULL;

int screenshot_num_drivers(void)
{
    return screendrv_list_count;
}

screendrv_t *screenshot_drivers_iter_init(void)
{
    screendrv_list_iter = screendrv_list;
    return screendrv_list_iter->drv;
}

screendrv_t *screenshot_drivers_iter_next(void)
{
    if (screendrv_list_iter)
        screendrv_list_iter = screendrv_list_iter->next;

    if (screendrv_list_iter)
        return screendrv_list_iter->drv;

    return NULL;
}

int screenshot_init(void)
{
    /* Setup logging system.  */
    screenshot_log = log_open("Screenshot");

    /* Initialize screen driver list.  */
    screendrv_list = (screendrv_list_t *)xmalloc(sizeof(screendrv_list_t));
    screendrv_list->drv = NULL;
    screendrv_list->next = NULL;

#if 1
    screenshot_init_bmp();
#endif
#if HAVE_PNG
    screenshot_init_png();
#endif
    return 0;
}

/*-----------------------------------------------------------------------*/

int screenshot_register(screendrv_t *drv)
{
    screendrv_list_t *current;

    current = screendrv_list;

    /* Warp to end of list.  */
    while (current->next != NULL)
        current = current->next;

    /* Fill in entry.  */
    current->drv = drv;
    current->next = (screendrv_list_t *)xmalloc(sizeof(screendrv_list_t));
    current->next->drv = NULL;
    current->next->next = NULL;

    screendrv_list_count++;

    return 0;
}

void screenshot_line_data(screenshot_t *screenshot, BYTE *data,
                          unsigned int line, unsigned int mode)
{
    unsigned int i;
    PIXEL *line_base;
    PIXEL color;

    if (line > screenshot->height) {
        log_error(screenshot_log, "Invalild line `%i' request.", line);
        return;
    }

    line_base = VIDEO_FRAME_BUFFER_LINE_START((screenshot->frame_buffer),
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

static screendrv_t *screenshot_get_driver(const char *drvname)
{
    screendrv_list_t *current = screendrv_list;

    while (current->next != NULL) {
       if (strcmp(drvname, current->drv->name) == 0)
           break;
       current = current->next;
    }

    /* Requested screenshot driver is not registered.  */
    if (current->next == NULL) {
        log_error(screenshot_log, "Request screenshot driver %s not found.",
                  drvname);
        return NULL;
    }
    return current->drv;
}

static int screenshot_save_core(screenshot_t *screenshot, screendrv_t *drv,
                                const char *filename)
{
    unsigned int i;

    screenshot->width  = screenshot->max_width & ~3;
    screenshot->height = screenshot->last_displayed_line
                       - screenshot->first_displayed_line;
    screenshot->y_offset = screenshot->first_displayed_line;

    screenshot->color_map = (PIXEL *)xmalloc(256 * sizeof(PIXEL));
    memset(screenshot->color_map, 0, 256 * sizeof(PIXEL));

    for (i = 0; i < screenshot->palette->num_entries; i++)
        screenshot->color_map[screenshot->pixel_table_sing[i]] = i;

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
    screendrv_t *drv;

    if ((drv = screenshot_get_driver(drvname)) == NULL)
        return -1;

    /* Retrive framebuffer and screen geometry.  */
    if (machine_screenshot(&screenshot, window_number) < 0) {
        log_error(screenshot_log, "Retrieving screen geometry failed.");
        return -1;
    }

    return screenshot_save_core(&screenshot, drv, filename);
}

int screenshot_canvas_save(const char *drvname, const char *filename,
                           struct canvas_s *canvas)
{
    screenshot_t screenshot;
    screendrv_t *drv;

    if ((drv = screenshot_get_driver(drvname)) == NULL)
        return -1;

    if (machine_canvas_screenshot(&screenshot, canvas) < 0) {
        log_error(screenshot_log, "Retrieving screen geometry failed.");
        return -1;
    }

    return screenshot_save_core(&screenshot, drv, filename);
}
