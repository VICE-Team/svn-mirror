/*
 * dirent.c - Fake implementation of dirent stuff for Windows.
 *
 * Written by
 *  Ettore Perazzoli	(ettore@comm2000.it)
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
/* This was inspired by Mathias Ortmann's `posixemu.c', used in the Windows
   port of UAE, the <insert your favourite adjective starting by `U' here>
   Amiga Emulator.  */

#include "vice.h"

#include <windows.h>
#include <stdlib.h>
#include <io.h>

#include "utils.h"
#include "dirent.h"

struct _vice_dir {
    WIN32_FIND_DATA find_data;
    HANDLE handle;
    int first_passed;
    char *filter;
};
/*typedef struct _vice_dir DIR;*/

DIR *opendir(const char *path)
{
    DIR *dir;

    dir = xmalloc(sizeof(DIR));
    dir->filter = concat(path, "\\*", NULL);
    dir->handle = FindFirstFile(dir->filter, &dir->find_data);
    if (dir->handle == INVALID_HANDLE_VALUE)
        return NULL;

    dir->first_passed = 0;
    return dir;
}

struct dirent *readdir(DIR *dir)
{
    static struct dirent ret;

    if (dir->first_passed)
        if (!FindNextFile(dir->handle, &dir->find_data))
            return NULL;

    dir->first_passed = 1;
    ret.d_name = dir->find_data.cFileName;
    ret.d_namlen = strlen(ret.d_name);

    return &ret;
}

void closedir(DIR *dir)
{
    FindClose(dir->handle);
    free(dir->filter);
    free(dir);
}

