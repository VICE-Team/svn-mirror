/*
 * video-color.c - Video implementation of YUV, YCbCr and RGB colors
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

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>       /* needed for pow function */

#include "machine.h"
#include "palette.h"
#include "video-canvas.h"
#include "video-color.h"
#include "video-resources.h"
#include "videoarch.h"
#include "video.h"


SDWORD ytable[128];
SDWORD cbtable[128];
SDWORD crtable[128];

DWORD gamma_red[256 * 3];
DWORD gamma_grn[256 * 3];
DWORD gamma_blu[256 * 3];

DWORD gamma_red_fac[256 * 3];
DWORD gamma_grn_fac[256 * 3];
DWORD gamma_blu_fac[256 * 3];

DWORD color_red[256];
DWORD color_grn[256];
DWORD color_blu[256];


void video_render_setrawrgb(unsigned int index, DWORD r, DWORD g, DWORD b)
{
    color_red[index] = r;
    color_grn[index] = g;
    color_blu[index] = b;
}

/*

  formulas for color space conversion:

  RGB to YCbCr:

  Y  = 0.2989*R + 0.5866*G + 0.1145*B
  Cb = B - Y
  Cr = R - Y

  YCbCr to RGB:

  G = Y - (0.1145/0.5866)*Cb - (0.2989/0.5866)*Cr
  B = Cb + Y
  R = Cr + Y

  YCbCr to YUV:

  U = 0.493111*Cb
  V = 0.877283*Cr

  YUV is the PAL colorspace. It's just a slightly modified YCbCr for
  better broadcasting.

*/

#define MATH_PI         3.141592653589793238462643383279
    /* 5028841971 6939937510 5820974944 5923078164 0628620899 8628034825
       3421170679 */

typedef struct video_ycbcr_color_s {
    float y;
    float cb;
    float cr;
} video_ycbcr_color_t;

/* variables needed for generating and activating a palette */

struct video_canvas_s *video_current_canvas = NULL;

void video_color_set_canvas(struct video_canvas_s *canvas)
{
    video_current_canvas = canvas;
}

/* conversion of VIC/VIC-II/TED colors to YCbCr */

static void video_convert_cbm_to_ycbcr(const video_cbm_color_t *src,
                                       float basesat, float phase,
                                       video_ycbcr_color_t *dst)
{
    dst->y = src->luminance;

    /* chrominance (U and V) of color */

    dst->cb = (float)(basesat * cos((src->angle + phase) * (MATH_PI / 180.0)));
    dst->cr = (float)(basesat * sin((src->angle + phase) * (MATH_PI / 180.0)));

    /* convert UV to CbCr */

    dst->cb /= 0.493111f;
    dst->cr /= 0.877283f;

    /* direction of color vector (-1 = inverted vector, 0 = grey vector) */

    if (src->direction == 0) {
        dst->cb = 0.0f;
        dst->cr = 0.0f;
    }
    if (src->direction < 0) {
        dst->cb = -dst->cb;
        dst->cr = -dst->cr;
    }

}

/* gamma correction */

static float video_gamma(float value, float gamma, float bri, float con)
{
    double factor;
    float ret;

    value += bri;
    value *= con;

    if (value <= 0.0f)
        return 0.0f;

    factor = pow(255.0f, 1.0f - gamma);
    ret = (float)(factor * pow(value, gamma));

    if (ret < 0.0f)
        ret = 0.0f;

    return ret;
}

/* conversion of YCbCr to RGB */

static void video_convert_ycbcr_to_rgb(video_ycbcr_color_t *src, float sat,
                                       float bri, float con, float gam,
                                       palette_entry_t *dst)
{
    float rf, bf, gf;
    int r, g, b;

    /* apply saturation */

    src->cb *= sat;
    src->cr *= sat;

    /* convert YCbCr to RGB */

    bf = src->cb + src->y;
    rf = src->cr + src->y;
    gf = src->y - (0.1145f / 0.5866f) * src->cb - (0.2989f / 0.5866f) * src->cr;

    rf = video_gamma(rf, gam, bri, con);
    gf = video_gamma(gf, gam, bri, con);
    bf = video_gamma(bf, gam, bri, con);

    /* convert to int and clip to 8 bit boundaries */

    r = (int)rf;
    g = (int)gf;
    b = (int)bf;

    if (r <   0) r =   0;
    if (r > 255) r = 255;
    if (g <   0) g =   0;
    if (g > 255) g = 255;
    if (b <   0) b =   0;
    if (b > 255) b = 255;

    dst->dither = 0;
    dst->red    = (BYTE)r;
    dst->green  = (BYTE)g;
    dst->blue   = (BYTE)b;
    dst->name   = NULL;
}

/* gammatable calculation */

static void video_calc_gammatable(void)
{
    int i;
    float bri, con, gam, scn, v;
    DWORD vi;

    bri = ((float)(video_resources.color_brightness - 1000))
          * (128.0f / 1000.0f);
    con = ((float)(video_resources.color_contrast   )) / 1000.0f;
    gam = ((float)(video_resources.color_gamma      )) / 1000.0f;
    scn = ((float)(video_resources.pal_scanlineshade)) / 1000.0f;

    for (i = 0; i < (256 * 3); i++) {
        v = video_gamma((float)(i - 256), gam, bri, con);

        vi = (DWORD)v;
        if (vi > 255)
            vi = 255;
        gamma_red[i] = color_red[vi];
        gamma_grn[i] = color_grn[vi];
        gamma_blu[i] = color_blu[vi];

        vi = (DWORD)(v * scn);
        if (vi > 255)
            vi = 255;
        gamma_red_fac[i] = color_red[vi];
        gamma_grn_fac[i] = color_grn[vi];
        gamma_blu_fac[i] = color_blu[vi];
    }
}

