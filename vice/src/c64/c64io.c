/*
 * c64io.c - C64 io handling ($DE00-$DFFF).
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

#include <string.h>

#include "c64-midi.h"
#include "c64-resources.h"
#include "c64_256k.h"
#include "c64acia.h"
#include "c64cart.h"
#include "c64io.h"
#include "cartridge.h"
#include "digimax.h"
#include "dqbb.h"
#include "emuid.h"
#include "lib.h"
#include "mmc64.h"
#include "cart/retroreplay.h"
#include "monitor.h"
#include "reu.h"
#include "georam.h"
#include "isepic.h"
#include "ramcart.h"
#include "resources.h"
#include "sfx_soundsampler.h"
#include "sfx_soundexpander.h"
#include "sid-resources.h"
#include "sid.h"
#include "translate.h"
#include "types.h"
#include "uiapi.h"
#include "util.h"
#include "vicii-phi1.h"
#include "vicii.h"

#ifdef HAVE_TFE
#include "tfe.h"
#endif

struct io_source_s {
    int id;
    char *name;
    int detach_id;
    char *resource_name;
};
typedef struct io_source_s io_source_t;

static void io_source_detach(int detach_id, char *resource_name)
{
    switch (detach_id) {
        case IO_DETACH_CART:
            cartridge_detach_image();
            break;
        case IO_DETACH_RESOURCE:
            resources_set_int(resource_name, 0);
            break;
    }
    ui_update_menus();
}

static io_source_t io_source_table[] = {
    { IO_SOURCE_NONE, "NONE", 0, NULL },
    { IO_SOURCE_ACIA, "ACIA/SWIFTLINK/TURBO232", IO_DETACH_RESOURCE, "Acia1Enable" },
    { IO_SOURCE_C64_256K, "C64 256K", IO_DETACH_RESOURCE, "C64_256K" },
    { IO_SOURCE_GEORAM, "GEORAM", IO_DETACH_RESOURCE, "GEORAM" },
    { IO_SOURCE_RAMCART, "RAMCART", IO_DETACH_RESOURCE, "RAMCART" },
    { IO_SOURCE_REU, "REU", IO_DETACH_RESOURCE, "REU" },
    { IO_SOURCE_TFE_RR_NET, "ETHERNET CART", IO_DETACH_RESOURCE, "ETHERNET_ACTIVE" },
    { IO_SOURCE_STEREO_SID, "STEREO SID", IO_DETACH_RESOURCE, "SidStereo" },
    { IO_SOURCE_ACTION_REPLAY, "ACTION_REPLAY", IO_DETACH_CART, NULL },
    { IO_SOURCE_ATOMIC_POWER, "ATOMIC POWER", IO_DETACH_CART, NULL },
    { IO_SOURCE_EPYX_FASTLOAD, "EPYX FASTLOAD", IO_DETACH_CART, NULL },
    { IO_SOURCE_FINAL1, "FINAL I", IO_DETACH_CART, NULL },
    { IO_SOURCE_FINAL3, "FINAL III", IO_DETACH_CART, NULL },
    { IO_SOURCE_IDE64, "IDE64", IO_DETACH_CART, NULL },
    { IO_SOURCE_IEEE488, "IEEE488", IO_DETACH_CART, NULL },
    { IO_SOURCE_KCS, "KCS POWER", IO_DETACH_CART, NULL },
    { IO_SOURCE_MAGIC_FORMEL, "MAGIC FORMEL", IO_DETACH_CART, NULL },
    { IO_SOURCE_RR, "RETRO REPLAY", IO_DETACH_CART, NULL },
    { IO_SOURCE_SS4, "SUPER SNAPSHOT 4", IO_DETACH_CART, NULL },
    { IO_SOURCE_SS5, "SUPER SNAPSHOT 5", IO_DETACH_CART, NULL },
    { IO_SOURCE_WARPSPEED, "WARPSPEED", IO_DETACH_CART, NULL },
    { IO_SOURCE_EMUID, "EMU ID", IO_DETACH_RESOURCE, "EmuID" },
    { IO_SOURCE_MIKRO_ASSEMBLER, "MIKRO ASSEMBLER", IO_DETACH_CART, NULL },
    { IO_SOURCE_MMC64, "MMC64", IO_DETACH_RESOURCE, "MMC64" },
    { IO_SOURCE_DIGIMAX, "DIGIMAX", IO_DETACH_RESOURCE, "DIGIMAX" },
    { IO_SOURCE_ACTION_REPLAY4, "ACTION REPLAY 4", IO_DETACH_CART, NULL },
    { IO_SOURCE_STARDOS, "STARDOS", IO_DETACH_CART, NULL },
    { IO_SOURCE_MIDI, "MIDI", IO_DETACH_RESOURCE, "MIDIEnable" },
    { IO_SOURCE_ISEPIC, "ISEPIC", IO_DETACH_RESOURCE, "Isepic" },
    { IO_SOURCE_SFX_SE, "SFX SOUND EXPANDER", IO_DETACH_RESOURCE, "SFXSoundExpander" },
    { IO_SOURCE_EASYFLASH, "EASYFLASH", IO_DETACH_CART, NULL },
    { -1, NULL, 0, NULL }
};

int io_source;

static char *get_io_source_name(int id)
{
    int i = 0;

    while (io_source_table[i].id != -1) {
        if (io_source_table[i].id == id) {
            return io_source_table[i].name;
        }
        i++;
    }
    return "Unknown";
}

static int get_io_source_index(int id)
{
    int i = 0;

    while (io_source_table[i].id != -1) {
        if (io_source_table[i].id == id) {
            return i;
        }
        i++;
    }
    return 0;
}

#define MAX_IO1_RETURNS 10
#define MAX_IO2_RETURNS 13

#if MAX_IO1_RETURNS>MAX_IO2_RETURNS
static int io_source_return[MAX_IO1_RETURNS];
#else
static int io_source_return[MAX_IO2_RETURNS];
#endif

static BYTE real_return_value;
static BYTE return_value;
static int returned;
static int io_source_start;
static int io_source_end;

static void io_source_check(int counter)
{
    if (io_source != 0) {
        returned++;
        real_return_value = return_value;
        io_source_return[counter] = io_source;
        io_source = 0;
        if (io_source_start == -1) {
            io_source_start = counter;
        }
        io_source_end = counter;
    }
}

static void io_source_msg_detach(int addr)
{
    char *old_msg = NULL;
    char *new_msg = NULL;
    int i;
    int index;

    for (i = io_source_start; i < io_source_end + 1; i++) {
        if (io_source_return[i] != 0) {
            if (i == io_source_start) {
                old_msg = lib_stralloc(translate_text(IDGS_IO_READ_COLL_AT_X_FROM));
                new_msg = util_concat(old_msg, get_io_source_name(io_source_return[i]), NULL);
                lib_free(old_msg);
            }
            if (i > io_source_start && i < io_source_end) {
                old_msg = new_msg;
                new_msg = util_concat(old_msg, ", ", get_io_source_name(io_source_return[i]), NULL);
                lib_free(old_msg);
            }
            if (i == io_source_end) {
                old_msg = new_msg;
                new_msg = util_concat(old_msg, translate_text(IDGS_AND), get_io_source_name(io_source_return[i]), translate_text(IDGS_ALL_DEVICES_DETACHED), NULL);
                lib_free(old_msg);
            }
        }
    }
    ui_error(new_msg,addr);
    lib_free(new_msg);

    for (i = io_source_start; i < io_source_end + 1; i++) {
        if (io_source_return[i] != 0) {
            index = get_io_source_index(io_source_return[i]);
            io_source_detach(io_source_table[index].detach_id, io_source_table[index].resource_name);
        }
    }
}

BYTE REGPARM1 c64io1_read(WORD addr)
{
    int io_source_counter = 0;

    vicii_handle_pending_alarms_external(0);

    memset(io_source_return, 0, sizeof(io_source_return));
    returned = 0;
    io_source_start = -1;
    io_source_end = -1;

    if (sid_stereo && addr >= sid_stereo_address_start && addr < sid_stereo_address_end) {
        return_value = sid2_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (c64_256k_enabled && addr >= c64_256k_start && addr <= c64_256k_start + 0x7f) {
        return_value = c64_256k_read((WORD)(addr & 0x03));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (digimax_enabled && addr >= digimax_address && addr <= digimax_address + 3) {
        return_value = digimax_sound_read((WORD)(addr & 3));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (georam_enabled) {
        return_value = georam_window_read((WORD)(addr & 0xff));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (ramcart_enabled) {
        return_value = ramcart_reg_read((WORD)(addr & 1));
        io_source_check(io_source_counter);
        io_source_counter++;
    }

    if (isepic_enabled) {
        return_value = isepic_reg_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }

#ifdef HAVE_TFE
    if (tfe_enabled) {
        if (mmc64_enabled && tfe_as_rr_net) {
            if (mmc64_hw_clockport == 0xde02 && mmc64_clockport_enabled && addr > 0xde01 && addr < 0xde10) {
                return_value = tfe_read((WORD)(addr & 0x0f));
                io_source_check(io_source_counter);
                io_source_counter++;
            }
        } else if (rr_active && tfe_as_rr_net) {
            if (rr_clockport_enabled && addr > 0xde01 && addr < 0xde10) {
                return_value = tfe_read((WORD)(addr & 0x0f));
                io_source_check(io_source_counter);
                io_source_counter++;
            }
        } else {
            if ((tfe_as_rr_net && addr < 0xde10) || !tfe_as_rr_net) {
                return_value = tfe_read((WORD)(addr & 0x0f));
                io_source_check(io_source_counter);
                io_source_counter++;
            }
        }
    }
#endif

    if (mem_cartridge_type != CARTRIDGE_NONE) {
        return_value = cartridge_read_io1(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }

#ifdef HAVE_RS232
    if (acia_de_enabled && addr <= 0xde07) {
        return_value = acia1_read((WORD)(addr & 0x07));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
#endif
#ifdef HAVE_MIDI
    if (midi_enabled && c64_midi_base_de00()) {
        if (midi_test_read((WORD)(addr & 0xff))) {
            return_value = midi_read((WORD)(addr & 0xff));
            io_source = IO_SOURCE_MIDI;
            io_source_check(io_source_counter);
        }
        io_source_counter++;
    }
#endif

    if (returned == 0) {
        return vicii_read_phi1();
    }

    if (returned == 1) {
        return real_return_value;
    }

    io_source_msg_detach(addr);

    return vicii_read_phi1();
}

void REGPARM2 c64io1_store(WORD addr, BYTE value)
{
    vicii_handle_pending_alarms_external_write();

    if (sid_stereo && addr >= sid_stereo_address_start && addr < sid_stereo_address_end) {
        sid2_store(addr, value);
    }
    if (c64_256k_enabled && addr >= c64_256k_start && addr <= c64_256k_start + 0x7f) {
        c64_256k_store((WORD)(addr & 0x03), value);
    }
    if (digimax_enabled && addr >= digimax_address && addr <= digimax_address + 3) {
        digimax_sound_store((WORD)(addr&3), value);
    }
    if (sfx_soundsampler_enabled) {
        sfx_soundsampler_sound_store(addr, value);
    }
    if (georam_enabled) {
        georam_window_store((WORD)(addr & 0xff), value);
    }
    if (ramcart_enabled) {
        ramcart_reg_store((WORD)(addr & 1), value);
    }
    if (isepic_enabled) {
        isepic_reg_store(addr, value);
    }
    if (mmc64_enabled && mmc64_hw_clockport == 0xde02 && addr == 0xde01) {
        mmc64_clockport_enable_store(value);
    }
#ifdef HAVE_TFE
    if (tfe_enabled) {
        if (mmc64_enabled && tfe_as_rr_net) {
            if (mmc64_hw_clockport == 0xde02 && mmc64_clockport_enabled && addr > 0xde01 && addr < 0xde10) {
                tfe_store((WORD)(addr & 0x0f), value);
            }
        } else if (rr_active && tfe_as_rr_net) {
            if (rr_clockport_enabled && addr > 0xde01 && addr < 0xde10) {
                tfe_store((WORD)(addr & 0x0f), value);
            }
        } else {
            if (tfe_enabled) {
                tfe_store((WORD)(addr & 0x0f), value);
            }
        }
    }
#endif
    if (mem_cartridge_type != CARTRIDGE_NONE) {
        cartridge_store_io1(addr, value);
    }
#ifdef HAVE_RS232
    if (acia_de_enabled) {
        acia1_store((WORD)(addr & 0x07), value);
    }
#endif
#ifdef HAVE_MIDI
    if (midi_enabled && c64_midi_base_de00()) {
        midi_store((WORD)(addr & 0xff), value);
    }
#endif
    if (dqbb_enabled) {
        dqbb_reg_store(addr, value);
    }
    return;
}

BYTE REGPARM1 c64io2_read(WORD addr)
{
    int io_source_counter = 0;

    vicii_handle_pending_alarms_external(0);

    memset(io_source_return, 0, sizeof(io_source_return));
    returned = 0;
    io_source_start = -1;
    io_source_end = -1;

    if (sid_stereo && addr >= sid_stereo_address_start && addr < sid_stereo_address_end) {
        return_value = sid2_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (c64_256k_enabled && addr >= c64_256k_start && addr <= c64_256k_start + 0x7f) {
        return_value = c64_256k_read((WORD)(addr & 0x03));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (digimax_enabled && addr >= digimax_address && addr <= digimax_address + 3) {
        return_value = digimax_sound_read((WORD)(addr & 0x03));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (ramcart_enabled) {
        return_value = ramcart_window_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (isepic_enabled) {
        return_value = isepic_window_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (mem_cartridge_type == CARTRIDGE_RETRO_REPLAY) {
        return_value = cartridge_read_io2(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (reu_enabled) {
        return_value = reu_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (mmc64_enabled && addr >= 0xdf10 && addr <= 0xdf13) {
        return_value = mmc64_io2_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (sfx_soundexpander_enabled && (addr == 0xdf60 || addr == 0xdfe0)) {
        return_value = sfx_soundexpander_sound_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (sfx_soundexpander_enabled && (addr & 16) == 0 && (addr & 8) == 8) {
        return_value = sfx_soundexpander_piano_read(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
#ifdef HAVE_TFE
    if (tfe_enabled && mmc64_enabled && tfe_as_rr_net && mmc64_hw_clockport == 0xdf22 && mmc64_clockport_enabled && addr > 0xdf21 && addr < 0xdf30) {
        return_value = tfe_read((WORD)(addr & 0x0f));
        io_source_check(io_source_counter);
        io_source_counter++;
    }
#endif
    if (mem_cartridge_type != CARTRIDGE_NONE && mem_cartridge_type != CARTRIDGE_RETRO_REPLAY) {
        return_value = cartridge_read_io2(addr);
        io_source_check(io_source_counter);
        io_source_counter++;
    }
    if (emu_id_enabled && addr >= 0xdfa0) {
        return_value = emuid_read((WORD)(addr - 0xdfa0));
        io_source=IO_SOURCE_EMUID;
        io_source_check(io_source_counter);
        io_source_counter++;
    }

#ifdef HAVE_MIDI
    if (midi_enabled && !c64_midi_base_de00()) {
        if (midi_test_read((WORD)(addr & 0xff))) {
            return_value = midi_read((WORD)(addr & 0xff));
            io_source = IO_SOURCE_MIDI;
            io_source_check(io_source_counter);
        }
        io_source_counter++;
    }
#endif

    if (returned == 0) {
        return vicii_read_phi1();
    }

    if (returned == 1) {
        return real_return_value;
    }

    io_source_msg_detach(addr);

    return vicii_read_phi1();
}

void REGPARM2 c64io2_store(WORD addr, BYTE value)
{
    vicii_handle_pending_alarms_external_write();

    if (sid_stereo && addr >= sid_stereo_address_start && addr < sid_stereo_address_end) {
        sid2_store(addr, value);
    }
    if (mem_cartridge_type == CARTRIDGE_RETRO_REPLAY) {
        cartridge_store_io2(addr, value);
    }
    if (c64_256k_enabled && addr >= c64_256k_start && addr <= c64_256k_start + 0x7f) {
        c64_256k_store((WORD)(addr & 0x03), value);
    }
    if (digimax_enabled && addr >= digimax_address && addr <= digimax_address + 3) {
        digimax_sound_store((WORD)(addr & 0x03), value);
    }
    if (reu_enabled) {
        reu_store(addr, value);
    }
    if (georam_enabled && addr >= 0xdf80) {
        georam_reg_store((WORD)(addr & 1), value);
    }
    if (ramcart_enabled) {
        ramcart_window_store(addr, value);
    }
    if (isepic_enabled) {
        isepic_window_store(addr, value);
    }
    if (mmc64_enabled && addr >= 0xdf10 && addr <= 0xdf13) {
        mmc64_io2_store(addr, value);
    }
    if (mmc64_enabled && mmc64_hw_clockport == 0xdf22 && addr == 0xdf21) {
        mmc64_clockport_enable_store(value);
    }
    if (sfx_soundexpander_enabled && (addr == 0xdf40 || addr == 0xdfc0)) {
        sfx_soundexpander_sound_address_store(addr, value);
    }
    if (sfx_soundexpander_enabled && (addr == 0xdf50 || addr == 0xdfd0)) {
        sfx_soundexpander_sound_register_store(addr, value);
    }
#ifdef HAVE_TFE
    if (tfe_enabled && mmc64_enabled && tfe_as_rr_net && mmc64_hw_clockport == 0xdf22 && mmc64_clockport_enabled && addr > 0xdf21 && addr < 0xdf30) {
        tfe_store((WORD)(addr & 0x0f), value);
    }
#endif
    if (mem_cartridge_type != CARTRIDGE_NONE) {
        cartridge_store_io2(addr, value);
    }

#ifdef HAVE_MIDI
    if (midi_enabled && !c64_midi_base_de00()) {
        midi_store((WORD)(addr & 0xff), value);
    }
#endif

    return;
}

void c64io_ioreg_add_list(struct mem_ioreg_list_s **mem_ioreg_list)
{
#ifdef HAVE_RS232
    if (acia_de_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "ACIA", 0xde00, 0xde07);
    }
#endif

    if (c64_256k_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "C64_256K", c64_256k_start, c64_256k_start + 0x7f);
    }

    if (digimax_enabled && digimax_address != 0xdd00) {
        mon_ioreg_add_list(mem_ioreg_list, "DIGIMAX", digimax_address, digimax_address + 3);
    }

    if (georam_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "GEORAM", 0xde00, 0xdeff);
        mon_ioreg_add_list(mem_ioreg_list, "GEORAM", 0xdffe, 0xdfff);
    }

    if (isepic_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "ISEPIC", 0xde00, 0xdfff);
    }

#ifdef HAVE_MIDI
    if (midi_enabled) {
        if (c64_midi_base_de00()) {
            if (midi_test_read((WORD)(0xde02 & 0xff))) {
                mon_ioreg_add_list(mem_ioreg_list, "MIDI", 0xde00, 0xde03);
            } else if (midi_test_read((WORD)(0xde06 & 0xff))) {
                mon_ioreg_add_list(mem_ioreg_list, "MIDI", 0xde04, 0xde07);
            } else if (midi_test_read((WORD)(0xde08 & 0xff))) {
                mon_ioreg_add_list(mem_ioreg_list, "MIDI", 0xde08, 0xde09);
            }
        } else {
            mon_ioreg_add_list(mem_ioreg_list, "MIDI", 0xdf00, 0xdf01);
        }
    }
#endif

    if (mmc64_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "MMC64", 0xdf10, 0xdf13);
    }

    if (ramcart_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "RAMCART", 0xde00, 0xde01);
        mon_ioreg_add_list(mem_ioreg_list, "RAMCART", 0xdf00, 0xdfff);
    }

    if (reu_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "REU", 0xdf00, 0xdf0f);
    }

    if (sfx_soundexpander_enabled) {
        mon_ioreg_add_list(mem_ioreg_list, "SFX SOUND EXPANDER", 0xdf60, 0xdf60);
        mon_ioreg_add_list(mem_ioreg_list, "SFX SOUND EXPANDER", 0xdfe0, 0xdfe0);
        mon_ioreg_add_list(mem_ioreg_list, "SFX SOUND EXPANDER PIANO", 0xdf08, 0xdf08);
    }

    if (sid_stereo) {
        mon_ioreg_add_list(mem_ioreg_list, "STEREO SID", sid_stereo_address_start, sid_stereo_address_start + 0x1f);
    }

#ifdef HAVE_TFE
    if (tfe_enabled) {
        if (mmc64_enabled && tfe_as_rr_net) {
            if (mmc64_hw_clockport == 0xde02 && mmc64_clockport_enabled) {
                mon_ioreg_add_list(mem_ioreg_list, "TFE", 0xde02, 0xde0f);
            } else if (mmc64_hw_clockport == 0xdf22 && mmc64_clockport_enabled) {
                mon_ioreg_add_list(mem_ioreg_list, "TFE", 0xdf22, 0xdf2f);
            }
        } else if (rr_active && tfe_as_rr_net) {
            if (rr_clockport_enabled) {
                mon_ioreg_add_list(mem_ioreg_list, "TFE", 0xde02, 0xde0f);
            }
        } else {
             mon_ioreg_add_list(mem_ioreg_list, "TFE", 0xde00, 0xde0f);
        }
    }
#endif

    if (mem_cartridge_type != CARTRIDGE_NONE) {
        switch (mem_cartridge_type) {
            case CARTRIDGE_ACTION_REPLAY3:
                if (actionreplay3_get_active()) {
                    mon_ioreg_add_list(mem_ioreg_list, "ACTION REPLAY 3", 0xdf00, 0xdfff);
                }
                break;
            case CARTRIDGE_IEEE488:
                mon_ioreg_add_list(mem_ioreg_list, "IEEE488", 0xdf00, 0xdf07);
                break;
            case CARTRIDGE_IDE64:
                mon_ioreg_add_list(mem_ioreg_list, "IDE64", 0xde30, 0xde32);
                if (!ide64_get_killport()) {
                    mon_ioreg_add_list(mem_ioreg_list, "IDE64", 0xde5f, 0xdeff);
                }
            case CARTRIDGE_RETRO_REPLAY:
                if (rr_active) {
                    mon_ioreg_add_list(mem_ioreg_list, "RETRO REPLAY", 0xde00, 0xde01);
                    if (reu_mapping) {
#ifdef HAVE_TFE
                        if (rr_clockport_enabled && tfe_enabled && tfe_as_rr_net) {
                            mon_ioreg_add_list(mem_ioreg_list, "RETRO REPLAY", 0xde10, 0xdeff);
                        } else 
#endif                        
                        {
                            mon_ioreg_add_list(mem_ioreg_list, "RETRO REPLAY", 0xde02, 0xdeff);
                        }
                    } else {
                        mon_ioreg_add_list(mem_ioreg_list, "RETRO REPLAY", 0xdf00, 0xdfff);
                    }
                }
                break;
            case CARTRIDGE_SUPER_SNAPSHOT:
                mon_ioreg_add_list(mem_ioreg_list, "SUPER SNAPSHOT V4", 0xde00, 0xdfff);
                break;
            case CARTRIDGE_KCS_POWER:
                mon_ioreg_add_list(mem_ioreg_list, "KCS POWER", 0xde00, 0xdfff);
                break;
            case CARTRIDGE_FINAL_III:
                mon_ioreg_add_list(mem_ioreg_list, "FINAL CARTRIDGE III", 0xde00, 0xdfff);
                break;
            case CARTRIDGE_ACTION_REPLAY:
                if (actionreplay_get_active()) {
                    mon_ioreg_add_list(mem_ioreg_list, "ACTION REPLAY", 0xdf00, 0xdfff);
                }
                break;
            case CARTRIDGE_SUPER_GAMES:
                mon_ioreg_add_list(mem_ioreg_list, "ACTION REPLAY", 0xde00, 0xdeff);
                break;
            case CARTRIDGE_EPYX_FASTLOAD:
                mon_ioreg_add_list(mem_ioreg_list, "EPYX FASTLOAD", 0xdf00, 0xdfff);
                break;
            case CARTRIDGE_FINAL_I:
                mon_ioreg_add_list(mem_ioreg_list, "FINAL CARTRIDGE I", 0xde00, 0xdfff);
                break;
            case CARTRIDGE_MAGIC_FORMEL:
                mon_ioreg_add_list(mem_ioreg_list, "MAGIC FORMEL", 0xde00, 0xdeff);
                break;
            case CARTRIDGE_WARPSPEED:
                mon_ioreg_add_list(mem_ioreg_list, "WARPSPEED", 0xde00, 0xdfff);
                break;
            case CARTRIDGE_SUPER_SNAPSHOT_V5:
                mon_ioreg_add_list(mem_ioreg_list, "SUPER SNAPSHOT V5", 0xde00, 0xdeff);
                break;
            case CARTRIDGE_MIKRO_ASSEMBLER:
                mon_ioreg_add_list(mem_ioreg_list, "MIKRO ASSEMBLER", 0xde00, 0xdfff);
                break;
            case CARTRIDGE_ACTION_REPLAY4:
                if (actionreplay4_get_active()) {
                    mon_ioreg_add_list(mem_ioreg_list, "ACTION REPLAY 4", 0xdf00, 0xdfff);
                }
                break;
            case CARTRIDGE_EASYFLASH:
                mon_ioreg_add_list(mem_ioreg_list, "EASYFLASH", 0xdf00, 0xdfff);
                break;
        }
    }
}
