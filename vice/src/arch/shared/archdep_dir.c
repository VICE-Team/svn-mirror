/** \file   archdep_dir.c
 * \brief   Read host directory
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 * \author  Andreas Boose <viceteam@t-online.de>
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
#include "archdep.h"

#include <string.h>
#include <errno.h>

#if defined(ARCHDEP_OS_WINDOWS)
/* FIXME:   This is obviously wrong and would require a user of MVSC to install
 *          some kind of drop-in dirent.h compatibility file in their build
 *          environment.
 *          This code should be amended to use FindFirstFile(), FindNextFile()
 *          etcetera from the win32 API on Windows.
 */
# include <dirent.h>
#elif defined(ARCHDEP_OS_UNIX) || defined(ARCHDEP_OS_HAIKU)
# include <sys/types.h>
# include <dirent.h>
#else
# error "Unsupported OS!"
#endif

#include "lib.h"
#include "util.h"

#include "archdep_dir.h"


static int dirs_amount = 0;
static int files_amount = 0;

static int archdep_compare_names(const void* a, const void* b)
{
    const archdep_name_table_t *arg1 = (const archdep_name_table_t*)a;
    const archdep_name_table_t *arg2 = (const archdep_name_table_t*)b;
    return strcmp(arg1->name, arg2->name);
}

/* checks a direntry for the given filter mode, 
   returns 1 if the file is not filtered out, 
           0 when the file is filtered out */
static int archdep_check_dir_filter(struct dirent *dp, int mode)
{
    if (mode & ARCHDEP_OPENDIR_NO_DOTFILES) {
        /* checks for a *nix "dotfile" */
        if ((dp->d_name[0] == '.') &&
            (dp->d_name[1] != 0) &&
            (dp->d_name[1] != '.')) {
            return 0;
        }
    }
    return 1;
}

/*
    NOTE: even when _DIRENT_HAVE_D_TYPE is defined, d_type may still be returned
          as DT_UNKNOWN - in that case we must fall back to using stat instead.
 */
static int archdep_count_dir_items(const char *path, int mode)
{
    DIR *dirp;
    struct dirent *dp;
/* #ifndef _DIRENT_HAVE_D_TYPE */
    size_t len;
    unsigned int isdir;
    char *filename;
    int retval;
/* #endif */

    dirs_amount = 0;
    files_amount = 0;

    dirp = opendir(path);

    if (dirp == NULL) {
        return -1;
    }

    dp = readdir(dirp);

    while (dp != NULL) {
        if (archdep_check_dir_filter(dp, mode)) {
#ifdef _DIRENT_HAVE_D_TYPE
            if (dp->d_type != DT_UNKNOWN) {
                if (dp->d_type == DT_DIR) {
                    dirs_amount++;
#ifdef DT_LNK
                } else if (dp->d_type == DT_LNK) {
                    filename = util_concat(path, FSDEV_DIR_SEP_STR, dp->d_name, NULL);
                    retval = archdep_stat(filename, &len, &isdir);
                    if (retval == 0) {
                        if (isdir) {
                            dirs_amount++;
                        } else {
                            files_amount++;
                        }
                    }
                    if (filename) {
                        lib_free(filename);
                        filename = NULL;
                    }
#endif /* DT_LNK */
                } else {
                    files_amount++;
                }
                dp = readdir(dirp);
            } else {
#endif /* _DIRENT_HAVE_D_TYPE */
                filename = util_concat(path, FSDEV_DIR_SEP_STR, dp->d_name, NULL);
                retval = archdep_stat(filename, &len, &isdir);
                if (retval == 0) {
                    if (isdir) {
                        dirs_amount++;
                    } else {
                        files_amount++;
                    }
                }
                dp = readdir(dirp);
                lib_free(filename);
#ifdef _DIRENT_HAVE_D_TYPE
            }
#endif
        } else {
            dp = readdir(dirp);
        }
    }
    closedir(dirp);
    return 0;
}

