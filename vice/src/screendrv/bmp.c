/*
 * bmp.c - Create a BMP file.
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

#include <stdio.h>

#include "log.h"
#include "screenshot.h"
#include "types.h"
#include "video.h"

int bmp_save(screenshot_t *screenshot, const char *filename)
{
    unsigned int x, y;
    PIXEL *line_start;

    for (y = 0; y < screenshot->height; y++) {

        line_start = FRAME_BUFFER_LINE_START(*(screenshot->frame_buffer), y);
        
        printf("LINE %03i\n", y);

        for (x = 0; x < screenshot->width; x++) {

            printf("%02x ", line_start[x]);
        }
        printf("\n");
    }

    return 0;
}

static screendrv_t bmp_drv =
{
    "BMP",
    bmp_save
};

void screenshot_init_bmp(void)
{
    screenshot_register(&bmp_drv);
}

