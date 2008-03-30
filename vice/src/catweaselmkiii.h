/*
 * catweaselmkiii.h
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

#ifndef _CATWEASELMKIII_H
#define _CATWEASELMKIII_H

#include "types.h"

extern int catweaselmkiii_init(void);
extern int catweaselmkiii_open(void);
extern int catweaselmkiii_close(void);
extern int catweaselmkiii_read(WORD addr, int chipno);
extern void catweaselmkiii_store(WORD addr, BYTE val, int chipno);
extern void catweaselmkiii_set_machine_parameter(long cycles_per_sec);
#endif

