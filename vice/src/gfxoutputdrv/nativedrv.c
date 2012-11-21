/*
 * nativedrv.c - Common code for native screenshots.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <string.h>
#include <stdlib.h>

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "gfxoutput.h"
#include "nativedrv.h"
#include "palette.h"
#include "resources.h"
#include "screenshot.h"
#include "translate.h"
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vsync.h"

void native_smooth_scroll_borderize_colormap(native_data_t *source, BYTE bordercolor, BYTE xcover, BYTE ycover)
{
    int i, j, k;
    int xstart = 0;
    int xsize;
    int xendamount = 0;
    int ystart = 0;
    int ysize;
    int yendamount = 0;

    if (xcover == 255) {
        xstart = 0;
        xsize = source->xsize;
        xendamount = 0;
    } else {
        xstart = 7 - xcover;
        xsize = source->xsize - 16;
        xendamount = 16 - xstart;
    }

    if (ycover == 255) {
        ystart = 0;
        ysize = source->ysize;
        yendamount = 0;
    } else {
        ystart = 7 - ycover;
        ysize = source->ysize - 8;
        yendamount = 8 - ystart;
    }

    k = 0;

    /* render top border if needed */
    for (i = 0; i < ystart; i++) {
        for (j = 0; j < source->xsize; j++) {
            source->colormap[k++] = bordercolor;
        }
    }

    for (i = 0; i < ysize; i++) {

        /* render left border if needed */
        for (j = 0; j < xstart; j++) {
            source->colormap[k++] = bordercolor;
        }

        /* skip screen data */
        k += xsize;

        /* render right border if needed */
        for (j = 0; j < xendamount; j++) {
            source->colormap[k++] = bordercolor;
        }
    }

    /* render bottom border if needed */
    for (i = 0; i < yendamount; i++) {
        for (j = 0; j < source->xsize; j++) {
            source->colormap[k++] = bordercolor;
        }
    }
}

native_data_t *native_borderize_colormap(native_data_t *source, BYTE bordercolor, int xsize, int ysize)
{
    int i, j, k, l;
    int xstart = 0;
    int xendamount = 0;
    int ystart = 0;
    int yendamount = 0;
    native_data_t *dest = lib_malloc(sizeof(native_data_t));

    dest->filename = source->filename;

    if (source->xsize < xsize) {
        dest->xsize = xsize;
        xstart = ((xsize - source->xsize) / 16) * 8;
        xendamount = xsize - xstart - source->xsize;
    } else {
        dest->xsize = source->xsize;
    }

    if (source->ysize < ysize) {
        dest->ysize = ysize;
        ystart = ((ysize - source->ysize) / 16) * 8;
        yendamount = ysize - ystart - source->ysize;
    } else {
        dest->ysize = source->ysize;
    }

    dest->colormap = lib_malloc(dest->xsize * dest->ysize);

    k = 0;
    l = 0;

    /* render top border if needed */
    for (i = 0; i < ystart; i++) {
        for (j = 0; j < dest->xsize; j++) {
            dest->colormap[k++] = bordercolor;
        }
    }

    for (i = 0; i < source->ysize; i++) {

        /* render left border if needed */
        for (j = 0; j < xstart; j++) {
            dest->colormap[k++] = bordercolor;
        }

        /* copy screen data */
        for (j = 0; j < source->xsize; j++) {
            dest->colormap[k++] = source->colormap[l++];
        }

        /* render right border if needed */
        for (j = 0; j < xendamount; j++) {
            dest->colormap[k++] = bordercolor;
        }
    }

    /* render bottom border if needed */
    for (i = 0; i < yendamount; i++) {
        for (j = 0; j < dest->xsize; j++) {
            dest->colormap[k++] = bordercolor;
        }
    }

    lib_free(source->colormap);
    lib_free(source);

    return dest;
}

native_data_t *native_crop_and_borderize_colormap(native_data_t *source, BYTE bordercolor, int xsize, int ysize, int oversize_handling)
{
    int startx;
    int starty;
    int skipxstart = 0;
    int skipxend = 0;
    int skipystart = 0;
    int i, j, k, l;
    native_data_t *dest = lib_malloc(sizeof(native_data_t));

    dest->filename = source->filename;

    startx = (xsize - source->xsize) / 2;
    starty = (ysize - source->ysize) / 2;

    if (source->xsize > xsize) {
        dest->xsize = xsize;
    } else {
        dest->xsize = source->xsize;
    }

    if (source->ysize > ysize) {
        dest->ysize = ysize;
    } else {
        dest->ysize = source->ysize;
    }

    dest->colormap = lib_malloc(dest->xsize * dest->ysize);

    if (startx < 0) {
        switch (oversize_handling) {
            default:
            case NATIVE_SS_OVERSIZE_CROP_LEFT_TOP:
            case NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER:
            case NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM:
                skipxend = source->xsize - xsize;
                break;
            case NATIVE_SS_OVERSIZE_CROP_CENTER_TOP:
            case NATIVE_SS_OVERSIZE_CROP_CENTER:
            case NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM:
                skipxstart = 0 - startx;
                skipxend = source->xsize - xsize - skipxstart;
                break;
            case NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP:
            case NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER:
            case NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM:
                skipxstart = source->xsize - xsize;
                break;
        }
        startx = 0;
    } else {
        startx = ((xsize - source->xsize) / 16) * 8;
    }

    if (starty < 0) {
        switch (oversize_handling) {
            default:
            case NATIVE_SS_OVERSIZE_CROP_LEFT_TOP:
            case NATIVE_SS_OVERSIZE_CROP_CENTER_TOP:
            case NATIVE_SS_OVERSIZE_CROP_RIGHT_TOP:
                break;
            case NATIVE_SS_OVERSIZE_CROP_LEFT_CENTER:
            case NATIVE_SS_OVERSIZE_CROP_CENTER:
            case NATIVE_SS_OVERSIZE_CROP_RIGHT_CENTER:
                skipystart = 0 - starty;
                break;
            case NATIVE_SS_OVERSIZE_CROP_LEFT_BOTTOM:
            case NATIVE_SS_OVERSIZE_CROP_CENTER_BOTTOM:
            case NATIVE_SS_OVERSIZE_CROP_RIGHT_BOTTOM:
                skipystart = source->ysize - ysize;
                break;
        }
        starty = 0;
    } else {
        starty = ((ysize - source->ysize) / 16) * 8;
    }

    k = 0;
    l = 0;

    /* skip top lines for cropping if needed */
    for (i = 0; i < skipystart; i++) {
        for (j = 0; j < source->ysize; j++) {
            l++;
        }
    }

    /* render top border if needed */
    for (i = 0; i < starty; i++) {
        for (j = 0; j < xsize; j++) {
            dest->colormap[k++] = bordercolor;
        }
    }

    for (i = starty; i < starty + dest->ysize; i++) {
        /* skip right part for cropping if needed */
        for (j = 0; j < skipxstart; j++) {
            l++;
        }

        /* render left border if needed */
        for (j = 0; j < startx; j++) {
            dest->colormap[k++] = bordercolor;
        }

        /* copy main body */
        for (j = startx; j < startx + dest->xsize; j++) {
            dest->colormap[k++] = source->colormap[l++];
        }

        /* render right border if needed */
        for (j = startx + dest->xsize; j < xsize; j++) {
            dest->colormap[k++] = bordercolor;
        }

        /* skip right part for cropping if needed */
        for (j = 0; j < skipxend; j++) {
            l++;
        }
    }

    /* render bottom border if needed */
    for (i = starty + dest->ysize; i < ysize; i++) {
        for (j = 0; j < xsize; j++) {
            dest->colormap[k++] = bordercolor;
        }
    }

    lib_free(source->colormap);
    lib_free(source);

    return dest;
}

