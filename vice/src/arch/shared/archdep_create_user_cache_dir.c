/** \file   archdep_create_user_cache_dir.c
 * \brief   Create XDG user cache dir
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
#include "archdep_defs.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "archdep_exit.h"
#include "archdep_home_path.h"
#include "archdep_user_cache_path.h"
#include "archdep_mkdir.h"
#include "lib.h"
#include "log.h"
#include "util.h"

#include "archdep_create_user_cache_dir.h"


/** \brief  Create user cache dir
 *
 * Create the XDG Base Directory Specification <tt>~/.cache/vice</tt> directory.
 */
void archdep_create_user_cache_dir(void)
{
    const char *cache = archdep_user_cache_path();

#if defined(UNIX_COMPILE) || defined(HAIKU_COMPILE)
    const char *home = archdep_home_path();
    char *tmp;

    /*
     * Brute force create XDG ~/.cache dir
     * Some systems without X11 don't have ~/.cache, which makes sense since
     * XDG is a Freedesktop spec. We use it however for vicerc and other files
     * and it expected to be there
     */
    tmp = util_join_paths(home, ARCHDEP_XDG_CACHE_HOME, NULL);
    archdep_mkdir(tmp, 0755);
    errno = 0;
    lib_free(tmp);
#endif

    if (archdep_mkdir(cache, 0755) == 0) {
        return;     /* we created the dir */
    } else if (errno != EEXIST) {
        log_error(LOG_DEFAULT, "failed to create user cache dir '%s': %d: %s.",
                cache, errno, strerror(errno));
        archdep_vice_exit(1);
    }
}
