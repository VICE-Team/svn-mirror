/*
 * icon.c - window icon(s) support
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include "archdep.h"
#include "vice_sdl.h"
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "lib.h"
#include "machine.h"


#include "icon.h"

#ifdef USE_SDLUI2

void sdl_ui_set_window_icon(SDL_Window *window)
{
    SDL_Surface *surface;
    char *datadir;
    char *path;
    const char *icon;

    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:
            icon = "C64_256.png";
            break;
        case VICE_MACHINE_C64DTV:
            icon = "DTV_256.png";
            break;
        case VICE_MACHINE_SCPU64:
            icon = "SCPU_256.png";
            break;
        case VICE_MACHINE_C128:
            icon = "C128_256.png";
            break;
        case VICE_MACHINE_PET:
            icon = "PET_256.png";
            break;
        case VICE_MACHINE_VIC20:
            icon = "VIC_256.png";
            break;
        case VICE_MACHINE_CBM5x0:   /* fall through */
        case VICE_MACHINE_CBM6x0:
            icon = "CBM2_256.png";
            break;
        case VICE_MACHINE_PLUS4:
            icon = "Plus4_256.png";
            break;
        case VICE_MACHINE_VSID:
            icon = "VSID_256.png";
            break;
        default:
            icon = NULL;
    }

    datadir = archdep_get_vice_datadir();
    path = archdep_join_paths(datadir, "common", icon, NULL);
    printf("path = %s\n", path);
    lib_free(datadir);

    IMG_Init(IMG_INIT_PNG);
    surface = IMG_Load(path);
    lib_free(path);

    /* The icon is attached to the window pointer */
    SDL_SetWindowIcon(window, surface);

    /* ...and the surface containing the icon pixel data is no longer required. */
    SDL_FreeSurface(surface);
}

#else
void sdl_ui_set_window_icon(void *window)
{
}
#endif
