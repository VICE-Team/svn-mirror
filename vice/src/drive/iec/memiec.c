/*
 * memiec.c - IEC drive memory.
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
#include <stdlib.h>

#include "ciad.h"
#include "drivemem.h"
#include "drivetypes.h"
#include "lib.h"
#include "memiec.h"
#include "types.h"
#include "via1d1541.h"
#include "viad.h"
#include "wd1770.h"
#include "via4000.h"
#include "pc8477.h"

static BYTE drive_read_rom(drive_context_t *drv, WORD address)
{
    return drv->drive->rom[address & 0x7fff];
}

static BYTE drive_read_rom_ds1216(drive_context_t *drv, WORD address)
{
    return ds1216e_read(drv->drive->ds1216, address, drv->drive->rom[address & 0x7fff]);
}

static BYTE drive_read_1541ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[address & 0x7ff];
}

static void drive_store_1541ram(drive_context_t *drv, WORD address, BYTE value)
{
    drv->cpud->drive_ram[address & 0x7ff] = value;
}

static BYTE drive_read_1581ram(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[address & 0x1fff];
}

static void drive_store_1581ram(drive_context_t *drv, WORD address, BYTE value)
{
    drv->cpud->drive_ram[address & 0x1fff] = value;
}

static BYTE drive_read_zero(drive_context_t *drv, WORD address)
{
    return drv->cpud->drive_ram[address & 0xff];
}

static void drive_store_zero(drive_context_t *drv, WORD address, BYTE value)
{
    drv->cpud->drive_ram[address & 0xff] = value;
}

static BYTE drive_read_ram2(drive_context_t *drv, WORD address)
{
    return drv->drive->drive_ram_expand2[address & 0x1fff];
}

static void drive_store_ram2(drive_context_t *drv, WORD address, BYTE value)
{
    drv->drive->drive_ram_expand2[address & 0x1fff] = value;
}

static BYTE drive_read_ram4(drive_context_t *drv, WORD address)
{
    return drv->drive->drive_ram_expand4[address & 0x1fff];
}

static void drive_store_ram4(drive_context_t *drv, WORD address, BYTE value)
{
    drv->drive->drive_ram_expand4[address & 0x1fff] = value;
}

static BYTE drive_read_ram6(drive_context_t *drv, WORD address)
{
    return drv->drive->drive_ram_expand6[address & 0x1fff];
}

static void drive_store_ram6(drive_context_t *drv, WORD address, BYTE value)
{
    drv->drive->drive_ram_expand6[address & 0x1fff] = value;
}

static BYTE drive_read_ram8(drive_context_t *drv, WORD address)
{
    return drv->drive->drive_ram_expand8[address & 0x1fff];
}

static void drive_store_ram8(drive_context_t *drv, WORD address, BYTE value)
{
    drv->drive->drive_ram_expand8[address & 0x1fff] = value;
}

static BYTE drive_read_rama(drive_context_t *drv, WORD address)
{
    return drv->drive->drive_ram_expanda[address & 0x1fff];
}

static void drive_store_rama(drive_context_t *drv, WORD address, BYTE value)
{
    drv->drive->drive_ram_expanda[address & 0x1fff] = value;
}

/* ------------------------------------------------------------------------- */

static void realloc_expram(BYTE **expram, int size)
{
    lib_free(*expram);
    *expram = lib_calloc(1, size);
}

