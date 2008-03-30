/*
 * kbd.c - X11 keyboard driver.
 *
 * Written by
 *  Ettore Perazzoli   (ettore@comm2000.it)
 *  Jouko Valta        (jopi@stekt.oulu.fi)
 *  André Fachat       (fachat@physik.tu-chemnitz.de)
 *  Bernhard Kuhn      (kuhn@eikon.e-technik.tu-muenchen.de)
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

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmachine.h"
#include "video.h"
#include "ui.h"
#include "resources.h"
#include "memutils.h"
#include "kbd.h"
#include "kbdef.h"
#include "interrupt.h"
#include "sysfile.h"
#include "utils.h"
#include "joystick.h"

/* ------------------------------------------------------------------------- */

/* #define DEBUG_KBD */

/* Keyboard array. */
int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];

/* Joystick status */

BYTE joy[3] = { 0, 0, 0 };
static int custom_joykeys[10];
static keyconv custom_joykeys_backup[10];

/* Shift status */
static short kbd_lshiftrow = 0;
static short kbd_lshiftcol = 0;
static short kbd_rshiftrow = 0;
static short kbd_rshiftcol = 0;

static int joy1pad_status[10];
static int joy2pad_status[10];

static int joypad_bits[10] = {
    0x10, 0x06, 0x02, 0x0a, 0x04, 0x00, 0x08, 0x05, 0x01, 0x09
};

static int left_shift_down, right_shift_down, virtual_shift_down;

static const char *default_keymap_name;

/* ------------------------------------------------------------------------- */

static inline void set_keyarr(int row, int col, int value)
{
    if (value) {
#ifdef DEBUG_KBD
	printf("Set keyarr row %d col %d\n", row, col);
#endif
	keyarr[row] |= 1 << col;
	rev_keyarr[col] |= 1 << row;
    } else {
#ifdef DEBUG_KBD
	printf("Unset keyarr row %d col %d\n", row, col);
#endif
	keyarr[row] &= ~(1 << col);
	rev_keyarr[col] &= ~(1 << row);
    }
}

int joyreleaseval(int column, int *status)
{
    int val = 0;

    switch (column) {
      case 1:
	val = (status[2] ? 0 : 0x2) | (status[4] ? 0 : 0x4);
	break;
      case 3:
	val = (status[2] ? 0 : 0x2) | (status[6] ? 0 : 0x8);
	break;
      case 7:
	val = (status[4] ? 0 : 0x4) | (status[8] ? 0 : 0x1);
	break;
      case 9:
	val = (status[8] ? 0 : 0x1) | (status[6] ? 0 : 0x8);
	break;
      default:
	val = joypad_bits[column];
	break;
    }
    return ~val;
}

