/*
 * archdep_unix.c - Miscellaneous system-specific stuff.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include <locale.h>
#include <pwd.h>
#include "vice_sdl.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#ifdef HAVE_VFORK_H
#include <vfork.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#if defined(__QNX__) && !defined(__QNXNTO__)
#include <sys/time.h>
#include <sys/timers.h>
#endif

#include "archdep.h"
#include "findpath.h"
#include "ioutil.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "monitor.h"
#include "ui.h"
#include "util.h"

#include "../shared/archdep_user_config_path.h"

#ifdef __NeXT__
#define waitpid(p, s, o) wait3((union wait *)(s), (o), (struct rusage *) 0)
#endif


/* alternate storage of preferences */
const char *archdep_pref_path = NULL; /* NULL -> use home_path + ".vice" */


#if defined(__QNX__) && !defined(__QNXNTO__)
int archdep_rtc_get_centisecond(void)
{
    struct timespec dtm;
    int status;

    if ((status = clock_gettime(CLOCK_REALTIME, &dtm)) == 0) {
        return dtm.tv_nsec / 10000L;
    }
    return 0;
}
#endif

static int archdep_init_extra(int *argc, char **argv)
{
    archdep_pref_path = archdep_user_config_path();
    return 0;
}


char *archdep_default_hotkey_file_name(void)
{
    if (archdep_pref_path == NULL) {
        const char *home;

        home = archdep_home_path();
        return util_concat(home, "/.vice/sdl-hotkey-", machine_get_name(), ".vkm", NULL);
    } else {
        return util_concat(archdep_pref_path, "/sdl-hotkey-", machine_get_name(), ".vkm", NULL);
    }
}

char *archdep_default_joymap_file_name(void)
{
    if (archdep_pref_path == NULL) {
        const char *home;

        home = archdep_home_path();
        return util_concat(home, "/.vice/sdl-joymap-", machine_get_name(), ".vjm", NULL);
    } else {
        return util_concat(archdep_pref_path, "/sdl-joymap-", machine_get_name(), ".vjm", NULL);
    }
}


#if 0
char *archdep_tmpnam(void)
{
#ifdef HAVE_MKSTEMP
    char *tmp_name;
    const char mkstemp_template[] = "/vice.XXXXXX";
    int fd;
    char *tmp;
    char *final_name;

    tmp_name = lib_malloc(ioutil_maxpathlen());
#ifdef USE_EXE_RELATIVE_TMP
    strcpy(tmp_name, archdep_boot_path());
    strcat(tmp_name, "/tmp");
#else
    if ((tmp = getenv("TMPDIR")) != NULL) {
        strncpy(tmp_name, tmp, ioutil_maxpathlen());
        tmp_name[ioutil_maxpathlen() - sizeof(mkstemp_template)] = '\0';
    } else {
        strcpy(tmp_name, "/tmp");
    }
#endif
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


int archdep_require_vkbd(void)
{
    return 0;
}


static void archdep_shutdown_extra(void)
{
}

/******************************************************************************/

static RETSIGTYPE break64(int sig)
{
    log_message(LOG_DEFAULT, "Received signal %d, exiting.", sig);
    exit (-1);
}

/*
    used once at init time to setup all signal handlers
*/
void archdep_signals_init(int do_core_dumps)
{
    if (!do_core_dumps) {
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


/* returns host keyboard mapping. used to initialize the keyboard map when
   starting with a blank (default) config, so an educated guess works good
   enough most of the time :)

   FIXME: add more languages
*/
int kbd_arch_get_host_mapping(void)
{
    int n;
    char *l;
    int maps[KBD_MAPPING_NUM] = {
        KBD_MAPPING_US, KBD_MAPPING_UK, KBD_MAPPING_DE, KBD_MAPPING_DA,
        KBD_MAPPING_NO, KBD_MAPPING_FI, KBD_MAPPING_IT, KBD_MAPPING_NL };
    char *s[KBD_MAPPING_NUM] = {
        "en_US", "en_UK", "de", "da", "no", "fi", "it", "nl" };
    /* setup the locale */
    setlocale(LC_ALL, "");
    l = setlocale(LC_ALL, NULL);
    if (l && (strlen(l) > 1)) {
        for (n = 1; n < KBD_MAPPING_NUM; n++) {
            if (strncmp(l, s[n], strlen(s[n])) == 0) {
                return maps[n];
            }
        }
    }
    return KBD_MAPPING_US;
}
