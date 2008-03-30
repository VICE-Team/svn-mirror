/* -*- C -*-
 *
 * c610acia.def - Definitions for a 6551 ACIA interface
 *
 * Written by
 *   Andre' Fachat (fachat@physik.tu-chemnitz.de)
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


#define mycpu maincpu

#define myclk clk

#define myacia acia1

#define I_MYACIA I_ACIA1


/* resource defaults */
#define        MyAcia          Acia1
#define        MyDevice        0
#define        MyIrq           IK_IRQ

#define myaciadev       acia1dev
#define MYACIADEV       "Acia1Dev"
#define MYACIAIRQ       "Acia1Irq"

#define myacia_init acia1_init
#define myacia_init_cmdline_options acia1_init_cmdline_options
#define myacia_init_resources acia1_init_resources
#define myacia_prevent_clk_overflow acia1_prevent_clk_overflow
#define myacia_read_snapshot_module acia1_read_snapshot_module
#define myacia_write_snapshot_module acia1_write_snapshot_module
#define peek_myacia peek_acia1
#define read_myacia read_acia1
#define reset_myacia reset_acia1
#define store_myacia store_acia1

#include "maincpu.h"

#define mycpu_alarm_context maincpu_alarm_context

#include "c610tpi.h"

#define mycpu_set_int(b,a)         	tpi1_set_int(4,(a))
#define mycpu_set_int_noclk(b,c)	tpi1_restore_int(4,(c))

#include "aciacore.c"

