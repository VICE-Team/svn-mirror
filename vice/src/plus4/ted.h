/*
 * ted.h
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _TED_H
#define _TED_H

#include "types.h"

struct snapshot_s;
struct screenshot_s;

extern int ted_init_resources(void);
extern int ted_init_cmdline_options(void);
extern struct raster_s *vic_ii_init(void);
extern struct canvas_s *vic_ii_get_canvas(void);

extern void ted_reset(void);
extern void vic_ii_exposure_handler(unsigned int width, unsigned int height);
extern void vic_ii_trigger_light_pen(CLOCK mclk);
extern void vic_ii_set_vbank(int new_vbank);
extern void vic_ii_set_ram_base(BYTE *base);
extern void vic_ii_enable_extended_keyboard_rows(int flag);
extern void vic_ii_prepare_for_snapshot(void);
extern void vic_ii_powerup(void);
extern void vic_ii_resize(void);
extern void vic_ii_set_set_canvas_refresh(int enable);
extern void vic_ii_change_timing(void);
extern void ted_reset_registers(void);
extern void vic_ii_update_memory_ptrs_external(void);
extern void ted_handle_pending_alarms(int num_write_cycles);
extern int vic_ii_screenshot(struct screenshot_s *screenshot);
extern void ted_free(void);

extern int vic_ii_write_snapshot_module(struct snapshot_s *s);
extern int vic_ii_read_snapshot_module(struct snapshot_s *s);

extern void vic_ii_video_refresh(void);
extern void video_free(void);

void vic_ii_set_phi1_vbank(int num_vbank);
void vic_ii_set_phi2_vbank(int num_vbank);
void vic_ii_set_phi1_ram_base(BYTE *base);
void vic_ii_set_phi2_ram_base(BYTE *base);
void vic_ii_set_phi1_addr_options(ADDRESS mask, ADDRESS offset);
void vic_ii_set_phi2_addr_options(ADDRESS mask, ADDRESS offset);
void vic_ii_set_phi1_chargen_addr_options(ADDRESS mask, ADDRESS value);
void vic_ii_set_phi2_chargen_addr_options(ADDRESS mask, ADDRESS value);

#endif

