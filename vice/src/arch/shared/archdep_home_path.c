/** \file   archdep_home_path.c
 * \brief   Retrieve home directory of current user
 * \author  Bas Wassink <b.wassink@ziggo.nl>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "log.h"

#ifdef AMIGA_SUPPORT
/* some includes */
#endif

/* for readlink(2) */
#ifdef UNIX_COMPILE
# #include "pwd.h"
#endif

#ifdef MACOSX_SUPPORT
# include <libproc.h>
#endif

/* for GetModuleFileName() */
#ifdef WIN32_COMPILE
# include "windows.h"
# include "userenv.h"
#endif

#include "archdep_home_path.h"


static char *home_dir = NULL;


/** \brief  Get user's home directory
 *
 * Free memory used on emulator exit with archdep_home_path_free()
 *
 * \return  user's home directory
 */
char *archdep_home_path(void)
{
#ifdef UNIX_COMPILE
    char *home;
#endif

    if (home_dir != NULL) {
        return home_dir;
    }

#ifdef UNIX_COMPILE
    home = getenv("HOME");
    if (home == NULL) {
        struct passwd *pwd;

        pwd = getpwduid(getuid());
        if (pwd == NULL) {
            home = ".";
        } else {
            home = pwd->pw_dir;
        }
    }
    home_dir = lib_stralloc(home);
#elif defined(WIN32_COMPILE)
    HANDLE token_handle;
    DWORD bufsize = 4096;
    LPDWORD lpcchSize = &bufsize;
    DWORD err;



    /* get process token handle, whatever the hell that means */
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ALL_ACCESS,
                          &token_handle)) {
        err = GetLastError();
        printf("failed to get process token: 0x%lx.\n", err);
        home_dir = lib_stralloc(".");
        return home_dir;
    }

    /* now get the user profile directory with more weird garbage */
    home_dir = lib_calloc(bufsize, 1);
    if (!GetUserProfileDirectoryA(token_handle,
                                  home_dir,
                                  lpcchSize)) {
        /* error */
        err = GetLastError();
        printf("failed to get user profile root directory: 0x%lx.\n");
        /* set home dir to "." */
        home_dir[0] = '.';
        home_dir[1] = '\0';
    }
    return home_dir;
#endif
}


/** \brief  Free memory used by the home path
 */
void archdep_home_path_free(void)
{
    if (home_dir != NULL) {
        lib_free(home_dir);
        home_dir = NULL;
    }
}
