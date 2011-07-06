/*
 * signals.c
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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

#include "monitor.h"
#include "signals.h"

/*
    used once at init time to setup all signal handlers
*/
void signals_init(int do_core_dumps)
{
    /* FIXME: should this really be empty? */
    return;
}

typedef void (*signal_handler_t)(int);

static signal_handler_t old_handler;

static void handle_abort(int signo)
{
    monitor_abort();
    signal(SIGINT, (signal_handler_t)handle_abort);
}

/*
    these two are used by the monitor, to handle aborting ongoing output by
    pressing CTRL+C (SIGINT)
*/
void signals_abort_set(void)
{
    old_handler = signal(SIGINT, handle_abort);
}

void signals_abort_unset(void)
{
    signal(SIGINT, old_handler);
}

/*
    these two are used if the monitor is in remote mode. in this case we might
    get SIGPIPE if the connection is unexpectedly closed.
*/
/*
    FIXME: confirm wether SIGPIPE must be handled or not. if the emulator quits
           or crashes when the connection is closed, you might have to install
           a signal handler which calls monitor_abort().

           see arch/unix/signals.c and bug #3201796
*/
void signals_pipe_set(void)
{
}

void signals_pipe_unset(void)
{
}
