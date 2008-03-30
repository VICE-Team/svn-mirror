/*
 * expert.c - Cartridge handling, Expert cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "cartridge.h"
#include "expert.h"
#include "interrupt.h"
#include "resources.h"
#include "reu.h"
#include "types.h"
#include "vicii-phi1.h"


/* Cartridge mode.  */
extern int cartmode;

static int ack_reset = 0;

/* De-assert ~GAME */
/* Assert ~EXROM */
/* Disable export_ram */
#define EXPERT_PRG      ((0 << 0) | (0 << 1))

/* De-assert ~GAME */
/* De-assert ~EXROM */
/* Disable export_ram */
#define EXPERT_OFF ((0 << 0) | (1 << 1))

/* Enable ~GAME */
/* Disable ~EXROM */
/* Disable export_ram */
#define EXPERT_ON       ((1 << 0) | (1 << 1))

BYTE REGPARM1 expert_io1_read(WORD addr)
{
    if (cartmode == CARTRIDGE_MODE_ON)
        cartridge_config_changed(EXPERT_OFF, EXPERT_OFF, CMODE_READ);
    return 0;
}

void REGPARM2 expert_io1_store(WORD addr, BYTE value)
{
    if (cartmode == CARTRIDGE_MODE_ON)
        cartridge_config_changed(EXPERT_OFF, EXPERT_OFF, CMODE_READ);
}

BYTE REGPARM1 expert_io2_read(WORD addr)
{
    if (reu_enabled)
        return reu_read((WORD)(addr & 0x0f));
    return vicii_read_phi1();
}

void REGPARM2 expert_io2_store(WORD addr, BYTE value)
{
    if (reu_enabled)
        reu_store((WORD)(addr & 0x0f), value);
}

BYTE REGPARM1 expert_roml_read(WORD addr)
{
    return roml_banks[addr & 0x1fff];
}

void REGPARM2 expert_roml_store(WORD addr, BYTE value)
{
    roml_banks[addr & 0x1fff] = value;
}

BYTE REGPARM1 expert_romh_read(WORD addr)
{
    return roml_banks[addr & 0x1fff];
}

void expert_ack_nmi(void)
{
    if (cartmode == CARTRIDGE_MODE_ON)
        cartridge_config_changed(EXPERT_ON, EXPERT_ON, CMODE_READ);
}

void expert_ack_reset(void)
{
    if (cartmode == CARTRIDGE_MODE_ON) {
        ack_reset = 1;
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
    interrupt_set_nmi_trap_func(maincpu_int_status, expert_ack_nmi);
    interrupt_set_reset_trap_func(maincpu_int_status, expert_ack_reset);
    /*
     * Initialize cartridge mode/configuration.
     */
    if (!ack_reset) {
        expert_mode_changed(cartmode);
    } else {
        /*
         * Do ack_reset mapping.
         */
        cartridge_config_changed(EXPERT_ON, EXPERT_ON, CMODE_READ);
        ack_reset = 0;
    }
}

void expert_config_setup(BYTE *rawcart)
{
        /* Clear Expert RAM */
    memcpy(roml_banks, rawcart, 0x2000);
}

int expert_bin_attach(const char *filename, BYTE *rawcart)
{
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

void expert_mode_changed(int mode)
{
    switch(mode) {
      case(CARTRIDGE_MODE_PRG):
        cartridge_config_changed(EXPERT_PRG, EXPERT_PRG, CMODE_READ);
        break;
      case(CARTRIDGE_MODE_OFF):
      case(CARTRIDGE_MODE_ON):
        cartridge_config_changed(EXPERT_OFF, EXPERT_OFF, CMODE_READ);
        break;
    }
}

