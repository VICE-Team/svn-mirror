/*
 * event.h - Event handling.
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

#ifndef _EVENT_H
#define _EVENT_H

#define EVENT_LIST_END        0
#define EVENT_KEYBOARD_MATRIX 1

struct snapshot_s;

extern void event_init(void);
extern int event_resources_init(void);
extern int event_cmdline_options_init(void);

extern int event_record_start(void);
extern int event_record_stop(void);
extern int event_playback_start(void);
extern int event_playback_stop(void);

extern void event_record(unsigned int type, void *data, unsigned int size);

extern int event_snapshot_read_module(struct snapshot_s *s, int event_mode);
extern int event_snapshot_write_module(struct snapshot_s *s, int event_mode);

#endif

