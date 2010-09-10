/*
 * retroreplay.h - Cartridge handling, Retro Replay cart.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#ifndef VICE_RETROREPLAY_H
#define VICE_RETROREPLAY_H

#include <stdio.h>

#include "types.h"

extern BYTE REGPARM1 retroreplay_roml_read(WORD addr);
extern void REGPARM2 retroreplay_roml_store(WORD addr, BYTE value);
extern int REGPARM2 retroreplay_roml_no_ultimax_store(WORD addr, BYTE value);
extern BYTE REGPARM1 retroreplay_romh_read(WORD addr);
extern BYTE REGPARM1 retroreplay_peek_mem(WORD addr);

extern void retroreplay_freeze(void);
extern int retroreplay_freeze_allowed(void);

extern void retroreplay_config_init(void);
extern void retroreplay_reset(void);
extern void retroreplay_config_setup(BYTE *rawcart);
extern int retroreplay_bin_attach(const char *filename, BYTE *rawcart);
extern int retroreplay_crt_attach(FILE *fd, BYTE *rawcart, const char *filename);
extern void retroreplay_detach(void);

extern unsigned int rr_active;
extern unsigned int rr_clockport_enabled;
extern unsigned int reu_mapping;

extern int retroreplay_cmdline_options_init(void);
extern int retroreplay_resources_init(void);

#endif
