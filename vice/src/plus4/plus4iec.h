/*
 * plus4iec.h - IEC bus handling for the Plus4.
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

#ifndef _PLUS4IEC_H
#define _PLUS4IEC_H

#include "types.h"

extern void iec_cpu_write(BYTE data);
extern void iec_cpu_write_conf0(BYTE data);
extern void iec_cpu_write_conf1(BYTE data);
extern void iec_cpu_write_conf2(BYTE data);
extern void iec_cpu_write_conf3(BYTE data);
extern BYTE iec_cpu_read(void);
extern void iec_cpu_undump(BYTE data);
extern void parallel_cable_cpu_write(BYTE data);
extern void parallel_cable_cpu_pulse(void);
extern BYTE parallel_cable_cpu_read(void);
extern void parallel_cable_cpu_undump(BYTE data);

struct iec_info_s;

extern struct iec_info_s *iec_get_drive_port(void);
extern int iec_callback_index;

typedef void (*iec_cpu_write_callback_t) (BYTE);

#endif

