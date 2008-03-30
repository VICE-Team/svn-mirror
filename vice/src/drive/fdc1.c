/*
 * fdc1.c - 1001/8x50 FDC emulation
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#include "fdccore.h"

/*************************************************************************
 * Renaming exported functions
 */

#define myfdc_init	fdc1_init
#define myfdc_reset	fdc1_reset

#define MYFDC_NAME      "fdc1"

/*************************************************************************
 * CPU binding
 */

#define myclk           drive_clk[1]
#define mycpu_clk_guard drive1_clk_guard
#define mycpu_alarm_context drive1_alarm_context

#include "fdccore.c"

