/*
 * delaep256.c - Cartridge handling, Dela EP256 cart.
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
#include <stdlib.h>
#include <string.h>

#include "c64cart.h"
#include "c64cartmem.h"
#include "c64export.h"
#include "delaep256.h"
#include "types.h"
#include "vicii-phi1.h"

/* This eprom system by DELA is similair to the EP64. It can handle
   what the EP64 can handle, plus the following features :

   - Alternate rom at $8000

   The system uses 33 8kb eproms, of which the first is used for
   the main menu. 32 8kb banks are used for above named features.

   Because of the fact that this system supports switching in a
   different eprom at $8000 (followed by a reset) it is possible
   to place other 8kb carts in the eproms and use them.
 */

static const c64export_resource_t export_res = {
    "Dela EP256", 1, 0
};

void REGPARM2 delaep256_io1_store(WORD addr, BYTE value)
{
  BYTE bank, config;

  /* D7 switches off EXROM */
  config = (value & 0x80) ? 2 : 0;

  cartridge_config_changed(config, config, CMODE_WRITE);

  bank=((0x30-(value&0x30))>>1)+(value&7)+1;

  if (bank<1 || bank>32)
    bank=0;

  cartridge_romlbank_set(bank);
}

void delaep256_config_init(void)
{
  cartridge_config_changed(0, 0, CMODE_READ);
  cartridge_romlbank_set(0);
}

void delaep256_config_setup(BYTE *rawcart)
{
  cartridge_config_changed(0, 0, CMODE_READ);
  cartridge_romlbank_set(0);
}

int delaep256_crt_attach(FILE *fd, BYTE *rawcart)
{
  WORD chip;
  WORD size;
  BYTE chipheader[0x10];

  memset(roml_banks, 0xff, 0x42000);

  while (1)
  {
    if (fread(chipheader, 0x10, 1, fd)<1)
      break;

    chip=(chipheader[0x0a]<<8)+chipheader[0x0b];
    size=(chipheader[0x0e]<<8)+chipheader[0x0f];

    if (size!=0x2000)
      return -1;

    if (chip > 32)
      return -1;

    if (fread(roml_banks + (chip<<13), 0x2000, 1, fd)<1)
      return -1;
  }

  if (c64export_add(&export_res) < 0)
    return -1;

  return 0;
}

void delaep256_detach(void)
{
  c64export_remove(&export_res);
}
