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

#include "archdep.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64mem.h"
#include "c64io.h"
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#include "expert.h"
#include "interrupt.h"
#include "lib.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "util.h"

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
static int expert_enabled = 0;
static int expert_ram_enabled = 0;

/* 8 KB RAM */
static BYTE *expert_ram = NULL;

#define EXPERT_RAM_SIZE 8192

static const char STRING_EXPERT[] = "Expert Cartridge";

BYTE REGPARM1 expert_io1_read(WORD addr);
void REGPARM2 expert_io1_store(WORD addr, BYTE value);

static io_source_t expert_io1_device = {
    "Expert",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    expert_io1_store,
    expert_io1_read,
    NULL,
    NULL,
    CARTRIDGE_EXPERT
};

static const c64export_resource_t export_res = {
    "Expert Cartridge", 1, 1, &expert_io1_device, NULL, CARTRIDGE_EXPERT
};

static io_source_list_t *expert_io1_list_item = NULL;

/* ---------------------------------------------------------------------*/
int expert_cart_enabled(void)
{
    if (expert_enabled) {
        return 1;
    }
    return 0;
}

static int expert_mode_changed(int mode, void *param)
{
    cartmode = mode;
    DBG(("EXPERT: expert_mode_changed cartmode: %d\n", cartmode));
    switch (mode) {
        case EXPERT_MODE_PRG:
            cartridge_config_changed(2, EXPERT_PRG, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
            expert_ram_enabled = 0;
            break;
        case EXPERT_MODE_ON:
#if USEFAKEONMAPPING
            cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
#else
            cartridge_config_changed(2, 2, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
#endif
            expert_ram_enabled = 1;
            break;
        case EXPERT_MODE_OFF:
            cartridge_config_changed(2, EXPERT_OFF, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
            expert_ram_enabled = 0;
            break;
    }

    return 0;
}

static int set_expert_enabled(int val, void *param)
{
    DBG(("EXPERT: set enabled: %d\n", val));

    if (expert_enabled && !val) {
        lib_free(expert_ram);
        expert_ram = NULL;
        c64io_unregister(expert_io1_list_item);
        expert_io1_list_item = NULL;
        c64export_remove(&export_res);
        expert_enabled = 0;
    } else if (!expert_enabled && val) {
        expert_ram = lib_malloc(EXPERT_RAM_SIZE);
        expert_io1_list_item = c64io_register(&expert_io1_device);
        if (c64export_add(&export_res) < 0) {
            DBG(("EXPERT: set enabled: error\n"));
            lib_free(expert_ram);
            expert_ram = NULL;
            c64io_unregister(expert_io1_list_item);
            expert_io1_list_item = NULL;
            expert_enabled = 0;
            return -1;
        }
        expert_enabled = 1;

        /* Set default mode
           since we don't attach a binary image (yet), but use this function
           to enable the expert cartridge, use PRG mode for convinience.
        */
        resources_set_int("ExpertCartridgeMode", EXPERT_MODE_PRG);
    }
    
    return 0;
}
/* ---------------------------------------------------------------------*/

BYTE REGPARM1 expert_io1_read(WORD addr)
{
    if ((cartmode == EXPERT_MODE_ON) && (expert_ram_enabled == 1)) {
        DBG(("EXPERT: io1 rd %04x\n", addr));
        cartridge_config_changed(2, EXPERT_OFF, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_ram_enabled = 0;
    }
    return 0;
}

void REGPARM2 expert_io1_store(WORD addr, BYTE value)
{
    if ((cartmode == EXPERT_MODE_ON) && (expert_ram_enabled == 1)) {
        DBG(("EXPERT: io1 st %04x %02x\n", addr, value));
        cartridge_config_changed(2, EXPERT_OFF, CMODE_WRITE | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_ram_enabled = 0;
    }
}

/* ---------------------------------------------------------------------*/

BYTE REGPARM1 expert_roml_read(WORD addr)
{
/*    DBG(("EXPERT: set expert_roml_read: %x\n", addr)); */
    if (cartmode == EXPERT_MODE_PRG) {
        return expert_ram[addr & 0x1fff];
    } else if (cartmode == EXPERT_MODE_ON) {
        return expert_ram[addr & 0x1fff];
    } else {
        return mem_read_without_ultimax(addr);
    }
}

void REGPARM2 expert_roml_store(WORD addr, BYTE value)
{
/*    DBG(("EXPERT: set expert_roml_store: %x\n", addr)); */
    if (cartmode == EXPERT_MODE_PRG) {
        expert_ram[addr & 0x1fff] = value;
    } else {
        /* mem_store_without_ultimax(addr, value); */
        ram_store(addr, value);
    }
}

void REGPARM2 expert_raml_store(WORD addr, BYTE value)
{
/*    DBG(("EXPERT: set expert_raml_store: %x\n", value)); */
    if (cartmode == EXPERT_MODE_PRG) {
        expert_ram[addr & 0x1fff] = value;
    } else {
        ram_store(addr, value);
    }
}

BYTE REGPARM1 expert_romh_read(WORD addr)
{
    DBG(("EXPERT: set expert_romh_read: %p %x\n", expert_ram, addr));
    if (cartmode == EXPERT_MODE_ON) {
#if USEFAKEONMAPPING
        cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_ram_enabled = 1;
        return expert_ram[addr & 0x1fff];
#else
        /* FIXME: how exactly does that damn NMI logic work? */
        switch (addr) {
/*
            case 0xfffa:
            case 0xfffb:
            case 0xfffc:
            case 0xfffd:
*/
            case 0xfffe:
            case 0xffff:
                cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
                expert_ram_enabled = 1;
                return expert_ram[addr & 0x1fff];
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
        expert_ram_enabled = 1;
    }
}

void expert_ack_nmi(void)
{
    if (cartmode == EXPERT_MODE_ON) {
        DBG(("EXPERT:ack nmi\n"));
        cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_ram_enabled = 1;
    }
}

void expert_ack_reset(void)
{
    if (cartmode == EXPERT_MODE_ON) {
        DBG(("EXPERT: ack reset\n"));
        ack_reset = 1;
        expert_ram_enabled = 1;
    }
}

/* ---------------------------------------------------------------------*/

void expert_config_init(void)
{
    if (expert_enabled) {
        DBG(("EXPERT: config_init cartmode: %d\n", cartmode));

        expert_ram_enabled = 1;

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
}

void expert_config_setup(BYTE *rawcart)
{
    /* memcpy(roml_banks, rawcart, 0x2000); */
}

/* ---------------------------------------------------------------------*/

static int expert_common_attach(BYTE *rawcart)
{
    if (resources_set_int("ExpertCartridgeEnabled", 1) < 0) {
        return -1;
    }
    if (expert_enabled) {
        memcpy(expert_ram, rawcart, EXPERT_RAM_SIZE);
        /* Set default mode
        here we want to load a previously saved image. we use ON as
        default here, loaded program may be activated by NMI (restore,
        freeze).
        */
        resources_set_int("ExpertCartridgeMode", EXPERT_MODE_ON);
        return 0;
    }
    return -1;
}

int expert_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, EXPERT_RAM_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return expert_common_attach(rawcart);
}

int expert_bin_save(const char *filename)
{
    FILE *fd;

    if (filename == NULL) {
        return -1;
    }

    fd = fopen(filename, MODE_WRITE);

    if (fd == NULL) {
        return -1;
    }

    if (fwrite(expert_ram, 1, EXPERT_RAM_SIZE, fd) != EXPERT_RAM_SIZE) {
        fclose(fd);
        return -1;
    }

    fclose(fd);
    return 0;
}

int expert_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(rawcart, EXPERT_RAM_SIZE, 1, fd) < 1) {
        return -1;
    }

    return expert_common_attach(rawcart);
}

int expert_crt_save(const char *filename)
{
    FILE *fd;
    BYTE header[0x40], chipheader[0x10];

    fd = fopen(filename, MODE_WRITE);

    if (fd == NULL) {
        return -1;
    }

    /*
     * Initialize headers to zero.
     */
    memset(header, 0x0, 0x40);
    memset(chipheader, 0x0, 0x10);

    /*
     * Construct CRT header.
     */
    strcpy((char *)header, CRT_HEADER);

    /*
     * fileheader-length (= 0x0040)
     */
    header[0x10] = 0x00;
    header[0x11] = 0x00;
    header[0x12] = 0x00;
    header[0x13] = 0x40;

    /*
     * Version (= 0x0100)
     */
    header[0x14] = 0x01;
    header[0x15] = 0x00;

    /*
     * Hardware type (= CARTRIDGE_EXPERT)
     */
    header[0x16] = 0x00;
    header[0x17] = CARTRIDGE_EXPERT;

    /*
     * Exrom line
     */
    header[0x18] = 0x01;            /* ? */

    /*
     * Game line
     */
    header[0x19] = 0x01;            /* ? */

    /*
     * Set name.
     */
    strcpy((char *)&header[0x20], STRING_EXPERT);

    /*
     * Write CRT header.
     */
    if (fwrite(header, sizeof(BYTE), 0x40, fd) != 0x40) {
        fclose(fd);
        return -1;
    }

    /*
     * Construct chip packet.
     */
    strcpy((char *)chipheader, CHIP_HEADER);

    /*
     * Packet length. (= 0x2010; 0x10 + 0x2000)
     */
    chipheader[0x04] = 0x00;
    chipheader[0x05] = 0x00;
    chipheader[0x06] = 0x20;
    chipheader[0x07] = 0x10;

    /*
     * Chip type. (= FlashROM?)
     */
    chipheader[0x08] = 0x00;
    chipheader[0x09] = 0x02;

    /*
     * Bank nr. (= 0)
     */
    chipheader[0x0a] = 0x00;
    chipheader[0x0b] = 0x00;

    /*
     * Address. (= 0x8000)
     */
    chipheader[0x0c] = 0x80;
    chipheader[0x0d] = 0x00;

    /*
     * Length. (= 0x2000)
     */
    chipheader[0x0e] = 0x20;
    chipheader[0x0f] = 0x00;

    /*
     * Write CHIP header.
     */
    if (fwrite(chipheader, sizeof(BYTE), 0x10, fd) != 0x10) {
        fclose(fd);
        return -1;
    }

    /*
     * Write CHIP packet data.
     */
    if (fwrite(expert_ram, sizeof(char), 0x2000, fd) != 0x2000) {
        fclose(fd);
        return -1;
    }

    fclose(fd);

    return 0;
}

void expert_detach(void)
{
    resources_set_int("ExpertCartridgeEnabled", 0);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
    { "-expert", SET_RESOURCE, 0,
      NULL, NULL, "ExpertCartridgeEnabled", (resource_value_t)1,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_ENABLE_EXPERT_CART,
      NULL, NULL },
    { "+expert", SET_RESOURCE, 0,
      NULL, NULL, "ExpertCartridgeEnabled", (resource_value_t)0,
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      IDCLS_UNUSED, IDCLS_DISABLE_EXPERT_CART,
      NULL, NULL },
    { NULL }
};

int expert_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

static const resource_int_t resources_int[] = {
    { "ExpertCartridgeEnabled", 0, RES_EVENT_STRICT, (resource_value_t)0,
      &expert_enabled, set_expert_enabled, NULL },
    { "ExpertCartridgeMode", EXPERT_MODE_DEFAULT, RES_EVENT_NO, NULL,
      &cartmode, expert_mode_changed, NULL },
    { NULL }
};

int expert_resources_init(void)
{
    return resources_register_int(resources_int);
}

void expert_resources_shutdown(void)
{
}
