/*
 * mon_breakpoint.h - The VICE built-in monitor breakpoint functions.
 *
 * Written by
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

#ifndef _MON_BREAKPOINT_H
#define _MON_BREAKPOINT_H

#include "mon.h"

#define check_breakpoints(mem, addr) \
    mon_breakpoint_check_checkpoint(mem, addr, breakpoints[mem])

extern void mon_breakpoint_init(void);

extern void mon_breakpoint_switch_checkpoint(int op, int breakpt_num);
extern void mon_breakpoint_set_ignore_count(int breakpt_num, int count);
extern void mon_breakpoint_print_checkpoints(void);
extern void mon_breakpoint_delete_checkpoint(int brknum);
extern void mon_breakpoint_set_checkpoint_condition(int brk_num,
                                                    CONDITIONAL_NODE *cnode);
extern void mon_breakpoint_set_checkpoint_command(int brk_num, char *cmd);
extern int mon_breakpoint_check_checkpoint(MEMSPACE mem, ADDRESS addr,
                                           BREAK_LIST *list);
extern int mon_breakpoint_add_checkpoint(MON_ADDR start_addr, MON_ADDR end_addr,
                                         bool is_trace, bool is_load,
                                         bool is_store, bool is_temp);

#endif

