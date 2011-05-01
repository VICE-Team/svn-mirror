/*
 * c64sound.c - C64/C128 sound emulation.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "c64io.h"
#include "cartridge.h"
#include "machine.h"
#include "sid.h"
#include "sid-resources.h"
#include "sound.h"
#include "types.h"

static BYTE machine_sid2_read(WORD addr)
{
    return sid2_read(addr);
}


static void machine_sid2_store(WORD addr, BYTE byte)
{
    sid2_store(addr, byte);
}

/* ---------------------------------------------------------------------*/

static io_source_t stereo_sid_device = {
    "Stereo SID",
    IO_DETACH_RESOURCE,
    "SidStereo",
    0xde00, 0xde1f, 0x1f,
    1, /* read is always valid */
    machine_sid2_store,
    machine_sid2_read,
    NULL, /* TODO: peek */
    NULL, /* TODO: dump */
    0,
    0
};

static io_source_list_t *stereo_sid_list_item = NULL;

/* ---------------------------------------------------------------------*/

int machine_sid2_check_range(unsigned int sid2_adr)
{
    if (machine_class == VICE_MACHINE_C128) {
        if ((sid2_adr >= 0xd400 && sid2_adr <= 0xd4e0) || (sid2_adr >= 0xd700 && sid2_adr <= 0xdfe0)) {
            stereo_sid_device.start_address = sid2_adr;
            stereo_sid_device.end_address = sid2_adr + 0x1f;
            if (stereo_sid_list_item != NULL) {
                c64io_unregister(stereo_sid_list_item);
                stereo_sid_list_item = c64io_register(&stereo_sid_device);
            } else {
                if (sid_stereo) {
                    stereo_sid_list_item = c64io_register(&stereo_sid_device);
                }
            }
            return 0;
        }
    } else {
        if (sid2_adr >= 0xd400 && sid2_adr <= 0xdfe0) {
            stereo_sid_device.start_address = sid2_adr;
            stereo_sid_device.end_address = sid2_adr + 0x1f;
            if (stereo_sid_list_item != NULL) {
                c64io_unregister(stereo_sid_list_item);
                stereo_sid_list_item = c64io_register(&stereo_sid_device);
            } else {
                if (sid_stereo) {
                    stereo_sid_list_item = c64io_register(&stereo_sid_device);
                }
            }
            return 0;
        }
    }
    return -1;
}

void machine_sid2_enable(int val)
{
    if (val) {
        stereo_sid_list_item = c64io_register(&stereo_sid_device);
    } else {
        if (stereo_sid_list_item != NULL) {
            c64io_unregister(stereo_sid_list_item);
            stereo_sid_list_item = NULL;
        }
    }
}

sound_t *sound_machine_open(int chipno)
{
    return sid_sound_machine_open(chipno);
}

int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    cartridge_sound_machine_init(psid, speed, cycles_per_sec);
    return sid_sound_machine_init(psid, speed, cycles_per_sec);
}

void sound_machine_close(sound_t *psid)
{
    cartridge_sound_machine_close(psid);
    sid_sound_machine_close(psid);
}

/* for read/store 0x00 <= addr <= 0x1f is the sid
 *                0x20 <= addr         is used by cartridges
 */

BYTE sound_machine_read(sound_t *psid, WORD addr)
{
    BYTE value;
    if (cartridge_sound_machine_read(psid, addr, &value)) {
        return value;
    }
    return sid_sound_machine_read(psid, addr);
}

void sound_machine_store(sound_t *psid, WORD addr, BYTE byte)
{
    cartridge_sound_machine_store(psid, addr, byte);
    sid_sound_machine_store(psid, addr, byte);
}

void sound_machine_reset(sound_t *psid, CLOCK cpu_clk)
{
    cartridge_sound_machine_reset(psid, cpu_clk);
    sid_sound_machine_reset(psid, cpu_clk);
}

int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr, int interleave, int *delta_t)
{
    int temp;

    temp=sid_sound_machine_calculate_samples(psid, pbuf, nr, interleave, delta_t);
    /* tell cartridges how many samples to generate to keep in sync with
     * resid's unpredictable sample generation. */
    cartridge_sound_machine_calculate_samples(psid, pbuf, temp, interleave, delta_t);

    return temp;
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
    sid_sound_machine_prevent_clk_overflow(psid, sub);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return sid_sound_machine_dump_state(psid);
}

int sound_machine_cycle_based(void)
{
    return sid_sound_machine_cycle_based();
}

int sound_machine_channels(void)
{
    return sid_sound_machine_channels();
}

void sound_machine_enable(int enable)
{
    sid_sound_machine_enable(enable);
}
