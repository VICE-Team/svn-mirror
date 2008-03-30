/*
 * keyboard.c - Common keyboard emulation.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
 *
 * Based on old code by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Bernhard Kuhn <kuhn@eikon.e-technik.tu-muenchen.de>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "alarm.h"
#include "archdep.h"
#include "joystick.h"
#include "kbd.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "resources.h"
#include "sysfile.h"
#include "types.h"
#include "utils.h"


#define KEYBOARD_RAND() (rand() & 0x3fff)

/* Keyboard array.  */
int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];

/* Keyboard status to be latched into the keyboard array.  */
static int latch_keyarr[KBD_ROWS];
static int latch_rev_keyarr[KBD_COLS];

static alarm_t keyboard_alarm;

static log_t keyboard_log = LOG_ERR;

static void keyboard_latch_matrix(CLOCK offset)
{
    alarm_unset(&keyboard_alarm);
    alarm_context_update_next_pending(keyboard_alarm.context);

    memcpy(keyarr, latch_keyarr, sizeof(keyarr));
    memcpy(rev_keyarr, latch_rev_keyarr, sizeof(rev_keyarr));
}

/*-----------------------------------------------------------------------*/

void keyboard_set_keyarr(int row, int col, int value)
{
    if (row < 0 || col < 0)
        return;
    if (value) {
        latch_keyarr[row] |= 1 << col;
        latch_rev_keyarr[col] |= 1 << row;
    } else {
        latch_keyarr[row] &= ~(1 << col);
        latch_rev_keyarr[col] &= ~(1 << row);
    }

    alarm_set(&keyboard_alarm, maincpu_clk + KEYBOARD_RAND());
}

void keyboard_set_keyarr_and_latch(int row, int col, int value)
{
    if (row < 0 || col < 0)
        return;
    if (value) {
        latch_keyarr[row] |= 1 << col;
        latch_rev_keyarr[col] |= 1 << row;
    } else {
        latch_keyarr[row] &= ~(1 << col);
        latch_rev_keyarr[col] &= ~(1 << row);
    }
    memcpy(keyarr, latch_keyarr, sizeof(keyarr));
    memcpy(rev_keyarr, latch_rev_keyarr, sizeof(rev_keyarr));
}

void keyboard_clear_keymatrix(void)
{
    memset(keyarr, 0, sizeof(keyarr));
    memset(rev_keyarr, 0, sizeof(rev_keyarr));
    memset(latch_keyarr, 0, sizeof(latch_keyarr));
    memset(latch_rev_keyarr, 0, sizeof(latch_rev_keyarr));
}

/*-----------------------------------------------------------------------*/

#ifdef COMMON_KBD

/* Is the resource code ready to load the keymap?  */
static int load_keymap_ok;

/* Memory size of array in sizeof(keyconv_t), 0 = static.  */
static int keyc_mem = 0;

/* Number of convs used in sizeof(keyconv_t).  */
static int keyc_num = 0;

/* Two possible restore keys.  */
static signed long key_ctrl_restore1 = -1;
static signed long key_ctrl_restore2 = -1;

/* 40/80 column key.  */
signed long key_ctrl_column4080 = -1;
key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;

/* CAPS (ASCII/DIN) key.  */
signed long key_ctrl_caps = -1;
key_ctrl_caps_func_t key_ctrl_caps_func = NULL;

static keyboard_conv_t *keyconvmap = NULL;

static int kbd_lshiftrow;
static int kbd_lshiftcol;
static int kbd_rshiftrow;
static int kbd_rshiftcol;

keyboard_conv_t joykeys[2][10];

/*-----------------------------------------------------------------------*/

static int left_shift_down, right_shift_down, virtual_shift_down;

void keyboard_key_pressed(signed long key)
{
    /* Restore */
    if (((key == key_ctrl_restore1) || (key == key_ctrl_restore2))
        && machine_set_restore_key(1))
        return;

    if (key == key_ctrl_column4080) {
        if (key_ctrl_column4080_func != NULL)
            key_ctrl_column4080_func();
        return;
    }

    if (key == key_ctrl_caps) {
        if (key_ctrl_caps_func != NULL)
            key_ctrl_caps_func();
        return;
    }

    if (joystick_check_set(key, 1))
        return;
    if (joystick_check_set(key, 2))
        return;

    if (keyconvmap) {
        int i;

        for (i = 0; keyconvmap[i].sym != 0; ++i) {
            if (key == keyconvmap[i].sym) {
                int row = keyconvmap[i].row;
                int column = keyconvmap[i].column;

                if (row >= 0) {
                    keyboard_set_keyarr(row, column, 1);

                    if (keyconvmap[i].shift == NO_SHIFT) {
                        keyboard_set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 0);
                        keyboard_set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 0);
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
                    return;
                }
            }
        }
    }
}

