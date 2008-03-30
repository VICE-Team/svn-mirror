/*
 * archdep.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif

#include "archdep.h"
#include "findpath.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "ui.h"
#include "util.h"


static char *argv0 = NULL;
static char *boot_path = NULL;

int archdep_init(int *argc, char **argv)
{
    argv0 = lib_stralloc(argv[0]);

    archdep_ui_init(*argc, argv);
    return 0;
}

char *archdep_program_name(void)
{
    static char *program_name = NULL;

    if (program_name == NULL) {
        char *p;

        p = strrchr(argv0, '/');
        if (p == NULL)
            program_name = lib_stralloc(argv0);
        else
            program_name = lib_stralloc(p + 1);
    }

    return program_name;
}

const char *archdep_boot_path(void)
{
    if (boot_path == NULL) {
        boot_path = findpath(argv0, getenv("PATH"), IOUTIL_ACCESS_X_OK);

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
        if ((pwd == NULL)
            || ((home = pwd->pw_dir) == NULL)) {
            /* give up */
            home = ".";
        }
    }

    return home;
}

char *archdep_default_sysfile_pathlist(const char *emu_id)
{
    static char *default_path;

    if (default_path == NULL) {
        const char *boot_path;
        const char *home_path;

        boot_path = archdep_boot_path();
        home_path = archdep_home_path();

        /* First search in the `LIBDIR' then the $HOME/.vice/ dir (home_path)
           and then in the `boot_path'.  */
        default_path = util_concat(LIBDIR, "/", emu_id,
                                   FINDPATH_SEPARATOR_STRING,
                                   home_path, "/", VICEUSERDIR, "/", emu_id,
                                   FINDPATH_SEPARATOR_STRING,
                                   boot_path, "/", emu_id,
                                   FINDPATH_SEPARATOR_STRING,
                                   LIBDIR, "/DRIVES",
                                   FINDPATH_SEPARATOR_STRING,
                                   home_path, "/", VICEUSERDIR, "/DRIVES",
                                   FINDPATH_SEPARATOR_STRING,
                                   boot_path, "/DRIVES",
                                   FINDPATH_SEPARATOR_STRING,
                                   LIBDIR, "/PRINTER",
                                   FINDPATH_SEPARATOR_STRING,
                                   home_path, "/", VICEUSERDIR, "/PRINTER",
                                   FINDPATH_SEPARATOR_STRING,
                                   boot_path, "/PRINTER",
                                   NULL);
    }

    return default_path;
}

/* Return a malloc'ed backup file name for file `fname'.  */
char *archdep_make_backup_filename(const char *fname)
{
    return util_concat(fname, "~", NULL);
}

char *archdep_default_resource_file_name(void)
{
    const char *home;

    home = archdep_home_path();
    return util_concat(home, "/.vice/vicerc", NULL);
}

char *archdep_default_fliplist_file_name(void)
{
    const char *home;

    home = archdep_home_path();
    return util_concat(home, "/.vice/fliplist-", machine_name, ".vfl", NULL);
}

char *archdep_default_save_resource_file_name(void)
{
    char *fname;
    const char *home;
    char *viceuserdir;

    home = archdep_home_path();

    viceuserdir = util_concat(home, "/.vice", NULL);

    if (access(viceuserdir, F_OK)) {
        mkdir(viceuserdir, 0700);
    }

    fname = util_concat(viceuserdir, "/vicerc", NULL);

    lib_free(viceuserdir);

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

int archdep_default_logger(const char *level_string, const char *txt) {
    if (fputs(level_string, stdout) == EOF
        || fprintf(stdout, txt) < 0
        || fputc ('\n', stdout) == EOF)
        return -1;
    return 0;
}

static RETSIGTYPE break64(int sig)
{
#ifdef SYS_SIGLIST_DECLARED
    log_message(LOG_DEFAULT, _("Received signal %d (%s)."),
                sig, sys_siglist[sig]);
#else
    log_message(LOG_DEFAULT, _("Received signal %d."), sig);
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

int archdep_path_is_relative(const char *path)
{
    if (path == NULL)
        return 0;

    return *path != '/';
}

int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{
    pid_t child_pid;
    int child_status;

    child_pid = vfork();
    if (child_pid < 0) {
        log_error(LOG_DEFAULT, "vfork() failed: %s.", strerror(errno));
        return -1;
    } else {
        if (child_pid == 0) {
            if (stdout_redir && freopen(stdout_redir, "w", stdout) == NULL) {
                log_error(LOG_DEFAULT, "freopen(\"%s\") failed: %s.",
                          stdout_redir, strerror(errno));
                _exit(-1);
            }
            if (stderr_redir && freopen(stderr_redir, "w", stderr) == NULL) {
                log_error(LOG_DEFAULT, "freopen(\"%s\") failed: %s.",
                          stderr_redir, strerror(errno));
                _exit(-1);
            }
            execvp(name, argv);
            _exit(-1);
        }
    }

    if (waitpid(child_pid, &child_status, 0) != child_pid) {
        log_error(LOG_DEFAULT, "waitpid() failed: %s", strerror(errno));
        return -1;
    }

    if (WIFEXITED(child_status))
        return WEXITSTATUS(child_status);
    else
        return -1;
}

/* return malloc'd version of full pathname of orig_name */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    /* Unix version.  */
    if (*orig_name == '/') {
        *return_path = lib_stralloc(orig_name);
    } else {
        static char *cwd;

        cwd = ioutil_current_dir();
        *return_path = util_concat(cwd, "/", orig_name, NULL);
        lib_free(cwd);
    }
    return 0;
}

void archdep_startup_log_error(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vfprintf(stderr, format, ap);
}

char *archdep_filename_parameter(const char *name)
{
    /* nothing special(?) */
    return lib_stralloc(name);
}

char *archdep_quote_parameter(const char *name)
{
    /*not needed(?) */
    return lib_stralloc(name);
}

char *archdep_tmpnam(void)
{
    return lib_stralloc(tmpnam(NULL));
}

int archdep_file_is_gzip(const char *name)
{
    size_t l = strlen(name);

    if ((l < 4 || strcasecmp(name + l - 3, ".gz"))
        && (l < 3 || strcasecmp(name + l - 2, ".z"))
        && (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '.'))
        return 0;
    return 1;
}

int archdep_file_set_gzip(const char *name)
{
    return 0;
}

int archdep_mkdir(const char *pathname, int mode)
{
    return mkdir(pathname, (mode_t)mode);
}

int archdep_file_is_blockdev(const char *name)
{
    struct stat buf;

    if (stat(name, &buf) != 0)
        return 0;

    if (S_ISBLK(buf.st_mode))
        return 1;

    return 0;
}

int archdep_file_is_chardev(const char *name)
{
    struct stat buf;

    if (stat(name, &buf) != 0)
        return 0;

    if (S_ISCHR(buf.st_mode))
        return 1;

    return 0;
}

void archdep_shutdown(void)
{
    lib_free(argv0);
    lib_free(boot_path);
}

