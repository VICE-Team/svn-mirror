/** \file   archdep_dir.h
 * \brief   Read host directory - header
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

#ifndef VICE_ARCHDEP_DIR_H
#define VICE_ARCHDEP_DIR_H

#include <stddef.h>
#include "archdep_defs.h"

/** \brief  Do not display files starting with '.'
 *
 * Do not display 'hidden' files on Unix.
 * Will still display the special files '.' and '..'
 */
#define ARCHDEP_OPENDIR_NO_DOTFILES 1

/** \brief  Show all files
 */
#define ARCHDEP_OPENDIR_ALL_FILES   0


/* XXX: This is a bit weird, could just use a list of strings instead? */
typedef struct archdep_name_table_s {
    char *name; /**< filename */
} archdep_name_table_t;


/** \brief  Directory object
 *
 * Contains a list of directories and a list of files for a given host directory.
 *
 * For the position in the directory the API concatenates the dirs and files
 * with the dirs coming first. The directories and files are sorted in ascending
 * order in a case-sensitive manner and thus sorted Unix-style and not Windows-
 * style (where case folding is normally applied).
 */
typedef struct archdep_dir_s {
    archdep_name_table_t *dirs;     /**< list of directories */
    archdep_name_table_t *files;    /**< list of files */
    int dir_amount;                 /**< number of entries in `dirs` */
    int file_amount;                /**< number of entries in `files` */
    int pos;                        /**< position in directory, adding together
                                         dirs and files with dirs coming first */
} archdep_dir_t;


archdep_dir_t * archdep_opendir(const char *path, int mode);
const char *    archdep_readdir(archdep_dir_t *dir);
void            archdep_closedir(archdep_dir_t *dir);
void            archdep_rewinddir(archdep_dir_t *dir);
void            archdep_seekdir(archdep_dir_t *dir, int pos);
int             archdep_telldir(archdep_dir_t *dir);

#endif
