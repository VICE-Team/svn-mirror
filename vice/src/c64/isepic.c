/*
 * isepic.c - ISEPIC emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

/*
 * Isepic is a ram based freeze cart.
 *
 * It has 2KB of ram which is banked into a 256 byte page in the $DF00-$DFFF area.
 *
 * The page is selected by any kind of access to the I/O-2 area, as follows:
 *
 * PAGE   ACCESS ADDRESS
 * ----   --------------
 *  0         $DE00
 *  1         $DE04
 *  2         $DE02
 *  3         $DE06
 *  4         $DE01
 *  5         $DE05
 *  6         $DE03
 *  7         $DE07
 *
 * Because of the incomplete decoding this 8 byte area is mirrored throughout $DE08-$DEFF.
 *
 * The isepic cart has a switch which controls if the registers and ram is mapped in.
 *
 * When the switch is switched away from the computer the cart is put in 'hidden' mode,
 * where the registers, window and ram is not accessable.
 *
 * When the switch is switched towards the computer the cart is put in ultimax mode,
 * with the registers mapped, and the current page being mapped into any unmapped ultimax
 * memory space, it will also generate an NMI. Which activates the freezer.
 *
 */

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cart/c64cartmem.h"
#include "c64cart.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "cmdline.h"
#include "isepic.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "resources.h"
#include "snapshot.h"
#include "translate.h"
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"

/* ------------------------------------------------------------------------- */

/* Flag: Do we enable the external ISEPEC?  */
int isepic_enabled;

/* Flag: what direction is the switch at, 0 = away, 1 = towards computer */
int isepic_switch = 0;

/* 2 KB RAM */
static BYTE *isepic_ram;

/* current page */
static unsigned int isepic_page = 0;

static int set_isepic_enabled(int val, void *param)
{
    if (isepic_enabled && !val)
    {
        lib_free(isepic_ram);
        isepic_enabled = 0;
        if (isepic_switch)
        {
            cartridge_config_changed(2, 2, 0);
            cartridge_release_freeze();
        }
    }

    if (!isepic_enabled && val)
    {
        isepic_ram = (BYTE *)lib_malloc(2048);
        isepic_enabled = 1;
        if (isepic_switch)
        {
            cartridge_config_changed(2, 3, 0);
        }
    }
    return 0;
}

static int set_isepic_switch(int val, void *param)
{
    if (isepic_switch && !val)
    {
        isepic_switch = 0;
        if (isepic_enabled)
        {
            cartridge_config_changed(2, 2, 0);
            cartridge_release_freeze();
        }
    }

    if (!isepic_switch && val)
    {
        isepic_switch = 1;
        if (isepic_enabled)
        {
            cartridge_trigger_freeze();
        }
    }
    return 0;
}

void isepic_freeze(void)
{
    cartridge_config_changed(2, 3, 0);
}

static const resource_int_t resources_int[] = {
    { "Isepic", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &isepic_enabled, set_isepic_enabled, NULL },
    { "IsepicSwitch", 0, RES_EVENT_STRICT, (resource_value_t)1,
      &isepic_switch, set_isepic_switch, NULL },
    { NULL }
};

int isepic_resources_init(void)
{
    return resources_register_int(resources_int);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
    { "-isepic", SET_RESOURCE, 0,
      NULL, NULL, "Isepic", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_ISEPIC,
      NULL, NULL },
    { "+isepic", SET_RESOURCE, 0,
      NULL, NULL, "Isepic", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_ISEPIC,
      NULL, NULL },
    { NULL }
};

int isepic_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 isepic_reg_read(WORD addr)
{
    if (isepic_switch)
    {
        isepic_page = ((addr & 4) >> 2) | (addr & 2) | ((addr & 1) << 2);
    }
    return 0;
}

void REGPARM2 isepic_reg_store(WORD addr, BYTE byte)
{
    if (isepic_switch)
    {
        isepic_page = ((addr & 4) >> 2) | (addr & 2) | ((addr & 1) << 2);
    }
}

BYTE REGPARM1 isepic_romh_read(WORD addr)
{
    switch (addr)
    {
        case 0xfffa:
        case 0xfffb:
            return isepic_ram[(isepic_page * 256) + (addr & 0xff)];
            break;
        default:
            return mem_read_without_ultimax(addr);
            break;
    }
}

void REGPARM2 isepic_romh_store(WORD addr, BYTE byte)
{
    switch (addr)
    {
        case 0xfffa:
        case 0xfffb:
            isepic_ram[(isepic_page * 256) + (addr & 0xff)] = byte;
            break;
        default:
            mem_store_without_ultimax(addr, byte);
            break;
    }
}

BYTE REGPARM1 isepic_window_read(WORD addr)
{
    BYTE retval = 0;

    if (isepic_switch)
    {
        io_source=IO_SOURCE_ISEPIC;
        retval=isepic_ram[(isepic_page * 256) + (addr & 0xff)];
    }

    return retval;
}

void REGPARM2 isepic_window_store(WORD addr, BYTE byte)
{
    if (isepic_switch)
    {
        isepic_ram[(isepic_page * 256) + (addr & 0xff)] = byte;
    }
}

BYTE REGPARM1 isepic_roml_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 isepic_roml_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 isepic_1000_7fff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 isepic_1000_7fff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 isepic_a000_bfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 isepic_a000_bfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}

BYTE REGPARM1 isepic_c000_cfff_read(WORD addr)
{
    return mem_read_without_ultimax(addr);
}

void REGPARM2 isepic_c000_cfff_store(WORD addr, BYTE value)
{
    mem_store_without_ultimax(addr, value);
}
