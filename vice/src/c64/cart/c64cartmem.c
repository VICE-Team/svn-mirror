/*
 * c64cartmem.c -- C64 cartridge emulation, memory handling.
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
#include <stdlib.h>
#include <string.h>

#include "actionreplay3.h"
#include "actionreplay4.h"
#include "actionreplay.h"
#include "atomicpower.h"
#include "c64acia.h"
#include "c64cart.h"
#include "c64mem.h"
#include "c64-midi.h"
#include "c64tpi.h"
#include "c64cartmem.h"
#include "c64io.h"
#include "cartridge.h"
#include "comal80.h"
#include "capture.h"
#include "crt.h"
#include "delaep256.h"
#include "delaep64.h"
#include "delaep7x8.h"
#include "dinamic.h"
#include "dqbb.h"
#include "easyflash.h"
#include "epyxfastload.h"
#include "exos.h"
#include "expert.h"
#include "final.h"
#include "finalplus.h"
#include "final3.h"
#include "freezeframe.h"
#include "freezemachine.h"
#include "funplay.h"
#include "gamekiller.h"
#include "generic.h"
#include "georam.h"
#include "gs.h"
#include "ide64.h"
#include "isepic.h"
#include "kcs.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "magicdesk.h"
#include "magicformel.h"
#include "magicvoice.h"
#include "mikroass.h"
#include "mmc64.h"
#include "mmcreplay.h"
#include "ocean.h"
#include "prophet64.h"
#include "ramcart.h"
#include "resources.h"
#include "retroreplay.h"
#include "reu.h"
#include "rexep256.h"
#include "rexutility.h"
#include "ramcart.h"
#include "ross.h"
#include "simonsbasic.h"
#include "snapshot64.h"
#include "stardos.h"
#include "stb.h"
#include "supergames.h"
#include "superexplode5.h"
#include "supersnapshot.h"
#include "supersnapshot4.h"
#include "types.h"
#include "vicii-phi1.h"
#include "warpspeed.h"
#include "westermann.h"
#include "zaxxon.h"

/* #define DEBUGCART */

#ifdef DEBUGCART
#define DBG(x)  printf x ; fflush(stdout);
#else
#define DBG(x)
#endif

/* Expansion port signals.  */
export_t export;

/* Expansion port ROML/ROMH images.  */
BYTE roml_banks[C64CART_ROM_LIMIT], romh_banks[C64CART_ROM_LIMIT];

/* Expansion port RAM images.  */
BYTE export_ram0[C64CART_RAM_LIMIT];

/* Expansion port ROML/ROMH/RAM banking.  */
int roml_bank = 0, romh_bank = 0, export_ram = 0;

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
unsigned int cart_ultimax_phi1 = 0;
unsigned int cart_ultimax_phi2 = 0;

/* Type of the cartridge attached. ("Main Slot") */
int mem_cartridge_type = CARTRIDGE_NONE;

/*
  mode_phiN:

  bits N..2: bank (currently max 0x3f)

  bits 1,0: !exrom, game

  mode_phiN & 3 = 0 : roml
  mode_phiN & 3 = 1 : roml & romh
  mode_phiN & 3 = 2 : ram
  mode_phiN & 3 = 3 : ultimax

  wflag:

  bit 4  0x10   - trigger nmi after config changed
  bit 3  0x08   - export ram enabled
  bit 2  0x04   - vic phi2 mode (always sees ram if set)
  bit 1  0x02   - release freeze (stop asserting NMI)
  bit 0  0x01   - r/w flag
*/
void cartridge_config_changed(BYTE mode_phi1, BYTE mode_phi2, unsigned int wflag)
{
    DBG(("CARTMEM: cartridge_config_changed phi1:%d phi2:%d bank: %d flags:%02x\n",mode_phi1 & 3, mode_phi2 & 3, (mode_phi2 >> CMODE_BANK_SHIFT) & CMODE_BANK_MASK, wflag));

    if ((wflag & CMODE_WRITE) == CMODE_WRITE) {
        machine_handle_pending_alarms(maincpu_rmw_flag + 1);
    } else {
        machine_handle_pending_alarms(0);
    }

    export.game = mode_phi2 & 1;
    export.exrom = ((mode_phi2 >> 1) & 1) ^ 1;
    cartridge_romhbank_set((mode_phi2 >> CMODE_BANK_SHIFT) & CMODE_BANK_MASK);
    cartridge_romlbank_set((mode_phi2 >> CMODE_BANK_SHIFT) & CMODE_BANK_MASK);
    export_ram = (wflag >> CMODE_EXPORT_RAM_SHIFT) & 1;
    mem_pla_config_changed();
    if ((wflag & CMODE_RELEASE_FREEZE) == CMODE_RELEASE_FREEZE) {
        cartridge_release_freeze();
    }
    cart_ultimax_phi1 = (mode_phi1 & 1) & ((mode_phi1 >> 1) & 1);
    cart_ultimax_phi2 = export.game & (export.exrom ^ 1) & ((~wflag >> CMODE_PHI2_RAM_SHIFT) & 1);
    /* TODO
    cartridge_romhbank_phi1_set((mode_phi1 >> CMODE_BANK_SHIFT) & CMODE_BANK_MASK);
    cartridge_romlbank_phi1_set((mode_phi1 >> CMODE_BANK_SHIFT) & CMODE_BANK_MASK);
    */
    machine_update_memory_ptrs();

    if ((wflag & CMODE_TRIGGER_FREEZE_NMI_ONLY) == CMODE_TRIGGER_FREEZE_NMI_ONLY) {
        cartridge_trigger_freeze_nmi_only();
    }
}

