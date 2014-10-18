/*
 * archdep.c - Miscellaneous system-specific stuff.
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

#include "vice.h"

#include "vice_sdl.h"
#include <stdio.h>

#ifdef AMIGA_SUPPORT
#include "archdep_amiga.c"
#endif

#ifdef BEOS_COMPILE
#include "archdep_beos.c"
#endif

#if defined(UNIX_COMPILE) && !defined(CEGCC_COMPILE)
#include "archdep_unix.c"
#endif

#if defined(WIN32_COMPILE) && !defined(__XBOX__)
#include "archdep_win32.c"
#endif

#ifdef __XBOX__
#include "archdep_xbox.c"
#endif

#ifdef CEGCC_COMPILE
#include "archdep_cegcc.c"
#endif

#ifdef DINGOO_NATIVE
#include "archdep_dingoo.c"
#endif

int archdep_init(int *argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        return 1;
    }

    return archdep_init_extra(argc, argv);
}

void archdep_shutdown(void)
{
    SDL_Quit();
    archdep_shutdown_extra();
}
