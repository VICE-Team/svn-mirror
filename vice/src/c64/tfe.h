/*
 * tfe.h - TFE ("The final ethernet" emulation.
 *
 * Written by
 *  Spiro Trikaliotis <trik-news@gmx.de>
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

#ifdef HAVE_TFE 
#else
  #error TFE.H should not be included if HAVE_TFE is not defined!
#endif /* #ifdef HAVE_TFE */

#ifndef _TFE_H
#define _TFE_H

#include "types.h"

struct snapshot_s;

extern int tfe_enabled;

extern void tfe_init(void);
extern int tfe_resources_init(void);
extern int tfe_cmdline_options_init(void);

extern void tfe_reset(void);
extern void tfe_shutdown(void);
extern BYTE REGPARM1 tfe_read(WORD addr);
extern void REGPARM2 tfe_store(WORD addr, BYTE byte);
extern int tfe_read_snapshot_module(struct snapshot_s *s);
extern int tfe_write_snapshot_module(struct snapshot_s *s);

#endif