/* ycbcr table calculation */

/* YUV table for hardware rendering: (Y << 16) | (U << 8) | V */
DWORD yuv_table[128];

static void video_calc_ycbcrtable(const video_cbm_palette_t *p)
{
    video_ycbcr_color_t primary;
    unsigned int i;
    float sat;

    sat = ((float)(video_resources.color_saturation)) * (256.0f / 1000.0f);
    for (i = 0;i < p->num_entries; i++) {
        video_convert_cbm_to_ycbcr(&p->entries[i], p->saturation,
                                   p->phase,&primary);
        ytable[i] = (SDWORD)(primary.y * 256.0f);
        cbtable[i] = (SDWORD)(primary.cb * sat);
        crtable[i] = (SDWORD)(primary.cr * sat);

        /* YCbCr to YUV, scale [0, 256] to [0, 255] */
        yuv_table[i] = ((BYTE)(primary.y * 255 / 256 + 0.5) << 16)
            | ((BYTE)(0.493111 * primary.cb * 255 / 256 + 128.5) << 8)
            | (BYTE)(0.877283 * primary.cr * 255 / 256 + 128.5);
    }
}

/* Calculate a RGB palette out of VIC/VIC-II/TED colors.  */
static palette_t *video_calc_palette(const video_cbm_palette_t *p)
{
    palette_t *prgb;
    video_ycbcr_color_t primary;
    unsigned int i, j, index;
    float sat, bri, con, gam, cb, cr;

    sat = ((float)(video_resources.color_saturation     )) / 1000.0f;
    bri = ((float)(video_resources.color_brightness-1000)) * (128.0f / 1000.0f);
    con = ((float)(video_resources.color_contrast       )) / 1000.0f;
    gam = ((float)(video_resources.color_gamma          )) / 1000.0f;
    if ((!video_resources.delayloop_emulation) || (p->num_entries > 16)) {
        /* create RGB palette with the base colors of the video chip */

        prgb = palette_create(p->num_entries, NULL);
        if (prgb == NULL)
            return NULL;

        for (i = 0; i <p->num_entries; i++) {
            video_convert_cbm_to_ycbcr(&p->entries[i], p->saturation,
                                       p->phase, &primary);
            video_convert_ycbcr_to_rgb(&primary, sat, bri, con, gam,
                                       &prgb->entries[i]);
        }
    } else {
        /* create RGB palette with the mixed base colors of the video chip */
        /* this is for the fake pal emu only, maximum 16 colors allowed */

        prgb = palette_create(p->num_entries * p->num_entries, NULL);
        if (prgb == NULL)
            return NULL;

        index = 0;
        for (j = 0; j < p->num_entries; j++) {
            video_convert_cbm_to_ycbcr(&p->entries[j], p->saturation,
                                       p->phase, &primary);
            cb = primary.cb;
            cr = primary.cr;
            for (i = 0; i < p->num_entries; i++) {
                video_convert_cbm_to_ycbcr(&p->entries[i], p->saturation,
                                           p->phase, &primary);
                primary.cb = (primary.cb + cb) * 0.5f;
                primary.cr = (primary.cr + cr) * 0.5f;
                video_convert_ycbcr_to_rgb(&primary, sat, bri, con, gam,
                                           &prgb->entries[index]);
                index++;
            }
        }
    }
    return prgb;
}

/* Load RGB palette.  */
static palette_t *video_load_palette(const video_cbm_palette_t *p,
                                     const char *name)
{
    palette_t *palette;

    palette = palette_create(p->num_entries, NULL);

    if (palette == NULL)
        return NULL;

    if (!console_mode && !vsid_mode && palette_load(name, palette) < 0) {
        /* log_message(vicii.log, "Cannot load palette file `%s'.", name); */
        return NULL;
    }

    return palette;
}

/* Calculate or load a palette, depending on configuration.  */
int video_color_update_palette(struct video_canvas_s *canvas)
{
    palette_t *palette;

    if (canvas == NULL)
        return 0;
    if (canvas->videoconfig->cbm_palette == NULL)
        return 0;

    if (canvas->videoconfig->external_palette) {
        palette = video_load_palette(canvas->videoconfig->cbm_palette,
                                     canvas->videoconfig->external_palette_name);
    } else {
        video_calc_gammatable();
        video_calc_ycbcrtable(canvas->videoconfig->cbm_palette);
        palette = video_calc_palette(canvas->videoconfig->cbm_palette);
    }

    if (palette != NULL)
        return video_canvas_palette_set(canvas, palette);

    return -1;
}

void video_color_palette_internal(struct video_canvas_s *canvas,
                                  struct video_cbm_palette_s *cbm_palette)
{
    canvas->videoconfig->cbm_palette = cbm_palette;
}

void video_color_palette_free(struct palette_s *palette)
{
    palette_free(palette);
}

void video_render_initraw(void)
{
    video_calc_gammatable();
}

