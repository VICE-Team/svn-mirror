/*
 * render2x2.c - 2x2 renderers
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

#include "render2x2.h"
#include "types.h"


extern SDWORD ytable[128];
extern SDWORD cbtable[128];
extern SDWORD crtable[128];

extern DWORD gamma_red[256 * 3];
extern DWORD gamma_grn[256 * 3];
extern DWORD gamma_blu[256 * 3];

extern DWORD gamma_red_fac[256 * 3];
extern DWORD gamma_grn_fac[256 * 3];
extern DWORD gamma_blu_fac[256 * 3];

extern SDWORD line_yuv_0[1024 * 3];
extern SDWORD line_yuv_1[1024 * 3];

/* PAL 2x2 renderers */

void render_16_2x2_palyc(const DWORD *colortab, const BYTE *src, BYTE *trg,
                         unsigned int width, const unsigned int height,
                         const unsigned int xs, const unsigned int ys,
                         const unsigned int xt, const unsigned int yt,
                         const unsigned int pitchs, const unsigned int pitcht,
                         const unsigned int doublescan)
{
    const BYTE *tmpsrc;
    WORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    WORD color;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 1);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        wstart = 8 - ((unsigned int)trg & 7); /* alignment: 8 pixels*/
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend  = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = line_yuv_0;
    lineptr1 = line_yuv_1;

    tmpsrc = src - pitchs;
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

    if (yys & 1) {
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
            line[0] = ytable[cl2];
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                    + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                    + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (WORD *)trg;
        if ((!(y & 1)) || doublescan) {
            if (!(y & 1)) {
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
                     line[0] = ytable[cl2];
                     line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                             + cbtable[cl3];
                     line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                             + crtable[cl3];
                     tmpsrc++;
                     line += 3;
                }

            }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            if (!(y & 1)) {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu]);
                    *tmptrg++ = (WORD)(gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = (WORD)(gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu]);
                }
            } else {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v+l) >> 8) + 256;
                    blu2 = ((u+l) >> 8) + 256;
                    grn2 = (((l << 8) - 50*u - 130*v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red_fac[red] | gamma_grn_fac[grn]
                              | gamma_blu_fac[blu]);
                    *tmptrg++ = (WORD)(gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = (WORD)(gamma_red_fac[red] | gamma_grn_fac[grn]
                            | gamma_blu_fac[blu]);
                }
            }
            if (y & 1)
                src += pitchs;
        } else {
            color = (WORD)colortab[0];
            if (wfirst) {
                *tmptrg++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            for (x = 0; x < wfast; x++) {
                tmptrg[0] = color;
                tmptrg[1] = color;
                tmptrg[2] = color;
                tmptrg[3] = color;
                tmptrg[4] = color;
                tmptrg[5] = color;
                tmptrg[6] = color;
                tmptrg[7] = color;
                tmptrg[8] = color;
                tmptrg[9] = color;
                tmptrg[10] = color;
                tmptrg[11] = color;
                tmptrg[12] = color;
                tmptrg[13] = color;
                tmptrg[14] = color;
                tmptrg[15] = color;
                tmptrg += 16;
            }
            for (x = 0; x < wend; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            if (wlast) {
                *tmptrg = color;
            }
            if (y & 1)
                src += pitchs;
        }
        trg += pitcht;
    }
}

void render_32_2x2_palyc(const DWORD *colortab, const BYTE *src, BYTE *trg,
                         unsigned int width, const unsigned int height,
                         const unsigned int xs, const unsigned int ys,
                         const unsigned int xt, const unsigned int yt,
                         const unsigned int pitchs, const unsigned int pitcht,
                         const unsigned int doublescan)
{
    const BYTE *tmpsrc;
    DWORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    register DWORD color;
    SDWORD l, u ,v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src = src + pitchs * ys + xs - 2;
    trg = trg + pitcht * yt + (xt << 2);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        wstart = 8 - ((unsigned int)trg & 7); /* alignment: 8 pixels*/
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = line_yuv_0;
    lineptr1 = line_yuv_1;

    tmpsrc = src - pitchs;
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

    if (yys & 1) {
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
            line[0] = ytable[cl2];
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2] + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2] + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (DWORD *)trg;
        if ((!(y & 1)) || doublescan) {
            if (!(y & 1)) {
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
                    line[0] = ytable[cl2];
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v + l) >> 8) + 256;
            blu = ((u + l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            if (!(y & 1)) {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu];
                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
                }
            } else {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0;x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red_fac[red] | gamma_grn_fac[grn]
                              | gamma_blu_fac[blu];
                    *tmptrg++ = gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red_fac[red] | gamma_grn_fac[grn]
                            | gamma_blu_fac[blu];
                }
            }
            if (y & 1)
                src += pitchs;
        } else {
            color = colortab[0];
            if (wfirst) {
                *tmptrg++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            for (x = 0;x < wfast; x++) {
                tmptrg[0] = color;
                tmptrg[1] = color;
                tmptrg[2] = color;
                tmptrg[3] = color;
                tmptrg[4] = color;
                tmptrg[5] = color;
                tmptrg[6] = color;
                tmptrg[7] = color;
                tmptrg[8] = color;
                tmptrg[9] = color;
                tmptrg[10] = color;
                tmptrg[11] = color;
                tmptrg[12] = color;
                tmptrg[13] = color;
                tmptrg[14] = color;
                tmptrg[15] = color;
                tmptrg += 16;
            }
            for (x = 0; x < wend; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            if (wlast) {
                *tmptrg = color;
            }
            if (y & 1)
                src += pitchs;
        }
        trg += pitcht;
    }
}

