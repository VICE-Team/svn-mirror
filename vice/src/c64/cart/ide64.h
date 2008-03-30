/*
 * ide64.h - Cartridge handling, IDE64 cart.
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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

#ifndef _IDE64_H
#define _IDE64_H

#include "types.h"

extern BYTE REGPARM1 ide64_io1_read(WORD addr);
extern void REGPARM2 ide64_io1_store(WORD addr, BYTE value);

extern int ide64_resources_init(void);
extern int ide64_cmdline_options_init(void);

extern void ide64_config_init(void);
extern void ide64_config_setup(BYTE *rawcart);
extern int ide64_bin_attach(const char *filename, BYTE *rawcart);
extern char *ide64_image_file;
extern void ide64_detach(void);

#endif

