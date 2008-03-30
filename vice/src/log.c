/*
 * log.c - Logging facility.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <stdarg.h>

#include "log.h"

#include "cmdline.h"
#include "resources.h"
#include "archdep.h"
#include "utils.h"

#ifdef __riscos
#include "ROlib.h"
#endif

static FILE *log_file = NULL;

static char **logs = NULL;
static unsigned int num_logs = 0;
static unsigned int num_allocated_logs = 0;

/* ------------------------------------------------------------------------- */

static char *log_file_name;

static int set_log_file_name(resource_value_t v)
{
    string_set(&log_file_name, (const char *) v);

    return 0;
}

static resource_t resources[] = {
    { "LogFileName", RES_STRING, (resource_value_t) NULL,
      (resource_value_t *) &log_file_name, set_log_file_name },
    { NULL }
};

int log_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-logfile", SET_RESOURCE, 1, NULL, NULL,
      "LogFileName", NULL, "<name>", "Specify log file name" },
    { NULL }
};

int log_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

int log_init_with_fd(FILE *f)
{
    if (f == NULL)
        return -1;

    log_file = f;
    return 0;
}

int log_init(void)
{
    if (logs != NULL)
        return -1;

    if (log_file_name == NULL || *log_file_name == 0) {
        log_file = archdep_open_default_log_file();
    } else {
        if (strcmp(log_file_name, "-") == 0)
            log_file = stdout;
        log_file = fopen(log_file_name, "wt");
    }

    return log_file == NULL ? -1 : 0;
}

int log_open(const char *id)
{
    log_t new_log = 0;
    int i;

    for (i = 0; i < num_logs; i++) {
        if (logs[i] == NULL) {
            new_log = i;
            break;
        }
    }
    if (i == num_logs) {
        new_log = num_logs++;
        if (num_logs > num_allocated_logs) {
            if (num_allocated_logs == 0) {
                num_allocated_logs = 128;
                logs = xmalloc(sizeof(*logs) * num_allocated_logs);
            } else {
                num_allocated_logs *= 2;
                logs = xrealloc(logs, sizeof(*logs) * num_allocated_logs);
            }
        }
    }

    logs[new_log] = stralloc(id);
    return new_log;
}

int log_close(log_t log)
{
    if (logs[(unsigned int) log] == NULL)
        return -1;
    free(logs[(unsigned int) log]);
    logs[(unsigned int) log] = NULL;
    return 0;
}

static int log_helper(log_t log, unsigned int level,
                      const char *format, va_list ap)
{
    static const char *level_strings[3] = {
        "",
        "Warning: ",
        "Error: "
    };

    if (log == LOG_ERR
        || (log != LOG_DEFAULT && logs[(unsigned int) log] == NULL)
        || log_file == NULL) 
        return -1;

    if (log != LOG_DEFAULT && *logs[(unsigned int) log] != 0) {
        if (fputs (logs[(unsigned int) log], log_file) == EOF
            || fputs (": ", log_file) == EOF)
            return -1;
    }

    if (fputs(level_strings[level], log_file) == EOF
        || vfprintf(log_file, format, ap) < 0
        || fputc ('\n', log_file) == EOF)
        return -1;

    return 0;
}

int log_message(log_t log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    return log_helper(log, 0, format, ap);
}

int log_warning(log_t log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    return log_helper(log, 1, format, ap);
}

int log_error(log_t log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    return log_helper(log, 2, format, ap);
}

int log_debug(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    return log_helper(LOG_DEFAULT, 0, format, ap);
}