void keyboard_key_released(signed long key)
{
    /* Restore */
    if (((key == key_ctrl_restore1) || (key == key_ctrl_restore2))
        && machine_set_restore_key(0))
        return;

    if (joystick_check_clr(key, 1))
        return;
    if (joystick_check_clr(key, 2))
        return;

    if (keyconvmap) {
        int i;

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
    return;                     /* KeyRelease */
}

void keyboard_key_clear(void)
{
    keyboard_clear_keymatrix();
    memset(joystick_value, 0, sizeof(joystick_value));
    virtual_shift_down = left_shift_down = right_shift_down = 0;
    joystick_joypad_clear();
    return;
}

/*-----------------------------------------------------------------------*/

static int keyboard_parse_keymap(const char *filename);

static void keyboard_parse_keyword(char *buffer)
{
    char *key, *p;
    signed long sym;
    int i, j;

    key = strtok(buffer + 1, " \t:");
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
        key_ctrl_restore1 = -1;
        key_ctrl_restore2 = -1;
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 10; j++) {
                joykeys[i][j].sym = -1;
            }
        }
    } else if (!strcmp(key, "INCLUDE")) {
        key = strtok(NULL, " \t");
        keyboard_parse_keymap(key);
    } else if (!strcmp(key, "UNDEF")) {
        /* TODO: this only unsets from the main table, not for joysticks */
        key = strtok(NULL, " \t");
        sym = kbd_arch_keyname_to_keynum(key);
        if (sym >= 0) {
            for (i = 0; i < keyc_num; i++) {
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

    joystick_joypad_clear();
}

static void keyboard_parse_entry(char *buffer)
{
    char *key, *p;
    signed long sym;
    int row, col, i;
    int shift = 0;
    keyboard_conv_t *kp;

    key = strtok(buffer, " \t:");

    sym = kbd_arch_keyname_to_keynum(key);

    if (sym < 0) {
        log_error(keyboard_log, _("Could not find key `%s'!"), key);
        return;
    }

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
                        if (keyc_num >= keyc_mem) {
                            i = keyc_mem * 1.5;
                            kp = xrealloc(keyconvmap,
                                          (i + 1) * sizeof(keyboard_conv_t));
                            keyconvmap = kp;
                            keyc_mem = i;
                        }
                        if (keyc_num < keyc_mem) {
                            keyconvmap[keyc_num].sym = sym;
                            keyconvmap[keyc_num].row = row;
                            keyconvmap[keyc_num].column = col;
                            keyconvmap[keyc_num].shift = shift;
                            keyconvmap[++keyc_num].sym = 0;
                        }
                    }
                } else { /* row < 0 */
                    if (row >= -2 && col >= 0 && col < 10) {
                        joykeys[-row - 1][col].sym = sym;
                    } else
                    if (row == -3 && col == 0) {
                        key_ctrl_restore1 = sym;
                    } else
                    if (row == -3 && col == 1) {
                        key_ctrl_restore2 = sym;
                    } else
                    if (row == -4 && col == 0) {
                        key_ctrl_column4080 = sym;
                    } else
                    if (row == -4 && col == 1) {
                        key_ctrl_caps = sym;
                    } else {
                        log_error(keyboard_log,
                                  _("Bad row/column value (%d/%d) for keysym `%s'."),
                                  row, col, key);
                    }
                }
            }
        }
    }
}


static int keyboard_parse_keymap(const char *filename)
{
    FILE *fp;
    char *complete_path;
    char buffer[1000];

    fp = sysfile_open(filename, &complete_path, MODE_READ_TEXT);

    if (fp == NULL) {
        free(complete_path);
        return -1;
    }

    log_message(keyboard_log, _("Loading keymap `%s'."), complete_path);

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
                keyboard_parse_keyword(buffer);
                break;
              default:
                /* table entry handling */
                keyboard_parse_entry(buffer);
                break;
            }
        }
    } while(!feof(fp));
    fclose(fp);

    free(complete_path);

    return 0;
}

