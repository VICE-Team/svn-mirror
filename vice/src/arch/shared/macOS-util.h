/**
 * \file macOS-util.h
 * \brief A collection of little macOS helpers.
 *
 * Calling Objective-C code from C is possible, but not very readable.
 * Nicer to just plop a few readable functions in here.
 *
 * \author David Hogan <david.q.hogan@gmail.com>
 */

/* This file is part of VICE, the Versatile Commodore Emulator.
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
#ifndef VICE_MACOS_UTIL_H
#define VICE_MACOS_UTIL_H

#include <stdbool.h>

void vice_macos_set_main_thread(void);
void vice_macos_set_vice_thread_priority(bool warp_enabled);
void vice_macos_set_render_thread_priority(void);

/* void vice_macos_render_log_create(void); */
/* void vice_macos_render_log_frame_event(void); */

#endif /* #ifndef VICE_MACOS_UTIL_H */
