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

#include <pwd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include "archdep.h"

#include "findpath.h"
#include "log.h"
#include "utils.h"

static char *argv0;

int archdep_startup(int *argc, char **argv)
{
    argv0 = stralloc(argv[0]);

    return 0;
}

const char *archdep_program_name(void)
{
    static char *program_name;

    if (program_name == NULL) {
        char *p;

        p = strrchr(argv0, '/');
        if (p == NULL)
            program_name = stralloc(argv0);
        else
            program_name = stralloc(p + 1);
    }

    return program_name;
}

const char *archdep_boot_path(void)
{
    static char *boot_path;

    if (boot_path == NULL) {
        boot_path = findpath(argv0, getenv("PATH"), X_OK);

        /* Remove the program name.  */
        *strrchr(boot_path, '/') = '\0';
    }

    return boot_path;
}

const char *archdep_home_path(void)
{
    char *home;

    home = getenv("HOME");
    if (home == NULL) {
        struct passwd *pwd;

        pwd = getpwuid(getuid());
        home = pwd->pw_dir;
    }

    return home;
}

const char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path;
        const char *home_path;

        boot_path = archdep_boot_path();
        home_path = archdep_home_path();

        /* First search in the `LIBDIR' then the $HOME/.vice/ dir (home_path)
	   and then in the `boot_path'.  */
        default_path = concat(LIBDIR, "/", emu_id,
                              FINDPATH_SEPARATOR_STRING,
			      home_path, "/", VICEUSERDIR, "/", emu_id, 
                              FINDPATH_SEPARATOR_STRING,
                              boot_path, "/", emu_id,
                              FINDPATH_SEPARATOR_STRING,
                              LIBDIR, "/DRIVES",
                              FINDPATH_SEPARATOR_STRING,
			      home_path, "/", VICEUSERDIR, "/DRIVES", 
                              FINDPATH_SEPARATOR_STRING,
                              boot_path, "/DRIVES", NULL);
    }

    return default_path;
}

const char *archdep_default_resource_file_name(void)
{
    static char *fname;
    const char *home;

    if (fname != NULL)
        free(fname);

    home = archdep_home_path();

    fname = concat(home, "/.vice/vicerc", NULL);

    return fname;
}

const char *archdep_default_save_resource_file_name(void)
{
    static char *fname;
    const char *home;
    char *viceuserdir;

    if (fname != NULL)
        free(fname);

    home = archdep_home_path();

    viceuserdir = concat(home, "/.vice", NULL);

    if(access(viceuserdir,F_OK)) {
	mkdir(viceuserdir,0700);
    }

    fname = concat(viceuserdir, "/vicerc", NULL);

    free(viceuserdir);

    return fname;
}

FILE *archdep_open_default_log_file(void)
{
    return stdout;
}

int archdep_num_text_lines(void)
{
    char *s;

    s = getenv("LINES");
    if (s == NULL) {
        printf("No LINES!\n");
        return -1;
    }
    return atoi(s);
}

int archdep_num_text_columns(void)
{
    char *s;

    s = getenv("COLUMNS");
    if (s == NULL)
        return -1;
    return atoi(s);
}

int archdep_default_logger(const char *level_string, const char *format,
                                                                va_list ap) {
    if (fputs(level_string, stdout) == EOF
        || vfprintf(stdout, format, ap) < 0
        || fputc ('\n', stdout) == EOF)
    	return -1;
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
    signal(SIGINT, break64);
    signal(SIGTERM, break64);

    if (!do_core_dumps) {
        signal(SIGSEGV,  break64);
        signal(SIGILL,   break64);
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
    }
}