native_data_t *native_scale_colormap(native_data_t *source, int xsize, int ysize)
{
    native_data_t *dest = lib_malloc(sizeof(native_data_t));
    int i, j;
    int xmult, ymult;

    dest->filename = source->filename;

    dest->xsize = xsize;
    dest->ysize = ysize;

    dest->colormap = lib_malloc(xsize * ysize);

    xmult = (source->xsize << 8) / xsize;
    ymult = (source->ysize << 8) / ysize;

    for (i = 0; i < ysize; i++) {
        for (j = 0; j < xsize; j++) {
            dest->colormap[(i * xsize) + j] = source->colormap[(((i * ymult) >> 8) * source->xsize) + ((j * xmult) >> 8)];
        }
    }

    lib_free(source->colormap);
    lib_free(source);

    return dest;
}

native_data_t *native_resize_colormap(native_data_t *source, int xsize, int ysize, BYTE bordercolor, int oversize_handling, int undersize_handling)
{
    native_data_t *data = source;
    int mc_data_present = source->mc_data_present;

    if (data->xsize > xsize) {
        if (oversize_handling == NATIVE_SS_OVERSIZE_SCALE) {
            data = native_scale_colormap(data, xsize, data->ysize);
        } else {
            data = native_crop_and_borderize_colormap(data, bordercolor, xsize, data->ysize, oversize_handling);
        }
    }

    if (data->xsize < xsize) {
        if (undersize_handling == NATIVE_SS_UNDERSIZE_SCALE) {
            data = native_scale_colormap(data, xsize, data->ysize);
        } else {
            data = native_borderize_colormap(data, bordercolor, xsize, data->ysize);
        }
    }

    if (data->ysize > ysize) {
        if (oversize_handling == NATIVE_SS_OVERSIZE_SCALE) {
            data = native_scale_colormap(data, xsize, ysize);
        } else {
            data = native_crop_and_borderize_colormap(data, bordercolor, xsize, ysize, oversize_handling);
        }
    }

    if (data->ysize < ysize) {
        if (undersize_handling == NATIVE_SS_UNDERSIZE_SCALE) {
            data = native_scale_colormap(data, xsize, ysize);
        } else {
            data = native_borderize_colormap(data, bordercolor, xsize, ysize);
        }
    }

    data->mc_data_present = mc_data_present;

    return data;
}

native_color_sort_t *native_sort_colors_colormap(native_data_t *source, int color_amount)
{
    int i, j;
    BYTE color;
    int highest;
    int amount;
    int highestindex = 0;
    native_color_sort_t *colors = lib_malloc(sizeof(native_color_sort_t) * color_amount);

    for (i = 0; i < color_amount; i++) {
        colors[i].color = i;
        colors[i].amount = 0;
    }

    /* count the colors used */
    for (i = 0; i < (source->xsize * source->ysize); i++) {
        colors[source->colormap[i]].amount++;
    }

    /* sort colors from highest to lowest */
    for (i = 0; i < color_amount; i++) {
        highest = 0;
        for (j = i; j < color_amount; j++) {
            if (colors[j].amount >= highest) {
                highest = colors[j].amount;
                highestindex = j;
            }
        }
        color = colors[i].color;
        amount = colors[i].amount;
        colors[i].color = colors[highestindex].color;
        colors[i].amount = colors[highestindex].amount;
        colors[highestindex].color = color;
        colors[highestindex].amount = amount;
    }
    return colors;
}

static BYTE vicii_color_bw_translate[16] = {
    0,    /* vicii black       (0) -> vicii black (0) */
    1,    /* vicii white       (1) -> vicii white (1) */
    0,    /* vicii red         (2) -> vicii black (0) */
    1,    /* vicii cyan        (3) -> vicii white (1) */
    1,    /* vicii purple      (4) -> vicii white (1) */
    0,    /* vicii green       (5) -> vicii black (0) */
    0,    /* vicii blue        (6) -> vicii black (0) */
    1,    /* vicii yellow      (7) -> vicii white (1) */
    0,    /* vicii orange      (8) -> vicii black (0) */
    0,    /* vicii brown       (9) -> vicii black (0) */
    1,    /* vicii light red   (A) -> vicii white (1) */
    0,    /* vicii dark gray   (B) -> vicii black (0) */
    1,    /* vicii medium gray (C) -> vicii white (1) */
    1,    /* vicii light green (D) -> vicii white (1) */
    1,    /* vicii light blue  (E) -> vicii white (1) */
    1     /* vicii light gray  (F) -> vicii white (1) */
};

