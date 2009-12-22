/*
 * epyxfastload.c - Cartridge handling, EPYX Fastload cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Fixed by
 *  Ingo Korb <ingo@akana.de>
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
#include <stdlib.h>
#include <string.h>

#include "alarm.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "epyxfastload.h"
#include "maincpu.h"
#include "types.h"

/* The Epyx FastLoad cart discharges a capacitor on ROML and IO1 accesses. */
/* This constant defines the number of cycles it takes to recharge it.     */
#define EPYX_ROM_CYCLES 512

struct alarm_s *epyxrom_alarm;

static const c64export_resource_t export_res_epyx = {
    "Epyx Fastload", 0, 0
};

static void epyxfastload_trigger_access(void)
{
    /* Discharge virtual capacitor, enable rom */
    alarm_unset(epyxrom_alarm);
    alarm_set(epyxrom_alarm, maincpu_clk + EPYX_ROM_CYCLES);
    cartridge_config_changed(0, 0, CMODE_READ);
}

static void epyxfastload_alarm_handler(CLOCK offset, void *data)
{
    /* Virtual capacitor charged, disable rom */
    alarm_unset(epyxrom_alarm);
    cartridge_config_changed(2, 2, CMODE_READ);
}


void epyxfastload_io1_read(void)
{
    /* IO1 discharges the capacitor, but does nothing else */
    io_source = IO_SOURCE_EPYX_FASTLOAD;

    epyxfastload_trigger_access();
}

BYTE REGPARM1 epyxfastload_io2_read(WORD addr)
{
    /* IO2 allows access to the last 256 bytes of the rom */
    io_source = IO_SOURCE_EPYX_FASTLOAD;

    return roml_banks[0x1f00 + (addr & 0xff)];
}

BYTE REGPARM1 epyxfastload_roml_read(WORD addr)
{
    /* ROML accesses also discharge the capacitor */
    epyxfastload_trigger_access();

    return roml_banks[(addr & 0x1fff)];
}

void epyxfastload_reset(void)
{
    /* RESET discharges the capacitor so the rom is visible */
    epyxfastload_trigger_access();
}

void epyxfastload_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

void epyxfastload_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

int epyxfastload_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(rawcart, 0x2000, 1, fd) < 1) {
        return -1;
    }

    if (c64export_add(&export_res_epyx) < 0) {
        return -1;
    }

    epyxrom_alarm = alarm_new(maincpu_alarm_context, "EPYXCartRomAlarm", epyxfastload_alarm_handler, NULL);

    return 0;
}

void epyxfastload_detach(void)
{
    alarm_destroy(epyxrom_alarm);
    c64export_remove(&export_res_epyx);
}
