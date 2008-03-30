/*
 * vic-cmdline-options.c - Command-line options for the VIC-I emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#include "archdep.h"
#include "cmdline.h"
#include "raster-cmdline-options.h"
#include "vic-cmdline-options.h"


/* VIC command-line options.  */
static cmdline_option_t cmdline_options[] =
{
    { "-palette", SET_RESOURCE, 1, NULL, NULL,
      "PaletteFile", NULL,
      "<name>", "Specify palette file name" },
    { NULL }
};

/* VIC double-size-specific command-line options.  */

static cmdline_option_t cmdline_options_2x[] =
{
#if ARCHDEP_VIC_DSIZE == 1
    { "-dsize", SET_RESOURCE, 0, NULL, NULL, "DoubleSize",
      (void *)1, NULL, "Enable double size" },
    { "+dsize", SET_RESOURCE, 0, NULL, NULL, "DoubleSize",
      (void *)0, NULL, "Disable double size" },
#endif
#if ARCHDEP_VIC_DSCAN == 1
    { "-dscan", SET_RESOURCE, 0, NULL, NULL, "DoubleScan",
      (void *)1, NULL, "Enable double scan" },
    { "+dscan", SET_RESOURCE, 0, NULL, NULL, "DoubleScan",
      (void *)0, NULL, "Disable double scan" },
#endif
    { NULL }
};

int vic_cmdline_options_init(void)
{
#if (ARCHDEP_VIC_DSIZE == 1) || (ARCHDEP_VIC_DSCAN == 1)
    if (cmdline_register_options(cmdline_options_2x) < 0)
        return -1;
#endif
    if (raster_cmdline_options_chip_init("VIC") < 0)
        return -1;

    return cmdline_register_options(cmdline_options);
}

