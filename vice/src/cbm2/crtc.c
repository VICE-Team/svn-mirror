/* -*- C -*-
 *
 * crtc.def - Definitions for CRTC emulation in the CBM-II
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#define	_CRTC_C

#define MY_WINDOW_TITLE            "VICE: CBM-II emulator"

#include "config.h"
#include "types.h"
#include "snapshot.h"
#include "c610tpi.h"
#include "crtc.h"

static inline void do_update_memory_ptrs(void);

#define	SIGNAL_VERT_BLANK_OFF	tpi1_set_int(0, 1);

#define	SIGNAL_VERT_BLANK_ON	tpi1_set_int(0, 0);

#include "crtccore.c"

static inline void do_update_memory_ptrs(void)
{
    if (!(scraddr & 0x1000)) {
        video_mode = CRTC_STANDARD_MODE;
    } else {
        video_mode = CRTC_REVERSE_MODE;
    }
    chargen_rel = (chargen_rel & ~0x1000) | ((scraddr & 0x800) ? 0x1000 : 0);
}

