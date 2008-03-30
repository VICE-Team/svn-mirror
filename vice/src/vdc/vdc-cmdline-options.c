/*
 * vdc-cmdline-options.c - Command-line options for the MOS 8563 (VDC)
 * emulation.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Markus Brenner (markus@brenner.de)
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

#include "cmdline.h"

#include "vdc.h"

#include "vdc-cmdline-options.h"



/* VDC command-line options.  */

static cmdline_option_t cmdline_options[] =
{
  { "-VDC_vcache", SET_RESOURCE, 0, NULL, NULL,
    "VDC_VideoCache", (resource_value_t) 1,
    NULL, "Enable the video cache"},
  { "+VDC_vcache", SET_RESOURCE, 0, NULL, NULL,
    "VDC_VideoCache", (resource_value_t) 0,
    NULL, "Disable the video cache"},
  { "-VDC_palette", SET_RESOURCE, 1, NULL, NULL,
    "VDC_PaletteFile", NULL,
    "<name>", "Specify palette file name"},
  { NULL }
};



/* VDC double-size-specific command-line options.  */

#ifdef VDC_NEED_2X
static cmdline_option_t cmdline_options_2x[] =
{
  { "-VDC_dsize", SET_RESOURCE, 0, NULL, NULL,
    "VDC_DoubleSize", (resource_value_t) 1,
    NULL, "Enable double size"},
  { "+VDC_dsize", SET_RESOURCE, 0, NULL, NULL,
    "VDC_DoubleSize", (resource_value_t) 0,
    NULL, "Disable double size"},
  { "-VDC_dscan", SET_RESOURCE, 0, NULL, NULL,
    "VDC_DoubleScan", (resource_value_t) 1,
    NULL, "Enable double scan"},
  { "+VDC_dscan", SET_RESOURCE, 0, NULL, NULL,
    "VDC_DoubleScan", (resource_value_t) 0,
    NULL, "Disable double scan"},
  { NULL }
};
#endif



int 
vdc_cmdline_options_init (void)
{
#ifdef VDC_NEED_2X
  if (cmdline_register_options (cmdline_options_2x) < 0)
      return -1;
#endif

  return cmdline_register_options (cmdline_options);
}
