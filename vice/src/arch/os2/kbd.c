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
#include <stdlib.h>
#include <stdarg.h>

#include "ui.h"
#include "kbd.h"
#include "vsync.h"
#include "attach.h"        // file_system_attach_disk
#include "machine.h"       // machine_powerup
#include "cmdline.h"
#include "resources.h"
#include "joystick.h"

int  keyarr        [KBD_ROWS];
int  rev_keyarr    [KBD_COLS];
BYTE joystick_value[3];

/* ------------------------------------------------------------------------ */

struct _convmap {
    keyconv *map;  // Conversion Map
    int virtual_shift_row, virtual_shift_column; // Location of virt shift key
};

static struct _convmap *keyconvmaps;
static struct _convmap *keyconv_base;
static int    num_keyconvmaps;
static int    keymap_index;

/* ------------------------------------------------------------------------ */

int kbd_init(int num, ...)
{
    //    DEBUG(("Allocating keymaps"));
    keyconvmaps=(struct _convmap*)malloc(num*sizeof(struct _convmap));
    //    DEBUG(("Installing keymaps"));
    {
        va_list p;
        int i;

        num_keyconvmaps=num;

        va_start(p,num);
        for (i =0; i<num_keyconvmaps; i++) {
            keyconv *map;
            unsigned int sizeof_map;
            int shift_row,shift_column;

            shift_row   =va_arg(p,int);
            shift_column=va_arg(p,int);
            map         =va_arg(p,keyconv*);
            sizeof_map  =va_arg(p,unsigned int);

            keyconvmaps[i].map = map;
            keyconvmaps[i].virtual_shift_row    = shift_row;
            keyconvmaps[i].virtual_shift_column = shift_column;
        }
    }
    keyconv_base=&keyconvmaps[keymap_index>>1];
    return 0;
}

static int set_keymap_index(resource_value_t v)
{
int real_index;

    keymap_index=(int)v;
    real_index  =keymap_index>>1;
    keyconv_base=&keyconvmaps[real_index];

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

/* ------------------------------------------------------------------------ */

inline static void set_keyarr(int row, int col, int value)
{
    if (row < 0 || col < 0) return;
    if (value) {
        keyarr    [row] |=   1 << col;
        rev_keyarr[col] |=   1 << row;
    }
    else {
        keyarr    [row] &= ~(1 << col);
        rev_keyarr[col] &= ~(1 << row);
    }
}

void klog(char *s, int i) {
    FILE *fl;
    fl=fopen("output","a");
    fprintf(fl,"%s 0x%x\n",s,i);
    fclose(fl);
}

//extern int warp_mode_enabled;

/* This is not a beautiful way, but the one I know :-) */
void switch_capslock_led_off()
{
    BYTE keyState[256];
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
    keyState[VK_CAPSLOCK] &= ~1;
    WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
}

void ui_attach_disk(int number)
{
    char result[256];
    ui_file_dialog("Attach disk image", "g:",
                   "*.d64;*.d71;*.d81;*.g64;*.g41;*.x64;*.d64;*.d71;*.d81;*.g64;*.g41;*.x64",
                   "Attach", result);
    if (file_system_attach_disk(number, result) < 0)
        ui_error("Cannot attach specified file");
}

void ui_reset() {
//    kbd_clear_keymatrix();
    if (ui_yesno_dialog("Reset","Do you really want to reset the emulated machine?"))
        machine_powerup();  // Hard_reset;
    //        } else {
    //            maincpu_trigger_reset();  // Soft Reset
    //        }
}


void wmChar(HWND hwnd, MPARAM mp1)
{
    USHORT fsFlags    = SHORT1FROMMP(mp1);
    CHAR   usScancode = CHAR4FROMMP (mp1);
    USHORT release;

    // toggle warp mode if ScrlLock is pressed
    if (usScancode==70) resources_set_value("WarpMode",(resource_value_t)(SHORT1FROMMP(mp1)&0x1000));
    // turn capslock led off if capslock is pressed
    if (usScancode==58) switch_capslock_led_off();

    if (fsFlags&KC_ALT) {
        switch (usScancode)
        {
        case  9/*8*/: ui_attach_disk(8); break;
        case 10/*9*/: ui_attach_disk(9); break;
        case 16/*Q*/:
        case 19/*R*/: ui_reset(); break;
        case 31/*S*/:
            resources_get_value("Sound",(resource_value_t *)&release);
            resources_set_value("Sound",(resource_value_t)(!release));
            break;

        case 20/*T*/: ui_OK_dialog("Test", "Na, klappt's?"); break;
        }
    }
    else {
        release = !(fsFlags&KC_KEYUP); // release = !(fsFlags&KC_KEYUP)
        if (!joystick_handle_key(usScancode, release)) {
            set_keyarr(keyconv_base->map[usScancode].row,
                       keyconv_base->map[usScancode].column,
                       release);
            if (keyconv_base->map[usScancode].vshift)
                set_keyarr(keyconv_base->virtual_shift_row,
                           keyconv_base->virtual_shift_column,
                           release);
        }
    }
    //    klog("usScancode",scancode);
    //    klog("release",SHORT1FROMMP(mp1));
    //    klog("row",keyconv_base->map[scancode].row);
    //    klog("col",keyconv_base->map[scancode].column);
}

/* ------------------------------------------------------------------------ */

void kbd_clear_keymatrix(void)
{
    int i;
    for (i=0; i<KBD_ROWS; i++) keyarr[i]=0;
    for (i=0; i<KBD_COLS; i++) rev_keyarr[i]=0;
}

