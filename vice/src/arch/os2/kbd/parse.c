/*
 * parse.c - Parse keyboard mapping file
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

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib.h"
#include "log.h"
#include "util.h"    // util_get_line
#include "sysfile.h" // sysfile_open
#include "parse.h"

convmap keyconvmap;

/* ------------------------------------------------------------------------ */

int load_keymap_file(const char *fname)
{
    FILE *fp;
    int num = 0;
    char *complete_path;

    if (fname == NULL) {
        return -1;
    }

    fp = sysfile_open(fname, &complete_path, "r");

    if (fp == NULL) {
        char *tmp = util_concat(fname, ".vkm", NULL);

        fp = sysfile_open(tmp, &complete_path, "r");
        lib_free(tmp);
    }

    if (fp == NULL) {
        return -1;
    } else {
        log_message(LOG_DEFAULT, "Loading keymap `%s'.", complete_path);
    }

    lib_free(complete_path);

    while (!feof(fp)) {
        char buffer[81];

        if (util_get_line(buffer, 80, fp)) {
            switch (buffer[0]) {
                case 0:
                case '/':
                    break;
                case '#':
                case 'S':
                case 'U':
                    // table entry handling
                    {
                        // FIXME: ERROR LOGGING MISSING
                        char *p;
                        char *dummy;
                        unsigned long code1, code2;
                        int row, col, shift;

                        if (keyconvmap.symbolic == 1) {
                            p = strtok(buffer + 1, " \t,");
                            if (!p) {
                                break;
                            }
                            code1 = strtoul(p, &dummy, 10);
                            if (code1 > 0xff) {
                                break;
                            }
                            p = strtok(NULL, " \t:");
                            if (!p) {
                                break;
                            }
                            code2 = strtoul(p, &dummy, 10);
                            if (code2 > 0xff) {
                                break;
                            }
                        } else {
                            p = strtok(buffer+1, " \t:");
                            if (!p) {
                                break;
                            }
                            code1 = strtoul(p, &dummy, 10);
                            if (code1 > 0xff) {
                                break;
                            }
                            code2 = 0;
                        }
                        p = strtok(NULL, " \t,");
                        if (!p) {
                            break;
                        }
                        row = atoi(p);
                        p = strtok(NULL, " \t,");
                        if (!p) {
                            break;
                        }
                        col = atoi(p);

                        p = strtok(NULL, " \t");
                        if (!(p || row < 0)) {
                            break;
                        }
                        if (p) {
                            shift = atoi(p);
                        }
                        {
                            switch (buffer[0])
                            {
                                case '#':
                                    keyconvmap.map[0][num].code = code1 | code2 << 8;
                                    keyconvmap.map[0][num].row = row;
                                    keyconvmap.map[0][num].column = col;
                                    keyconvmap.map[0][num].vshift = shift;
                                    keyconvmap.map[1][num].code = code1 | code2 << 8;
                                    keyconvmap.map[1][num].row = row;
                                    keyconvmap.map[1][num].column = col;
                                    keyconvmap.map[1][num].vshift = shift;
                                    num++;
                                    break;
                                case 'S':
                                    keyconvmap.map[1][num].code = code1 | code2 << 8;
                                    keyconvmap.map[1][num].row = row;
                                    keyconvmap.map[1][num].column = col;
                                    keyconvmap.map[1][num].vshift = shift;
                                    num++;
                                    break;
                                case 'U':
                                    keyconvmap.map[0][num].code = code1 | code2 << 8;
                                    keyconvmap.map[0][num].row = row;
                                    keyconvmap.map[0][num].column = col;
                                    keyconvmap.map[0][num].vshift = shift;
                                    num++;
                                    break;
                            }
                        }
                    }
                case '!': // keyword handling
                    {
                        char *p;
                        int row, col;

                        p = strtok(buffer + 1, " \t:");
                        if (!p) {
                            break;
                        }

                        if (!strcmp(p, "LSHIFT")) {
                            p = strtok(NULL, " \t,");
                            if (!p) {
                                break;
                            }
                            row = atoi(p);
                            p = strtok(NULL, " \t,");
                            if (!p) {
                                break;
                            }
                            col = atoi(p);

                            keyconvmap.lshift_row = row;
                            keyconvmap.lshift_col = col;
                        }
                        if (!strcmp(p, "RSHIFT")) {
                            p = strtok(NULL, " \t,");
                            if (!p) {
                                break;
                            }
                            row = atoi(p);
                            p = strtok(NULL, " \t,");
                            if (!p) {
                                break;
                            }
                            col = atoi(p);

                            keyconvmap.rshift_row = row;
                            keyconvmap.rshift_col = col;
                        }
                        if (!strcmp(p, "KSCODE")) {
                            keyconvmap.symbolic = 0;  // FALSE
                        }
                        if (!strcmp(p, "KSYM")) {
                            keyconvmap.symbolic = 1;  // TRUE
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        if (num == 0x100) {
            log_message(LOG_DEFAULT, "parse.c: Warning: keyboard file contains more than 255 entries.");
            break;
        }
    }
    keyconvmap.entries = num;

    fclose(fp);

    return 0;
}
