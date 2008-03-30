/*
 * kbd.c - X11 keyboard driver.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andre Fachat <fachat@physik.tu-chemnitz.de>
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

#include "vice.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "kbd.h"
#include "keyboard.h"
#include "machine.h"
#include "ui.h"
#include "uihotkey.h"


void kbd_arch_init(void)
{
}

signed long kbd_arch_keyname_to_keynum(char *keyname)
{
    KeySym sym;

    sym = XStringToKeysym(keyname);

    if (sym == NoSymbol)
        return -1;

    return (signed long)sym;
}

const char *kbd_arch_keynum_to_keyname(signed long keynum)
{
    return XKeysymToString((KeySym)keynum);
}

/* ------------------------------------------------------------------ */

/* Meta status (used to filter out keypresses when meta is pressed).  */
static int meta_count = 0;
static int control_count, shift_count;

void x11kbd_press(signed long key)
{
#ifdef DEBUG_KBD
    log_debug("KeyPress `%s'.", XKeysymToString(key));
#endif

    /* Hotkeys */
    switch (key) {
      case XK_Shift_L:
      case XK_Shift_R:
        shift_count++;
        break;

      case XK_Control_L:
      case XK_Control_R:
        control_count++;
        break;
      case XK_Meta_L:
      case XK_Meta_R:
#ifdef ALT_AS_META
      case XK_Alt_L:
      case XK_Alt_R:
#endif
#ifdef MODE_SWITCH_AS_META
      case XK_Mode_switch:
#endif
        meta_count++;
        break;
    }

    if ((meta_count != 0) &&
        ui_dispatch_hotkeys(key))
        return;

    if (vsid_mode)
        return;

    if (meta_count != 0)
        return;

    if (key == NoSymbol)
        return;

    keyboard_key_pressed(key);
}

void x11kbd_release(signed long key)
{
#ifdef DEBUG_KBD
    log_debug("KeyRelease `%s'.", XKeysymToString(key));
#endif

    /* Hotkeys */
    switch (key) {
      case XK_Shift_L:
      case XK_Shift_R:
        if (shift_count > 0)
            shift_count--;
        break;
      case XK_Control_L:
      case XK_Control_R:
        if (control_count > 0)
            control_count--;
        break;
      case XK_Meta_L:
      case XK_Meta_R:
#ifdef ALT_AS_META
      case XK_Alt_L:
      case XK_Alt_R:
#endif
#ifdef MODE_SWITCH_AS_META
      case XK_Mode_switch:
#endif
        if (meta_count > 0)
            meta_count--;
        break;
    }

    if (vsid_mode)
        return;

    if (meta_count != 0)
        return;
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
        /*virtual_shift_down = 0;*/
        keyboard_key_clear();
        /* TODO: do we have to cleanup joypads here too? */
    }

    if (key == NoSymbol)
        return;

    keyboard_key_released(key);
}

void x11kbd_enter_leave(void)
{
    keyboard_key_clear();
    meta_count = 0;
    return;
}

void x11kbd_focus_change(void)
{
    meta_count = control_count = shift_count = 0;
    return;
}

