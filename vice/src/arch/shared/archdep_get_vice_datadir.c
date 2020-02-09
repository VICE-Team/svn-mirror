/** \file   archdep_get_vice_datadir.c
 * \brief   Get path to data dir for Gtk3
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

#include "archdep_defs.h"
#include "archdep_boot_path.h"
#include "archdep_join_paths.h"
#include "archdep_user_config_path.h"

#include "archdep_get_vice_datadir.h"


/** \brief  Get the absolute path to the VICE data directory
 *
 * \return  Path to VICE data directory (typically /usr/local/share/vice)
 */
char *archdep_get_vice_datadir(void)
{
    char *path;
    
#ifdef ARCHDEP_OS_WINDOWS
    path = archdep_boot_path();
# else
    path = VICE_DATADIR;
# endif

    printf("VICE_DATADIR = '%s'\n", path);

    return path;
}