static inline BYTE vicii_color_to_bw(BYTE color)
{
    return vicii_color_bw_translate[color];
}

void vicii_color_to_vicii_bw_colormap(native_data_t *source)
{
    int i, j;

    for (i = 0; i < 200; i++) {
        for (j = 0; j < 320; j++) {
            source->colormap[(i * 320) + j] = vicii_color_to_bw(source->colormap[(i * 320) + j]);
        }
    }
}

static BYTE vicii_color_gray_translate[16] = {
    0x0,    /* vicii black       (0) -> vicii black       (0) */
    0xF,    /* vicii white       (1) -> vicii light gray  (F) */
    0xB,    /* vicii red         (2) -> vicii dark gray   (B) */
    0xC,    /* vicii cyan        (3) -> vicii medium gray (C) */
    0xC,    /* vicii purple      (4) -> vicii medium gray (C) */
    0xB,    /* vicii green       (5) -> vicii dark gray   (B) */
    0xB,    /* vicii blue        (6) -> vicii dark gray   (B) */
    0xC,    /* vicii yellow      (7) -> vicii medium gray (C) */
    0xC,    /* vicii orange      (8) -> vicii medium gray (C) */
    0xB,    /* vicii brown       (9) -> vicii dark gray   (B) */
    0xC,    /* vicii light red   (A) -> vicii medium gray (C) */
    0xB,    /* vicii dark gray   (B) -> vicii dark gray   (B) */
    0xC,    /* vicii medium gray (C) -> vicii medium gray (C) */
    0xF,    /* vicii light green (D) -> vicii light gray  (F) */
    0xC,    /* vicii light blue  (E) -> vicii medium gray (C) */
    0xF     /* vicii light gray  (F) -> vicii light gray  (F) */
};

static inline BYTE vicii_color_to_gray(BYTE color)
{
    return vicii_color_gray_translate[color];
}

void vicii_color_to_vicii_gray_colormap(native_data_t *source)
{
    int i, j;

    for (i = 0; i < 200; i++) {
        for (j = 0; j < 320; j++) {
            source->colormap[(i * 320) + j] = vicii_color_to_gray(source->colormap[(i * 320) + j]);
        }
    }
}

static BYTE vicii_closest_color[16][16] = {
    /* vicii black (0) */
    { 0, 9, 11, 2, 6, 8, 5, 12, 4, 10, 14, 3, 13, 15, 7, 1 },

    /* vicii white (1) */
    { 1, 15, 13, 7, 3, 10, 14, 12, 4, 5, 11, 8, 6, 2, 9, 0 },

    /* vicii red (2) */
    { 2, 8, 9, 11, 0, 10, 12, 5, 4, 6, 7, 14, 15, 3, 13, 1 },

    /* vicii cyan (3) */
    { 3, 13, 14, 15, 12, 10, 5, 7, 4, 11, 1, 6, 8, 9, 2, 0 },

    /* vicii purple (4) */
    { 4, 10, 12, 11, 15, 14, 6, 8, 2, 3, 13, 9, 7, 5, 1, 0 },

    /* vicii green (5) */
    { 5, 11, 12, 8, 9, 3, 10, 2, 13, 7, 14, 15, 0, 4, 6, 1 },

    /* vicii blue (6) */
    { 6, 11, 9, 0, 4, 12, 14, 2, 8, 10, 3, 5, 13, 15, 7, 1 },

    /* vicii yellow (7) */
    { 7, 13, 15, 10, 3, 12, 1, 5, 8, 4, 14, 11, 2, 9, 6, 0 },

    /* vicii orange (8) */
    { 8, 2, 9, 11, 10, 5, 12, 4, 0, 7, 6, 15, 3, 14, 13, 1 },

    /* vicii brown (9) */
    { 9, 11, 2, 0, 8, 6, 5, 12, 4, 10, 14, 3, 15, 13, 7, 1 },

    /* vicii light red (10) */
    { 10, 12, 4, 15, 7, 8, 3, 11, 13, 14, 2, 5, 9, 1, 6, 0 },

    /* vicii dark gray (11) */
    { 11, 9, 12, 6, 2, 8, 5, 0, 4, 10, 14, 3, 15, 13, 7, 1 },

    /* vicii medium gray (12) */
    { 12, 10, 4, 3, 14, 11, 15, 5, 13, 8, 9, 6, 7, 2, 1, 0 },

    /* vicii light green (13) */
    { 13, 3, 15, 7, 12, 15, 1, 10, 5, 4, 11, 8, 9, 2, 6, 0 },

    /* vicii light blue (14) */
    { 14, 3, 12, 11, 4, 13, 6, 11, 10, 5, 9, 1, 7, 8, 2, 0 },

    /* vicii light gray (15) */
    { 15, 13, 3, 12, 14, 10, 7, 1, 4, 5, 11, 8, 6, 9, 2, 0 }
};

static inline BYTE vicii_color_to_nearest_color(BYTE color, native_color_sort_t *altcolors)
{
    int i, j;

    for (i = 0; i < 16; i++) {
        for (j = 0; altcolors[j].color != 255; j++) {
            if (vicii_closest_color[color][i] == altcolors[j].color) {
                return vicii_closest_color[color][i];
            }
        }
    }
    return 0;
}

void vicii_color_to_nearest_vicii_color_colormap(native_data_t *source, native_color_sort_t *colors)
{
    int i, j;

    for (i = 0; i < source->ysize; i++) {
        for (j = 0; j < source->xsize; j++) {
            source->colormap[(i * source->xsize) + j] = vicii_color_to_nearest_color(source->colormap[(i * source->xsize) +  j], colors);
        }
    }
}

/* ------------------------------------------------------------------------ */

