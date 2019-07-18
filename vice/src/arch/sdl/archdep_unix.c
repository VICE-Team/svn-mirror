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


/* alternate storage of preferences */
const char *archdep_pref_path = NULL; /* NULL -> use home_path + ".vice" */

/* called from archdep.c:archdep_init */
static int archdep_init_extra(int *argc, char **argv)
{
    archdep_pref_path = archdep_user_config_path();
    return 0;
}

/* called from archdep.c:archdep_shutdown */
static void archdep_shutdown_extra(void)
{
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
