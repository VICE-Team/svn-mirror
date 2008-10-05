/*
 * render1x1pal.c - 1x1 PAL renderers
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

#include "render1x1pal.h"
#include "types.h"
#include "video-resources.h"

extern DWORD gamma_red[256 * 3];
extern DWORD gamma_grn[256 * 3];
extern DWORD gamma_blu[256 * 3];

static inline void
store_pixel_2(BYTE *trg, WORD red, WORD grn, WORD blu)
{
    WORD *tmp = (WORD *) trg;
    *tmp = (WORD) (gamma_red[red] | gamma_grn[grn] | gamma_blu[blu]);
}

static inline void
store_pixel_3(BYTE *trg, WORD red, WORD grn, WORD blu)
{
    DWORD tmp = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
    trg[0] = (BYTE) tmp;
    tmp >>= 8;
    trg[1] = (BYTE) tmp;
    tmp >>= 8;
    trg[2] = (BYTE) tmp;
}

static inline void
store_pixel_4(BYTE *trg, WORD red, WORD grn, WORD blu)
{
    DWORD *tmp = (DWORD *) trg;
    *tmp = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
}

/* PAL 1x1 renderers */
static inline void
render_generic_1x1_pal(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int pixelstride,
                       void (*store_func)(BYTE *trg,
                                          WORD red, WORD grn, WORD blu))
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const BYTE *tmpsrc;
    BYTE *tmptrg;
    unsigned int x, y;
    SDWORD *line, l, u, v, unew, vnew;
    WORD red, grn, blu;
    BYTE cl0, cl1, cl2, cl3;
    int off, off_flip;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + xt * pixelstride;
    
    line = color_tab->line_yuv_0;
    tmpsrc = ys > 0 ? src - pitchs : src;
    off_flip = ys > 0 ? 1 : -1;

    /* is the previous line odd or even? (inverted condition!) */
    if (ys & 1) {
        cbtable = color_tab->cbtable;
        crtable = color_tab->crtable;
    } else {
        cbtable = color_tab->cbtable_odd;
        crtable = color_tab->crtable_odd;
    }

    for (x = 0; x < width; x++) {
        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        tmpsrc += 1;
        line[0] = (cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3]) * off_flip;
        line[1] = (crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3]) * off_flip;
        line += 2;
    }

    /* Calculate odd line shading */
    off = (int) (((float) video_resources.pal_oddlines_offset * (1.5f / 2000.0f) - (1.5f / 2.0f - 1.0f)) * (1 << 5) * -1);

    for (y = ys; y < height + ys; y++) {
        tmpsrc = src;
        tmptrg = trg;

        line = color_tab->line_yuv_0;

        if (y & 1) { /* odd sourceline */
            off_flip = off;
            cbtable = color_tab->cbtable_odd;
            crtable = color_tab->crtable_odd;
        } else {
            off_flip = 1 << 5;
            cbtable = color_tab->cbtable;
            crtable = color_tab->crtable;
        }

        for (x = 0; x < width; x++) {
            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            tmpsrc += 1;
            l = ytablel[cl1] + ytableh[cl2] + ytablel[cl3];
            unew = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
            vnew = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];

            u = (unew - line[0]) * off_flip;
            v = (vnew - line[1]) * off_flip;
            line[0] = unew;
            line[1] = vnew;
            line += 2;

            red = (WORD) ((l + v) >> 16);
            blu = (WORD) ((l + u) >> 16);
            grn = (WORD) ((l - ((50 * u + 130 * v) >> 8)) >> 16);

            store_func(tmptrg, red, grn, blu);
            tmptrg += pixelstride;
        }

        src += pitchs;
        trg += pitcht;
    }
}

void
render_16_1x1_pal(video_render_color_tables_t *color_tab,
                  const BYTE *src, BYTE *trg,
                  unsigned int width, const unsigned int height,
                  const unsigned int xs, const unsigned int ys,
                  const unsigned int xt, const unsigned int yt,
                  const unsigned int pitchs, const unsigned int pitcht)
{
    render_generic_1x1_pal(color_tab, src, trg, width, height, xs, ys, xt, yt,
                           pitchs, pitcht,
                           2, store_pixel_2);
}

void
render_24_1x1_pal(video_render_color_tables_t *color_tab,
                  const BYTE *src, BYTE *trg,
                  unsigned int width, const unsigned int height,
                  const unsigned int xs, const unsigned int ys,
                  const unsigned int xt, const unsigned int yt,
                  const unsigned int pitchs, const unsigned int pitcht)
{
    render_generic_1x1_pal(color_tab, src, trg, width, height, xs, ys, xt, yt,
                           pitchs, pitcht,
                           3, store_pixel_3);
}

void
render_32_1x1_pal(video_render_color_tables_t *color_tab,
                  const BYTE *src, BYTE *trg,
                  unsigned int width, const unsigned int height,
                  const unsigned int xs, const unsigned int ys,
                  const unsigned int xt, const unsigned int yt,
                  const unsigned int pitchs, const unsigned int pitcht)
{
    render_generic_1x1_pal(color_tab, src, trg, width, height, xs, ys, xt, yt,
                           pitchs, pitcht,
                           4, store_pixel_4);
}
