/** \file   archdep_serial_control_close.c
 * \brief   Close a host serial modem-control port
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

#include "lib.h"

#include "archdep_serial_control_close.h"

#if defined(WINDOWS_COMPILE)

#include <windows.h>

/** \brief  Close a port opened with archdep_serial_control_open()
 *
 * \param[in]   ctl port handle (NULL is allowed and ignored)
 */
void archdep_serial_control_close(archdep_serial_control_t *ctl)
{
    if (ctl == NULL) {
        return;
    }
    CloseHandle((HANDLE)ctl->handle);
    lib_free(ctl);
}

#elif defined(UNIX_COMPILE)

#include <unistd.h>

/** \brief  Close a port opened with archdep_serial_control_open()
 *
 * \param[in]   ctl port handle (NULL is allowed and ignored)
 */
void archdep_serial_control_close(archdep_serial_control_t *ctl)
{
    if (ctl == NULL) {
        return;
    }
    close(ctl->fd);
    lib_free(ctl);
}

#else

/** \brief  Close a port opened with archdep_serial_control_open()
 *
 * \param[in]   ctl port handle (always NULL on this platform)
 */
void archdep_serial_control_close(archdep_serial_control_t *ctl)
{
    (void)ctl;
}

#endif
