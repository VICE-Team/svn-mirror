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

#include "actionreplay.h"
#include "atomicpower.h"
#include "c64cart.h"
#include "c64mem.h"
#include "c64tpi.h"
#include "c64cartmem.h"
#include "c64io.h"
#include "cartridge.h"
#include "comal80.h"
#include "crt.h"
#include "delaep256.h"
#include "delaep64.h"
#include "delaep7x8.h"
#include "epyxfastload.h"
#include "expert.h"
#include "final.h"
#include "generic.h"
#include "kcs.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "magicformel.h"
#include "mikroass.h"
#include "resources.h"
#include "retroreplay.h"
#include "rexep256.h"
#include "ide64.h"
#include "ramcart.h"
#include "ross.h"
#include "stb.h"
#include "supergames.h"
#include "supersnapshot.h"
#include "types.h"
#include "vicii-phi1.h"
#include "zaxxon.h"


/* #define DEBUG */

/* Expansion port signals.  */
export_t export;

/* Expansion port ROML/ROMH images.  */
BYTE roml_banks[0x80000], romh_banks[0x20000];

/* Expansion port RAM images.  */
BYTE export_ram0[C64CART_RAM_LIMIT];

/* Expansion port ROML/ROMH/RAM banking.  */
int roml_bank = 0, romh_bank = 0, export_ram = 0;

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
unsigned int cart_ultimax_phi1 = 0;
unsigned int cart_ultimax_phi2 = 0;

/* Type of the cartridge attached.  */
int mem_cartridge_type = CARTRIDGE_NONE;


void cartridge_config_changed(BYTE mode_phi1, BYTE mode_phi2,
                              unsigned int wflag)
{
    if (wflag == CMODE_WRITE)
        machine_handle_pending_alarms(maincpu_rmw_flag + 1);
    else
        machine_handle_pending_alarms(0);

    export.game = mode_phi2 & 1;
    export.exrom = ((mode_phi2 >> 1) & 1) ^ 1;
    cartridge_romhbank_set((mode_phi2 >> 3) & 3);
    cartridge_romlbank_set((mode_phi2 >> 3) & 3);
    export_ram = (mode_phi2 >> 5) & 1;
    mem_pla_config_changed();
    if (mode_phi2 & 0x40)
        cartridge_release_freeze();
    cart_ultimax_phi1 = (mode_phi1 & 1) & ((mode_phi1 >> 1) & 1);
    cart_ultimax_phi2 = export.game & (export.exrom ^ 1) & ((~mode_phi1 >> 2) & 1);
    machine_update_memory_ptrs();
}

