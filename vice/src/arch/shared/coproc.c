/** \file   coproc.c
 * \brief   co-process fork
 *
 * \author  Andre Fachat <a.fachat@physik.tu-chemnitz.de>
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

/*
 * This is modelled after some examples in Stevens, "Advanced Progamming
 * in the Unix environment", Addison Wesley.
 *
 * It simply opens two uni-directional pipes and forks a process to
 * use the pipes as bidirectional connection for the stdin/out of the
 * child.
 * This, however, implies that the child knows its being piped and _buffers_
 * all stdio. To avoid that one has to open a pseudo terminal device,
 * which is too heavily system dependant to be included here.
 * Instead a wrapper like the program "pty" described in the book mentioned
 * above could be used.
 *
 * Technicalities: It does not store the PID of the forked child but
 * instead it relies on the child being killed when the parent terminates
 * prematurely or the child terminates itself on EOF on stdin.
 *
 * The command string is given to "/bin/sh -c cmdstring" such that
 * the shell can do fileexpansion.
 *
 * We ignore all SIGCHLD and SIGPIPE signals that may occur here by
 * installing an ignoring handler.
 */

#include "vice.h"

#ifdef UNIX_COMPILE

/* Solaris and Gtk3 eh? */
#ifdef __svr4__
#define _POSIX_SOURCE
#endif

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


#include "archdep.h"

#include "coproc.h"

#include "log.h"

#define SHELL "/bin/sh"

#ifndef sigset_t
#define sigset_t int
#endif

/* HP-UX 9 fix */
#ifndef SA_RESTART
#define SA_RESTART 0
#endif


static struct sigaction ignore;

int fork_coproc(int *fd_wr, int *fd_rd, char *cmd)
{
    int fd1[2], fd2[2];
    pid_t pid;

    ignore.sa_handler = SIG_IGN;
    sigemptyset(&ignore.sa_mask);
    ignore.sa_flags = SA_NOCLDSTOP | SA_RESTART;

    sigaction(SIGCHLD, &ignore, NULL);
    sigaction(SIGPIPE, &ignore, NULL);

    if (pipe(fd1) < 0) {
        log_error(LOG_DEFAULT, "Coproc: Couldn't open pipe!");
        return -1;
    }
    if (pipe(fd2) < 0) {
        log_error(LOG_DEFAULT, "Coproc: Couldn't open pipe!");
        close(fd1[0]);
        close(fd1[1]);
        return -1;
    }
    if ((pid = fork()) < 0) {
        log_error(LOG_DEFAULT, "Coproc: Couldn't fork()!");
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        return -1;
    } else if (pid == 0) {      /* child */
        close(fd1[0]);
        if (fd1[1] != STDOUT_FILENO) {
            dup2(fd1[1], STDOUT_FILENO);
            close(fd1[1]);
        }
        close(fd2[1]);
        if (fd2[0] != STDIN_FILENO) {
            dup2(fd2[0], STDIN_FILENO);
            close(fd2[0]);
        }
        /* Hm, we have to close all other files that are currently
           open now...  */
        execl(SHELL, "sh", "-c", cmd, NULL);

        archdep_vice_exit(127); /* child dies on error */
    } else {                    /* parent */
        close(fd1[1]);
        close(fd2[0]);

        *fd_rd = fd1[0];
        *fd_wr = fd2[1];
    }
    return 0;
}

#endif

#ifdef WIN32_COMPILE

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <io.h>
#include <fcntl.h>

#include "archdep.h"
#include "coproc.h"
#include "log.h"

/* https://docs.microsoft.com/en-us/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output */

/* https://marc.info/?l=apreq-dev&m=104840521714506 mentions some hackery we might need too */

/* FIXME: the following will only work with exactly one sub-process (not stacked commands like foo|bar)
          and likely even with just a program name and no additional arguments. we probably have to call
          cmd.exe in a similar way as the shell in the unix code above */

/* Create a child process that uses the previously created pipes for STDIN and STDOUT. */
static int CreateChildProcess(
    TCHAR *szCmdline,
    HANDLE hChildStd_IN_Rd,
    HANDLE hChildStd_OUT_Wr)
{
    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO siStartInfo;
    BOOL bSuccess = FALSE;

    /* Set up members of the PROCESS_INFORMATION structure. */
    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

    /* Set up members of the STARTUPINFO structure. */
    /* This structure specifies the STDIN and STDOUT handles for redirection. */
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO); 
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    /* Create the child process.  */
    bSuccess = CreateProcess(NULL, 
        szCmdline,     /* command line */
        NULL,          /* process security attributes */
        NULL,          /* primary thread security attributes */ 
        TRUE,          /* handles are inherited */ 
        0,             /* creation flags */ 
        NULL,          /* use parent's environment */ 
        NULL,          /* use parent's current directory */
        &siStartInfo,  /* STARTUPINFO pointer */ 
        &piProcInfo);  /* receives PROCESS_INFORMATION */ 

    /* If an error occurs, exit */
    if (!bSuccess) {
        return -1;
    } else {
        /* Close handles to the child process and its primary thread.
            Some applications might keep these handles to monitor the status
            of the child process, for example. */
        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        /* Close handles to the stdin and stdout pipes no longer needed by the child process.
            If they are not explicitly closed, there is no way to recognize that the child process has ended. */
        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_IN_Rd);
    }
    return 0;
}

int fork_coproc(int *fd_wr, int *fd_rd, char *cmd)
{
    HANDLE hChildStd_IN_Rd = NULL;
    HANDLE hChildStd_OUT_Wr = NULL;
    HANDLE hChildStd_IN_Wr = NULL;
    HANDLE hChildStd_OUT_Rd = NULL;
    SECURITY_ATTRIBUTES saAttr; 

    /* Set the bInheritHandle flag so pipe handles are inherited.  */
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    /* Create a pipe for the child process's STDOUT. */
     if (!CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0)) {
        return -1;
    }

    /* Ensure the read handle to the pipe for STDOUT is not inherited. */
    if (!SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        return -1;
    }

    /* Create a pipe for the child process's STDIN. */
    if (!CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0)) { 
        return -1;
    }

    /* Ensure the write handle to the pipe for STDIN is not inherited. */
    if (!SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
        return -1;
    }
 
    /* Create the child process. */
    if (CreateChildProcess(cmd, hChildStd_IN_Rd, hChildStd_OUT_Wr) < 0) {
        return -1;
    }

    /* convert the windows HANDLEs to a regular file handle */
    *fd_wr = _open_osfhandle((intptr_t)hChildStd_IN_Wr, _O_WRONLY | _O_BINARY);
    *fd_rd = _open_osfhandle((intptr_t)hChildStd_OUT_Rd, _O_RDONLY | _O_BINARY);

    return 0;
}

#endif
