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
#include "viad.h"
#include "wd1770.h"


static BYTE REGPARM2 drive_read_ram(drive_context_t *drv, WORD address)
{
    return drv->cpud.drive_ram[address & 0x7ff];
}

static void REGPARM3 drive_store_ram(drive_context_t *drv, WORD address,
                                     BYTE value)
{
    drv->cpud.drive_ram[address & 0x7ff] = value;
}

static BYTE REGPARM2 drive_read_1581ram(drive_context_t *drv, WORD address)
{
    return drv->cpud.drive_ram[address & 0x1fff];
}

static void REGPARM3 drive_store_1581ram(drive_context_t *drv, WORD address,
                                         BYTE value)
{
    drv->cpud.drive_ram[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_zero(drive_context_t *drv, WORD address)
{
    return drv->cpud.drive_ram[address & 0xff];
}

static void REGPARM3 drive_store_zero(drive_context_t *drv, WORD address,
                                      BYTE value)
{
    drv->cpud.drive_ram[address & 0xff] = value;
}

static BYTE REGPARM2 drive_read_ram2(drive_context_t *drv, WORD address)
{
    return drv->drive_ptr->drive_ram_expand2[address & 0x1fff];
}

static void REGPARM3 drive_store_ram2(drive_context_t *drv, WORD address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand2[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_ram4(drive_context_t *drv, WORD address)
{
    return drv->drive_ptr->drive_ram_expand4[address & 0x1fff];
}

static void REGPARM3 drive_store_ram4(drive_context_t *drv, WORD address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand4[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_ram6(drive_context_t *drv, WORD address)
{
    return drv->drive_ptr->drive_ram_expand6[address & 0x1fff];
}

static void REGPARM3 drive_store_ram6(drive_context_t *drv, WORD address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand6[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_ram8(drive_context_t *drv, WORD address)
{
    return drv->drive_ptr->drive_ram_expand8[address & 0x1fff];
}

static void REGPARM3 drive_store_ram8(drive_context_t *drv, WORD address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand8[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_rama(drive_context_t *drv, WORD address)
{
    return drv->drive_ptr->drive_ram_expanda[address & 0x1fff];
}

static void REGPARM3 drive_store_rama(drive_context_t *drv, WORD address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expanda[address & 0x1fff] = value;
}

/* ------------------------------------------------------------------------- */

void memiec_init(struct drive_context_s *drv, unsigned int type)
{
    unsigned int i, j;

    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_1581) {

        /* Setup drive RAM.  */
        switch (type) {
          case DRIVE_TYPE_1541:
          case DRIVE_TYPE_1541II:
            for (j = 0; j < 0x80; j += 0x20) {
                for (i = 0x00 + j; i < 0x08 + j; i++) {
                    drv->cpud.read_func_nowatch[i] = drive_read_ram;
                    drv->cpud.store_func_nowatch[i] = drive_store_ram;
                }
            }
            break;
          case DRIVE_TYPE_1571:
            for (i = 0x00; i < 0x10; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram;
                drv->cpud.store_func_nowatch[i] = drive_store_ram;
            }
            break;
          case DRIVE_TYPE_1581:
            for (i = 0x00; i < 0x20; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_1581ram;
                drv->cpud.store_func_nowatch[i] = drive_store_1581ram;
            }
            break;
        }

        drv->cpu.pageone = drv->cpud.drive_ram + 0x100;

        drv->cpud.read_func_nowatch[0] = drive_read_zero;
        drv->cpud.store_func_nowatch[0] = drive_store_zero;

        /* Setup drive ROM.  */
        for (i = 0x80; i < 0x100; i++)
            drv->cpud.read_func_nowatch[i] = drive_read_rom;
    }

    /* Setup 1541, 1541-II VIAs.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II) {
        for (j = 0; j < 0x80; j += 0x20) {
            for (i = 0x18 + j; i < 0x1c + j; i++) {
                drv->cpud.read_func_nowatch[i] = via1d_read;
                drv->cpud.store_func_nowatch[i] = via1d_store;
            }
            for (i = 0x1c + j; i < 0x20 + j; i++) {
                drv->cpud.read_func_nowatch[i] = via2d_read;
                drv->cpud.store_func_nowatch[i] = via2d_store;
            }
        }
    }

    /* Setup 1571 VIA1, VIA2, WD1770 and CIA.  */
    if (type == DRIVE_TYPE_1571) {
        for (i = 0x18; i < 0x1c; i++) {
            drv->cpud.read_func_nowatch[i] = via1d_read;
            drv->cpud.store_func_nowatch[i] = via1d_store;
        }
        for (i = 0x1c; i < 0x20; i++) {
            drv->cpud.read_func_nowatch[i] = via2d_read;
            drv->cpud.store_func_nowatch[i] = via2d_store;
        }
        for (i = 0x20; i < 0x30; i++) {
            drv->cpud.read_func_nowatch[i] = wd1770d_read;
            drv->cpud.store_func_nowatch[i] = wd1770d_store;
        }
        for (i = 0x40; i < 0x80; i++) {
            drv->cpud.read_func_nowatch[i] = cia1571_read;
            drv->cpud.store_func_nowatch[i] = cia1571_store;
        }
    }

    /* Setup 1581 CIA.  */
    if (type == DRIVE_TYPE_1581) {
        for (i = 0x40; i < 0x60; i++) {
            drv->cpud.read_func_nowatch[i] = cia1581_read;
            drv->cpud.store_func_nowatch[i] = cia1581_store;
        }
        for (i = 0x60; i < 0x80; i++) {
            drv->cpud.read_func_nowatch[i] = wd1770d_read;
            drv->cpud.store_func_nowatch[i] = wd1770d_store;
        }
    }

    if (rom_loaded && (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II)) {
        if (drv->drive_ptr->drive_ram2_enabled) {
            if (drv->drive_ptr->drive_ram_expand2 != NULL)
                lib_free(drv->drive_ptr->drive_ram_expand2);
            drv->drive_ptr->drive_ram_expand2 = lib_calloc(1, 0x2000);
            for (i = 0x20; i < 0x40; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram2;
                drv->cpud.store_func_nowatch[i] = drive_store_ram2;
            }
        }
        if (drv->drive_ptr->drive_ram4_enabled) {
            if (drv->drive_ptr->drive_ram_expand4 != NULL)
                lib_free(drv->drive_ptr->drive_ram_expand4);
            drv->drive_ptr->drive_ram_expand4 = lib_calloc(1, 0x2000);
            for (i = 0x40; i < 0x60; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram4;
                drv->cpud.store_func_nowatch[i] = drive_store_ram4;
            }
        }
        if (drv->drive_ptr->drive_ram6_enabled) {
            if (drv->drive_ptr->drive_ram_expand6 != NULL)
                lib_free(drv->drive_ptr->drive_ram_expand6);
            drv->drive_ptr->drive_ram_expand6 = lib_calloc(1, 0x2000);
            for (i = 0x60; i < 0x80; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram6;
                drv->cpud.store_func_nowatch[i] = drive_store_ram6;
            }
        }
        if (drv->drive_ptr->drive_ram8_enabled) {
            if (drv->drive_ptr->drive_ram_expand8 != NULL)
                lib_free(drv->drive_ptr->drive_ram_expand8);
            drv->drive_ptr->drive_ram_expand8 = lib_calloc(1, 0x2000);
            for (i = 0x80; i < 0xa0; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram8;
                drv->cpud.store_func_nowatch[i] = drive_store_ram8;
            }
        }
        if (drv->drive_ptr->drive_rama_enabled) {
            if (drv->drive_ptr->drive_ram_expanda != NULL)
                lib_free(drv->drive_ptr->drive_ram_expanda);
            drv->drive_ptr->drive_ram_expanda = lib_calloc(1, 0x2000);
            for (i = 0xa0; i < 0xc0; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_rama;
                drv->cpud.store_func_nowatch[i] = drive_store_rama;
            }
        }
    }
}

