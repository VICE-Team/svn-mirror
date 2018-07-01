/** \file   debug_gtk3.h
 * \brief   Gtk3 port debugging code - header
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

#ifndef VICE_DEBUG_GTK3_H
# define VICE_DEBUG_GTK3_H

# include "vice.h"

/* HAVE_DEBUG_GTK3UI comes from configure */
# ifdef HAVE_DEBUG_GTK3UI

#  include <glib.h>

/** \brief  Print debugging info on stdout
 *
 * Works just like g_print() and printf(), except that every line is prefixed
 * with "[debug-gtk3] $FILE:$LINE::$FUNC(): "
 */
#  define debug_gtk3(...) \
    g_print("[debug-gtk3] %s:%d::%s(): ", __FILE__, __LINE__, __func__); \
    g_print(__VA_ARGS__);

# else  /* HAVE_DEBUG_GTK3UI */
#  define debug_gtk3(...)
# endif /* HAVE DEBUG_GTK3UI */

#endif  /* VICE_DEBUG_GTK3_H */
