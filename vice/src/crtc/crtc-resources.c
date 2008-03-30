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

#include "crtc-resources.h"

#include "crtc.h"

/* ------------------------------------------------------------------------- */

/* CRTC resources.  */

crtc_resources_t crtc_resources;

static int set_video_cache_enabled(resource_value_t v)
{
    crtc_resources.video_cache_enabled = (int) v;
    return 0;
}

/* prototype for resources - moved to raster.c */
static int set_palette_file_name(resource_value_t v);

#ifdef NEED_2x
static int set_double_size_enabled(resource_value_t v)
{
    crtc_resources.double_size_enabled = (int) v;
#ifdef USE_VIDMODE_EXTENSION
    if(!fullscreen)
#endif
        video_resize();
    return 0;
}
#endif

static int set_double_scan_enabled(resource_value_t v)
{
    crtc_resources.double_scan_enabled = (int) v;
#if 0	/* def USE_VIDMODE_EXTENSION */
    if(!fullscreen)
#endif
        video_resize();
    return 0;
}

#if 0	/* def USE_VIDMODE_EXTENSION */

void fullscreen_forcerepaint(void);

#ifdef NEED_2x
static int set_fullscreen_double_size_enabled(resource_value_t v)
{
    crtc_resources.fullscreen_double_size_enabled = (int) v;
    fullscreen_forcerepaint();
    return 0;
}
#endif

static int set_fullscreen_double_scan_enabled(resource_value_t v)
{
    crtc_resources.fullscreen_double_scan_enabled = (int) v;
    fullscreen_forcerepaint();
    return 0;
}

#endif

static resource_t resources[] = {
    { "PaletteFile", RES_STRING, (resource_value_t) "default",
      (resource_value_t *) &crtc_resources.palette_file_name, set_palette_file_name },
#ifdef NEED_2x
    { "DoubleSize", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &crtc_resources.double_size_enabled, set_double_size_enabled },
#endif
#if defined NEED_2x || defined __MSDOS__
    { "DoubleScan", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &crtc_resources.double_scan_enabled, set_double_scan_enabled },
#endif

#if 0	/* def USE_VIDMODE_EXTENSION */
#ifdef NEED_2x
    { "FullscreenDoubleSize", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &crtc_resources.fullscreen_double_size_enabled,
      set_fullscreen_double_size_enabled },
#endif
    { "FullscreenDoubleScan", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &crtc_resources.fullscreen_double_scan_enabled,
      set_fullscreen_double_scan_enabled },
#endif

/* what´s the difference? */
#if 1	/* ndef __MSDOS__ */
    { "VideoCache", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
#else
    { "VideoCache", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &video_cache_enabled, set_video_cache_enabled },
#endif
    { NULL }
};

int crtc_init_resources(void)
{
    return resources_register(resources);
}


