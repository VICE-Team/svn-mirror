/*
 * vicii.h - A cycle-exact event-driven MOS6569 (VIC-II) emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#ifndef _VICII_H
#define _VICII_H

#include "snapshot.h"
#include "types.h"
#include "vicii-mem.h"

extern int vic_ii_init_resources (void);
extern int vic_ii_init_cmdline_options (void);
extern void *vic_ii_init (void);
extern void vic_ii_reset (void);
extern void vic_ii_exposure_handler (unsigned int width, unsigned int height);
extern void vic_ii_trigger_light_pen (CLOCK mclk);
extern void vic_ii_set_vbank (int new_vbank);
extern void vic_ii_set_ram_base (BYTE * base);
extern void vic_ii_enable_extended_keyboard_rows (int flag);
extern void vic_ii_prepare_for_snapshot (void);
extern void vic_ii_powerup (void);
extern void vic_ii_resize (void);
extern void vic_ii_set_set_canvas_refresh(int enable);
extern void vic_ii_change_timing(void);
extern void vic_ii_reset_registers(void);

extern int vic_ii_write_snapshot_module (snapshot_t *s);
extern int vic_ii_read_snapshot_module (snapshot_t *s);

extern void vic_ii_handle_pending_alarms_external(int num_write_cycles);

extern void video_free (void);
#if 0				/*  def USE_VIDMODE_EXTENSION */
extern void video_setfullscreen (int v, int width, int height);
#endif

#endif

