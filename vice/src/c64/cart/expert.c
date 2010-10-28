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
#include "c64cartsystem.h"
#include "c64export.h"
#include "c64mem.h"
#include "c64io.h"
#include "cartridge.h"
#include "cmdline.h"
#include "crt.h"
#define CARTRIDGE_INCLUDE_PRIVATE_API
#include "expert.h"
#undef CARTRIDGE_INCLUDE_PRIVATE_API
#include "interrupt.h"
#include "lib.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "util.h"

/*
    FIXME: the following description is atleast inaccurate, if not plain wrong.

    Trilogic Expert Cartridge

    - one 8K RAM (!) bank
    - IO1 seems to be connected to a flipflop, which makes a single bit "register".
      wether it is decoded fully or mirrored over the full IO1 range is unknown
      (the software only uses $de00).
      - any access to io1 toggles the flipflop, which means enabling or disabling
        ROMH (exrom) (?)

    the cartridge has a 3 way switch:

    PRG:

      - NMI logic and registers are disabled
      - RAM is mapped to 8000 (writeable) in 8k game mode

    ON:

    - after switching from OFF to ON
      - NMI logic is active. the "freezer" can now be activated by either
        pressing restore or the freezer button.
      - Io1 "register" logic is disabled
      - RAM not mapped

    - after reset:
      - RAM is mapped to 8000 (writeable) and E000 (read only) in ultimax mode
      - Io1 "register" logic is enabled (?)

    - on NMI (restore or freezer button pressed):
      - RAM is mapped to 8000 (writeable) and E000 (read only) in ultimax mode
      - Io1 "register" logic is enabled

    OFF:

      - NMI logic and registers are disabled
      - RAM not mapped

    - according to the documentation, the cartridge is disabled. however,
      the NMI logic of the cart still seems to interfer somehow and makes
      some program misbehave. the solution for this was to put an additional
      switch at the NMI line of the cartridge port, which then allows to 
      completely disable the cartridge for real.

      this misbehavior is NOT emulated

    there also was an "upgrade" to the hardware at some point, called "EMS".
    this pretty much was no more no less than a freezer button :=)

    short instructions:

    note: the state of the switch and the order of doing things is very important,
          making a mistake here will almost certainly cause it not to work as
          expected. also, since the software tracks its state internally to act
          accordingly, saving it will not result in a runable image at any time.

    preparing the expert ram (skip this if you use a crt file)

    - switch to OFF
    - reset
    - switch to PRG
    - load and run the install software
    - switch to ON (as beeing told on screen)
    - reset. this will usually start some menu/intro screen
    - space to go to monitor

    using the freezer / monitor

    - "pp" or "p" clears memory (and will reset)
    - (if you want to save a crt file for later use, do that now)
    - switch to OFF
    - reset
    - load and run game
    - switch to ON
    - hit restore (or freezer button)
    - z "name" to save backup
    - r to restart running program
*/

/*
    notes from c64mem.c:
    - ROML is enabled at memory configs 11, 15, 27, 31 and Ultimax.
    - Allow writing at ROML at $8000-$9FFF.
    - Allow ROML being visible independent of charen, hiram & loram
    - Copy settings from "normal" operation mode into "ultimax" configuration.
*/

/*
this sequence from expert 2.10 indicates that a full ROM is available at E000
when the cartridge is ON, HIROM is selected.

it also indicates that the de00 register is a toggle

.C:038b   A9 37      LDA #$37
.C:038d   85 01      STA $01
.C:038f   AD 00 DE   LDA $DE00  toggle expert RAM on/off
.C:0392   AD BD FD   LDA $FDBD  fdbd is $00 in kernal rom
.C:0395   D0 F8      BNE $038F  do again if expert RAM not off

Reset entry from expert 2.70:

.C:9fd1   78         SEI
.C:9fd2   D8         CLD
.C:9fd3   A2 FF      LDX #$FF
.C:9fd5   9A         TXS
.C:9fd6   E8         INX
.C:9fd7   8E F2 9F   STX $9FF2  set to 0

.C:03b0   48         PHA
.C:03b1   A9 37      LDA #$37
.C:03b3   85 01      STA $01
.C:03b5   AD 00 DE   LDA $DE00  toggle expert RAM on/off  
.C:03b8   AD 00 E0   LDA $E000  is $85 in kernal
.C:03bb   C9 85      CMP #$85
.C:03bd   D0 F6      BNE $03B5
.C:03bf   68         PLA
.C:03c0   60         RTS

NMI entry from expert 2.70:

.C:9c00   2C F2 9F   BIT $9FF2  get bit7 into N
.C:9c03   10 01      BPL $9C06  if N=0 branch
.C:9c05   40         RTI

.C:9c06   78         SEI
.C:9c07   8D 01 80   STA $8001
.C:9c0a   A9 FF      LDA #$FF
.C:9c0c   8D F2 9F   STA $9FF2
*/

/* #define DBGEXPERT */

