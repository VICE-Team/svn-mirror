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

#include "log.h"
#include "utils.h"   // get_line
#include "sysfile.h" // sysfile_open

#include "parse.h"

convmap keyconvmap;

/* ------------------------------------------------------------------------ */

int load_keymap_file(const char *fname)
{
    FILE *fp;
    char buffer[1025];
    char *complete_path;

    if (!fname)
        return -1;

    fp = sysfile_open(fname, &complete_path, "r");

    if (!fp)
    {
        char *tmp = concat(fname, ".vkm", NULL);
        free(complete_path);
        fp = sysfile_open(tmp, &complete_path, "r");
        free(tmp);
    }

    if (fp)
        log_message(LOG_DEFAULT, "Loading keymap `%s'.", complete_path);

    free(complete_path);

    if (!fp)
        return -1;

    while (!feof(fp))
    {
        if (get_line(buffer, 1024, fp))
            switch (buffer[0])
            {
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
                    int num, row, col, shift;

                    p = strtok(buffer+1, " \t:");
                    if (!p)
                        break;
                    num = atoi(p);
                    if (num<0 || num>254)
                        break;
                    p = strtok(NULL, " \t,");
                    if (!p)
                        break;
                    row = atoi(p);
                    p = strtok(NULL, " \t,");
                    if (!p)
                        break;
                    col = atoi(p);

                    p = strtok(NULL, " \t");
                    if (!(p || row < 0))
                        break;
                    if (p)
                        shift = atoi(p);

                    {
                        int s;
                        switch (buffer[0])
                        {
                        case '#':
                            keyconvmap.map[0][num].row    = row;
                            keyconvmap.map[0][num].column = col;
                            keyconvmap.map[0][num].vshift = shift;
                            keyconvmap.map[1][num].row    = row;
                            keyconvmap.map[1][num].column = col;
                            keyconvmap.map[1][num].vshift = shift;
                            break;
                        case 'S':
                            keyconvmap.map[1][num].row    = row;
                            keyconvmap.map[1][num].column = col;
                            keyconvmap.map[1][num].vshift = shift;
                            break;
                        case 'U':
                            keyconvmap.map[0][num].row    = row;
                            keyconvmap.map[0][num].column = col;
                            keyconvmap.map[0][num].vshift = shift;
                            break;
                        }
//                        log_debug("setting %i (%i) to c%i r%i s%i", num, s, row, col, shift);
                    }
                }
            case '!': // keyword handling
                {
                    char *p;
                    int num, row, col, shift;

                    p = strtok(buffer+1, " \t:");
                    if (!p)
                        break;

                    if (!strcmp(p, "LSHIFT"))
                    {
                        p = strtok(NULL, " \t,");
                        if (!p)
                            break;
                        row = atoi(p);
                        p = strtok(NULL, " \t,");
                        if (!p)
                            break;
                        col = atoi(p);

                        keyconvmap.lshift_row = row;
                        keyconvmap.lshift_col = col;
                    }
                    if (!strcmp(p, "RSHIFT"))
                    {
                        p = strtok(NULL, " \t,");
                        if (!p)
                            break;
                        row = atoi(p);
                        p = strtok(NULL, " \t,");
                        if (!p)
                            break;
                        col = atoi(p);

                        keyconvmap.rshift_row = row;
                        keyconvmap.rshift_col = col;
                    }
                    if (!strcmp(p, "CLEAR"))
                    {
                        int i;
                        for (i=0; i<255; i++)
                        {
                            keyconvmap.map[0][i].row    = -1;
                            keyconvmap.map[1][i].column = -1;
                        }
                    }
                    if (!strcmp(p, "KSCODE"))
                        keyconvmap.symbolic = 0;  // FALSE
                    if (!strcmp(p, "KSYM"))
                        keyconvmap.symbolic = 1;  // TRUE
                }

                break;
            default:
                break;
            }
    }

    fclose(fp);

    return 0;
}