/*
    generic helper routines, should be used *only* by carts that are in the
    "main cartridge" category
*/
void cartridge_romhbank_set(unsigned int bank)
{
    romh_bank = (int)bank;
}

void cartridge_romlbank_set(unsigned int bank)
{
    roml_bank = (int)bank;
}

/*
   - only CPU accesses go through the following hooks. the VICII directly accesses
     the roml_ romh_ ram0_ tables.
   - carts that switch game/exrom (ie, the memory config) based on adress, BA, phi2
     or similar can not be supported correctly with the existing system.
     the common workaround is to put the cart into ultimax mode, and wrap all hooks
     to fake the expected mapping.

    carts that use fake ultimax mapping:

        stardos      (works)
        exos         (works)
        final plus   (works)
        game killer  (works)
        capture      (mostly works, bug when exiting to basic)
        magicformel  (mostly works, extra banks bug; magic-windows)
        isepic       (mostly works, bug when saving frozen program?)
        expert       (buggy)
        mmcreplay    (generally very buggy)

    carts that use "unusual" mapping:

        ide64
        mmc64        (Allow writing at ROML at 8000-9fff)
        easyflash    (Allow writing at ROML at 8000-9FFF in Ultimax mode.)
        expert       (Allow writing at ROML at 8000-9FFF in Ultimax mode.)
        capture      (RAM at 6000-7fff in Ultimax mode.)
        ramcart

    internal extensions:

        plus60k
        plus256k
        c64_256k

    pure io extensions:

        georam
        digimax
        reu
        midi
        acia

        clockport

    the default cartridge works like this:

    1 banking register (for ROM only)
    - 8k ROM banks
    - 8k RAM may be enabled at ROML

              hook                    default

    8K Game Config:

    8000-9fff roml_read               roml_banks
              roml_no_ultimax_store   mem_store_without_romlh

    16K Game Config:

    8000-9fff roml_read               roml_banks
              roml_no_ultimax_store   mem_store_without_romlh
    a000-bfff romh_read               romh_banks
              romh_no_ultimax_store   mem_store_without_romlh

    Ultimax Config:

    1000-7fff ultimax_1000_7fff_read  vicii_read_phi1
              ultimax_1000_7fff_store n/a
    8000-9fff roml_read               roml_banks
              roml_store              n/a
    a000-bfff ultimax_a000_bfff_read  vicii_read_phi1
              ultimax_a000_bfff_store n/a
    c000-cfff ultimax_c000_cfff_read  vicii_read_phi1
              ultimax_c000_cfff_store n/a
    d000-dfff ultimax_d000_dfff_read  read_bank_io
              ultimax_d000_dfff_store store_bank_io
    e000-ffff romh_read               romh_banks
              romh_store              n/a

    ***

    as a first step to a completely generic cart system, everything should
    get reorganised based on the following assumptions:

    - it is pointless to attach/use several cartridges of the same type at
      once. by not supporting this, we can use the cartridge id as a unique
      id for a given cart, regardless of the underlaying "slot logic"
    - moreover it is also pointless to support a lot of combinations of
      cartridges, simply because they won't work anyway.

    "Slot 0"
    - carts that have a passthrough port go here
    - the passthrough of individual "Slot 0" carts must be handled on a
      per case basis.
    - if any "Slot 0" cart is active, then all following slots are assumed
      to be attached to the respective "Slot 0" passthrough port.
    - only ONE of the carts in the "Slot 0" can be active at a time

    mmc64
    Magic Voice
    ramlink, scpu, ...

    "Slot 1"
    - other ROM/RAM carts that can be enabled individually
    - any number of "Slot 1" carts can be, in theory, active at a time

    isepic
    expert
    dqbb
    ramcart

    "Main Slot"
    - the vast majority of carts go here, since only one of them
      can be used at a time anyway.
    - only ONE of the carts in the "Main Slot" can be active at a time

    this pretty much resembles the remains of the old cart system. ultimativly
    all carts should go into one of the other slots (to be completely generic),
    but it doesnt make a lot of sense to rewrite them all before passthrough-
    and mapping is handled correctly.

    "IO Slot"
    - all carts that do not use the game/exrom lines, and which do only
      map into io1/io2 go here
    - any number of "IO Slot" carts can be, in theory, active at a time

    reu
    georam
    ethernet
    acia (rs232)
    midi

    - all cards *except* those in the "Main Slot" should:
      - maintain a resource (preferably XYZCartridgeEnabled) that tells
        wether said cart is "inserted" into our virtual "expansion port expander".
      - maintain their own arrays to store rom/ram content.
      - as a consequence, changing said resource equals attaching/detaching the
        cartridge.
*/

