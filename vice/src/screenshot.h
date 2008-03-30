/*
 * screenshot.h - Create a screenshot.
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

#ifndef _SCREENSHOT_H
#define _SCREENSHOT_H

#include <stdio.h>

#include "types.h"

struct palette_s;
struct video_canvas_s;
struct gfxoutputdrv_data_s;

typedef struct screenshot_s {
    struct palette_s *palette;
    BYTE *color_map;
    BYTE *draw_buffer;
    unsigned int draw_buffer_line_size;

    /* Screen size we actually want to save.  */
    unsigned int width;
    unsigned int height;

    /* Overall screen size.  */
    unsigned int max_width;
    unsigned int max_height;

    /* First and last displayed line.  */
    unsigned int first_displayed_line;
    unsigned int last_displayed_line;

    /* Offset to the overall screen.  */
    unsigned int x_offset;
    unsigned int y_offset;

    /* Pixel size.  */
    unsigned int size_width;
    unsigned int size_height;

    /* Upper left corner of viewport.  */
    unsigned int first_displayed_col;

    /* Line data convert function.  */
    void (*convert_line)(struct screenshot_s *screenshot, BYTE *data,
                         unsigned int line, unsigned int mode);

    /* Pointer for graphics outout driver internal data.  */
    struct gfxoutputdrv_data_s *gfxoutputdrv_data;
} screenshot_t;

#define SCREENSHOT_MODE_PALETTE 0
#define SCREENSHOT_MODE_RGB32   1

/* Functions called by external emulator code.  */
extern int screenshot_init(void);

extern int screenshot_save(const char *drvname, const char *filename,
                           unsigned int window_number);
extern int screenshot_canvas_save(const char *drvname, const char *filename,
                                  struct video_canvas_s *canvas);
#endif

