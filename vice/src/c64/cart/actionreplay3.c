/*
 * actionreplay3.c - Cartridge handling, Action Replay III cart.
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
#include <string.h>

#include "actionreplay.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cartridge.h"
#include "types.h"
#include "util.h"

/*
    FIXME: everything here is purely guesswork and doesnt quite work like it should

    Action Replay 3

    Short Instructions:

    after reset, there dont seem to be any basic extensions, just a fastloader

    press freeze, then:
    - f1/f2  restart
    - f7/f8  backup to disk ?
    - b ? (crash ?)
    - c ? (delay, blanked screen)
    - d      backup to disk ?
    - f      freeze program + save
    - h ?
    - k ?
    - m ?    backup to disk ?
    - s ?    backup to disk ?
    - t      backup to tape ?
    - x ? (delay, blanked screen)
    - fire port 1 ?
    - left arrow ? (crash ?)

    Technical:

    - Freeze Button, Reset Button. rumours are that there exist ARs with a switch too
    - 16k ROM, 2*8kb banks

    io1:

    - is accessed in a loop, probably to re-enable the rom. however how exactly it does
      that remains unclear.

    io2:

    - last page of selected rom bank is visible here
    - accesses disable the ROM
*/

/*

; reset vector in bank 1
.C:8000
    .word $8030

; setup flags and stack pointer
.C:8030   78         SEI
.C:8031   D8         CLD
.C:8032   A2 FF      LDX #$FF
.C:8034   9A         TXS
; setup pla
.C:8035   A9 27      LDA #$27
.C:8037   85 01      STA $01
.C:8039   A9 2F      LDA #$2F
.C:803b   85 00      STA $00
; push some return values to stack
.C:803d   A2 0F      LDX #$0F
.C:803f   BD 47 80   LDA $8047,X
.C:8042   48         PHA
.C:8043   CA         DEX
.C:8044   10 F9      BPL $803F
.C:8046   60         RTS

.C:8047
    .word $ff84-1  1
    .word $df15-1
    .word $80f9-1
    .word $80ff-1
    .word $df15-1  2
    .word $80c3-1  3
    .word $8057-1
    .word $ff84-1  

.C:ff84   4C A3 FD   JMP $FDA3 ; Init I/O Devices, Ports & Timers
...


.C:8057   A2 09      LDX #$09
.C:8059   86 C6      STX $C6

.C:805b   BD 26 80   LDA $8026,X
.C:805e   9D 76 02   STA $0276,X
.C:8061   BD B9 80   LDA $80B9,X
.C:8064   9D 0A C0   STA $C00A,X
.C:8067   CA         DEX
.C:8068   D0 F1      BNE $805B

.C:806a   A9 AA      LDA #$AA
.C:806c   8D 05 80   STA $8005

#.C:806f   20 0B C0   JSR $C00B
#.C:c00b   20 38 DF   JSR $DF38

; accessing io2 seems to disable the ROM somehow

; save akku and status on stack
.C:df38   08         PHP ; carry saved
.C:df39   48         PHA

; disable the ROM
.C:df3a   A9 FF      LDA #$FF
.C:df3c   E9 01      SBC #$01
.C:df3e   48         PHA
.C:df3f   68         PLA
.C:df40   B0 FA      BCS $DF3C

; restore akku and status
.C:df42   68         PLA
.C:df43   28         PLP ; carry restored
.C:df44   58         CLI

#.C:df45   60         RTS

.C:c00e   CD 05 80   CMP $8005

#.C:c011   4C 15 DF   JMP $DF15

.C:df15   78         SEI
; save akku and status on stack
.C:df16   08         PHP
.C:df17   48         PHA

; this loop perhaps re-enables the ROM
.C:df18   A9 0C      LDA #$0C
.C:df1a   FE 00 DE   INC $DE00,X ; x = 0
.C:df1d   FE 00 DE   INC $DE00,X
.C:df20   E9 01      SBC #$01
.C:df22   B0 F6      BCS $DF1A

; restore akku and status
.C:df24   68         PLA
.C:df25   28         PLP

#.C:df26   60         RTS

; loop forever ...
.C:8072   D0 FE      BNE $8072

*/

/* #define DEBUGAR */
/* #define USEFAKE */

#ifdef DEBUGAR
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/* ---------------------------------------------------------------------*/

/* some prototypes are needed */
static BYTE REGPARM1 actionreplay3_io1_read(WORD addr);
static BYTE REGPARM1 actionreplay3_io1_peek(WORD addr);
static void REGPARM2 actionreplay3_io1_store(WORD addr, BYTE value);
static BYTE REGPARM1 actionreplay3_io2_read(WORD addr);
static BYTE REGPARM1 actionreplay3_io2_peek(WORD addr);

static io_source_t actionreplay3_io1_device = {
    "Action Replay III",
    IO_DETACH_CART,
    NULL,
    0xde00, 0xdeff, 0xff,
    0, /* read is never valid */
    actionreplay3_io1_store,
    actionreplay3_io1_read,
    actionreplay3_io1_peek,
    NULL, /* FIXME: dump */
    CARTRIDGE_ACTION_REPLAY3
};

static io_source_t actionreplay3_io2_device = {
    "Action Replay III",
    IO_DETACH_CART,
    NULL,
    0xdf00, 0xdfff, 0xff,
    1, /* read is always valid */
    NULL,
    actionreplay3_io2_read,
    actionreplay3_io2_peek,
    NULL, /* FIXME: dump */
    CARTRIDGE_ACTION_REPLAY3
};