void kbd_event_handler(Widget w, XtPointer client_data, XEvent *report,
		       Boolean *ctd)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;
    int count, i;

    count = XLookupString(&report->xkey, buffer, 20, &key, &compose);

    if (key == XK_Tab) {	/* Restore */
	if (report->type == KeyPress)
	    maincpu_set_nmi(I_RESTORE, 1);
	else if (report->type == KeyRelease)
	    maincpu_set_nmi(I_RESTORE, 0);
	return;
    }

    switch (report->type) {

      case KeyPress:
	for (i = 0; keyconvmap[i].sym != 0; ++i) {
	    if (key == keyconvmap[i].sym) {
		int row = keyconvmap[i].row;
		int column = keyconvmap[i].column;

		if (row < 0) {
                    if ((row == -1) && (joystick_port_map[0] == JOYDEV_NUMPAD)) {
                        if (joypad_bits[column]) {
                            joy[1] |= joypad_bits[column];
                            joy1pad_status[column]=1;
                        } else {
                            joy[1] = 0;
                            memset(joy1pad_status, 0, sizeof(joy1pad_status));
                        }
                    } else if ((row == -1) && (joystick_port_map[1] == JOYDEV_NUMPAD)) {
                        if (joypad_bits[column]) {
                            joy[2] |= joypad_bits[column];
                            joy2pad_status[column]=1;
                        } else {
                            joy[2] = 0;
                            memset(joy2pad_status, 0,
                                   sizeof(joy2pad_status));
                        }
                    }
                    if ((row == -2) && (joystick_port_map[0] == JOYDEV_CUSTOM_KEYS)) {
                        if (joypad_bits[column]) {
                            joy[1] |= joypad_bits[column];
                            joy1pad_status[column]=1;
                        } else {
                            joy[1] = 0;
                            memset(joy1pad_status, 0, sizeof(joy1pad_status));
                        }
                    } else if ((row == -2) && (joystick_port_map[1] == JOYDEV_CUSTOM_KEYS)) {
                        if (joypad_bits[column]) {
                            joy[2] |= joypad_bits[column];
                            joy2pad_status[column]=1;
                        } else {
                            joy[2] = 0;
                            memset(joy2pad_status, 0,
                                   sizeof(joy2pad_status));
                        }
                    }
                    break;
		}
		if (row >= 0) {
                    set_keyarr(row, column, 1);

                    if (keyconvmap[i].shift == NO_SHIFT) {
                        set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 0);
                        set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 0);
                    } else {
                        if (keyconvmap[i].shift & VIRTUAL_SHIFT)
                            virtual_shift_down++;
                        if (keyconvmap[i].shift & LEFT_SHIFT)
                            left_shift_down++;
                        if (left_shift_down + virtual_shift_down > 0)
                            set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 1);
                        if (keyconvmap[i].shift & RIGHT_SHIFT)
                            right_shift_down++;
                        if (right_shift_down > 0)
                            set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 1);
                    }
                    break;
		}
	    }
	}
	break;			/* KeyPress */

      case KeyRelease:
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
               avoid this X11 oddity? */
	    memset(keyarr, 0, sizeof(keyarr));
	    memset(rev_keyarr, 0, sizeof(rev_keyarr));
	    virtual_shift_down = 0;
	    /* TODO: do we have to cleanup joypads here too? */
	}

	for (i = 0; keyconvmap[i].sym != 0; i++) {
	    if (key == keyconvmap[i].sym) {
		int row = keyconvmap[i].row;
		int column = keyconvmap[i].column;

		if (row < 0) {
                    if ((row == -1) && (joystick_port_map[0] == JOYDEV_NUMPAD)) {
                        joy[1] &= joyreleaseval(column, joy1pad_status);
                        joy1pad_status[column] = 0;
                    } else if ((row == -1) && (joystick_port_map[1] == JOYDEV_NUMPAD)) {
                        joy[2] &= joyreleaseval(column, joy2pad_status);
                        joy2pad_status[column] = 0;
                    }
                    if ((row == -2) && (joystick_port_map[0] == JOYDEV_CUSTOM_KEYS)) {
                        joy[1] &= joyreleaseval(column, joy1pad_status);
                        joy1pad_status[column] = 0;
                    } else if ((row == -2) && (joystick_port_map[1] == JOYDEV_CUSTOM_KEYS)) {
                        joy[2] &= joyreleaseval(column, joy2pad_status);
                        joy2pad_status[column] = 0;
                    }
		} else {
                    set_keyarr(row, column, 0);
                    if (keyconvmap[i].shift & VIRTUAL_SHIFT)
                        virtual_shift_down--;
                    if (keyconvmap[i].shift & LEFT_SHIFT)
                        left_shift_down--;
                    if (keyconvmap[i].shift & RIGHT_SHIFT)
                        right_shift_down--;
		}
	    }
	}

	/* Map shift keys. */
	if (right_shift_down > 0)
	    set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 1);
	else
	    set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 0);
	if (left_shift_down + virtual_shift_down > 0)
	    set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 1);
	else
	    set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 0);
	break;			/* KeyRelease */

      case LeaveNotify:
	/* Clean up. */
	memset(keyarr, 0, sizeof(keyarr));
	memset(rev_keyarr, 0, sizeof(rev_keyarr));
	memset(joy, 0, sizeof(joy));
	virtual_shift_down = left_shift_down = right_shift_down = 0;
	memset(joy1pad_status, 0, sizeof(joy1pad_status));
	memset(joy2pad_status, 0, sizeof(joy2pad_status));
	break;			/* LeaveNotify */

      default:
	break;

    }				/* switch */
}

/* ------------------------------------------------------------------------ */

/* Handling of different keyboard mappings.  */

