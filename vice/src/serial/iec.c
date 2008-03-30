/*
 * iec.c - Common IEC bus emulation.
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

#include "attach.h"
#include "fsdrive.h"
#include "iec.h"
#include "realdevice.h"
#include "types.h"
#include "vdrive.h"


void iec_open(unsigned int device, BYTE b)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        realdevice_open(device, b);
    else
#endif
        fsdrive_open(b);
}

void iec_close(unsigned int device, BYTE b)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        realdevice_close(device, b);
    else
#endif
        fsdrive_close(b);
}

void iec_listentalk(unsigned int device, BYTE b)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        realdevice_listentalk(device, b);
    else
#endif
        fsdrive_listentalk(b);
}

void iec_unlisten(unsigned int device)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        realdevice_unlisten();
    else
#endif
        fsdrive_unlisten();
}

void iec_untalk(unsigned int device)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        realdevice_untalk();
    else
#endif
        fsdrive_untalk();
}

void iec_write(unsigned int device, BYTE data)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        realdevice_write(data);
    else
#endif
        fsdrive_write(data);
}

BYTE iec_read(unsigned int device)
{
#if HAVE_OPENCBM
    vdrive_t *vdrive;

    vdrive = (vdrive_t *)file_system_get_vdrive(device & 0x0f);

    if (vdrive->device == VDRIVE_DEVICE_REAL)
        return realdevice_read();
    else
#endif
        return fsdrive_read();
}

void iec_reset(void)
{
    fsdrive_reset();
#if HAVE_OPENCBM
    realdevice_reset();
#endif
}

void iec_init(void)
{
    fsdrive_init();
#if HAVE_OPENCBM
    realdevice_init();
#endif
}

