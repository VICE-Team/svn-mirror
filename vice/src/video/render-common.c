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

#include "render-common.h"
#include "types.h"

inline void render_source_line(uint32_t *tmptrg, const uint8_t *tmpsrc, const uint32_t *colortab,
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

inline void render_solid_line(uint32_t *tmptrg, const uint8_t *tmpsrc, const uint32_t color,
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
