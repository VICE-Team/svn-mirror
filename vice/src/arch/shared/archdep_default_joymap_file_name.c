/* This file is part of VICE, the Versatile Commodore Emulator.
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

#include <stdlib.h>

#include "archdep.h"

#include "machine.h"
#include "kbd.h"
#include "lib.h"
#include "util.h"

#include "archdep_boot_path.h"
#include "archdep_pref_path.h"
#include "archdep_home_path.h"
#include "archdep_default_joymap_file_name.h"

/* FIXME: includes for OS/2 */
/* FIXME: includes for beos */
/* FIXME: includes for amiga */

#ifdef UNIX_COMPILE
char *archdep_default_joymap_file_name(void)
{
    const char *pref_path = archdep_pref_path();
    if (pref_path == NULL) {
        const char *home;

        home = archdep_home_path();
        return util_concat(home, "/.vice/" KBD_PORT_PREFIX "-joymap-", machine_get_name(), ".vjm", NULL);
    } else {
        return util_concat(pref_path, "/" KBD_PORT_PREFIX "-joymap-", machine_get_name(), ".vjm", NULL);
    }
}
#endif

#if defined(ARCHDEP_OS_WINDOWS)
char *archdep_default_joymap_file_name(void)
{
    return util_concat(archdep_boot_path(), "\\" KBD_PORT_PREFIX "-joymap-", machine_get_name(), ".vjm", NULL);
}
#endif

#if defined(ARCHDEP_OS_BEOS) || defined(ARCHDEP_OS_AMIGA)
char *archdep_default_joymap_file_name(void)
{
    static char *fname;

    lib_free(fname);
    fname = util_concat(archdep_boot_path(), "/" KBD_PORT_PREFIX "-joymap-", machine_get_name(), ".vjm", NULL);
    return fname;
}
#endif


