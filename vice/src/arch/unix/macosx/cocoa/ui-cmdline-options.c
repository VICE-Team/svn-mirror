/*
 * ui-cmdline-options.c - MacVICE command line options
 *
 * Written by
 *  Christian Vogelgsang <chris@vogelgsang.org>
 *  Michael Klein <michael.klein@puffin.lb.shuttle.de>
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

#include <string.h>
#include <stdio.h>

#include "vice.h"
#include "cmdline.h"

static const cmdline_option_t cmdline_options[] = {
    { "-saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (void *)1,
      NULL, N_("Save settings (resources) on exit") },
    { "+saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (void *)0,
      NULL, N_("Never save settings (resources) on exit") },
    { "+confirmexit", SET_RESOURCE, 0, NULL, NULL,
      "ConfirmOnExit", (void *)0,
      NULL, N_("Never confirm quiting VICE") },
    { "-confirmexit", SET_RESOURCE, 0, NULL, NULL,
      "ConfirmOnExit", (void *)1,
      NULL, N_("Confirm quiting VICE") },
    { NULL }
};

int ui_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

void ui_cmdline_show_help(unsigned int num_options,
                          cmdline_option_ram_t *options, void *userparam)
{
    unsigned int i;

    printf("\nAvailable command-line options:\n\n");
    for (i = 0; i < num_options; i++) {
        fputs(options[i].name, stdout);
        if (options[i].need_arg && options[i].param_name != NULL)
            printf(" %s", options[i].param_name);
        printf("\n\t%s\n", options[i].description);
    }
    putchar('\n');
}
