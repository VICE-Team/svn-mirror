/*
 * archdep_unix.c - Miscellaneous system-specific stuff.
 *
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <glib.h>

#include "ioutil.h"
#include "lib.h"
#include "platform.h"
#include "util.h"

#include "not_implemented.h"

#include "archdep.h"


/* fix VICE userdir */
#ifdef VICEUSERDIR
# undef VICEUSERDIR
#endif
#define VICEUSERDIR "vice"



/** \brief  Path separator used in GLib code
 */
static const gchar *path_separator = "/";


/** \brief  Tokens that are illegal in a path/filename
 */
static const char *illegal_name_tokens = "/";



/** \brief  String containing search paths
 *
 * Allocated in the first call to archdep_default_sysfile_pathlist(),
 * deallocated in archdep_shutdown().
 */
static char *default_path = NULL;


/** \brief  Write log message to stdout
 *
 * param[in]    level_string    log level string
 * param[in]    txt             log message
 *
 * \note    Shamelessly copied from unix/archdep.c
 *
 * \return  0 on success, < 0 on failure
 */
int archdep_default_logger(const char *level_string, const char *txt)
{
    if (fputs(level_string, stdout) == EOF || fprintf(stdout, "%s", txt) < 0 || fputc ('\n', stdout) == EOF) {
        return -1;
    }
    return 0;
}


char *archdep_default_save_resource_file_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}


/** \brief  Build a list of search paths for emulator \a emu_id
 *
 * \param[in]   emu_id  emulator name
 *
 * \return  string containing search paths
 */
