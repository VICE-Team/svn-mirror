/*
 * archdep.h - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_ARCHDEP_H
#define VICE_ARCHDEP_H

#include "vice.h"
#include "vice_sdl.h"

#include "sound.h"

#ifndef BEOS_COMPILE
/* Video chip scaling.  */
#define ARCHDEP_VICII_DSIZE   1
#define ARCHDEP_VICII_DSCAN   1
#define ARCHDEP_VDC_DSIZE     1
#define ARCHDEP_VDC_DSCAN     1
#define ARCHDEP_VIC_DSIZE     1
#define ARCHDEP_VIC_DSCAN     1
#define ARCHDEP_CRTC_DSIZE    1
#define ARCHDEP_CRTC_DSCAN    1
#define ARCHDEP_TED_DSIZE     1
#define ARCHDEP_TED_DSCAN     1
#endif

/* Video chip double buffering.  */
#define ARCHDEP_VICII_DBUF 0
#define ARCHDEP_VDC_DBUF   0
#define ARCHDEP_VIC_DBUF   0
#define ARCHDEP_CRTC_DBUF  0
#define ARCHDEP_TED_DBUF   0

/* No key symcode.  */
#define ARCHDEP_KEYBOARD_SYM_NONE SDLK_UNKNOWN

/* Default sound output mode */
#define ARCHDEP_SOUND_OUTPUT_MODE SOUND_OUTPUT_SYSTEM

/* define if the platform supports the monitor in a seperate window */
/* #define ARCHDEP_SEPERATE_MONITOR_WINDOW */

/* FIXME: Ugly hack for preventing SDL crash using -help */
extern int sdl_help_shutdown;

/******************************************************************************/

#ifdef AMIGA_SUPPORT
/* FIXME: naming? */
extern int load_libs(void);
extern void close_libs(void);

#include "archdep_amiga.h"
/* This platform supports choosing drives. */
#define SDL_CHOOSE_DRIVES
#endif

#ifdef BEOS_COMPILE
#include "archdep_beos.h"
#endif

#ifdef __OS2__
#include "archdep_os2.h"
/* This platform supports choosing drives. */
#define SDL_CHOOSE_DRIVES
#endif

#if defined(UNIX_COMPILE) && !defined(CEGCC_COMPILE)
#include "archdep_unix.h"
/* Allow native monitor code (on host console) */
#define ALLOW_NATIVE_MONITOR
#endif

#ifdef WIN32_COMPILE
#include "archdep_win32.h"
/* This platform supports choosing drives. */
#define SDL_CHOOSE_DRIVES
#endif

#endif
