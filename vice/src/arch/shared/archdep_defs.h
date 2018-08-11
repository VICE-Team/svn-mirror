/** \file   archdep_defs.h
 * \brief   Defines, enums and types used by the archdep functions
 *
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

#ifndef VICE_ARCHDEP_DEFS_H
#define VICE_ARCHDEP_DEFS_H

#include "vice.h"


/** \brief  Arch-dependent directory separator used in paths
 */
#if defined(WIN32_COMPILE) || defined(OS2_COMPILE) || \
    defined(MSDOS) || defined(_MSDOS) || defined(__MSDOS__) || defined(__DOS__)
# define ARCHDEP_DIR_SEPARATOR  '\\'
#elif defined(macintosh) || defined(Macintosh)
# define ARCHDEP_DIR_SEPARATOR  ':'
#else
# define ARCHDEP_DIR_SEPARATOR  '/'
#endif


#endif
