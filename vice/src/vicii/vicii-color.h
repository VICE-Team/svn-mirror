/*
 * vicii-color.h - Colors for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *
 * Research about the YUV values by
 *  Philip Timmermann <pepto@pepto.de>
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

#ifndef _VIC_II_COLOR_H
#define _VIC_II_COLOR_H

#include "palette.h"

#define VIC_II_COLOR_PALETTE_16     1
#define VIC_II_COLOR_PALETTE_256    2
#define VIC_II_COLOR_PALETTE_LOOKUP 3

extern const char *vic_ii_color_names[16];
extern palette_t *vic_ii_color_calcpalette(int type,int saturation,int contrast,int brightness,int gamma,int newlum);

#endif

