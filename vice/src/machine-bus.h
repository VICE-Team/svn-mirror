/*
 * machine-bus.c - Generic interface to IO bus functions.
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

#ifndef _MACHINE_BUS_H
#define _MACHINE_BUS_H

#include "types.h"

extern void machine_bus_init(void);

extern int machine_bus_lib_directory(unsigned int unit, const char *pattern,
                                     BYTE **buf);
extern int machine_bus_lib_read_sector(unsigned int unit, unsigned int track,
                                       unsigned int sector, BYTE *buf);
extern int machine_bus_lib_write_sector(unsigned int unit, unsigned int track,
                                        unsigned int sector, BYTE *buf);

extern unsigned int machine_bus_device_fsimage_state_get(unsigned int unit);
extern unsigned int machine_bus_device_realdevice_state_get(unsigned int unit);

extern void machine_bus_status_truedrive_set(unsigned int enable);
extern void machine_bus_status_drivetype_set(unsigned int unit,
                                             unsigned int enable);
extern void machine_bus_status_virtualdevices_set(unsigned int enable);

extern void machine_bus_eof_callback_set(void (*func)(void));
extern void machine_bus_attention_callback_set(void (*func)(void));

#endif