native_data_t *native_vicii_text_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;
    data->colormap = lib_malloc(320 * 200);

    bgcolor = regs[0x21] & 0xf;

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            fgcolor = screenshot->color_ram_ptr[(i * 40) + j] & 0xf;
            for (k = 0; k < 8; k++) {
                bitmap = screenshot->chargen_ptr[(screenshot->screen_ptr[(i * 40) + j] * 8) + k];
                for (l = 0; l < 8; l++) {
                    if (bitmap & (1 << (7 - l))) {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = fgcolor;
                    } else {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = bgcolor;
                    }
                }
            }
        }
    }
    if (((regs[0x16] & 8) == 0) || ((regs[0x11] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, (BYTE)(regs[0x20] & 0xf), (BYTE)((regs[0x16] & 8) ? 255 : regs[0x16] & 7), (BYTE)((regs[0x11]  & 8) ? 255 : regs[0x11] & 7));
    }
    return data;
}

native_data_t *native_vicii_extended_background_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;
    data->colormap = lib_malloc(320 * 200);

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            fgcolor = screenshot->color_ram_ptr[(i * 40) + j] & 0xf;
            bgcolor = regs[0x21 + ((screenshot->screen_ptr[(i * 40) + j] & 0xc0) >> 6)] & 0xf;
            for (k = 0; k < 8; k++) {
                bitmap = screenshot->chargen_ptr[((screenshot->screen_ptr[(i * 40) + j] & 0x3f) * 8) + k];
                for (l = 0; l < 8; l++) {
                    if (bitmap & (1 << (7 - l))) {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = fgcolor;
                    } else {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = bgcolor;
                    }
                }
            }
        }
    }
    if (((regs[0x16] & 8) == 0) || ((regs[0x11] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, (BYTE)(regs[0x20] & 0xf), (BYTE)((regs[0x16] & 8) ? 255 : regs[0x16] & 7), (BYTE)((regs[0x11]  & 8) ? 255 : regs[0x11] & 7));
    }
    return data;
}

native_data_t *native_vicii_multicolor_text_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE color0;
    BYTE color1;
    BYTE color2;
    BYTE color3;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;
    data->colormap = lib_malloc(320 * 200);

    color0 = regs[0x21] & 0xf;
    color1 = regs[0x22] & 0xf;
    color2 = regs[0x23] & 0xf;

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            color3 = screenshot->color_ram_ptr[(i * 40) + j] & 0xf;
            for (k = 0; k < 8; k++) {
                bitmap = screenshot->chargen_ptr[(screenshot->screen_ptr[(i * 40) + j] * 8) + k];
                if (color3 & 8) {
                    for (l = 0; l < 4; l++) {
                        data->mc_data_present = 1;
                        switch ((bitmap & (3 << ((3 - l) * 2))) >> ((3 - l) * 2)) {
                            case 0:
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color0;
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color0;
                                break;
                            case 1:
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color1;
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color1;
                                break;
                            case 2:
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color2;
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color2;
                                break;
                            case 3:
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color3 & 7;
                                data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color3 & 7;
                                break;
                        }
                    }
                } else {
                    for (l = 0; l < 8; l++) {
                        if (bitmap & (1 << (7 - l))) {
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = color3;
                        } else {
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = color0;
                        }
                    }
                }
            }
        }
    }
    if (((regs[0x16] & 8) == 0) || ((regs[0x11] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, (BYTE)(regs[0x20] & 0xf), (BYTE)((regs[0x16] & 8) ? 255 : regs[0x16] & 7), (BYTE)((regs[0x11]  & 8) ? 255 : regs[0x11] & 7));
    }
    return data;
}

native_data_t *native_vicii_hires_bitmap_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;
    data->colormap = lib_malloc(320 * 200);

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            fgcolor = (screenshot->screen_ptr[(i * 40) + j] & 0xf0) >> 4;
            bgcolor = screenshot->screen_ptr[(i * 40) + j] & 0xf;
            for (k = 0; k < 8; k++) {
                if (((i * 40 * 8) + (j * 8) + k) < 4096) {
                    bitmap = screenshot->bitmap_low_ptr[(i * 40 * 8) + (j * 8) + k];
                } else {
                    bitmap = screenshot->bitmap_high_ptr[((i * 40 * 8) + (j * 8) + k) - 4096];
                }
                for (l = 0; l < 8; l++) {
                    if (bitmap & (1 << (7 - l))) {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = fgcolor;
                    } else {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = bgcolor;
                    }
                }
            }
        }
    }
    if (((regs[0x16] & 8) == 0) || ((regs[0x11] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, (BYTE)(regs[0x20] & 0xf), (BYTE)((regs[0x16] & 8) ? 255 : regs[0x16] & 7), (BYTE)((regs[0x11]  & 8) ? 255 : regs[0x11] & 7));
    }
    return data;
}

native_data_t *native_vicii_multicolor_bitmap_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE color0;
    BYTE color1;
    BYTE color2;
    BYTE color3;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 1;

    data->xsize = 320;
    data->ysize = 200;
    data->colormap = lib_malloc(320 * 200);

    color0 = regs[0x21] & 0xf;
    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            color1 = (screenshot->screen_ptr[(i * 40) + j] & 0xf0) >> 4;
            color2 = screenshot->screen_ptr[(i * 40) + j] & 0xf;
            color3 = screenshot->color_ram_ptr[(i * 40) + j] & 0xf;
            for (k = 0; k < 8; k++) {
                if (((i * 40 * 8) + (j * 8) + k) < 4096) {
                    bitmap = screenshot->bitmap_low_ptr[(i * 40 * 8) + (j * 8) + k];
                } else {
                    bitmap = screenshot->bitmap_high_ptr[((i * 40 * 8) + (j * 8) + k) - 4096];
                }
                for (l = 0; l < 4; l++) {
                    switch ((bitmap & (3 << ((3 - l) * 2))) >> ((3 - l) * 2)) {
                        case 0:
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color0;
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color0;
                            break;
                        case 1:
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color1;
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color1;
                            break;
                        case 2:
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color2;
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color2;
                            break;
                        case 3:
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2)] = color3;
                            data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + (l * 2) + 1] = color3;
                            break;
                    }
                }
            }
        }
    }
    if (((regs[0x16] & 8) == 0) || ((regs[0x11] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, (BYTE)(regs[0x20] & 0xf), (BYTE)((regs[0x16] & 8) ? 255 : regs[0x16] & 7), (BYTE)((regs[0x11]  & 8) ? 255 : regs[0x11] & 7));
    }
    return data;
}