/* Memory size of array in sizeof(keyconv), 0 = static.  */
static int keyc_mem = 0;
/* Number of convs used in sizeof(keyconv).  */
static int keyc_num = 0;

int kbd_init(const char *keymap_name)
{
    default_keymap_name = keymap_name;

    if (kbd_load_keymap(NULL) < 0) {
	fprintf(stderr,"Couldn't load default keymap file `%s', aborting!\n",
                keymap_name);
	return -1;
    }

    return 0;
}

static int kbd_parse_keymap(const char *filename);

static void kbd_parse_keyword(char *buffer)
{
    char *key, *p;
    KeySym sym;
    int i;

    key = strtok(buffer+1, " \t:");
    if (!strcmp(key, "LSHIFT")) {
	p = strtok(NULL, " \t,");
	if (p) {
            kbd_lshiftrow = atoi(p);
            p = strtok(NULL, " \t,");
            if (p) {
                kbd_lshiftcol = atoi(p);
            }
	}
    } else if (!strcmp(key, "RSHIFT")) {
        p = strtok(NULL, " \t,");
        if (p) {
            kbd_rshiftrow = atoi(p);
            p = strtok(NULL, " \t,");
            if (p) {
                kbd_rshiftcol = atoi(p);
            }
        }
    } else if (!strcmp(key, "CLEAR")) {
        keyc_num = 0;
        keyconvmap[0].sym = 0;
    } else if (!strcmp(key, "INCLUDE")) {
        key = strtok(NULL, " \t");
        kbd_parse_keymap(key);
    } else if (!strcmp(key, "UNDEF")) {
        key = strtok(NULL, " \t");
        sym = XStringToKeysym(key);
        if (sym != NoSymbol) {
            for(i=0;i<keyc_num;i++) {
                if (keyconvmap[i].sym == sym) {
                    if (keyc_num) {
                        keyconvmap[i] = keyconvmap[--keyc_num];
                    }
                    keyconvmap[keyc_num].sym = 0;
                    break;
                }
            }
        }
    }

    memset(joy1pad_status, 0, sizeof(joy1pad_status));
    memset(joy1pad_status, 0, sizeof(joy2pad_status));
}

static void kbd_parse_entry(char *buffer)
{
    char *key, *p;
    KeySym sym;
    int row, col, i;
    int shift = 0;
    keyconv *kp;

    key = strtok(buffer, " \t:");
    sym = XStringToKeysym(key);
    if (sym == NoSymbol) {
	printf("Could not find KeySym `%s'!\n", key);
    } else {
	p = strtok(NULL, " \t,");
	if (p) {
            row = strtol(p,NULL,10);
            p = strtok(NULL, " \t,");
            if (p) {
                col = atoi(p);
                p = strtok(NULL, " \t");
                if (p || row < 0) {
                    if (p)
                        shift = atoi(p);
                    if (row>=0 || (row>=-2 && col>=0 && col<10)) {
                        for(i=0;keyconvmap[i].sym;i++) {
                            if (sym == keyconvmap[i].sym) {
                                if (row >= -1) {
                                    keyconvmap[i].row = row;
                                    keyconvmap[i].column = col;
                                    keyconvmap[i].shift = shift;
                                    break;
                                } else {
                                    custom_joykeys[col] = i;
                                    custom_joykeys_backup[col].row = keyconvmap[i].row;
                                    custom_joykeys_backup[col].column = keyconvmap[i].column;
                                    custom_joykeys_backup[col].shift = keyconvmap[i].shift;
                                }
                            }
                        }

                        /* not in table -> add */
                        if (i>=keyc_num && row != -2) {
                            /* table too small -> realloc */
                            if (keyc_num>=keyc_mem) {
                                i = keyc_mem * 1.5;
                                kp = xrealloc(keyconvmap,
                                              (i + 1) * sizeof(keyconv));
                                keyconvmap = kp;
                                keyc_mem = i;
                            }
                            if (keyc_num<keyc_mem) {
                                keyconvmap[keyc_num].sym = sym;
                                keyconvmap[keyc_num].row = row;
                                keyconvmap[keyc_num].column = col;
                                keyconvmap[keyc_num].shift = shift;
                                keyconvmap[++keyc_num].sym = 0;
                            }
                        }
                    } else {
                        fprintf(stderr,
                                "Bad row/column value (%d/%d) for keysym %s\n",
                                row, col, key);
                    }
                }
            }
	}
    }
}

