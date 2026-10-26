/** \file   archdep_serial_control.h
 * \brief   Host serial-port modem-control - shared types
 * \author  Felix Erckenbrecht <code@eligs.de>
 *
 * Common constants and the port handle used by the archdep_serial_control_*()
 * functions (one function per file, see archdep_serial_control_open.h,
 * archdep_serial_control_close.h, archdep_serial_control_set.h and
 * archdep_serial_control_list.h).
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

#ifndef VICE_ARCHDEP_SERIAL_CONTROL_H
#define VICE_ARCHDEP_SERIAL_CONTROL_H

#include "archdep_defs.h"

/* Defined when this platform can actually drive serial modem-control lines;
   consumers use it to leave their serial/PTT options out entirely on
   platforms where they could never work. */
#if defined(WINDOWS_COMPILE) || defined(UNIX_COMPILE)
#define ARCHDEP_HAVE_SERIAL_CONTROL
#endif

/* modem-control lines selectable with archdep_serial_control_set() */
#define ARCHDEP_SERIAL_CONTROL_RTS 0
#define ARCHDEP_SERIAL_CONTROL_DTR 1

/* Handle to an open host serial port. The members are platform specific and
   private to the archdep_serial_control_*() implementations. */
struct archdep_serial_control_s {
    void *handle;   /* Windows: the CreateFile() HANDLE */
    int   fd;       /* *nix: the tty file descriptor */
};
typedef struct archdep_serial_control_s archdep_serial_control_t;

#endif
