/*
 * kbd.h - X11 keyboard driver.
 *
 * Written by
 *  Jouko Valta        (jopi@stekt.oulu.fi)
 *  Andre Fachat       (fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli   (ettore@comm2000.it)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README file for copyright notice.
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

/* X11 keyboard driver. */

#ifndef _KBD_X_H
#define _KBD_X_H

#include <X11/Intrinsic.h>

#define	KBD_ROWS	11	/* maximum size of keyboard array */
#if 0 
#ifdef C128
#define KBD_ROWS	11	/* C128 Keyboard size */
#else
#ifdef PET
#define	KBD_ROWS	10	/* PET Keyboard size */
#else
#define KBD_ROWS	8	/* C64/VIC20/Plus4 Keyboard size */
#endif
#endif
#endif /* 0 */

#define KBD_COLS	8

extern int keyarr[KBD_ROWS], rev_keyarr[KBD_COLS];
extern BYTE joy[3];

extern void kbd_event_handler(Widget w, XtPointer client_data, XEvent *report,
			      Boolean *ctd);
extern int kbd_load_keymap(const char *filename);
extern int kbd_dump_keymap(const char *filename);

extern void kbd_flag_joykeys(int,int);

extern int kbd_init(void);
extern int do_kbd_init_cmdline_options(void);
extern int do_kbd_init_resources(void);

extern int kbd_init_cmdline_options(void);
extern int kbd_init_resources(void);

extern int pet_kbd_init_cmdline_options(void);
extern int pet_kbd_init_resources(void);

extern char **keymap_res_name_list;

#endif
