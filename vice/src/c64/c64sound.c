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

#include "cartio.h"
#include "cartridge.h"
#include "machine.h"
#include "sid.h"
#include "sid-resources.h"
#include "sound.h"
#include "types.h"

#define MACHINE_SIDx_RFUNC(fname, func)  \
    static uint8_t fname(uint16_t addr) \
    {                                   \
        return func(addr);              \
    }

MACHINE_SIDx_RFUNC(machine_sid2_read, sid2_read)
MACHINE_SIDx_RFUNC(machine_sid3_read, sid3_read)
MACHINE_SIDx_RFUNC(machine_sid4_read, sid4_read)

MACHINE_SIDx_RFUNC(machine_sid2_peek, sid2_peek)
MACHINE_SIDx_RFUNC(machine_sid3_peek, sid3_peek)
MACHINE_SIDx_RFUNC(machine_sid4_peek, sid4_peek)

#define MACHINE_SIDx_STORE(fname, func)            \
    static void fname(uint16_t addr, uint8_t byte) \
    {                                              \
        func(addr, byte);                          \
    }

MACHINE_SIDx_STORE(machine_sid2_store, sid2_store)
MACHINE_SIDx_STORE(machine_sid3_store, sid3_store)
MACHINE_SIDx_STORE(machine_sid4_store, sid4_store)

/* ---------------------------------------------------------------------*/

/* 2nd SID, can be a cartridge or an internal board */
static io_source_t sid2_device = {
    "Stereo SID",         /* name of the device */
    IO_DETACH_RESOURCE,   /* use resource to detach the device when involved in a read-collision */
    "SidStereo",          /* resource to set to '0' */
    0xde00, 0xde1f, 0x1f, /* range for the 2nd SID device, can be changed to other ranges */
    1,                    /* read is always valid */
    machine_sid2_store,   /* store function */
    NULL,                 /* NO poke function */
    machine_sid2_read,    /* read function */
    machine_sid2_peek,    /* peek function */
    sid2_dump,            /* device state information dump function */
    IO_CART_ID_NONE,      /* none is used here, because it is an I/O only device */
    IO_PRIO_NORMAL,       /* normal priority, device read needs to be checked for collisions */
    0                     /* insertion order, gets filled in by the registration function */
};

/* 3rd SID, can be a cartridge or an internal board */
static io_source_t sid3_device = {
    "Triple SID",         /* name of the device */
    IO_DETACH_RESOURCE,   /* use resource to detach the device when involved in a read-collision */
    "SidStereo",          /* resource to set to '0' */
    0xdf00, 0xdf1f, 0x1f, /* range for the 3rd SID device, can be changed to other ranges */
    1,                    /* read is always valid */
    machine_sid3_store,   /* store function */
    NULL,                 /* NO poke function */
    machine_sid3_read,    /* read function */
    machine_sid3_peek,    /* peek function */
    sid3_dump,            /* device state information dump function */
    IO_CART_ID_NONE,      /* none is used here, because it is an I/O only device */
    IO_PRIO_NORMAL,       /* normal priority, device read needs to be checked for collisions */
    0                     /* insertion order, gets filled in by the registration function */
};

/* 4th SID, can be a cartridge or an internal board */
static io_source_t sid4_device = {
    "Quad SID",           /* name of the device */
    IO_DETACH_RESOURCE,   /* use resource to detach the device when involved in a read-collision */
    "SidStereo",          /* resource to set to '0' */
    0xdf80, 0xdf9f, 0x1f, /* range for the 4th SID device, can be changed to other ranges */
    1,                    /* read is always valid */
    machine_sid4_store,   /* store function */
    NULL,                 /* NO poke function */
    machine_sid4_read,    /* read function */
    machine_sid4_peek,    /* peek function */
    sid4_dump,            /* device state information dump function */
    IO_CART_ID_NONE,      /* none is used here, because it is an I/O only device */
    IO_PRIO_NORMAL,       /* normal priority, device read needs to be checked for collisions */
    0                     /* insertion order, gets filled in by the registration function */
};

static io_source_list_t *sid2_list_item = NULL;
static io_source_list_t *sid3_list_item = NULL;
static io_source_list_t *sid4_list_item = NULL;

/* ---------------------------------------------------------------------*/

/* C64 SID sound chip */
static sound_chip_t sid_sound_chip = {
    sid_sound_machine_open,              /* sound chip open function */
    sid_sound_machine_init,              /* sound chip init function */
    sid_sound_machine_close,             /* sound chip close function */
    sid_sound_machine_calculate_samples, /* sound chip calculate samples function */
    sid_sound_machine_store,             /* sound chip store function */
    sid_sound_machine_read,              /* sound chip read function */
    sid_sound_machine_reset,             /* sound chip reset function */
    sid_sound_machine_cycle_based,       /* sound chip 'is_cycle_based()' function, resid engine is cycle based, all other engines are not */
    sid_sound_machine_channels,          /* sound chip 'get_amount_of_channels()' function, the amount of channels depends on the extra amount of active SIDs */
    1                                    /* sound chip is always enabled */
};

static uint16_t sid_sound_chip_offset = 0;

void sid_sound_chip_init(void)
{
    sid_sound_chip_offset = sound_chip_register(&sid_sound_chip);
}

/* ---------------------------------------------------------------------*/

