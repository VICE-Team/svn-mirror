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

#include <stdio.h>
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "output-select.h"
#include "output-graphics.h"
#include "resources.h"
#include "utils.h"
#include "types.h"


static resource_t resources[] = {
    { NULL }
};

static cmdline_option_t cmdline_options[] =
{
    { NULL }
};

int output_graphics_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static int output_graphics_open(unsigned int prnr)
{
    return 0;
}

static void output_graphics_close(unsigned int prnr)
{
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

    output_select_register(&output_select);

    return resources_register(resources);
}

