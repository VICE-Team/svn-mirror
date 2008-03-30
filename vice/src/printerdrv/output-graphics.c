/*
 * output-graphics.c - Output a graphics file.
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

#include "log.h"
#include "cmdline.h"
#include "gfxoutput.h"
#include "output-select.h"
#include "output-graphics.h"
#include "output.h"
#include "palette.h"
#include "resources.h"
#include "screenshot.h"
#include "utils.h"
#include "types.h"


struct output_gfx_s
{
    gfxoutputdrv_t *gfxoutputdrv;
    screenshot_t screenshot;
    BYTE *line;
    unsigned int line_pos;
};
typedef struct output_gfx_s output_gfx_t;

static output_gfx_t output_gfx[3];

static unsigned int current_prnr;

static int ppb;

static int set_ppb(resource_value_t v, void *param)
{
    ppb = (int)v;

    if (ppb<0) ppb=0;
    if (ppb>3) ppb=3;

    return 0;
}

static resource_t resources[] = {
    { "PixelsPerBit", RES_INTEGER, (resource_value_t)3,
      (resource_value_t *)&ppb, set_ppb, (void *)0 },
    { NULL }
};

static const cmdline_option_t cmdline_options[] =
{
    { "-ppb", SET_RESOURCE, 1, NULL, NULL, "PixelsPerBit", NULL,
      "<0-3>", "Number of pixel size in graphic [3]" },
    { NULL }
};

int output_graphics_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static void output_graphics_line_data(screenshot_t *screenshot, BYTE *data,
                                      unsigned int line, unsigned int mode)
{
    unsigned int i;
    BYTE *line_base;
    unsigned int color;

    line_base = output_gfx[current_prnr].line;

    switch (mode) {
      case SCREENSHOT_MODE_PALETTE:
        for (i = 0; i < screenshot->width; i++) {
            /* FIXME: Use a table here if color printers are introduced.  */
            if (line_base[i] == OUTPUT_PIXEL_BLACK)
                data[i] = 0;
            else
                data[i] = 1;
        }
        break;
      case SCREENSHOT_MODE_RGB32:
        for (i = 0; i < screenshot->width; i++) {
            /* FIXME: Use a table here if color printers are introduced.  */
            if (line_base[i] == OUTPUT_PIXEL_BLACK)
                color = 0;
            else
                color = 1;
            data[i * 4] = screenshot->palette->entries[color].red;
            data[i * 4 + 1] = screenshot->palette->entries[color].green;
            data[i * 4 + 2] = screenshot->palette->entries[color].blue;
            data[i * 4 + 3] = 0;
        }
        break;
      default:
        log_error(LOG_ERR, "Invalid mode %i.", mode);
    }
}

/* ------------------------------------------------------------------------- */

static int output_graphics_open(unsigned int prnr,
                                output_parameter_t *output_parameter)
{
    output_gfx[prnr].gfxoutputdrv = gfxoutput_get_driver("BMP");

    if (output_gfx[prnr].gfxoutputdrv == NULL)
        return -1;

    output_gfx[prnr].screenshot.width  = output_parameter->maxcol;
    output_gfx[prnr].screenshot.height = output_parameter->maxrow;
    output_gfx[prnr].screenshot.y_offset = 0;
    output_gfx[prnr].screenshot.palette = output_parameter->palette;

    free(output_gfx[prnr].line);
    output_gfx[prnr].line = (BYTE *)xmalloc(output_parameter->maxcol);
    memset(output_gfx[prnr].line, OUTPUT_PIXEL_WHITE, output_parameter->maxcol);

    output_gfx[prnr].screenshot.convert_line = output_graphics_line_data;

    output_gfx[prnr].gfxoutputdrv->open(&output_gfx[prnr].screenshot,
                                        "prngfx");

    return 0;
}

static void output_graphics_close(unsigned int prnr)
{
    output_gfx[prnr].gfxoutputdrv->close(&output_gfx[prnr].screenshot);
}

static int output_graphics_putc(unsigned int prnr, BYTE b)
{
    if (b == OUTPUT_NEWLINE) {
        current_prnr = prnr;
        (output_gfx[prnr].gfxoutputdrv->write)(&output_gfx[prnr].screenshot);
        memset(output_gfx[prnr].line, OUTPUT_PIXEL_WHITE,
               output_gfx[prnr].screenshot.width);
        output_gfx[prnr].line_pos = 0;
    } else {
        output_gfx[prnr].line[output_gfx[prnr].line_pos] = b;
        if (output_gfx[prnr].line_pos < output_gfx[prnr].screenshot.width - 1)
            output_gfx[prnr].line_pos++;
    }
    return 0;
}

static int output_graphics_getc(unsigned int prnr, BYTE *b)
{
    return 0;
}

static int output_graphics_flush(unsigned int prnr)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

void output_graphics_init(void)
{
    unsigned int i;

    for (i = 0; i < 3; i++) {
        output_gfx[i].line = NULL;
        output_gfx[i].line_pos = 0;
    }
}

void output_graphics_reset(void)
{
}

int output_graphics_init_resources(void)
{
    output_select_t output_select;

    output_select.output_name = "graphics";
    output_select.output_open = output_graphics_open;
    output_select.output_close = output_graphics_close;
    output_select.output_putc = output_graphics_putc;
    output_select.output_getc = output_graphics_getc;
    output_select.output_flush = output_graphics_flush;

    output_select_register(&output_select);

    return resources_register(resources);
}

