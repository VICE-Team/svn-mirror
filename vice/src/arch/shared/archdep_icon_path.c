/** \file   archdep_icon_path.c
 * \brief   Get path to icon file
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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

#include <stddef.h>
#include <string.h>

#include "machine.h"
#include "lib.h"
#include "sysfile.h"

#include "archdep_icon_path.h"


/** \brief  Get path to emu-specific PNG icon of \a size pixels
 *
 * \param[in]   size    size in pixels (square)
 *
 * \return  path to PNG icon
 *
 * \note    free with lib_free() after use.
 */
char *archdep_app_icon_path_png(int size)
{
    char        buffer[256];
    char       *path = NULL;
    const char *icon;

    /*
     * This sucks, but will have to do until I regenerate/repixel the icons
     */
    /* printf("MACHINE_NAME = %s\n", machine_name); */
    switch (machine_class) {
        case VICE_MACHINE_C64:      /* fall through */
        case VICE_MACHINE_C64SC:
            icon = "x64";
            break;
        case VICE_MACHINE_C64DTV:
            icon = "x64dtv";
            break;
        case VICE_MACHINE_SCPU64:
            icon = "xscpu64";
            break;
        case VICE_MACHINE_C128:
            icon = "x128";
            break;
        case VICE_MACHINE_PET:
            icon = "xpet";
            break;
        case VICE_MACHINE_VIC20:
            icon = "xvic";
            break;
        case VICE_MACHINE_CBM5x0:   /* fall through */
        case VICE_MACHINE_CBM6x0:
            icon = "xcbm2";
            break;
        case VICE_MACHINE_PLUS4:
            icon = "xplus4";
            break;
        case VICE_MACHINE_VSID:
            icon = "vsid";
            break;
        default:
            icon = "error";
    }

    snprintf(buffer, sizeof buffer, "vice-%s_%d.png", icon, size);
    buffer[sizeof buffer - 1u] = '\0';
    sysfile_locate(buffer, "common", &path);
    return path;
}