static int keyboard_keymap_load(const char *filename)
{
    keyboard_conv_t *p;

    if (filename == NULL) {
        return -1;
    }

    /* Dynamicalize keymap table.  */
    if (!keyc_mem) {
        if (keyconvmap) {
            p = xmalloc((keyc_num + 1) * sizeof(keyboard_conv_t));
            memcpy(p, keyconvmap, (keyc_num + 1) * sizeof(keyboard_conv_t));
            keyc_mem = keyc_num;
            keyconvmap = p;
        } else {
            keyconvmap = xmalloc(151 * sizeof(keyboard_conv_t));
            keyc_num = 0;
            keyc_mem = 150;
            keyconvmap[0].sym = 0;
        }
    }

    if (keyc_mem)
        return keyboard_parse_keymap(filename);

    return 0;
}

/*-----------------------------------------------------------------------*/

int keyboard_keymap_dump(const char *filename)
{
    FILE *fp;
    int i, j;

    if (filename == NULL)
        return -1;

    fp = fopen(filename, MODE_WRITE_TEXT);

    if (fp == NULL)
        return -1;

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
                kbd_arch_keynum_to_keyname(keyconvmap[i].sym),
                keyconvmap[i].row, keyconvmap[i].column,
                keyconvmap[i].shift);
    }

    /* Dump joystick keys.  */
    for (i = 0; i<2; i++) {
        for (j = 0; j < 10; j++) {
            if (joykeys[i][j].sym != -1) {
                fprintf(fp, "%s %d %d\n",
                        kbd_arch_keynum_to_keyname(joykeys[i][j].sym),
                        -1 - i,j);
            }
        }
    }
    /* Dump control keys. */
    if (key_ctrl_restore1 != -1)
        fprintf(fp, "%s -3 0\n",
                kbd_arch_keynum_to_keyname(key_ctrl_restore1));
    if (key_ctrl_restore2 != -1)
        fprintf(fp, "%s -3 1\n",
                kbd_arch_keynum_to_keyname(key_ctrl_restore2));
    fclose(fp);

    return 0;
}

/*-----------------------------------------------------------------------*/

int keyboard_set_keymap_index(resource_value_t v, void *param)
{
    const char *name, *resname;

    resname = machine_keymap_res_name_list[(unsigned int)v];

    if (resources_get_value(resname, (resource_value_t *)&name) < 0)
        return -1;

    if (load_keymap_ok) {
        if (keyboard_keymap_load(name) >= 0) {
            machine_keymap_index = (unsigned int)v;
            return 0;
        } else {
            log_error(keyboard_log, _("Cannot load keymap `%s'."),
                      name ? name : _("(null)"));
        }
        return -1;
    }

    machine_keymap_index = (unsigned int)v;
    return 0;
}

int keyboard_set_keymap_file(resource_value_t v, void *param)
{
    int oldindex, newindex;

    newindex = (int)param;

    if (newindex >= machine_num_keyboard_mappings())
        return -1;

    if (resources_get_value("KeymapIndex", (resource_value_t *)&oldindex) < 0)
        return -1;

    if (util_string_set(&machine_keymap_file_list[newindex], (const char *)v))
        return 0;

    /* reset oldindex -> reload keymap file if this keymap is active */
    if (oldindex == newindex)
        resources_set_value("KeymapIndex", (resource_value_t)oldindex);

    return 0;
}

/*-----------------------------------------------------------------------*/

void keyboard_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}

void keyboard_register_caps_key(key_ctrl_caps_func_t func)
{
    key_ctrl_caps_func = func;
}
#endif

/*-----------------------------------------------------------------------*/

void keyboard_init(void)
{
#ifdef COMMON_KBD
    int i, j;
#endif

    keyboard_log = log_open("Keyboard");

    alarm_init(&keyboard_alarm, maincpu_alarm_context,
               "Keyboard", keyboard_latch_matrix);

#ifdef COMMON_KBD
    for (i = 0; i < 2; i++)
        for (j = 0; j < 10; j++)
            joykeys[i][j].sym = -1;

    kbd_arch_init();

    load_keymap_ok = 1;
    keyboard_set_keymap_index((resource_value_t)machine_keymap_index, NULL);
#endif
}

