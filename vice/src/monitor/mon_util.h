/*
 * mon_util.h - Utilities for the VICE built-in monitor.
 *
 * Written by
 *  Spiro Trikaliotis <spiro.trikaliotis@gmx.de>
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

#ifndef _MON_UTIL_H
#define _MON_UTIL_H

#include "types.h"
#include "console.h"

extern
char *mon_disassemble_with_label(
          MEMSPACE  memspace,
          ADDRESS   loc,
          int       hex,
          unsigned *opc_size_p,
          unsigned *label_p );

extern
void mon_set_command( console_t *console_log, char *command, void (*)(void) );

/* what do you want? extern or static?
 *
 extern monitor_interface_t *mon_interfaces[NUM_MEMSPACES]; */


#endif /* #ifndef _MON_UTIL_H */
