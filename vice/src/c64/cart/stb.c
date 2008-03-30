/*
 * stb.c - Cartridge handling, Structured Basic cart.
 *
 * Written by
 *  Walter Zimmer, adapted from kcs.c
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
#include <util.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "stb.h"
#include "types.h"


/* Stb cartridge uses io1 and roml */
static const c64export_resource_t export_res = {
    "Structured Basic", 1, 0, 1, 0
};

/* Structured Basic IO1 logic for the roml range $8000-$9fff
*
*  Any read/write access for IO/1 latch the lower two address bits:
*  - bit 1 selects bank 1 of the stb rom
*  - bit 0 and bit 1 set deactivate EXROM and expose the RAM
*/

static void stb_io (WORD addr)
{
    switch (addr & 0xff03) {

      /* normal config: bank 0 visible */
      case 0xde00:
      case 0xde01:
        cartridge_config_changed(0, 0, CMODE_READ);
        break; 

      /* bank 1 visible, gets copied to RAM during reset */
      case 0xde02:
        cartridge_config_changed(0x08, 0x08, CMODE_READ);
        break;

      /* RAM visible, which contains bank 1 */
      case 0xde03:
        cartridge_config_changed(2, 2, CMODE_READ);
        break;
    }
}

BYTE REGPARM1 stb_io1_read(WORD addr)
{
    stb_io(addr);
    return 0;
}

void REGPARM2 stb_io1_store(WORD addr, BYTE value)
{
    stb_io(addr);
}

void stb_config_init(void)
{
    /* turn on normal config: bank 0 */
    cartridge_config_changed(0, 0, CMODE_READ);
}

void stb_config_setup(BYTE *rawcart)
{
    /* copy banks 0 and 1 */
    memcpy(roml_banks, rawcart, 0x4000);

    /* turn on normal config: bank 0 */
    cartridge_config_changed(0, 0, CMODE_READ);
}

int stb_bin_attach(const char *filename, BYTE *rawcart)
{
    /* load file into cartridge address space */
    if (util_file_load(filename, rawcart, 0x4000,
        UTIL_FILE_LOAD_RAW) < 0)
        return -1;

    /* add export */
    if (c64export_add(&export_res) < 0)
        return -1;

    return 0;
}

void stb_detach(void)
{
    c64export_remove(&export_res);
}

