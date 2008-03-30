/*
 * fsdevice-write.c - File system device.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Olaf Seibert <rhialto@mbfys.kun.nl>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#include "vice.h"

#include "fsdevice-flush.h"
#include "fsdevice-write.h"
#include "fsdevicetypes.h"
#include "types.h"
#include "vdrive-command.h"
#include "vdrive.h"


int fsdevice_write(vdrive_t *vdrive, BYTE data, unsigned int secondary)
{
    if (secondary == 15)
        return fsdevice_flush_write_byte(vdrive, data);

    if (fs_info[secondary].mode != Write && fs_info[secondary].mode != Append)
        return FLOPPY_ERROR;

    if (fs_info[secondary].fd) {
        fputc(data, fs_info[secondary].fd);
        return FLOPPY_COMMAND_OK;
    };

    return FLOPPY_ERROR;
}

