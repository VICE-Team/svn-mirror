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
#endif

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

