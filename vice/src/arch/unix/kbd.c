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

#include "vice.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmdline.h"
#include "interrupt.h"
#include "joystick.h"
#include "kbd.h"
#include "kbdef.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "sysfile.h"
#include "ui.h"
#include "utils.h"
#include "video.h"

/* ------------------------------------------------------------------------- */

/* #define DEBUG_KBD */
/* #define DEBUG_JOY */

static KeySym key_ctrl_restore1 = XK_Prior;
#ifdef XK_Page_Up
static KeySym key_ctrl_restore2 = XK_Page_Up;
#else
static KeySym key_ctrl_restore2 = NoSymbol;
#endif

/* Joystick status */

BYTE joystick_value[3] = { 0, 0, 0 };

static keyconv joykeys  [2][10];
static int joypad_status[2][10];

/* Shift status */
static short kbd_lshiftrow = 0;
static short kbd_lshiftcol = 0;
static short kbd_rshiftrow = 0;
static short kbd_rshiftcol = 0;

/* Meta status (used to filter out keypresses when meta is pressed).  */
static int meta_count = 0;

static int joypad_bits[10] = {
    0x10, 0x06, 0x02, 0x0a, 0x04, 0x00, 0x08, 0x05, 0x01, 0x09
};

static int left_shift_down, right_shift_down, virtual_shift_down;

/* ------------------------------------------------------------------------- */

/* Resource handling.  */

static int keymap_index;
static int load_keymap_ok = 0;

static int set_keymap_index(resource_value_t v)
{
    const char *name, *resname = keymap_res_name_list[(int) v];

    if (resources_get_value(resname, (resource_value_t*) &name) < 0)
 	return -1;

    if (load_keymap_ok) { /* to reduce multiple parsing during startup */
        if (kbd_load_keymap(name) >= 0) {
	    keymap_index = (int) v;
	    return 0;
        } else {
            log_error(LOG_DEFAULT, "Cannot load keymap `%s'.",
                      name ? name : "(null)");
        }
        return -1;
    }
    keymap_index = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "KeymapIndex", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &keymap_index, set_keymap_index },
    { NULL }
};

int do_kbd_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-keymap", SET_RESOURCE, 1, NULL, NULL, "KeymapIndex", NULL,
      "<number>", "Specify index of keymap file (0=symbol, 1=positional)" },
    { NULL },
};

int do_kbd_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

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

static int check_set_joykeys(KeySym key, int joynum)
{
    int column, joyport;

    if (joystick_port_map[0] == joynum)
        joyport = 1;
    else if (joystick_port_map[1] == joynum)
        joyport = 2;
    else
        return 0;

    joynum--;

    for (column = 0; column < 10; column++) {
        if (key == joykeys[joynum][column].sym) {
            if (joypad_bits[column]) {
                joystick_value[joyport] |= joypad_bits[column];
                joypad_status[joynum][column]=1;
            } else {
                joystick_value[joyport] = 0;
                memset(joypad_status[joynum], 0, sizeof(joypad_status[joynum]));
            }
#ifdef DEBUG_JOY
            log_debug("got joyport %d, joynum %d, keysym=`%s'.",
                      joyport, joynum,
                      XKeysymToString(joykeys[joynum][column].sym));
#endif
            return 1;
        }
    }
    return 0;
}

static int check_clr_joykeys(KeySym key, int joynum)
{
    int column, joyport;

    if (joystick_port_map[0] == joynum)
        joyport = 1;
    else if (joystick_port_map[1] == joynum)
        joyport = 2;
    else
        return 0;

    joynum--;

    for (column = 0; column < 10; column++) {
        if (key == joykeys[joynum][column].sym) {
            joystick_value[joyport] &= joyreleaseval(column,
                                                     joypad_status[joynum]);
            joypad_status[joynum][column] = 0;
            return 1;
        }
    }
    return 0;
}

void kbd_event_handler(Widget w, XtPointer client_data, XEvent *report,
                       Boolean *ctd)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;
    int count, i;

    count = XLookupString(&report->xkey, buffer, 20, &key, &compose);

    if (key != NoSymbol
        && ((key == key_ctrl_restore1)
            || (key == key_ctrl_restore2))) {	/* Restore */
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

/* ------------------------------------------------------------------------ */

/* Handling of different keyboard mappings.  */

/* list with resource names for different keymap filenames */
const char **keymap_res_name_list;

/* Memory size of array in sizeof(keyconv), 0 = static.  */
static int keyc_mem = 0;
/* Number of convs used in sizeof(keyconv).  */
static int keyc_num = 0;

int kbd_init(void)
{
    int i,j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 10; j++)
            joykeys[i][j].sym = NoSymbol;

    /* load current keymap table */
    load_keymap_ok = 1;
    set_keymap_index((resource_value_t)keymap_index);

    return 0;
}

