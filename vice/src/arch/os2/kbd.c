/*
 * kbd.c - Keyboard emulation. (OS/2)
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Tibor Biczo <crown@mail.matav.hu>
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

#include "vice.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ui.h"
#include "kbd.h"
#include "log.h"
#include "mon.h"
#include "vsync.h"         //suspend_speed_eval
#include "utils.h"
#include "machine.h"       // machine_powerup
#include "cmdline.h"
#include "resources.h"
#include "interrupt.h"     // maincpu_trigger_reset
#include "joystick.h"
#include "keyboard.h"
#include "kbdbuf.h"
#include "dialogs.h"
#include "maincpu.h"

BYTE joystick_value[3];

/* ------------------------------------------------------------------------ */

typedef struct _convmap {
    keyconv *map;         // Conversion Map
    int vshift_row;       // Location of virt shift key
    int vshift_col;       // Location of virt shift key
} convmap;

static convmap *keyconvmaps;
static convmap *keyconv_base;
static int num_keyconvmaps;
static int keymap_index;

#ifdef __X128__
/* 40/80 column key.  */
static key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;
#endif

/* ------------------------------------------------------------------------ */

int kbd_init(int num, ...)
{
    keyconvmaps=(convmap*)malloc(num*sizeof(convmap));
    {
        va_list p;
        int i;

        num_keyconvmaps = num;

        va_start(p,num);
        for (i =0; i<num_keyconvmaps; i++) {
            keyconv *map;
            unsigned int sizeof_map;
            int shift_row,shift_col;

            shift_row  = va_arg(p,int);
            shift_col  = va_arg(p,int);
            map        = va_arg(p,keyconv*);
            sizeof_map = va_arg(p,unsigned int);

            keyconvmaps[i].map = map;
            keyconvmaps[i].vshift_row = shift_row;
            keyconvmaps[i].vshift_col = shift_col;
        }
    }
    keyconv_base=&keyconvmaps[keymap_index>>1];
    return 0;
}

static int set_keymap_index(resource_value_t v)
{
    int real_index;

    keymap_index = (int)v;
    real_index   = keymap_index>>1;
    keyconv_base = &keyconvmaps[real_index];

    return 0;
}

static resource_t resources[] = {
    { "KeymapIndex", RES_INTEGER, (resource_value_t) 0,
    (resource_value_t *) &keymap_index, set_keymap_index },
    { NULL }
};

int kbd_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      "<number>", "Specify index of used keymap" },
    { NULL },
};

int kbd_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* This is not a beautiful way, but the one I know :-) */
void switch_capslock_led_off(void)
{
    BYTE keyState[256];
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    keyState[VK_CAPSLOCK] &= ~1;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
}

static void ui_hard_reset(HWND hwnd)
{
    if (ui_yesno_dialog(hwnd, "Hard Reset",
                        "Do you really want to hard reset the emulated machine?"))
    {
        suspend_speed_eval();
        machine_powerup();  // Hard_reset;
    }
}

static void ui_soft_reset(HWND hwnd)
{
    if (ui_yesno_dialog(hwnd, "Soft Reset",
                        "Do you really want to soft reset the emulated machine?"))
    {
        suspend_speed_eval();
        maincpu_trigger_reset();  // Soft Reset
    }
}

extern int toggle(char *resource_name);

void toggle_dialog(char *resource_name, const char *text)
{
    char *str = xcalloc(1,strlen(text)+15);
    sprintf(str, "%s%s%s", text, " switched ", toggle(resource_name)?"ON.":"OFF.");
    ui_OK_dialog(resource_name, str);
    free(str);
}
static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

#include "console.h"

extern HWND hwndMonitor;

void wmChar(HWND hwnd, MPARAM mp1)
{   // super warp mode? ohne status window?
    USHORT fsFlags    = SHORT1FROMMP(mp1);
    CHAR   usScancode = CHAR4FROMMP (mp1);
    USHORT release    = !(fsFlags&KC_KEYUP);

    // turn capslock led off if capslock is pressed
    if (usScancode==K_CAPSLOCK)
        switch_capslock_led_off();
    // toggle warp mode if ScrlLock is pressed
    if (usScancode==K_SCROLLOCK)
        resources_set_value("WarpMode",
                            (resource_value_t)(SHORT1FROMMP(mp1)&0x1000));
    if (usScancode==K_PGUP)
        machine_set_restore_key(release);

    if (fsFlags&KC_ALT && release) {
        switch (usScancode)
        {
        case K_8: attach_dialog   (hwnd, 8); break;
        case K_9: attach_dialog   (hwnd, 9); break;
        case K_0: attach_dialog   (hwnd, 0); break;
        case K_A: about_dialog    (hwnd);    break;
        case K_C: datasette_dialog(hwnd);    break;
        case K_D: drive_dialog    (hwnd);    break;
        case K_E: emulator_dialog (hwnd);    break;
        case K_Q: ui_hard_reset   (hwnd);    break;
        case K_R: ui_soft_reset   (hwnd);    break;
        case K_S: sound_dialog    (hwnd);    break;
#ifdef HAS_JOYSTICK
        case K_J: joystick_dialog (hwnd);    break;
#endif
        case K_M:
            //            monitor_dialog(hwnd);
            hwndMonitor=monitor_dialog(HWND_DESKTOP/*hwnd*/);

            maincpu_trigger_trap(mon_trap, (void *) 0);
            //mon(MOS6510_REGS_GET_PC(&maincpu_regs));
            break;
        case K_N:
            console_out(NULL, "test: %s", "hallo");
            break;

        case K_T:
            toggle_dialog("DriveTrueEmulation", "True drive emulation");
            break;
#ifdef __X128__
        case K_V: // VDC
            if (key_ctrl_column4080_func)
                key_ctrl_column4080_func();
            break;
#endif
        case K_W/*W*/:
            if (resources_save(NULL) < 0)
                ui_OK_dialog("Resources","Cannot save settings.");
            else
                ui_OK_dialog("Resources","Settings written successfully.");
            break;
        }
    }
    else {
        if (!joystick_handle_key((kbd_code_t)usScancode, release)) {
            keyboard_set_keyarr(keyconv_base->map[usScancode].row,
                       keyconv_base->map[usScancode].column,
                       release);
            if (keyconv_base->map[usScancode].vshift)
                keyboard_set_keyarr(keyconv_base->vshift_row,
                           keyconv_base->vshift_col,
                           release);
            kbd_buf_flush();
        }
    }
}

/* ------------------------------------------------------------------------ */

#ifdef __X128__
void kbd_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}
#endif

