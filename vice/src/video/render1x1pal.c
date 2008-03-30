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

#include "render1x1.h"
#include "render1x1pal.h"
#include "types.h"

extern DWORD gamma_red[256 * 3];
extern DWORD gamma_grn[256 * 3];
extern DWORD gamma_blu[256 * 3];

/* PAL 1x1 renderers */

void render_16_1x1_pal(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht)
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const BYTE *tmpsrc;
    WORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wstart, wfast, wend, wint;
    SDWORD l, u, v;
    DWORD red, grn, blu;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 1);
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + 5;
    lineptr0 = color_tab->line_yuv_0;
    lineptr1 = color_tab->line_yuv_1;

    tmpsrc = src - (ys ? pitchs : 0);
    line = lineptr0;
    for (x = 0; x < wint; x++) {
        register DWORD cl0, cl1, cl2, cl3;

        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = 0;
        line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
        line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
        tmpsrc++;
        line += 3;
    }

    for (y = 0; y < height; y++) {
        tmpsrc = src;
        tmptrg = (WORD *)trg;

        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
        for (x = 0; x < wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
            tmpsrc++;
            line += 3;
        }

        line = lineptr0;
        linepre = lineptr1;
        for (x = 0; x < (wfast << 3) + wend + wstart; x++) {

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v + l) >> 8) + 256;
            blu = ((u + l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            *tmptrg++ = (WORD)(gamma_red[red] | gamma_grn[grn]
                        | gamma_blu[blu]);
        }

        src += pitchs;
        trg += pitcht;
    }
}

void render_32_1x1_pal(video_render_color_tables_t *color_tab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht)
{
    const SDWORD *cbtable = color_tab->cbtable;
    const SDWORD *crtable = color_tab->crtable;
    const SDWORD *ytablel = color_tab->ytablel;
    const SDWORD *ytableh = color_tab->ytableh;
    const BYTE *tmpsrc;
    DWORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wstart, wfast, wend, wint;
    SDWORD l, u, v;
    DWORD red, grn, blu;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 2);
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)(8 - ((unsigned long)trg & 7));
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + 5;
    lineptr0 = color_tab->line_yuv_0;
    lineptr1 = color_tab->line_yuv_1;

    tmpsrc = src - (ys ? pitchs : 0);
    line = lineptr0;
    for (x = 0; x < wint; x++) {
        register DWORD cl0, cl1, cl2, cl3;

        cl0 = tmpsrc[0];
        cl1 = tmpsrc[1];
        cl2 = tmpsrc[2];
        cl3 = tmpsrc[3];
        line[0] = 0;
        line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
        line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
        tmpsrc++;
        line += 3;
    }

    for (y = 0; y < height; y++) {
        tmpsrc = src;
        tmptrg = (DWORD *)trg;

        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
        for (x = 0; x < wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytablel[cl1] + ytableh[cl2] + ytablel[cl3]) >> 8;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
            tmpsrc++;
            line += 3;
        }

        line = lineptr0;
        linepre = lineptr1;
        for (x = 0; x < (wfast << 3) + wend + wstart; x++) {

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50*u - 130*v) >> 16) + 256;

            *tmptrg++ = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
        }

        src += pitchs;
        trg += pitcht;
    }
}

