/*
 * mon_ui.h - The VICE built-in monitor, external interface for the UI.
 *
 * Written by
 *  Spiro Trikaliotis <Spiro.Trikaliotis@gmx.de>
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

#ifndef _MON_UI_H
#define _MON_UI_H

#include "mon.h"

struct mon_disassembly_private;

struct mon_disassembly_flags
{
    int active_line       : 1;
    int is_breakpoint     : 1;
    int breakpoint_active : 1;
};

struct mon_disassembly
{
    struct mon_disassembly *     next;
    struct mon_disassembly_flags flags;
    size_t                       length;
    char *                       content;
};

typedef
enum MON_SCROLL_TYPE_TAG
{
    MON_SCROLL_NOTHING,
    MON_SCROLL_UP,
    MON_SCROLL_DOWN,
    MON_SCROLL_PAGE_UP,
    MON_SCROLL_PAGE_DOWN
} MON_SCROLL_TYPE;

struct mon_disassembly_private * mon_disassembly_init     ( void );
void                             mon_disassembly_deinit   ( struct mon_disassembly_private * );
void                             mon_disassembly_update   ( struct mon_disassembly_private * );
struct mon_disassembly *         mon_disassembly_get_lines( struct mon_disassembly_private *, int lines );

void                             mon_disassembly_set_memspace( struct mon_disassembly_private *, MEMSPACE );
MEMSPACE                         mon_disassembly_get_memspace( struct mon_disassembly_private * );

ADDRESS                          mon_scroll   ( struct mon_disassembly_private *, MON_SCROLL_TYPE );
ADDRESS                          mon_scroll_to( struct mon_disassembly_private *, ADDRESS         );

#endif
