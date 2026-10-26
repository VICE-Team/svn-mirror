/** \file   archdep_serial_control_open.c
 * \brief   Open a host serial port for modem-control use
 * \author  Felix Erckenbrecht <code@eligs.de>
 *
 * Open a host serial device (a tty on *nix, a COM port on Windows) so its
 * RTS or DTR modem-control line can be driven with
 * archdep_serial_control_set(). Used by the CASCOM tape-port modem to key a
 * radio (PTT).
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

#include "lib.h"
#include "util.h"

#include "archdep_serial_control_open.h"

#if defined(WINDOWS_COMPILE)

#include <windows.h>

/** \brief  Open a host serial port for modem-control use
 *
 * \param[in]   device  COM port name ("COM3" or "\\\\.\\COM12")
 *
 * \return  port handle, or NULL on failure
 */
archdep_serial_control_t *archdep_serial_control_open(const char *device)
{
    archdep_serial_control_t *ctl;
    HANDLE handle;
    char *path;

    if (device == NULL || device[0] == '\0') {
        return NULL;
    }
    /* plain "COMn" names only work for n <= 9; the \\.\ prefix works for all */
    if (device[0] != '\\') {
        path = util_concat("\\\\.\\", device, NULL);
    } else {
        path = lib_strdup(device);
    }
    handle = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                        OPEN_EXISTING, 0, NULL);
    lib_free(path);
    if (handle == INVALID_HANDLE_VALUE) {
        return NULL;
    }
    ctl = lib_malloc(sizeof(*ctl));
    ctl->handle = handle;
    ctl->fd = -1;
    return ctl;
}

#elif defined(UNIX_COMPILE)

#include <fcntl.h>
#include <unistd.h>

/** \brief  Open a host serial port for modem-control use
 *
 * \param[in]   device  tty device path (e.g. "/dev/ttyUSB0")
 *
 * \return  port handle, or NULL on failure
 */
archdep_serial_control_t *archdep_serial_control_open(const char *device)
{
    archdep_serial_control_t *ctl;
    int fd;

    if (device == NULL || device[0] == '\0') {
        return NULL;
    }
    fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        return NULL;
    }
    ctl = lib_malloc(sizeof(*ctl));
    ctl->handle = NULL;
    ctl->fd = fd;
    return ctl;
}

#else

/** \brief  Open a host serial port for modem-control use
 *
 * \param[in]   device  serial device name
 *
 * \return  NULL; no serial modem-control support on this platform
 */
archdep_serial_control_t *archdep_serial_control_open(const char *device)
{
    (void)device;
    return NULL;
}

#endif