void render_16_2x2_pal(const DWORD *colortab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const BYTE *tmpsrc;
    WORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    WORD color;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src=src + pitchs * ys + xs - 2;
    trg=trg + pitcht * yt + (xt << 1);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        wstart = 8 - ((unsigned int)trg & 7); /* alignment: 8 pixels*/
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = line_yuv_0;
    lineptr1 = line_yuv_1;

    tmpsrc = src - pitchs;
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

    if (yys & 1) {
        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
        for (x = 0;x < wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytable[cl1] + ytable[cl2] + ytable[cl2]
                      + ytable[cl3]) >> 2;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                      + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                      + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (WORD *)trg;
        if ((!(y & 1)) || doublescan) {
            if (!(y & 1)) {
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
                    line[0] = (ytable[cl1] + ytable[cl2] + ytable[cl2]
                            + ytable[cl3]) >> 2;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v + l) >> 8) + 256;
            blu = ((u + l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            if (!(y & 1)) {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red[(red+red2) >> 1]
                                | gamma_grn[(grn+grn2) >> 1]
                                | gamma_blu[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu]);
                    *tmptrg++ = (WORD)(gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = (WORD)(gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu]);
                }
            } else {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v+l) >> 8) + 256;
                    blu2 = ((u+l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x <(wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = (WORD)(gamma_red_fac[red] | gamma_grn_fac[grn]
                              | gamma_blu_fac[blu]);
                    *tmptrg++ = (WORD)(gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1]);

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = (WORD)(gamma_red_fac[red] | gamma_grn_fac[grn]
                            | gamma_blu_fac[blu]);
                }
            }
            if (y & 1)
                src += pitchs;
        } else {
            color = (WORD)colortab[0];
            if (wfirst) {
                *tmptrg++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            for (x = 0; x < wfast; x++) {
                tmptrg[0] = color;
                tmptrg[1] = color;
                tmptrg[2] = color;
                tmptrg[3] = color;
                tmptrg[4] = color;
                tmptrg[5] = color;
                tmptrg[6] = color;
                tmptrg[7] = color;
                tmptrg[8] = color;
                tmptrg[9] = color;
                tmptrg[10] = color;
                tmptrg[11] = color;
                tmptrg[12] = color;
                tmptrg[13] = color;
                tmptrg[14] = color;
                tmptrg[15] = color;
                tmptrg += 16;
            }
            for (x = 0;x < wend; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            if (wlast) {
                *tmptrg = color;
            }
            if (y & 1)
                src += pitchs;
        }
        trg += pitcht;
    }
}

