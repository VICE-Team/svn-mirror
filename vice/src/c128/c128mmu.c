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
#include "interrupt.h"
#include "kbd.h"
#include "log.h"
#include "mem.h"
#include "resources.h"
#include "vdc.h"
#include "vicii.h"
#include "z80.h"
#include "z80mem.h"

/* #define MMU_DEBUG */

/* MMU register.  */
static BYTE mmu[11];

/* Memory configuration.  */
static int chargen_in;
static int basic_lo_in;
static int basic_hi_in;
static int kernal_in;
static int editor_in;
static int io_in;

/* State of the 40/80 column key.  */
static int mmu_column4080_key = 1;

/* Logging goes here.  */
static log_t mmu_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

static int set_column4080_key(resource_value_t v)
{
    mmu_column4080_key = (int)v;

#ifdef HAS_SINGLE_CANVAS
    vdc_set_set_canvas_refresh(mmu_column4080_key ? 0 : 1);
    vic_ii_set_set_canvas_refresh(mmu_column4080_key ? 1 : 0);
#endif
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
    log_message(mmu_log, "40/80 column key %s.",
                (mmu_column4080_key) ? "released" : "pressed");
}

static void mmu_switch_cpu(int value)
{
    if (value) {
        log_message(mmu_log, "Switching to 8502 CPU.");
        z80_trigger_dma();
    } else {
        log_message(mmu_log, "Switching to Z80 CPU.");
        maincpu_trigger_dma();
    }
}

static void mmu_set_ram_bank(BYTE value)
{
    /* (We handle only 128K here.)  */
    ram_bank = ram + (((long) value & 0x40) << 10);
#ifdef MMU_DEBUG
    log_message(mmu_log, "Set RAM bank %i.", (value & 0x40) ? 1 : 0);
#endif
}

/* ------------------------------------------------------------------------- */

BYTE REGPARM1 mmu_read(ADDRESS addr)
{
    addr &= 0xff;

#ifdef MMU_DEBUG
    log_message(mmu_log, "MMU READ $%x.", addr);
#endif

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

#ifdef MMU_DEBUG
    log_message(mmu_log, "MMU STORE $%x <- #$%x.", address, value);
#endif

    if (address < 0xb) {
        BYTE oldvalue;

        oldvalue = mmu[address];
        mmu[address] = value;

        switch (address) {
          case 0:
            /* Configuration register (CR).  */
            io_in = !(value & 0x1);
            basic_lo_in = !(value & 0x2);
            basic_hi_in = !(value & 0xc);
            kernal_in = chargen_in = editor_in = !(value & 0x30);
            mmu_set_ram_bank(value);
            break;
          case 5:
            value = (value & 0x7f) | 0x30;
            if ((value & 1) ^ (oldvalue & 1))
                mmu_switch_cpu(value & 1);
            break;
          case 6:
            /* RAM configuration register (RCR).  */
            mem_set_ram_config(value);
            break;
          case 7:
          case 8:
          case 9:
          case 10:
            page_zero = (ram + (mmu[0x8] & 0x1 ? 0x10000 : 0x00000)
                         + (mmu[0x7] << 8));
            page_one = (ram + (mmu[0xa] & 0x1 ? 0x10000 : 0x00000)
                        + (mmu[0x9] << 8));
#ifdef MMU_DEBUG
            log_message(mmu_log, "PAGE ZERO %05x PAGE ONE %05x",
                (mmu[0x8] & 0x1 ? 0x10000 : 0x00000) + (mmu[0x7] << 8),
                (mmu[0xa] & 0x1 ? 0x10000 : 0x00000) + (mmu[0x9] << 8));
#endif
            break;
        }

        mem_update_config(((basic_lo_in) ? 1 : 0) | ((basic_hi_in) ? 2 : 0)
                          | ((kernal_in) ? 4 : 0) | ((mmu[0] & 0x40) ? 8 : 0)
                          | ((io_in) ? 16 : 0));
        z80mem_update_config(((io_in) ? 1 : 0) | ((mmu[0] & 0x40) ? 2 : 0));
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

void mmu_init(void)
{
    if (mmu_log == LOG_ERR)
        mmu_log = log_open("MMU");

    set_column4080_key((resource_value_t)mmu_column4080_key);

    mmu[5] = 0;
}

void mmu_reset(void)
{
    /* FIXME?  Is this the real configuration?  */
    basic_lo_in = basic_hi_in = kernal_in = editor_in = 1;
    io_in = 1;
    chargen_in = 0;

    kbd_register_column4080_key(mmu_toggle_column4080_key);
}

