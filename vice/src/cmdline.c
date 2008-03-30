/*
 * cmdline.c - Command-line parsing.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "uicmdline.h"
#include "util.h"


#ifdef HAS_TRANSLATION

/* the current changes in this file for HAS_TRANSLATION
   are temporary to make both the new and old command-line
   stuff to work properly during the transition to string tables. */

static unsigned int num_options_trans, num_allocated_options_trans;
static cmdline_option_ram_trans_t *options_trans;

#endif

static unsigned int num_options, num_allocated_options;
static cmdline_option_ram_t *options;

#ifdef HAS_TRANSLATION
int cmdline_init_trans(void)
{
    if (options_trans != NULL)
        lib_free(options_trans);

    num_allocated_options_trans = 100;
    num_options_trans = 0;
    options_trans = (cmdline_option_ram_trans_t *)lib_malloc(sizeof(cmdline_option_ram_trans_t)
              * num_allocated_options_trans);

    return 0;
}
#endif

int cmdline_init(void)
{
#ifdef HAS_TRANSLATION
    cmdline_init_trans();
#endif
    if (options != NULL)
        lib_free(options);

    num_allocated_options = 100;
    num_options = 0;
    options = (cmdline_option_ram_t *)lib_malloc(sizeof(cmdline_option_ram_t)
              * num_allocated_options);

    return 0;
}

#ifdef HAS_TRANSLATION
int cmdline_register_options_trans(const cmdline_option_trans_t *c)
{
    cmdline_option_ram_trans_t *p;

    p = options_trans + num_options_trans;
    for (; c->name != NULL; c++, p++) {
        if (num_allocated_options_trans <= num_options_trans) {
            num_allocated_options_trans *= 2;
            options_trans = (cmdline_option_ram_trans_t *)lib_realloc(options_trans,
                      (sizeof(cmdline_option_ram_trans_t) * num_allocated_options_trans));
            p = options_trans + num_options_trans;
        }

        p->name = lib_stralloc(c->name);
        p->type = c->type;
        p->need_arg = c->need_arg;
        p->set_func = c->set_func;
        p->extra_param = c->extra_param;
        if (c->resource_name != NULL)
            p->resource_name = lib_stralloc(c->resource_name);
        else
            p->resource_name = NULL;
        p->resource_value = c->resource_value;
        p->param_name = c->param_name;
        p->description = c->description;

        num_options_trans++;
    }

    return 0;
}
#endif

int cmdline_register_options(const cmdline_option_t *c)
{
    cmdline_option_ram_t *p;

    p = options + num_options;
    for (; c->name != NULL; c++, p++) {
        if (num_allocated_options <= num_options) {
            num_allocated_options *= 2;
            options = (cmdline_option_ram_t *)lib_realloc(options,
                      (sizeof(cmdline_option_ram_t) * num_allocated_options));
            p = options + num_options;
        }

        p->name = lib_stralloc(c->name);
        p->type = c->type;
        p->need_arg = c->need_arg;
        p->set_func = c->set_func;
        p->extra_param = c->extra_param;
        if (c->resource_name != NULL)
            p->resource_name = lib_stralloc(c->resource_name);
        else
            p->resource_name = NULL;
        p->resource_value = c->resource_value;
        p->param_name = c->param_name;
        p->description = c->description;

        num_options++;
    }

    return 0;
}

#ifdef HAS_TRANSLATION
static void cmdline_free_trans(void)
{
    unsigned int i;

    for (i = 0; i < num_options_trans; i++) {
        lib_free((options_trans + i)->name);
        lib_free((options_trans + i)->resource_name);
    }
}
#endif

static void cmdline_free(void)
{
    unsigned int i;

    for (i = 0; i < num_options; i++) {
        lib_free((options + i)->name);
        lib_free((options + i)->resource_name);
    }
}

void cmdline_shutdown(void)
{
#ifdef HAS_TRANSLATION
    cmdline_free_trans();
#endif
    cmdline_free();

    lib_free(options);
#ifdef HAS_TRANSLATION
    lib_free(options_trans);
#endif
}

