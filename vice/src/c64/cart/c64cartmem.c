/*
 * c64cartmem.c -- C64 cartridge emulation, memory handling.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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
#include "cartridge.h"
#include "crt.h"
#include "expert.h"
#include "final.h"
#include "generic.h"
#include "interrupt.h"
#include "kcs.h"
#include "log.h"
#include "machine.h"
#include "retroreplay.h"
#include "supersnapshot.h"
#include "types.h"

/* #define DEBUG */

/* Expansion port signals.  */
export_t export;

/* Exansion port ROML/ROMH images.  */
BYTE roml_banks[0x80000], romh_banks[0x20000];

/* Exansion port RAM images.  */
BYTE export_ram0[C64CART_RAM_LIMIT];

/* Expansion port ROML/ROMH/RAM banking.  */
int roml_bank = 0, romh_bank = 0, export_ram = 0;

/* Flag: Ultimax (VIC-10) memory configuration enabled.  */
int ultimax = 0;

/* Type of the cartridge attached.  */
int mem_cartridge_type = CARTRIDGE_NONE;


void cartridge_config_changed(BYTE mode)
{
    export.game = mode & 1;
    export.exrom = ((mode >> 1) & 1) ^ 1;
    romh_bank = roml_bank = (mode >> 3) & 3;
    export_ram = (mode >> 5) & 1;
    pla_config_changed();
    if (mode & 0x40)
        cartridge_release_freeze();
    ultimax = export.game & (export.exrom ^ 1);
    machine_update_memory_ptrs();
}

BYTE REGPARM1 cartridge_read_io1(ADDRESS addr)
{
#ifdef DEBUG
    log_debug("Read IO1 %02x.", addr);
#endif
    switch (mem_cartridge_type) {
      case CARTRIDGE_ACTION_REPLAY:
        return atomicpower_io1_read(addr);
      case CARTRIDGE_ATOMIC_POWER:
        return actionreplay_io1_read(addr);
      case CARTRIDGE_RETRO_REPLAY:
        return retroreplay_io1_read(addr);
      case CARTRIDGE_KCS_POWER:
        return kcs_io1_read(addr);
      case CARTRIDGE_FINAL_III:
        return final_v3_io1_read(addr);
      case CARTRIDGE_FINAL_I:
        return final_v1_io1_read(addr);
      case CARTRIDGE_SIMONS_BASIC:
      case CARTRIDGE_GS:
        cartridge_config_changed(0);
        return rand();
      case CARTRIDGE_WARPSPEED:
        return roml_banks[0x1e00 + (addr & 0xff)];
      case CARTRIDGE_DINAMIC:
        roml_bank = (addr & 0x0f);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        return supersnapshot_v4_io1_read(addr);
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        return supersnapshot_v5_io1_read(addr);
      case CARTRIDGE_EXPERT:
        return expert_io1_read(addr);
    }
    return rand();
}

void REGPARM2 cartridge_store_io1(ADDRESS addr, BYTE value)
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
      case CARTRIDGE_SIMONS_BASIC:
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_WARPSPEED:
        cartridge_config_changed(1);
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
            romh_bank = roml_bank = value & 0x3f;
            break;
          case CARTRIDGE_FUNPLAY:
            romh_bank = roml_bank = ((value >> 2) | (value & 1)) & 15;
            break;
        }
        export.game = export.exrom = 1;
        pla_config_changed();
        ultimax = 0;
        break;
      case CARTRIDGE_GS:
        roml_bank = addr & 0x3f;
        export.game = 0;
        export.exrom = 1;
        break;
      case CARTRIDGE_EXPERT:
        expert_io1_store(addr, value);
        break;
    }
    return;
}

BYTE REGPARM1 cartridge_read_io2(ADDRESS addr)
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
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        return supersnapshot_v5_io2_read(addr);
      case CARTRIDGE_FINAL_III:
        return final_v3_io2_read(addr);
      case CARTRIDGE_FINAL_I:
        return final_v1_io2_read(addr);
      case CARTRIDGE_KCS_POWER:
        return kcs_io2_read(addr);
      case CARTRIDGE_IEEE488:
        return tpi_read((ADDRESS)(addr & 0x07));
      case CARTRIDGE_EPYX_FASTLOAD:
        if (addr == 0xdf18)
            cartridge_config_changed(0);
        if (addr == 0xdf38)
            cartridge_config_changed(2);
        return roml_banks[0x1f00 + (addr & 0xff)];
      case CARTRIDGE_WESTERMANN:
        cartridge_config_changed(0);
        return rand();
      case CARTRIDGE_REX:
        if ((addr & 0xff) < 0xc0)
            cartridge_config_changed(2);
        else
            cartridge_config_changed(0);
        return 0;
      case CARTRIDGE_WARPSPEED:
        return roml_banks[0x1f00 + (addr & 0xff)];
      case CARTRIDGE_EXPERT:
        return expert_io2_read(addr);
    }
    return rand();
}

void REGPARM2 cartridge_store_io2(ADDRESS addr, BYTE value)
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
        cartridge_config_changed(2);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_io2_store(addr, value);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_io2_store(addr, value);
        break;
      case CARTRIDGE_FINAL_III:
        final_v3_io2_store(addr, value);
        break;
      case CARTRIDGE_SUPER_GAMES:
        romh_bank = roml_bank = value & 3;
        if (value & 0x4) {
            export.game = 0;
            export.exrom = 1;
        } else {
            export.game = export.exrom = 1;
        }
        if (value == 0xc)
            export.game = export.exrom = 0;
        pla_config_changed();
        break;
      case CARTRIDGE_IEEE488:
        tpi_store((ADDRESS)(addr & 0x07), value);
        break;
      case CARTRIDGE_EXPERT:
        expert_io2_store(addr, value);
        break;
    }
    return;
}

