/*
 * console.h - Console access interface.
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

#ifndef _CONSOLE_H
#define _CONSOLE_H

typedef struct console_s {
    /* Console geometry.  */
	/* be careful - geometry might change at run-time! */
    unsigned int console_xres;
    unsigned int console_yres;

    /* It is allowed to leave the console open atfer control is given back
       to the emulation.  */
    int console_can_stay_open;
} console_t;

extern int console_init(void);

extern console_t *console_open(const char *id);
extern int console_close(console_t *log);

/* the following should be called when quitting VICE */
extern int console_shutdown(console_t *log);

extern int console_out(console_t *log, const char *format, ...);
extern char *console_in(console_t *log);

#endif