#ifdef HAS_TRANSLATION
static cmdline_option_ram_trans_t *lookup_trans(const char *name, int *is_ambiguous)
{
    cmdline_option_ram_trans_t *match;
    size_t name_len;
    unsigned int i;

    name_len = strlen(name);

    match = NULL;
    for (i = 0; i < num_options_trans; i++) {
        if (strncmp(options_trans[i].name, name, name_len) == 0) {
            if (options_trans[i].name[name_len] == '\0') {
                *is_ambiguous = 0;
                return &options_trans[i];
            } else if (match != NULL) {
                *is_ambiguous = 1;
                return match;
            }
            match = &options_trans[i];
        }
    }

    *is_ambiguous = 0;
    return match;
}
#endif

static cmdline_option_ram_t *lookup(const char *name, int *is_ambiguous)
{
    cmdline_option_ram_t *match;
    size_t name_len;
    unsigned int i;

    name_len = strlen(name);

    match = NULL;
    for (i = 0; i < num_options; i++) {
        if (strncmp(options[i].name, name, name_len) == 0) {
            if (options[i].name[name_len] == '\0') {
                *is_ambiguous = 0;
                return &options[i];
            } else if (match != NULL) {
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
#ifdef HAS_TRANSLATION
            int is_trans;
            cmdline_option_ram_trans_t *p_trans;
#endif
            cmdline_option_ram_t *p;

            if (argv[i][1] == '\0') {
                archdep_startup_log_error("Invalid option '%s'.\n", argv[i]);
                return -1;
            }

            /* `--' delimits the end of the option list.  */
            if (argv[i][1] == '-') {
                i++;
                break;
            }

            p = lookup(argv[i], &is_ambiguous);
#ifdef HAS_TRANSLATION
            if (p == NULL)
              p_trans=lookup_trans(argv[i], &is_ambiguous);
            else
              p_trans=NULL;

            if (p == NULL && p_trans == NULL) {
#else
            if (p == NULL) {
#endif
                archdep_startup_log_error("Unknown option '%s'.\n", argv[i]);
                return -1;
            }

            if (is_ambiguous) {
                archdep_startup_log_error("Option '%s' is ambiguous.\n",
                                          argv[i]);
                return -1;
            }

#ifdef HAS_TRANSLATION
            if (p != NULL)
            {
              if (p->need_arg && i >= *argc - 1)
              {
                archdep_startup_log_error("Option '%s' requires a parameter.\n",
                                          p->name);
                return -1;
              }
            }
            else
            {
              if (p_trans->need_arg && i >= *argc - 1)
              {
                archdep_startup_log_error("Option '%s' requires a parameter.\n",
                                          p_trans->name);
                return -1;
              }
            }
#else
            if (p->need_arg && i >= *argc - 1) {
                archdep_startup_log_error("Option '%s' requires a parameter.\n",
                                          p->name);
                return -1;
            }
#endif

#ifdef HAS_TRANSLATION
            if (p!=NULL)
            {
              switch(p->type) {
                case SET_RESOURCE:
                  if (p->need_arg)
                      retval = resources_set_value_string(p->resource_name,
                                                          argv[i + 1]);
                  else
                      retval = resources_set_value(p->resource_name,
                                                   p->resource_value);
                  break;
                case CALL_FUNCTION:
                  retval = p->set_func(p->need_arg ? argv[i+1] : NULL,
                           p->extra_param);
                  break;
                default:
                  archdep_startup_log_error("Invalid type for option '%s'.\n",
                                            p->name);
                  return -1;
              }
              if (retval < 0) {
                  if (p->need_arg)
                      archdep_startup_log_error("Argument '%s' not valid for option `%s'.\n",
                                                argv[i + 1], p->name);
                  else
                      archdep_startup_log_error("Option '%s' not valid.\n", p->name);
                  return -1;
              }
              i += p->need_arg ? 2 : 1;
            }
            else
            {
              switch(p_trans->type) {
                case SET_RESOURCE:
                  if (p_trans->need_arg)
                      retval = resources_set_value_string(p_trans->resource_name,
                                                          argv[i + 1]);
                  else
                      retval = resources_set_value(p_trans->resource_name,
                                                   p_trans->resource_value);
                  break;
                case CALL_FUNCTION:
                  retval = p_trans->set_func(p_trans->need_arg ? argv[i+1] : NULL,
                           p_trans->extra_param);
                  break;
                default:
                  archdep_startup_log_error("Invalid type for option '%s'.\n",
                                            p_trans->name);
                  return -1;
              }
              if (retval < 0) {
                  if (p_trans->need_arg)
                      archdep_startup_log_error("Argument '%s' not valid for option `%s'.\n",
                                                argv[i + 1], p_trans->name);
                  else
                      archdep_startup_log_error("Option '%s' not valid.\n", p_trans->name);
                  return -1;
              }
              i += p_trans->need_arg ? 2 : 1;
            }
#else
            switch(p->type) {
              case SET_RESOURCE:
                if (p->need_arg)
                    retval = resources_set_value_string(p->resource_name,
                                                        argv[i + 1]);
                else
                    retval = resources_set_value(p->resource_name,
                                                 p->resource_value);
                break;
              case CALL_FUNCTION:
                retval = p->set_func(p->need_arg ? argv[i+1] : NULL,
                         p->extra_param);
                break;
              default:
                archdep_startup_log_error("Invalid type for option '%s'.\n",
                                          p->name);
                return -1;
            }
            if (retval < 0) {
                if (p->need_arg)
                    archdep_startup_log_error("Argument '%s' not valid for option `%s'.\n",
                                              argv[i + 1], p->name);
                else
                    archdep_startup_log_error("Option '%s' not valid.\n", p->name);
                return -1;
            }

            i += p->need_arg ? 2 : 1;
#endif

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

void cmdline_show_help(void *userparam)
{
#ifdef HAS_TRANSLATION
    ui_cmdline_show_help(num_options, options, num_options_trans, options_trans, userparam);
#else
    ui_cmdline_show_help(num_options, options, userparam);
#endif
}

char *cmdline_options_string(void)
{
    unsigned int i;
    char *cmdline_string, *new_cmdline_string;
    char *add_to_options1, *add_to_options2, *add_to_options3;

    cmdline_string = lib_stralloc("\n");

    for (i = 0; i < num_options; i++) {
        add_to_options1 = lib_msprintf("%s", options[i].name);
        add_to_options3 = lib_msprintf("\n\t%s\n", options[i].description);
        if (options[i].need_arg && options[i].param_name != NULL) {
            add_to_options2 = lib_msprintf(" %s", options[i].param_name);
            new_cmdline_string = util_concat(cmdline_string, add_to_options1,
                                             add_to_options2, add_to_options3,
                                             NULL);
            lib_free(add_to_options2);
        } else {
            new_cmdline_string = util_concat(cmdline_string, add_to_options1,
                                             add_to_options3, NULL);
        }
        lib_free(add_to_options1);
        lib_free(add_to_options3);

        lib_free(cmdline_string);

        cmdline_string = new_cmdline_string;
    }

#ifdef HAS_TRANSLATION
    for (i = 0; i < num_options_trans; i++) {
        add_to_options1 = lib_msprintf("%s", options_trans[i].name);
        add_to_options3 = lib_msprintf("\n\t%s\n", translate_text(options_trans[i].description));
        if (options_trans[i].need_arg && translate_text(options_trans[i].param_name) != NULL) {
            add_to_options2 = lib_msprintf(" %s", translate_text(options_trans[i].param_name));
            new_cmdline_string = util_concat(cmdline_string, add_to_options1,
                                             add_to_options2, add_to_options3,
                                             NULL);
            lib_free(add_to_options2);
        } else {
            new_cmdline_string = util_concat(cmdline_string, add_to_options1,
                                             add_to_options3, NULL);
        }
        lib_free(add_to_options1);
        lib_free(add_to_options3);

        lib_free(cmdline_string);

        cmdline_string = new_cmdline_string;
    }
#endif

    return cmdline_string;
}
