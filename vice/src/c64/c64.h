/*
 * c64.h
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

#ifndef _C64_H
#define _C64_H

#define C64_PAL_CYCLES_PER_SEC	985248
#define C64_PAL_CYCLES_PER_LINE 63
#define C64_PAL_SCREEN_LINES    312
#define C64_PAL_CYCLES_PER_RFSH (C64_PAL_SCREEN_LINES \
                                 * C64_PAL_CYCLES_PER_LINE)

/* PAL refresh rate: 50.123432124542124 */
#define C64_PAL_RFSH_PER_SEC    (1.0 / ((double)C64_PAL_CYCLES_PER_RFSH	\
                                        / (double)C64_PAL_CYCLES_PER_SEC))

#define C64_NTSC_CYCLES_PER_SEC  1022730
#define C64_NTSC_CYCLES_PER_LINE 65
#define C64_NTSC_SCREEN_LINES	 262
#define C64_NTSC_CYCLES_PER_RFSH (C64_NTSC_SCREEN_LINES \
                                  * C64_NTSC_CYCLES_PER_LINE)
#define C64_NTSC_RFSH_PER_SEC    (1.0 / ((double)C64_NTSC_CYCLES_PER_RFSH   \
                                        / (double)C64_NTSC_CYCLES_PER_SEC))

#endif