int machine_sid2_check_range(unsigned int sid2_adr)
{
    if (machine_class == VICE_MACHINE_C128) {
        if ((sid2_adr >= 0xd400 && sid2_adr <= 0xd4e0) || (sid2_adr >= 0xd700 && sid2_adr <= 0xdfe0)) {
            sid_stereo_address_start = sid2_adr;
            sid2_device.start_address = sid2_adr;
            sid_stereo_address_end = sid2_adr + 0x1f;
            sid2_device.end_address = sid2_adr + 0x1f;
            if (sid2_list_item != NULL) {
                io_source_unregister(sid2_list_item);
                sid2_list_item = io_source_register(&sid2_device);
            } else {
                if (sid_stereo >= 1) {
                    sid2_list_item = io_source_register(&sid2_device);
                }
            }
            return 0;
        }
    } else {
        if (sid2_adr >= 0xd400 && sid2_adr <= 0xdfe0) {
            sid_stereo_address_start = sid2_adr;
            sid2_device.start_address = sid2_adr;
            sid_stereo_address_end = sid2_adr + 0x1f;
            sid2_device.end_address = sid2_adr + 0x1f;
            if (sid2_list_item != NULL) {
                io_source_unregister(sid2_list_item);
                sid2_list_item = io_source_register(&sid2_device);
            } else {
                if (sid_stereo >= 1) {
                    sid2_list_item = io_source_register(&sid2_device);
                }
            }
            return 0;
        }
    }
    return -1;
}

int machine_sid3_check_range(unsigned int sid3_adr)
{
    if (machine_class == VICE_MACHINE_C128) {
        if ((sid3_adr >= 0xd400 && sid3_adr <= 0xd4e0) || (sid3_adr >= 0xd700 && sid3_adr <= 0xdfe0)) {
            sid_triple_address_start = sid3_adr;
            sid3_device.start_address = sid3_adr;
            sid_triple_address_end = sid3_adr + 0x1f;
            sid3_device.end_address = sid3_adr + 0x1f;
            if (sid3_list_item != NULL) {
                io_source_unregister(sid3_list_item);
                sid3_list_item = io_source_register(&sid3_device);
            } else {
                if (sid_stereo >= 2) {
                    sid3_list_item = io_source_register(&sid3_device);
                }
            }
            return 0;
        }
    } else {
        if (sid3_adr >= 0xd400 && sid3_adr <= 0xdfe0) {
            sid_triple_address_start = sid3_adr;
            sid3_device.start_address = sid3_adr;
            sid_triple_address_end = sid3_adr + 0x1f;
            sid3_device.end_address = sid3_adr + 0x1f;
            if (sid3_list_item != NULL) {
                io_source_unregister(sid3_list_item);
                sid3_list_item = io_source_register(&sid3_device);
            } else {
                if (sid_stereo >= 2) {
                    sid3_list_item = io_source_register(&sid3_device);
                }
            }
            return 0;
        }
    }
    return -1;
}

int machine_sid4_check_range(unsigned int sid4_adr)
{
    if (machine_class == VICE_MACHINE_C128) {
        if ((sid4_adr >= 0xd400 && sid4_adr <= 0xd4e0) || (sid4_adr >= 0xd700 && sid4_adr <= 0xdfe0)) {
            sid_quad_address_start = sid4_adr;
            sid4_device.start_address = sid4_adr;
            sid_quad_address_end = sid4_adr + 0x1f;
            sid4_device.end_address = sid4_adr + 0x1f;
            if (sid4_list_item != NULL) {
                io_source_unregister(sid4_list_item);
                sid4_list_item = io_source_register(&sid4_device);
            } else {
                if (sid_stereo >= 3) {
                    sid4_list_item = io_source_register(&sid4_device);
                }
            }
            return 0;
        }
    } else {
        if (sid4_adr >= 0xd400 && sid4_adr <= 0xdfe0) {
            sid_quad_address_start = sid4_adr;
            sid4_device.start_address = sid4_adr;
            sid_quad_address_end = sid4_adr + 0x1f;
            sid4_device.end_address = sid4_adr + 0x1f;
            if (sid4_list_item != NULL) {
                io_source_unregister(sid4_list_item);
                sid4_list_item = io_source_register(&sid4_device);
            } else {
                if (sid_stereo >= 3) {
                    sid4_list_item = io_source_register(&sid4_device);
                }
            }
            return 0;
        }
    }
    return -1;
}

void machine_sid2_enable(int val)
{
    if (sid2_list_item != NULL) {
        io_source_unregister(sid2_list_item);
        sid2_list_item = NULL;
    }
    if (sid3_list_item != NULL) {
        io_source_unregister(sid3_list_item);
        sid3_list_item = NULL;
    }
    if (sid4_list_item != NULL) {
        io_source_unregister(sid4_list_item);
        sid4_list_item = NULL;
    }

    if (val >= 1) {
        sid2_list_item = io_source_register(&sid2_device);
    }
    if (val >= 2) {
        sid3_list_item = io_source_register(&sid3_device);
    }
    if (val >= 3) {
        sid4_list_item = io_source_register(&sid4_device);
    }
}

void sound_machine_prevent_clk_overflow(sound_t *psid, CLOCK sub)
{
    sid_sound_machine_prevent_clk_overflow(psid, sub);
}

char *sound_machine_dump_state(sound_t *psid)
{
    return sid_sound_machine_dump_state(psid);
}

void sound_machine_enable(int enable)
{
    sid_sound_machine_enable(enable);
}
