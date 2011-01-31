/*
 * crtc-cmdline-options.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "crtc-cmdline-options.h"
#include "crtctypes.h"
#include "cmdline.h"
#include "raster-cmdline-options.h"
#include "resources.h"
#include "translate.h"

/* CRTC command-line options.  */
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
      "<0-4000>", NULL },
    { "-tint", SET_RESOURCE, 1,
      NULL, NULL, "ColorTint", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_TINT,
      "<0-2000>", NULL },
    { "-oddlinesphase", SET_RESOURCE, 1,
      NULL, NULL, "PALOddLinePhase", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_ODDLINES_PHASE,
      "<0-2000>", NULL },
    { "-oddlinesoffset", SET_RESOURCE, 1,
      NULL, NULL, "PALOddLineOffset", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_SET_ODDLINES_OFFSET,
      "<0-2000>", NULL },
    { "-crtblur", SET_RESOURCE, 1,
      NULL, NULL, "PALBlur", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-1000>", T_("Amount of horizontal blur for the CRT emulation. [500]") },
    { "-crtscanlineshade", SET_RESOURCE, 1,
      NULL, NULL, "PALScanLineShade", NULL,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      "<0-1000>", T_("Amount of scan line shading for the CRT emulation [667]") },
    { NULL }
};

int crtc_cmdline_options_init(void)
{
    if (raster_cmdline_options_chip_init("Crtc", crtc.video_chip_cap) < 0)
        return -1;

    return cmdline_register_options(cmdline_options);
}

