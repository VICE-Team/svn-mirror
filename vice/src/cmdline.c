/*
 * cmdline.c - Command-line parsing.
 *
 * Written by
 *  Ettore Perazzoli	(ettore@comm2000.it)
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

#include "utils.h"
#include "cmdline.h"

static int num_options, num_allocated_options;
static cmdline_option_t *options;

int cmdline_init(void)
{
    if (options != NULL)
        free(options);

    num_allocated_options = 100;
    num_options = 0;
    options = xmalloc(sizeof(cmdline_option_t) * num_allocated_options);

    return 0;
}

int cmdline_register_options(const cmdline_option_t *c)
{
    cmdline_option_t *p;

    p = options + num_options;
    for (; c->name != NULL; c++, p++) {
        if (num_allocated_options <= num_options) {
            num_allocated_options *= 2;
            options = xrealloc(options, (sizeof(cmdline_option_t)
                                         * num_allocated_options));
            p = options + num_options;
        }

        memcpy(p, c, sizeof(cmdline_option_t));
        num_options++;
    }
}

static cmdline_option_t *lookup(const char *name, int *is_ambiguous)
{
    cmdline_option_t *match;
    int is_partial_match;
    int name_len = strlen(name);
    int i;

    match = NULL;
    for (i = 0; i < num_options; i++) {
        if (strncmp(options[i].name, name, name_len) == 0) {
            if (match != NULL) {
                *is_ambiguous = 1;
                return match;
            }
            match = &options[i];
        }
    }

    *is_ambiguous = 0;
    return match;
}

int cmdline_parse(int *argc, char **argv)
{
    int i = 1;

    while (i < *argc) {
        if (*argv[i] == '-' || *argv[i] == '+') {
            int is_ambiguous, retval;
            cmdline_option_t *p = lookup(argv[i], &is_ambiguous);

            if (p == NULL) {
                if (is_ambiguous)
                    fprintf(stderr, "Option `%s' is ambiguous.\n", argv[i]);
                else
                    fprintf(stderr, "Unknown option `%s'.\n", argv[i]);
                return -1;
            }

            if (p->need_arg && i >= *argc) {
                fprintf(stderr, "Option `%s' requires a parameter.\n",
                        p->name);
                return -1;
            }

            switch(p->type) {
              case SET_RESOURCE:
                if (p->need_arg)
                    retval = resources_set_value(p->resource_name,
                                                 argv[i + 1]);
                else
                    retval = resources_set_value(p->resource_name,
                                                 p->resource_value);
                break;
              case CALL_FUNCTION:
                if (p->need_arg)
                    retval = p->set_func(argv[i + 1], p->extra_param);
                else
                    retval = p->set_func(NULL, NULL);
                break;
              default:
                fprintf(stderr, "Invalid type for option `%d'.\n",
                        p->name);
                return -1;
            }

            if (retval < 0) {
                if (p->need_arg)
                    fprintf(stderr,
                            "Argument `%s' not valid for option `%s'.\n",
                            argv[1], p->name);
                else
                    fprintf(stderr,
                            "Option `%s' not valid.\n", p->name);
                return -1;
            }

            i += p->need_arg ? 2 : 1;
        } else
            break;
    }

    /* Remove all the parsed options.  */
    {
        int j;

        for (j = 1; j <= (*argc - i); j++)
            argv[j] = argv[i + j - 1];

        *argc -= i;
    }

    return 0;
}
