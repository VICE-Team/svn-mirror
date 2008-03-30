/*
 * ioutil.c - Miscellaneous IO utility functions.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdio.h>

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

#include "utils.h"


/* Mostly POSIX compatibily */

int ioutil_chdir(const char *path)
{
    return chdir(path);
}

int ioutil_mkdir(const char *pathname, int mode)
{
    return mkdir(pathname, (mode_t)mode);
}

int ioutil_remove(const char *name)
{
    return unlink(name);
}

int ioutil_rename(const char *oldpath, const char *newpath)
{
    return rename(oldpath, newpath);
}

/* ------------------------------------------------------------------------- */
/* IO helper functions.  */
char *ioutil_current_dir(void)
{
#ifdef __riscos
    return GetCurrentDirectory();
#else
    static size_t len = 128;
    char *p = (char *)xmalloc(len);

    while (getcwd(p, len) == NULL) {
        if (errno == ERANGE) {
            len *= 2;
            p = (char *)xrealloc(p, len);
        } else
            return NULL;
    }

    return p;
#endif
}

