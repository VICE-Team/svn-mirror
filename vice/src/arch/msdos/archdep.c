/*
 * archdep.c - Miscellaneous system-specific stuff.
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

#include <conio.h>
#include <dir.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "fcntl.h"
#include "findpath.h"
#include "archdep.h"

#include "log.h"
#include "utils.h"

static char *orig_workdir;
static char *argv0;

static void restore_workdir(void)
{
    if (orig_workdir)
	chdir(orig_workdir);
}

int archdep_startup(int *argc, char **argv)
{
    _fmode = O_BINARY;

    argv0 = stralloc(argv[0]);

    orig_workdir = getcwd(NULL, GET_PATH_MAX);
    atexit(restore_workdir);

    return 0;
}

const char *archdep_program_name(void)
{
    static char *program_name;

    if (program_name == NULL) {
        char *s, *e;
        int len;

        s = strrchr(argv0, '/');
        if (s == NULL)
            s = argv0;
        else
            s++;
        e = strchr(s, '.');
        if (e == NULL)
            e = argv0 + strlen(argv0);

        len = e - s + 1;
        program_name = xmalloc(len);
        memcpy(program_name, s, len - 1);
        program_name[len - 1] = 0;
    }

    return program_name;
}

const char *archdep_boot_path(void)
{
    static char *boot_path;

    if (boot_path == NULL) {
        fname_split(argv0, &boot_path, NULL);

        /* This should not happen, but you never know...  */
        if (boot_path == NULL)
            boot_path = stralloc("./");
    }

    return boot_path;
}

const char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path = archdep_boot_path();

        default_path = concat(emu_id,
                              FINDPATH_SEPARATOR_STRING,
                              "DRIVES", NULL);
    }

    return default_path;
}

const char *archdep_default_save_resource_file_name(void) {
    return archdep_default_resource_file_name();
}

const char *archdep_default_resource_file_name(void)
{
    static char *fname;

    if (fname != NULL)
        free(fname);

    fname = concat(archdep_boot_path(), "\\vicerc", NULL);
    return fname;
}

FILE *archdep_open_default_log_file(void)
{
    char *fname;
    FILE *f;

    fname = concat(archdep_boot_path(), "\\vice.log", NULL);
    f = fopen(fname, "wt");
    free(fname);

    return f;
}

int archdep_num_text_lines(void)
{
    struct text_info text_mode_info;

    gettextinfo(&text_mode_info);
    return text_mode_info.screenheight;
}

int archdep_num_text_columns(void)
{
    struct text_info text_mode_info;

    gettextinfo(&text_mode_info);
    return text_mode_info.screenwidth;
}
 
int archdep_default_logger(const char *level_string, const char *format,
                                                                va_list ap)
{
    return 0;
}

static RETSIGTYPE break64(int sig)
{

#ifdef SYS_SIGLIST_DECLARED
    log_message(LOG_DEFAULT, "Received signal %d (%s).",
                sig, sys_siglist[sig]);
#else
    log_message(LOG_DEFAULT, "Received signal %d.", sig);
#endif

    exit (-1);
}

void archdep_setup_signals(int do_core_dumps)
{
    signal(SIGINT, SIG_IGN);

    if (!do_core_dumps) {
        signal(SIGSEGV,  break64);
        signal(SIGILL,   break64);
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
    }
}

int archdep_path_is_relative(const char *path)
{
    return (isalpha(path[0]) && path[1] == ':'
            && (path[2] == '/' || path[2] == '\\'));
}
