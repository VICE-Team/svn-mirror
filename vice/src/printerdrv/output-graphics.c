/*
 * output-graphics.c - Output a graphics file.
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

#include <string.h>

#include "log.h"
#include "archdep.h"
#include "cmdline.h"
#include "output-select.h"
#include "output-graphics.h"
#include "resources.h"
#include "utils.h"
#include "types.h"
#include "drv-mps803.h"

static int linecnt[3] = { 0, 0, 0 };
static FILE *gfxf[3]  = { NULL, NULL, NULL };
static int npix[3]    = { 0, 0, 0 };

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
      NULL, set_ppb, (void *)0 },
    { NULL }
};

static cmdline_option_t cmdline_options[] =
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

static int output_graphics_open(unsigned int prnr)
{
    mps_t *mps=&drv_mps803[prnr];

    char *filename;

    npix[prnr] = ppb;

    switch (npix[prnr])
    {
    case 0:
        filename = "output.txt";
        break;
    case 3:
        filename = "output.pgm";
        break;
    default:
        filename = "output.pnm";
    }

    memset(mps, 0, sizeof(mps));

    gfxf[prnr] = fopen(filename, MODE_WRITE);

    if (!gfxf[prnr])
    {
        log_error(LOG_DEFAULT, "Error opening file '%s' for printing.",
                  filename);
        return -1;
    }

    switch (npix[prnr])
    {
    case 0:
        break;
    case 3:
        fprintf(gfxf[prnr], "P5\n%10d %10d\n255\n", npix[prnr]*480, 0);
        break;
    default:
        fprintf(gfxf[prnr], "P4\n%10d %10d\n", npix[prnr]*480, 0);
    }

    log_message(LOG_DEFAULT, "File '%s' opened for printing.",
                filename);

    return 0;
}

static void output_graphics_close(unsigned int prnr)
{
    mps_t *mps=&drv_mps803[prnr];

    if (npix[prnr])
    {
        fseek(gfxf[prnr], 14, SEEK_SET);

        fprintf(gfxf[prnr], "%10d", linecnt[prnr]);
    }

    fclose(gfxf[prnr]);

    log_message(LOG_DEFAULT, "Printer file closed. (Size: %dx%d)",
                (npix[prnr]?npix[prnr]:1)*480, linecnt[prnr]);

    linecnt[prnr] = 0;
    gfxf[prnr]    = 0;
}

static int output_graphics_putc(unsigned int prnr, BYTE b)
{
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

static void output_graphics_writeline(unsigned int prnr)
{
    mps_t *mps = &drv_mps803[prnr];
    FILE *f = gfxf[prnr];

    int x, y, i;

    switch (npix[prnr])
    {
    case 0:
        for (y=0; y<7; y++)
        {
            for (x=0; x<480; x++)
                fprintf(f, "%c", mps->line[x][y]?'*':' ');

            fprintf(f, "\n");
        }
        break;

    case 1:
        for (y=0; y<7; y++)
            for (x=0; x<MAX_COL; x+=8)
            {
                char byte = 0;
                for (i=0; i<8; i++)
                    byte |= (mps->line[x+i][y]?1:0)<<(7-i);

                fprintf(f, "%c", byte);
            }
        break;

    case 2:
        for (y=0; y<7; y++)
        {
            for (x=0; x<MAX_COL; x+=8)
            {
                int byte = 0;
                for (i=0; i<8; i++)
                {
                    const int bit = mps->line[x+i][y] ? 1 : 0;
                    byte |= bit << (2*(7-i));
                    byte |= bit << (2*(7-i)+1);
                }
                fprintf(f, "%c%c", byte>>8, byte);
            }
            for (x=0; x<MAX_COL; x+=8)
            {
                int byte = 0;
                for (i=0; i<8; i++)
                {
                    const int bit = mps->line[x+i][y] ? 1 : 0;
                    byte |= bit << (2*(7-i));
                    byte |= bit << (2*(7-i)+1);
                }
                fprintf(f, "%c%c", byte>>8, byte);
            }
        }
        break;

    case 3:
        for (y=0; y<7; y++)
        {
            for (x=0; x<MAX_COL; x+=8)
            {
                int byte = 0;
                for (i=0; i<8; i++)
                {
                    byte = mps->line[x+i][y]?1:0;
                    fprintf(f, "%c%c%c", byte?143:255, byte?47:255, byte?143:255);
                }
            }
            for (x=0; x<MAX_COL; x+=8)
            {
                int byte = 0;
                for (i=0; i<8; i++)
                {
                    byte = mps->line[x+i][y]?1:0;
                    fprintf(f, "%c%c%c", byte?47:255, byte?0:255, byte?47:255);
                }
            }
            for (x=0; x<MAX_COL; x+=8)
            {
                int byte = 0;
                for (i=0; i<8; i++)
                {
                    byte = mps->line[x+i][y]?1:0;
                    fprintf(f, "%c%c%c", byte?143:255, byte?47:255, byte?143:255);
                }
            }
        }
        break;
    }

    if (!is_mode(mps, MPS_BITMODE))
    {
        // bitmode:  9 rows/inch (7lines/row * 9rows/inch=63 lines/inch)
        // charmode: 6 rows/inch (7lines/row * 6rows/inch=42 lines/inch)
        //   --> 63lines/inch - 42lines/inch = 21lines/inch missing
        //   --> 21lines/inch / 9row/inch = 3lines/row missing
        switch (npix[prnr])
        {
        case 0:
            fprintf(f, "\n\n\n");
            linecnt[prnr] += 3;
            break;

        case 3:
            for (x=0; x<npix[prnr]*MAX_COL*(npix[prnr]*3); x++)
                fprintf(f, "%c", 255);

            linecnt[prnr] += 3*npix[prnr];
            break;

        default:
            for (x=0; x<npix[prnr]*MAX_COL*(npix[prnr]*3)/8; x++)
                fprintf(f, "%c", 0);

            linecnt[prnr] += 3*npix[prnr];
        }
    }
    linecnt[prnr] += 7*npix[prnr];

    mps->pos=0;
}

/* ------------------------------------------------------------------------- */

void output_graphics_init(void)
{
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
    output_select.output_writeline = output_graphics_writeline;

    output_select_register(&output_select);

    return resources_register(resources);
}

