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

extern char *get_snapshot(int *save_roms, int *save_disks);

static void save_snapshot(ADDRESS addr, void *hwnd)
{
    int save_roms, save_disks;
    char *s=get_snapshot(&save_roms, &save_disks);

    if (machine_write_snapshot(s, save_roms, save_disks) < 0)
            WinMessageBox(HWND_DESKTOP, (HWND)hwnd,
                          "Unable to save snapshot - sorry!",
                          "Save Snapshot", 0, MB_OK);
}

inline void kbd_set_key(const CHAR code1, const CHAR code2,
                        const USHORT release, const USHORT shift)
{
    int nr;
    for (nr=0; nr<keyconvmap.entries; nr++)
    {
        if (keyconvmap.map[shift][nr].code==(code1 | code2<<8))
            break;
    }

    if (nr==keyconvmap.entries)
    {
        if (!release)
            log_message(LOG_DEFAULT, "proc.c: Unknown key %d %d pressed by user.", code1, code2);
        return;
    }

    // send pressed key to CBM
    keyboard_set_keyarr(keyconvmap.map[shift][nr].row,
                        keyconvmap.map[shift][nr].column,
                        release);
    /*
     if (!release)
     log_debug("key  %3i %3i  -%d->  %3i %3i %i",
     code1, code2, fsFlags&KC_CHAR,
     keyconvmap.map[shift][nr].row,
     keyconvmap.map[shift][nr].column,
     keyconvmap.map[shift][nr].vshift);
     */

    // process virtual shift key
    switch (keyconvmap.map[shift][nr].vshift)
    {
    case 0x1: // left shifted, press/release left shift
        keyboard_set_keyarr(keyconvmap.lshift_row,
                            keyconvmap.lshift_col,
                            release);
        break;
    case 0x2: // right shifted, press/release right shift
        keyboard_set_keyarr(keyconvmap.rshift_row,
                            keyconvmap.rshift_col,
                            release);
        break;
    case 0x3: // unshifted, release virtual shift keys
        keyboard_set_keyarr(keyconvmap.lshift_row,
                            keyconvmap.lshift_col,
                            0);
        keyboard_set_keyarr(keyconvmap.rshift_row,
                            keyconvmap.rshift_col,
                            0);
        break;
    }
}

extern char *screenshot_name(void);
extern char *screenshot_type(void);

void save_screenshot(HWND hwnd)
{
    const char *type = screenshot_type();
    const char *name = screenshot_name();
    if (!screenshot_canvas_save(type, name, (canvas_t *)WinQueryWindowPtr(hwnd, QWL_USER)))
        log_debug("proc.c: Screenshot successfully saved as %s (%s)", name, type);
}

/* Needed prototype funtions                                        */
/*----------------------------------------------------------------- */

