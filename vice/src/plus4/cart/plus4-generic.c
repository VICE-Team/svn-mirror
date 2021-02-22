
/*
 * plus4cart.c -- Plus4 generic cartridge handling.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include "cartridge.h"
#include "cartio.h"
#include "plus4cart.h"
#include "plus4mem.h"

#include "plus4-generic.h"

uint8_t generic_c1lo_read(uint16_t addr)
{
    return extromlo2[addr & 0x3fff];
}

uint8_t generic_c1hi_read(uint16_t addr)
{
    return extromhi2[addr & 0x3fff];
}
