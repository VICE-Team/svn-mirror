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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep_get_vice_datadir.h"
#include "archdep_join_paths.h"
#include "machine.h"
#include "lib.h"

#include "archdep_icon_path.h"


/** \brief  Get path to emu-specific PNG icon of \a size pixels
 *
 * \param[in]   size    size in pixels (square)
 *
 * \return  path to PNG icon, free with lib_free()
 */
char *archdep_app_icon_path_png(int size)
{
    char buffer[1024];
    char *datadir = archdep_get_vice_datadir();
    char *path;

    /* TODO: rename icons to lower case and thus turn machine_name into lower
     *       case before generating the final path.
     */
    if (machine_class == VICE_MACHINE_VSID) {
        /* VSID is a special case */
        snprintf(buffer, sizeof(buffer), "SID_%d.png", size);
    } else {
        snprintf(buffer, sizeof(buffer), "%s_%d.png", machine_name, size);
    }
    path = archdep_join_paths(datadir, "common", buffer, NULL);
    lib_free(datadir);
    return path;
}
