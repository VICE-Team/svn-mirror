/*
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <sys/utsname.h>

#include "ui.h"

#ifdef HAVE_DIR_H
#include <dir.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef __HAIKU__
#include <sys/wait.h>
#endif

#include "archdep.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "util.h"


static char *orig_workdir;
static char *argv0 = NULL;

static int archdep_init_extra(int *argc, char **argv)
{
    argv0 = lib_strdup(argv[0]);
    orig_workdir = getcwd(NULL, PATH_MAX);

    return 0;
}


#if 0
/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    char *tmp;

    tmp = util_concat(fname, NULL);
    tmp[strlen(tmp) - 1] = '~';
    return tmp;
}
#endif


#if 0
char *archdep_default_autostart_disk_image_file_name(void)
{
    const char *home;

    home = archdep_boot_path();
    return util_concat(home, "/autostart-", machine_get_name(), ".d64", NULL);
}
#endif

char *archdep_default_hotkey_file_name(void)
{
    static char *fname;

    lib_free(fname);
    fname = util_concat(archdep_boot_path(), "/sdl-hotkey-", machine_get_name(), ".vkm", NULL);
    return fname;
}

char *archdep_default_joymap_file_name(void)
{
    static char *fname;

    lib_free(fname);
    fname = util_concat(archdep_boot_path(), "/sdl-joymap-", machine_get_name(), ".vjm", NULL);
    return fname;
}

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




#if 0
char *archdep_tmpnam(void)
{
    return lib_strdup(tmpnam(NULL));
}
#endif



int archdep_require_vkbd(void)
{
    return 0;
}


static void archdep_shutdown_extra(void)
{
    lib_free(argv0);
}

/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a black (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages/actual detection
*/
int kbd_arch_get_host_mapping(void)
{
    return KBD_MAPPING_US;
}


/* This check is needed for haiku, since it always returns 1 on
   SupportsWindowMode() */
int CheckForHaiku(void)
{
    struct utsname name;

    uname(&name);
    if (!strncasecmp(name.sysname, "Haiku", 5)) {
        return -1;
    }
    return 0;
}
