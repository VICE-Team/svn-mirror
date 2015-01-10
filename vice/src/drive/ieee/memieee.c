/*
 * memieee.c - IEEE drive memory.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Kajtar Zsolt <soci@c64.rulez.org>
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

#include "drive-check.h"
#include "drivemem.h"
#include "driverom.h"
#include "drivetypes.h"
#include "memieee.h"
#include "riotd.h"
#include "types.h"
#include "via1d2031.h"
#include "viad.h"


static BYTE drive_read_rom(drive_context_t *drv, WORD address)
{
    return drv->drive->rom[address & 0x7fff];
}

static BYTE drive_read_2031ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[address & 0x7ff];
}

static void drive_store_2031ram(drive_context_t *drv, WORD address, BYTE value)
{
    drv->cpud->drive_ram[address & 0x7ff] = value;
}

static BYTE drive_read_zero(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[address & 0xff];
}

static void drive_store_zero(drive_context_t *drv, WORD address, BYTE value)
{
    drv->cpud->drive_ram[address & 0xff] = value;
}

/* SFD1001 specific memory.  */

static BYTE drive_read_1001_io(drive_context_t *drv, WORD address)
{
    if (address & 0x80) {
        return riot2_read(drv, address);
    }
    return riot1_read(drv, address);
}

static void drive_store_1001_io(drive_context_t *drv, WORD address, BYTE byte)
{
    if (address & 0x80) {
        riot2_store(drv, address, byte);
    } else {
        riot1_store(drv, address, byte);
    }
}

static BYTE drive_read_1001zero_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[address & 0xff];
}

static void drive_store_1001zero_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[address & 0xff] = byte;
}

static BYTE drive_read_1001buffer1_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[(address & 0x7ff) + 0x100];
}

static void drive_store_1001buffer1_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[(address & 0x7ff) + 0x100] = byte;
}

static BYTE drive_read_1001buffer2_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[(address & 0x7ff) + 0x900];
}

static void drive_store_1001buffer2_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[(address & 0x7ff) + 0x900] = byte;
}

static BYTE drive_read_2040buffer1_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[(address & 0x3ff) + 0x100];
}

static void drive_store_2040buffer1_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[(address & 0x3ff) + 0x100] = byte;
}

static BYTE drive_read_2040buffer2_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[(address & 0x3ff) + 0x500];
}

static void drive_store_2040buffer2_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[(address & 0x3ff) + 0x500] = byte;
}

static BYTE drive_read_2040buffer3_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[(address & 0x3ff) + 0x900];
}

static void drive_store_2040buffer3_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[(address & 0x3ff) + 0x900] = byte;
}

static BYTE drive_read_2040buffer4_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[(address & 0x3ff) + 0xd00];
}

static void drive_store_2040buffer4_ram(drive_context_t *drv, WORD address, BYTE byte)
{
    drv->cpud->drive_ram[(address & 0x3ff) + 0xd00] = byte;
}

