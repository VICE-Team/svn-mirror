/*
 * crtc-cmdline-options.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#include "cmdline.h"
#include "crtc-cmdline-options.h"

/* CRTC command-line options.  */

static cmdline_option_t cmdline_options[] =
  {
    { "-crtcvcache", SET_RESOURCE, 0, NULL, NULL,
      "CrtcVideoCache", (resource_value_t) 1,
      NULL, "Enable the video cache" },
    { "+crtcvcache", SET_RESOURCE, 0, NULL, NULL,
      "CrtcVideoCache", (resource_value_t) 0,
      NULL, "Disable the video cache" },
    { "-crtcpalette", SET_RESOURCE, 1, NULL, NULL,
      "CrtcPaletteFile", NULL,
      "<name>", "Specify palette file name" },
#ifdef CRTC_NEED_2X
    { "-crtcdsize", SET_RESOURCE, 0, NULL, NULL,
      "CrtcDoubleSize", (resource_value_t) 1,
      NULL, "Enable double size" },
    { "+crtcdsize", SET_RESOURCE, 0, NULL, NULL,
      "CrtcDoubleSize", (resource_value_t) 0,
      NULL, "Disable double size" },
    { "-crtcdscan", SET_RESOURCE, 0, NULL, NULL,
      "CrtcDoubleScan", (resource_value_t) 1,
      NULL, "Enable double scan" },
    { "+crtcdscan", SET_RESOURCE, 0, NULL, NULL,
      "CrtcDoubleScan", (resource_value_t) 0,
      NULL, "Disable double scan" },
#endif
    { NULL }
  };


int crtc_init_cmdline_options (void)
{
  return cmdline_register_options (cmdline_options);
}
