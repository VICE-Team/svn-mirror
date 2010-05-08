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
#include "c64mem.h"
#include "c64io.h"
#include "cartridge.h"
#include "expert.h"
#include "interrupt.h"
#include "resources.h"
#include "reu.h"
#include "types.h"

/* #define DBGEXPERT */

#ifdef DBGEXPERT
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/*
    FIXME: the following description is atleast inaccurate, if not plain wrong.

    Trilogic Expert Cartridge

    - one 8K RAM (!) bank
    - any access to IO1 area disables the cartridge (if ON)

    the cartridge has a 3 way switch:

    PRG:
    - NMI logic and registers are disabled

      - RAM is mapped to 8000 (writeable)

    ON:
    - after reset: NMI logic is active. the "freezer" can now be activated by
      either pressing restore or the freezer button.

      - RAM not mapped

    - freezer button pressed: on NMI the cartridge will be mapped to E000 for
      just the few cycles it takes the cpu to fetch the NMI vector, then it
      will be mapped to 8000 again.

      - RAM is mapped to E000 (read only)
      - any access to IO1 area disables the RAM at E000

    OFF:
    - according to the documentation, the cartridge is disabled. however,
      the NMI logic of the cart still seems to interfer somehow and makes
      some program misbehave. the solution for this was to put an additional
      switch at the NMI line of the cartridge port, which then allows to 
      completely disable the cartridge for real.

      this misbehavior is NOT emulated

      - RAM not mapped

    there also was an "upgrade" to the hardware at some point, called "EMS".
    this pretty much was no more no less than a freezer button :=)

*/

/*
this sequence from expert 2.10 indicates that a full ROM is available at E000
when the cartridge is ON, HIROM is selected.

.C:038b   A9 37      LDA #$37
.C:038d   85 01      STA $01
.C:038f   AD 00 DE   LDA $DE00
.C:0392   AD BD FD   LDA $FDBD  fdbd is $00 in kernal rom
.C:0395   D0 F8      BNE $038F
*/

#define USEFAKEONMAPPING        1 /* permanently use ultimax in ON mode */
#define USEFAKEPRGMAPPING       0 /* emulate PRG mode as 8k game */

#if USEFAKEPRGMAPPING
#define EXPERT_PRG ((0 << 0) | (0 << 1)) /* 8k game */
#else
#define EXPERT_PRG ((1 << 0) | (1 << 1)) /* ultimax */
#endif
#define EXPERT_OFF ((0 << 0) | (1 << 1)) /* ram */
#define EXPERT_ON  ((1 << 0) | (1 << 1)) /* ultimax */

static int ack_reset = 0;
static int cartmode = EXPERT_MODE_DEFAULT;
static int expert_enabled;

