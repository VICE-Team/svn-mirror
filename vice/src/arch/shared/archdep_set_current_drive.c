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

#if defined(ARCHDEP_OS_WINDOWS)
# include <direct.h>
#endif

#include "archdep.h"
#include "ui.h"

/* FIXME: includes for amiga */

#if defined(ARCHDEP_OS_WINDOWS)

/* FIXME: is this needed* */
#ifdef SDL_CHOOSE_DRIVES
void archdep_set_current_drive(const char *drive)
{
    if (_chdir(drive)) {
        ui_error("Failed to change drive to %s", drive);
    }
}
#endif

#endif

#if defined(ARCHDEP_OS_AMIGA)

/* FIXME: is this needed* */
#ifdef SDL_CHOOSE_DRIVES
void archdep_set_current_drive(const char *drive)
{
    BPTR lck = Lock(drive, ACCESS_READ);

    if (lck) {
        CurrentDir(lck);
        UnLock(lck);
    } else {
        ui_error("Failed to change to drive %s", drive);
    }
}
#endif

#endif
