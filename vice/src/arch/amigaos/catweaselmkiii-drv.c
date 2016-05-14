/*
 * catweaselmkiii-drv.c - AmigaOS specific cw3 driver.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

static int cw_use_device = 0;

#ifdef HAVE_PROTO_OPENPCI_H
static int cw_use_openpci = 0;
#endif

#ifdef AMIGA_OS4
static int cw_use_os4 = 0;
#endif

#include "catweaselmkiii.h"
#include "cw.h"
#include "loadlibs.h"
#include "types.h"

/* read value from SIDs */
int catweaselmkiii_drv_read(WORD addr, int chipno)
{
    if (cw_use_device) {
        return cw_device_read(addr, chipno);
    }

#ifdef HAVE_PROTO_OPENPCI_H
    if (cw_use_openpci) {
        return cw_openpci_read(addr, chipno);
    }
#endif

#ifdef AMIGA_OS4
    if (cw_use_os4) {
        return cw_os4_read(addr, chipno);
    }
#endif

    return 0;
}

/* write value into SID */
void catweaselmkiii_drv_store(WORD addr, BYTE val, int chipno)
{
    if (cw_use_device) {
        cw_device_store(addr, val, chipno);
    }

#ifdef HAVE_PROTO_OPENPCI_H
    if (cw_use_openpci) {
        cw_openpci_store(addr, val, chipno);
    }
#endif

#ifdef AMIGA_OS4
    if (cw_use_os4) {
        cw_os4_store(addr, val, chipno);
    }
#endif
}

int catweaselmkiii_drv_open(void)
{
    int rc = cw_device_open();

    if (rc == 1) {
        cw_use_device = 1;
        return 0;
    }

#if defined(HAVE_PROTO_OPENPCI_H) || defined(AMIGA_OS4)
    if (!pci_lib_loaded) {
        return -1;
    }
#endif

#ifdef HAVE_PROTO_OPENPCI_H
    rc = cw_openpci_open();
    if (rc == 1) {
        cw_use_openpci = 1;
        return 0;
    }
#endif

#ifdef AMIGA_OS4
    rc = cw_os4_open();
    if (rc == 1) {
        cw_use_os4 = 1;
        return 0;
    }
#endif

    return -1;
}

int catweaselmkiii_drv_close(void)
{
    if (cw_use_device) {
        cw_device_close();
        cw_use_device = 0;
    }

#ifdef HAVE_PROTO_OPENPCI_H
    if (cw_use_openpci) {
        cw_openpci_close();
        cw_use_openpci = 0;
    }
#endif

#ifdef AMIGA_OS4
    if (cw_use_os4) {
        cw_os4_close();
        cw_use_os4 = 0;
    }
#endif

    return 0;
}

int catweaselmkiii_drv_available(void)
{
    int i = catweaselmkiii_open();

    if (i != -1) {
        return 1;
    }
    return 0;
}
