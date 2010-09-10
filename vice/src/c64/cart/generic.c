/*
 * generic.c - Cartridge handling, generic carts.
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

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64mem.h"
#include "cartridge.h"
#include "crt.h"
#include "generic.h"
#include "types.h"
#include "util.h"

/*
    the default cartridge works like this:

    1 banking register (for ROM only)
    - 8k ROM banks
    - 8k RAM may be enabled at ROML

    The mappings of the carts supported are as follows:

    size   type   roml          romh
    ----   ----   ----          ----
     4k    ulti   n/a           $F000-$FFFF
     8k    norm   $8000-$9FFF   n/a
     8k    ulti   n/a           $E000-$FFFF
    16k    norm   $8000-$9FFF   $A000-$BFFF
    16k    ulti   $8000-$9FFF   $E000-$FFFF

*/

/* #define DBGGENERIC */

#ifdef DBGGENERIC
#define DBG(x) printf x
#else
#define DBG(x)
#endif

/* ---------------------------------------------------------------------*/

static const c64export_resource_t export_res_8kb = {
    "Generic 8KB", 1, 0, NULL, NULL, CARTRIDGE_GENERIC_8KB
};

static const c64export_resource_t export_res_16kb = {
    "Generic 16KB", 1, 1, NULL, NULL, CARTRIDGE_GENERIC_16KB
};

static c64export_resource_t export_res_ultimax = {
    "Generic Ultimax", 0, 1, NULL, NULL, CARTRIDGE_ULTIMAX
};

/* ---------------------------------------------------------------------*/

void generic_8kb_config_init(void)
{
    cartridge_config_changed(0, 0, CMODE_READ);
}

void generic_16kb_config_init(void)
{
    cartridge_config_changed(1, 1, CMODE_READ);
}

void generic_ultimax_config_init(void)
{
    cartridge_config_changed(3, 3, CMODE_READ);
}

void generic_8kb_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    cartridge_config_changed(0, 0, CMODE_READ);
}

void generic_16kb_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x2000);
    memcpy(romh_banks, &rawcart[0x2000], 0x2000);
    cartridge_config_changed(1, 1, CMODE_READ);
}

void generic_ultimax_config_setup(BYTE *rawcart)
{
    memcpy(&roml_banks[0x0000], &rawcart[0x0000], 0x2000);
    memcpy(&romh_banks[0x0000], &rawcart[0x2000], 0x2000);
    cartridge_config_changed(3, 3, CMODE_READ);
}

int generic_8kb_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x2000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    if (c64export_add(&export_res_8kb) < 0) {
        return -1;
    }

    return 0;
}

int generic_16kb_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    if (c64export_add(&export_res_16kb) < 0) {
        return -1;
    }

    return 0;
}

int generic_ultimax_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x4000, UTIL_FILE_LOAD_SKIP_ADDRESS) < 0) {
        return -1;
    }

    if (c64export_add(&export_res_ultimax) < 0) {
        return -1;
    }

    return 0;
}

/*
    returns -1 on error, else a positive CRT ID
*/
int generic_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int crttype = -1;

    export_res_ultimax.game = 0;

    if (fread(chipheader, 0x10, 1, fd) < 1) {
        return -1;
    }

    DBG(("chip1 at %02x len %02x\n", chipheader[0xc], chipheader[0xe]));
    if (chipheader[0xc] == 0x80 && chipheader[0xe] != 0 && chipheader[0xe] <= 0x40) {
        if (fread(rawcart, chipheader[0xe] << 8, 1, fd) < 1) {
            return -1;
        }
        crttype = (chipheader[0xe] <= 0x20) ? CARTRIDGE_GENERIC_8KB : CARTRIDGE_GENERIC_16KB;
        DBG(("type %d\n", crttype));
        /* try to read next CHIP header in case of 16k Ultimax cart */
        if (fread(chipheader, 0x10, 1, fd) < 1) {
        DBG(("type %d (generic game)\n", crttype));
            if (crttype == CARTRIDGE_GENERIC_8KB) {
                if (c64export_add(&export_res_8kb) < 0) {
                    return -1;
                }
            } else {
                if (c64export_add(&export_res_16kb) < 0) {
                    return -1;
                }
            }
            return crttype;
        } else {
            export_res_ultimax.game = 1;
        }
        DBG(("chip2 at %02x len %02x\n", chipheader[0xc], chipheader[0xe]));
    }

    if (chipheader[0xc] >= 0xe0 && chipheader[0xe] != 0 && (chipheader[0xe] + chipheader[0xc]) == 0x100) {
        if (fread(rawcart + ((chipheader[0xc] << 8) & 0x3fff), chipheader[0xe] << 8, 1, fd) < 1) {
            return -1;
        }

        crttype = CARTRIDGE_ULTIMAX;

        if (c64export_add(&export_res_ultimax) < 0) {
            return -1;
        }

        return crttype;
    }

    return -1;
}

void generic_8kb_detach(void)
{
    c64export_remove(&export_res_8kb);
}

void generic_16kb_detach(void)
{
    c64export_remove(&export_res_16kb);
}

void generic_ultimax_detach(void)
{
    c64export_remove(&export_res_ultimax);
}

/* ---------------------------------------------------------------------*/

/* ROML read - mapped to 8000 in 8k,16k,ultimax */
BYTE REGPARM1 generic_roml_read(WORD addr)
{
    if (export_ram) {
        return export_ram0[addr & 0x1fff];
    }

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

/* ROML store - mapped to 8000 in ultimax mode */
void REGPARM2 generic_roml_store(WORD addr, BYTE value)
{
    if (export_ram) {
        export_ram0[addr & 0x1fff] = value;
    }
}

/* ROMH read - mapped to A000 in 16k, to E000 in ultimax */
BYTE REGPARM1 generic_romh_read(WORD addr)
{
    return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
}

BYTE generic_peek_mem(WORD addr)
{
    if (addr >= 0x8000 && addr <= 0x9fff) {
        if (export_ram) {
            return export_ram0[addr & 0x1fff];
        }
        return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
    }

    if (!export.exrom && export.game) {
        if (addr >= 0xe000 && addr <= 0xffff) {
            return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
        }
    } else {
        if (addr >= 0xa000 && addr <= 0xbfff) {
            return romh_banks[(addr & 0x1fff) + (romh_bank << 13)];
        }
    }

    return ram_read(addr);
}
