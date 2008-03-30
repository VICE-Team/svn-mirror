/*
 * iec.c
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

#include <stdlib.h>

#include "ciad.h"
#include "drivetypes.h"
#include "iec-cmdline-options.h"
#include "iec-resources.h"
#include "iec.h"
#include "iecrom.h"
#include "jobcode1581.h"
#include "lib.h"
#include "memiec.h"
#include "resources.h"
#include "viad.h"
#include "wd1770.h"


int iec_drive_resources_init(void)
{
    return iec_resources_init();
}

int iec_drive_cmdline_options_init(void)
{
    return iec_cmdline_options_init();
}

void iec_drive_init(struct drive_context_s *drv)
{
    iecrom_init();
    cia1571_init(drv);
    cia1581_init(drv);
    wd1770d_init(drv);
    jobcode1581_init();
}

void iec_drive_reset(struct drive_context_s *drv)
{
    cia1571_reset(drv);
    cia1581_reset(drv);
    wd1770d_reset(drv);
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

void iec_drive_idling_method(unsigned int dnr)
{
    char *tmp;

    tmp = lib_msprintf("Drive%iIdleMethod", dnr + 8);

    resources_touch(tmp);

    lib_free(tmp);
}

void iec_drive_vsync_hook(void)
{
    wd1770_vsync_hook();
}

void iec_drive_handle_job_code(unsigned int dnr)
{
    jobcode1581_handle_job_code(dnr);
}

void iec_drive_rom_load(void)
{
    iecrom_load_1541();
    iecrom_load_1541ii();
    iecrom_load_1571();
    iecrom_load_1581();
}

void iec_drive_rom_setup_image(unsigned int dnr)
{
    iecrom_setup_image(dnr);
}

int iec_drive_rom_read(unsigned int type, WORD addr, BYTE *data)
{
    return iecrom_read(type, addr, data);
}

int iec_drive_rom_check_loaded(unsigned int type)
{
    return iecrom_check_loaded(type);
}

void iec_drive_rom_do_checksum(unsigned int dnr)
{
    iecrom_do_checksum(dnr);
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

int iec_drive_image_attach(struct disk_image_s *image, unsigned int unit)
{
    return wd1770_attach_image(image, unit);
}

int iec_drive_image_detach(struct disk_image_s *image, unsigned int unit)
{
    return wd1770_detach_image(image, unit);
}

