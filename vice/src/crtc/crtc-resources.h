/*
 * crtc-resources.h - A line-based CRTC emulation (under construction).
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

#ifndef _CRTC_RESOURCES_H
#define _CRTC_RESOURCES_H

#include "resources.h"

/* CRTC resources.  */
struct _crtc_resources
{
    /* Name of palette file.  */
    static char *palette_file_name;

    /* Flag: Do we use double size?  */
    static int double_size_enabled;

    /* Flag: Do we enable the video cache?  */
    static int video_cache_enabled;

    /* Flag: Do we copy lines in double size mode?  */
    static int double_scan_enabled;

#if 0	/* def USE_VIDMODE_EXTENSION */
    /* Flag: Fullscreenmode?  */
    static int fullscreen = 0; 

    /* Flag: Do we use double size?  */
    static int fullscreen_double_size_enabled;

    /* Flag: Do we copy lines in double size mode?  */
    static int fullscreen_double_scan_enabled;

    static int fullscreen_width;
    static int fullscreen_height;
#endif
};

typedef struct _crtc_resources crtc_resources_t;

extern crtc_resources_t crtc_resources;

int crtc_init_resources(void);

#endif