BYTE REGPARM1 cartridge_read_io1(WORD addr)
{
#ifdef DEBUG
    log_debug("Read IO1 %02x.", addr);
#endif
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        return actionreplay_io1_read(addr);
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_io1_read(addr);
      case CARTRIDGE_RETRO_REPLAY:
        return retroreplay_io1_read(addr);
      case CARTRIDGE_IDE64:
        return ide64_io1_read(addr);
      case CARTRIDGE_KCS_POWER:
        return kcs_io1_read(addr);
      case CARTRIDGE_FINAL_III:
        return final_v3_io1_read(addr);
      case CARTRIDGE_FINAL_I:
        return final_v1_io1_read(addr);
      case CARTRIDGE_MIKRO_ASSEMBLER:
        return mikroass_io1_read(addr);
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GS:
        cartridge_config_changed(0, 0, CMODE_READ);
        return vicii_read_phi1();
      case CARTRIDGE_WARPSPEED:
        io_source=IO_SOURCE_WARPSPEED;
        return roml_banks[0x1e00 + (addr & 0xff)];
      case CARTRIDGE_DINAMIC:
        cartridge_romlbank_set(addr & 0x0f);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        return supersnapshot_v4_io1_read(addr);
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        return supersnapshot_v5_io1_read(addr);
      case CARTRIDGE_EXPERT:
        return expert_io1_read(addr);
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_io1_read(addr);
      case CARTRIDGE_ROSS:
        return ross_io1_read(addr);
      case CARTRIDGE_STRUCTURED_BASIC:
        return stb_io1_read(addr);
      case CARTRIDGE_DELA_EP64:
        return delaep64_io1_read(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 cartridge_store_io1(WORD addr, BYTE value)
{
#ifdef DEBUG
    log_debug("Store IO1 %02x <- %02x.", addr, value);
#endif

    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        actionreplay_io1_store(addr, value);
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_io1_store(addr, value);
        break;
      case CARTRIDGE_IDE64:
        ide64_io1_store(addr, value);
        break;
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_io1_store(addr, value);
        break;
      case CARTRIDGE_KCS_POWER:
        kcs_io1_store(addr, value);
        break;
      case CARTRIDGE_FINAL_I:
        final_v1_io1_store(addr, value);
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_io1_store(addr, value);
        break;
      case CARTRIDGE_COMAL80:
        comal80_io1_store(addr, value);
        break;
      case CARTRIDGE_SIMONS_BASIC:
        cartridge_config_changed(1, 1, CMODE_WRITE);
        break;
      case CARTRIDGE_WARPSPEED:
        cartridge_config_changed(1, 1, CMODE_WRITE);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_io1_store(addr, value);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_io1_store(addr, value);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        switch (mem_cartridge_type) {
          case CARTRIDGE_OCEAN:
            cartridge_romhbank_set(value & 0x3f);
            cartridge_romlbank_set(value & 0x3f);
            break;
          case CARTRIDGE_FUNPLAY:
            cartridge_romhbank_set(((value >> 2) | (value & 1)) & 15);
            cartridge_romlbank_set(((value >> 2) | (value & 1)) & 15);
            break;
        }
        export.game = export.exrom = 1;
        mem_pla_config_changed();
        cart_ultimax_phi1 = 0;
        cart_ultimax_phi2 = 0;
        break;
      case CARTRIDGE_GS:
        cartridge_romlbank_set(addr & 0x3f);
        export.game = 0;
        export.exrom = 1;
        break;
      case CARTRIDGE_EXPERT:
        expert_io1_store(addr, value);
        break;
      case CARTRIDGE_MAGIC_DESK:
        cartridge_romlbank_set(value & 0x3f);
        export.game = 0;
        if (value & 0x80)
            export.exrom = 0;
        else
            export.exrom = 1;  /* turn off cart ROM */
        mem_pla_config_changed();
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_io1_store(addr, value);
        break;
      case CARTRIDGE_STRUCTURED_BASIC:
        stb_io1_store(addr, value);
        break;
      case CARTRIDGE_DELA_EP64:
        delaep64_io1_store(addr, value);
        break;
      case CARTRIDGE_DELA_EP7x8:
        delaep7x8_io1_store(addr, value);
        break;
      case CARTRIDGE_DELA_EP256:
        delaep256_io1_store(addr, value);
        break;
    }
    return;
}

BYTE REGPARM1 cartridge_read_io2(WORD addr)
{
#ifdef DEBUG
    log_debug("Read IO2 %02x.", addr);
#endif
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        return atomicpower_io2_read(addr);
      case CARTRIDGE_ATOMIC_POWER:
        return actionreplay_io2_read(addr);
      case CARTRIDGE_RETRO_REPLAY:
        return retroreplay_io2_read(addr);
      case CARTRIDGE_SUPER_SNAPSHOT:
        return supersnapshot_v4_io2_read(addr);
      case CARTRIDGE_FINAL_III:
        return final_v3_io2_read(addr);
      case CARTRIDGE_FINAL_I:
        return final_v1_io2_read(addr);
      case CARTRIDGE_KCS_POWER:
        return kcs_io2_read(addr);
      case CARTRIDGE_IEEE488:
        return tpi_read((WORD)(addr & 0x07));
      case CARTRIDGE_EPYX_FASTLOAD:
        return epyxfastload_io2_read(addr);
      case CARTRIDGE_MIKRO_ASSEMBLER:
        return mikroass_io2_read(addr);
      case CARTRIDGE_WESTERMANN:
        cartridge_config_changed(0, 0, CMODE_READ);
        return vicii_read_phi1();
      case CARTRIDGE_REX:
        if ((addr & 0xff) < 0xc0)
            cartridge_config_changed(2, 2, CMODE_READ);
        else
            cartridge_config_changed(0, 0, CMODE_READ);
        return 0;
      case CARTRIDGE_WARPSPEED:
        io_source=IO_SOURCE_WARPSPEED;
        return roml_banks[0x1f00 + (addr & 0xff)];
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_io2_read(addr);
      case CARTRIDGE_ROSS:
        return ross_io2_read(addr);
      case CARTRIDGE_REX_EP256:
        return rexep256_io2_read(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 cartridge_store_io2(WORD addr, BYTE value)
{
#ifdef DEBUG
    log_debug("Store IO2 %02x <- %02x.", addr, value);
#endif
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        atomicpower_io2_store(addr, value);
        break;
      case CARTRIDGE_ATOMIC_POWER:
        actionreplay_io2_store(addr, value);
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_io2_store(addr, value);
        break;
      case CARTRIDGE_FINAL_I:
        final_v1_io2_store(addr, value);
        break;
      case CARTRIDGE_KCS_POWER:
        kcs_io2_store(addr, value);
        break;
      case CARTRIDGE_WARPSPEED:
        cartridge_config_changed(2, 2, CMODE_WRITE);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_io2_store(addr, value);
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_io2_store(addr, value);
        break;
      case CARTRIDGE_SUPER_GAMES:
        supergames_io2_store(addr, value);
        break;
      case CARTRIDGE_IEEE488:
        tpi_store((WORD)(addr & 0x07), value);
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_io2_store(addr, value);
        break;
      case CARTRIDGE_REX_EP256:
        rexep256_io2_store(addr, value);
        break;
    }
}

BYTE REGPARM1 roml_read(WORD addr)
{
    if (ramcart_enabled)
    {
      return ramcart_roml_read(addr);
    }
    switch (mem_cartridge_type) {
      case CARTRIDGE_ZAXXON:
        return zaxxon_roml_read(addr);
      case CARTRIDGE_SUPER_SNAPSHOT:
        return supersnapshot_v4_roml_read(addr);
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        return supersnapshot_v5_roml_read(addr);
      case CARTRIDGE_ACTION_REPLAY:
        return actionreplay_roml_read(addr);
      case CARTRIDGE_RETRO_REPLAY:
        return retroreplay_roml_read(addr);
      case CARTRIDGE_IDE64:
        return roml_banks[(addr & 0x3fff) | (roml_bank << 14)];
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_roml_read(addr);
      case CARTRIDGE_EXPERT:
        return expert_roml_read(addr);
      case CARTRIDGE_FINAL_I:
        return final_v1_roml_read(addr);
      case CARTRIDGE_FINAL_III:
        return final_v3_roml_read(addr);
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_roml_read(addr);
    }

    if (export_ram)
        return export_ram0[addr & 0x1fff];

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 roml_store(WORD addr, BYTE value)
{
    if (ramcart_enabled)
    {
      ramcart_roml_store(addr,value);
      return;
    }
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
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_roml_store(addr, value);
        return;
      case CARTRIDGE_EXPERT:
        expert_roml_store(addr, value);
        return;
      case CARTRIDGE_FINAL_I:
        final_v1_roml_store(addr, value);
        return;
      case CARTRIDGE_FINAL_III:
        final_v3_roml_store(addr, value);
        return;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_roml_store(addr, value);
        return;
    }

    if (export_ram)
        export_ram0[addr & 0x1fff] = value;
}

BYTE REGPARM1 romh_read(WORD addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_romh_read(addr);
      case CARTRIDGE_EXPERT:
        return expert_romh_read(addr);
      case CARTRIDGE_OCEAN:
        /* 256 kB OCEAN carts may access memory either at $8000 or $a000 */
        return roml_banks[(addr & 0x1fff) + (romh_bank << 13)];
      case CARTRIDGE_IDE64:
        return romh_banks[(addr & 0x3fff) | (romh_bank << 14)];
    }
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

void REGPARM2 romh_store(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_romh_store(addr, value);
        return;
    }
}

BYTE REGPARM1 ultimax_1000_7fff_read(WORD addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_IDE64:
        return export_ram0[addr & 0x7fff];
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_1000_7fff_read(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 ultimax_1000_7fff_store(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_IDE64:
        export_ram0[addr & 0x7fff] = value;
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_1000_7fff_store(addr, value);
        break;
    }
}

BYTE REGPARM1 ultimax_a000_bfff_read(WORD addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_a000_bfff_read(addr);
      case CARTRIDGE_IDE64:
        return romh_banks[(addr & 0x3fff) | (romh_bank << 14)];
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_a000_bfff_read(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 ultimax_a000_bfff_store(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_a000_bfff_store(addr, value);
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_a000_bfff_store(addr, value);
        break;
    }
}

BYTE REGPARM1 ultimax_c000_cfff_read(WORD addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_IDE64:
        return export_ram0[addr & 0x7fff];
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_c000_cfff_read(addr);
    }
    return vicii_read_phi1();
}

void REGPARM2 ultimax_c000_cfff_store(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_IDE64:
        export_ram0[addr & 0x7fff] = value;
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_c000_cfff_store(addr, value);
        break;
    }
}

BYTE REGPARM1 ultimax_d000_dfff_read(WORD addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_MAGIC_FORMEL:
        return magicformel_d000_dfff_read(addr);
    }
    return read_bank_io(addr);
}

