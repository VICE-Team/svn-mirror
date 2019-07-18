/** \file   archdep_open_default_log_file.c
 * \brief   Open default log file
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
#include "archdep_join_paths.h"
#include "archdep_user_config_path.h"
#include "lib.h"
#include "log.h"
#ifdef UNIX_COMPILE
#include "unistd.h"
#endif
#include "archdep_open_default_log_file.h"

/* amiga */
#if 0
FILE *archdep_open_default_log_file(void)
{
    if (run_from_wb) {
        char *fname;
        FILE *f;

        fname = util_concat(archdep_boot_path(), "vice.log", NULL);
        f = fopen(fname, MODE_WRITE_TEXT);

        lib_free(fname);

        if (f == NULL) {
            return stdout;
        }

        return f;
    } else {
        return stdout;
    }
}
#endif

/* beos */
#if 0
FILE *archdep_open_default_log_file(void)
{
    char *fname;
    FILE *f;

    fname = util_concat(archdep_boot_path(), "/vice.log", NULL);
    f = fopen(fname, "wt");
    lib_free(fname);

    return f;
}
#endif

/* os2 */
#if 0
FILE *archdep_open_default_log_file(void)
{
    char *fname;
    FILE *f;

    fname = util_concat(archdep_boot_path(), "\\vice.log", NULL);
    f = fopen(fname, "wt");
    lib_free(fname);

    return f;
}
#endif


/** \brief  Opens the default log file. On *nix the log goes to stdout by
 *          default. If that does not exist, attempt to open a log file in 
 *          the user's vice config dir. If the file cannot be opened for some 
 *          reason, stdout is returned anyway.
 *
 * \return  file pointer to log file
 */
FILE *archdep_open_default_log_file(void)
{
    FILE *fp = stdout;
    char *path;

    /* quick fix. on non windows platforms this should check if VICE has been
       started from a terminal, and only if not open a file instead of stdout */
#ifdef UNIX_COMPILE
    if (!isatty(fileno(fp))) {
#endif
        path = archdep_join_paths(archdep_user_config_path(), "vice.log", NULL);
        fp = fopen(path, "w");
        if (fp == NULL) {
            log_error(LOG_ERR,
                    "failed to open log file '%s' for writing, reverting to stdout",
                    path);
            fp = stdout;
        }
        lib_free(path);
#ifdef UNIX_COMPILE
    }
#endif
    return fp;
}