#ifdef DBGEXPERT
#define DBG(x) printf x
#else
#define DBG(x)
#endif

#define USEFAKEPRGMAPPING       1 /* emulate PRG mode as 8k game */

#if USEFAKEPRGMAPPING
#define EXPERT_PRG ((0 << 0) | (0 << 1)) /* 8k game */
#else
#define EXPERT_PRG ((1 << 0) | (1 << 1)) /* ultimax */
#endif
#define EXPERT_OFF ((0 << 0) | (1 << 1)) /* ram */
#define EXPERT_ON  ((1 << 0) | (1 << 1)) /* ultimax */

static int cartmode = EXPERT_MODE_DEFAULT;
static int expert_enabled = 0;
static int expert_register_enabled = 0;
static int expert_ram_writeable = 0;
static int expert_ramh_enabled = 0; /* equals EXROM ? */

/* 8 KB RAM */
static BYTE *expert_ram = NULL;

static char *expert_filename = NULL;
static int expert_filetype = 0;

#define EXPERT_RAM_SIZE 8192

static const char STRING_EXPERT[] = "Expert Cartridge";

BYTE REGPARM1 expert_io1_read(WORD addr);
void REGPARM2 expert_io1_store(WORD addr, BYTE value);
BYTE REGPARM1 expert_io2_read(WORD addr);
void REGPARM2 expert_io2_store(WORD addr, BYTE value);

