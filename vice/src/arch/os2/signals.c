/*
 * signals.c
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#include <os2.h>

#include <signal.h>
#include <stdlib.h>

#include "lib.h"
#include "log.h"
#include "monitor.h"
#include "signals.h"

#if defined(__IBMC__) || defined(WATCOM_COMPILE)
//   #define SIGILL       1       /* illegal instruction - invalid function image    */
//   #define SIGSEGV      2       /* invalid access to memory                        */
//   #define SIGFPE       3       /* floating point exception                        */
//   #define SIGTERM      4       /* OS/2 SIGTERM (killprocess) signal               */
//   #define SIGABRT      5       /* abort() signal                                  */
//   #define SIGINT       6       /* OS/2 SIGINTR signal                             */
//   #define SIGUSR1      7       /* user exception in range 0xa0000000 - 0xbfffffff */
//   #define SIGUSR2      8       /* user exception in range 0xc0000000 - 0xdfffffff */
//   #define SIGUSR3      9       /* user exception in range 0xe0000000 - 0xffffffff */
//   #define SIGBREAK    10       /* OS/2 Ctrl-Break sequence                        */
char sys_siglist[][9] = {
    "NULL",
    "SIGILL",
    "SIGSEGV",
    "SIGFPE",
    "SIGTERM",
    "SIGABRT",
    "SIGINT",
    "SIGUSR1",
    "SIGUSR2",
    "SIGUSR3",
    "SIGBREAK"
};
#endif

#ifdef __EMXC__
//   #define SIGHUP    1 /* Hangup */
//   #define SIGINT    2 /* Interrupt (Ctrl-C) */
//   #define SIGQUIT   3 /* Quit */
//   #define SIGILL    4 /* Illegal instruction */
//   #define SIGTRAP   5 /* Single step (debugging) */
//   #define SIGABRT   6 /* abort () */
//   #define SIGEMT    7 /* EMT instruction */
//   #define SIGFPE    8 /* Floating point */
//   #define SIGKILL   9 /* Kill process */
//   #define SIGBUS   10 /* Bus error */
//   #define SIGSEGV  11 /* Segmentation fault */
//   #define SIGSYS   12 /* Invalid argument to system call */
//   #define SIGPIPE  13 /* Broken pipe */
//   #define SIGALRM  14 /* Alarm */
//   #define SIGTERM  15 /* Termination, process killed */
//   #define SIGUSR1  16 /* User-defined signal #1 */
//   #define SIGUSR2  17 /* User-defined signal #2 */
//   #define SIGCHLD  18 /* Death of a child process */
//   #define SIGBREAK 21 /* Break (Ctrl-Break) */
char sys_siglist[][] = {
    "NULL",
    "SIGHUP",
    "SIGINT",
    "SIGQUIT",
    "SIGILL",
    "SIGTRAP",
    "SIGABRT",
    "SIGEMT",
    "SIGFPE",
    "SIGKILL",
    "SIGBUS",
    "SIGSEGV",
    "SIGSYS",
    "SIGPIPE",
    "SIGALRM",
    "SIGTERM",
    "SIGUSR1",
    "SIGUSR2",
    "SIGCHLD",
    "SIG19",
    "SIG20",
    "SIGBREAK"
};
#endif

#if !defined __X1541__ && !defined __PETCAT__
extern int trigger_shutdown;
#endif

static RETSIGTYPE break64(int sig)
{
    char *sigtxt;
    sigtxt = lib_msprintf("Received signal %d (%s). Vice will be closed.",
                          sig, sys_siglist[sig]);
    log_message(LOG_DEFAULT, sigtxt);
#if !defined __X1541__ && !defined __PETCAT__
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                  sigtxt, "VICE/2 Exception", 0, MB_OK);
    trigger_shutdown = TRUE;

#endif
    lib_free(sigtxt);
    exit (-1);
}

void signals_init(int do_core_dumps)
{
    // at the place where it's called at the moment it's only valid
    // for the vice (simulation) thread

    signal(SIGINT, SIG_IGN);

    if (!do_core_dumps) {
        signal(SIGSEGV,  break64);
        signal(SIGILL,   break64);
        signal(SIGFPE,   break64);
        signal(SIGABRT,  break64);
        // signal(SIGINT,   break64);
        signal(SIGTERM,  break64);
        // signal(SIGUSR1,  break64);
        // signal(SIGUSR2,  break64);
        signal(SIGBREAK, break64);
#if defined(__IBMC__) || defined(WATCOM_COMPILE)
        // signal(SIGUSR3,  break64);
#else
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
#endif
    }
}
/*
typedef void (*signal_handler_t)(int);

static signal_handler_t old_handler;

static void handle_abort(int signo)
{
    monitor_abort();
    signal(SIGINT, (signal_handler_t)handle_abort);
}
*/
void signals_abort_set(void)
{
    //old_handler = signal(SIGINT, handle_abort);
}

void signals_abort_unset(void)
{
    //signal(SIGINT, old_handler);
}