extern void emulator_pause(void);
extern void emulator_resume(void);
extern int isEmulatorPaused(void);
   /*** Virtual key values *************************************************
   #define VK_BUTTON1                 0x01
   #define VK_BUTTON2                 0x02
   #define VK_BUTTON3                 0x03
   #define VK_BREAK                   0x04
   #define VK_BACKSPACE               0x05
   #define VK_TAB                     0x06
   #define VK_BACKTAB                 0x07
   #define VK_NEWLINE                 0x08
   #define VK_SHIFT                   0x09
   #define VK_CTRL                    0x0A
   #define VK_ALT                     0x0B
   #define VK_ALTGRAF                 0x0C
   #define VK_PAUSE                   0x0D
   #define VK_CAPSLOCK                0x0E
   #define VK_ESC                     0x0F
   #define VK_SPACE                   0x10
   #define VK_PAGEUP                  0x11
   #define VK_PAGEDOWN                0x12
   #define VK_END                     0x13
   #define VK_HOME                    0x14
   #define VK_LEFT                    0x15
   #define VK_UP                      0x16
   #define VK_RIGHT                   0x17
   #define VK_DOWN                    0x18
   #define VK_PRINTSCRN               0x19
   #define VK_INSERT                  0x1A
   #define VK_DELETE                  0x1B
   #define VK_SCRLLOCK                0x1C
   #define VK_NUMLOCK                 0x1D
   #define VK_ENTER                   0x1E
   #define VK_SYSRQ                   0x1F
   #define VK_F1                      0x20
   #define VK_F2                      0x21
   #define VK_F3                      0x22
   #define VK_F4                      0x23
   #define VK_F5                      0x24
   #define VK_F6                      0x25
   #define VK_F7                      0x26
   #define VK_F8                      0x27
   #define VK_F9                      0x28
   #define VK_F10                     0x29
   #define VK_F11                     0x2A
   #define VK_F12                     0x2B
   #define VK_F13                     0x2C
   #define VK_F14                     0x2D
   #define VK_F15                     0x2E
   #define VK_F16                     0x2F
   #define VK_F17                     0x30
   #define VK_F18                     0x31
   #define VK_F19                     0x32
   #define VK_F20                     0x33
   #define VK_F21                     0x34
   #define VK_F22                     0x35
   #define VK_F23                     0x36
   #define VK_F24                     0x37
   #define VK_ENDDRAG                 0x38
   #define VK_CLEAR                   0x39
   #define VK_EREOF                   0x3A
   #define VK_PA1                     0x3B
   #define VK_ATTN                    0x3C
   #define VK_CRSEL                   0x3D
   #define VK_EXSEL                   0x3E
   #define VK_COPY                    0x3F
   #define VK_BLK1                    0x40
   #define VK_BLK2                    0x41
*/
void kbd_proc(HWND hwnd, MPARAM mp1, MPARAM mp2)
{
    USHORT fsFlags    = SHORT1FROMMP(mp1);
    CHAR   usScancode = CHAR4FROMMP(mp1);   //fsFlags&KC_SCANCODE
    CHAR   usKeycode  = SHORT1FROMMP(mp2);  //fsFlags&KC_CHAR
    CHAR   usVK       = SHORT2FROMMP(mp2);  //fsFlags&KC_VIRTUALKEY VK_TAB/LEFT/RIGHT/UP/DOWN
    USHORT release    = !(fsFlags&KC_KEYUP);

    // ----- Process virtual keys -----
    if (fsFlags&KC_VIRTUALKEY)
    {
        switch (usVK)
        {
        case VK_CAPSLOCK:  // turn capslock led off if capslock is pressed
            {
                /* This is not a beautiful way, but the one I know :-) */
                BYTE keyState[256];
                WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
                keyState[VK_CAPSLOCK] &= ~1;
                WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
            }
            break;
        case VK_SCRLLOCK: // toggle warp mode if ScrlLock is pressed
            resources_set_value("WarpMode",
                                (resource_value_t)(fsFlags&KC_TOGGLE));
            return;
        case VK_F12:      // restore key pressed
            machine_set_restore_key(release);
            return;
#ifdef __X128__
        case VK_F11: // press/release 40/80-key
            if (key_ctrl_column4080_func)
                key_ctrl_column4080_func();
            return;
#endif
        }
    }

    // ----- give all keypresses without Alt to Vice -----
    if (!(fsFlags&KC_ALT))
    {
        if (fsFlags&KC_PREVDOWN)
            return;

        if (joystick_handle_key((kbd_code_t)usScancode, release))
            return;

        kbd_set_key(keyconvmap.symbolic?usKeycode:usScancode,
                    keyconvmap.symbolic?usVK:0,
                    release, fsFlags&KC_SHIFT?1:0);
        return;
    }

    // if key is released: return
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
    case K_H:      hardware_dialog  (hwnd);    return;
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
        maincpu_trigger_trap(mon_trap, NULL);
        return;
    case K_N: // invoke build-in monitor
        maincpu_trigger_trap(save_snapshot, (void*)hwnd);
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

