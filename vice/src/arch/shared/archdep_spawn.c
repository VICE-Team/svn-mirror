/** \file   archdep_spawn.c
 * \brief   Process spawning
 *
 * Hopefully at some point this won't be required anymore.
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

#include "archdep_defs.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
# include <fcntl.h>

#ifdef ARCHDEP_OS_UNIX
# include <sys/wait.h>
#endif

#ifdef ARCHDEP_OS_WINDOWS
# include <windows.h>
# include <io.h>
# include <process.h>
#endif

#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "util.h"

/* #include "debug_gtk3.h" */

#include "archdep.h"
#include "archdep_defs.h"

/* WTF? */
/* fix VICE userdir */
#ifdef VICEUSERDIR
# undef VICEUSERDIR
#endif
/** \brief  User directory inside ./config
 */
#define VICEUSERDIR "vice"


#include "archdep_spawn.h"


#ifdef ARCHDEP_OS_UNIX

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

#elif defined(ARCHDEP_OS_WINDOWS)

/** \brief  Spawn new process
 *
 * Shamelessly stolen from arch/sdl/archdep_win32.c
 */
int archdep_spawn(const char *name, char **argv, char **pstdout_redir, const char *stderr_redir)
{
    int new_stdout, new_stderr;
    int old_stdout_mode, old_stderr_mode;
    int old_stdout, old_stderr;
    int retval;
    char *stdout_redir = NULL;

    if (pstdout_redir != NULL) {
        if (*pstdout_redir == NULL) {
            *pstdout_redir = archdep_tmpnam();
        }
        stdout_redir = *pstdout_redir;
    }

    new_stdout = new_stderr = old_stdout = old_stderr = -1;

    /* Make sure we are in binary mode.  */
    old_stdout_mode = _setmode(STDOUT_FILENO, _O_BINARY);
    old_stderr_mode = _setmode(STDERR_FILENO, _O_BINARY);

    /* Redirect stdout and stderr as requested, saving the old
       descriptors.  */
    if (stdout_redir != NULL) {
        old_stdout = _dup(STDOUT_FILENO);
        new_stdout = _open(stdout_redir,
                _O_WRONLY | _O_TRUNC | _O_CREAT, _S_IWRITE | _S_IREAD);
        if (new_stdout == -1) {
            log_error(LOG_DEFAULT,
                    "open(\"%s\") failed: %s.", stdout_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        _dup2(new_stdout, STDOUT_FILENO);
    }
    if (stderr_redir != NULL) {
        old_stderr = _dup(STDERR_FILENO);
        new_stderr = _open(stderr_redir,
                _O_WRONLY | _O_TRUNC | _O_CREAT, _S_IWRITE | _S_IREAD);
        if (new_stderr == -1) {
            log_error(LOG_DEFAULT,
                    "open(\"%s\") failed: %s.", stderr_redir, strerror(errno));
            retval = -1;
            goto cleanup;
        }
        _dup2(new_stderr, STDERR_FILENO);
    }

    /* Spawn the child process.  */
    retval = (int)_spawnvp(_P_WAIT, name, (const char * const *)argv);

cleanup:
    if (old_stdout >= 0) {
        _dup2(old_stdout, STDOUT_FILENO);
        _close(old_stdout);
    }
    if (old_stderr >= 0) {
        _dup2(old_stderr, STDERR_FILENO);
        _close(old_stderr);
    }
    if (old_stdout_mode >= 0) {
        _setmode(STDOUT_FILENO, old_stdout_mode);
    }
    if (old_stderr_mode >= 0) {
        _setmode(STDERR_FILENO, old_stderr_mode);
    }
    if (new_stdout >= 0) {
        _close(new_stdout);
    }
    if (new_stderr >= 0) {
        _close(new_stderr);
    }

    return retval;
}

#elif defined(ARCHDEP_OS_BEOS)

int archdep_spawn(const char *name, char **argv,
                  char **pstdout_redir, const char *stderr_redir)
{
    pid_t child_pid;
    int child_status;
    char *stdout_redir = NULL;

    if (pstdout_redir != NULL) {
        if (*pstdout_redir == NULL) {
            *pstdout_redir = archdep_tmpnam();
        }
        stdout_redir = *pstdout_redir;
    }

#ifdef WORDS_BIGENDIAN
    child_pid = -1;
#else
    child_pid = vfork();
#endif

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

#elif defined(ARCHDEP_OS_OS2)

/* Good luck finding the proper headers for this shit :) */

static HMTX hmtxSpawn;

/* Launch program `name' (searched via the PATH environment variable)
   passing `argv' as the parameters, wait for it to exit and return its
   exit status. If `stdout_redir' or `stderr_redir' are != NULL,
   redirect stdout or stderr to the corresponding file.  */
int archdep_spawn(const char *name, char **argv,
                  char **pstdout_redir, const char *stderr_redir)
{
    // how to redirect stdout & stderr??
    typedef struct _CHILDINFO {  /* Define a structure for the queue data */
        USHORT usSessionID;
        USHORT usReturn;
    } CHILDINFO;

    UCHAR fqName[256] = ""; /* Result of PATH search             */
    HQUEUE hqQueue;         /* Queue handle                      */
    REQUESTDATA rdRequest;  /* Request data for the queue        */
    ULONG ulSzData;         /* Size of the queue data            */
    BYTE bPriority;         /* For the queue                     */
    PVOID pvData;           /* Pointer to the queue data         */
    STARTDATA sd;           /* Start Data for DosStartSession    */
    PID pid;                /* PID for the started child session */
    ULONG ulSession;        /* Session ID for the child session  */
    APIRET rc;              /* Return code from API's            */
    char *cmdline;
    char *stdout_redir = NULL;

    if (pstdout_redir != NULL) {
        if (*pstdout_redir == NULL) {
            *pstdout_redir = archdep_tmpnam();
        }
        stdout_redir = *pstdout_redir;
    }

    if (archdep_search_path(name, fqName, sizeof(fqName))) {
        return -1;
    }

    // Make the needed command string
    cmdline = archdep_cmdline(fqName, argv, stdout_redir, stderr_redir);

    memset(&sd, 0, sizeof(STARTDATA));
    sd.Length = sizeof(STARTDATA);
    sd.FgBg = SSF_FGBG_BACK;      /* Start session in background */
    sd.Related = SSF_RELATED_CHILD;  /* Start a child session       */
    sd.PgmName = "cmd.exe";
    sd.PgmInputs = cmdline;
    sd.PgmControl = SSF_CONTROL_INVISIBLE;
    sd.TermQ = "\\QUEUES\\VICE2\\CHILD.QUE";
    sd.InheritOpt = SSF_INHERTOPT_SHELL;

    /* Start a child process and return it's session ID.
     Wait for the session to end and get it's session ID
     from the termination queue */

    // this prevents you from closing Vice while a child is running
    if (DosRequestMutexSem(hmtxSpawn, SEM_INDEFINITE_WAIT)) {
        return 0;
    }

    if (!(rc = DosCreateQueue(&hqQueue, QUE_FIFO|QUE_CONVERT_ADDRESS, sd.TermQ))) {
        if (!(rc = DosStartSession(&sd, &ulSession, &pid)))  {
            if (!(rc = DosReadQueue(hqQueue, &rdRequest, &ulSzData, &pvData,
                            0, DCWW_WAIT, &bPriority, 0))) {
                rc = ((CHILDINFO*)pvData)->usReturn;

                DosFreeMem(pvData); /* Free the memory of the queue data element read */
            }
        }
        DosCloseQueue(hqQueue);
    }

    DosReleaseMutexSem(hmtxSpawn);

    lib_free(cmdline);
    return rc;
}


#else
    /* Unsupported OS's */
int archdep_spawn(const char *name, char **argv,
                  char **stdout_redir, const char *stderr_redir)
{
    return -1;
}

#endif
