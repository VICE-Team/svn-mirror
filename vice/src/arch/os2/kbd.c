/*
 * kbd.c - Keyboard emulation. (OS/2)
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  Tibor Biczo      (crown@mail.matav.hu)
 *  Thomas Bretz     (tbretz@gsi.de)
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

//#define INCL_WININPUT
#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <direct.h>


#include "ui.h"
#include "kbd.h"
#include "log.h"
#include "sound.h"
#include "vsync.h"
#include "tape.h"
#include "utils.h"
#include "attach.h"        // file_system_attach_disk
#include "machine.h"       // machine_powerup
#include "cmdline.h"
#include "resources.h"
#include "interrupt.h"     // maincpu_trigger_reset
#include "joystick.h"
#include "keyboard.h"
#include "kbdbuf.h"
#include "dialogs.h"

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

/* 40/80 column key.  */
static key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;

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

#ifdef __EMXC__
#define _getcwd _getcwd2
#endif
// _beginthread(PM_mainloop,NULL,0x4000,&canvas_new);
static void ui_attach(HWND hwnd, int number)
{
    static char drive[3]="g:";                        // maybe a resource
    static char path[CCHMAXPATH-2]="\\c64\\images";   // maybe a resource
    char   result [CCHMAXPATH];
    char   dirname[CCHMAXPATH];

    _getcwd(dirname, CCHMAXPATH);        // store working dir

    strcat(strcpy(result, drive),path);
    if (chdir(result))                   // try if actual image dir exist
    {                                    // if it doesn't exist, set
        drive[0]=dirname[0];             // imagedir to working dir
        drive[1]=':';                    // maybe drive is empty at first call
        strcpy(path, dirname+2);
    }
    chdir(dirname);                      // change back to working dir

    if (!ui_file_dialog(hwnd,        number?"Attach disk image":"Attach tape image",
                        drive, path, number?"*.d??;*.d??.gz":"*.t64;*.t64.gz",
                        "Attach", result))
        return;
    if ((number?file_system_attach_disk(number, result):tape_attach_image(result)) < 0)
    {
        ui_error("Cannot attach specified file");
        return;
    }
    drive[0]=result[0];
    *strrchr(result,'\\')='\0';
    strcpy(path, result+2);
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

int toggle(char *resource_name)
{
    int value;
    if (resources_get_value(resource_name, (resource_value_t *) &value) < 0)
        return -1;
    resources_set_value(resource_name, (resource_value_t) !value);
    return !value;
}

void toggle_dialog(char *resource_name, const char *text)
{
    char *str = xcalloc(1,strlen(text)+15);
    strcat(strcat(strcpy(str, text)," switched "),
           toggle(resource_name) ? "ON." : "OFF.");
    ui_OK_dialog(resource_name, str);
    free(str);
}

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
        case K_F7:
            if (key_ctrl_column4080_func != NULL) key_ctrl_column4080_func();
            break;
        case K_8: ui_attach      (hwnd, 8); break;
        case K_9: ui_attach      (hwnd, 9); break;
        case K_0: ui_attach      (hwnd, 0); break;
        case K_A: about_dialog   (hwnd);    break;
        case K_D: drive_dialog   (hwnd);    break;
        case K_Q: ui_hard_reset  (hwnd);    break;
        case K_R: ui_soft_reset  (hwnd);    break;
        case K_J: joystick_dialog(hwnd);    break;
        case K_S: sound_dialog   (hwnd);    break;
            // toggle_dialog("Sound", "Sound playback");
            break;
        case K_T:
            toggle_dialog("DriveTrueEmulation", "True drive emulation");
            break;
        case K_W/*W*/:
            if (resources_save(NULL) < 0)
                ui_OK_dialog("Resources","Cannot save settings.");
            else
                ui_OK_dialog("Resources","Settings written successfully.");
            break;
//        case K_S: toggle_dialog("Sound", "Sound playback"); break;
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

void kbd_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}

