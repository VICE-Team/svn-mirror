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

#include "log.h"
#include "machine.h"
#include "screenshot.h"
#include "utils.h"

struct screendrv_list_s {
    struct screendrv_s *drv;
    struct screendrv_list_s *next;
};
typedef struct screendrv_list_s screendrv_list_t;

static screendrv_list_t *screendrv_list;

static log_t screenshot_log = LOG_ERR;

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

    return 0;
}

int screenshot_save(const char *drvname, const char *filename,
                    unsigned int window_number)
{
    screenshot_t screenshot;
    screendrv_list_t *current;

    current = screendrv_list;

    while (current->next != NULL) {
       if (strcmp(drvname, current->drv->name) == 0)
           break;
    }

    /* Requested screenshot driver is not registered.  */
    if (current->next == NULL) {
        log_error(screenshot_log, "Request screenshot driver %s not found.",
                  drvname);
        return -1;
    }

    /* Retrive framebuffer and screen geometry.  */
    if (machine_screenshot(&screenshot, window_number) < 0) {
        log_error(screenshot_log, "Retriving screen geometry failed.");
        return -1;
    }

    if ((current->drv->save)(&screenshot, filename) < 0) {
        log_error(screenshot_log, "Saving failed...");
        return -1;
    }

    return 0;
}

