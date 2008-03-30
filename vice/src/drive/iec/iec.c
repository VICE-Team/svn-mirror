/*
 * ieee.c
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

#include "ciad.h"
#include "drivetypes.h"
#include "iec.h"
#include "memiec.h"
#include "viad.h"


void iec_drive_init(struct drive_context_s *drv)
{
    cia1571_init(drv);
    cia1581_init(drv);
}

void iec_drive_reset(struct drive_context_s *drv)
{
    cia1571_reset(drv);
    cia1581_reset(drv);
}

void iec_drive_mem_init(struct drive_context_s *drv, unsigned int type)
{
    memiec_init(drv, type);
}

void iec_drive_setup_context(struct drive_context_s *drv)
{
    cia1571_setup_context(drv);
    cia1581_setup_context(drv);
}

int iec_drive_snapshot_read(struct drive_context_s *ctxptr,
                            struct snapshot_s *s)
{
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1571) {
        if (via1d_snapshot_read_module(ctxptr, s) < 0
            || via2d_snapshot_read_module(ctxptr, s) < 0
            || cia1571_snapshot_read_module(ctxptr, s) < 0)
            return -1;
    }

    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1581) {
        if (cia1581_snapshot_read_module(ctxptr, s) < 0)
            return -1;
    }

    return 0;
}

int iec_drive_snapshot_write(struct drive_context_s *ctxptr,
                             struct snapshot_s *s)
{
    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1571) {
        if (via1d_snapshot_write_module(ctxptr, s) < 0
            || via2d_snapshot_write_module(ctxptr, s) < 0
            || cia1571_snapshot_write_module(ctxptr, s) < 0)
            return -1;
    }

    if (ctxptr->drive_ptr->type == DRIVE_TYPE_1581) {
        if (cia1581_snapshot_write_module(ctxptr, s) < 0)
            return -1;
    }

    return 0;
}

