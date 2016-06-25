/*
 * hardsid-drv.c - Windows specific hardsid driver.
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

/* Tested and confirmed working on:

 - Windows 95C (ISA HardSID, hardsid.dll)
 - Windows 95C (ISA HardSID, Direct ISA I/O, inpout32.dll is incompatible with windows 95)
 - Windows 98SE (ISA HardSID, hardsid.dll)
 - Windows 98SE (ISA HardSID, inpout32.dll ISA I/O)
 - Windows 98SE (ISA HardSID, Direct ISA I/O)
 */

#include "vice.h"

#ifdef HAVE_HARDSID

#include "hardsid.h"
#include "hs.h"
#include "types.h"

static int use_hs_isa = 0;
static int use_hs_dll = 0;

void hardsid_drv_reset(void)
{
    if (use_hs_dll) {
        hs_dll_reset();
    }
}

int hardsid_drv_open(void)
{
    int retval;

    retval = hs_dll_open();
    if (!retval) {
        use_hs_dll = 1;
        return 0;
    }

    retval = hs_isa_open();
    if (!retval) {
        use_hs_isa = 1;
        return 0;
    }
    return -1;
}

int hardsid_drv_close(void)
{
    if (use_hs_isa) {
        use_hs_isa = 0;
        hs_isa_close();
    }
    if (use_hs_dll) {
        use_hs_dll = 0;
        hs_dll_close();
    }
    return 0;
}

int hardsid_drv_read(WORD addr, int chipno)
{
    if (use_hs_isa) {
        return hs_isa_read(addr, chipno);
    }
    if (use_hs_dll) {
        return hs_dll_read(addr, chipno);
    }
    return 0;
}

void hardsid_drv_store(WORD addr, BYTE val, int chipno)
{
    if (use_hs_isa) {
        hs_isa_store(addr, val, chipno);
    }
    if (use_hs_dll) {
        hs_dll_store(addr, val, chipno);
    }
}

int hardsid_drv_available(void)
{
    if (use_hs_isa) {
        return hs_isa_available();
    }

    if (use_hs_dll) {
        return hs_dll_available();
    }
    return 0;
}

void hardsid_drv_set_device(unsigned int chipno, unsigned int device)
{
    if (use_hs_dll) {
        hs_dll_set_device(chipno, device);
    }
}

/* ---------------------------------------------------------------------*/

void hardsid_drv_state_read(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
    if (use_hs_isa) {
        hs_isa_state_read(chipno, sid_state);
    }

    if (use_hs_dll) {
        hs_dll_state_read(chipno, sid_state);
    }
}

void hardsid_drv_state_write(int chipno, struct sid_hs_snapshot_state_s *sid_state)
{
    if (use_hs_isa) {
        hs_isa_state_write(chipno, sid_state);
    }

    if (use_hs_dll) {
        hs_dll_state_write(chipno, sid_state);
    }
}
#endif
