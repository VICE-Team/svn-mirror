/*
 * embedded.c - Code for embedding data files.
 *
 * Written by
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

#ifdef USE_EMBEDDED
#include <string.h>

#include "c64mem.h"
#include "machine.h"

int embedded_check_file(const char *name, int minsize, int maxsize)
{
    switch (machine_class) {
        case VICE_MACHINE_C64:
        default:
            if (!strcmp(name, "basic") && minsize == maxsize && minsize == C64_BASIC_ROM_SIZE) {
                return 1;
            }
            if (!strcmp(name, "kernal") && minsize == maxsize && minsize == C64_KERNAL_ROM_SIZE) {
                return 1;
            }
            if (!strcmp(name, "chargen") && minsize == maxsize && minsize == C64_CHARGEN_ROM_SIZE) {
                return 1;
            }
            break;
    }
    return 0;
}

#endif