static void archdep_filldir(const char *path, archdep_name_table_t *dirs, archdep_name_table_t *files, int mode)
{
    DIR *dirp = NULL;
    struct dirent *dp = NULL;
    int dir_count = 0;
    int file_count = 0;
/* #ifndef _DIRENT_HAVE_D_TYPE */
    size_t len;
    unsigned int isdir;
    char *filename;
    int retval;
/* #endif */

    dirp = opendir(path);

    dp = readdir(dirp);

    while (dp != NULL) {
        if (archdep_check_dir_filter(dp, mode)) {
#ifdef _DIRENT_HAVE_D_TYPE
            if (dp->d_type != DT_UNKNOWN) {
                if (dp->d_type == DT_DIR) {
                    dirs[dir_count].name = lib_strdup(dp->d_name);
                    dir_count++;
#ifdef DT_LNK
                } else if (dp->d_type == DT_LNK) {
                    filename = util_concat(path, FSDEV_DIR_SEP_STR, dp->d_name, NULL);
                    retval = archdep_stat(filename, &len, &isdir);
                    if (retval == 0) {
                        if (isdir) {
                            dirs[dir_count].name = lib_strdup(dp->d_name);
                            dir_count++;
                        } else {
                            files[file_count].name = lib_strdup(dp->d_name);
                            file_count++;
                        }
                    }
                    if (filename) {
                        lib_free(filename);
                        filename = NULL;
                    }
#endif /* DT_LNK */
                } else {
                    files[file_count].name = lib_strdup(dp->d_name);
                    file_count++;
                }
                dp = readdir(dirp);
            } else {
#endif /* _DIRENT_HAVE_D_TYPE */
                filename = util_concat(path, FSDEV_DIR_SEP_STR, dp->d_name, NULL);
                retval = archdep_stat(filename, &len, &isdir);
                if (retval == 0) {
                    if (isdir) {
                        dirs[dir_count].name = lib_strdup(dp->d_name);
                        dir_count++;
                    } else {
                        files[file_count].name = lib_strdup(dp->d_name);
                        file_count++;
                    }
                }
                dp = readdir(dirp);
                lib_free(filename);
#ifdef _DIRENT_HAVE_D_TYPE
            }
#endif
        } else {
            dp = readdir(dirp);
        }
    }
    closedir(dirp);
}

archdep_dir_t *archdep_opendir(const char *path, int mode)
{
    int retval;
    archdep_dir_t *archdep_dir;

    retval = archdep_count_dir_items(path, mode);
    if (retval < 0) {
        return NULL;
    }

    archdep_dir = lib_malloc(sizeof(archdep_dir_t));

    archdep_dir->dirs = lib_malloc(sizeof(archdep_name_table_t) * dirs_amount);
    archdep_dir->files = lib_malloc(sizeof(archdep_name_table_t) * files_amount);

    archdep_filldir(path, archdep_dir->dirs, archdep_dir->files, mode);
    qsort(archdep_dir->dirs, dirs_amount, sizeof(archdep_name_table_t), archdep_compare_names);
    qsort(archdep_dir->files, files_amount, sizeof(archdep_name_table_t), archdep_compare_names);

    archdep_dir->dir_amount = dirs_amount;
    archdep_dir->file_amount = files_amount;
    archdep_dir->counter = 0;

    return archdep_dir;
}

char *archdep_readdir(archdep_dir_t *archdep_dir)
{
    char *retval;

    if (archdep_dir->counter >= (archdep_dir->dir_amount + archdep_dir->file_amount)) {
        return NULL;
    }

    if (archdep_dir->counter >= archdep_dir->dir_amount) {
        retval = archdep_dir->files[archdep_dir->counter - archdep_dir->dir_amount].name;
    } else {
        retval = archdep_dir->dirs[archdep_dir->counter].name;
    }
    archdep_dir->counter++;

    return retval;
}

void archdep_closedir(archdep_dir_t *archdep_dir)
{
    int i;

    for (i = 0; i < archdep_dir->dir_amount; i++) {
        lib_free(archdep_dir->dirs[i].name);
    }
    for (i = 0; i < archdep_dir->file_amount; i++) {
        lib_free(archdep_dir->files[i].name);
    }
    lib_free(archdep_dir->dirs);
    lib_free(archdep_dir->files);
    lib_free(archdep_dir);
}

void archdep_resetdir(archdep_dir_t *archdep_dir)
{
    archdep_dir->counter = 0;
}

void archdep_setdirpos(archdep_dir_t *archdep_dir, int pos)
{
    archdep_dir->counter = pos;
}

int archdep_getdirpos(archdep_dir_t *archdep_dir)
{
    return archdep_dir->counter;
}

