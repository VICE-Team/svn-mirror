/** \file   archdep_user_config_path.c
 * \brief   Retrieve path to the user's configuration directory
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * Get path to user's VICE configuration directory, this is where the vice
 * files like vicerc/vice.ini, vice.log, autostart-$emu.d64 will be.
 *
 * OS support:
 *  - Linux
 *  - Windows
 *  - MacOS
 *  - BeOS/Haiku (untested)
 *  - AmigaOS (untested)
 *  - OS/2 (untested)
 *  - MS-DOS (untested)
 *
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
#include "archdep_defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "log.h"
#include "archdep_boot_path.h"
#include "archdep_home_path.h"
#include "archdep_join_paths.h"

#include "archdep_user_config_path.h"


/** \brief  Get path to the user's VICE configuration directory
 *
 * On systems supporting home directories this will return a directory inside
 * the home directory, depending on OS:
 *
 * - Windows: $HOME\AppData\Roaming\vice
 * - Unix: $HOME/.config/vice
 * - BeOS: $HOME/.config/vice
 *
 * On other systems the path to the executable is returned.
 *
 * \return  path to VICE config directory, free with lib_free()
 */
char *archdep_user_config_path(void)
{
    char *path;

#if defined(UNIX_COMPILE) || defined(WIN32_COMPILE) || defined(BEOS_COMPILE)
    const char *home_path = archdep_home_path();

# ifdef WIN32_COMPILE
    path = archdep_join_paths(home_path, "AppData", "Roaming", "vice", NULL);
# else
    path = archdep_join_paths(home_path, ".config", "vice", NULL);
# endif

#else
    const char *boot_path = archdep_boot_path();
    path = lib_stralloc(boot_path);
#endif
    printf("%s(): USER_CONFIG_PATH = '%s'\n", __func__, path);
    return path;
}
