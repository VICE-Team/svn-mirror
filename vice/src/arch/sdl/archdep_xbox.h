/*
 * archdep_xbox.h - Architecture dependant defines.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef VICE_ARCHDEP_XBOX_H
#define VICE_ARCHDEP_XBOX_H

#include "archapi.h"

/* Filesystem dependant operators.  */
#define FSDEVICE_DEFAULT_DIR "."
#define FSDEV_DIR_SEP_STR    "\\"
#define FSDEV_DIR_SEP_CHR    '\\'
#define FSDEV_EXT_SEP_STR    "."
#define FSDEV_EXT_SEP_CHR    '.'

/* Path separator.  */
#define ARCHDEP_FINDPATH_SEPARATOR_CHAR   ';'
#define ARCHDEP_FINDPATH_SEPARATOR_STRING ";"

/* Modes for fopen().  */
#define MODE_READ              "r"
#define MODE_READ_TEXT         "rt"
#define MODE_READ_WRITE        "r+"
#define MODE_WRITE             "w"
#define MODE_WRITE_TEXT        "wt"
#define MODE_APPEND            "a"
#define MODE_APPEND_READ_WRITE "a+"

/* Printer default devices.  */
#define ARCHDEP_PRINTER_DEFAULT_DEV1 "viceprnt.out"
#define ARCHDEP_PRINTER_DEFAULT_DEV2 "LPT1:"
#define ARCHDEP_PRINTER_DEFAULT_DEV3 "LPT2:"

/* Default RS232 devices.  */
#define ARCHDEP_RS232_DEV1 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV2 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV3 "10.0.0.1:25232"
#define ARCHDEP_RS232_DEV4 "10.0.0.1:25232"

/* Default location of raw disk images.  */
#define ARCHDEP_RAWDRIVE_DEFAULT "A:"

/* Access types */
#define ARCHDEP_R_OK 4
#define ARCHDEP_W_OK 2
#define ARCHDEP_X_OK 1
#define ARCHDEP_F_OK 0

/* Standard line delimiter.  */
#define ARCHDEP_LINE_DELIMITER "\r\n"

/* Ethernet default device */
#define ARCHDEP_ETHERNET_DEFAULT_DEVICE ""

/* Default sound fragment size */
#define ARCHDEP_SOUND_FRAGMENT_SIZE SOUND_FRAGMENT_MEDIUM

extern void archdep_workaround_nop(const char *otto);

/*
    FIXME: confirm wether SIGPIPE must be handled or not. if the emulator quits
           or crashes when the connection is closed, you might have to install
           a signal handler which calls monitor_abort().

           see archdep_unix.c and bug #3201796
*/
#define archdep_signals_init(x)
#define archdep_signals_pipe_set()
#define archdep_signals_pipe_unset()

/* what to use to return an error when a socket error happens */
#define ARCHDEP_SOCKET_ERROR errno

/* Keyword to use for a static prototype */
#define STATIC_PROTOTYPE static

#endif
