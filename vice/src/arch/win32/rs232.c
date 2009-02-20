/*
 * rs232.c - RS232 emulation.
 *
 * Written by
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

/*
 * The RS232 emulation captures the bytes sent to the RS232 interfaces
 * available (currently ACIA 6551, std C64 and Daniel Dallmanns fast RS232
 * with 9600 Baud).
 *
 * I/O is done to a socket.  If the socket isnt connected, no data
 * is read and written data is discarded.
 */

#undef        DEBUG
/* #define DEBUG */

#include "vice.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

#include "log.h"
#include "rs232.h"
#include "rs232win.h"
#include "types.h"
#include "util.h"

#ifdef DEBUG
# define DEBUG_LOG_MESSAGE(_xxx) log_message _xxx
#else
# define DEBUG_LOG_MESSAGE(_xxx)
#endif

/* ------------------------------------------------------------------------- */

enum { RS232_IS_PHYSICAL_DEVICE = 0x4000 };

/* ------------------------------------------------------------------------- */

int rs232_resources_init(void)
{
    rs232dev_resources_init();
    rs232net_resources_init();
    return 0;
}

void rs232_resources_shutdown(void)
{
    rs232dev_resources_shutdown();
    rs232net_resources_shutdown();
}

int rs232_cmdline_options_init(void)
{
    rs232dev_cmdline_options_init();
    rs232net_cmdline_options_init();
    return 0;
}

/* ------------------------------------------------------------------------- */

/* initializes all RS232 stuff */
void rs232_init(void)
{
    rs232dev_init();
    rs232net_init();
}

/* reset RS232 stuff */
void rs232_reset(void)
{
    rs232dev_reset();
    rs232net_reset();
}

/*! \internal find out if the rs232 channel is for a physical device (COMx:) or for networking.
 *
 * A RS232 channel is for a physical device if its name starts with "\\\\.\\COM"
 *
 */
static int rs232_is_physical_device(int device)
{
    if (strnicmp(rs232_devfile[device], "\\\\.\\COM", (sizeof "\\\\.\\COM") - 1) == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/* opens a rs232 window, returns handle to give to functions below. */
int rs232_open(int device)
{
    int ret;

    assert(device < RS232_NUM_DEVICES);

    if (rs232_is_physical_device(device)) {
        ret = rs232dev_open(device);
        if (ret >= 0) {
            ret |= RS232_IS_PHYSICAL_DEVICE;
        }
    }
    else {
        ret = rs232net_open(device);
    }
    return ret;
}

/* closes the rs232 window again */
void rs232_close(int fd)
{
    if (fd & RS232_IS_PHYSICAL_DEVICE)
        rs232dev_close(fd & ~RS232_IS_PHYSICAL_DEVICE);
    else
        rs232net_close(fd);
}

/* sends a byte to the RS232 line */
int rs232_putc(int fd, BYTE b)
{
    if (fd & RS232_IS_PHYSICAL_DEVICE)
        return rs232dev_putc(fd & ~RS232_IS_PHYSICAL_DEVICE, b);
    else
        return rs232net_putc(fd, b);
}

/* gets a byte to the RS232 line, returns !=0 if byte received, byte in *b. */
int rs232_getc(int fd, BYTE * b)
{
    if (fd & RS232_IS_PHYSICAL_DEVICE)
        return rs232dev_getc(fd & ~RS232_IS_PHYSICAL_DEVICE, b);
    else
        return rs232net_getc(fd, b);
}

/* set the status lines of the RS232 device */
int rs232_set_status(int fd, enum rs232handshake_out status)
{
    if (fd & RS232_IS_PHYSICAL_DEVICE)
        return rs232dev_set_status(fd & ~RS232_IS_PHYSICAL_DEVICE, status);
    else
        return rs232net_set_status(fd, status);
}

/* get the status lines of the RS232 device */
enum rs232handshake_in rs232_get_status(int fd)
{
    if (fd & RS232_IS_PHYSICAL_DEVICE)
        return rs232dev_get_status(fd & ~RS232_IS_PHYSICAL_DEVICE);
    else
        return rs232net_get_status(fd);
}

/* set the bps rate of the physical device */
void rs232_set_bps(int fd, unsigned int bps)
{
    if (fd & RS232_IS_PHYSICAL_DEVICE)
        rs232dev_set_bps(fd & ~RS232_IS_PHYSICAL_DEVICE, bps);
}
