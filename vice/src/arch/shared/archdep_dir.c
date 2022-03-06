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
#include <stdbool.h>

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


/** \brief  Comparision function for qsort() in archdep_opendir()
 *
 * \param[in]   a   first item
 * \param[in]   b   second item
 *
 * \return  \< 0 if \a a \< \a b, 0 if \a a == \a b, \> 0 if \a a \> \a b
 */ 
static int compare_names(const void* a, const void* b)
{
    const archdep_name_table_t *arg1 = (const archdep_name_table_t*)a;
    const archdep_name_table_t *arg2 = (const archdep_name_table_t*)b;
    return strcmp(arg1->name, arg2->name);
}


/** \brief  Check if an entry must be shown
 *
 * Check if \a name matches the \a mode filter.
 *
 * Currently checks if a dotfile must be shown according to \a mode.
 *
 * \return  true if the file or directory must be shown
 */
static bool must_show_entry(const char *name, int mode)
{
    if (mode & ARCHDEP_OPENDIR_NO_DOTFILES) {
        /* checks for a *nix "dotfile" */
        if ((name[0] == '.') &&
            (name[1] != 0) &&
            (name[1] != '.')) {
            return false;
        }
    }
    return true;
}


/** \brief  Count the number of directories and files in a given directory
 *
 * Count the number of directories and files in \a path, filtered according to
 * \a mode, storing the results in \a num_dir and \a num_files.
 *
 * \param[in]   path        directory to scan
 * \param[in]   mode        filter to apply
 * \param[out]  num_dirs    number of directories found
 * \param[out]  num_files   number of files found
 *
 * \return  0 on success, -1 on failure
 *
 */
static int count_dir_items(const char *path, int mode, int *num_dirs, int *num_files)
{
    DIR *dirp;
    struct dirent *dp;
    int dirs_amount = 0;
    int files_amount = 0;

    dirp = opendir(path);
    if (dirp == NULL) {
        *num_dirs = 0;
        *num_files = 0;
        return -1;
    }

    dp = readdir(dirp);
    while (dp != NULL) {
        if (must_show_entry(dp->d_name, mode)) {
            char *filename = NULL;
            unsigned int isdir;
            size_t len;

    /* NOTE: even when _DIRENT_HAVE_D_TYPE is defined, d_type may still be
     *       returned as DT_UNKNOWN - in that case we must fall back to using
     *       stat instead.
     */
#ifdef _DIRENT_HAVE_D_TYPE
            if (dp->d_type != DT_UNKNOWN) {
                if (dp->d_type == DT_DIR) {
                    dirs_amount++;
#ifdef DT_LNK
                } else if (dp->d_type == DT_LNK) {
                    filename = util_concat(path, FSDEV_DIR_SEP_STR, dp->d_name, NULL);
                    if (archdep_stat(filename, &len, &isdir) == 0) {
                        if (isdir) {
                            dirs_amount++;
                        } else {
                            files_amount++;
                        }
                    }
#endif /* DT_LNK */
                } else {
                    files_amount++;
                }
            } else {
#endif /* _DIRENT_HAVE_D_TYPE */
                filename = util_concat(path, FSDEV_DIR_SEP_STR, dp->d_name, NULL);
                if (archdep_stat(filename, &len, &isdir) == 0) {
                    if (isdir) {
                        dirs_amount++;
                    } else {
                        files_amount++;
                    }
                }
#ifdef _DIRENT_HAVE_D_TYPE
            }
#endif
            if (filename != NULL) {
                lib_free(filename);
            }
        }
        dp = readdir(dirp);
    }

    closedir(dirp);
    *num_dirs = dirs_amount;
    *num_files = files_amount;
    return 0;
}


/** \brief  Populate the directory and file lists
 *
 * Scan \a path for files and directories, filtering them according to \a mode.
 * The \a dirs and \a files arguments are expected to have been allocated with
 * to count_dir_items().
 *
 * \param[in]   path    directory to scan
 * \param[out]  dirs    list of directories inside \a path
 * \param[out]  file    list of files inside \a path
 * \param[in]   mode    filter to test pathnames against
 */
