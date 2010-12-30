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
#include "machine.h"
#include "sid.h"
#include "sid-resources.h"
#include "sound.h"
#include "types.h"

/* FIXME: make proper hooks for carts */
#define CARTRIDGE_INCLUDE_PRIVATE_API
#include "digimax.h"
#include "magicvoice.h"
#include "sfx_soundexpander.h"
#include "sfx_soundsampler.h"
#undef CARTRIDGE_INCLUDE_PRIVATE_API

static BYTE REGPARM1 machine_sid2_read(WORD addr)
{
    return sid2_read(addr);
}


static void REGPARM2 machine_sid2_store(WORD addr, BYTE byte)
{
    sid2_store(addr, byte);
}

/* ---------------------------------------------------------------------*/

static io_source_t stereo_sid_device = {
    "STEREO SID",
    IO_DETACH_RESOURCE,
    "SidStereo",
    0xde00, 0xde1f, 0x1f,
    1, /* read is always valid */
    machine_sid2_store,
    machine_sid2_read
};

static io_source_list_t *stereo_sid_list_item = NULL;

/* current config, NULL if not in the range of $de00-$dfff */
static io_source_t *current_device = NULL;

/* ---------------------------------------------------------------------*/

int machine_sid2_check_range(unsigned int sid2_adr)
{
    if (machine_class == VICE_MACHINE_C128) {
        if (sid2_adr >= 0xd420 && sid2_adr <= 0xd4e0) {
            if (stereo_sid_list_item != NULL) {
                c64io_unregister(stereo_sid_list_item);
                stereo_sid_list_item = NULL;
            }
            current_device = NULL;
            return 0;
        }

        if (sid2_adr >= 0xd700 && sid2_adr <= 0xd7e0) {
            if (stereo_sid_list_item != NULL) {
                c64io_unregister(stereo_sid_list_item);
                stereo_sid_list_item = NULL;
            }
            current_device = NULL;
            return 0;
        }
    } else {
        if (sid2_adr >= 0xd420 && sid2_adr <= 0xd7e0) {
            if (stereo_sid_list_item != NULL) {
                c64io_unregister(stereo_sid_list_item);
                stereo_sid_list_item = NULL;
            }
            current_device = NULL;
            return 0;
        }
    }

    if (sid2_adr >= 0xde00 && sid2_adr <= 0xdfe0) {
        stereo_sid_device.start_address = sid2_adr;
        stereo_sid_device.end_address = sid2_adr + 0x1f;
        current_device = &stereo_sid_device;
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

    return -1;
}

void machine_sid2_enable(int val)
{
    if (val) {
        if (current_device != NULL) {
            stereo_sid_list_item = c64io_register(&stereo_sid_device);
        }
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

/* FIXME: make hook for carts */
int sound_machine_init(sound_t *psid, int speed, int cycles_per_sec)
{
    digimax_sound_machine_init(psid, speed, cycles_per_sec);
    sfx_soundexpander_sound_machine_init(psid, speed, cycles_per_sec);
    sfx_soundsampler_sound_machine_init(psid, speed, cycles_per_sec);
    magicvoice_sound_machine_init(psid, speed, cycles_per_sec);

    return sid_sound_machine_init(psid, speed, cycles_per_sec);
}

/* FIXME: make hook for carts */
void sound_machine_close(sound_t *psid)
{
    sfx_soundexpander_sound_machine_close(psid);
    magicvoice_sound_machine_close(psid);

    sid_sound_machine_close(psid);
}

/* for read/store 0x00 <= addr <= 0x1f is the sid
 *                0x20 <= addr <= 0x3f is the digimax
 *                0x40 <= addr <= 0x5f is the SFX sound sampler
 *                0x60 <= addr <= 0x7f is the SFX sound expander
 *                0x80 <= addr <= 0x9f is the Magic Voice
 *
 * future sound devices will be able to use 0x80 and up
 */

/* FIXME: make hook for carts */
BYTE sound_machine_read(sound_t *psid, WORD addr)
{
    if (addr >= 0x20 && addr <= 0x3f) {
        return digimax_sound_machine_read(psid, (WORD)(addr - 0x20));
    }

    if (addr >= 0x40 && addr <= 0x5f) {
        return sfx_soundsampler_sound_machine_read(psid, (WORD)(addr - 0x40));
    }

    if (addr >= 0x60 && addr <= 0x7f) {
        return sfx_soundexpander_sound_machine_read(psid, (WORD)(addr - 0x60)); /* <- typo? was 0x40 */
    }

    if (addr >= 0x80 && addr <= 0x9f) {
        return magicvoice_sound_machine_read(psid, (WORD)(addr - 0x80));
    }

    return sid_sound_machine_read(psid, addr);
}

/* FIXME: make hook for carts */
void sound_machine_store(sound_t *psid, WORD addr, BYTE byte)
{
    if (addr >= 0x20 && addr <= 0x3f) {
        digimax_sound_machine_store(psid, (WORD)(addr - 0x20), byte);
    }

    if (addr >= 0x40 && addr <= 0x5f) {
        sfx_soundsampler_sound_machine_store(psid, (WORD)(addr - 0x40), byte);
    }

    if (addr >= 0x60 && addr <= 0x7f) {
        sfx_soundexpander_sound_machine_store(psid, (WORD)(addr - 0x60), byte);
    }

    if (addr >= 0x80 && addr <= 0x9f) {
        magicvoice_sound_machine_store(psid, (WORD)(addr - 0x80), byte);
    }

    sid_sound_machine_store(psid, addr, byte);
}

/* FIXME: make hook for carts */
void sound_machine_reset(sound_t *psid, CLOCK cpu_clk)
{
    digimax_sound_reset();
    sfx_soundexpander_sound_reset();
    sfx_soundsampler_sound_reset();
    magicvoice_sound_machine_reset(psid, cpu_clk);

    sid_sound_machine_reset(psid, cpu_clk);
}

/* FIXME: make hook for carts */
int sound_machine_calculate_samples(sound_t *psid, SWORD *pbuf, int nr, int interleave, int *delta_t)
{
    int temp;

    temp=sid_sound_machine_calculate_samples(psid, pbuf, nr, interleave, delta_t);
    /* tell digimax & others how many samples to generate to keep in sync with
     * resid's unpredictable sample generation. */
    digimax_sound_machine_calculate_samples(psid, pbuf, temp, interleave, delta_t);
    sfx_soundexpander_sound_machine_calculate_samples(psid, pbuf, temp, interleave, delta_t);
    sfx_soundsampler_sound_machine_calculate_samples(psid, pbuf, temp, interleave, delta_t);
    magicvoice_sound_machine_calculate_samples(psid, pbuf, temp, interleave, delta_t);

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
