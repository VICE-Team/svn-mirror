/*
 * pngdrv.c - Create a PNG file.
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
#include "gfxoutput.h"
#include "screenshot.h"
#include "types.h"
#include "utils.h"

typedef struct gfxoutputdrv_data_s {
    FILE *fd;
    char *ext_filename;
    png_structp png_ptr;
    png_infop info_ptr;
    BYTE *data;
    unsigned int line;
} gfxoutputdrv_data_t;

static gfxoutputdrv_t png_drv;

int pngdrv_open(screenshot_t *screenshot, const char *filename)
{
    screenshot->gfxoutputdrv_data
        = (gfxoutputdrv_data_t *)xmalloc(sizeof(gfxoutputdrv_data_t));

    screenshot->gfxoutputdrv_data->png_ptr
        = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                  (void *)NULL, NULL, NULL);

    if (screenshot->gfxoutputdrv_data->png_ptr == NULL) {
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    screenshot->gfxoutputdrv_data->info_ptr
        = png_create_info_struct(screenshot->gfxoutputdrv_data->png_ptr);

    if (screenshot->gfxoutputdrv_data->info_ptr == NULL) {
        png_destroy_write_struct(&(screenshot->gfxoutputdrv_data->png_ptr),
                                 (png_infopp)NULL);
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    if (setjmp(screenshot->gfxoutputdrv_data->png_ptr->jmpbuf)) {
        png_destroy_write_struct(&(screenshot->gfxoutputdrv_data->png_ptr),
                                 &(screenshot->gfxoutputdrv_data->info_ptr));
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    screenshot->gfxoutputdrv_data->ext_filename
        = util_add_extension_const(filename, png_drv.default_extension);

    screenshot->gfxoutputdrv_data->fd
        = fopen(screenshot->gfxoutputdrv_data->ext_filename, MODE_WRITE);

    if (screenshot->gfxoutputdrv_data->fd == NULL) {
        free(screenshot->gfxoutputdrv_data->ext_filename);
        free(screenshot->gfxoutputdrv_data);
        return -1;
    }

    screenshot->gfxoutputdrv_data->data
        = (BYTE *)xmalloc(screenshot->width * 4);

    png_init_io(screenshot->gfxoutputdrv_data->png_ptr,
                screenshot->gfxoutputdrv_data->fd);
    png_set_compression_level(screenshot->gfxoutputdrv_data->png_ptr,
                              Z_BEST_COMPRESSION);

    screenshot->gfxoutputdrv_data->info_ptr->width = screenshot->width;
    screenshot->gfxoutputdrv_data->info_ptr->height= screenshot->height;
    screenshot->gfxoutputdrv_data->info_ptr->bit_depth = 8;
    screenshot->gfxoutputdrv_data->info_ptr->color_type
        = PNG_COLOR_TYPE_RGB_ALPHA;
    png_write_info(screenshot->gfxoutputdrv_data->png_ptr,
                   screenshot->gfxoutputdrv_data->info_ptr);
    png_set_invert_alpha(screenshot->gfxoutputdrv_data->png_ptr);

    return 0;
}

int pngdrv_write(screenshot_t *screenshot)
{
    (screenshot->convert_line)(screenshot, screenshot->gfxoutputdrv_data->data,
                               screenshot->gfxoutputdrv_data->line,
                               SCREENSHOT_MODE_RGB32);
    png_write_row(screenshot->gfxoutputdrv_data->png_ptr,
                  (png_bytep)(screenshot->gfxoutputdrv_data->data));

    return 0;
}

int pngdrv_close(screenshot_t *screenshot)
{
    png_write_end(screenshot->gfxoutputdrv_data->png_ptr,
                  screenshot->gfxoutputdrv_data->info_ptr);

    png_destroy_write_struct(&(screenshot->gfxoutputdrv_data->png_ptr),
                             &(screenshot->gfxoutputdrv_data->info_ptr));

    fclose(screenshot->gfxoutputdrv_data->fd);
    free(screenshot->gfxoutputdrv_data->data);
    free(screenshot->gfxoutputdrv_data->ext_filename);
    free(screenshot->gfxoutputdrv_data);

    return 0;
}

int pngdrv_save(screenshot_t *screenshot, const char *filename)
{
    if (pngdrv_open(screenshot, filename) < 0)
        return -1;

    for (screenshot->gfxoutputdrv_data->line = 0;
        screenshot->gfxoutputdrv_data->line < screenshot->height;
        (screenshot->gfxoutputdrv_data->line)++) {
        pngdrv_write(screenshot);
    }

    if (pngdrv_close(screenshot) < 0)
        return -1;

    return 0;
}

static gfxoutputdrv_t png_drv =
{
    "PNG",
    "png",
    pngdrv_open,
    pngdrv_close,
    pngdrv_write,
    pngdrv_save
};

void gfxoutput_init_png(void)
{
    gfxoutput_register(&png_drv);
}

