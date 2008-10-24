/*
 * ted-cmdline-options.c - Command-line options for the TED emulation.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
 * */

#include "vice.h"

#include <stdio.h>

#include "cmdline.h"
#include "raster-cmdline-options.h"
#include "ted-cmdline-options.h"
#include "tedtypes.h"
#include "translate.h"

/* TED command-line options.  */
static const cmdline_option_t cmdline_options[] =
{
    { "-saturation", SET_RESOURCE, 1,
      NULL, NULL, "ColorSaturation", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_SATURATION,
      "<0-2000>", NULL },
    { "-contrast", SET_RESOURCE, 1,
      NULL, NULL, "ColorContrast", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_CONTRAST,
      "<0-2000>", NULL },
    { "-brightness", SET_RESOURCE, 1,
      NULL, NULL, "ColorBrightness", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_BRIGHTNESS,
      "<0-2000>", NULL },
    { "-gamma", SET_RESOURCE, 1,
      NULL, NULL, "ColorGamma", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_GAMMA,
      "<0-2000>", NULL },
    { NULL }
};

int ted_cmdline_options_init(void)
{
    if (raster_cmdline_options_chip_init("TED", ted.video_chip_cap) < 0)
        return -1;

    return cmdline_register_options(cmdline_options);
}
