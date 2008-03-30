/*
 * video-color.h - Video implementation of YUV, YCbCr and RGB colors
 *
 * Written by
 *  John Selck <graham@cruise.de>
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

#ifndef _VIDEO_COLOR_H
#define _VIDEO_COLOR_H

#include "palette.h"
#include "raster/raster.h"

/* commodore VIC/VIC-II/TED related color/palette types */

typedef struct video_cbm_color_s {
	float luminance;	/* luminance                      */
	float angle;		/* angle on color wheel           */
	int direction;		/* +1 (pos), -1 (neg) or 0 (grey) */
	char *name;			/* name of this color             */
} video_cbm_color_t;

typedef struct video_cbm_palette_s {
    unsigned int num_entries;		/* number of colors in palette */
    video_cbm_color_t *entries;		/* array of colors             */
	float saturation;				/* base saturation of all colors except the grey tones */
	float phase;					/* color phase (will be added to all color angles) */
} video_cbm_palette_t;

extern void video_set_palette(video_cbm_palette_t *palette);
extern void video_set_raster(raster_t *raster);
extern int video_update_palette(void);

#endif

