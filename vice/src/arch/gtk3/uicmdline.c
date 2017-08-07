/*
 * uicmdline.c - Native GTK3 UI cmdline stuff.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
 *  Bas Wassink <b.wassink@ziggo.nl>
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

#include "not_implemented.h"

#include "cmdline.h"
#include "uicmdline.h"


/** \brief  Dump command line options on stdout
 *
 * \param[in]   num_options number of options
 * \param[in]   options     list of options
 * \param[in]   userparm    ignored for some reason
 *
 * XXX: this function and its brethren all over archdep should probably be moved
 *      to cmdline.c and renamed.   -- compyx, 2017-08-7
 */
void ui_cmdline_show_help(unsigned int num_options,
                          cmdline_option_ram_t *options,
                          void *userparam)
{
    unsigned int i;

    printf("\nAvailable command-line options:\n\n");
    for (i = 0; i < num_options; i++) {
        char *param = cmdline_options_get_param(i);
        puts(options[i].name);
        if (options[i].need_arg && param != NULL) {
            printf(" %s", param);
        }
        printf("\n\t%s\n", cmdline_options_get_description(i));
    }
    putchar('\n');
}