/* ROML read - mapped to 8000 in 8k,16k,ultimax */
BYTE REGPARM1 roml_read(WORD addr)
{
    /* "Slot 0" */
    if (mmc64_cart_enabled()) {
        return mmc64_roml_read(addr);
    }
    if (magicvoice_cart_enabled()) {
        return magicvoice_roml_read(addr);
    }
    /* "Slot 1" */
    if (isepic_cart_enabled()) {
        return isepic_page_read(addr);
    }
    if (expert_cart_enabled()) {
        return expert_roml_read(addr);
    }
    if (ramcart_cart_enabled()) {
        return ramcart_roml_read(addr);
    }
    if (dqbb_cart_enabled()) {
        return dqbb_roml_read(addr);
    }
    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_FREEZE_FRAME:
            /* use default cartridge */
            break;
        case CARTRIDGE_FREEZE_MACHINE:
            return freezemachine_roml_read(addr);
        case CARTRIDGE_STARDOS:
            return stardos_roml_read(addr);
        case CARTRIDGE_ZAXXON:
            return zaxxon_roml_read(addr);
        case CARTRIDGE_SUPER_SNAPSHOT:
            return supersnapshot_v4_roml_read(addr);
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            return supersnapshot_v5_roml_read(addr);
        case CARTRIDGE_ACTION_REPLAY4:
            return actionreplay4_roml_read(addr);
        case CARTRIDGE_ACTION_REPLAY:
            return actionreplay_roml_read(addr);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_roml_read(addr);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_roml_read(addr);
        case CARTRIDGE_IDE64:
            return ide64_roml_read(addr);
        case CARTRIDGE_ATOMIC_POWER:
            return atomicpower_roml_read(addr);
        case CARTRIDGE_FINAL_I:
            return final_v1_roml_read(addr);
        case CARTRIDGE_FINAL_PLUS:
            return final_plus_roml_read(addr);
        case CARTRIDGE_FINAL_III:
            return final_v3_roml_read(addr);
        case CARTRIDGE_MAGIC_FORMEL:
            return magicformel_roml_read(addr);
        case CARTRIDGE_EASYFLASH:
            return easyflash_roml_read(addr);
        case CARTRIDGE_EPYX_FASTLOAD:
            return epyxfastload_roml_read(addr);
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_EXOS:
            /* fake ultimax hack */
            return mem_read_without_ultimax(addr);
    }

    /* default cartridge */
    if (export_ram) {
        return export_ram0[addr & 0x1fff];
    }

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

