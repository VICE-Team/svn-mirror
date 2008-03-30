/*
 * c610.h
 *
 * Written by
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _C610_H
#define _C610_H


/* except for the exact CYCLES_PER_SEC those values are reasonable default
   values. they get overwritten when writing to the CRTC */

#define C610_PAL_CYCLES_PER_SEC		2000000
#define C610_PAL_CYCLES_PER_LINE 	128
#define C610_PAL_SCREEN_LINES    	312
#define C610_PAL_CYCLES_PER_RFSH (C610_PAL_SCREEN_LINES \
                                 * C610_PAL_CYCLES_PER_LINE)
#define C610_PAL_RFSH_PER_SEC    (1.0 / ((double)C610_PAL_CYCLES_PER_RFSH    \
                                        / (double)C610_PAL_CYCLES_PER_SEC))

#define C610_NTSC_CYCLES_PER_SEC  	C610_PAL_CYCLES_PER_SEC
/*
#define C610_NTSC_CYCLES_PER_LINE 	C610_PAL_CYCLES_PER_LINE
#define C610_NTSC_SCREEN_LINES	 	C610_PAL_SCREEN_LINES
#define C610_NTSC_CYCLES_PER_RFSH 	C610_PAL_CYCLES_PER_RFSH
#define C610_NTSC_RFSH_PER_SEC		C610_PAL_RFSH_PER_SEC
*/

#endif
