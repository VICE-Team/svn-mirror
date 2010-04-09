/*
 * expert.c - Cartridge handling, Expert cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Nathan Huizinga <nathan.huizinga@chess.nl>
 *  Groepaz <groepaz@gmx.net>
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
#include "c64export.h"
#include "c64io.h"
#include "cartridge.h"
#include "expert.h"
#include "interrupt.h"
#include "resources.h"
#include "reu.h"
#include "types.h"

/*
    Trilogic Expert Cartridge

    - one 8K RAM (!) bank
    - any access to IO1 area disables the cartridge (if ON)

    the cartridge has a 3 way switch:

    PRG:
    - RAM is mapped to 8000 and writeable

    ON:
    - NMI logic is active. on NMI the cartridge will be mapped to E000 for
      just the few cycles it takes the cpu to fetch the NMI vector, then it
      will be mapped to 8000 again. the "freezer" can now be activated by
      either pressing restore or the freezer button.

    OFF:
    - according to the documentation, the cartridge is disabled. however,
      the NMI logic of the cart still seems to interfer somehow and makes
      some program misbehave. the solution for this was to put an additional
      switch at the NMI line of the cartridge port, which then allows to 
      completely disable the cartridge for real.

      this misbehavior is NOT emulated

    there also was an "upgrade" to the hardware at some point, called "EMS".
    this pretty much was no more no less than a freezer button :=)

*/

/* De-assert ~GAME */
/* Assert ~EXROM */
/* Disable export_ram */
#define EXPERT_PRG ((0 << 0) | (0 << 1))

/* De-assert ~GAME */
/* De-assert ~EXROM */
/* Disable export_ram */
#define EXPERT_OFF ((0 << 0) | (1 << 1))

/* Enable ~GAME */
/* Disable ~EXROM */
/* Disable export_ram */
#define EXPERT_ON ((1 << 0) | (1 << 1))

static int ack_reset = 0;

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

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 expert_io1_read(WORD addr)
{
    if (cartmode == CARTRIDGE_MODE_ON) {
        cartridge_config_changed(EXPERT_OFF, EXPERT_OFF, CMODE_READ);
    }
    return 0;
}

void REGPARM2 expert_io1_store(WORD addr, BYTE value)
{
    if (cartmode == CARTRIDGE_MODE_ON) {
        cartridge_config_changed(EXPERT_OFF, EXPERT_OFF, CMODE_READ);
    }
}

/* ---------------------------------------------------------------------*/

static io_source_t expert_device = {
    "EXPERT",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    expert_io1_store,
    expert_io1_read
};

static io_source_list_t *expert_list_item = NULL;

/* ---------------------------------------------------------------------*/

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

/* ---------------------------------------------------------------------*/

void expert_ack_nmi(void)
{
    if (cartmode == CARTRIDGE_MODE_ON) {
        cartridge_config_changed(EXPERT_ON, EXPERT_ON | 0x40, CMODE_READ);
    }
}

void expert_ack_reset(void)
{
    if (cartmode == CARTRIDGE_MODE_ON) {
        ack_reset = 1;
    }
}

/* ---------------------------------------------------------------------*/

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

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res = {
    "Expert", 1, 1
};

static int expert_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    expert_list_item = c64io_register(&expert_device);

    return 0;
}

int expert_bin_attach(const char *filename, BYTE *rawcart)
{
    /* Set default mode
       HACK: since we dont actually attach a binary image, but abuse this
             function to enable the expert cartridge, use PRG mode for
             convinience.
    */
    resources_set_int("CartridgeMode", CARTRIDGE_MODE_PRG);

    return expert_common_attach();
}

int expert_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(rawcart, 0x2000, 1, fd) < 1) {
        return -1;
    }

    /* Set default mode
       here we want to load a previously saved image. we use OFF as
       default here, loaded program may be activated by NMI (restore,
       freeze) or reset.
    */
    resources_set_int("CartridgeMode", CARTRIDGE_MODE_OFF);

    return expert_common_attach();
}

void expert_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(expert_list_item);
    expert_list_item = NULL;
}