/* ROML store - mapped to 8000 in ultimax mode */
void REGPARM2 roml_store(WORD addr, BYTE value)
{
    /* DBG(("ultimax w 8000: %04x %02x\n", addr, value)); */

    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        /* fake ultimax hack */
        mem_store_without_ultimax(addr, value);
        return;
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        expert_roml_store(addr,value);
        return;
    }
    if (ramcart_cart_enabled()) {
        ramcart_roml_store(addr,value);
        return;
    }
    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_SUPER_SNAPSHOT:
            supersnapshot_v4_roml_store(addr, value);
            return;
        case CARTRIDGE_SUPER_SNAPSHOT_V5:
            supersnapshot_v5_roml_store(addr, value);
            return;
        case CARTRIDGE_ACTION_REPLAY:
            actionreplay_roml_store(addr, value);
            return;
        case CARTRIDGE_RETRO_REPLAY:
            retroreplay_roml_store(addr, value);
            return;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_roml_store(addr, value);
            return;
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_roml_store(addr, value);
            return;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_roml_store(addr, value);
            return;
        case CARTRIDGE_EASYFLASH:
            easyflash_roml_store(addr, value);
            return;
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack */
            mem_store_without_ultimax(addr, value);
            return;
    }

    /* default cartridge */
    if (export_ram) {
        export_ram0[addr & 0x1fff] = value;
    }
}

/* ROMH read - mapped to A000 in 16k, to E000 in ultimax */
BYTE REGPARM1 romh_read(WORD addr)
{
    /* DBG(("ultimax r e000: %04x\n", addr)); */

    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        return magicvoice_romh_read(addr);
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        return expert_romh_read(addr);
    }
    if (dqbb_cart_enabled()) {
        return dqbb_romh_read(addr);
    }
    if (isepic_cart_enabled()) {
        return isepic_romh_read(addr);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_FREEZE_FRAME:
        case CARTRIDGE_FREEZE_MACHINE:
            /* use default cartridge */
            break;
        case CARTRIDGE_ATOMIC_POWER:
            return atomicpower_romh_read(addr);
        case CARTRIDGE_OCEAN:
            return ocean_romh_read(addr);
        case CARTRIDGE_IDE64:
            return ide64_romh_read(addr);
        case CARTRIDGE_EASYFLASH:
            return easyflash_romh_read(addr);
        case CARTRIDGE_CAPTURE:
            return capture_romh_read(addr);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_romh_read(addr);
        case CARTRIDGE_MAGIC_FORMEL:
            return magicformel_romh_read(addr);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_romh_read(addr);
        case CARTRIDGE_FINAL_PLUS:
            return final_plus_romh_read(addr);
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, read from ram */
            return ram_read(addr);
            /* return mem_read_without_ultimax(addr); */
    }

    /* default cartridge */
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

/* ROMH read if hirom is selected - mapped to E000 in ultimax

   most carts that use romh_read also need to use this one. carts
   that map an "external kernal" _only_ use this one, and wrap to
   ram_read in romh_read.
*/
BYTE REGPARM1 ultimax_romh_read_hirom(WORD addr)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        return magicvoice_romh_read(addr);
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        return expert_romh_read(addr);
    }
    if (dqbb_cart_enabled()) {
        return dqbb_romh_read(addr);
    }
    if (isepic_cart_enabled()) {
        return isepic_romh_read(addr);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_FREEZE_FRAME:
        case CARTRIDGE_FREEZE_MACHINE:
            /* use default cartridge */
            break;
        case CARTRIDGE_ATOMIC_POWER:
            return atomicpower_romh_read(addr);
        case CARTRIDGE_OCEAN:
            return ocean_romh_read(addr);
        case CARTRIDGE_IDE64:
            return ide64_romh_read(addr);
        case CARTRIDGE_EASYFLASH:
            return easyflash_romh_read(addr);
        case CARTRIDGE_CAPTURE:
            return capture_romh_read(addr);
        case CARTRIDGE_RETRO_REPLAY:
            return retroreplay_romh_read(addr);
        case CARTRIDGE_MAGIC_FORMEL:
            return magicformel_romh_read(addr);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_romh_read(addr);
        case CARTRIDGE_FINAL_PLUS:
            return final_plus_romh_read(addr);
        case CARTRIDGE_EXOS:
            return exos_romh_read(addr);
        case CARTRIDGE_STARDOS:
            return stardos_romh_read(addr);
    }

    /* default cartridge */
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

