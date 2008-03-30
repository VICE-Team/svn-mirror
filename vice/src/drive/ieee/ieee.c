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

#include "drivetypes.h"
#include "fdc.h"
#include "ieee.h"
#include "memieee.h"
#include "riotd.h"
#include "viad.h"


void ieee_drive_init(struct drive_context_s *drv)
{
    riot1_init(drv);
    riot2_init(drv);
}

void ieee_drive_reset(struct drive_context_s *drv)
{
    riot1_reset(drv);
    riot2_reset(drv);
}

void ieee_drive_mem_init(struct drive_context_s *drv, unsigned int type)
{
    memieee_init(drv, type);
}

void ieee_drive_setup_context(struct drive_context_s *drv)
{
    riot1_setup_context(drv);
    riot2_setup_context(drv);
}

int ieee_drive_snapshot_read(struct drive_context_s *ctxptr,
                             struct snapshot_s *s)
{
    if (DRIVE_IS_OLDTYPE(ctxptr->drive_ptr->type)) {
        if (riot1_snapshot_read_module(ctxptr, s) < 0
            || riot2_snapshot_read_module(ctxptr, s) < 0
            || fdc_snapshot_read_module(s, ctxptr->mynumber) < 0)
            return -1;
    }

    return 0;
}

int ieee_drive_snapshot_write(struct drive_context_s *ctxptr,
                              struct snapshot_s *s)
{
    if (DRIVE_IS_OLDTYPE(ctxptr->drive_ptr->type)) {
        if (riot1_snapshot_write_module(ctxptr, s) < 0
            || riot2_snapshot_write_module(ctxptr, s) < 0
            || fdc_snapshot_write_module(s, ctxptr->mynumber) < 0)
            return -1;
    }

    return 0;
}

void ieee_drive0_parallel_set_atn(int state)
{
    drive0_via_set_atn(state);
    drive0_riot_set_atn(state);
}

void ieee_drive1_parallel_set_atn(int state)
{
    drive1_via_set_atn(state);
    drive1_riot_set_atn(state);
}

