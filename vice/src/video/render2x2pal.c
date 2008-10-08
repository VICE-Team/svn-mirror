/*
 * render2x2pal.c - 2x2 PAL renderers
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

#include "render2x2.h"
#include "render2x2pal.h"
#include "types.h"
#include "video-resources.h"

extern DWORD gamma_red[256 * 3];
extern DWORD gamma_grn[256 * 3];
extern DWORD gamma_blu[256 * 3];

extern DWORD gamma_red_fac[256 * 3];
extern DWORD gamma_grn_fac[256 * 3];
extern DWORD gamma_blu_fac[256 * 3];

/* Often required function that stores gamma-corrected pixel to current line,
 * averages the current rgb with the contents of previous non-scanline-line,
 * stores the gamma-corrected scanline, and updates the prevline rgb buffer.
 * The variants 4, 3, 2 refer to pixel width of output. */

/* 1-line high artifacts appear on screen if compiler reorders these
 * writes. We could fix that by having a scratch buffer, though. */

static inline
void store_line_and_scanline_2(
    BYTE *line, BYTE *scanline, WORD *prevline,
    const WORD red, const WORD grn, const WORD blu)
{
    WORD *tmp1 = (WORD *) scanline;
    WORD *tmp2 = (WORD *) line;
    *tmp1 = (WORD) (gamma_red_fac[(red + prevline[0]) >> 1]
          | gamma_grn_fac[(grn + prevline[1]) >> 1]
          | gamma_blu_fac[(blu + prevline[2]) >> 1]);
    
    *tmp2 = (WORD) (gamma_red[red] | gamma_grn[grn] | gamma_blu[blu]);

    prevline[0] = red;
    prevline[1] = grn;
    prevline[2] = blu;
}

static inline
void store_line_and_scanline_3(
    BYTE *line, BYTE *scanline, WORD *prevline,
    const WORD red, const WORD grn, const WORD blu)
{
    DWORD tmp1 = gamma_red_fac[(red + prevline[0]) >> 1]
               | gamma_grn_fac[(grn + prevline[1]) >> 1]
               | gamma_blu_fac[(blu + prevline[2]) >> 1];
    DWORD tmp2 = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
    scanline[0] = (BYTE) tmp1;
    tmp1 >>= 8;
    scanline[1] = (BYTE) tmp1;
    tmp1 >>= 8;
    scanline[2] = (BYTE) tmp1;
    
    line[0] = (BYTE) tmp2;
    tmp2 >>= 8;
    line[1] = (BYTE) tmp2;
    tmp2 >>= 8;
    line[2] = (BYTE) tmp2;

    prevline[0] = red;
    prevline[1] = grn;
    prevline[2] = blu;
}

static inline
void store_line_and_scanline_4(
    BYTE *line, BYTE *scanline, WORD *prevline,
    const WORD red, const WORD grn, const WORD blu)
{
    DWORD *tmp1 = (DWORD *) scanline;
    DWORD *tmp2 = (DWORD *) line;
    *tmp1 = gamma_red_fac[(red + prevline[0]) >> 1]
          | gamma_grn_fac[(grn + prevline[1]) >> 1]
          | gamma_blu_fac[(blu + prevline[2]) >> 1];
    
    *tmp2 = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];

    prevline[0] = red;
    prevline[1] = grn;
    prevline[2] = blu;
}

static inline
void get_rgb_from_video(
    const BYTE *src, SDWORD *line,
    const int off_flip,
    const SDWORD* ytablel, const SDWORD* ytableh,
    const SDWORD* cbtable, const SDWORD* crtable,
    WORD *red, WORD *grn, WORD *blu)
{
    BYTE cl0, cl1, cl2, cl3;
    SDWORD unew, vnew;
    SDWORD l, u, v;
    cl0 = src[0];
    cl1 = src[1];
    cl2 = src[2];
    cl3 = src[3];

    l = ytablel[cl1] + ytableh[cl2] + ytablel[cl3];
    unew = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
    vnew = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
    u = (unew - line[0]) * off_flip;
    v = (vnew - line[1]) * off_flip;
    line[0] = unew;
    line[1] = vnew;

    *red = (WORD) ((l + v) >> 16);
    *blu = (WORD) ((l + u) >> 16);
    *grn = (WORD) ((l - ((50 * u + 130 * v) >> 8)) >> 16);
}

