/*
 * sysfile.h - Simple locator for VICE system files.
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

#include <stdio.h>

#include "resources.h"
#include "findpath.h"
#include "sysfile.h"
#include "utils.h"

static char *default_path;
static char *system_path;
static char *expanded_system_path;

static int set_system_path(resource_value_t v)
{
    string_set(&system_path, (char *) v);

    if (expanded_system_path != NULL)
        free(expanded_system_path);

    expanded_system_path = subst(system_path, "$$", default_path);

    return 0;
}

static resource_t resources[] = {
    { "Directory", RES_STRING, (resource_value_t) "$$",
      (resource_value_t *) &system_path, set_system_path },
    { NULL },
};

/* ------------------------------------------------------------------------- */

int sysfile_init(const char *boot_path, const char *emu_id)
{
#ifdef __MSDOS__
    /* On MS-DOS, always search in the directory in which the binary is
           stored. */
    default_path = concat(boot_path, "/", emu_id, NULL);
#else
    /* On Unix, first search in the `LIBDIR' and then in the
           `boot_path'.  */
    default_path = concat(LIBDIR, "/", emu_id, FINDPATH_SEPARATOR_STRING,
                          boot_path, "/", emu_id, NULL);
#endif

    printf("Default path set to `%s'\n", default_path);
    return 0;
}

int sysfile_init_resources(void)
{
    return resources_register(resources);
}

/* Locate a system file called `name' by using the search path in
   `Directory', checking that the file can be accesses in mode `mode', and
   return an open stdio stream for that file.  If `complete_path_return' is
   not NULL, `*complete_path_return' points to a malloced string with the
   complete path if the file was found or is NULL if not.  */
FILE *sysfile_open(const char *name, char **complete_path_return)
{
    char *p = findpath(name, expanded_system_path, R_OK);

    if (p == NULL) {
        if (complete_path_return != NULL)
            *complete_path_return = NULL;
        return NULL;
    } else {
        FILE *f = fopen(p, "r");

        if (f == NULL || complete_path_return == NULL) {
            free(p);
            p = NULL;
        }
        if (complete_path_return != NULL)
            *complete_path_return = p;
        return f;
    }
}