void memiec_init(struct drive_context_s *drv, unsigned int type)
{
    drivecpud_context_t *cpud = drv->cpud;

    switch (type) {
    case DRIVE_TYPE_1541:
    case DRIVE_TYPE_1541II:
        drv->cpu->pageone = cpud->drive_ram + 0x100;
        drivemem_set_func(cpud, 0x00, 0x01, drive_read_zero, drive_store_zero);
        drivemem_set_func(cpud, 0x01, 0x08, drive_read_1541ram, drive_store_1541ram);
        drivemem_set_func(cpud, 0x18, 0x1c, via1d1541_read, via1d1541_store);
        drivemem_set_func(cpud, 0x1c, 0x20, via2d_read, via2d_store);
        if (drv->drive->drive_ram2_enabled) {
            realloc_expram(&drv->drive->drive_ram_expand2, 0x2000);
            drivemem_set_func(cpud, 0x20, 0x40, drive_read_ram2, drive_store_ram2);
        } else {
            drivemem_set_func(cpud, 0x20, 0x28, drive_read_1541ram, drive_store_1541ram);
            drivemem_set_func(cpud, 0x38, 0x3c, via1d1541_read, via1d1541_store);
            drivemem_set_func(cpud, 0x3c, 0x40, via2d_read, via2d_store);
        }
        if (drv->drive->drive_ram4_enabled) {
            realloc_expram(&drv->drive->drive_ram_expand4, 0x2000);
            drivemem_set_func(cpud, 0x40, 0x60, drive_read_ram4, drive_store_ram4);
        } else {
            drivemem_set_func(cpud, 0x40, 0x48, drive_read_1541ram, drive_store_1541ram);
            drivemem_set_func(cpud, 0x58, 0x5c, via1d1541_read, via1d1541_store);
            drivemem_set_func(cpud, 0x5c, 0x60, via2d_read, via2d_store);
        }
        if (drv->drive->drive_ram6_enabled) {
            realloc_expram(&drv->drive->drive_ram_expand6, 0x2000);
            drivemem_set_func(cpud, 0x60, 0x80, drive_read_ram6, drive_store_ram6);
        } else {
            drivemem_set_func(cpud, 0x60, 0x68, drive_read_1541ram, drive_store_1541ram);
            drivemem_set_func(cpud, 0x78, 0x7c, via1d1541_read, via1d1541_store);
            drivemem_set_func(cpud, 0x7c, 0x80, via2d_read, via2d_store);
        }
        if (drv->drive->drive_ram8_enabled) {
            realloc_expram(&drv->drive->drive_ram_expand8, 0x2000);
            drivemem_set_func(cpud, 0x80, 0xa0, drive_read_ram8, drive_store_ram8);
        } else {
            drivemem_set_func(cpud, 0x80, 0xa0, drive_read_rom, NULL);
        }
        if (drv->drive->drive_rama_enabled) {
            realloc_expram(&drv->drive->drive_ram_expanda, 0x2000);
            drivemem_set_func(cpud, 0xa0, 0xc0, drive_read_rama, drive_store_rama);
        } else {
            drivemem_set_func(cpud, 0xa0, 0xc0, drive_read_rom, NULL);
        }
        drivemem_set_func(cpud, 0xc0, 0x100, drive_read_rom, NULL);
        break;
    case DRIVE_TYPE_1570:
    case DRIVE_TYPE_1571:
    case DRIVE_TYPE_1571CR:
        drv->cpu->pageone = cpud->drive_ram + 0x100;
        drivemem_set_func(cpud, 0x00, 0x01, drive_read_zero, drive_store_zero);
        drivemem_set_func(cpud, 0x01, 0x10, drive_read_1541ram, drive_store_1541ram);
        drivemem_set_func(cpud, 0x18, 0x1c, via1d1541_read, via1d1541_store);
        drivemem_set_func(cpud, 0x1c, 0x20, via2d_read, via2d_store);
        drivemem_set_func(cpud, 0x20, 0x30, wd1770d_read, wd1770d_store);
        if (drv->drive->drive_ram4_enabled) {
            drivemem_set_func(cpud, 0x40, 0x48, cia1571_read, cia1571_store);
            realloc_expram(&drv->drive->drive_ram_expand4, 0x2000);
            drivemem_set_func(cpud, 0x48, 0x60, drive_read_ram4, drive_store_ram4);
        } else {
            drivemem_set_func(cpud, 0x40, 0x60, cia1571_read, cia1571_store);
        }
        if (drv->drive->drive_ram6_enabled) {
            realloc_expram(&drv->drive->drive_ram_expand6, 0x2000);
            drivemem_set_func(cpud, 0x60, 0x80, drive_read_ram6, drive_store_ram6);
        } else {
            drivemem_set_func(cpud, 0x60, 0x80, cia1571_read, cia1571_store);
        }
        drivemem_set_func(cpud, 0x80, 0x100, drive_read_rom, NULL);
        break;
    case DRIVE_TYPE_1581:
        drv->cpu->pageone = cpud->drive_ram + 0x100;
        drivemem_set_func(cpud, 0x00, 0x01, drive_read_zero, drive_store_zero);
        drivemem_set_func(cpud, 0x01, 0x20, drive_read_1581ram, drive_store_1581ram);
        drivemem_set_func(cpud, 0x40, 0x60, cia1581_read, cia1581_store);
        drivemem_set_func(cpud, 0x60, 0x80, wd1770d_read, wd1770d_store);
        drivemem_set_func(cpud, 0x80, 0x100, drive_read_rom, NULL);
        break;
    case DRIVE_TYPE_2000:
    case DRIVE_TYPE_4000:
        drv->cpu->pageone = cpud->drive_ram + 0x100;
        drivemem_set_func(cpud, 0x00, 0x01, drive_read_zero, drive_store_zero);
        drivemem_set_func(cpud, 0x01, 0x20, drive_read_1581ram, drive_store_1581ram);
        realloc_expram(&drv->drive->drive_ram_expand2, 0x2000);
        drivemem_set_func(cpud, 0x20, 0x40, drive_read_ram2, drive_store_ram2);
        drivemem_set_func(cpud, 0x40, 0x4c, via4000_read, via4000_store);
        drivemem_set_func(cpud, 0x4e, 0x50, pc8477d_read, pc8477d_store);
        realloc_expram(&drv->drive->drive_ram_expand4, 0x2000);
        drivemem_set_func(cpud, 0x50, 0x60, drive_read_ram4, drive_store_ram4);
        realloc_expram(&drv->drive->drive_ram_expand6, 0x2000);
        drivemem_set_func(cpud, 0x60, 0x80, drive_read_ram6, drive_store_ram6);
        drivemem_set_func(cpud, 0x80, 0x100, drive_read_rom, NULL);
        /* for performance reasons it's only this page */
        drivemem_set_func(cpud, 0xf0, 0xf1, drive_read_rom_ds1216, NULL);
        break;
    default:
        return;
    }
}
