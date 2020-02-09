/** \file   archdep_get_vice_docsdir.c
 * \brief   Get path to VICE doc/ dir
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
#include "config.h"

#include <stdlib.h>

#include "archdep_boot_path.h"
#include "archdep_defs.h"
#include "archdep_is_macos_bindist.h"
#include "archdep_join_paths.h"
#include "lib.h"

#include "archdep_get_vice_docsdir.h"


/** \brief  Get path to VICE doc dir
 *
 * \return  heap-allocated string, to be freed after use with lib_free()
 */
char *archdep_get_vice_docsdir(void)
{
    char *path;
    printf("VICE_DOCDIR = '%s'\n", VICE_DOCDIR);

#ifdef ARCHDEP_OS_WINDOWS
    /* Cannot use VICE_DOCDIR here since Windows installs assume any file to
     * be relative to the emu binary.
     */
    path = archdep_join_paths(archdep_boot_path(), "doc", NULL);
#else
# ifdef ARCHDEP_OS_OSX
    if (archdep_is_macos_bindist()) {
        path = archdep_join_paths(archdep_boot_path(), "..", "share", "vice", "doc", NULL);
    } else {
        path = lib_strdup(VICE_DOCDIR);
    }
# else
    path = lib_strdup(VICE_DOCDIR);
# endif
#endif

    return path;
}