BYTE REGPARM1 roml_read(ADDRESS addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ZAXXON:
        romh_bank = (addr & 0x1000) ? 1 : 0;
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        return supersnapshot_v4_roml_read(addr);
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        return supersnapshot_v5_roml_read(addr);
      case CARTRIDGE_ACTION_REPLAY:
        return actionreplay_roml_read(addr);
      case CARTRIDGE_RETRO_REPLAY:
        return retroreplay_roml_read(addr);
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_roml_read(addr);
      case CARTRIDGE_EXPERT:
        return expert_roml_read(addr);
      case CARTRIDGE_FINAL_I:
        return final_v1_roml_read(addr);
      case CARTRIDGE_FINAL_III:
        return final_v3_roml_read(addr);
    }

    if (export_ram)
        return export_ram0[addr & 0x1fff];

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

BYTE REGPARM1 romh_read(ADDRESS addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_romh_read(addr);
      case CARTRIDGE_EXPERT:
        return expert_romh_read(addr);
      case CARTRIDGE_OCEAN:
        /* 256 kB OCEAN carts may access memory either at $8000 or $a000 */
        return roml_banks[(addr & 0x1fff) + (romh_bank << 13)];
    }
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

void REGPARM2 roml_store(ADDRESS addr, BYTE value)
{
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
    }

    if (export_ram)
        export_ram0[addr & 0x1fff] = value;

    return;
}

BYTE REGPARM1 ultimax_a000_bfff_read(ADDRESS addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ATOMIC_POWER:
        return atomicpower_a000_bfff_read(addr);
    }
    return 0x55;
}

void REGPARM2 ultimax_a000_bfff_store(ADDRESS addr, BYTE value)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_ATOMIC_POWER:
        atomicpower_a000_bfff_store(addr, value);
    }
    return;
}

void REGPARM1 cartridge_decode_address(ADDRESS addr)
{
    switch (mem_cartridge_type) {
      case CARTRIDGE_EXPERT:
        expert_decode_address(addr);
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
      case CARTRIDGE_KCS_POWER:
        kcs_config_init();
        break;
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_SUPER_GAMES:
      case CARTRIDGE_EPYX_FASTLOAD:
      case CARTRIDGE_REX:
        generic_8kb_config_init();
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
      case CARTRIDGE_ZAXXON:
        generic_16kb_config_init();
        break;
      case CARTRIDGE_ULTIMAX:
        cartridge_config_changed(3);
        break;
      case CARTRIDGE_SUPER_SNAPSHOT:
        supersnapshot_v4_config_init();
        break;
      case CARTRIDGE_SUPER_SNAPSHOT_V5:
        supersnapshot_v5_config_init();
        break;
      case CARTRIDGE_OCEAN:
      case CARTRIDGE_FUNPLAY:
        cartridge_config_changed(1);
        cartridge_store_io1((ADDRESS)0xde00, 0);
        break;
      case CARTRIDGE_GS:
        cartridge_config_changed(0);
        cartridge_store_io1((ADDRESS)0xde00, 0);
        break;
      case CARTRIDGE_DINAMIC:
        cartridge_config_changed(0);
        cartridge_read_io1((ADDRESS)0xde00);
        break;
      case CARTRIDGE_IEEE488:
        cartridge_config_changed(0);
        /* FIXME: Insert interface init here.  */
        break;
      case CARTRIDGE_EXPERT:
        expert_config_init();
        break;
    }
}

void cartridge_attach(int type, BYTE *rawcart)
{
    mem_cartridge_type = type;
    roml_bank = romh_bank = 0;
    switch (type) {
      case CARTRIDGE_GENERIC_8KB:
      case CARTRIDGE_IEEE488:
      case CARTRIDGE_EPYX_FASTLOAD:
      case CARTRIDGE_REX:
        generic_8kb_config_setup(rawcart);
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
        cartridge_config_changed(1);
        break;
      case CARTRIDGE_ULTIMAX:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        cartridge_config_changed(3);
        break;
      case CARTRIDGE_SUPER_GAMES:
        memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
        memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
        memcpy(&roml_banks[0x2000], &rawcart[0x4000], 0x2000);
        memcpy(&romh_banks[0x2000], &rawcart[0x6000], 0x2000);
        memcpy(&roml_banks[0x4000], &rawcart[0x8000], 0x2000);
        memcpy(&romh_banks[0x4000], &rawcart[0xa000], 0x2000);
        memcpy(&roml_banks[0x6000], &rawcart[0xc000], 0x2000);
        memcpy(&romh_banks[0x6000], &rawcart[0xe000], 0x2000);
        cartridge_config_changed(0);
        break;
      case CARTRIDGE_EXPERT:
        expert_config_setup(rawcart);
        break;
      case CARTRIDGE_ZAXXON:
        memcpy(roml_banks, rawcart, 0x2000);
        memcpy(romh_banks, &rawcart[0x2000], 0x4000);
        cartridge_config_changed(1);
        break;
      default:
        mem_cartridge_type = CARTRIDGE_NONE;
    }
    return;
}

void cartridge_detach(int type)
{
    switch (type) {
      case CARTRIDGE_IEEE488:
      /* FIXME: Insert interface removal here.  */
      break;
    }
    cartridge_config_changed(6);
    mem_cartridge_type = CARTRIDGE_NONE;
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

void cartridge_ack_nmi_reset(void)
{
    if (mem_cartridge_type == CARTRIDGE_EXPERT)
        expert_ack_nmi_reset();
}

/*
 * This function writes Expert .crt images ONLY!!!
 */
int cartridge_save_image(const char *filename)
{
    if (mem_cartridge_type == CARTRIDGE_EXPERT)
        return crt_save(filename);

    return -1;
}