static io_source_t expert_io1_device = {
    "Expert",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xde01, 0xff,
    0, /* read is never valid */
    NULL,
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
    if (expert_enabled) {
        switch (mode) {
            case EXPERT_MODE_PRG:
                cartridge_config_changed(2, EXPERT_PRG, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
                expert_register_enabled = 1;
                expert_ramh_enabled = 0;
                expert_ram_writeable = 1;
                break;
            case EXPERT_MODE_ON:
                cartridge_config_changed(2, 2, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
                expert_register_enabled = 0;
                expert_ramh_enabled = 0;
                expert_ram_writeable = 0;
                break;
            case EXPERT_MODE_OFF:
                cartridge_config_changed(2, EXPERT_OFF, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
                expert_register_enabled = 0;
                expert_ramh_enabled = 0;
                expert_ram_writeable = 0;
                break;
        }
    }

    return 0;
}

static int set_expert_enabled(int val, void *param)
{
    DBG(("EXPERT: set enabled: %d:%d\n", expert_enabled, val));

    if (expert_enabled && !val) {
        DBG(("EXPERT: disable\n"));
        lib_free(expert_ram);
        expert_ram = NULL;
        if (expert_filename) {
            lib_free(expert_filename);
            expert_filename = NULL;
        }
        c64io_unregister(expert_io1_list_item);
        expert_io1_list_item = NULL;
        c64export_remove(&export_res);
        expert_enabled = 0;
        cart_power_off();
    } else if (!expert_enabled && val) {
        DBG(("EXPERT: enable\n"));
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
        resources_set_int("ExpertCartridgeMode", cartmode);
        cart_power_off();
    }
    
    return 0;
}
/* ---------------------------------------------------------------------*/

BYTE REGPARM1 expert_io1_read(WORD addr)
{
    expert_io1_device.io_source_valid = 0;
    /* DBG(("EXPERT: io1 rd %04x (%d)\n", addr, expert_ramh_enabled)); */
    if ((cartmode == EXPERT_MODE_ON) && (expert_register_enabled == 1)) {
        cartridge_config_changed(2, 3, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_ramh_enabled ^= 1;
        expert_ram_writeable = 0; /* =0 ? */
        DBG(("EXPERT: ON (regs: %d ramh: %d ramwrite: %d)\n",expert_register_enabled, expert_ramh_enabled, expert_ram_writeable));
    }
    return 0;
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
        return ram_read(addr);
    }
}

void REGPARM2 expert_roml_store(WORD addr, BYTE value)
{
/*    DBG(("EXPERT: set expert_roml_store: %x\n", addr)); */
    if (expert_ram_writeable) {
        if (cartmode == EXPERT_MODE_PRG) {
            expert_ram[addr & 0x1fff] = value;
        } else if (cartmode == EXPERT_MODE_ON) {
            expert_ram[addr & 0x1fff] = value;
        } else {
            /* mem_store_without_ultimax(addr, value); */
            ram_store(addr, value);
        }
    } else {
        ram_store(addr, value);
    }
}

BYTE REGPARM1 expert_romh_read(WORD addr)
{
/*    DBG(("EXPERT: set expert_romh_read: %x mode %d %02x %02x\n", addr, cartmode, expert_ram[0x1ffe], expert_ram[0x1fff])); */
    if ((cartmode == EXPERT_MODE_ON) && expert_ramh_enabled) {
        return expert_ram[addr & 0x1fff];
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
    if (cartmode == EXPERT_MODE_ON) {
        DBG(("EXPERT: freeze\n"));
        /* DBG(("ram %02x %02x\n", expert_ram[0x1ffe], expert_ram[0x1fff])); */
        cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_register_enabled = 1;
        expert_ram_writeable = 1;
        expert_ramh_enabled = 1;
    }
}

void expert_ack_nmi(void)
{
    if (cartmode == EXPERT_MODE_ON) {
        DBG(("EXPERT:ack nmi\n"));
        /* DBG(("ram %02x %02x\n", expert_ram[0x1ffe], expert_ram[0x1fff])); */
        cartridge_config_changed(2, EXPERT_ON, CMODE_READ | CMODE_RELEASE_FREEZE | CMODE_PHI2_RAM);
        expert_register_enabled = 1;
        expert_ram_writeable = 1;
        expert_ramh_enabled = 1;
    }
}

void expert_reset(void)
{
    DBG(("EXPERT: reset\n"));
    if (cartmode == EXPERT_MODE_ON) {
        expert_register_enabled = 1;
        expert_ram_writeable = 1;
        expert_ramh_enabled = 1;
        cartridge_config_changed(2, 3, CMODE_READ | CMODE_PHI2_RAM);
    } else if (cartmode == EXPERT_MODE_PRG) {
        expert_register_enabled = 1;
        expert_ram_writeable = 1;
        expert_ramh_enabled = 0;
        cartridge_config_changed(2, EXPERT_PRG, CMODE_READ);
    } else {
        expert_register_enabled = 0;
        expert_ram_writeable = 0;
        expert_ramh_enabled = 0;
        cartridge_config_changed(2, EXPERT_OFF, CMODE_READ | CMODE_PHI2_RAM);
    }
}

/* ---------------------------------------------------------------------*/

void expert_config_init(void)
{
    if (expert_enabled) {
        DBG(("EXPERT: config_init cartmode: %d\n", cartmode));
        expert_reset();
        interrupt_set_nmi_trap_func(maincpu_int_status, expert_ack_nmi);
    }
}

void expert_config_setup(BYTE *rawcart)
{
    memcpy(expert_ram, rawcart, EXPERT_RAM_SIZE);
    /* DBG(("ram %02x %02x\n", expert_ram[0x1ffe], expert_ram[0x1fff])); */
}

/* ---------------------------------------------------------------------*/

const char *expert_get_file_name(void)
{
    return expert_filename;
}

static void expert_set_filename(const char *filename)
{
    if (expert_filename) {
        lib_free(expert_filename);
    }
    expert_filename = strdup(filename);
}

static int expert_common_attach(void)
{
    DBG(("EXPERT: common attach\n"));
    if (resources_set_int("ExpertCartridgeEnabled", 1) < 0) {
        return -1;
    }
    if (expert_enabled) {
        /* Set default mode
        here we want to load a previously saved image. we use ON as
        default here.
        */
        resources_set_int("ExpertCartridgeMode", EXPERT_MODE_ON);
        DBG(("EXPERT: common attach ok\n"));
        return 0;
    }
    return -1;
}

int expert_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, EXPERT_RAM_SIZE, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    expert_set_filename(filename);
    expert_filetype = CARTRIDGE_FILETYPE_BIN;

    return expert_common_attach();
}

int expert_bin_save(const char *filename)
{
    FILE *fd;

    if (expert_ram == NULL) {
        DBG(("EXPERT: ERROR expert_ram == NULL\n"));
        return -1;
    }

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

int expert_crt_attach(FILE *fd, BYTE *rawcart, const char *filename)
{
    BYTE chipheader[0x10];

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    if (fread(rawcart, EXPERT_RAM_SIZE, 1, fd) < 1) {
        return -1;
    }

    expert_set_filename(filename);
    expert_filetype = CARTRIDGE_FILETYPE_CRT;

    return expert_common_attach();
}

int expert_crt_save(const char *filename)
{
    FILE *fd;
    BYTE header[0x40], chipheader[0x10];

    DBG(("EXPERT: save crt '%s'\n", filename));

    if (expert_ram == NULL) {
        DBG(("EXPERT: ERROR expert_ram == NULL\n"));
        return -1;
    }

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
    if (fwrite(expert_ram, sizeof(char), EXPERT_RAM_SIZE, fd) != EXPERT_RAM_SIZE) {
        fclose(fd);
        return -1;
    }

    fclose(fd);
    DBG(("EXPERT: ERROR save crt ok\n"));

    return 0;
}

int expert_flush_image(void)
{
    if (expert_filetype == CARTRIDGE_FILETYPE_BIN) {
        return expert_bin_save(expert_filename);
    } else if (expert_filetype == CARTRIDGE_FILETYPE_CRT) {
        return expert_crt_save(expert_filename);
    }
    return -1;
}

void expert_detach(void)
{
    resources_set_int("ExpertCartridgeEnabled", 0);
}

int expert_enable(void)
{
    DBG(("EXPERT: enable\n"));
    if (resources_set_int("ExpertCartridgeEnabled", 1) < 0) {
        return -1;
    }
    return 0;
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
