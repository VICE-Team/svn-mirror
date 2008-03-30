/*
 * sysfile.c - Simple locator for VICE system files.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "findpath.h"
#include "ioutil.h"
#include "log.h"
#include "resources.h"
#include "sysfile.h"
#include "utils.h"

/* ------------------------------------------------------------------------- */

/* Resources.  */

static char *default_path = NULL;
static char *system_path = NULL;
static char *expanded_system_path = NULL;

static int set_system_path(resource_value_t v, void *param)
{
    char *tmp_path, *tmp_path_save, *p, *s, *current_dir;

    util_string_set(&system_path, (char *)v);

    if (expanded_system_path != NULL) {
        free(expanded_system_path);
    }

    expanded_system_path = NULL; /* will subsequently be replaced */

    tmp_path_save = util_subst(system_path, "$$", default_path); /* malloc'd */

    current_dir = ioutil_current_dir();

    tmp_path = tmp_path_save; /* tmp_path points into tmp_path_save */
    do {
        p = strstr(tmp_path, FINDPATH_SEPARATOR_STRING);

        if (p != NULL) {
            *p = 0;
        }
        if (!archdep_path_is_relative(tmp_path)) {
            /* absolute path */
            if (expanded_system_path == NULL) {
                s = util_concat(tmp_path, NULL ); /* concat allocs a new str. */
            } else {
                s = util_concat(expanded_system_path,
                    FINDPATH_SEPARATOR_STRING,
                    tmp_path, NULL );
            }
        } else { /* relative path */
            if (expanded_system_path == NULL) {
                s = util_concat(current_dir,
                    FSDEV_DIR_SEP_STR,
                    tmp_path, NULL );
            } else {
                s = util_concat(expanded_system_path,
                    FINDPATH_SEPARATOR_STRING,
                    current_dir,
                    FSDEV_DIR_SEP_STR,
                    tmp_path, NULL );
            }
        }
        free(expanded_system_path);
        expanded_system_path = s;

        tmp_path = p + strlen(FINDPATH_SEPARATOR_STRING);
    } while(p != NULL);

    free(current_dir);
    free(tmp_path_save);

    return 0;
}

static resource_t resources[] = {
    { "Directory", RES_STRING, (resource_value_t) "$$",
      (resource_value_t *) &system_path, set_system_path, NULL },
    { NULL },
};

/* Command-line options.  */

static cmdline_option_t cmdline_options[] = {
    { "-directory", SET_RESOURCE, 1, NULL, NULL, "Directory", NULL,
      "<path>", "Define search path to locate system files" },
    { NULL },
};


/* ------------------------------------------------------------------------- */

int sysfile_init(const char *emu_id)
{

    default_path = stralloc(archdep_default_sysfile_pathlist(emu_id));

    return 0;
}

int sysfile_resources_init(void)
{
    return resources_register(resources);
}

int sysfile_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* Locate a system file called `name' by using the search path in
   `Directory', checking that the file can be accesses in mode `mode', and
   return an open stdio stream for that file.  If `complete_path_return' is
   not NULL, `*complete_path_return' points to a malloced string with the
   complete path if the file was found or is NULL if not.  */
FILE *sysfile_open(const char *name, char **complete_path_return,
                   const char *open_mode)
{
    char *p = NULL;
    FILE *f;

#ifdef __riscos
    char buffer[256];
#endif

    if (name == NULL || *name == '\0') {
        log_error(LOG_DEFAULT, "Missing name for system file.");
        return NULL;
    }

#ifdef __riscos
    p = (char*)name;
    while (*p != '\0') {
        if ((*p == ':') || (*p == '$'))
            break;
        p++;
    }
    if (*p != '\0') {
        if (complete_path_return != NULL)
            *complete_path_return = stralloc(name);
        return fopen(name, open_mode);
    }

    f = NULL;
    sprintf(buffer, "%s%s",default_path, name);

    if (ioutil_access(buffer, IOUTIL_ACCESS_R_OK)) {
        sprintf(buffer, "Vice:DRIVES.%s", name);
        if (ioutil_access(buffer, IOUTIL_ACCESS_R_OK)) {
            sprintf(buffer, "Vice:PRINTER.%s", name);
            if (ioutil_access(buffer, IOUTIL_ACCESS_R_OK)) {
                buffer[0] = '\0';
            }
        }
    }
    if (buffer[0] != '\0')
        f = fopen(buffer, open_mode);

    if (f == NULL) {
        if (complete_path_return != NULL)
            *complete_path_return = NULL;
        return NULL;
    } else {
        if (complete_path_return != NULL)
            *complete_path_return = stralloc(buffer);
        return f;
    }
#else
    p = findpath(name, expanded_system_path, IOUTIL_ACCESS_R_OK);

    if (p == NULL) {
        if (complete_path_return != NULL)
            *complete_path_return = NULL;
        return NULL;
    } else {
        f = fopen(p, open_mode);

        if (f == NULL || complete_path_return == NULL) {
            free(p);
            p = NULL;
        }
        if (complete_path_return != NULL)
            *complete_path_return = p;
        return f;
    }
#endif
}

/* As `sysfile_open', but do not open the file.  Just return 0 if the file is
   found and is readable, or -1 if an error occurs.  */
int sysfile_locate(const char *name, char **complete_path_return)
{
    FILE *f = sysfile_open(name, complete_path_return, MODE_READ);

    if (f != NULL) {
        fclose(f);
        return 0;
    } else
        return -1;
}

/* ------------------------------------------------------------------------- */

int sysfile_load(const char *name, BYTE *dest, int minsize, int maxsize)
{
    FILE *fp = NULL;
    size_t rsize = 0;
    char *complete_path = NULL;

    fp = sysfile_open(name, &complete_path, MODE_READ);
    if (fp == NULL)
        goto fail;

    log_message(LOG_DEFAULT, "Loading system file `%s'.", complete_path);

    rsize = util_file_length(fp);

    if (rsize < ((size_t)minsize)) {
        log_error(LOG_DEFAULT, "ROM %s: short file.", complete_path);
        goto fail;
    }
    if (rsize == ((size_t)maxsize + 2)) {
        log_warning(LOG_DEFAULT,
                    "ROM `%s': two bytes too large - removing assumed "
                    "start address.", complete_path);
        fread((char *)dest, 1, 2, fp);
        rsize -= 2;
    }
    if (rsize < ((size_t)maxsize)) {
        dest += maxsize - rsize;
    } else if (rsize > ((size_t)maxsize)) {
        log_warning(LOG_DEFAULT, "ROM `%s': long file, discarding end.",
                    complete_path);
        rsize = maxsize;
    }
    if ((rsize = fread((char *)dest, 1, rsize, fp)) < ((size_t)minsize))
        goto fail;

    fclose(fp);
    free(complete_path);
    return rsize;  /* return ok */

fail:
    free(complete_path);
    return -1;
}

