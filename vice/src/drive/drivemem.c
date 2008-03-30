/*
 * drivemem.c - Drive memory handling.
 *
 * Written by
 *   Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include <string.h>

#include "ciad.h"
#include "drive.h"
#include "drivetypes.h"
#include "fdc.h"
#include "log.h"
#include "riotd.h"
#include "types.h"
#include "utils.h"
#include "viad.h"
#include "wd1770.h"

/* ------------------------------------------------------------------------- */
/* SFD1001 specific memory.  */

static BYTE REGPARM2 drive_read_1001_io(drive_context_t *drv, ADDRESS address)
{
    if (address & 0x80) {
        return riot2_read(drv, address);
    }
    return riot1_read(drv, address);
}

static void REGPARM3 drive_store_1001_io(drive_context_t *drv,
                                         ADDRESS address, BYTE byte)
{
    if (address & 0x80) {
        riot2_store(drv, address, byte);
    } else {
        riot1_store(drv, address, byte);
    }
}

static BYTE REGPARM2 drive_read_1001zero_ram(drive_context_t *drv,
                                             ADDRESS address)
{
    return drv->cpud.drive_ram[address & 0xff];
}

static void REGPARM3 drive_store_1001zero_ram(drive_context_t *drv,
                                              ADDRESS address, BYTE byte)
{
    drv->cpud.drive_ram[address & 0xff] = byte;
}

static BYTE REGPARM2 drive_read_1001buffer_ram(drive_context_t *drv,
                                               ADDRESS address)
{
    return drv->cpud.drive_ram[(((address >> 2) & 0x1c00)
                               | (address & 0x03ff)) - 0x300];
}

static void REGPARM3 drive_store_1001buffer_ram(drive_context_t *drv,
                     ADDRESS address, BYTE byte)
{
    drv->cpud.drive_ram[(((address >> 2) & 0x1c00) | (address & 0x03ff))
                        - 0x300] = byte;
}

/* ------------------------------------------------------------------------- */
/* Common memory access.  */

static BYTE REGPARM2 drive_read_ram(drive_context_t *drv, ADDRESS address)
{
    /* FIXME: This breaks the 1541 RAM mirror!  */
    return drv->cpud.drive_ram[address & 0x1fff];
}

static void REGPARM3 drive_store_ram(drive_context_t *drv, ADDRESS address,
                                     BYTE value)
{
    /* FIXME: This breaks the 1541 RAM mirror!  */
    drv->cpud.drive_ram[address & 0x1fff] = value;
}

static BYTE REGPARM2 drive_read_rom(drive_context_t *drv, ADDRESS address)
{
    return drv->drive_ptr->rom[address & 0x7fff];
}

static BYTE REGPARM2 drive_read_free(drive_context_t *drv, ADDRESS address)
{
    return address >> 8;
}

static void REGPARM3 drive_store_free(drive_context_t *drv, ADDRESS address,
                                      BYTE value)
{
    return;
}

/* ------------------------------------------------------------------------- */
/* Drive memory expansion.  */

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
/* Watchpoint memory access.  */

static BYTE REGPARM2 drive_read_watch(drive_context_t *drv, ADDRESS address)
{
    mon_watch_push_load_addr(address, drv->cpu.monspace);
    return drv->cpud.read_func_nowatch[address>>8](drv,address);
}

static void REGPARM3 drive_store_watch(drive_context_t *drv, ADDRESS address, BYTE value)
{
    mon_watch_push_store_addr(address, drv->cpu.monspace);
    drv->cpud.store_func_nowatch[address>>8](drv,address, value);
}

/* ------------------------------------------------------------------------- */

#ifdef _MSC_VER
#pragma optimize("",off);
#endif

