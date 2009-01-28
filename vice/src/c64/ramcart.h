/*
 * ramcart.h - RAMCART emulation.
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

#ifndef VICE_RAMCART_H
#define VICE_RAMCART_H

#include "types.h"

struct snapshot_s;

extern void ramcart_init(void);
extern int ramcart_resources_init(void);
extern void ramcart_resources_shutdown(void);
extern int ramcart_cmdline_options_init(void);

extern void ramcart_init_config(void);
extern void ramcart_reset(void);
extern void ramcart_shutdown(void);
extern BYTE REGPARM1 ramcart_reg_read(WORD addr);
extern void REGPARM2 ramcart_reg_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 ramcart_window_read(WORD addr);
extern void REGPARM2 ramcart_window_store(WORD addr, BYTE byte);
extern BYTE REGPARM1 ramcart_roml_read(WORD addr);
extern void REGPARM2 ramcart_roml_store(WORD addr, BYTE byte);
extern int ramcart_read_snapshot_module(struct snapshot_s *s);
extern int ramcart_write_snapshot_module(struct snapshot_s *s);
extern int ramcart_attach(const char *filename, BYTE *rawcart);

extern int ramcart_enabled;
extern int ramcart_readonly;

#endif
