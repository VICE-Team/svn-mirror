/*
 * render1x1.h - Implementation of framebuffer to physical screen copy
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _RENDER1X1_H
#define _RENDER1X1_H

#include "types.h"

extern void render_08_1x1_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_16_1x1_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_24_1x1_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_32_1x1_04(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_08_1x1_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_16_1x1_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_24_1x1_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
extern void render_32_1x1_08(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                             int height, int xs, int ys, int xt, int yt,
                             int pitchs, int pitcht);
#endif