/* ------------------------------------------------------------------------ */

static BYTE ted_vicii_translate[16] = {
    0x0,    /* ted black        (0) -> vicii black       (0) */
    0x1,    /* ted white        (1) -> vicii white       (1) */
    0x2,    /* ted red          (2) -> vicii red         (2) */
    0x3,    /* ted cyan         (3) -> vicii cyan        (3) */
    0x4,    /* ted purple       (4) -> vicii purple      (4) */
    0x5,    /* ted green        (5) -> vicii green       (5) */
    0x6,    /* ted blue         (6) -> vicii blue        (6) */
    0x7,    /* ted yellow       (7) -> vicii yellow      (7) */
    0x8,    /* ted orange       (8) -> vicii orange      (8) */
    0x9,    /* ted brown        (9) -> vicii brown       (9) */
    0xD,    /* ted yellow-green (A) -> vicii light green (D) */
    0xA,    /* ted pink         (B) -> vicii light red   (A) */
    0xE,    /* ted blue-green   (C) -> vicii light blue  (E) */
    0xE,    /* ted light blue   (D) -> vicii light blue  (E) */
    0x6,    /* ted dark blue    (E) -> vicii blue        (6) */
    0xD     /* ted light green  (F) -> vicii light green (D) */
};

static BYTE ted_to_vicii_color(BYTE color)
{
    return ted_vicii_translate[color];
}

