/*
 * vic20.h
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _VIC20_H
#define _VIC20_H

#define VIC20_PAL_CYCLES_PER_SEC  1108405
#define VIC20_PAL_CYCLES_PER_LINE 71
#define VIC20_PAL_SCREEN_LINES    312
#define VIC20_PAL_CYCLES_PER_RFSH (VIC20_PAL_SCREEN_LINES \
                                 * VIC20_PAL_CYCLES_PER_LINE)
#define VIC20_PAL_RFSH_PER_SEC    (1.0 / ((double)VIC20_PAL_CYCLES_PER_RFSH  \
                                        / (double)VIC20_PAL_CYCLES_PER_SEC))

#define VIC20_NTSC_CYCLES_PER_SEC  1022727
#define VIC20_NTSC_CYCLES_PER_LINE 65
#define VIC20_NTSC_SCREEN_LINES	 261
#define VIC20_NTSC_CYCLES_PER_RFSH (VIC20_NTSC_SCREEN_LINES \
                                  * VIC20_NTSC_CYCLES_PER_LINE)
#define VIC20_NTSC_RFSH_PER_SEC    (1.0 / ((double)VIC20_NTSC_CYCLES_PER_RFSH \
                                        / (double)VIC20_NTSC_CYCLES_PER_SEC))

#endif
