/*
 * sysdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <dir.h>
#include <unistd.h>

#include "fcntl.h"
#include "findpath.h"
#include "sysdep.h"

#include "utils.h"

static char *orig_workdir;
static char *argv0;

static void restore_workdir(void)
{
    if (orig_workdir)
	chdir(orig_workdir);
}

int sysdep_startup(int *argc, char **argv)
{
    _fmode = O_BINARY;

    argv0 = stralloc(argv[0]);

    orig_workdir = getcwd(NULL, GET_PATH_MAX);
    atexit(restore_workdir);

    return 0;
}

const char *sysdep_boot_path(void)
{
    static char *boot_path;

    if (boot_path == NULL) {
        fname_split(argv0, &boot_path, NULL);

        /* This should not happen, but you never know...  */
        if (boot_path == NULL)
            boot_path = stralloc("./");
    }

    return boot_path;
}

const char *sysdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path = sysdep_boot_path();

        default_path = concat(boot_path, "/", emu_id,
                              FINDPATH_SEPARATOR_STRING,
                              boot_path, "/DRIVES", NULL);
    }

    return default_path;
}

const char *sysdep_default_resource_file_name(void)
{
    static char *fname;
    char *home;

    if (fname != NULL)
        free(fname);

    fname = concat(sysdep_boot_path(), "\\vicerc", NULL);
    return fname;
}

FILE *sysdep_open_default_log_file(void)
{
    char *fname;
    FILE *f;

    fname = concat(sysdep_boot_path(), "\\vice.log", NULL);
    f = fopen(fname, "wt");
    free(fname);

    return f;
}