void drive_mem_init(drive_context_t *drv, int type)
{
    int i;

    for (i = 0; i < 0x101; i++) {
        drv->cpud.read_func_watch[i] = drive_read_watch;
        drv->cpud.store_func_watch[i] = drive_store_watch;
        drv->cpud.read_func_nowatch[i] = drive_read_free;
        drv->cpud.store_func_nowatch[i] = drive_store_free;
    }

    /* FIXME: ROM mirrors! */
    /* Setup firmware ROM.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_2031 || type == DRIVE_TYPE_1001
        || type == DRIVE_TYPE_8050 || type == DRIVE_TYPE_8250)
        for (i = 0xC0; i < 0x100; i++)
            drv->cpud.read_func_nowatch[i] = drive_read_rom;

    if (type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_1581)
        for (i = 0x80; i < 0x100; i++)
            drv->cpud.read_func_nowatch[i] = drive_read_rom;

    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_1581
        || type == DRIVE_TYPE_2031) {

        drv->cpu.pageone = drv->cpud.drive_ram + 0x100;

        /* Setup drive RAM.  */
        for (i = 0x00; i < 0x08; i++) {
            drv->cpud.read_func_nowatch[i] = drive_read_ram;
            drv->cpud.store_func_nowatch[i] = drive_store_ram;
        }
        if (type == DRIVE_TYPE_1581)
            for (i = 0x08; i < 0x20; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram;
                drv->cpud.store_func_nowatch[i] = drive_store_ram;
            }
    }
    if (type == DRIVE_TYPE_1001 || type == DRIVE_TYPE_8050
        || type == DRIVE_TYPE_8050) {
        /* The 1001/8050/8250 have 256 byte at $00xx, mirrored at
           $01xx, $04xx, $05xx, $08xx, $09xx, $0cxx, $0dxx.
           (From the 2 RIOT's 128 byte RAM each. The RIOT's I/O fill
           the gaps, x00-7f the first and x80-ff the second, at
           $02xx, $03xx, $06xx, $07xx, $0axx, $0bxx, $0exx, $0fxx).
           Then we have 4k of buffers, at $1000-13ff, 2000-23ff, 3000-33ff
           and 4000-43ff, each mirrored at $x400-$x7fff, $x800-$xbff,
           and $xc00-$xfff.

           Here we set zeropage, stack and buffer RAM as well as I/O */

        drv->cpu.pageone = drv->cpud.drive_ram;

        for (i = 0; i <= 0x10; i += 4) {
           drv->cpud.read_func_nowatch[i] = drive_read_1001zero_ram;
           drv->cpud.store_func_nowatch[i] = drive_store_1001zero_ram;
           drv->cpud.read_func_nowatch[i + 1] = drive_read_1001zero_ram;
           drv->cpud.store_func_nowatch[i + 1] = drive_store_1001zero_ram;
           drv->cpud.read_func_nowatch[i + 2] = drive_read_1001_io;
           drv->cpud.store_func_nowatch[i + 2] = drive_store_1001_io;
           drv->cpud.read_func_nowatch[i + 3] = drive_read_1001_io;
           drv->cpud.store_func_nowatch[i + 3] = drive_store_1001_io;
        }
        for (i = 0x10; i <= 0x50; i ++) {
           drv->cpud.read_func_nowatch[i] = drive_read_1001buffer_ram;
           drv->cpud.store_func_nowatch[i] = drive_store_1001buffer_ram;
        }
    }

    /* Setup 1541, 1541-II and 1571 VIAs.  */
    if (type == DRIVE_TYPE_1541 || type == DRIVE_TYPE_1541II
        || type == DRIVE_TYPE_1571 || type == DRIVE_TYPE_2031) {
        for (i = 0x18; i < 0x1C; i++) {
            drv->cpud.read_func_nowatch[i] = via1d_read;
            drv->cpud.store_func_nowatch[i] = via1d_store;
        }
        for (i = 0x1C; i < 0x20; i++) {
            drv->cpud.read_func_nowatch[i] = via2d_read;
            drv->cpud.store_func_nowatch[i] = via2d_store;
        }
    }
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
            drv->drive_ptr->drive_ram_expand2 = xmalloc(0x2000);
            memset(drv->drive_ptr->drive_ram_expand2, 0, 0x2000);
            for (i = 0x20; i < 0x40; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram2;
                drv->cpud.store_func_nowatch[i] = drive_store_ram2;
            }
        }
        if (drv->drive_ptr->drive_ram4_enabled) {
            if (drv->drive_ptr->drive_ram_expand4 != NULL)
                free(drv->drive_ptr->drive_ram_expand4);
            drv->drive_ptr->drive_ram_expand4 = xmalloc(0x2000);
            memset(drv->drive_ptr->drive_ram_expand4, 0, 0x2000);
            for (i = 0x40; i < 0x60; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram4;
                drv->cpud.store_func_nowatch[i] = drive_store_ram4;
            }
        }
        if (drv->drive_ptr->drive_ram6_enabled) {
            if (drv->drive_ptr->drive_ram_expand6 != NULL)
                free(drv->drive_ptr->drive_ram_expand6);
            drv->drive_ptr->drive_ram_expand6 = xmalloc(0x2000);
            memset(drv->drive_ptr->drive_ram_expand6, 0, 0x2000);
            for (i = 0x60; i < 0x80; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram6;
                drv->cpud.store_func_nowatch[i] = drive_store_ram6;
            }
        }
        if (drv->drive_ptr->drive_ram8_enabled) {
            if (drv->drive_ptr->drive_ram_expand8 != NULL)
                free(drv->drive_ptr->drive_ram_expand8);
            drv->drive_ptr->drive_ram_expand8 = xmalloc(0x2000);
            memset(drv->drive_ptr->drive_ram_expand8, 0, 0x2000);
            for (i = 0x80; i < 0xa0; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_ram8;
                drv->cpud.store_func_nowatch[i] = drive_store_ram8;
            }
        }
        if (drv->drive_ptr->drive_rama_enabled) {
            if (drv->drive_ptr->drive_ram_expanda != NULL)
                free(drv->drive_ptr->drive_ram_expanda);
            drv->drive_ptr->drive_ram_expanda = xmalloc(0x2000);
            memset(drv->drive_ptr->drive_ram_expanda, 0, 0x2000);
            for (i = 0xa0; i < 0xc0; i++) {
                drv->cpud.read_func_nowatch[i] = drive_read_rama;
                drv->cpud.store_func_nowatch[i] = drive_store_rama;
            }
        }
    }

    drv->cpud.read_func_nowatch[0x100] = drv->cpud.read_func_nowatch[0];
    drv->cpud.store_func_nowatch[0x100] = drv->cpud.store_func_nowatch[0];

    memcpy(drv->cpud.read_func, drv->cpud.read_func_nowatch,
           sizeof(drive_read_func_t *) * 0x101);
    memcpy(drv->cpud.store_func, drv->cpud.store_func_nowatch,
           sizeof(drive_store_func_t *) * 0x101);

    switch (type) {
      case DRIVE_TYPE_1541:
      case DRIVE_TYPE_1541II:
      case DRIVE_TYPE_2031:
      case DRIVE_TYPE_1001:
      case DRIVE_TYPE_8050:
      case DRIVE_TYPE_8250:
        drv->drive_ptr->rom_start = 0xc000;
        break;
      case DRIVE_TYPE_1571:
      case DRIVE_TYPE_1581:
        drv->drive_ptr->rom_start = 0x8000;
        break;
      default:
        log_error(LOG_ERR, "DRIVEMEM: Unknown drive type.");
    }
}

#ifdef _MSC_VER
#pragma optimize("",on);
#endif

