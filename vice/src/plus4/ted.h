/*
 * ted.h
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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
struct canvas_refresh_s;

extern int ted_resources_init(void);
extern int ted_cmdline_options_init(void);
extern struct raster_s *ted_init(void);
extern struct video_canvas_s *ted_get_canvas(void);

extern void ted_reset(void);
extern void ted_prepare_for_snapshot(void);
extern void ted_powerup(void);
extern void ted_reset_registers(void);
extern void ted_handle_pending_alarms(int num_write_cycles);
extern void ted_screenshot(struct screenshot_s *screenshot);
extern void ted_async_refresh(struct canvas_refresh_s *r);
extern void ted_free(void);

extern int ted_snapshot_write_module(struct snapshot_s *s);
extern int ted_snapshot_read_module(struct snapshot_s *s);

extern void ted_video_refresh(void);
extern void video_free(void);

#endif

