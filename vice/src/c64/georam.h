/*
 * georam.h - GEORAM emulation.
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

#ifndef _GEORAM_H
#define _GEORAM_H

#include "types.h"

struct snapshot_s;

extern void georam_init(void);
extern int georam_resources_init(void);
extern void georam_resources_shutdown(void);
extern int georam_cmdline_options_init(void);

extern void georam_reset(void);
extern void georam_shutdown(void);
extern BYTE REGPARM1 georam_window_read(WORD addr);
extern void REGPARM2 georam_reg_store(WORD addr, BYTE byte);
extern void REGPARM2 georam_window_store(WORD addr, BYTE byte);
extern int georam_read_snapshot_module(struct snapshot_s *s);
extern int georam_write_snapshot_module(struct snapshot_s *s);

extern int georam_enabled;

#endif

