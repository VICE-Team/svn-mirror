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

#ifdef HAVE_DIRECT_H
#include <direct.h>
#endif
#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_IO_H
#include <io.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
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
#ifdef __OS2__
#include "snippets/dirport.h"
#endif

#include "archdep.h"
#include "ioutil.h"
#include "lib.h"
#include "types.h"


/* Mostly POSIX compatibily */

int ioutil_access(const char *pathname, int mode)
{
    int access_mode = 0;

    if ((mode & IOUTIL_ACCESS_R_OK) == IOUTIL_ACCESS_R_OK)
        access_mode |= ARCHDEP_R_OK;
    if ((mode & IOUTIL_ACCESS_W_OK) == IOUTIL_ACCESS_W_OK)
        access_mode |= ARCHDEP_W_OK;
    if ((mode & IOUTIL_ACCESS_X_OK) == IOUTIL_ACCESS_X_OK)
        access_mode |= ARCHDEP_X_OK;
    if ((mode & IOUTIL_ACCESS_F_OK) == IOUTIL_ACCESS_F_OK)
        access_mode |= ARCHDEP_F_OK;

    return access(pathname, access_mode);
}

int ioutil_chdir(const char *path)
{
    return chdir((char*)path);
}

int ioutil_errno(unsigned int check)
{
    switch (check) {
#ifndef __OS2__
      case IOUTIL_ERRNO_EPERM:
        if (errno == EPERM)
            return 1;
        break;
#endif
      case IOUTIL_ERRNO_EEXIST:
        if (errno == EEXIST)
            return 1;
        break;
      case IOUTIL_ERRNO_EACCES:
        if (errno == EACCES)
            return 1;
        break;
      case IOUTIL_ERRNO_ENOENT:
        if (errno == ENOENT)
            return 1;
        break;
#ifndef __OS2__
      case IOUTIL_ERRNO_ERANGE:
        if (errno == ERANGE)
            return 1;
        break;
#endif
      default:
        return 0;
    }

    return 0;
}

char *ioutil_getcwd(char *buf, int size)
{
    return getcwd(buf, (size_t)size);
}

int ioutil_isatty(int desc)
{
    return isatty(desc);
}

#ifndef PATH_MAX
#ifdef MAX_PATH
#define PATH_MAX MAX_PATH
#else
#define PATH_MAX 1024
#endif
#endif

int ioutil_maxpathlen(void)
{
    return PATH_MAX;
}

int ioutil_mkdir(const char *pathname, int mode)
{
    return archdep_mkdir(pathname, mode);
}

int ioutil_remove(const char *name)
{
    return unlink(name);
}

int ioutil_rename(const char *oldpath, const char *newpath)
{
    return rename(oldpath, newpath);
}

int ioutil_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
    return archdep_stat(file_name, len, isdir);
}

/* ------------------------------------------------------------------------- */
/* IO helper functions.  */
char *ioutil_current_dir(void)
{
#ifdef __riscos
    return GetCurrentDirectory();
#else
    static size_t len = 128;
    char *p = (char *)lib_malloc(len);

    while (getcwd(p, len) == NULL) {
        if (errno == ERANGE) {
            len *= 2;
            p = (char *)lib_realloc(p, len);
        } else
            return NULL;
    }

    return p;
#endif
}

struct ioutil_dir_s {
    DIR *dp;
};
typedef struct ioutil_dir_s ioutil_dir_t;

ioutil_dir_t *ioutil_opendir(const char *path)
{
    DIR *dp;
    ioutil_dir_t *ioutil_dir;

    dp = opendir((char*)path);

    if (dp == NULL)
        return NULL;

    ioutil_dir = (ioutil_dir_t *)lib_malloc(sizeof(ioutil_dir_t));

    ioutil_dir->dp = dp;

    return ioutil_dir;
}

char *ioutil_readdir(ioutil_dir_t *ioutil_dir)
{
    struct dirent *dirp;

    dirp = readdir(ioutil_dir->dp);

    if (dirp == NULL)
        return NULL;

    return (char *)(dirp->d_name);
}

void ioutil_closedir(ioutil_dir_t *ioutil_dir)
{
    closedir(ioutil_dir->dp);
    lib_free(ioutil_dir);
}

