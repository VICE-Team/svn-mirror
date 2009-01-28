/*
 * video-resources.h - Resources for the video output
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

#ifndef VICE_VIDEO_RESOURCES_H
#define VICE_VIDEO_RESOURCES_H

#define VIDEO_RESOURCE_PAL_MODE_FAST	0
#define VIDEO_RESOURCE_PAL_MODE_TRUE	1
#define VIDEO_RESOURCE_PAL_MODE_NEW	2

struct video_resources_s
{
    int color_saturation;
    int color_contrast;
    int color_brightness;
    int color_gamma;
    int color_tint;
    int delayloop_emulation;	/* flag for fake PAL emu */
    int pal_scanlineshade;	/* amount of scanline shade */
    int pal_blur;		/* exact pal emu, luma blur */
    int pal_mode;
    int pal_oddlines_phase;	/* exact pal emu, oddlines UV phase offset */
    int pal_oddlines_offset;	/* exact pal emu, oddlines UV multiplier */
};

typedef struct video_resources_s video_resources_t;

extern video_resources_t video_resources;

#endif