static inline
void render_generic_2x2_pal(video_render_color_tables_t *color_tab,
                       const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       unsigned int xs, const unsigned int ys,
                       unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
		       unsigned int viewport_height, unsigned int pixelstride,
                       void (*store_func)(
                            BYTE *line, BYTE *scanline, WORD *prevline,
                            const WORD red, const WORD grn, const WORD blu))
{
    WORD *prevrgblineptr;
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const BYTE *tmpsrc;
    BYTE *tmptrg, *tmptrgscanline;
    SDWORD *line;
    DWORD x, y, wfirst, wlast, yys;
    SDWORD off, off_flip;
    WORD red, grn, blu, red2, grn2, blu2;
    BYTE cl0, cl1, cl2, cl3;

    viewport_height *= 2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + xt * pixelstride;
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;

    line = color_tab->line_yuv_0;
    /* get previous line into buffer. */
    tmpsrc = ys > 0 ? src - pitchs : src;
    /* trick to get the first line to average with itself */
    off_flip = ys > 0 ? 1 : -1;

    /* is the previous line odd or even? (inverted condition!) */
    if (ys & 1) {
        cbtable = color_tab->cbtable;
        crtable = color_tab->crtable;
    } else {
        cbtable = color_tab->cbtable_odd;
        crtable = color_tab->crtable_odd;
    }
    
    /* Initialize line */
    for (x = 0; x < width + wfirst + wlast + 1; x++) {
        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = (cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3]) * off_flip;
        line[1] = (crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3]) * off_flip;
        tmpsrc++;
        line += 2;
    }
    /* That's all initialization we need for full lines. Unfortunately, for
     * scanlines we also need to calculate the RGB color of the previous
     * full line, and that requires initialization from 2 full lines above our
     * rendering target. We just won't render the scanline above the target row,
     * so you need to call us with 1 line before the desired rectangle, and
     * for one full line after it! */

    /* Calculate odd line shading */
    off = (int) (((float) video_resources.pal_oddlines_offset * (1.5f / 2000.0f) - (1.5f / 2.0f - 1.0f)) * (1 << 5) * -1);

    /* height & 1 == 0. */
    for (y = yys; y < yys + height + 1; y += 2) {

        /* when we are dealing with the last line, the rules change:
         * we no longer write the main output to screen, we just put it into
         * the scanline. */
        if (y == yys + height) {
            /* no place to put scanline in: we are outside viewport or still
             * doing the first iteration (height == 0) */
            if (y == yys || yys == 0 || yys > viewport_height)
                break;

            tmptrg = &color_tab->rgbscratchbuffer[0];
            tmptrgscanline = trg - pitcht;
        } else {
            /* pixel data to surface */
            tmptrg = trg;
            /* write scanline data to previous line if possible,
             * otherwise we dump it to the scratch region... We must never
             * render the scanline for the first row, because prevlinergb is not
             * yet initialized and scanline data would be bogus! */
            tmptrgscanline = y != yys && yys > 0 && yys <= viewport_height
                ? trg - pitcht
                : &color_tab->rgbscratchbuffer[0];
        }

	/* current source image for YUV xform */
        tmpsrc = src;
	/* prev line's YUV-xformed data */
        line = color_tab->line_yuv_0;

	if (y & 2) { /* odd sourceline */
            off_flip = off;
            cbtable = color_tab->cbtable_odd;
            crtable = color_tab->crtable_odd;
        } else {    
            off_flip = 1 << 5;
            cbtable = color_tab->cbtable;
            crtable = color_tab->crtable;
        }

	get_rgb_from_video(tmpsrc, line, off_flip, ytablel, ytableh, cbtable, crtable, &red, &grn, &blu);
        tmpsrc += 1;
        line += 2;

        /* actual line */
	prevrgblineptr = &color_tab->prevrgbline[0];
        if (wfirst) {
            get_rgb_from_video(tmpsrc, line, off_flip, ytablel, ytableh, cbtable, crtable, &red2, &grn2, &blu2);
            tmpsrc += 1;
            line += 2;
            store_func(tmptrg, tmptrgscanline, prevrgblineptr, (WORD) ((red+red2)>>1), (WORD) ((grn+grn2)>>1), (WORD) ((blu+blu2)>>1));
            tmptrgscanline += pixelstride;
            tmptrg += pixelstride;
            prevrgblineptr += 3;

            red = red2;
            blu = blu2;
            grn = grn2;
        }
        for (x = 0; x < width; x++) {
            store_func(tmptrg, tmptrgscanline, prevrgblineptr, red, grn, blu);
            tmptrgscanline += pixelstride;
            tmptrg += pixelstride;
            prevrgblineptr += 3;
            get_rgb_from_video(tmpsrc, line, off_flip, ytablel, ytableh, cbtable, crtable, &red2, &grn2, &blu2);
            tmpsrc += 1;
            line += 2;
            store_func(tmptrg, tmptrgscanline, prevrgblineptr, (WORD) ((red+red2)>>1), (WORD) ((grn+grn2)>>1), (WORD) ((blu+blu2)>>1));
            tmptrgscanline += pixelstride;
            tmptrg += pixelstride;
            prevrgblineptr += 3;

            red = red2;
            blu = blu2;
            grn = grn2;
        }
        if (wlast)
            store_func(tmptrg, tmptrgscanline, prevrgblineptr, red, grn, blu);
        
        src += pitchs;
        trg += pitcht * 2;
    }
}

void render_16_2x2_pal(video_render_color_tables_t *color_tab,
                       const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
		       const unsigned int viewport_height)
{
    render_generic_2x2_pal(color_tab, src, trg, width, height, xs, ys,
                           xt, yt, pitchs, pitcht, viewport_height,
                           2, store_line_and_scanline_2);
}

void render_24_2x2_pal(video_render_color_tables_t *color_tab,
                       const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
		       const unsigned int viewport_height)
{
    render_generic_2x2_pal(color_tab, src, trg, width, height, xs, ys,
                           xt, yt, pitchs, pitcht, viewport_height,
                           3, store_line_and_scanline_3);
}

void render_32_2x2_pal(video_render_color_tables_t *color_tab,
                       const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
		       const unsigned int viewport_height)
{
    render_generic_2x2_pal(color_tab, src, trg, width, height, xs, ys,
                           xt, yt, pitchs, pitcht, viewport_height,
                           4, store_line_and_scanline_4);
}
