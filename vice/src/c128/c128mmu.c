/*
 * c128mmu.c
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

#include "c128mem.h"
#include "c128mmu.h"
#include "c64mem.h"
#include "cmdline.h"
#include "kbd.h"
#include "log.h"
#include "mem.h"
#include "resources.h"

/* #define DEBUG_MMU */

#ifdef DEBUG_MMU
#define DEBUG(x) printf x
#else
#define DEBUG(x)
#endif

/* MMU register.  */
BYTE mmu[11];

/* Memory configuration.  */
static int chargen_in;
static int basic_lo_in;
static int basic_hi_in;
static int kernal_in;
static int editor_in;
static int io_in;

/* State of the 40/80 column key.  */
static int mmu_column4080_key = 1;

/* ------------------------------------------------------------------------- */

static int set_column4080_key(resource_value_t v)
{
    mmu_column4080_key = (int)v;
    return 0;
}

static resource_t resources[] = {
    { "40/80ColumnKey", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &mmu_column4080_key, set_column4080_key },
    { NULL }
};

int mmu_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-40col", SET_RESOURCE, 0, NULL, NULL, "40/80ColumnKey",
      (resource_value_t) 1,
      NULL, "Activate 40 column mode" },
    { "-80col", SET_RESOURCE, 0, NULL, NULL, "40/80ColumnKey",
      (resource_value_t) 0,
      NULL, "Activate 80 comumn mode" },
    { NULL }
};

int mmu_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static void mmu_toggle_column4080_key(void)
{
    mmu_column4080_key = !mmu_column4080_key;
    resources_set_value("40/80ColumnKey",
                        (resource_value_t)mmu_column4080_key);
    log_message(LOG_DEFAULT, "40/80 column key %s.",
                (mmu_column4080_key) ? "released" : "pressed");
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 mmu_read(ADDRESS addr)
{
    addr &= 0xff;

    if (addr < 0xb) {
        if (addr == 5) {
            /* 0x80 = 40/80 key released.  */
            return (mmu[5] & 0x7f) | (mmu_column4080_key ? 0x80 : 0);
        } else {
            return mmu[addr];
        }
    } else {
        return 0xf;
    }
}

void REGPARM2 mmu_store(ADDRESS address, BYTE value)
{
    address &= 0xff;

    if (address < 0xb) {
        mmu[address] = value;

        switch (address) {
          case 0:
            /* Configuration register (CR).  */
            {
                io_in = !(value & 0x1);
                basic_lo_in = !(value & 0x2);
                basic_hi_in = !(value & 0xc);
                kernal_in = chargen_in = editor_in = !(value & 0x30);
                mem_set_ram_bank(value);
            }
            break;
          case 6:
            /* RAM configuration register (RCR).  */
            mem_set_ram_config(value);
            break;
          case 5:
            value = (value & 0x7f) | 0x30;
            if ((value & 0x41) != 0x01)
                log_error(LOG_ERR,
                          "MMU: Attempted accessing unimplemented mode: $D505 <- $%02X.",
                          value);
            break;
          case 7:
          case 8:
          case 9:
          case 10:
            page_zero = (ram + (mmu[0x8] & 0x1 ? 0x10000 : 0x00000)
                         + (mmu[0x7] << 8));
            page_one = (ram + (mmu[0xa] & 0x1 ? 0x10000 : 0x00000)
                        + (mmu[0x9] << 8));
            break;
        }

        mem_update_config(((basic_lo_in) ? 1 : 0) | ((basic_hi_in) ? 2 : 0)
                          | ((kernal_in) ? 4 : 0) | ((mmu[0] & 0x40) ? 8 : 0)
                          | ((io_in) ? 16 : 0));
    }
}

/* $FF00 - $FFFF: RAM or Kernal, with MMU at $FF00 - $FF04.  */
BYTE REGPARM1 mmu_ffxx_read(ADDRESS addr)
{
    if (addr == 0xff00)
        return mmu[0];
    else if (kernal_in)
        return read_kernal(addr);
    else
        return read_top_shared(addr);
}

void REGPARM2 mmu_ffxx_store(ADDRESS addr, BYTE value)
{
    if (addr == 0xff00)
        mmu_store(0, value);
    else if (addr <= 0xff04)
        mmu_store(0, mmu[addr & 0xf]);
    else
        store_top_shared(addr, value);
}

/* ------------------------------------------------------------------------- */

void mmu_reset(void)
{
    /* FIXME?  Is this the real configuration?  */
    basic_lo_in = basic_hi_in = kernal_in = editor_in = 1;
    io_in = 1;
    chargen_in = 0;

    kbd_register_column4080_key(mmu_toggle_column4080_key);
}

