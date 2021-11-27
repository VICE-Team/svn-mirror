/** \file   archdep_real_path.c
 * \brief   Normalize path names
 * \author  Michael C. Martin <mcmartin@gmail.com>
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
#include "archdep_defs.h"

#include <stdlib.h>

#include "archdep_real_path.h"


/** \brief  Resolve \a pathname to its canonicalized absolute form
 *
 * \param[in]   pathname             pathname to normalize
 * \param[out]  resolved_pathname    buffer for result (must be at least PATH_MAX long)
 *
 * \return      resolved_pathname on success, NULL on failure. On failure, contents of
 *              resolved_pathname are undefined.
 */
char *archdep_real_path(const char *pathname, char *resolved_pathname)
{
#ifdef ARCHDEP_OS_WINDOWS
    return _fullpath(resolved_pathname, pathname, _MAX_PATH);
#else
    return realpath(pathname, resolved_pathname);
#endif
}