/* ROMH store - mapped to E000 in ultimax mode */
void REGPARM2 romh_store(WORD addr, BYTE value)
{
    /* DBG(("ultimax w e000: %04x %02x\n", addr, value)); */

    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        /* fake ultimax hack, c64 ram */
        mem_store_without_ultimax(addr, value);
    }
    /* "Slot 1" */
    if (isepic_cart_enabled()) {
        isepic_romh_store(addr, value);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_EASYFLASH:
            easyflash_romh_store(addr, value);
            break;
        case CARTRIDGE_MAGIC_FORMEL:
            magicformel_romh_store(addr, value);
            break;
        case CARTRIDGE_CAPTURE:
            capture_romh_store(addr, value);
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_romh_store(addr, value);
            break;
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 ram */
            mem_store_without_ultimax(addr, value);
            return;
    }
}

/* ROMH store - A000-BFFF in 16kGame

   normally writes to ROM area would go to RAM an not generate
   a write select. some carts however map RAM here and also
   accept writes in this mode.
*/
void REGPARM2 romh_no_ultimax_store(WORD addr, BYTE value)
{
    /* DBG(("game    w a000: %04x %02x\n", addr, value)); */

    /* "Slot 0" */
    /* "Slot 1" */
    if (dqbb_cart_enabled()) {
        dqbb_romh_store(addr, value);
        return;
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_ATOMIC_POWER:
            atomicpower_romh_store(addr, value);
            break;
    }
    /* store to c64 ram */
    mem_store_without_romlh(addr, value);
}

/* ROML store - mapped to 8000-9fff in 8kGame, 16kGame

   normally writes to ROM area would go to RAM an not generate
   a write select. some carts however map ram here and also
   accept writes in this mode.
*/
void REGPARM2 roml_no_ultimax_store(WORD addr, BYTE value)
{
    /* DBG(("game rom    w 8000: %04x %02x\n", addr, value)); */
    /* "Slot 0" */
    if (mmc64_cart_enabled()) {
        mmc64_roml_store(addr, value);
        return; /* ? */
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        expert_roml_store(addr, value);
        return; /* ? */
    }
    if (dqbb_cart_enabled()) {
        dqbb_roml_store(addr, value);
        return; /* ? */
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_roml_no_ultimax_store(addr, value))
            {
                return; /* FIXME: this is weird */
            }
            break;
    }

    /* store to c64 ram */
    mem_store_without_romlh(addr, value);
}

/* RAML store (ROML _NOT_ selected) - mapped to 8000-9fff in 8kGame, 16kGame

   WARNING:
      mem_store_without_ultimax(addr, value)
      must NOT be called by any functions called here, as this will cause an
      endless loop
*/
void REGPARM2 raml_no_ultimax_store(WORD addr, BYTE value)
{
    /* DBG(("game ram    w 8000: %04x %02x\n", addr, value)); */
    /* "Slot 0" */
    if (mmc64_cart_enabled()) {
        mmc64_roml_store(addr, value);
        /* return; */
    }

    /* "Slot 1" */
    if (expert_cart_enabled()) {
        expert_roml_store(addr, value);
        /* return; */
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_RETRO_REPLAY:
            if (retroreplay_roml_no_ultimax_store(addr, value))
            {
                return; /* FIXME: this is weird */
            }
            break;
    }

    /* store to c64 ram */
    ram_store(addr, value);
    /* mem_store_without_romlh(addr, value); */
}

/* ultimax read - 1000 to 7fff */
BYTE REGPARM1 ultimax_1000_7fff_read(WORD addr)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        return mem_read_without_ultimax(addr); /* fake ultimax hack, c64 ram */
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        return mem_read_without_ultimax(addr); /* fake ultimax hack, c64 ram */
    }
    if (isepic_cart_enabled()) {
        /* return mem_read_without_ultimax(addr); */ /* fake ultimax hack */
        return isepic_page_read(addr);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_IDE64:
            return ide64_1000_7fff_read(addr);
        case CARTRIDGE_CAPTURE:
            return capture_1000_7fff_read(addr);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_1000_7fff_read(addr);
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 ram */
            return mem_read_without_ultimax(addr);
    }

    /* default; no cart, open bus */
    return vicii_read_phi1();
}

/* ultimax store - 1000 to 7fff */
void REGPARM2 ultimax_1000_7fff_store(WORD addr, BYTE value)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack, c64 ram */
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack, c64 ram */
    }
    if (isepic_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack */
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_IDE64:
            ide64_1000_7fff_store(addr, value);
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_1000_7fff_store(addr, value);
            break;
        case CARTRIDGE_CAPTURE:
            capture_1000_7fff_store(addr, value);
            break;
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 ram */
            mem_store_without_ultimax(addr, value);
            break;
    }

    /* default; no cart, open bus */
}

