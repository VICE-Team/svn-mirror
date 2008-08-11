/*
 * c64dtvcart.c - C64 cartridge emulation stubs.
 *
 * Written by
 *  Daniel Kahlin <daniel@kahlin.net>
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

#include "c64cart.h"
#include "cartridge.h"

int mem_cartridge_type = 0;
unsigned int cart_ultimax_phi1 = 0;
unsigned int cart_ultimax_phi2 = 0;
int romh_bank = 0;
BYTE romh_banks[1];

int cartridge_save_image(const char *filename)
{
    return 0;
}

int cartridge_resources_init(void)
{
    return 0;
}

void cartridge_resources_shutdown(void)
{
}

int cartridge_cmdline_options_init(void)
{
    return 0;
}

int cartridge_attach_image(int type, const char *filename)
{
    return 0;
}

void cartridge_detach_image(void)
{
}

void cartridge_set_default(void)
{
}
void cartridge_init(void)
{
}

void cartridge_trigger_freeze(void)
{
}

void cartridge_trigger_freeze_nmi_only(void)
{
}

const char *cartridge_get_file_name(WORD addr_ignored)
{
    return 0; /* NULL */
}

