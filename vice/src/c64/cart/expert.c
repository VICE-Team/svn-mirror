/*
 * expert.c - Cartridge handling, Expert cart.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Nathan Huizinga <nathan.huizinga@chess.nl>
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "cartridge.h"
#include "expert.h"
#include "interrupt.h"
#include "resources.h"
#include "reu.h"
#include "types.h"
#include "utils.h"

/* Cartridge mode.  */
extern int cartmode;

/* Expert cart configuration flags.  */
static BYTE ramconfig = 0xff;

/* Remember whether or not the cartridge should be activated or not. */
static int enable_trigger = 0;


BYTE REGPARM1 expert_io1_read(ADDRESS addr)
{
    switch (cartmode) {
      case CARTRIDGE_MODE_PRG:
      case CARTRIDGE_MODE_OFF:
        return 0;
      case CARTRIDGE_MODE_ON:
        /*
         * Reset the nmi/reset trigger.
         */
        enable_trigger = 0;
        return 0;
    }
    return 0;
}

void REGPARM2 expert_io1_store(ADDRESS addr, BYTE value)
{
    switch (cartmode) {
      case CARTRIDGE_MODE_PRG:
      case CARTRIDGE_MODE_OFF:
        break;
      case CARTRIDGE_MODE_ON:
        /*
         * Reset the nmi/reset trigger (= cartridge disabled).
         */
        enable_trigger = 0;
        break;
    }
}

BYTE REGPARM1 expert_io2_read(ADDRESS addr)
{
    if (reu_enabled)
        return reu_read((ADDRESS)(addr & 0x0f));
    return rand();
}

void REGPARM2 expert_io2_store(ADDRESS addr, BYTE value)
{
    if (reu_enabled)
        reu_store((ADDRESS)(addr & 0x0f), value);
}

BYTE REGPARM1 expert_roml_read(ADDRESS addr)
{
    if (export_ram)
        return export_ram0[addr & 0x1fff];

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 expert_roml_store(ADDRESS addr, BYTE value)
{
    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
}

BYTE REGPARM1 expert_romh_read(ADDRESS addr)
{
    return export_ram0[addr & 0x1fff];
}

void REGPARM1 expert_decode_address(ADDRESS addr)
{
    /* Default: disable ~GAME, export_ram and enable ~EXROM */
    BYTE config = (1 << 1);

    switch (cartmode) {
      case CARTRIDGE_MODE_ON:
        /*
         * Mask A15-A13.
         */
        addr = addr & 0xe000;

        if (enable_trigger &&
            ((addr == 0x8000) || (addr = 0xe000))) {
            config = (1 << 0);              /* Enable ~GAME */
            config |= (1 << 1);             /* Disable ~EXROM */
            config |= (1 << 5);             /* Enable export_ram */
        }
        break;
      case CARTRIDGE_MODE_PRG:
        /*
         * Mask A15-A13.
         */
        if ((addr & 0xe000) == 0x8000) {
            config = (1 << 0);              /* Enable ~GAME */
            config |= (1 << 1);             /* Disable ~EXROM */
            config |= (1 << 5);             /* Enable export_ram */
        }
        break;
    }

    if (ramconfig != config) {
        cartridge_config_changed(config);
        ramconfig = config;
    }
}

void expert_ack_nmi_reset(void)
{
    switch (cartmode) {
      case CARTRIDGE_MODE_PRG:
      case CARTRIDGE_MODE_OFF:
        break;
      case CARTRIDGE_MODE_ON:
        enable_trigger = 1;
        break;
    }
}

void expert_freeze(void)
{
}

void expert_config_init(void)
{
    /*
     * Initialize nmi/reset trap functions.
     */
    interrupt_set_nmi_trap_func(&maincpu_int_status,
                                cartridge_ack_nmi_reset);
    interrupt_set_reset_trap_func(&maincpu_int_status,
                                  cartridge_ack_nmi_reset);

    /*
     * Set the nmi/reset trigger (= cartridge enabled).
     */
    enable_trigger = 1;
    ramconfig = (1 << 1);       /* Disable ~EXROM */
    cartridge_config_changed(ramconfig);
}

void expert_config_setup(BYTE *rawcart)
{
    memcpy(export_ram0, rawcart, 0x2000);
    ramconfig = (1 << 1);       /* Disable ~EXROM */
    enable_trigger = 0;
    cartridge_config_changed(ramconfig);
}

int expert_bin_attach(const char *filename, BYTE *rawcart)
{
    memset(rawcart, 0xff, 0x2000);
    /* Set default mode */
    resources_set_value("CartridgeMode",
                        (resource_value_t)CARTRIDGE_MODE_PRG);
    return 0;
}

int expert_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1)
        return -1;

    if (fread(rawcart, 0x2000, 1, fd) < 1)
        return -1;

    resources_set_value("CartridgeMode",
                        (resource_value_t)CARTRIDGE_MODE_ON);

    return 0;
}

