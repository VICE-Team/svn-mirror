/*
 * archdep_win32.c - Miscellaneous system-specific stuff.
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
#include <glib.h>

#include "lib.h"
#include "util.h"

#include "not_implemented.h"

#include "archdep.h"


/** \brief  String containing search paths
 *
 * Allocated in the first call to archdep_default_sysfile_pathlist(),
 * deallocated in archdep_shutdown().
 */
static char *default_path = NULL;


char *archdep_default_autostart_disk_image_file_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

char *archdep_default_fliplist_file_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}


/** \brief  Write message to Windows debugger/logger
 *
 * param[in]    level_string    log level string
 * param[in]    txt             log message
 *
 * \note    Shamelessly copied from win32/archdep.c
 *
 * \return  0 on success, < 0 on failure
 */
int archdep_default_logger(const char *level_string, const char *txt)
{
    TCHAR *st_out;

    char *out = lib_msprintf("*** %s %s\n", level_string, txt);
    st_out = system_mbstowcs_alloc(out);
    OutputDebugString(st_out);
    system_mbstowcs_free(st_out);
    lib_free(out);
    return 0;
}

char *archdep_default_resource_file_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

char *archdep_default_save_resource_file_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}


/** \brief  Build a list of search paths for emulator \a emu_id
 *
 * \param[in]   emu_id  emulator name (without '.exe')
 *
 * \return  string containing search paths
 */
char *archdep_default_sysfile_pathlist(const char *emu_id)
{

    if (default_path == NULL) {
        const char *boot_path = archdep_boot_path();

        default_path = util_concat(
                boot_path, "\\", emu_id, ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "\\DRIVES", ARCHDEP_FINDPATH_SEPARATOR_STRING,
                boot_path, "\\PRINTER", NULL);
    }

    return default_path;
}


int archdep_expand_path(char **return_path, const char *orig_name)
{
    NOT_IMPLEMENTED();
    return 0;
}

char *archdep_filename_parameter(const char *name)
{
    NOT_IMPLEMENTED();
    return NULL;
}

int archdep_file_is_gzip(const char *name)
{
    NOT_IMPLEMENTED();
    return 0;
}

int archdep_file_set_gzip(const char *name)
{
    NOT_IMPLEMENTED();
    return 0;
}

char *archdep_get_runtime_cpu(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

char *archdep_get_runtime_os(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

#if 0
int archdep_init(int *argc, char **argv)
{
    NOT_IMPLEMENTED();
    return 0;
}
#endif

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

int archdep_network_init(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

FILE *archdep_open_default_log_file(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}

#if 0
int archdep_path_is_relative(const char *path)
{
    NOT_IMPLEMENTED();
    return 0;
}
#endif

#if 0
char *archdep_program_name(void)
{
    NOT_IMPLEMENTED();
    return NULL;
}
#endif

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

char *archdep_tmpnam(void)
{
    NOT_IMPLEMENTED();
    return 0;
}

void archdep_signals_init(int do_core_dumps)
{
    NOT_IMPLEMENTED();
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

