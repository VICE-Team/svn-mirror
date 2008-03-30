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

#ifndef _CMDLINE_H
#define _CMDLINE_H

#include "resources.h"

/* This describes a command-line option.  */
/* Warning: all the pointers should point to areas that are valid throughout
   the execution.  No reallocation is performed.  */
typedef struct cmdline_option {

    /* Name of command-line option.  */
    const char *name;

    /* Behavior of this command-line option.  */
    enum {
        SET_RESOURCE,
        CALL_FUNCTION
    } type;

    /* Flag: Does this option need an argument?  */
    int need_arg;

    /* Function to call if type is `CALL_FUNCTION'.  */
    int (*set_func)(const char *value, void *extra_param);

    /* Extra parameter to pass to `set_func' if type is `CALL_FUNCTION'.  */
    void *extra_param;

    /* Resource to change if `type' is `SET_RESOURCE'.  */
    const char *resource_name;

    /* Value to assign to `resource_name' if `type' is `SET_RESOURCE' and
       `need_arg' is zero.  */
    resource_value_t resource_value;

} cmdline_option_t;

int cmdline_init(void);
int cmdline_register_options(const cmdline_option_t *c);
int cmdline_parse(int *argc, char **argv);

#endif