static BYTE ted_lum_vicii_translate[16 * 8] = {
    0x0,    /* ted black L0        (0) -> vicii black     (0) */
    0x9,    /* ted white L0        (1) -> vicii brown     (9) */
    0x2,    /* ted red L0          (2) -> vicii red       (2) */
    0xB,    /* ted cyan L0         (3) -> vicii dark gray (B) */
    0x6,    /* ted purple L0       (4) -> vicii blue      (6) */
    0x0,    /* ted green L0        (5) -> vicii black     (0) */
    0x6,    /* ted blue L0         (6) -> vicii blue      (6) */
    0x9,    /* ted yellow L0       (7) -> vicii brown     (9) */
    0x9,    /* ted orange L0       (8) -> vicii brown     (9) */
    0x9,    /* ted brown L0        (9) -> vicii brown     (9) */
    0x9,    /* ted yellow-green L0 (A) -> vicii brown     (9) */
    0x9,    /* ted pink L0         (B) -> vicii brown     (9) */
    0x0,    /* ted blue-green L0   (C) -> vicii black     (0) */
    0x6,    /* ted light blue L0   (D) -> vicii blue      (6) */
    0x6,    /* ted dark blue L0    (E) -> vicii blue      (6) */
    0x9,    /* ted light green L0  (F) -> vicii brown     (9) */

    0x0,    /* ted black L1        (0) -> vicii black     (0) */
    0xB,    /* ted white L1        (1) -> vicii dark gray (B) */
    0x2,    /* ted red L1          (2) -> vicii red       (2) */
    0xB,    /* ted cyan L1         (3) -> vicii dark gray (B) */
    0x6,    /* ted purple L1       (4) -> vicii blue      (6) */
    0x9,    /* ted green L1        (5) -> vicii brown     (9) */
    0x6,    /* ted blue L1         (6) -> vicii blue      (6) */
    0x9,    /* ted yellow L1       (7) -> vicii brown     (9) */
    0x2,    /* ted orange L1       (8) -> vicii red       (2) */
    0x9,    /* ted brown L1        (9) -> vicii brown     (9) */
    0x9,    /* ted yellow-green L1 (A) -> vicii brown     (9) */
    0xB,    /* ted pink L1         (B) -> vicii dark gray (B) */
    0xB,    /* ted blue-green L1   (C) -> vicii dark gray (B) */
    0x6,    /* ted light blue L1   (D) -> vicii blue      (6) */
    0x6,    /* ted dark blue L1    (E) -> vicii blue      (6) */
    0x9,    /* ted light green L1  (F) -> vicii brown     (9) */

    0x0,    /* ted black L2        (0) -> vicii black     (0) */
    0xB,    /* ted white L2        (1) -> vicii dark gray (B) */
    0x2,    /* ted red L2          (2) -> vicii red       (2) */
    0xB,    /* ted cyan L2         (3) -> vicii dark gray (B) */
    0x4,    /* ted purple L2       (4) -> vicii purple    (4) */
    0x9,    /* ted green L2        (5) -> vicii brown     (9) */
    0x6,    /* ted blue L2         (6) -> vicii blue      (6) */
    0x9,    /* ted yellow L2       (7) -> vicii brown     (9) */
    0x2,    /* ted orange L2       (8) -> vicii red       (2) */
    0x9,    /* ted brown L2        (9) -> vicii brown     (9) */
    0x9,    /* ted yellow-green L2 (A) -> vicii brown     (9) */
    0xB,    /* ted pink L2         (B) -> vicii dark gray (B) */
    0xB,    /* ted blue-green L2   (C) -> vicii dark gray (B) */
    0x6,    /* ted light blue L2   (D) -> vicii blue      (6) */
    0x6,    /* ted dark blue L2    (E) -> vicii blue      (6) */
    0x9,    /* ted light green L2  (F) -> vicii brown     (9) */

    0x0,    /* ted black L3        (0) -> vicii black     (0) */
    0xB,    /* ted white L3        (1) -> vicii dark gray (B) */
    0x2,    /* ted red L3          (2) -> vicii red       (2) */
    0xB,    /* ted cyan L3         (3) -> vicii dark gray (B) */
    0x4,    /* ted purple L3       (4) -> vicii purple    (4) */
    0x9,    /* ted green L3        (5) -> vicii brown     (9) */
    0x6,    /* ted blue L3         (6) -> vicii blue      (6) */
    0x9,    /* ted yellow L3       (7) -> vicii brown     (9) */
    0x8,    /* ted orange L3       (8) -> vicii orange    (8) */
    0x8,    /* ted brown L3        (9) -> vicii orange    (8) */
    0x9,    /* ted yellow-green L3 (A) -> vicii brown     (9) */
    0x4,    /* ted pink L3         (B) -> vicii purple    (4) */
    0xB,    /* ted blue-green L3   (C) -> vicii dark gray (B) */
    0x6,    /* ted light blue L3   (D) -> vicii blue      (6) */
    0x6,    /* ted dark blue L3    (E) -> vicii blue      (6) */
    0x9,    /* ted light green L3  (F) -> vicii brown     (9) */

    0x0,    /* ted black L4        (0) -> vicii black       (0) */
    0xC,    /* ted white L4        (1) -> vicii medium gray (C) */
    0xA,    /* ted red L4          (2) -> vicii light red   (A) */
    0xE,    /* ted cyan L4         (3) -> vicii light blue  (E) */
    0x4,    /* ted purple L4       (4) -> vicii purple      (4) */
    0x5,    /* ted green L4        (5) -> vicii green       (5) */
    0xE,    /* ted blue L4         (6) -> vicii light blue  (E) */
    0x5,    /* ted yellow L4       (7) -> vicii green       (5) */
    0xA,    /* ted orange L4       (8) -> vicii light red   (A) */
    0x8,    /* ted brown L4        (9) -> vicii orange      (8) */
    0x5,    /* ted yellow-green L4 (A) -> vicii green       (5) */
    0x4,    /* ted pink L4         (B) -> vicii purple      (4) */
    0xC,    /* ted blue-green L4   (C) -> vicii medium gray (C) */
    0xE,    /* ted light blue L4   (D) -> vicii light blue  (E) */
    0xE,    /* ted dark blue L4    (E) -> vicii light blue  (E) */
    0x5,    /* ted light green L4  (F) -> vicii green       (5) */

    0x0,    /* ted black L5        (0) -> vicii black       (0) */
    0xC,    /* ted white L5        (1) -> vicii medium gray (C) */
    0xA,    /* ted red L5          (2) -> vicii light red   (A) */
    0x3,    /* ted cyan L5         (3) -> vicii cyan        (3) */
    0xF,    /* ted purple L5       (4) -> vicii light gray  (F) */
    0x5,    /* ted green L5        (5) -> vicii green       (5) */
    0xE,    /* ted blue L5         (6) -> vicii light blue  (E) */
    0x5,    /* ted yellow L5       (7) -> vicii green       (5) */
    0xA,    /* ted orange L5       (8) -> vicii light red   (A) */
    0xA,    /* ted brown L5        (9) -> vicii light red   (A) */
    0x5,    /* ted yellow-green L5 (A) -> vicii green       (5) */
    0xA,    /* ted pink L5         (B) -> vicii light red   (A) */
    0x3,    /* ted blue-green L5   (C) -> vicii cyan        (3) */
    0xE,    /* ted light blue L5   (D) -> vicii light blue  (E) */
    0xE,    /* ted dark blue L5    (E) -> vicii light blue  (E) */
    0x5,    /* ted light green L5  (F) -> vicii green       (5) */

    0x0,    /* ted black L6        (0) -> vicii black       (0) */
    0xF,    /* ted white L6        (1) -> vicii light gray  (F) */
    0xA,    /* ted red L6          (2) -> vicii light red   (A) */
    0x3,    /* ted cyan L6         (3) -> vicii cyan        (3) */
    0xF,    /* ted purple L6       (4) -> vicii light gray  (F) */
    0xD,    /* ted green L6        (5) -> vicii light green (D) */
    0xF,    /* ted blue L6         (6) -> vicii light gray  (F) */
    0x7,    /* ted yellow L6       (7) -> vicii yellow      (7) */
    0xA,    /* ted orange L6       (8) -> vicii light red   (A) */
    0xA,    /* ted brown L6        (9) -> vicii light red   (A) */
    0x7,    /* ted yellow-green L6 (A) -> vicii yellow      (7) */
    0xF,    /* ted pink L6         (B) -> vicii light gray  (F) */
    0x3,    /* ted blue-green L6   (C) -> vicii cyan        (3) */
    0xF,    /* ted light blue L6   (D) -> vicii light gray  (F) */
    0xF,    /* ted dark blue L6    (E) -> vicii light gray  (F) */
    0x7,    /* ted light green L6  (F) -> vicii yellow      (7) */

    0x0,    /* ted black L7        (0) -> vicii black       (0) */
    0x1,    /* ted white L7        (1) -> vicii white       (1) */
    0x1,    /* ted red L7          (2) -> vicii white       (1) */
    0x1,    /* ted cyan L7         (3) -> vicii white       (1) */
    0x1,    /* ted purple L7       (4) -> vicii white       (1) */
    0xD,    /* ted green L7        (5) -> vicii light green (D) */
    0x1,    /* ted blue L7         (6) -> vicii white       (1) */
    0x7,    /* ted yellow L7       (7) -> vicii yellow      (7) */
    0xF,    /* ted orange L7       (8) -> vicii light gray  (F) */
    0x7,    /* ted brown L7        (9) -> vicii yellow      (7) */
    0x7,    /* ted yellow-green L7 (A) -> vicii yellow      (7) */
    0x1,    /* ted pink L7         (B) -> vicii white       (1) */
    0xD,    /* ted blue-green L7   (C) -> vicii light green (D) */
    0x1,    /* ted light blue L7   (D) -> vicii white       (1) */
    0x1,    /* ted dark blue L7    (E) -> vicii white       (1) */
    0xD     /* ted light green L7  (F) -> vicii light green (D) */
};

static BYTE ted_lum_to_vicii_color(BYTE color, BYTE lum)
{
    return ted_lum_vicii_translate[(lum * 16) + color];
}

void ted_color_to_vicii_color_colormap(native_data_t *source, int ted_lum_handling)
{
    int i, j;
    BYTE colorbyte;

    for (i = 0; i < source->ysize; i++) {
        for (j = 0; j < source->xsize; j++) {
            colorbyte = source->colormap[(i * source->xsize) +  j];
            if (ted_lum_handling == NATIVE_SS_TED_LUM_DITHER) {
                source->colormap[(i * source->xsize) + j] = ted_lum_to_vicii_color(colorbyte & 0xf, colorbyte >> 4);
            } else {
                source->colormap[(i * source->xsize) + j] = ted_to_vicii_color(colorbyte & 0xf);
            }
        }
    }
}

