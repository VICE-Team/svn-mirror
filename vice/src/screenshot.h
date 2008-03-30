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

struct video_frame_buffer_s;

typedef struct screenshot_s {
    struct video_frame_buffer_s *frame_buffer;
    unsigned int width;
    unsigned int height;
} screenshot_t;

typedef struct screendrv_s {
    const char *name;
    int (*save)(struct screenshot_s *, const char *);
} screendrv_t;

extern int screenshot_init(void);
extern int screenshot_register(screendrv_t *drv);
extern int screenshot_save(const char *drvname, const char *filename,
                           unsigned int window_number);

/* Initialization prototypes */
extern void screenshot_init_bmp(void);
extern void screenshot_init_png(void);

#endif

