/*
 * debug_gtk3.c - Gtk3 port debugging code
 *
 * Written by
 *  Bas Wassink <b.wassink@ziggo.nl>
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

/** \file   src/arch/gtk3/debug_gtk3.h
 * \brief   Debugging code for the Gtk3 native port - header
 */

#include <vice.h>
#include "config.h"

#ifndef HAVE_DEBUG_GTK3_H
# define HAVE_DEBUG_GTK3_H

/* HAVE_DEBUG_NATIVE_GTK3 comes from configure */
# ifdef HAVE_DEBUG_GTK3UI

/* __func__ is non-standard */
#  ifdef __func__
#   define __func__ "<undefined>"
#  endif

#  define VICE_GTK3_FUNC_ENTERED(X) \
    printf("GTK3:%s:%d: %s() entered\n", \
            __FILE__, __LINE__, __func__)
# else
#  define VICE_GTK3_FUNC_ENTERED(X)
# endif
#endif


