/*
 * log.c - Logging facility.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "translate.h"
#include "util.h"


static FILE *log_file = NULL;

static char **logs = NULL;
static log_t num_logs = 0;

/* ------------------------------------------------------------------------- */

static char *log_file_name = NULL;

static void log_file_open(void)
{
    if (log_file_name == NULL || *log_file_name == 0) {
        log_file = archdep_open_default_log_file();
        return;
    } else {
#ifndef __OS2__
        if (strcmp(log_file_name, "-") == 0)
            log_file = stdout;
        else
#endif
            log_file = fopen(log_file_name, MODE_WRITE_TEXT);
    }
    /* flush all data direct to the output stream. */
    if (log_file)
        setbuf(log_file, NULL);
}

static int set_log_file_name(const char *val, void *param)
{
    if (util_string_set(&log_file_name, val) < 0) {
        return 0;
    }

    if (log_file) {
        fclose(log_file);
        log_file_open();
    }

    return 0;
}

#ifndef __X1541__
static const resource_string_t resources_string[] = {
    { "LogFileName", "", RES_EVENT_NO, NULL,
      &log_file_name, set_log_file_name, NULL },
    { NULL }
};

int log_resources_init(void)
{
    return resources_register_string(resources_string);
}

void log_resources_shutdown(void)
{
    lib_free(log_file_name);
}

static const cmdline_option_t cmdline_options[] = {
    { "-logfile", SET_RESOURCE, 1,
      NULL, NULL, "LogFileName", NULL,
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      IDCLS_P_NAME, IDCLS_SPECIFY_LOG_FILE_NAME,
      NULL, NULL },
    { NULL }
};

int log_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}
#endif

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

    log_file_open();

    return log_file == NULL ? -1 : 0;
}

log_t log_open(const char *id)
{
    log_t new_log = 0;
    log_t i;

    for (i = 0; i < num_logs; i++) {
        if (logs[i] == NULL) {
            new_log = i;
            break;
        }
    }
    if (i == num_logs) {
        new_log = num_logs++;
        logs = (char **)lib_realloc(logs, sizeof(*logs) * num_logs);
    }

    logs[new_log] = lib_stralloc(id);

    return new_log;
}

int log_close(log_t log)
{
    if (logs[(unsigned int)log] == NULL)
        return -1;

    lib_free(logs[(unsigned int)log]);
    logs[(unsigned int)log] = NULL;

    return 0;
}

void log_close_all(void)
{
    log_t i;

    for (i = 0; i < num_logs; i++)
        log_close(i);

    lib_free(logs);
}

static int log_archdep(const char *logtxt, const char *fmt, va_list ap)
{
    /*
     * ------ Split into single lines ------
     */
    int rc = 0;

    char *txt = lib_mvsprintf(fmt, ap);

    char *beg = txt;
    char *end = txt + strlen(txt) + 1;

    while (beg < end) {
        char *eol = strchr(beg, '\n');

        if (eol)
            *eol='\0';

        if (archdep_default_logger(*beg ? logtxt : "", beg) < 0) {
            rc = -1;
            break;
        }

        if (!eol)
            break;

        beg = eol + 1;
    }

    lib_free(txt);

    return rc;
}

static int log_helper(log_t log, unsigned int level, const char *format,
                      va_list ap)
{
    static const char *level_strings[3] = {
        "",
        "Warning - ",
        "Error - "
    };

    const signed int logi = (signed int)log;
    int rc = 0;
    char *logtxt = NULL;

    if (logi == LOG_ERR
        || (logi != LOG_DEFAULT && (logs == NULL || logs[logi] == NULL)))
        return -1;

    if (logi != LOG_DEFAULT && *logs[logi] != '\0')
        logtxt = lib_msprintf("%s: %s", logs[logi], level_strings[level]);
    else
        logtxt = lib_stralloc("");

    if (log_file == NULL) {
        rc = log_archdep(logtxt, format, ap);
    } else {
#ifdef WIN32
        log_archdep(logtxt, format, ap);
#endif /* #ifdef WIN32 */
        if (fputs(logtxt, log_file) == EOF
            || vfprintf(log_file, format, ap) < 0
            || fputc ('\n', log_file) == EOF)
            rc = -1;
    }

    lib_free(logtxt);

    return rc;
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

