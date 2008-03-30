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
#include "keyboard.h"
#include "machine.h"
#include "ui.h"
#include "uiarch.h"


void kbd_event_handler(Widget w, XtPointer client_data, XEvent *report,
                       Boolean *ctd)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;
    int count;

    count = XLookupString(&report->xkey, buffer, 20, &key, &compose);

    switch (report->type) {
      case KeyPress:
        x11kbd_press((signed long)key);
        break;
      case KeyRelease:
        x11kbd_release((signed long)key);
        break;
      case EnterNotify:
      case LeaveNotify:
        x11kbd_enter_leave();
        break;                  /* LeaveNotify */
      case FocusOut:
      case FocusIn:
        x11kbd_focus_change();
        break;
      default:
        break;
    }                           /* switch */
}

