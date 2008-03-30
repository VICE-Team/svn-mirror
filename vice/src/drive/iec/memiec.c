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
#include "memiec.h"
#include "types.h"
#include "utils.h"
#include "wd1770.h"


static BYTE REGPARM2 drive_read_ram2(drive_context_t *drv, ADDRESS address)
{
    return drv->drive_ptr->drive_ram_expand2[address & 0x1fff];
}

static void REGPARM3 drive_store_ram2(drive_context_t *drv, ADDRESS address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand2[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_ram4(drive_context_t *drv, ADDRESS address)
{
    return drv->drive_ptr->drive_ram_expand4[address & 0x1fff];
}

static void REGPARM3 drive_store_ram4(drive_context_t *drv, ADDRESS address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand4[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_ram6(drive_context_t *drv, ADDRESS address)
{
    return drv->drive_ptr->drive_ram_expand6[address & 0x1fff];
}

static void REGPARM3 drive_store_ram6(drive_context_t *drv, ADDRESS address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand6[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_ram8(drive_context_t *drv, ADDRESS address)
{
    return drv->drive_ptr->drive_ram_expand8[address & 0x1fff];
}

static void REGPARM3 drive_store_ram8(drive_context_t *drv, ADDRESS address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expand8[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_rama(drive_context_t *drv, ADDRESS address)
{
    return drv->drive_ptr->drive_ram_expanda[address & 0x1fff];
}

static void REGPARM3 drive_store_rama(drive_context_t *drv, ADDRESS address,
                                      BYTE value)
{
    drv->drive_ptr->drive_ram_expanda[address & 0x1fff] = value;
}

/* ------------------------------------------------------------------------- */

void memiec_init(struct drive_context_s *drv, unsigned int type)
{
    unsigned int i;

    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_1581)
        for (i = 0x80; i < 0x100; i++)
            drv->cpud.read_func_nowatch[i] = drive_read_rom;

    /* Setup 1571 CIA.  */
    if (type == DRIVE_TYPE_1571) {
        for (i = 0x40; i < 0x44; i++) {
            drv->cpud.read_func_nowatch[i] = cia1571_read;
            drv->cpud.store_func_nowatch[i] = cia1571_store;
        }
        for (i = 0x20; i < 0x24; i++) {
            drv->cpud.read_func_nowatch[i] = wd1770d_read;
            drv->cpud.store_func_nowatch[i] = wd1770d_store;
        }
    }

    /* Setup 1581 CIA.  */
    if (type == DRIVE_TYPE_1581) {
        for (i = 0x40; i < 0x44; i++) {
            drv->cpud.read_func_nowatch[i] = cia1581_read;
            drv->cpud.store_func_nowatch[i] = cia1581_store;
        }
        for (i = 0x60; i < 0x64; i++) {
            drv->cpud.read_func_nowatch[i] = wd1770d_read;
            drv->cpud.store_func_nowatch[i] = wd1770d_store;
        }
    }

    if (rom_loaded && (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II)) {
        if (drv->drive_ptr->drive_ram2_enabled) {
            if (drv->drive_ptr->drive_ram_expand2 != NULL)
                free(drv->drive_ptr->drive_ram_expand2);
            drv->drive_ptr->drive_ram_expand2 = xcalloc(1, 0x2000);
            for (i = 0x20; i < 0x40; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram2;
                drv->cpud.store_func_nowatch[i] = drive_store_ram2;
            }
        }
        if (drv->drive_ptr->drive_ram4_enabled) {
            if (drv->drive_ptr->drive_ram_expand4 != NULL)
                free(drv->drive_ptr->drive_ram_expand4);
            drv->drive_ptr->drive_ram_expand4 = xcalloc(1, 0x2000);
            for (i = 0x40; i < 0x60; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram4;
                drv->cpud.store_func_nowatch[i] = drive_store_ram4;
            }
        }
        if (drv->drive_ptr->drive_ram6_enabled) {
            if (drv->drive_ptr->drive_ram_expand6 != NULL)
                free(drv->drive_ptr->drive_ram_expand6);
            drv->drive_ptr->drive_ram_expand6 = xcalloc(1, 0x2000);
            for (i = 0x60; i < 0x80; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram6;
                drv->cpud.store_func_nowatch[i] = drive_store_ram6;
            }
        }
        if (drv->drive_ptr->drive_ram8_enabled) {
            if (drv->drive_ptr->drive_ram_expand8 != NULL)
                free(drv->drive_ptr->drive_ram_expand8);
            drv->drive_ptr->drive_ram_expand8 = xcalloc(1, 0x2000);
            for (i = 0x80; i < 0xa0; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram8;
                drv->cpud.store_func_nowatch[i] = drive_store_ram8;
            }
        }
        if (drv->drive_ptr->drive_rama_enabled) {
            if (drv->drive_ptr->drive_ram_expanda != NULL)
                free(drv->drive_ptr->drive_ram_expanda);
            drv->drive_ptr->drive_ram_expanda = xcalloc(1, 0x2000);
            for (i = 0xa0; i < 0xc0; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_rama;
                drv->cpud.store_func_nowatch[i] = drive_store_rama;
            }
        }
    }
}

