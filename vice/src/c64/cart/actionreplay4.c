/*
 * actionreplay4.h - Cartridge handling, Action Replay 4 cart.
 * and it's clones. post AR3 (16K) and pre AR4.1 hardware.
 *
 * (w)2008 Groepaz/Hitmen
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

#include "actionreplay4.h"
#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "c64io.h"
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"

static const c64export_resource_t export_res = {
    "Action Replay 4", 1, 1
};

static unsigned int ar_active;

/*
$de00 control register
bit 0: Eprom banking bit 0 (bank address 13)
bit 1: controls the GAME line (0 sets GAME low, 1 sets GAME high)
bit 2: Freeze-end bit (disables the register and hides any rom bank)
bit 3: controls the Exrom line (1 sets EXROM low, 0 sets EXROM high)
bit 4: Eprom banking bit 1 (bank address 14)
bit 5 to 7: unused.
*/

void REGPARM2 actionreplay4_io1_store(WORD addr, BYTE value)
{
    BYTE exrom, bank, conf, game, disable;

    game    = (value >> 1) & 1;
    disable = (value >> 2) & 1;
    exrom   = (value >> 3) & 1;
    bank    = (value >> 4) & 1;
    bank    = (value & 1) | (bank<<1);
    conf = (bank << 3) | ((exrom ^ 1) << 1) | ((game ^ 1)<<0);
    
    if (ar_active) {
        cartridge_config_changed((BYTE)(conf&3), conf, CMODE_WRITE);
        if (disable)
            ar_active = 0;
    }
}

BYTE REGPARM1 actionreplay4_io2_read(WORD addr)
{
    if (!ar_active)
        return vicii_read_phi1();

    io_source = IO_SOURCE_ACTION_REPLAY4;

    switch (roml_bank) {
      case 0:
        return roml_banks[addr & 0x1fff];
      case 1:
        return roml_banks[(addr & 0x1fff) + 0x2000];
      case 2:
        return roml_banks[(addr & 0x1fff) + 0x4000];
      case 3:
        return roml_banks[(addr & 0x1fff) + 0x6000];
    }

    io_source = IO_SOURCE_NONE;

    return 0;
}

BYTE REGPARM1 actionreplay4_roml_read(WORD addr)
{
#if 0
    int test=(addr & 0x1fff) + (roml_bank << 13);

    return roml_banks[test];
#endif

    return roml_banks[(addr & 0x1fff) + (roml_bank << 13)];
}

void actionreplay4_freeze(void)
{
    ar_active = 1;
    cartridge_config_changed(3, 3, CMODE_READ);
}

void actionreplay4_config_init(void)
{
    ar_active = 1;
    cartridge_config_changed(8, 8, CMODE_READ);
}

void actionreplay4_reset(void)
{
    ar_active = 1;
}

void actionreplay4_config_setup(BYTE *rawcart)
{
    memcpy(roml_banks, rawcart, 0x8000);
    memcpy(romh_banks, rawcart, 0x8000);

    cartridge_config_changed(8, 8, CMODE_READ);
}

int actionreplay4_bin_attach(const char *filename, BYTE *rawcart)
{
    if (util_file_load(filename, rawcart, 0x8000,
        UTIL_FILE_LOAD_SKIP_ADDRESS) < 0)
        return -1;

    if (c64export_add(&export_res) < 0)
        return -1;

    return 0;
}

int actionreplay4_crt_attach(FILE *fd, BYTE *rawcart)
{
    BYTE chipheader[0x10];
    int i;

    for (i = 0; i <= 3; i++) {
        if (fread(chipheader, 0x10, 1, fd) < 1)
            return -1;

        if (chipheader[0xb] > 3)
            return -1;

        if (fread(&rawcart[chipheader[0xb] << 13], 0x2000, 1, fd) < 1)
            return -1;
    }

    if (c64export_add(&export_res) < 0)
        return -1;

    return 0;
}

void actionreplay4_detach(void)
{
    c64export_remove(&export_res);
}