int c64_kbd_init(void)
{
    return kbd_init();
}

int c128_kbd_init(void)
{
    return kbd_init();
}

int vic20_kbd_init(void)
{
    return kbd_init();
}

int pet_kbd_init(void)
{
    return kbd_init();
}

int c610_kbd_init(void)
{
    return kbd_init();
}

static int kbd_parse_keymap(const char *filename);

static void kbd_parse_keyword(char *buffer)
{
    char *key, *p;
    KeySym sym;
    int i, j;

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
	key_ctrl_restore1 = NoSymbol;
	key_ctrl_restore2 = NoSymbol;
	for (i=0;i<2;i++) {
            for (j=0;j<10;j++) {
                joykeys[i][j].sym = NoSymbol;
            }
        }
    } else if (!strcmp(key, "INCLUDE")) {
        key = strtok(NULL, " \t");
        kbd_parse_keymap(key);
    } else if (!strcmp(key, "UNDEF")) {
	/* TODO: this only unsets from the main table, not for joysticks */
        key = strtok(NULL, " \t");
        sym = XStringToKeysym(key);
        if (sym != NoSymbol) {
            for (i=0;i<keyc_num;i++) {
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

    memset(joypad_status, 0, sizeof(joypad_status));
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
	log_error(LOG_DEFAULT, "Could not find KeySym `%s'!", key);
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
                    if (row >= 0) {
                        for (i = 0; keyconvmap[i].sym; i++) {
                            if (sym == keyconvmap[i].sym) {
                                keyconvmap[i].row = row;
                                keyconvmap[i].column = col;
                                keyconvmap[i].shift = shift;
                                break;
                            }
                        }

                        /* not in table -> add */
                        if (i >= keyc_num) {
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
                    } else  /* row < 0 */
                        if (row>=-2 && col>=0 && col<10) {
                            joykeys[-row-1][col].sym = sym;
                        } else
			if (row == -3 && col == 0) {
			    key_ctrl_restore1 = sym;
			} else
			if (row == -3 && col == 1) {
			    key_ctrl_restore2 = sym;
			} else {
                            log_error(LOG_DEFAULT,
                                      "Bad row/column value (%d/%d) for keysym `%s'.",
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
        free(complete_path);
        return -1;
    }

    log_message(LOG_DEFAULT, "Loading keymap `%s'.", complete_path);

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

    if (filename == NULL) {
	return -1;
    }

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
    int i,j;

    if (!filename)
        return -1;

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
		"# if row is negative, column gives numpad-equivalent number\n"
		"# for joystick emulation (e.g. col==8 -> up)\n"
		"# row == -1 : 'numpad' joystick emulation\n"
		"# row == -2 : 'custom' joystick emulation\n"
		"#\n"
		"# if row is -3, then we define control keys:\n"
		"# col == 0  : Restore key (C64/C128)\n"
                "#\n\n"
            );
        fprintf(fp, "!CLEAR\n");
        fprintf(fp, "!LSHIFT %d %d\n", kbd_lshiftrow, kbd_lshiftcol);
        fprintf(fp, "!RSHIFT %d %d\n\n", kbd_rshiftrow, kbd_rshiftcol);

        /* Dump "normal" keys.  */
        for (i = 0; keyconvmap[i].sym; i++) {
            fprintf(fp, "%s %d %d %d\n",
                    XKeysymToString(keyconvmap[i].sym),
                    keyconvmap[i].row, keyconvmap[i].column,
                    keyconvmap[i].shift);
        }

        /* Dump joystick keys.  */
        for (i = 0; i<2; i++) {
	    for (j = 0; j < 10; j++) {
                if (joykeys[i][j].sym != NoSymbol) {
                    fprintf(fp, "%s %d %d\n",
                            XKeysymToString(joykeys[i][j].sym),
			    -1 - i,j);
		}
            }
        }
	/* Dump control keys. */
	if (key_ctrl_restore1 != NoSymbol)
	    fprintf(fp, "%s -3 0\n", XKeysymToString(key_ctrl_restore1));
	if (key_ctrl_restore2 != NoSymbol)
	    fprintf(fp, "%s -3 1\n", XKeysymToString(key_ctrl_restore2));
        fclose(fp);
    }
    return 0;
}