static void populate_lists(const char *path,
                           archdep_name_table_t *dirs,
                           archdep_name_table_t *files,
                           int mode)
{
    DIR *dirp = NULL;
    struct dirent *dp = NULL;
    int dir_count = 0;
    int file_count = 0;
    size_t len;
    unsigned int isdir;
    char *filename;
    int retval;

    dirp = opendir(path);
    if (dirp == NULL) {
        return;
    }

    dp = readdir(dirp);

    while (dp != NULL) {
        if (must_show_entry(dp->d_name, mode)) {
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


/** \brief  Scan directory and return a directory object
 *
 * \param[in]   path    directory to scan
 * \param[in]   mode    filter to apply on each directory entry
 *
 * \return  directory object or NULL on failure
 */
archdep_dir_t *archdep_opendir(const char *path, int mode)
{
    archdep_dir_t *archdep_dir;
    int dirs_amount = 0;
    int files_amount = 0;

    if (count_dir_items(path, mode, &dirs_amount, &files_amount) < 0) {
        return NULL;
    }

    archdep_dir = lib_malloc(sizeof(archdep_dir_t));

    archdep_dir->dirs = lib_malloc(sizeof(archdep_name_table_t) * dirs_amount);
    archdep_dir->files = lib_malloc(sizeof(archdep_name_table_t) * files_amount);

    populate_lists(path, archdep_dir->dirs, archdep_dir->files, mode);
    qsort(archdep_dir->dirs, dirs_amount, sizeof(archdep_name_table_t), compare_names);
    qsort(archdep_dir->files, files_amount, sizeof(archdep_name_table_t), compare_names);

    archdep_dir->dir_amount = dirs_amount;
    archdep_dir->file_amount = files_amount;
    archdep_dir->pos = 0;

    return archdep_dir;
}


/** \brief  Get an entry from the directory
 *
 * \param[in]   dir     directory object
 *
 * \return  entry or NULL when the directory is exhausted
 */
const char *archdep_readdir(archdep_dir_t *dir)
{
    const char *retval = NULL;
    int dir_amount = dir->dir_amount;
    int file_amount = dir->file_amount;
    int pos = dir->pos;

    if (pos >= 0 && pos < dir_amount) {
        retval = dir->dirs[pos].name;
    } else if (pos >= dir_amount && pos <= (dir_amount + file_amount)) {
        retval = dir->files[pos - dir_amount].name;
    }
    return retval;
}



/** \brief  Close directory
 *
 * Free memory used by \a dir and its members.
 *
 * \param[in]   dir     directory object
 */
void archdep_closedir(archdep_dir_t *dir)
{
    int i;

    for (i = 0; i < dir->dir_amount; i++) {
        lib_free(dir->dirs[i].name);
    }
    for (i = 0; i < dir->file_amount; i++) {
        lib_free(dir->files[i].name);
    }
    lib_free(dir->dirs);
    lib_free(dir->files);
    lib_free(dir);
}


/** \brief  Rewind position in directory
 *
 * Rewind position in \a dir to the first entry.
 *
 * \param[in]   dir     directory object
 */
void archdep_rewinddir(archdep_dir_t *dir)
{
    dir->pos = 0;
}


/** \brief  Set position in directory
 *
 * \param[in]   dir     directory object
 * \param[in]   pos     new position
 */
void archdep_seekdir(archdep_dir_t *dir, int pos)
{
    if (pos >= 0 && pos < (dir->dir_amount + dir->file_amount)) {
        dir->pos = pos;
    }
}


/** \brief  Get position in directory
 *
 * \param[in]   dir     directory object
 *
 * \return  position in \a dir or -1 when \a dir is exhausted
 */
int archdep_telldir(archdep_dir_t *dir)
{
    if (dir->pos >= (dir->dir_amount + dir->file_amount)) {
        return -1;
    }
    return dir->pos;
}
