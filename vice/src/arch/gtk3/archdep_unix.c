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

#include "not_implemented.h"

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

int archdep_default_logger(const char *level_string, const char *txt)
{
    NOT_IMPLEMENTED();
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

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    NOT_IMPLEMENTED();
    return NULL;
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

int archdep_init(int *argc, char **argv)
{
    NOT_IMPLEMENTED();
    return 0;
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

int archdep_path_is_relative(const char *path)
{
    NOT_IMPLEMENTED();
    return 0;
}

char *archdep_program_name(void)
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

