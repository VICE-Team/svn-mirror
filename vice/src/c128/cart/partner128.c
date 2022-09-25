/*
 * partner128.c -- "Partner 128" cartridge emulation
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include "cartridge.h"
#include "cartio.h"
#include "util.h"

#include "c64cart.h"
#include "export.h"
#include "c128cart.h"
#include "functionrom.h"

#include "crt.h"
#include "partner128.h"

#define DBGWS

#ifdef DBGWS
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/*

Partner 128

$8000-$9fff contains the ROM
$de00-$de7f contains 128 bytes ram
$de80-$deff contains a mirror of $9e80-$9eff

Writing to $de80 selects which one of 64 blocks of 128 byte ram is visible in $de00-$de7f.

- it has a button which probably generates an NMI
- it has a cable which has to be connected to joystick port 2, and from a
rough looking at the code it seems to simply be used to tell the nmi handler
the button is the source of the nmi.

*/

#define PARTNER_ROM_SIZE  0x4000
#define PARTNER_RAM_SIZE  (64 * 128)    /* 64 * 128bytes */

static uint8_t rambank = 0;
static uint8_t rambanks[PARTNER_RAM_SIZE];

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static uint8_t partner128_io1_peek(uint16_t addr);
static uint8_t partner128_io1_read(uint16_t addr);
static void partner128_io1_store(uint16_t addr, uint8_t value);

/*static uint8_t partner128_io2_read(uint16_t addr);
static uint8_t partner128_io2_peek(uint16_t addr);
static void partner128_io2_store(uint16_t addr, uint8_t value);*/

static int partner128_dump(void);

static io_source_t partner128_io1_device = {
    CARTRIDGE_C128_NAME_PARTNER128, /* name of the device */
    IO_DETACH_CART,                 /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,          /* does not use a resource for detach */
    0xde00, 0xdeff, 0xff,           /* range for the device, address is ignored by the write functions, reg:$de00, mirrors:$de01-$deff */
    1,                              /* read is never valid */
    partner128_io1_store,           /* store function */
    NULL,                           /* NO poke function */
    partner128_io1_read,            /* read function */
    partner128_io1_peek,            /* peek function */
    partner128_dump,                /* device state information dump function */
    CARTRIDGE_C128_PARTNER128,      /* cartridge ID */
    IO_PRIO_NORMAL,                 /* normal priority, device read needs to be checked for collisions */
    0                               /* insertion order, gets filled in by the registration function */
};
static io_source_list_t *partner128_io1_list_item = NULL;

#if 0
static io_source_t partner128_io2_device = {
    CARTRIDGE_C128_NAME_PARTNER128, /* name of the device */
    IO_DETACH_CART,                 /* use cartridge ID to detach the device when involved in a read-collision */
    IO_DETACH_NO_RESOURCE,          /* does not use a resource for detach */
    0xdf00, 0xdfff, 0xff,           /* range for the device, address is ignored by the read/write functions, reg:$df00, mirrors:$df01-$dfff */
    1,                              /* validity of the read is determined by the cartridge at read time */
    partner128_io2_store,           /* store function */
    NULL,                           /* NO poke function */
    partner128_io2_read,            /* read function */
    partner128_io2_peek,            /* peek function */
    partner128_dump,                /* device state information dump function */
    CARTRIDGE_C128_PARTNER128,      /* cartridge ID */
    IO_PRIO_NORMAL,                 /* normal priority, device read needs to be checked for collisions */
    0                               /* insertion order, gets filled in by the registration function */
};
static io_source_list_t *partner128_io2_list_item = NULL;
#endif

static const export_resource_t export_res = {
    CARTRIDGE_C128_NAME_PARTNER128, 1, 1, &partner128_io1_device, NULL, CARTRIDGE_C128_PARTNER128
};

/* ---------------------------------------------------------------------*/

static void partner128_io1_store(uint16_t addr, uint8_t value)
{
/* DBG(("partner128_io1_store %04x %02x\n", addr, value)); */
    if (addr < 0x80) {
        /* RAM */
        rambanks[(rambank * 128) + addr] = value;
    } else if (addr >= 0x80) {
        rambank = value & 0x3f;
        DBG(("partner128 bank:%02x\n", rambank));
    }
}

static uint8_t partner128_io1_read(uint16_t addr)
{
    uint8_t value;

    if (addr < 0x80) {
        /* RAM */
        value = rambanks[(rambank * 128) + addr];
    } else {
        value = ext_function_rom[0x1e80 + (addr & 0x7f)];
        /* value = rambanks[(rambank * 128) + addr]; */
        /* value = rambank; */
    }
    /* DBG(("partner128_io1_read %04x %02x\n", addr, value)); */
    return value;
}

static uint8_t partner128_io1_peek(uint16_t addr)
{
    uint8_t value;

    if (addr < 0x80) {
        /* RAM */
        value = rambanks[(rambank * 128) + addr];
    } else {
        value = rambank;
    }
    /*DBG(("partner128_io1_read %04x %02x\n", addr, value));*/
    return value;
}

#if 0
static uint8_t partner128_io2_read(uint16_t addr)
{
    uint8_t value;

    value = ext_function_rom[0x1f00 + (addr & 0xff)];
    DBG(("partner128_io2_read %04x %02x\n", addr, value));
    return value;
}

static uint8_t partner128_io2_peek(uint16_t addr)
{
    return ext_function_rom[0x1f00 + (addr & 0xff)];
}

static void partner128_io2_store(uint16_t addr, uint8_t value)
{
    DBG(("partner128_io2_store %04x %02x\n", addr, value));
}
#endif

static int partner128_dump(void)
{
    return 0;
}

/* ---------------------------------------------------------------------*/

void partner128_config_setup(uint8_t *rawcart)
{
    /* copy loaded cartridge data into actually used ROM array */
    memcpy(&ext_function_rom[0], rawcart, PARTNER_ROM_SIZE);
}

static int partner128_common_attach(void)
{
    /* setup i/o device */
    if (export_add(&export_res) < 0) {
        return -1;
    }
    partner128_io1_list_item = io_source_register(&partner128_io1_device);
    /* partner128_io2_list_item = io_source_register(&partner128_io2_device); */

    return 0;
}

int partner128_bin_attach(const char *filename, uint8_t *rawcart)
{
    if (util_file_load(filename, rawcart, PARTNER_ROM_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    memcpy(rawcart + PARTNER_ROM_SIZE, rawcart, PARTNER_ROM_SIZE);
    return partner128_common_attach();
}

/*
    returns -1 on error, else a positive CRT ID
*/
int partner128_crt_attach(FILE *fd, uint8_t *rawcart)
{
    crt_chip_header_t chip;

    if (crt_read_chip_header(&chip, fd)) {
        return -1;
    }

    DBG(("chip1 at %02x len %02x\n", chip.start, chip.size));
    if (chip.start == 0x8000 && chip.size == 0x4000) {
        if (crt_read_chip(rawcart, 0, &chip, fd)) {
            return -1;
        }
        memcpy(rawcart + PARTNER_ROM_SIZE, rawcart, PARTNER_ROM_SIZE);
        return partner128_common_attach();
    }
    return -1;
}

void partner128_detach(void)
{
    if (partner128_io1_list_item) {
        io_source_unregister(partner128_io1_list_item);
        partner128_io1_list_item = NULL;
    }
    /*io_source_unregister(partner128_io2_list_item);*/
    /*partner128_io2_list_item = NULL;*/
    export_remove(&export_res);
}

void partner128_reset(void)
{
    DBG(("partner128_reset\n"));
    rambank = 0;
}
