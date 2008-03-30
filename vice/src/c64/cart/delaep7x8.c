/*
 * delaep7x8.c - Cartridge handling, Dela EP7x8kb cart.
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
#include "delaep7x8.h"
#include "types.h"
#include "vicii-phi1.h"

/* This eprom system by DELA seems to be a bit more advanced
   than the EP64 and EP128. It can handle what the EP64 can
   handle, plus the following features :

   - Alternate rom at $8000
   - Alternate basic at $A000
   - Extended basic
   - Basic programs
   - Simulated 16kb roms
   - Kernel replacement

   The system uses 8 8kb eproms, of which the first is used for
   the base menu. 7 8kb banks are used for above named features.

   Because of the fact that this system supports switching in a
   different eprom at $8000 (followed by a reset) it is possible
   to place generic 8kb carts (games/tools) in the eproms and
   use them.
 */

static BYTE delaep7x8_reg;

static const c64export_resource_t export_res = {
    "Dela EP7x8", 1, 0, 1, 0
};

void REGPARM2 delaep7x8_io1_store(WORD addr, BYTE value)
{
  BYTE bank, config, test_value;

  /* Each bit of the register set to low activates a
     respective EPROM, $FF switches off EXROM */
  delaep7x8_reg=value;

  config = (value==0xff) ? 2 : 0;

  cartridge_config_changed(config, config, CMODE_WRITE);

  bank = 0;
  test_value = (~value);
  while (test_value!=0)
  {
    bank++;
    test_value=(test_value>>1);
  }
  if (bank!=0)
    cartridge_romlbank_set(bank-1);
}

/* I'm unsure whether the register is write-only
   or what. */
BYTE REGPARM1 delaep7x8_io1_read(WORD addr)
{
  return delaep7x8_reg;
}

void delaep7x8_config_init(void)
{
  delaep7x8_reg=0xfe;
  cartridge_config_changed(0, 0, CMODE_READ);
  cartridge_romlbank_set(0);
}

void delaep7x8_config_setup(BYTE *rawcart)
{
  delaep7x8_reg=0xfe;
  cartridge_config_changed(0, 0, CMODE_READ);
  cartridge_romlbank_set(0);
}

int delaep7x8_crt_attach(FILE *fd, BYTE *rawcart)
{
  WORD chip;
  WORD size;
  BYTE chipheader[0x10];

  memset(roml_banks, 0xff, 0x10000);

  while (1)
  {
    if (fread(chipheader, 0x10, 1, fd)<1)
      break;

    chip=(chipheader[0x0a]<<8)+chipheader[0x0b];
    size=(chipheader[0x0e]<<8)+chipheader[0x0f];

    if (size!=0x2000)
      return -1;

    if (chip > 7)
      return -1;

    if (fread(roml_banks + (chip<<13), 0x2000, 1, fd)<1)
      return -1;
  }

  if (c64export_add(&export_res) < 0)
    return -1;

  return 0;
}

void delaep7x8_detach(void)
{
  c64export_remove(&export_res);
}
