/*
 * png.c - Create a PNG file.
 *
 * Written by
 *  Daniel Sladic <dsladic@cs.cmu.edu>
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

#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "archdep.h"
#include "screenshot.h"
#include "types.h"
#include "utils.h"

int png_save(screenshot_t *screenshot, const char *filename)
{
    unsigned int line;
    BYTE *data;
    FILE *fd;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING,
                                       (void *)NULL, NULL, NULL);

    if (!png_ptr)
        return -1;

    info_ptr = png_create_info_struct (png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct (&png_ptr, (png_infopp)NULL);
        return -1;
    }

    if (setjmp (png_ptr->jmpbuf)) {
        png_destroy_write_struct (&png_ptr, &info_ptr);
        return -1;
    }

    fd = fopen(filename, MODE_WRITE);

    if (fd == NULL)
        return -1;

    data = (BYTE *)xmalloc(screenshot->width * 4);

    png_init_io (png_ptr, fd);
    png_set_compression_level (png_ptr, Z_BEST_COMPRESSION);

    info_ptr->width = screenshot->width;
    info_ptr->height= screenshot->height;
    info_ptr->bit_depth = 8;
    info_ptr->color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    png_write_info (png_ptr, info_ptr);
    png_set_invert_alpha(png_ptr);

    for (line = 0; line < screenshot->height; line++) {
        screenshot_line_data(screenshot, data, line, SCREENSHOT_MODE_RGB32);
	png_write_row (png_ptr, (png_bytep)data);
    }

    png_write_end (png_ptr, info_ptr);

    png_destroy_write_struct (&png_ptr, &info_ptr);
    free(data);
    fclose(fd);

    return 0;
}

static screendrv_t png_drv =
{
    "PNG",
    png_save
};

void screenshot_init_png(void)
{
    screenshot_register(&png_drv);
}