static io_source_list_t *actionreplay3_io1_list_item = NULL;
static io_source_list_t *actionreplay3_io2_list_item = NULL;

/* ---------------------------------------------------------------------*/

#ifdef USEFAKE

int fake_addr = 0, fake_count = 0;

void dofake(int addr) {
    if (addr == fake_addr) {
        if ((addr == 0xde00)&&(fake_count == 1)) {
            DBG(("AR3: enabling\n"));
            cartridge_config_changed((BYTE) 0 | (1 << CMODE_BANK_SHIFT), (BYTE) 0 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
        } else if ((addr == 0xdf00)&&(fake_count == 1000)) {
            DBG(("AR3: disabling\n"));
            cartridge_config_changed((BYTE) 2 | (1 << CMODE_BANK_SHIFT), (BYTE) 2 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
        }
        fake_count++;
    } else {
        fake_addr = addr;
        fake_count = 0;
    }
}

#else

#define CAPHI   500     /* steps until disable (30 < x < 100 ?) */
#define CAPD    300

static int ar_cap = 0;

static void cap_charge(void)
{
/*    DBG(("cap+ %d (bank %d)\n", ar_cap, roml_bank)); */
    if (ar_cap == CAPHI) {
        DBG(("AR3: disabling\n"));
        cartridge_config_changed((BYTE) 2 | (1 << CMODE_BANK_SHIFT), (BYTE) 2 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
        ar_cap++;
    } else if (ar_cap < CAPHI) {
        ar_cap++;
        if (ar_cap > CAPHI) {
            ar_cap = CAPHI;
        }
    }
}

static void cap_discharge(void)
{
/*    DBG(("cap- %d\n", ar_cap)); */
    if (ar_cap == 0) {
        DBG(("AR3: enabling\n"));
        cartridge_config_changed((BYTE) 0 | (1 << CMODE_BANK_SHIFT), (BYTE) 0 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
        ar_cap--;
    } else if (ar_cap > 0) {
        ar_cap -= CAPD;
        if (ar_cap < 0) {
            ar_cap = 0;
        }
    }
}
#endif

static BYTE REGPARM1 actionreplay3_io1_read(WORD addr)
{
#ifdef USEFAKE
    dofake(0xde00);
#else
    cap_discharge();
#endif
    return 0;
}

static BYTE REGPARM1 actionreplay3_io1_peek(WORD addr)
{
    return 0;
}

static void REGPARM2 actionreplay3_io1_store(WORD addr, BYTE value)
{
    /* DBG(("cap- %d\n", ar_cap)); */
#ifdef USEFAKE
    dofake(0xde00);
#else
    cap_discharge();
#endif
}

static BYTE REGPARM1 actionreplay3_io2_read(WORD addr)
{
#ifdef USEFAKE
    dofake(0xdf00);
#else
    cap_charge();
#endif

    addr |= 0xdf00;
    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

static BYTE REGPARM1 actionreplay3_io2_peek(WORD addr)
{
    addr |= 0xdf00;
    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}
/* ---------------------------------------------------------------------*/

BYTE REGPARM1 actionreplay3_roml_read(WORD addr)
{
#ifdef USEFAKE
    dofake(addr);
#endif
    if (addr < 0x9f00) {
        return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
    } else {
        return ram_read(addr);
        /* return mem_read_without_ultimax(addr); */
    }
}

BYTE REGPARM1 actionreplay3_romh_read(WORD addr)
{
#ifdef USEFAKE
    dofake(addr);
#endif
    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}
/* ---------------------------------------------------------------------*/

void actionreplay3_freeze(void)
{
    DBG(("AR3: freeze\n"));
    ar_cap = 0;
    cartridge_config_changed(3, 3, CMODE_READ);
}

void actionreplay3_config_init(void)
{
    DBG(("AR3: config init\n"));
    ar_cap = 0;
    cartridge_config_changed(0 | (1 << CMODE_BANK_SHIFT), 0 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
}

void actionreplay3_reset(void)
{
    DBG(("AR3: reset\n"));
    ar_cap = 0;
}

void actionreplay3_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x4000);
    cartridge_config_changed(0 | (1 << CMODE_BANK_SHIFT), 0 | (1 << CMODE_BANK_SHIFT), CMODE_READ);
}

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res = {
    "Action Replay III", 1, 1, &actionreplay3_io1_device, &actionreplay3_io2_device, CARTRIDGE_ACTION_REPLAY3
};

static int actionreplay3_common_attach(void)
{
    if (c64export_add(&export_res) < 0) {
        return -1;
    }

    actionreplay3_io1_list_item = c64io_register(&actionreplay3_io1_device);
    actionreplay3_io2_list_item = c64io_register(&actionreplay3_io2_device);

    return 0;
}

int actionreplay3_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    return actionreplay3_common_attach();
}

int actionreplay3_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 1; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1) {
            return -1;
        }

        if (chipheader[0xb] > 1) {
            return -1;
        }

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1) {
            return -1;
        }
    }

    return actionreplay3_common_attach();
}

void actionreplay3_detach(void)
{
    c64export_remove(&export_res);
    c64io_unregister(actionreplay3_io1_list_item);
    c64io_unregister(actionreplay3_io2_list_item);
    actionreplay3_io1_list_item = NULL;
    actionreplay3_io2_list_item = NULL;
}
