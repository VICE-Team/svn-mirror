/*
 * render1x2.c - 1x2 renderers (no pal emu needed here)
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

#include "render1x2.h"
#include "types.h"
#include <string.h>

/* 16 color 1x2 renderer */

static inline void render_source_line(uint32_t *tmptrg, const uint8_t *tmpsrc, const uint32_t *colortab,
                                      unsigned int wstart, unsigned int wfast, unsigned int wend)
{
    unsigned int x;
    
    for (x = 0; x < wstart; x++) {
        *tmptrg++ = colortab[*tmpsrc++];
    }
    for (x = 0; x < wfast; x++) {
        tmptrg[0] = colortab[tmpsrc[0]];
        tmptrg[1] = colortab[tmpsrc[1]];
        tmptrg[2] = colortab[tmpsrc[2]];
        tmptrg[3] = colortab[tmpsrc[3]];
        tmptrg[4] = colortab[tmpsrc[4]];
        tmptrg[5] = colortab[tmpsrc[5]];
        tmptrg[6] = colortab[tmpsrc[6]];
        tmptrg[7] = colortab[tmpsrc[7]];
        tmpsrc += 8;
        tmptrg += 8;
    }
    for (x = 0; x < wend; x++) {
        *tmptrg++ = colortab[*tmpsrc++];
    }
}

static inline void render_solid_line(uint32_t *tmptrg, const uint8_t *tmpsrc, const uint32_t color,
                                      unsigned int wstart, unsigned int wfast, unsigned int wend)
{
    unsigned int x;
    
    for (x = 0; x < wstart; x++) {
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
        tmptrg += 8;
    }
    for (x = 0; x < wend; x++) {
        *tmptrg++ = color;
    }
}

void render_32_1x2_04(const video_render_color_tables_t *color_tab, const uint8_t *src, uint8_t *trg,
                      unsigned int width, const unsigned int height,
                      const unsigned int xs, const unsigned int ys,
                      const unsigned int xt, const unsigned int yt,
                      const unsigned int pitchs, const unsigned int pitcht,
                      const unsigned int doublescan, video_render_config_t *config)
{
    const uint32_t *colortab = color_tab->physical_colors;
    const uint8_t *tmpsrc;
    uint32_t *tmptrg;
    uint32_t *blank_line = NULL;
    unsigned int y, wstart, wfast, wend, yys;
    uint32_t color;
    int readable = config->readable;

    src = src + pitchs * ys + xs;
    trg = trg + pitcht * yt + (xt << 2);
    yys = (ys << 1) | (yt & 1);
    if (width < 8) {
        wstart = width;
        wfast = 0;
        wend = 0;
    } else {
        /* alignment: 8 pixels*/
        wstart = (unsigned int)8 - (vice_ptr_to_uint(trg) & 7);
        wfast = (width - wstart) >> 3; /* fast loop for 8 pixel segments*/
        wend = (width - wstart) & 0x07;  /* do not forget the rest*/
    }
    
    for (y = yys; y < (yys + height); y++) {
        tmpsrc = src;
        tmptrg = (uint32_t *)trg;
        
        if (config->interlaced) {
            /*
             * If it's an even line and an even frame, or if it's an odd line
             * and an odd frame, then this line contains new pixels from the video
             * chip. Otherwise it contains a translucent blank line to be alpha
             * blended with the previous frame.
             */
            if ((y & 1) == config->interlace_odd_frame) {
                /* New pixels */
                render_source_line(tmptrg, tmpsrc, colortab, wstart, wfast, wend);
            } else {
                /* Blank line */
                if (blank_line) {
                    /* Copy the first blank line we created */
                    memcpy(tmptrg, blank_line, width * 4);
                } else {
                    /* Next time, memcpy this blank line as it's much faster. */
                    blank_line = tmptrg;
                    
                    /* Create a blank line of color[0], with 50% alpha */
                    color = (colortab[0] & 0x00ffffff) | 0x8000000;
                    render_solid_line(tmptrg, tmpsrc, color, wstart, wfast, wend);
                }
            }
        } else {
            /*
             * Non-interlace code path, supporting doublescan
             */
            if (!(y & 1) || doublescan) {
                if ((y & 1) && readable && y > yys) { /* copy previous line */
                    memcpy(trg, trg - pitcht, width << 2);
                } else {
                    render_source_line(tmptrg, tmpsrc, colortab, wstart, wfast, wend);
                }
            } else {
                if (readable && y > yys + 1) { /* copy 2 lines before */
                    memcpy(trg, trg - pitcht * 2, width << 2);
                } else {
                    color = colortab[0];
                    render_solid_line(tmptrg, tmpsrc, color, wstart, wfast, wend);
                }
            }
        }
        
        if (y & 1) {
            src += pitchs;
        }
        trg += pitcht;
    }
}
