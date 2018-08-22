/** \file   archdep_default_sysfile_pathlist.c
 * \brief   Get a list of paths of required data files
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

#include "archdep_boot_path.h"
#include "archdep_join_paths.h"
#include "archdep_user_config_path.h"


#include "archdep_default_sysfile_pathlist.h"


#define TOTAL_PATHS 16


static char *sysfile_path = NULL;

#if 0

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    const char *boot_path = archdep_boot_path();
    const char *config_path = archdep_user_config_path();

    char *lib_machine_roms;
    char *lib_drive_roms;
    char *lib_printer_roms;
    char *usr_machine_roms;
    char *usr_drive_roms;
    char *usr_printer_roms;

    char *paths[TOTAL_PATHS + 1];


    if (sysfile_path != NULL) {
        return sysfile_path;
    }

    /* zero out the array of paths to join later */
    for (int i = 0; i <= TOTAL_PATHS; i++) {
        paths[i] = NULL;
    }

    /* lib roms */
    lib_machine_roms = archdep_join_paths(LIBDIR, emu_id, NULL);
    lib_drive_roms = archdep_join_paths(LIBDIR, emu_id, NULL);


    return NULL;
}
#endif