void memieee_init(struct drive_context_s *drv, unsigned int type)
{
    drivecpud_context_t *cpud = drv->cpud;

    switch (type) {
    case DRIVE_TYPE_2031: 
        drv->cpu->pageone = cpud->drive_ram + 0x100;
        drivemem_set_func(cpud, 0x00, 0x01, drive_read_zero, drive_store_zero);
        drivemem_set_func(cpud, 0x01, 0x08, drive_read_2031ram, drive_store_2031ram);
        drivemem_set_func(cpud, 0x18, 0x1c, via1d2031_read, via1d2031_store);
        drivemem_set_func(cpud, 0x1c, 0x20, via2d_read, via2d_store);
        drivemem_set_func(cpud, 0x80, 0x100, drive_read_rom, NULL);
        return;
    case DRIVE_TYPE_1001:
        drv->cpu->pageone = cpud->drive_ram;
        drivemem_set_func(cpud, 0x00, 0x02, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x02, 0x04, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x04, 0x06, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x06, 0x08, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x08, 0x0a, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x0a, 0x0c, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x0c, 0x0e, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x0e, 0x10, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x10, 0x30, drive_read_1001buffer1_ram, drive_store_1001buffer1_ram);
        drivemem_set_func(cpud, 0x30, 0x50, drive_read_1001buffer2_ram, drive_store_1001buffer2_ram);
        drivemem_set_func(cpud, 0x80, 0x100, drive_read_rom, NULL);
        return;
    case DRIVE_TYPE_8050:
    case DRIVE_TYPE_8250:
        drv->cpu->pageone = cpud->drive_ram;
        drivemem_set_func(cpud, 0x00, 0x02, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x02, 0x04, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x04, 0x06, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x06, 0x08, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x08, 0x0a, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x0a, 0x0c, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x0c, 0x0e, drive_read_1001zero_ram, drive_store_1001zero_ram);
        drivemem_set_func(cpud, 0x0e, 0x10, drive_read_1001_io, drive_store_1001_io);
        drivemem_set_func(cpud, 0x10, 0x20, drive_read_2040buffer1_ram, drive_store_2040buffer1_ram);
        drivemem_set_func(cpud, 0x20, 0x30, drive_read_2040buffer2_ram, drive_store_2040buffer2_ram);
        drivemem_set_func(cpud, 0x30, 0x40, drive_read_2040buffer3_ram, drive_store_2040buffer3_ram);
        drivemem_set_func(cpud, 0x40, 0x50, drive_read_2040buffer4_ram, drive_store_2040buffer4_ram);
        drivemem_set_func(cpud, 0x80, 0x100, drive_read_rom, NULL);
        return;
    case DRIVE_TYPE_2040:
        drivemem_set_func(cpud, 0x60, 0x80, drive_read_rom, NULL);
        drivemem_set_func(cpud, 0xe0, 0x100, drive_read_rom, NULL);
        break;
    case DRIVE_TYPE_3040:
    case DRIVE_TYPE_4040:
        drivemem_set_func(cpud, 0x50, 0x80, drive_read_rom, NULL);
        drivemem_set_func(cpud, 0xd0, 0x100, drive_read_rom, NULL);
        break;
    default:
        return;
    }

    drv->cpu->pageone = cpud->drive_ram;
    drivemem_set_func(cpud, 0x00, 0x02, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x02, 0x04, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x04, 0x06, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x06, 0x08, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x08, 0x0a, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x0a, 0x0c, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x0c, 0x0e, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x0e, 0x10, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x10, 0x20, drive_read_2040buffer1_ram, drive_store_2040buffer1_ram);
    drivemem_set_func(cpud, 0x20, 0x30, drive_read_2040buffer2_ram, drive_store_2040buffer2_ram);
    drivemem_set_func(cpud, 0x30, 0x40, drive_read_2040buffer3_ram, drive_store_2040buffer3_ram);
    drivemem_set_func(cpud, 0x40, 0x50, drive_read_2040buffer4_ram, drive_store_2040buffer4_ram);
    drivemem_set_func(cpud, 0x80, 0x82, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x82, 0x84, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x84, 0x86, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x86, 0x88, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x88, 0x8a, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x8a, 0x8c, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x8c, 0x8e, drive_read_1001zero_ram, drive_store_1001zero_ram);
    drivemem_set_func(cpud, 0x8e, 0x90, drive_read_1001_io, drive_store_1001_io);
    drivemem_set_func(cpud, 0x90, 0xa0, drive_read_2040buffer1_ram, drive_store_2040buffer1_ram);
    drivemem_set_func(cpud, 0xa0, 0xb0, drive_read_2040buffer2_ram, drive_store_2040buffer2_ram);
    drivemem_set_func(cpud, 0xb0, 0xc0, drive_read_2040buffer3_ram, drive_store_2040buffer3_ram);
    drivemem_set_func(cpud, 0xc0, 0xd0, drive_read_2040buffer4_ram, drive_store_2040buffer4_ram);
}
