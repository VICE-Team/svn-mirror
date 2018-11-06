/** \file   archdep_unix.c
 * \brief   Miscellaneous UNIX-specific stuff
 *
 * \author  Marco van den Heuvel <blackystardust68@yahoo.com>
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <glib.h>

#include "ioutil.h"
#include "lib.h"
#include "util.h"

#include "debug_gtk3.h"

#include "archdep.h"


/* fix VICE userdir */
#ifdef VICEUSERDIR
# undef VICEUSERDIR
#endif
/** \brief  User directory inside ./config
 */
#define VICEUSERDIR "vice"


#if 0
/** \brief  Path separator used in GLib code
 */
static const gchar *path_separator = "/";
#endif

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


/** \brief  Get the absolute path to the directory that contains resources, icons, etc
 *
 * \return  Path to the gui data directory
 */
char *archdep_get_vice_datadir(void)
{
#ifdef MACOSX_BUNDLE
    char *user_config_path = archdep_user_config_path();
    char *res = util_concat(user_config_path, "/gui/", NULL);
    debug_gtk3("FIXME: archdep_get_vice_datadir '%s'.", res);
# if 0
    lib_free(user_config_path);
# endif
    return res;
#else
    return util_concat(LIBDIR, "/gui/", NULL);
#endif
}


#if 0
/** \brief  Get the absolute path to the directory that contains the documentation
 *
 * \return  Path to the docs directory
 */
char *archdep_get_vice_docsdir(void)
{
#ifdef MACOSX_BUNDLE
    debug_gtk3("FIXME: archdep_get_vice_docsdir '%s%s'.",
            archdep_boot_path(), "/../doc/");
    return util_concat(archdep_boot_path(), "/../doc/", NULL);
#else
    return util_concat(DOCDIR, "/", NULL);
#endif
}
#endif

/** \brief  Architecture-dependent shutdown hanlder
 */
void archdep_shutdown(void)
{
    /* free memory used by the exec path */
    archdep_program_path_free();
    /* free memory used by the exec name */
    archdep_program_name_free();
    /* free memory used by the boot path */
    archdep_boot_path_free();
    /* free memory used by the home path */
    archdep_home_path_free();
    /* free memory used by the config files path */
    archdep_user_config_path_free();
    /* free memory used by the sysfile pathlist */
    archdep_default_sysfile_pathlist_free();

#if 0
    if (default_path != NULL) {
        lib_free(default_path);
    }
#endif

    /* this should be removed soon */
    if (argv0 != NULL) {
        lib_free(argv0);
        argv0 = NULL;
    }

    archdep_network_shutdown();

    /* partially implemented */
    INCOMPLETE_IMPLEMENTATION();
}


int archdep_spawn(const char *name, char **argv,
                  char **pstdout_redir, const char *stderr_redir)
{
    pid_t child_pid;
    int child_status;
    char *stdout_redir;


    if (pstdout_redir != NULL) {
        if (*pstdout_redir == NULL) {
            *pstdout_redir = archdep_tmpnam();
        }
        stdout_redir = *pstdout_redir;
    } else {
        stdout_redir = NULL;
    }

    child_pid = vfork();
    if (child_pid < 0) {
        log_error(LOG_DEFAULT, "vfork() failed: %s.", strerror(errno));
        return -1;
    } else {
        if (child_pid == 0) {
            if (stdout_redir && freopen(stdout_redir, "w", stdout) == NULL) {
                log_error(LOG_DEFAULT, "freopen(\"%s\") failed: %s.", stdout_redir, strerror(errno));
                _exit(-1);
            }
            if (stderr_redir && freopen(stderr_redir, "w", stderr) == NULL) {
                log_error(LOG_DEFAULT, "freopen(\"%s\") failed: %s.", stderr_redir, strerror(errno));
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

    if (WIFEXITED(child_status)) {
        return WEXITSTATUS(child_status);
    } else {
        return -1;
    }
}

/* for when I figure this out: */
#if 0
    char *stdout_redir;
    gboolean result;
    GPid child_pid;
    GError *err;

    if (pstdout_redir != NULL) {
        if (*pstdout_redir == NULL) {
            *pstdout_redir = archdep_tmpnam();
        }
        stdout_redir = *pstdout_redir;
    } else {
        stdout_redir = NULL;
    }

    result = g_spawn_async(NULL,        /* working_directory, NULL = inherit */
                           argv,        /* argv */
                           NULL,        /* envp, NULL = inherit */
                           G_SPAWN_DEFAULT,   /* flags */
                           NULL,        /* child_setup */
                           NULL,        /* user_data */
                           &child_pid,  /* child PID object */
                           &err);
#endif


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


static RETSIGTYPE break64(int sig)
{
    log_message(LOG_DEFAULT, "Received signal %d, exiting.", sig);
    exit (-1);
}

void archdep_signals_init(int do_core_dumps)
{
    if (do_core_dumps) {
        signal(SIGPIPE, break64);
    }
}

typedef void (*signal_handler_t)(int);

static signal_handler_t old_pipe_handler;

/*
    these two are used for socket send/recv. in this case we might
    get SIGPIPE if the connection is unexpectedly closed.
*/
void archdep_signals_pipe_set(void)
{
    old_pipe_handler = signal(SIGPIPE, SIG_IGN);
}

void archdep_signals_pipe_unset(void)
{
    signal(SIGPIPE, old_pipe_handler);
}

