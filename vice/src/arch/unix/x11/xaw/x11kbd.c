/*
 * x11kbd.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
 *
 * Support for multiple visuals and depths by
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

/* X11 keyboard driver. */

#include "vice.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "kbd.h"
#include "kbdef.h"
#include "keyboard.h"
#include "machine.h"
#include "ui.h"

/* Meta status (used to filter out keypresses when meta is pressed).  */
static int meta_count = 0;

static int left_shift_down, right_shift_down, virtual_shift_down;

void kbd_event_handler(Widget w, XtPointer client_data, XEvent *report,
                       Boolean *ctd)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;
    int count, i;

    count = XLookupString(&report->xkey, buffer, 20, &key, &compose);

    if (psid_mode) {
        return;
    }

    if (key != NoSymbol
        && ((key == key_ctrl_restore1)
        || (key == key_ctrl_restore2))) {   /* Restore */
        int retfl = 0;
        if (report->type == KeyPress) {
            retfl = machine_set_restore_key(1);
        } else if (report->type == KeyRelease) {
            retfl = machine_set_restore_key(0);
        }
        if (retfl)
            return;
    }

    switch (report->type) {

      case KeyPress:

#ifdef DEBUG_KBD
        log_debug("KeyPress `%s'.", XKeysymToString(key));
#endif

        if (key == XK_Meta_R
            || key == XK_Meta_L
#ifdef ALT_AS_META
            || key == XK_Alt_R
            || key == XK_Alt_L
#endif
#ifdef MODE_SWITCH_AS_META
            || key == XK_Mode_switch
#endif
            )
            meta_count++;

        if (key != NoSymbol && key == key_ctrl_column4080) {
            if (key_ctrl_column4080_func != NULL)
                key_ctrl_column4080_func();
            break;
        }

        if (meta_count != 0)
            break;

        if (check_set_joykeys(key, 1))
            break;
        if (check_set_joykeys(key, 2))
            break;

        if (keyconvmap) {
            for (i = 0; keyconvmap[i].sym != 0; ++i) {
                if (key == keyconvmap[i].sym) {
                    int row = keyconvmap[i].row;
                    int column = keyconvmap[i].column;

                    if (row >= 0) {
                        keyboard_set_keyarr(row, column, 1);

                        if (keyconvmap[i].shift == NO_SHIFT) {
                            keyboard_set_keyarr(kbd_lshiftrow, kbd_lshiftcol,
                                                0);
                            keyboard_set_keyarr(kbd_rshiftrow, kbd_rshiftcol,
                                                0);
                        } else {
                            if (keyconvmap[i].shift & VIRTUAL_SHIFT)
                                virtual_shift_down++;
                            if (keyconvmap[i].shift & LEFT_SHIFT)
                                left_shift_down++;
                            if (left_shift_down + virtual_shift_down > 0)
                                keyboard_set_keyarr(kbd_lshiftrow,
                                                    kbd_lshiftcol, 1);
                            if (keyconvmap[i].shift & RIGHT_SHIFT)
                                right_shift_down++;
                            if (right_shift_down > 0)
                                keyboard_set_keyarr(kbd_rshiftrow,
                                                    kbd_rshiftcol, 1);
                        }
                        break;
                    }
                }
            }
	}
	break;			/* KeyPress */

      case KeyRelease:

#ifdef DEBUG_KBD
        log_debug("KeyRelease `%s'.", XKeysymToString(key));
#endif

	if (IsModifierKey(key)) {
	    /* FIXME: This is a dirty kludge.  X11 can sometimes give the
               KeyPress event with the shifted KeySym, and the KeyRelease one
               with the same KeySym unshifted, so we loose control of what
               has been pressed and what has been released (all KeySyms are
               handled independently here).  For example, if the user does
               <Press Shift> <Press 1> <Release Shift> <Release 1>, we get
               <KeyPress Shift>, <KeyPress !>, <KeyRelease Shift>,
               <KeyRelease 1>.  To avoid disasters, we reset all the keyboard
               when a modifier has been released, but this heavily simplifies
               the behavior of multiple keys.  Does anybody know a way to
               avoid this X11 oddity?  */
            keyboard_clear_keymatrix();
            virtual_shift_down = 0;
	    /* TODO: do we have to cleanup joypads here too? */
	}

        if (meta_count > 0
            && (key == XK_Meta_R
                || key == XK_Meta_L
#ifdef ALT_AS_META
                || key == XK_Alt_R
                || key == XK_Alt_L
#endif
#ifdef MODE_SWITCH_AS_META
                || key == XK_Mode_switch
#endif
                ))
            meta_count--;

        if (meta_count != 0)
            break;

	if (check_clr_joykeys(key, 1))
            break;
	if (check_clr_joykeys(key, 2))
            break;

	if (keyconvmap) {
            for (i = 0; keyconvmap[i].sym != 0; i++) {
                if (key == keyconvmap[i].sym) {
                    int row = keyconvmap[i].row;
                    int column = keyconvmap[i].column;

                    if (row >= 0) {
                        keyboard_set_keyarr(row, column, 0);
                        if (keyconvmap[i].shift & VIRTUAL_SHIFT)
                            virtual_shift_down--;
                        if (keyconvmap[i].shift & LEFT_SHIFT)
                            left_shift_down--;
                        if (keyconvmap[i].shift & RIGHT_SHIFT)
                            right_shift_down--;
                    }
                }
            }
	}

	/* Map shift keys. */
	if (right_shift_down > 0)
	    keyboard_set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 1);
	else
	    keyboard_set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 0);
	if (left_shift_down + virtual_shift_down > 0)
	    keyboard_set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 1);
	else
	    keyboard_set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 0);
	break;			/* KeyRelease */

      case EnterNotify:
      case LeaveNotify:
	/* Clean up. */
        keyboard_clear_keymatrix();
	memset(joystick_value, 0, sizeof(joystick_value));
	virtual_shift_down = left_shift_down = right_shift_down = 0;
	memset(joypad_status, 0, sizeof(joypad_status));
        meta_count = 0;
	break;			/* LeaveNotify */

      default:
	break;

    }				/* switch */
}
