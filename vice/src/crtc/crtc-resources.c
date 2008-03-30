/*
 * crtc-resources.c - A line-based CRTC emulation (under construction).
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
 *
 * 16/24bpp support added by
 *  Steven Tieu (stieu@physics.ubc.ca)
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#include "utils.h"

#include "crtc.h"
#include "crtc-resources.h"



crtc_resources_t crtc_resources;



static int 
set_palette_file_name (resource_value_t v)
{
  string_set (&crtc_resources.palette_file_name, (char *) v);
  if (crtc.initialized)
    return crtc_load_palette (crtc_resources.palette_file_name);

  return 0;
}

static int 
set_video_cache_enabled (resource_value_t v)
{
  crtc_resources.video_cache_enabled = (int) v;
  return 0;
}

#ifdef CRTC_NEED_2X
static int 
set_double_size_enabled (resource_value_t v)
{
  crtc_resources.double_size_enabled = (int) v;
  crtc_resize ();
  return 0;
}
#endif

#if defined CRTC_NEED_2X || defined __MSDOS__
static int 
set_double_scan_enabled (resource_value_t v)
{
  crtc_resources.double_scan_enabled = (int) v;
  crtc_resize ();
  return 0;
}
#endif



static resource_t resources[] =
{
  { "PaletteFile", RES_STRING, (resource_value_t) "default",
    (resource_value_t *) &crtc_resources.palette_file_name,
    set_palette_file_name },
#ifdef CRTC_NEED_2X
  { "DoubleSize", RES_INTEGER, (resource_value_t) 0,
    (resource_value_t *) &crtc_resources.double_size_enabled,
    set_double_size_enabled },
#endif
#if defined CRTC_NEED_2X || defined __MSDOS__
  { "DoubleScan", RES_INTEGER, (resource_value_t) 0,
    (resource_value_t *) &crtc_resources.double_scan_enabled,
    set_double_scan_enabled },
#endif
  { "VideoCache", RES_INTEGER, (resource_value_t) 1,
    (resource_value_t *) &crtc_resources.video_cache_enabled,
    set_video_cache_enabled },
  { NULL }
};


int crtc_init_resources (void)
{
  return resources_register (resources);
}