void render_32_2x2_pal(const DWORD *colortab, const BYTE *src, BYTE *trg,
                       unsigned int width, const unsigned int height,
                       const unsigned int xs, const unsigned int ys,
                       const unsigned int xt, const unsigned int yt,
                       const unsigned int pitchs, const unsigned int pitcht,
                       const unsigned int doublescan)
{
    const BYTE *tmpsrc;
    DWORD *tmptrg;
    SDWORD *lineptr0;
    SDWORD *lineptr1;
    SDWORD *line;
    SDWORD *linepre;
    unsigned int x, y, wfirst, wstart, wfast, wend, wlast, wint, yys;
    register DWORD color;
    SDWORD l, u, v;
    DWORD red, grn, blu, red2, grn2, blu2;

    src=src + pitchs * ys + xs - 2;
    trg=trg + pitcht * yt + (xt << 2);
    yys = (ys << 1) | (yt & 1);
    wfirst = xt & 1;
    width -= wfirst;
    wlast = width & 1;
    width >>= 1;
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        wstart = 8 - ((unsigned int)trg & 7); /* alignment: 8 pixels*/
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07; /* do not forget the rest*/
    }
    wint = width + wfirst + wlast + 5;
    lineptr0 = line_yuv_0;
    lineptr1 = line_yuv_1;

    tmpsrc = src - pitchs;
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

    if (yys & 1) {
        line = lineptr0;
        lineptr0 = lineptr1;
        lineptr1 = line;

        tmpsrc = src;
        line = lineptr0;
        for (x = 0; x <wint; x++) {
            register DWORD cl0, cl1, cl2, cl3;

            cl0 = tmpsrc[0];
            cl1 = tmpsrc[1];
            cl2 = tmpsrc[2];
            cl3 = tmpsrc[3];
            line[0] = (ytable[cl1] + ytable[cl2] + ytable[cl2]
                    + ytable[cl3]) >> 2;
            line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                    + cbtable[cl3];
            line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                    + crtable[cl3];
            tmpsrc++;
            line += 3;
        }
    }
    line = lineptr1;

    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (DWORD *)trg;
        if ((!(y & 1)) || doublescan) {
            if (!(y & 1)) {
                line = lineptr0;
                lineptr0 = lineptr1;
                lineptr1 = line;

                tmpsrc = src;
                line = lineptr0;
                for (x = 0; x <wint; x++) {
                    register DWORD cl0, cl1, cl2, cl3;

                    cl0 = tmpsrc[0];
                    cl1 = tmpsrc[1];
                    cl2 = tmpsrc[2];
                    cl3 = tmpsrc[3];
                    line[0] = (ytable[cl1] + ytable[cl2] + ytable[cl2]
                            + ytable[cl3]) >> 2;
                    line[1] = cbtable[cl0] + cbtable[cl1] + cbtable[cl2]
                            + cbtable[cl3];
                    line[2] = crtable[cl0] + crtable[cl1] + crtable[cl2]
                            + crtable[cl3];
                    tmpsrc++;
                    line += 3;
                }

            }

            line = lineptr0;
            linepre = lineptr1;

            l = line[0];
            u = (line[1] + linepre[1]) >> 3;
            v = (line[2] + linepre[2]) >> 3;
            line += 3;
            linepre += 3;

            red = ((v+l) >> 8) + 256;
            blu = ((u+l) >> 8) + 256;
            grn = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

            if (!(y & 1)) {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red[red] | gamma_grn[grn]
                              | gamma_blu[blu];
                    *tmptrg++ = gamma_red[(red+red2) >> 1]
                              | gamma_grn[(grn+grn2) >> 1]
                              | gamma_blu[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red[red] | gamma_grn[grn] | gamma_blu[blu];
                }
            } else {
                if (wfirst) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                for (x = 0; x < (wfast << 3) + wstart + wend; x++) {
                    l = line[0];
                    u = (line[1] + linepre[1]) >> 3;
                    v = (line[2] + linepre[2]) >> 3;
                    line += 3;
                    linepre += 3;

                    red2 = ((v + l) >> 8) + 256;
                    blu2 = ((u + l) >> 8) + 256;
                    grn2 = (((l << 8) - 50 * u - 130 * v) >> 16) + 256;

                    *tmptrg++ = gamma_red_fac[red] | gamma_grn_fac[grn]
                              | gamma_blu_fac[blu];
                    *tmptrg++ = gamma_red_fac[(red+red2) >> 1]
                              | gamma_grn_fac[(grn+grn2) >> 1]
                              | gamma_blu_fac[(blu+blu2) >> 1];

                    red = red2;
                    blu = blu2;
                    grn = grn2;
                }
                if (wlast) {
                    *tmptrg = gamma_red_fac[red] | gamma_grn_fac[grn]
                            | gamma_blu_fac[blu];
                }
            }
            if (y & 1)
                src += pitchs;
        } else {
            color = colortab[0];
            if (wfirst) {
                *tmptrg++ = color;
            }
            for (x = 0; x < wstart; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            for (x = 0; x < wfast; x++) {
                tmptrg[0] = color;
                tmptrg[1] = color;
                tmptrg[2] = color;
                tmptrg[3] = color;
                tmptrg[4] = color;
                tmptrg[5] = color;
                tmptrg[6] = color;
                tmptrg[7] = color;
                tmptrg[8] = color;
                tmptrg[9] = color;
                tmptrg[10] = color;
                tmptrg[11] = color;
                tmptrg[12] = color;
                tmptrg[13] = color;
                tmptrg[14] = color;
                tmptrg[15] = color;
                tmptrg += 16;
            }
            for (x = 0; x < wend; x++) {
                *tmptrg++ = color;
                *tmptrg++ = color;
            }
            if (wlast) {
                *tmptrg = color;
            }
            if (y & 1)
                src += pitchs;
        }
        trg += pitcht;
    }
}

