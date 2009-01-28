/*
 * mmc64.h - Cartridge handling, MMC64 cart.
 *
 * Written by
 *  Markus Stehr <bastetfurry@ircnet.de>
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

#ifndef VICE_MMC64_H
#define VICE_MMC64_H

#include "types.h"

extern int mmc64_enabled;
extern int mmc64_clockport_enabled;
extern int mmc64_hw_clockport;

extern void mmc64_init_card_config(void);
extern void mmc64_clockport_enable_store(BYTE value);
extern void REGPARM2 mmc64_io2_store(WORD addr, BYTE value);
extern BYTE REGPARM1 mmc64_io2_read(WORD addr);
extern BYTE REGPARM1 mmc64_roml_read(WORD addr);
extern void REGPARM2 mmc64_roml_store(WORD addr, BYTE byte);

extern int mmc64_resources_init(void);
extern void mmc64_resources_shutdown(void);
extern int mmc64_cmdline_options_init(void);
extern void mmc64_init(void);
extern void mmc64_shutdown(void);
extern void mmc64_reset(void);

#endif
