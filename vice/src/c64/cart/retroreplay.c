/*
 * retroreplay.c - Cartridge handling, Retro Replay cart.
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "retroreplay.h"
#include "reu.h"
#ifdef HAVE_TFE
#include "tfe.h"
#endif
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"


static const c64export_resource_t export_res = {
    "Retro Replay", 1, 1
};

/* Cart is activated.  */
unsigned int rr_active;
unsigned int rr_clockport_enabled;

/* Only one write access is allowed.  */
static unsigned int write_once;

/* RAM bank switching allowed.  */
static unsigned int allow_bank;

/* Freeze is disallowed.  */
static int no_freeze;

/* REU compatibility mapping.  */
static unsigned int reu_mapping;

BYTE REGPARM1 retroreplay_io1_read(WORD addr)
{
    if (rr_active) {
        switch (addr & 0xff) {
          case 0:
          case 1:
            io_source=IO_SOURCE_RR;
            return ((roml_bank & 3) << 3) | ((roml_bank & 4) << 5) | allow_bank
                   | reu_mapping;
          default:
#ifdef HAVE_TFE
            if (rr_clockport_enabled && tfe_enabled && tfe_as_rr_net && (addr&0xff)<0x10)
              return 0;
#endif
            if (reu_mapping) {
                if (export_ram) {
                    if (allow_bank) {
                        io_source=IO_SOURCE_RR;
                        switch (roml_bank & 3) {
                          case 0:
                            return export_ram0[0x1e00 + (addr & 0xff)];
                          case 1:
                            return export_ram0[0x3e00 + (addr & 0xff)];
                          case 2:
                            return export_ram0[0x5e00 + (addr & 0xff)];
                          case 3:
                            return export_ram0[0x7e00 + (addr & 0xff)];
                        }
                    } else {
                        io_source=IO_SOURCE_RR;
                        return export_ram0[0x1e00 + (addr & 0xff)];
                    }
                }
                io_source=IO_SOURCE_RR;
                return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
            }
            return 0;
        }
    } else {
        return vicii_read_phi1();
    }
}

void REGPARM2 retroreplay_io1_store(WORD addr, BYTE value)
{
    if (rr_active) {
        switch (addr & 0xff) {
          case 0:
            cartridge_config_changed(0, value, CMODE_WRITE);
            cartridge_romhbank_set(((value >> 3) & 3) | ((value >> 5) & 4));
            cartridge_romlbank_set(((value >> 3) & 3) | ((value >> 5) & 4));
            if (value & 4)
                rr_active = 0;
            break;
          case 1:
            if (write_once == 0) {
                cartridge_romhbank_set(((value >> 3) & 3) | ((value >> 5) & 4));
                cartridge_romlbank_set(((value >> 3) & 3) | ((value >> 5) & 4));
                allow_bank = value & 2;
                no_freeze = value & 4;
                reu_mapping = value & 0x40;
                rr_clockport_enabled = value & 1;
                write_once = 1;
            }
            break;
          default:
#ifdef HAVE_TFE
            if (rr_clockport_enabled && tfe_enabled && tfe_as_rr_net && (addr&0xff)<0x10)
              return;
#endif
            if (reu_mapping) {
                if (export_ram) {
                    if (allow_bank) {
                        switch (roml_bank & 3) {
                          case 0:
                            export_ram0[0x1e00 + (addr & 0xff)] = value;
                            break;
                          case 1:
                            export_ram0[0x3e00 + (addr & 0xff)] = value;
                            break;
                          case 2:
                            export_ram0[0x5e00 + (addr & 0xff)] = value;
                            break;
                          case 3:
                            export_ram0[0x7e00 + (addr & 0xff)] = value;
                            break;
                        }
                    } else {
                        export_ram0[0x1e00 + (addr & 0xff)] = value;
                    }
                }
            }
        }
    }
}

BYTE REGPARM1 retroreplay_io2_read(WORD addr)
{
    if (rr_active) {
        if (!reu_mapping) {
            if (export_ram) {
                if (allow_bank) {
                    io_source=IO_SOURCE_RR;
                    switch (roml_bank & 3) {
                      case 0:
                        return export_ram0[0x1f00 + (addr & 0xff)];
                      case 1:
                        return export_ram0[0x3f00 + (addr & 0xff)];
                      case 2:
                        return export_ram0[0x5f00 + (addr & 0xff)];
                      case 3:
                        return export_ram0[0x7f00 + (addr & 0xff)];
                    }
                } else {
                    io_source=IO_SOURCE_RR;
                    return export_ram0[0x1f00 + (addr & 0xff)];
                }
            }
            io_source=IO_SOURCE_RR;
            return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
        }
        return 0;
    } else {
        return vicii_read_phi1();
    }
}

void REGPARM2 retroreplay_io2_store(WORD addr, BYTE value)
{
    if (rr_active) {
        if (!reu_mapping) {
            if (export_ram) {
                if (allow_bank) {
                    switch (roml_bank & 3) {
                      case 0:
                        export_ram0[0x1f00 + (addr & 0xff)] = value;
                        break;
                      case 1:
                        export_ram0[0x3f00 + (addr & 0xff)] = value;
                        break;
                      case 2:
                        export_ram0[0x5f00 + (addr & 0xff)] = value;
                        break;
                      case 3:
                        export_ram0[0x7f00 + (addr & 0xff)] = value;
                        break;
                    }
                } else {
                    export_ram0[0x1f00 + (addr & 0xff)] = value;
                }
            }
        }
    }
}

BYTE REGPARM1 retroreplay_roml_read(WORD addr)
{
    if (export_ram) {
        switch (roml_bank & 3) {
          case 0:
            return export_ram0[addr & 0x1fff];
          case 1:
            return export_ram0[(addr & 0x1fff) + 0x2000];
          case 2:
            return export_ram0[(addr & 0x1fff) + 0x4000];
          case 3:
            return export_ram0[(addr & 0x1fff) + 0x6000];
        }
    }

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 retroreplay_roml_store(WORD addr, BYTE value)
{
    if (export_ram) {
        switch (roml_bank & 3) {
          case 0:
            export_ram0[addr & 0x1fff] = value;
            break;
          case 1:
            export_ram0[(addr & 0x1fff) + 0x2000] = value;
            break;
          case 2:
            export_ram0[(addr & 0x1fff) + 0x4000] = value;
            break;
          case 3:
            export_ram0[(addr & 0x1fff) + 0x6000] = value;
            break;
        }
    }
}

void retroreplay_freeze(void)
{
    rr_active = 1;
    cartridge_config_changed(35, 35, CMODE_READ);
}

int retroreplay_freeze_allowed(void)
{
    if (no_freeze)
        return 0;
    return 1;
}

void retroreplay_config_init(void)
{
    rr_active = 1;
    cartridge_config_changed(0, 0, CMODE_READ);
    write_once = 0;
    no_freeze = 0;
    reu_mapping = 0;
    allow_bank = 0;
}

void retroreplay_reset(void)
{
    rr_active = 1;
}

void retroreplay_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x10000);
    memcpy(romh_banks, rawcart, 0x10000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

int retroreplay_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x10000,
        UTIL_FILE_LOAD_SKIP_ADDRESS | UTIL_FILE_LOAD_FILL) < 0)
        return -1;

    if (c64export_add(&export_res) < 0)
        return -1;

    return 0;
}

void retroreplay_detach(void)
{
    c64export_remove(&export_res);
}
