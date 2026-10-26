/** \file   archdep_serial_control_set.c
 * \brief   Drive a modem-control line of a host serial port
 * \author  Felix Erckenbrecht <code@eligs.de>
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

#include <stddef.h>

#include "archdep_serial_control_set.h"

#if defined(WINDOWS_COMPILE)

#include <windows.h>

/** \brief  Set or clear a modem-control line
 *
 * \param[in]   ctl     port handle (NULL is allowed and ignored)
 * \param[in]   line    ARCHDEP_SERIAL_CONTROL_RTS or ARCHDEP_SERIAL_CONTROL_DTR
 * \param[in]   active  raise (1) or drop (0) the line
 */
void archdep_serial_control_set(archdep_serial_control_t *ctl, int line, int active)
{
    DWORD func;

    if (ctl == NULL) {
        return;
    }
    if (line == ARCHDEP_SERIAL_CONTROL_DTR) {
        func = active ? SETDTR : CLRDTR;
    } else {
        func = active ? SETRTS : CLRRTS;
    }
    EscapeCommFunction((HANDLE)ctl->handle, func);
}

#elif defined(UNIX_COMPILE)

#include <termios.h>
#include <sys/ioctl.h>

/** \brief  Set or clear a modem-control line
 *
 * \param[in]   ctl     port handle (NULL is allowed and ignored)
 * \param[in]   line    ARCHDEP_SERIAL_CONTROL_RTS or ARCHDEP_SERIAL_CONTROL_DTR
 * \param[in]   active  raise (1) or drop (0) the line
 */
void archdep_serial_control_set(archdep_serial_control_t *ctl, int line, int active)
{
    int bits;
    int mask = (line == ARCHDEP_SERIAL_CONTROL_DTR) ? TIOCM_DTR : TIOCM_RTS;

    if (ctl == NULL) {
        return;
    }
    if (ioctl(ctl->fd, TIOCMGET, &bits) < 0) {
        return;
    }
    if (active) {
        bits |= mask;
    } else {
        bits &= ~mask;
    }
    ioctl(ctl->fd, TIOCMSET, &bits);
}

#else

/** \brief  Set or clear a modem-control line
 *
 * \param[in]   ctl     port handle (always NULL on this platform)
 * \param[in]   line    ARCHDEP_SERIAL_CONTROL_RTS or ARCHDEP_SERIAL_CONTROL_DTR
 * \param[in]   active  raise (1) or drop (0) the line
 */
void archdep_serial_control_set(archdep_serial_control_t *ctl, int line, int active)
{
    (void)ctl;
    (void)line;
    (void)active;
}

#endif