native_data_t *native_ted_text_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    BYTE brdrcolor;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;

    data->colormap = lib_malloc(320 * 200);

    bgcolor = regs[0x15] & 0x7f;

    brdrcolor = regs[0x19] & 0x7f;

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            fgcolor = screenshot->color_ram_ptr[(i * 40) + j] & 0x7f;
            for (k = 0; k < 8; k++) {
                if (regs[0x07] & 0x80) {
                    bitmap = screenshot->chargen_ptr[(screenshot->screen_ptr[(i * 40) + j] * 8) + k];
                } else {
                    bitmap = screenshot->chargen_ptr[((screenshot->screen_ptr[(i * 40) + j] & 0x7f) * 8) + k];
                    if (screenshot->screen_ptr[(i * 40) + j] & 0x80) {
                        bitmap = ~bitmap;
                    }
                }
                for (l = 0; l < 8; l++) {
                    if (bitmap & (1 << (7 - l))) {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = fgcolor;
                    } else {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = bgcolor;
                    }
                }
            }
        }
    }
    if (((regs[0x07] & 8) == 0) || ((regs[0x06] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, brdrcolor, (BYTE)((regs[0x07] & 8) ? 255  : regs[0x07] & 7), (BYTE)((regs[0x06] & 8) ? 255 : regs[0x06] & 7));
    }
    return data;
}

native_data_t *native_ted_extended_background_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    BYTE brdrcolor;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;

    data->colormap = lib_malloc(320 * 200);

    brdrcolor = regs[0x19] & 0x7f;

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            fgcolor = screenshot->color_ram_ptr[(i * 40) + j] & 0x7f;
            bgcolor = regs[0x15 + ((screenshot->screen_ptr[(i * 40) + j] & 0xc0) >> 6)] & 0x7f;
            for (k = 0; k < 8; k++) {
                bitmap = screenshot->chargen_ptr[((screenshot->screen_ptr[(i * 40) + j] & 0x3f) * 8) + k];
                if ((regs[0x07] & 0x80) && (screenshot->screen_ptr[(i * 40) + j] & 0x80)) {
                    bitmap = ~bitmap;
                }
                for (l = 0; l < 8; l++) {
                    if (bitmap & (1 << (7 - l))) {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = fgcolor;
                    } else {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = bgcolor;
                    }
                }
            }
        }
    }
    if (((regs[0x07] & 8) == 0) || ((regs[0x06] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, brdrcolor, (BYTE)((regs[0x07] & 8) ? 255  : regs[0x07] & 7), (BYTE)((regs[0x06] & 8) ? 255 : regs[0x06] & 7));
    }
    return data;
}

native_data_t *native_ted_hires_bitmap_mode_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    BYTE brdrcolor;
    int i, j, k, l;
    native_data_t *data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = 320;
    data->ysize = 200;

    data->colormap = lib_malloc(320 * 200);

    brdrcolor = regs[0x19] & 0x7f;

    for (i = 0; i < 25; i++) {
        for (j = 0; j < 40; j++) {
            fgcolor = (screenshot->screen_ptr[(i * 40) + j] & 0xf0) >> 4;
            fgcolor |= (screenshot->screen_ptr[(i * 40) + j] & 0x70);
            bgcolor = screenshot->screen_ptr[(i * 40) + j] & 0xf;
            bgcolor |= ((screenshot->screen_ptr[(i * 40) + j] & 0x7) << 4);
            for (k = 0; k < 8; k++) {
                bitmap = screenshot->bitmap_ptr[(i * 40 * 8) + j + (k * 40)];
                for (l = 0; l < 8; l++) {
                    if (bitmap & (1 << (7 - l))) {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = fgcolor;
                    } else {
                        data->colormap[(i * 320 * 8) + (j * 8) + (k * 320) + l] = bgcolor;
                    }
                }
            }
        }
    }
    if (((regs[0x07] & 8) == 0) || ((regs[0x06] & 8) == 0)) {
        native_smooth_scroll_borderize_colormap(data, brdrcolor, (BYTE)((regs[0x07] & 8) ? 255  : regs[0x07] & 7), (BYTE)((regs[0x06] & 8) ? 255 : regs[0x06] & 7));
    }
    return data;
}

/* ------------------------------------------------------------------------ */

static BYTE vic_vicii_translate[16] = {
    0x0,    /* vic black        (0) -> vicii black       (0) */
    0x1,    /* vic white        (1) -> vicii white       (1) */
    0x2,    /* vic red          (2) -> vicii red         (2) */
    0x3,    /* vic cyan         (3) -> vicii cyan        (3) */
    0x4,    /* vic purple       (4) -> vicii purple      (4) */
    0x5,    /* vic green        (5) -> vicii green       (5) */
    0x6,    /* vic blue         (6) -> vicii blue        (6) */
    0x7,    /* vic yellow       (7) -> vicii yellow      (7) */
    0x8,    /* vic orange       (8) -> vicii orange      (8) */
    0x8,    /* vic light orange (9) -> vicii orange      (8) */
    0x8,    /* vic pink         (A) -> vicii orange      (8) */
    0xD,    /* vic light cyan   (B) -> vicii light green (D) */
    0x4,    /* vic light purple (C) -> vicii purple      (4) */
    0xD,    /* vic light green  (D) -> vicii light green (D) */
    0xE,    /* vic light blue   (E) -> vicii light blue  (E) */
    0x7     /* vic light yellow (F) -> vicii yellow      (7) */
};

static inline BYTE vic_to_vicii_color(BYTE color)
{
    return vic_vicii_translate[color];
}

void vic_color_to_vicii_color_colormap(native_data_t *source)
{
    int i, j;

    for (i = 0; i < source->ysize; i++) {
        for (j = 0; j < source->xsize; j++) {
            source->colormap[(i * source->xsize) + j] = vic_to_vicii_color(source->colormap[(i * source->xsize) + j]);
        }
    }
}