void REGPARM2 ultimax_d000_dfff_store(WORD addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_d000_dfff_store(addr, value);
        break;
    }
    store_bank_io(addr, value);
}

void REGPARM1 cartridge_decode_address(WORD addr)
{
    switch (mem_cartridge_type) {
    }
}

void cartridge_init_config(void)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        actionreplay_config_init();
        break;
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_config_init();
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_config_init();
        break;
      case CARTRIDGE_IDE64:
        ide64_config_init();
        break;
      case CARTRIDGE_KCS_POWER:
        kcs_config_init();
        break;
      case CARTRIDGE_SUPER_GAMES:
        supergames_config_init();
        break;
      case CARTRIDGE_COMAL80:
        comal80_config_init();
        break;
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_REX:
        generic_8kb_config_init();
        break;
      case CARTRIDGE_EPYX_FASTLOAD:
        epyxfastload_config_init();
        break;
      case CARTRIDGE_MIKRO_ASSEMBLER:
        mikroass_config_init();
        break;
      case CARTRIDGE_FINAL_I:
        final_v1_config_init();
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_config_init();
        break;
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GENERIC_16KB:
      case CARTRIDGE_WESTERMANN:
      case CARTRIDGE_WARPSPEED:
        generic_16kb_config_init();
        break;
      case CARTRIDGE_ZAXXON:
        zaxxon_config_init();
        break;
      case CARTRIDGE_ULTIMAX:
        generic_ultimax_config_init();
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_config_init();
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_config_init();
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        cartridge_config_changed(1, 1, CMODE_READ);
        cartridge_store_io1((WORD)0xde00, 0);
        break;
      case CARTRIDGE_GS:
        cartridge_config_changed(0, 0, CMODE_READ);
        cartridge_store_io1((WORD)0xde00, 0);
        break;
      case CARTRIDGE_DINAMIC:
        cartridge_config_changed(0, 0, CMODE_READ);
        cartridge_read_io1((WORD)0xde00);
        break;
      case CARTRIDGE_IEEE488:
        cartridge_config_changed(0, 0, CMODE_READ);
        /* FIXME: Insert interface init here.  */
        break;
      case CARTRIDGE_EXPERT:
        expert_config_init();
        break;
      case CARTRIDGE_MAGIC_DESK:
        cartridge_config_changed(0, 0, CMODE_READ);
        cartridge_store_io1((WORD)0xde00, 0);
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_config_init();
        break;
      case CARTRIDGE_ROSS:
        ross_config_init();
        break;
      case CARTRIDGE_STRUCTURED_BASIC:
        stb_config_init();
        break;
      case CARTRIDGE_DELA_EP64:
        delaep64_config_init();
        break;
      case CARTRIDGE_DELA_EP7x8:
        delaep7x8_config_init();
        break;
      case CARTRIDGE_DELA_EP256:
        delaep256_config_init();
        break;
      case CARTRIDGE_REX_EP256:
        rexep256_config_init();
        break;
      default:
        cartridge_config_changed(2, 2, CMODE_READ);
    }
}

