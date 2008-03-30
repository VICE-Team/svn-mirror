/*
 * screenshot.h - Create a screenshot.
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

#ifndef _SCREENSHOT_H
#define _SCREENSHOT_H

#include "types.h"

struct video_frame_buffer_s;
struct palette_s;

typedef struct screenshot_s {
    struct video_frame_buffer_s *frame_buffer;
    struct palette_s *palette;
    PIXEL *pixel_table_sing;
    PIXEL *color_map;
    unsigned int width;
    unsigned int height;
} screenshot_t;

typedef struct screendrv_s {
    const char *name;
    int (*save)(struct screenshot_s *, const char *);
} screendrv_t;

#define SCREENSHOT_MODE_PALETTE 0
#define SCREENSHOT_MODE_RGB32   1

/* Functions called by external emulator code.  */
extern int screenshot_init(void);
extern int screenshot_save(const char *drvname, const char *filename,
                           unsigned int window_number);

/* Functions called by screenshot modules.  */
extern int screenshot_register(screendrv_t *drv);
extern void screenshot_line_data(screenshot_t *screenshot, BYTE *data,
                                 unsigned int line, unsigned int mode);

/* Initialization prototypes */
extern void screenshot_init_bmp(void);
extern void screenshot_init_png(void);

#endif

