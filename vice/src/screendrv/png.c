/*
 * png.c - Create a PNG file.
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
#include <stdlib.h>

#include "screenshot.h"
#include "types.h"
#include "utils.h"

int png_save(screenshot_t *screenshot, const char *filename)
{
    unsigned int line, x;
    BYTE *data;

    data = (BYTE *)xmalloc(screenshot->width * 4);

    for (line = 0; line < screenshot->height; line++) {

        printf("LINE %03i\n", line);

        screenshot_line_data(screenshot, data, line, SCREENSHOT_MODE_RGB32);

        for (x = 0; x < screenshot->width * 4; x++) {
            printf("%02x ", data[x]);
        }
        printf("\n");
    }

    free(data);
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