void cartridge_reset(void)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        actionreplay_reset();
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_reset();
        break;
    }
}

void cartridge_attach(int type, BYTE *rawcart)
{
    int cartridge_reset;

    mem_cartridge_type = type;
    cartridge_romhbank_set(0);
    cartridge_romlbank_set(0);
    switch (type) {
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_IEEE488:
      case CARTRIDGE_REX:
        generic_8kb_config_setup(rawcart);
        break;
      case CARTRIDGE_EPYX_FASTLOAD:
        epyxfastload_config_setup(rawcart);
        break;
      case CARTRIDGE_MIKRO_ASSEMBLER:
        mikroass_config_setup(rawcart);
        break;
      case CARTRIDGE_GENERIC_16KB:
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_WESTERMANN:
      case CARTRIDGE_WARPSPEED:
        generic_16kb_config_setup(rawcart);
        break;
      case CARTRIDGE_FINAL_I:
        final_v1_config_setup(rawcart);
        break;
      case CARTRIDGE_ACTION_REPLAY:
        actionreplay_config_setup(rawcart);
        break;
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_config_setup(rawcart);
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_config_setup(rawcart);
        break;
      case CARTRIDGE_IDE64:
        ide64_config_setup(rawcart);
        break;
      case CARTRIDGE_KCS_POWER:
        kcs_config_setup(rawcart);
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_config_setup(rawcart);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_config_setup(rawcart);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_config_setup(rawcart);
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
      case CARTRIDGE_GS:
      case CARTRIDGE_DINAMIC:
        memcpy(roml_banks, rawcart, 0x2000 * 64);
        memcpy(romh_banks, &rawcart[0x2000 * 16], 0x2000 * 16);
        /* Hack: using 16kB configuration, but some carts are 8kB only */
        cartridge_config_changed(1, 1, CMODE_READ);
        break;
      case CARTRIDGE_ULTIMAX:
        generic_ultimax_config_setup(rawcart);
        break;
      case CARTRIDGE_SUPER_GAMES:
        supergames_config_setup(rawcart);
        break;
      case CARTRIDGE_COMAL80:
        comal80_config_setup(rawcart);
        break;
      case CARTRIDGE_EXPERT:
        expert_config_setup(rawcart);
        break;
      case CARTRIDGE_ZAXXON:
        zaxxon_config_setup(rawcart);
        break;
      case CARTRIDGE_MAGIC_DESK:
        memcpy(roml_banks, rawcart, 0x2000 * 64);
        cartridge_config_changed(0, 0, CMODE_READ);
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_config_setup(rawcart);
        break;
      case CARTRIDGE_ROSS:
        ross_config_setup(rawcart);
        break;
      case CARTRIDGE_STRUCTURED_BASIC:
        stb_config_setup(rawcart);
        break;
      case CARTRIDGE_DELA_EP64:
        delaep64_config_setup(rawcart);
        break;
      case CARTRIDGE_DELA_EP7x8:
        delaep7x8_config_setup(rawcart);
        break;
      case CARTRIDGE_DELA_EP256:
        delaep256_config_setup(rawcart);
        break;
      case CARTRIDGE_REX_EP256:
        rexep256_config_setup(rawcart);
        break;
      default:
        mem_cartridge_type = CARTRIDGE_NONE;
    }
    
    resources_get_value("CartridgeReset", (void *)&cartridge_reset);

    if (cartridge_reset != 0) {
        /* "Turn off machine before inserting cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }

    return;
}

void cartridge_detach(int type)
{
    int cartridge_reset;

    switch (type) {
      case CARTRIDGE_ACTION_REPLAY:
        actionreplay_detach();
        break;
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_detach();
        break;
      case CARTRIDGE_EPYX_FASTLOAD:
        epyxfastload_detach();
        break;
      case CARTRIDGE_MIKRO_ASSEMBLER:
        mikroass_detach();
        break;
      case CARTRIDGE_REX:
        rex_detach();
        break;
      case CARTRIDGE_EXPERT:
        expert_detach();
        break;
      case CARTRIDGE_FINAL_I:
        final_v1_detach();
        break;
      case CARTRIDGE_WESTERMANN:
        westermann_detach();
        break;
      case CARTRIDGE_WARPSPEED:
        warpspeed_detach();
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_detach();
        break;
      case CARTRIDGE_GENERIC_16KB:
        generic_16kb_detach();
        break;
      case CARTRIDGE_GENERIC_8KB:
        generic_8kb_detach();
        break;
      case CARTRIDGE_IDE64:
        ide64_detach();
        break;
      case CARTRIDGE_IEEE488:
        /* FIXME: Insert interface removal here.  */
        break;
      case CARTRIDGE_KCS_POWER:
        kcs_detach();
        break;
      case CARTRIDGE_SIMONS_BASIC:
        simon_detach();
        break;
      case CARTRIDGE_MAGIC_FORMEL:
        magicformel_detach();
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_detach();
        break;
      case CARTRIDGE_SUPER_GAMES:
        supergames_detach();
        break;
      case CARTRIDGE_COMAL80:
        comal80_detach();
        break;
      case CARTRIDGE_STRUCTURED_BASIC:
        stb_detach();
        break;
      case CARTRIDGE_ROSS:
        ross_detach();
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_detach();
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_detach();
        break;
      case CARTRIDGE_ULTIMAX:
        generic_ultimax_detach();
        break;
      case CARTRIDGE_ZAXXON:
        zaxxon_detach();
        break;
      case CARTRIDGE_DELA_EP64:
        delaep64_detach();
        break;
      case CARTRIDGE_DELA_EP7x8:
        delaep7x8_detach();
        break;
      case CARTRIDGE_DELA_EP256:
        delaep256_detach();
        break;
      case CARTRIDGE_REX_EP256:
        rexep256_detach();
        break;
    }
    cartridge_config_changed(6, 6, CMODE_READ);
    mem_cartridge_type = CARTRIDGE_NONE;

    resources_get_value("CartridgeReset", (void *)&cartridge_reset);

    if (cartridge_reset != 0) {
        /* "Turn off machine before removing cartridge" */
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }

    return;
}

void cartridge_freeze(int type)
{
    switch (type) {
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_freeze();
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_freeze();
        break;
      case CARTRIDGE_ACTION_REPLAY:
        actionreplay_freeze();
        break;
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_freeze();
        break;
      case CARTRIDGE_RETRO_REPLAY:
        retroreplay_freeze();
        break;
      case CARTRIDGE_KCS_POWER:
        kcs_freeze();
        break;
      case CARTRIDGE_FINAL_I:
        final_v1_freeze();
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_freeze();
        break;
    }
}

int cartridge_save_image(const char *filename)
{
    if (mem_cartridge_type == CARTRIDGE_EXPERT)
        return crt_save_expert(filename);

    return -1;
}

void cartridge_romhbank_set(unsigned int bank)
{
    romh_bank = (int)bank;
}

void cartridge_romlbank_set(unsigned int bank)
{
    roml_bank = (int)bank;
}