/* ultimax read - a000 to bfff */
BYTE REGPARM1 ultimax_a000_bfff_read(WORD addr)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        return magicvoice_a000_bfff_read(addr);
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        return mem_read_without_ultimax(addr); /* fake ultimax hack, c64 basic, ram */
    }
    if (isepic_cart_enabled()) {
        /* return mem_read_without_ultimax(addr); */ /* fake ultimax hack */
        return isepic_page_read(addr);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_IDE64:
            return ide64_a000_bfff_read(addr);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_a000_bfff_read(addr);
        case CARTRIDGE_FINAL_PLUS:
            return final_plus_a000_bfff_read(addr);
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 basic, ram */
            return mem_read_without_ultimax(addr);
    }
    /* default; no cart, open bus */
    return vicii_read_phi1();
}

/* ultimax store - a000 to bfff */
void REGPARM2 ultimax_a000_bfff_store(WORD addr, BYTE value)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        /* fake ultimax hack, c64 ram */
        mem_store_without_ultimax(addr, value);
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack, c64 ram */
    }
    if (isepic_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack */
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_a000_bfff_store(addr, value);
            break;
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 ram */
            mem_store_without_ultimax(addr, value);
            break;
    }

    /* default; no cart, open bus */
}

/* ultimax read - c000 to cfff */
BYTE REGPARM1 ultimax_c000_cfff_read(WORD addr)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        return mem_read_without_ultimax(addr); /* fake ultimax hack, c64 ram */
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        return mem_read_without_ultimax(addr); /* fake ultimax hack, c64 ram */
    }
    if (isepic_cart_enabled()) {
        /* return mem_read_without_ultimax(addr); */ /* fake ultimax hack */
        return isepic_page_read(addr);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_IDE64:
            return ide64_c000_cfff_read(addr);
        case CARTRIDGE_MMC_REPLAY:
            return mmcreplay_c000_cfff_read(addr);
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 ram */
            return mem_read_without_ultimax(addr);
    }
    /* default; no cart, open bus */
    return vicii_read_phi1();
}

/* ultimax store - c000 to cfff */
void REGPARM2 ultimax_c000_cfff_store(WORD addr, BYTE value)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack, c64 ram */
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack, c64 ram */
    }
    if (isepic_cart_enabled()) {
        mem_store_without_ultimax(addr, value); /* fake ultimax hack */
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_IDE64:
            ide64_c000_cfff_store(addr, value);
            break;
        case CARTRIDGE_MMC_REPLAY:
            mmcreplay_c000_cfff_store(addr, value);
            break;
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_GAME_KILLER:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 ram */
            mem_store_without_ultimax(addr, value);
            break;
    }

    /* default; no cart, open bus */
}

/* ultimax read - d000 to dfff */
BYTE REGPARM1 ultimax_d000_dfff_read(WORD addr)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        /* fake ultimax hack, c64 io,colram,ram */
        return mem_read_without_ultimax(addr);
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        /* fake ultimax hack, c64 io,colram,ram */
        return mem_read_without_ultimax(addr);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 io,colram,ram */
            return mem_read_without_ultimax(addr);
    }
    /* default; no cart, c64 i/o */
    return read_bank_io(addr);
}

/* ultimax store - d000 to dfff */
void REGPARM2 ultimax_d000_dfff_store(WORD addr, BYTE value)
{
    /* "Slot 0" */
    if (magicvoice_cart_enabled()) {
        /* fake ultimax hack, c64 io,colram,ram */
        mem_store_without_ultimax(addr, value);
        return;
    }
    /* "Slot 1" */
    if (expert_cart_enabled()) {
        /* fake ultimax hack, c64 io,colram,ram */
        mem_store_without_ultimax(addr, value);
    }

    /* "Main Slot" */
    switch (mem_cartridge_type) {
        case CARTRIDGE_MAGIC_FORMEL:
        case CARTRIDGE_CAPTURE:
        case CARTRIDGE_FINAL_PLUS:
        case CARTRIDGE_EXOS:
        case CARTRIDGE_STARDOS:
            /* fake ultimax hack, c64 io,colram,ram */
            mem_store_without_ultimax(addr, value);
            return;
    }
    /* default;no cart, c64 i/o */
    store_bank_io(addr, value);
}

