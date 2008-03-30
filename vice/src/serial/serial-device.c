/*
 * serial-device.c
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

#include "vice.h"

#include <stdio.h>

#include "attach.h"
#include "diskimage.h"
#include "serial.h"
#include "vdrive.h"


unsigned int serial_device_get_realdevice_state(unsigned int unit)
{
    vdrive_t *vdrive;

    if (unit < 8)
        return 0;

    vdrive = (vdrive_t *)file_system_get_vdrive(unit);

    if (vdrive->image == NULL)
        return 0;

    if (vdrive->image->device == DISK_IMAGE_DEVICE_REAL)
        return 1;

    return 0;
}

