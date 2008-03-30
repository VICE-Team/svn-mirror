/*
 * autostart.h - Automatic image loading and starting.
 *
 * Written by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Ettore Perazzoli <ettore@comm2000.it>
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

#ifndef _AUTOSTART_H
#define _AUTOSTART_H

#include "types.h"

#define AUTOSTART_MODE_RUN  0
#define AUTOSTART_MODE_LOAD 1

extern int autostart_init(CLOCK _min_cycles, int _handle_drive_true_emulation,
                          int _blnsw, int _pnt, int _pntr, int _lnmx);

extern int autostart_autodetect(const char *file_name,
                                const char *program_name,
                                unsigned int program_number,
                                unsigned int runmode);
extern int autostart_disk(const char *file_name, const char *program_name,
                          unsigned int program_number, unsigned int runmode);
extern int autostart_tape(const char *file_name, const char *program_name,
                          unsigned int program_number, unsigned int runmode);
extern int autostart_prg(const char *file_name, unsigned int runmode);

extern void autostart_disable(void);
extern void autostart_advance(void);

extern int autostart_device(int num);
extern void autostart_reset(void);

extern int autostart_ignore_reset;

#endif /* !_AUTOSTART_H */