static int kbd_parse_keymap(const char *filename)
{
    FILE *fp;
    char *complete_path;
    char buffer[1000];

    if (!filename)
        return -1;

    fp = sysfile_open(filename, &complete_path);
    if (!fp) {
        perror(complete_path);
        free(complete_path);
        return -1;
    }

    printf("Parsing keymap `%s'.\n", complete_path);

    do {
        buffer[0] = 0;
        if (fgets(buffer, 999, fp)) {
            buffer[strlen(buffer)-1] = 0;
            switch(*buffer) {
              case 0:
              case '#':
                break;
              case '!':
                /* keyword handling */
                kbd_parse_keyword(buffer);
                break;
              default:
                /* table entry handling */
                kbd_parse_entry(buffer);
                break;
            }
        }
    } while(!feof(fp));
    fclose(fp);

    free(complete_path);

    return 0;
}

int kbd_load_keymap(const char *filename)
{
    keyconv *p;

    if (filename == NULL)
        filename = default_keymap_name;

    /* Dynamicalize keymap table.  */
    if (!keyc_mem) {
        if (keyconvmap) {
            p = xmalloc((keyc_num+1) * sizeof(keyconv));
            memcpy(p, keyconvmap, (keyc_num + 1) * sizeof(keyconv));
            keyc_mem = keyc_num;
            keyconvmap = p;
        } else {
            keyconvmap = xmalloc(151 * sizeof(keyconv));
            keyc_num = 0;
            keyc_mem = 150;
            keyconvmap[0].sym = 0;
        }
    }

    if (keyc_mem)
        return kbd_parse_keymap(filename);

    return 0;
}

int kbd_dump_keymap(const char *filename)
{
    FILE *fp;
    int i;

    if (!filename)
        return -1;
    printf("kbd_dump_keymap() called, keyb file='%s'\n", filename);

    fp = fopen(filename, "w");

    if (!fp) {
        return -1;
    } else {
        fprintf(fp, "# VICE keyboard mapping file\n"
                "#\n"
                "# A Keyboard map is read in as patch to the current map.\n"
                "#\n"
                "# File format:\n"
                "# - comment lines start with '#'\n"
                "# - keyword lines start with '!keyword'\n"
                "# - normal line has 'keysym row column shiftflag'\n"
                "#\n"
                "# keywords and their lines are:\n"
                "# '!INCLUDE filename'    read file as mapping file\n"
                "# '!LSHIFT row col'      left shift keyboard row/column\n"
                "# '!RSHIFT row col'      right shift keyboard row/column\n"
                "# '!UNDEF keysym'        remove keysym from table\n"
                "#\n"
                "# shiftflag can have the values:\n"
                "# 0      key is not shifted for this keysym\n"
                "# 1      key is shifted for this keysym\n"
                "# 2      left shift\n"
                "# 4      right shift\n"
                "# 8      key can be shifted or not with this keysym\n"
                "#\n"
            );
        fprintf(fp, "!CLEAR\n");
        fprintf(fp, "!LSHIFT %d %d\n", kbd_lshiftrow, kbd_lshiftcol);
        fprintf(fp, "!RSHIFT %d %d\n", kbd_rshiftrow, kbd_rshiftcol);
        for(i = 0; keyconvmap[i].sym; i++) {
            fprintf(fp, "%s %d %d %d\n",
                    XKeysymToString(keyconvmap[i].sym),
                    keyconvmap[i].row, keyconvmap[i].column,
                    keyconvmap[i].shift);
        }
        fclose(fp);
    }
    return 0;
}

void kbd_add_custom_joykeys(void)
{
   int i,j;

   for (i=0;i<10;i++) {
       j = custom_joykeys[i];

       keyconvmap[j].row = -2;
       keyconvmap[j].column = i;
   }
}

void kbd_remove_custom_joykeys(void)
{
   int i,j;

   for (i=0;i<10;i++) {
       j = custom_joykeys[i];

       keyconvmap[j].row = custom_joykeys_backup[i].row;
       keyconvmap[j].column = custom_joykeys_backup[i].column;
       keyconvmap[j].shift = custom_joykeys_backup[i].shift;
   }
}