native_data_t *native_vic_render(screenshot_t *screenshot, const char *filename)
{
    BYTE *regs = screenshot->video_regs;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    BYTE brdrcolor;
    BYTE auxcolor;
    int i, j, k, l;
    native_data_t *data;
    BYTE xsize;
    BYTE ysize;

    xsize = regs[0x02] & 0x7f;
    ysize = (regs[0x03] & 0x7e) >> 1;

    if (xsize == 0 || ysize == 0) {
        ui_error("Screen is blank, no save will be done");
        return NULL;
    }

    if (screenshot->chargen_ptr == NULL) {
        ui_error("Character generator memory is illegal");
        return NULL;
    }

    data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = xsize * 8;
    data->ysize = ysize * 8;

    data->colormap = lib_malloc(data->xsize * data->ysize);

    bgcolor = (regs[0xf] & 0xf0) >> 4;
    auxcolor = (regs[0xe] & 0xf0) >> 4;
    brdrcolor = regs[0xf] & 3;
    for (i = 0; i < ysize; i++) {
        for (j = 0; j < xsize; j++) {
            fgcolor = screenshot->color_ram_ptr[(i * xsize) + j] & 7;
            for (k = 0; k < 8; k++) {
                bitmap = screenshot->chargen_ptr[(screenshot->screen_ptr[(i * xsize) + j] * 8) + k];
                if (!(regs[0xf] & 8)) {
                    bitmap = ~bitmap;
                }
                if (screenshot->color_ram_ptr[(i * xsize) + j] & 8) {
                    for (l = 0; l < 4; l++) {
                        data->mc_data_present = 1;
                        switch ((bitmap & (3 << ((3 - l) * 2))) >> ((3 - l) * 2)) {
                            case 0:
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2)] =  bgcolor;
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2) + 1] =  bgcolor;
                                break;
                            case 1:
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2)] =  brdrcolor;
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2) + 1] =  brdrcolor;
                                break;
                            case 2:
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2)] =  fgcolor;
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2) + 1] =  fgcolor;
                                break;
                            case 3:
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2)] =  auxcolor;
                                data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + (l * 2) + 1] =  auxcolor;
                                break;
                        }
                    }
                } else {
                    for (l = 0; l < 8; l++) {
                        if (bitmap & (1 << (7 - l))) {
                            data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + l] = fgcolor;
                        } else {
                            data->colormap[(i * data->xsize * 8) + (j * 8) + (k * data->xsize) + l] = bgcolor;
                        }
                    }
                }
            }
        }
    }
    return data;
}

/* ------------------------------------------------------------------------ */

#define MA_WIDTH        64
#define MA_LO           (MA_WIDTH - 1)          /* 6 bits */
#define MA_HI           (~MA_LO)

native_data_t *native_crtc_render(screenshot_t *screenshot, const char *filename, int crtc_fgcolor)
{
    BYTE *regs = screenshot->video_regs;
    BYTE *petdww_ram = screenshot->bitmap_ptr;
    BYTE bitmap;
    BYTE fgcolor;
    BYTE bgcolor;
    int x, y, k, l;
    native_data_t *data;
    BYTE xsize;
    BYTE ysize;
    BYTE invert;
    BYTE charheight;
    int shift;
    int shiftand;
    int chars = 1;
    int hre = 0;
    int scr_rel;

    switch (screenshot->bitmap_low_ptr[0]) {
        default:
        case 40:
            xsize = regs[0x01];
            shift = ((regs[0x0c] & 3) << 8) + regs[0x0d];
            shiftand = 0x3ff;
            break;
        case 60:
            xsize = regs[0x01];
            shift = ((regs[0x0c] & 3) << 8) + regs[0x0d];
            shiftand = 0x7ff;
            break;
        case 80:
            xsize = regs[0x01] << 1;
            shift = ((regs[0x0c] & 3) << 9) + regs[0x0d];
            shiftand = 0x7ff;
            break;
    }

    ysize = regs[0x06];
    invert = (regs[0x0c] & 0x10) >> 4;

    if (xsize == 0 || ysize == 0) {
        ui_error("Screen is blank, no save will be done");
        return NULL;
    }

    charheight = screenshot->bitmap_high_ptr[0];

    data = lib_malloc(sizeof(native_data_t));

    data->filename = filename;
    data->mc_data_present = 0;

    data->xsize = xsize * 8;
    data->ysize = ysize * charheight;

    data->colormap = lib_malloc(data->xsize * data->ysize);

    if (!invert) {	/* On 8296 only! */
        hre = 1;
        chars = 0;
        invert = 1;
    }

    bgcolor = 0;
    fgcolor = crtc_fgcolor;
    scr_rel = shift;

    /*
     * Assumes screenshot->screen_ptr points to 0x8000 on a PET,
     * not taking the CRTC registers into account
     * (since then they would be added twice)
     */
    for (y = 0; y < ysize; y++) {
        for (x = 0; x < xsize; x++) {
            for (k = 0; k < charheight; k++) {
                if (hre) {
                    int ma_hi = scr_rel & MA_HI;    /* MA<9...6> */
                    int ma_lo = scr_rel & MA_LO;    /* MA<5...0> */
                    /* Form <MA 9-6><RA 2-0><MA 5-0> */
                    int addr = (ma_hi << 3) + (k << 6) + ma_lo;
                    bitmap = screenshot->screen_ptr[addr];
                } else {
                    bitmap = 0;
                    if (chars) {
                        BYTE chr = screenshot->screen_ptr[scr_rel & shiftand];
                        bitmap = screenshot->chargen_ptr[(chr * 16) + k];
                    }
                    if (petdww_ram) {
                        int addr = (k * 1024) + scr_rel;
                        BYTE b = petdww_ram[addr];

                        /* now reverse the bits...
                           http://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith32Bits */
                        b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
                        bitmap |= (b & 0xFF);
                    }
                }
                if (!invert) {
                    bitmap = ~bitmap;
                }
                for (l = 0; l < 8; l++) {
                    int color;

                    if (bitmap & (1 << (7 - l))) {
                        color = fgcolor;
                    } else {
                        color = bgcolor;
                    }
                    data->colormap[(y * data->xsize * charheight) + (x * 8) + (k * data->xsize) + l] = color;
                }
            }
            scr_rel++;
        }
    }
    return data;
}
