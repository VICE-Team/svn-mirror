/*
 * uicmdline.c - The command line user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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

#include <stdio.h>
#include <stdlib.h>

#include "cmdline.h"

/* This does not work as stdout is directed to null.  */
void ui_cmdline_show_help(unsigned int num_options, cmdline_option_ram_t *options, void *userparam)
{
    unsigned int i;

    printf("\nAvailable command-line options:\n\n");
    for (i = 0; i < num_options; i++) {
        fputs(options[i].name, stdout);
        if (options[i].need_arg && cmdline_options_get_param(i) != NULL) {
            printf(" %s", cmdline_options_get_param(i));
        }
        printf("\n\t%s\n", cmdline_options_get_description(i));
    }
    putchar('\n');
}