char *archdep_default_sysfile_pathlist(const char *emu_id)
{
#ifdef MINIX_SUPPORT
    static char *default_path_temp;
#endif

    if (default_path == NULL) {
        const char *boot_path;
        const char *home_path;

        boot_path = archdep_boot_path();
        home_path = archdep_user_config_path();

        /* First search in the `LIBDIR' then the $HOME/.vice/ dir (home_path)
           and then in the `boot_path'.  */

#ifdef MINIX_SUPPORT
        default_path_temp = util_concat(
                LIBDIR, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/", emu_id,NULL);

        default_path = util_concat(
                default_path_temp, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                LIBDIR, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                LIBDIR, "/PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/PRINTER", NULL);
        lib_free(default_path_temp);

#else
# if defined(MACOSX_BUNDLE)
        /* Mac OS X Bundles keep their ROMS in Resources/bin/../ROM */
#  if defined(MACOSX_COCOA)
#   define MACOSX_ROMDIR "/../Resources/ROM/"
#  else
#   define MACOSX_ROMDIR "/../ROM/"
#  endif
        default_path = util_concat(
                boot_path, MACOSX_ROMDIR, emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, MACOSX_ROMDIR, "DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, MACOSX_ROMDIR, "PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/PRINTER", NULL);
# else
        default_path = util_concat(
                LIBDIR, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                LIBDIR, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                LIBDIR, "/PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                home_path, "/", VICEUSERDIR, "/PRINTER", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "/PRINTER", NULL);
# endif
#endif
    }

    return default_path;
}


/** \brief  Generate heap-allocated full pathname of \a orig_name
 *
 * Returns the absolute path of \a orig_name. Expands '~' to the user's home
 * path. If the prefix in \a orig_name is not '~/', the file is assumed to
 * reside in the current working directory whichever that may be.
 *
 * \param[out]  return_path pointer to expand path destination
 * \param[in]   orig_name   original path
 *
 * \return  0
 */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    /* Unix version.  */
    if (*orig_name == '/') {
        *return_path = lib_stralloc(orig_name);
    } else if (*orig_name == '~' && *(orig_name +1) == '/') {
        *return_path = util_concat(archdep_home_path(), orig_name + 1, NULL);
    } else {
        static char *cwd;

        cwd = ioutil_current_dir();
        *return_path = util_concat(cwd, "/", orig_name, NULL);
        lib_free(cwd);
    }
    return 0;
}



char *archdep_filename_parameter(const char *name)
{
    NOT_IMPLEMENTED();
    return NULL;
}


int archdep_file_set_gzip(const char *name)
{
    NOT_IMPLEMENTED();
    return 0;
}


/** \brief  Determine OS during runtime
 *
 * \return  OS name
 */
char *archdep_get_runtime_os(void)
{
/* TODO: add runtime os detection code for other *nix os'es */
#ifndef RUNTIME_OS_CALL
    return "*nix";
#else
    return RUNTIME_OS_CALL();
#endif
}


/** \brief  Determine CPU type during runtime
 *
 * \return  CPU name
 */
char *archdep_get_runtime_cpu(void)
{
/* TODO: add runtime cpu detection code for other cpu's */
#ifndef RUNTIME_CPU_CALL
    return "Unknown CPU";
#else
    return RUNTIME_CPU_CALL();
#endif
}


char *archdep_make_backup_filename(const char *fname)
{
    NOT_IMPLEMENTED();
    return 0;
}

int archdep_mkdir(const char *pathname, int mode)
{
    NOT_IMPLEMENTED();
    return 0;
}

FILE *archdep_mkstemp_fd(char **filename, const char *mode)
{
    NOT_IMPLEMENTED();
    return NULL;
}

char *archdep_quote_parameter(const char *name)
{
    NOT_IMPLEMENTED();
    return NULL;
}

int archdep_rename(const char *oldpath, const char *newpath)
{
    NOT_IMPLEMENTED();
    return 0;
}

void archdep_shutdown(void)
{
    if (default_path != NULL) {
        lib_free(default_path);
    }

    archdep_network_shutdown();

    /* partially implemented */
    NOT_IMPLEMENTED();
}

int archdep_spawn(const char *name, char **argv, char **pstdout_redir, const char *stderr_redir)
{
    NOT_IMPLEMENTED();
    return 0;
}

void archdep_startup_log_error(const char *format, ...)
{
    NOT_IMPLEMENTED();
}

int archdep_stat(const char *file_name, unsigned int *len, unsigned int *isdir)
{
    NOT_IMPLEMENTED();
    return 0;
}


/** \brief  Create a unique temporary filename
 *
 * Uses mkstemp(3) when available.
 *
 * \return  temporary filename
 */
char *archdep_tmpnam(void)
{
#ifdef HAVE_MKSTEMP
    char *tmp_name;
    const char mkstemp_template[] = "/vice.XXXXXX";
    int fd;
    char *tmp;
    char *final_name;

    tmp_name = lib_malloc(ioutil_maxpathlen());
    if ((tmp = getenv("TMPDIR")) != NULL) {
        strncpy(tmp_name, tmp, ioutil_maxpathlen());
        tmp_name[ioutil_maxpathlen() - sizeof(mkstemp_template)] = '\0';
    } else {
        strcpy(tmp_name, "/tmp");
    }
    strcat(tmp_name, mkstemp_template);
    if ((fd = mkstemp(tmp_name)) < 0) {
        tmp_name[0] = '\0';
    } else {
        close(fd);
    }

    final_name = lib_stralloc(tmp_name);
    lib_free(tmp_name);
    return final_name;
#else
    return lib_stralloc(tmpnam(NULL));
#endif
}


void archdep_signals_pipe_set(void)
{
    NOT_IMPLEMENTED();
}

void archdep_signals_pipe_unset(void)
{
    NOT_IMPLEMENTED();
}

char *archdep_default_rtc_file_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

int archdep_file_is_chardev(const char *name)
{
    NOT_IMPLEMENTED();
    return 0;
}

int archdep_file_is_blockdev(const char *name)
{
    NOT_IMPLEMENTED();
    return 0;
}

int archdep_fix_permissions(const char *file_name)
{
    NOT_IMPLEMENTED();
    return 0;
}


