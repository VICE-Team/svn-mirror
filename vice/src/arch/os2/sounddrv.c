/*
 * sounddrv.c - Implementation of Sound for Vice/2
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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
#include <os2me.h>

#include <ctype.h>

#ifdef WATCOM_COMPILE
#include <mcios2.h>
#endif

#include "log.h"

int sound_err(log_t log, ULONG rc, char *s)
{
    char text[128];

    log_error(log, s);
    mciGetErrorString(rc, text, 128);

    if (!isprint(text[0])) {
        return 1;
    }

    log_error(log, "%s (rc=%li)", text, rc);
    return 1;
}
