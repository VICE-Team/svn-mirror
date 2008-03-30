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

#include <signal.h>
#include <stdlib.h>

#include "lib.h"
#include "log.h"
#include "signals.h"


#if !defined __X1541__ && !defined __PETCAT__
extern int trigger_shutdown;
#endif

static RETSIGTYPE break64(int sig)
{
    char *sigtxt;
    sigtxt = lib_msprintf("Received signal %d (%s). Vice will be closed.",
                          sig, sys_siglist[sig]);
    log_message(archlog, sigtxt);
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
#ifdef __IBMC__
        // signal(SIGUSR3,  break64);
#else
        signal(SIGPIPE,  break64);
        signal(SIGHUP,   break64);
        signal(SIGQUIT,  break64);
#endif
    }
}

