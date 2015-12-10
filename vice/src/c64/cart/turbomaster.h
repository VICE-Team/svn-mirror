/*
 * turbo-master.h
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_TURBOMASTER_H
#define VICE_TURBOMASTER_H

#include "alarm.h"
#include "interrupt.h"

extern void turbomaster_reset(void);
extern int turbomaster_resources_init(void);
extern int turbomaster_cmdline_options_init(void);
extern int turbomaster_cart_enabled(void);
extern void turbomaster_resources_shutdown(void);

extern void turbomaster_check_and_run_65c02(interrupt_cpu_status_t *cpu_int_status, alarm_context_t *cpu_alarm_context);

typedef int turbomaster_ba_check_callback_t (void);
typedef void turbomaster_ba_steal_callback_t (void);

extern void turbomaster_ba_register(turbomaster_ba_check_callback_t *ba_check,
                                    turbomaster_ba_steal_callback_t *ba_steal,
                                    int *ba_var, int ba_mask);

#endif
