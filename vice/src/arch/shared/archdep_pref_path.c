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

#include <stdlib.h>
#include <stdio.h>

#include "archdep.h"

#include "archdep_pref_path.h"
#include "archdep_user_config_path.h"

/* alternate storage of preferences */
static char *pref_path = NULL; /* NULL -> use home_path + ".vice" */

const char *archdep_pref_path(void)
{
    if (pref_path == NULL) {
#ifdef UNIX_COMPILE
        /* unix */
        pref_path = archdep_user_config_path();
#endif
    }
    return pref_path;
}
