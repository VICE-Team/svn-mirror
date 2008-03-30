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
#include <ctype.h>
#include <dir.h>
#include <errno.h>
#include <io.h>
#include <process.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "fcntl.h"
#include "archdep.h"

#include "log.h"
#include "utils.h"
#include "video.h"

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

        default_path = concat(boot_path, "/", emu_id,
                              FINDPATH_SEPARATOR_STRING,
                              boot_path, "/", "DRIVES", NULL);
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
    if (path == NULL)
        return 0;

    /* `c:\foo', `c:/foo', `c:foo', `\foo' and `/foo' are absolute.  */

    return !((isalpha(path[0]) && path[1] == ':')
            || path[0] == '/' || path[0] == '\\');
}

int archdep_spawn(const char *name, char **argv,
                  const char *stdout_redir, const char *stderr_redir)
{
    int new_stdout, new_stderr;
    int old_stdout_mode, old_stderr_mode;
    int old_stdout, old_stderr;
    int retval;

    new_stdout = new_stderr = old_stdout = old_stderr = -1;

    /* Make sure we are in binary mode.  */
    old_stdout_mode = setmode(STDOUT_FILENO, O_BINARY);
    old_stderr_mode = setmode(STDERR_FILENO, O_BINARY);

    /* Redirect stdout and stderr as requested, saving the old
       descriptors.  */
    if (stdout_redir != NULL) {
        old_stdout = dup(STDOUT_FILENO);
        new_stdout = open(stdout_redir, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (new_stdout == -1) {
            log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                      stdout_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        dup2(new_stdout, STDOUT_FILENO);
    }
    if (stderr_redir != NULL) {
        old_stderr = dup(STDERR_FILENO);
        new_stderr = open(stderr_redir, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (new_stderr == -1) {
            log_error(LOG_DEFAULT, "open(\"%s\") failed: %s.",
                      stderr_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        dup2(new_stderr, STDERR_FILENO);
    }

    /* Spawn the child process.  */
    retval = spawnvp(P_WAIT, name, argv);

cleanup:
    if (old_stdout >= 0) {
        dup2(old_stdout, STDOUT_FILENO);
        close(old_stdout);
    }
    if (old_stderr >= 0) {
        dup2(old_stderr, STDERR_FILENO);
        close(old_stderr);
    }
    if (old_stdout_mode >= 0)
        setmode(STDOUT_FILENO, old_stdout_mode);
    if (old_stderr_mode >= 0)
        setmode(STDERR_FILENO, old_stderr_mode);
    if (new_stdout >= 0)
        close(new_stdout);
    if (new_stderr >= 0)
        close(new_stderr);

    return retval;
}

/* return malloc´d version of full pathname of orig_name */
int archdep_expand_path(char **return_path, const char *orig_name)
{
    /* MS-DOS version.  */
    *return_path = _truename(orig_name, NULL);
    if (*return_path == NULL) {
        log_error(LOG_ERR,
                  "zfile_list_add: warning, illegal file name `%s'.",
                  orig_name);
        *return_path = stralloc(orig_name);
    }
    return 0;
}

static int old_input_mode, old_output_mode;

void archdep_open_monitor_console(FILE **mon_input, FILE **mon_output)
{
    enable_text();
    clrscr();
    _set_screen_lines(43);
    _setcursortype(_SOLIDCURSOR);

    old_input_mode = setmode(STDIN_FILENO, O_TEXT);
    old_output_mode = setmode(STDOUT_FILENO, O_TEXT);

    *mon_output = fopen("CON", "wt");
    *mon_input = fopen("CON", "rt");
    setbuf(*mon_output, NULL);    /* No buffering */
}

void archdep_close_monitor_console(FILE *mon_input, FILE *mon_output)
{
    setmode(STDIN_FILENO, old_input_mode);
    setmode(STDIN_FILENO, old_output_mode);

    disable_text();

    fclose(mon_input);
    fclose(mon_output);
}

