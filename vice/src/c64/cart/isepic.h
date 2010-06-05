/*
 * isepic.h - ISEPIC cart emulation.
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

#ifndef VICE_ISEPIC_H
#define VICE_ISEPIC_H

#include "types.h"

extern int isepic_switch;
extern int isepic_cart_enabled(void);
extern int isepic_freeze_allowed(void);

extern int isepic_resources_init(void);
extern int isepic_cmdline_options_init(void);

extern BYTE REGPARM1 isepic_romh_read(WORD addr);
extern void REGPARM2 isepic_romh_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 isepic_page_read(WORD addr);
extern void REGPARM2 isepic_page_store(WORD addr, BYTE byte);

extern int isepic_bin_attach(const char *filename, BYTE *rawcart);
extern int isepic_bin_save(const char *filename);
extern int isepic_crt_attach(FILE *fd, BYTE *rawcart);
extern int isepic_crt_save(const char *filename);
extern void isepic_detach(void);

#endif
