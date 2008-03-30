/*
 * pet.h
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

#ifndef _PET_H
#define _PET_H

/* Except for the exact CYCLES_PER_SEC those are only reasonable defaults.
   They get overwritten when writing to the CRTC */

#define PET_PAL_CYCLES_PER_SEC	1000000
#define PET_PAL_CYCLES_PER_LINE 71
#define PET_PAL_SCREEN_LINES    312
#define PET_PAL_CYCLES_PER_RFSH (PET_PAL_SCREEN_LINES \
                                 * PET_PAL_CYCLES_PER_LINE)
#define PET_PAL_RFSH_PER_SEC    (1.0 / ((double)PET_PAL_CYCLES_PER_RFSH	\
                                        / (double)PET_PAL_CYCLES_PER_SEC))

#define PET_NTSC_CYCLES_PER_SEC  1000000
/*
#define PET_NTSC_CYCLES_PER_LINE 71
#define PET_NTSC_SCREEN_LINES	 261
#define PET_NTSC_CYCLES_PER_RFSH (PET_NTSC_SCREEN_LINES \
                                  * PET_NTSC_CYCLES_PER_LINE)
#define PET_NTSC_RFSH_PER_SEC    (1.0 / ((double)PET_NTSC_CYCLES_PER_RFSH   \
                                        / (double)PET_NTSC_CYCLES_PER_SEC))
*/

#endif
