/** \file   archdep_file_exists.c
 * \brief   Check if a file exisys
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
#include <stdlib.h>
#include <errno.h>

#include "archdep.h"
#include "archdep_defs.h"


#ifdef ARCHDEP_OS_UNIX
# include <unistd.h>
#endif

#include "archdep_file_exists.h"


bool archdep_file_exists(const char *path)
{
#ifdef ARCHDEP_OS_UNIX

    if (access(path, F_OK) == 0) {
        return true;
    }
#endif
    return false;

}