static int expert_mode_changed(int mode, void *param)
{
    cartmode = mode;
    DBG(("expert_mode_changed cartmode: %d\n", cartmode));
    switch (mode) {
        case EXPERT_MODE_PRG:
            cartridge_config_changed(2, EXPERT_PRG, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
            expert_enabled = 0;
            break;
        case EXPERT_MODE_ON:
#if USEFAKEONMAPPING
            cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
#else
            cartridge_config_changed(2, 2, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
#endif
            expert_enabled = 1;
            break;
        case EXPERT_MODE_OFF:
            cartridge_config_changed(2, EXPERT_OFF, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
            expert_enabled = 0;
            break;
    }

    return 0;
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 expert_io1_read(WORD addr)
{
    if ((cartmode == EXPERT_MODE_ON) && (expert_enabled == 1)) {
        DBG(("EXPERT: io1 rd %04x\n", addr));
        cartridge_config_changed(2, EXPERT_OFF, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_enabled = 0;
    }
    return 0;
}

void REGPARM2 expert_io1_store(WORD addr, BYTE value)
{
    if ((cartmode == EXPERT_MODE_ON) && (expert_enabled == 1)) {
        DBG(("EXPERT: io1 st %04x %02x\n", addr, value));
        cartridge_config_changed(2, EXPERT_OFF, CMODE_WRITE | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_enabled = 0;
    }
}

/* ---------------------------------------------------------------------*/

static io_source_t expert_device = {
    "Expert",
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
    if (cartmode == EXPERT_MODE_PRG) {
        return roml_banks[addr & 0x1fff];
    } else if (cartmode == EXPERT_MODE_ON) {
        return roml_banks[addr & 0x1fff];
    } else {
        return mem_read_without_ultimax(addr);
    }
}

void REGPARM2 expert_roml_store(WORD addr, BYTE value)
{
    if (cartmode == EXPERT_MODE_PRG) {
        roml_banks[addr & 0x1fff] = value;
    } else {
        mem_store_without_ultimax(addr, value);
    }
}

void REGPARM2 expert_raml_store(WORD addr, BYTE value)
{
    if (cartmode == EXPERT_MODE_PRG) {
        roml_banks[addr & 0x1fff] = value;
    } else {
        ram_store(addr, value);
    }
}

BYTE REGPARM1 expert_romh_read(WORD addr)
{
    if (cartmode == EXPERT_MODE_ON) {
#if USEFAKEONMAPPING
        return roml_banks[addr & 0x1fff];
#else
        /* FIXME: how exactly does that damn NMI logic work? */
        switch (addr) {
            case 0xfffa:
            case 0xfffb:
            case 0xfffc:
            case 0xfffd:
            case 0xfffe:
            case 0xffff:
                return roml_banks[addr & 0x1fff];
                break;
            default:
                return mem_read_without_ultimax(addr);
                break;
        }
#endif
    } else {
        return mem_read_without_ultimax(addr);
    }
}

/* ---------------------------------------------------------------------*/

int expert_freeze_allowed(void)
{
    if (cartmode == EXPERT_MODE_ON) {
        return 1;
    }
    return 0;
}

void expert_freeze(void)
{
    DBG(("EXPERT: freeze\n"));
    if (cartmode == EXPERT_MODE_ON) {
        cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_enabled = 1;
    }
}

void expert_ack_nmi(void)
{
    if (cartmode == EXPERT_MODE_ON) {
        DBG(("EXPERT:ack nmi\n"));
        cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_enabled = 1;
    }
}

void expert_ack_reset(void)
{
    if (cartmode == EXPERT_MODE_ON) {
        DBG(("EXPERT: ack reset\n"));
        ack_reset = 1;
        expert_enabled = 1;
    }
}

/* ---------------------------------------------------------------------*/

void expert_config_init(void)
{
    DBG(("EXPERT: config_init cartmode: %d\n", cartmode));

    expert_enabled = 1;

    /*
     * Initialize nmi/reset trap functions.
     */
    interrupt_set_nmi_trap_func(maincpu_int_status, expert_ack_nmi);
    interrupt_set_reset_trap_func(maincpu_int_status, expert_ack_reset);
    /*
     * Initialize cartridge mode/configuration.
     */
    if (!ack_reset) {
        expert_mode_changed(cartmode, NULL);
    } else {
        /*
         * Do ack_reset mapping.
         * - reset with switch ON starts the cart with cart RAM not mapped
         */
        cartridge_config_changed(2, 2, CMODE_READ | CMODE_PHI2_RAM);
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
    resources_set_int("ExpertCartridgeMode", EXPERT_MODE_PRG);

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
       here we want to load a previously saved image. we use ON as
       default here, loaded program may be activated by NMI (restore,
       freeze).
    */
    resources_set_int("ExpertCartridgeMode", EXPERT_MODE_ON);

    return expert_common_attach();
}

void expert_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(expert_list_item);
    expert_list_item = NULL;
}

static const resource_int_t resources_int[] = {
    { "ExpertCartridgeMode", EXPERT_MODE_DEFAULT, RES_EVENT_NO, NULL,
      &cartmode,
      expert_mode_changed, NULL },
    { NULL }
};

int expert_resources_init(void)
{
    return resources_register_int(resources_int);
}

void expert_resources_shutdown(void)
{
}
