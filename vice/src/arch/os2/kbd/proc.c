/*
 * proc.c - Keyboard processing
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_WININPUT     // VK_CAPSLOCK, KC_*, WinSetKeyboardStateTable
#define INCL_WINWINDOWMGR // QWL_USER
#include "vice.h"

#include "log.h"
#include "mon.h"          // mon
#include "video.h"        // canvas_t
#include "parse.h"        // keyconvmap
#include "dialogs.h"      // dialogs
#include "machine.h"      // machine_set_restore_key
#include "console.h"      // console
#include "joystick.h"     // joystick_handle_key
#include "keyboard.h"     // keyboard_set_keyarr
#include "fliplist.h"     // flip_attach_head
#include "resources.h"    // resource_value_t
#include "interrupt.h"    // maincpu_trigger_trap
#include "screenshot.h"   // screenshot_canvas_save


// -----------------------------------------------------------------

#ifdef __X128__
/* 40/80 column key.  */
static key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;
#endif

// -----------------------------------------------------------------

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

inline void kbd_set_key(CHAR usScancode, USHORT release, USHORT fsFlags)
{
        if (!joystick_handle_key((kbd_code_t)usScancode, release))
        {
            int shift = !!(fsFlags&KC_SHIFT);
            keyboard_set_keyarr(keyconvmap.map[shift][usScancode].row,
                                keyconvmap.map[shift][usScancode].column,
                                release);

            switch (keyconvmap.map[shift][usScancode].vshift)
            {
            case 0x3: // unshifted
                keyboard_set_keyarr(keyconvmap.lshift_row,
                                    keyconvmap.lshift_col,
                                    0);
                keyboard_set_keyarr(keyconvmap.rshift_row,
                                    keyconvmap.rshift_col,
                                    0);
                break;
            case 0x1: // left shifted
                keyboard_set_keyarr(keyconvmap.lshift_row,
                                    keyconvmap.lshift_col,
                                    release);
                break;
            case 0x2: // right shifted
                keyboard_set_keyarr(keyconvmap.rshift_row,
                                    keyconvmap.rshift_col,
                                    release);
                break;
            case 0x0:
                if (shift)
                    keyboard_set_keyarr(keyconvmap.lshift_row,
                                        keyconvmap.lshift_col,
                                        release);
                break;

            }
        }
}

extern char *screenshot_name(void);
extern char *screenshot_type(void);

void save_screenshot(HWND hwnd)
{
    const char *type = screenshot_type();
    const char *name = screenshot_name();
    if (!screenshot_canvas_save(type, name, (canvas_t *)WinQueryWindowPtr(hwnd,QWL_USER)))
        log_debug("proc.c: Screenshot successfully saved as %s (%s)", name, type);
}

/* Needed prototype funtions                                        */
/*----------------------------------------------------------------- */

extern void emulator_pause(void);
extern void emulator_resume(void);
extern int isEmulatorPaused(void);


void kbd_proc(HWND hwnd, MPARAM mp1)
{
    USHORT fsFlags    = SHORT1FROMMP(mp1);
    CHAR   usScancode = CHAR4FROMMP (mp1);
    USHORT release    = !(fsFlags&KC_KEYUP);

    // ----- Process all keys with 'special' meanings -----
    switch (usScancode)
    {
    case K_CAPSLOCK:  // turn capslock led off if capslock is pressed
        {
            /* This is not a beautiful way, but the one I know :-) */
            BYTE keyState[256];
            WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
            keyState[VK_CAPSLOCK] &= ~1;
            WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
        }
        break;;
    case K_SCROLLOCK: // toggle warp mode if ScrlLock is pressed
        resources_set_value("WarpMode",
                            (resource_value_t)(SHORT1FROMMP(mp1)&0x1000));
        return;
    case K_PGUP:      // restore key pressed
        machine_set_restore_key(release);
        return;

    }

    // ----- give all keypresses without Alt to Vice -----
    if (!(fsFlags&KC_ALT))
    {
        kbd_set_key(usScancode, release, fsFlags);
        return;
    }

    // if key is only released: return
    if (release)
        return;

    // Process all keys whichs are pressed together with Alt.
    switch (usScancode)
    {
    case K_8:      attach_dialog    (hwnd, 8); return; // Drive #8
    case K_9:      attach_dialog    (hwnd, 9); return; // Drive #9
    case K_0:      attach_dialog    (hwnd, 0); return; // Datasette
    case K_A:      about_dialog     (hwnd);    return;
    case K_C:      datasette_dialog (hwnd);    return;
    case K_D:      drive_dialog     (hwnd);    return;
    case K_E:      emulator_dialog  (hwnd);    return;
    case K_Q:      hardreset_dialog (hwnd);    return;
    case K_R:      softreset_dialog (hwnd);    return;
    case K_S:      sound_dialog     (hwnd);    return;
    case K_PRTSCR: save_screenshot  (hwnd);    return;
#ifdef HAS_JOYSTICK
    case K_J:      joystick_dialog  (hwnd);    return;
#endif
    case K_P:
        if (!isEmulatorPaused())
        {
            emulator_pause();
            WinMessageBox(HWND_DESKTOP, hwnd,
                          "Emulation paused. Press OK to resume",
                          "Pause Emulation", 0, MB_OK);
            emulator_resume();
        }
        break;
    case K_M: // invoke build-in monitor
        monitor_dialog(hwnd);
        maincpu_trigger_trap(mon_trap, (void *) 0);
        //mon(MOS6510_REGS_GET_PC(&maincpu_regs));
        return;
    case K_F: // Flip Drive #8 to next image of fliplist
        flip_attach_head(8, FLIP_NEXT);
        return;
    case K_T: // Switch True Drive Emulatin on/off
        {
            char str[35];
            sprintf(str, "True drive emulation switched %s",
                    toggle("DriveTrueEmulation")?"ON.":"OFF.");
            WinMessageBox(HWND_DESKTOP, hwnd,
                          str, "Drive Emulation", 0, MB_OK);
        }
        return;
#ifdef __X128__
    case K_V: // press/release 40/80-key
        if (key_ctrl_column4080_func)
            key_ctrl_column4080_func();
        return;
#endif
    case K_W: // Write settings to configuration file
        WinMessageBox(HWND_DESKTOP, hwnd,
                      resources_save(NULL)<0?"Cannot save settings.":
                      "Settings written successfully.",
                      "Resources", 0, MB_OK);
        return;
    }
}

// ------------------------------------------------------------------

#ifdef __X128__
void kbd_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}
#endif

