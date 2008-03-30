/*
 * vdc-resources.c - Resources for the MOS 8563 (VDC) emulation.
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
 *
 */

#include "vice.h"

#include "resources.h"
#include "utils.h"

#include "vdc.h"

#include "vdc-resources.h"



#ifdef __MSDOS__
#define DEFAULT_VideoCache_VALUE 0
#else
#define DEFAULT_VideoCache_VALUE 1
#endif



vdc_resources_t vdc_resources;



static int 
set_video_cache_enabled (resource_value_t v)
{
  vdc_resources.video_cache_enabled = (int) v;
  if (vdc.initialized)
    raster_enable_cache (&vdc.raster,
			 vdc_resources.video_cache_enabled);

  return 0;
}

static int 
set_palette_file_name (resource_value_t v)
{
  string_set (&vdc_resources.palette_file_name, (char *) v);
  if (vdc.initialized)
    return vdc_load_palette (vdc_resources.palette_file_name);

  return 0;
}

static resource_t resources[] =
  {
    { "VDC_PaletteFile", RES_STRING, (resource_value_t) "vdc_deft",
      (resource_value_t *) &vdc_resources.palette_file_name,
      set_palette_file_name },
    { "VDC_VideoCache", RES_INTEGER, (resource_value_t) DEFAULT_VideoCache_VALUE,
      (resource_value_t *) &vdc_resources.video_cache_enabled,
      set_video_cache_enabled },
    { NULL }
  };



#ifdef VDC_NEED_2X

static int 
set_double_size_enabled (resource_value_t v)
{
  vdc_resources.double_size_enabled = (int) v;
  vdc_resize ();

  return 0;
}

static int 
set_double_scan_enabled (resource_value_t v)
{
  vdc_resources.double_scan_enabled = (int) v;
  if (vdc.initialized)
    raster_enable_double_scan (&vdc.raster,
			       vdc_resources.double_scan_enabled);

  return 0;
}


static resource_t resources_2x[] =
  {
    { "VDC_DoubleSize", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) &vdc_resources.double_size_enabled,
     set_double_size_enabled },
    { "VDC_DoubleScan", RES_INTEGER, (resource_value_t) 0,
     (resource_value_t *) &vdc_resources.double_scan_enabled,
     set_double_scan_enabled },
    { NULL }
  };

#endif /* VDC_NEED_2X */



int 
vdc_resources_init (void)
{
#ifdef VDC_NEED_2X
  if (resources_register (resources_2x) < 0)
    return -1;
#endif

  return resources_register (resources);
}
