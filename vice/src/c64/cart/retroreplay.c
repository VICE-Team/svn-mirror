/*
 * retroreplay.c - Cartridge handling, Retro Replay cart.
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "retroreplay.h"
#include "reu.h"
#include "types.h"
#include "utils.h"


/* Only one write access is allowed.  */
static unsigned int write_once;

/* RAM bank switching allowed.  */
static unsigned int allow_bank;

/* Freeze is disallowed.  */
static int no_freeze;

/* REU compatibility mapping.  */
static unsigned int reu_mapping;


BYTE REGPARM1 retroreplay_io1_read(ADDRESS addr)
{
    switch (addr & 0xff) {
      case 0:
      case 1:
        return ((roml_bank & 3) << 3) | ((roml_bank & 4) << 5) | allow_bank
               | reu_mapping;
      default:
        if (reu_mapping) {
            if (export_ram) {
                if (allow_bank) {
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
                    return export_ram0[0x1e00 + (addr & 0xff)];
                }
            }

            switch (roml_bank) {
              case 0:
                return roml_banks[addr & 0x1fff];
              case 1:
                return roml_banks[(addr & 0x1fff) + 0x2000];
              case 2:
                return roml_banks[(addr & 0x1fff) + 0x4000];
              case 3:
                return roml_banks[(addr & 0x1fff) + 0x6000];
              case 4:
                return roml_banks[(addr & 0x1fff) + 0x8000];
              case 5:
                return roml_banks[(addr & 0x1fff) + 0xa000];
              case 6:
                return roml_banks[(addr & 0x1fff) + 0xc000];
              case 7:
                return roml_banks[(addr & 0x1fff) + 0xe000];
            }
        }
        return 0;
    }
}

void REGPARM2 retroreplay_io1_store(ADDRESS addr, BYTE value)
{
    switch (addr & 0xff) {
      case 0:
        cartridge_config_changed(value);
        romh_bank = roml_bank = ((value >> 3) & 3) | ((value >> 5) & 4);
        break;
      case 1:
        romh_bank = roml_bank = ((value >> 3) & 3) | ((value >> 5) & 4);
        if (write_once == 0) {
            allow_bank = value & 2;
            reu_mapping = value & 0x40;
            write_once = 1;
        }
        break;
      default:
        if (reu_mapping) {
            if (export_ram) {
                if (allow_bank) {
                    switch (roml_bank & 3) {
                      case 0:
                        export_ram0[0x1e00 + (addr & 0xff)] = value;
                      case 1:
                        export_ram0[0x3e00 + (addr & 0xff)] = value;
                      case 2:
                        export_ram0[0x5e00 + (addr & 0xff)] = value;
                      case 3:
                        export_ram0[0x7e00 + (addr & 0xff)] = value;
                    }
                } else {
                    export_ram0[0x1e00 + (addr & 0xff)] = value;
                }
            }
        }
    }
}

BYTE REGPARM1 retroreplay_io2_read(ADDRESS addr)
{
    if (reu_mapping) {
        if (reu_enabled)
            return reu_read((ADDRESS)(addr & 0x0f));
    } else {
        if (export_ram) {
            if (allow_bank) {
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
                return export_ram0[0x1f00 + (addr & 0xff)];
            }
        }

        switch (roml_bank) {
          case 0:
            return roml_banks[addr & 0x1fff];
          case 1:
            return roml_banks[(addr & 0x1fff) + 0x2000];
          case 2:
            return roml_banks[(addr & 0x1fff) + 0x4000];
          case 3:
            return roml_banks[(addr & 0x1fff) + 0x6000];
          case 4:
            return roml_banks[(addr & 0x1fff) + 0x8000];
          case 5:
            return roml_banks[(addr & 0x1fff) + 0xa000];
          case 6:
            return roml_banks[(addr & 0x1fff) + 0xc000];
          case 7:
            return roml_banks[(addr & 0x1fff) + 0xe000];
        }
    }
    return 0;
}

void REGPARM2 retroreplay_io2_store(ADDRESS addr, BYTE value)
{
    if (reu_mapping) {
        if (reu_enabled)
            reu_store((ADDRESS)(addr & 0x0f), value);
    } else {
        if (export_ram) {
            if (allow_bank) {
                switch (roml_bank & 3) {
                  case 0:
                    export_ram0[0x1f00 + (addr & 0xff)] = value;
                  case 1:
                    export_ram0[0x3f00 + (addr & 0xff)] = value;
                  case 2:
                    export_ram0[0x5f00 + (addr & 0xff)] = value;
                  case 3:
                    export_ram0[0x7f00 + (addr & 0xff)] = value;
                }
            } else {
                export_ram0[0x1f00 + (addr & 0xff)] = value;
            }
        }
    }
}

BYTE REGPARM1 retroreplay_roml_read(ADDRESS addr)
{
    if (export_ram) {
        if (1) {
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
        } else {
            return export_ram0[addr & 0x1fff];
        }
    }

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void REGPARM2 retroreplay_roml_store(ADDRESS addr, BYTE value)
{
    if (export_ram) {
        if (1) {
            switch (roml_bank & 3) {
              case 0:
                export_ram0[addr & 0x1fff] = value;
              case 1:
                export_ram0[(addr & 0x1fff) + 0x2000] = value;
              case 2:
                export_ram0[(addr & 0x1fff) + 0x4000] = value;
              case 3:
                export_ram0[(addr & 0x1fff) + 0x6000] = value;
            }
        } else {
            export_ram0[addr & 0x1fff] = value;
        }
    }
}

void retroreplay_freeze(void)
{
    cartridge_config_changed(35);
}

int retroreplay_freeze_allowed(void)
{
    if (no_freeze)
        return 0;
    return 1;
}

void retroreplay_config_init(void)
{
    cartridge_config_changed(0);
    write_once = 0;
    no_freeze = 0;
    reu_mapping = 0;
    allow_bank = 0;
}

void retroreplay_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x10000);
    memcpy(romh_banks, rawcart, 0x10000);
    cartridge_config_changed(0);
}

int retroreplay_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x10000,
        UTIL_FILE_LOAD_SKIP_ADDRESS) < 0)
        return -1;

    return 0;
}

