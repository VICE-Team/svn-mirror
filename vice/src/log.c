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

/* #define DBGLOGGING */

#include "vice.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "archdep.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "monitor.h"
#include "resources.h"
#include "util.h"

#ifdef DBGLOGGING
#define DBG(x) printf x
#else
#define DBG(x)
#endif

#ifdef USE_VICE_THREAD
/*
 * It was observed that stdout logging from the UI thread under Windows
 * wasn't reliable, possibly only when the vice mainlock has not been
 * obtained.
 *
 * This lock serialises access to logging functions without requiring
 * ownership of the main lock.
 *
 *******************************************************************
 * ANY NEW NON-STATIC FUNCTIONS NEED CALLS TO LOCK() and UNLOCK(). *
 *******************************************************************
 */

#include <pthread.h>
static pthread_mutex_t log_lock;

#define LOCK() pthread_mutex_lock(&log_lock)
#define UNLOCK() pthread_mutex_unlock(&log_lock)
#define UNLOCK_AND_RETURN_INT(i) { int result = (i); UNLOCK(); return result; }

#else /* #ifdef USE_VICE_THREAD */

#define LOCK()
#define UNLOCK()
#define UNLOCK_AND_RETURN_INT(i) return (i)

#endif /* #ifdef USE_VICE_THREAD */

static FILE *log_file = NULL;

static char **logs = NULL;
static log_t num_logs = 0;

static int log_limit_early = -1; /* -1 means not set */

static int locked = 0;

/* resources */

static char *log_file_name = NULL; /* name of the log file */

static int log_limit = LOG_LIMIT_DEBUG; /* before the default is set, we want all messages */

static int log_to_file = 1;
static int log_to_stdout = 1;
static int log_to_monitor = 0;

/* ------------------------------------------------------------------------- */

static int log_file_type = 0;

static int log_file_open(void)
{
    int rc = 0;
    DBG(("log_file_open\n"));
    if (log_file != NULL) {
        DBG(("log_file_open is open, return -1\n"));
        return -1;
    }
    if ((log_file_name == NULL) || (*log_file_name == 0)) {
        /* no log file name was given, in this case we use the arch dependent
           default name */
        /* CAUTION: this may return stdout */
        log_file_type = 2;
        DBG(("log_file_open default\n"));
        log_file = archdep_open_default_log_file();
        if (log_file == NULL) {
            /* if the archdep layer returned NULL for whatever reason (usually
               because it could not open the log file), enable logging to stdout,
               disable logging to file, and set log_file to NULL */
            DBG(("archdep_open_default_log_file returned NULL, using stdout\n"));
            log_to_stdout = 1;
            log_to_file = 0;
            log_file = NULL;
            log_file_type = 1;
        }
    } else if (strcmp(log_file_name, "-") == 0) {
        DBG(("log_file_open stdout\n"));
        /* HACK: if log_file_name is "-", enable logging to stdout,
                 disable logging to file, and set log_file to NULL */
        log_to_stdout = 1;
        log_to_file = 0;
        log_file = NULL;
        log_file_name[0] = 0;
        log_file_type = 1;
    } else if (log_to_file) {
        DBG(("log_file_open file '%s'\n", log_file_name));
        log_file_type = 3;
        log_file = fopen(log_file_name, MODE_WRITE_TEXT);
        if (log_file == NULL) {
            log_file_type = 0;
            rc = -1;
        }
    }

    /* at this point log_file is either NULL, or a valid fd for the log file,
       however it will NOT be stdout! */
#if 1
    /* FIXME: we probably should not do this for performance reasons? */
    /* flush all data direct to the output stream. */
    if (log_file) {
        setbuf(log_file, NULL);
    }
#endif
    DBG(("log_file_open return:%d\n", rc));
    return rc;
}

static void log_file_close(void)
{
    DBG(("log_file_close %p\n", log_file));
    if (log_file) {
        if (log_file != stdout) {
            fclose(log_file);
        }
        fflush(log_file);
        log_file = NULL;
    }
}

/******************************************************************************/

int log_set_limit(int n)
{
    LOCK();

    if ((n < 0) || (n > 0xff)) {
        UNLOCK_AND_RETURN_INT(-1);
    }

    log_limit = n;
    UNLOCK_AND_RETURN_INT(0);
}

/* called by code that is executed *before* the resources are registered */
int log_set_limit_early(int n)
{
    LOCK();

    log_limit = n;
    log_limit_early = n;

    UNLOCK_AND_RETURN_INT(0);
}

/* called via main_program()->archdep_init() */
int log_early_init(int argc, char **argv)
{
    int i;

#ifdef USE_VICE_THREAD
    pthread_mutexattr_t lock_attributes;
    pthread_mutexattr_init(&lock_attributes);
    pthread_mutexattr_settype(&lock_attributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&log_lock, &lock_attributes);
#endif

    DBG(("log_early_init: %d %s\n", argc, argv[0]));
    if (argc > 1) {
        for (i = 1; i < argc; i++) {
            DBG(("log_early_init: %d %s\n", i, argv[i]));
            if ((strcmp("-verbose", argv[i]) == 0) || (strcmp("--verbose", argv[i]) == 0)) {
                log_set_limit_early(LOG_LIMIT_VERBOSE);
                break;
            } else if ((strcmp("-silent", argv[i]) == 0) || (strcmp("--silent", argv[i]) == 0)) {
                log_set_limit_early(LOG_LIMIT_SILENT);
                break;
            } else if ((strcmp("-debug", argv[i]) == 0) || (strcmp("--debug", argv[i]) == 0)) {
                log_set_limit_early(LOG_LIMIT_DEBUG);
                break;
            }
        }
    }
    return 0;
}

/******************************************************************************/

#ifndef __X1541__

static int set_log_limit(int val, void *param)
{
    LOCK();
    /* HACK: When a log limit was given "early" (meaning: before the resources
             are being initialized) on the commandline, the wanted log limit will
             be saved into log_limit_early (which is NOT tied to a resource).
             We initialize log_limit_early with -1 so we can check if the log
             limit was given on the command line or not.
    */
    if (log_limit_early != -1) {
        /* log limit was given on command line */
        val = log_limit_early;  /* set the resource to the value given on command line */
        log_limit_early = -1;   /* reset to initial ("ignored") value */
    }
    UNLOCK_AND_RETURN_INT(log_set_limit(val));
}

static int set_log_to_file(int val, void *param)
{
    LOCK();
    log_to_file = val ? 1 : 0;
#if 0
    log_file_close();
    if (log_to_file) {
        log_file_open();
    }
#endif
    DBG(("set_log_to_file:%d\n", val));
    UNLOCK();
    return 0;
}

static int set_log_to_stdout(int val, void *param)
{
    LOCK();
    log_to_stdout = val ? 1 : 0;
    DBG(("set_log_to_stdout:%d\n", val));
    UNLOCK();
    return 0;
}

static int set_log_to_monitor(int val, void *param)
{
    LOCK();
    log_to_monitor = val ? 1 : 0;
    DBG(("set_log_to_monitor:%d\n", val));
    UNLOCK();
    return 0;
}

static int set_log_file_name(const char *val, void *param)
{
    if (locked) {
        DBG(("set_log_file_name (locked):%s\n", val));
        return 0;
    }
    DBG(("set_log_file_name:'%s'->'%s'\n", log_file_name, val));
    if (((log_file_name == NULL) || (*log_file_name == 0)) &&
        ((val == NULL) || (*val == 0))) {
        util_string_set(&log_file_name, val);
        return 0;
    }

    if (strcmp(log_file_name, val) == 0) {
        return 0;
    }

    if (util_string_set(&log_file_name, val) < 0) {
        return 0;
    }

    /* if log file was open, close and re-open it */
    log_file_close();
    if (log_to_file) {
        log_file_open();
    }

    return 0;
}

static const resource_int_t resources_int[] = {
    { "LogLimit", LOG_LIMIT_STANDARD, RES_EVENT_NO, NULL,
      &log_limit, set_log_limit, NULL },
    { "LogToFile", 1, RES_EVENT_NO, NULL,
      &log_to_file, set_log_to_file, NULL },
    { "LogToStdout", 1, RES_EVENT_NO, NULL,
      &log_to_stdout, set_log_to_stdout, NULL },
    { "LogToMonitor", 0, RES_EVENT_NO, NULL,
      &log_to_monitor, set_log_to_monitor, NULL },
    RESOURCE_INT_LIST_END
};

static const resource_string_t resources_string[] = {
    { "LogFileName", "", RES_EVENT_NO, NULL,
      &log_file_name, set_log_file_name, NULL },
    RESOURCE_STRING_LIST_END
};

/* called via init_resources() */
int log_resources_init(void)
{
    LOCK();

    if (resources_register_int(resources_int) < 0) {
        UNLOCK_AND_RETURN_INT(-1);
    }

    UNLOCK_AND_RETURN_INT(resources_register_string(resources_string));
}

void log_resources_shutdown(void)
{
    LOCK();

    lib_free(log_file_name);

    UNLOCK();
}

static int log_verbose_opt(const char *param, void *extra_param)
{
    return log_set_limit(LOG_LIMIT_VERBOSE);
}

static int log_silent_opt(const char *param, void *extra_param)
{
    return log_set_limit(LOG_LIMIT_SILENT);
}

static int log_debug_opt(const char *param, void *extra_param)
{
    return log_set_limit(LOG_LIMIT_DEBUG);
}

static int log_logfile_opt(const char *val, void *param)
{
    locked = 0;

    DBG(("log_logfile_opt:%s\n", val));

    if (util_string_set(&log_file_name, val) < 0) {
        return 0;
    }

    /* if log file was open, close and re-open it */
    log_file_close();
    if (log_to_file) {
        log_file_open();
    }

    locked = 1;

    return 0;
}

static const cmdline_option_t cmdline_options[] =
{
    /* NOTE: although we use CALL_FUNCTION, we put the resource that will be
             modified into the array - this helps reconstructing the cmdline */
    { "-logfile", CALL_FUNCTION, CMDLINE_ATTRIB_NEED_ARGS,
      log_logfile_opt, NULL, "LogFileName", NULL,
      "<Name>", "Specify log file name" },
    { "-loglimit", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "LogLimit", NULL,
      "<Type>", "Set log level: (0: None, 255: All)" },
    { "-silent", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      log_silent_opt, NULL, NULL, NULL,
      NULL, "Disable all log output, except errors." },
    { "-verbose", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      log_verbose_opt, NULL, NULL, NULL,
      NULL, "Enable verbose log output." },
    { "-debug", CALL_FUNCTION, CMDLINE_ATTRIB_NONE,
      log_debug_opt, NULL, NULL, NULL,
      NULL, "Enable verbose debug log output." },
    { "-logtofile", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "LogToFile", (void *)1,
      NULL, "Log to the log file." },
    { "+logtofile", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "LogToFile", (void *)0,
      NULL, "Do not log to the log file." },
    { "-logtostdout", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "LogToStdout", (void *)1,
      NULL, "Log to stdout." },
    { "+logtostdout", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "LogToStdout", (void *)0,
      NULL, "Do not log to stdout." },
    { "-logtomonitor", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "LogToMonitor", (void *)1,
      NULL, "Log to the monitor." },
    { "+logtomonitor", SET_RESOURCE, CMDLINE_ATTRIB_NONE,
      NULL, NULL, "LogToMonitor", (void *)0,
      NULL, "Do not log to the monitor." },

    CMDLINE_LIST_END
};

int log_cmdline_options_init(void)
{
    LOCK();

    UNLOCK_AND_RETURN_INT(cmdline_register_options(cmdline_options));
}
#endif

/******************************************************************************/

/* called from c1541.c */
int log_init_with_fd(FILE *f)
{
    LOCK();

    if (f == NULL) {
        UNLOCK_AND_RETURN_INT(-1);
    }

    if (log_file != NULL) {
        DBG(("log_init_with_fd log already open\n"));
        UNLOCK_AND_RETURN_INT(-1);
    }

    if (f == stdout) {
        DBG(("log_init_with_fd stdout\n"));
        log_to_stdout = 1;
    } else {
        log_file = f;
        log_to_file = 1;
    }

    UNLOCK_AND_RETURN_INT(0);
}

/* called via main.c:main_program() */
int log_init(void)
{
    LOCK();

#if 0
    /*
     * The current calling order in main.c (log_init() after processing
     * resources) makes this break if anything in the resource set_*
     * functions does a log_open().  On platforms that have no regular
     * stdout (e.g win32) no logging will be seen.  On win32 startup will
     * also be preceeded by a modal error requester.  / tlr
     */
    if (logs != NULL) {
        UNLOCK_AND_RETURN_INT(-1);
    }
#endif
    DBG(("log_init (log file type:%d)\n", log_file_type));
    if (log_to_file) {
        /* HACK: do not close/reopen if previous log_file_open triggered stdout
                 or custom filename */
        if ((log_file_type == 0) || (log_file_type == 3)) {
            log_file_close();
            log_file_open();
        }
    }

    UNLOCK_AND_RETURN_INT(0);
}

/******************************************************************************/

log_t log_open(const char *id)
{
    log_t new_log = 0;
    log_t i;

    LOCK();

    for (i = 0; i < num_logs; i++) {
        if (logs[i] == NULL) {
            new_log = i;
            break;
        }
    }
    if (i == num_logs) {
        new_log = num_logs++;
        logs = lib_realloc(logs, sizeof(*logs) * num_logs);
    }

    logs[new_log] = lib_strdup(id);

    /* printf("log_open(%s) = %d\n", id, (int)new_log); */
    UNLOCK();
    return new_log;
}

int log_close(log_t log)
{
    LOCK();

    /* printf("log_close(%d)\n", (int)log); */
    if (logs[(unsigned int)log] == NULL) {
        UNLOCK_AND_RETURN_INT(-1);
    }

    lib_free(logs[(unsigned int)log]);
    logs[(unsigned int)log] = NULL;

    UNLOCK_AND_RETURN_INT(0);
}

void log_close_all(void)
{
    log_t i;

    LOCK();

    for (i = 0; i < num_logs; i++) {
        log_close(i);
    }

    lib_free(logs);
    logs = NULL;

    UNLOCK();
}

/******************************************************************************/

/* helper function for formatted output to default logger (stdout) */
static int log_archdep(const char *pretxt, const char *logtxt)
{
    /*
     * ------ Split into single lines ------
     */
    int rc = 0;

    const char *beg = logtxt;
    const char *end = logtxt + strlen(logtxt) + 1;

    while (beg < end) {
        char *eol = strchr(beg, '\n');

        if (eol) {
            *eol = '\0';
        }

        /* output to stdout */
        if (archdep_default_logger(*beg ? pretxt : "", beg) < 0) {
            rc = -1;
            break;
        }

        if (!eol) {
            break;
        }

        beg = eol + 1;
    }

    return rc;
}

/* helper function for formatted output to the monitor */
static int log_monitor(const char *pretxt, const char *logtxt)
{
    int rc = 0;

    mon_out("%s%s\n", pretxt, logtxt);

    return rc;
}

/* helper function for formatted output to the log file */
static int log_tofile(const char *pretxt, const char *logtxt)
{
    int rc = 0;
    if (log_file != NULL) {
        if (fputs(pretxt, log_file) == EOF) {
            rc = -1;
        } else if (fputs(logtxt, log_file) == EOF) {
            rc = -1;
        } else if (fputc ('\n', log_file) == EOF) {
            rc = -1;
        }
    }
    return rc;
}

/* main log helper */
static int log_helper(log_t log, unsigned int level, const char *format,
                      va_list ap)
{
    static const char * const level_strings[8] = {
        "",             /* LOG_LEVEL_NONE */
        "Fatal - ",     /* LOG_LEVEL_FATAL */
        "Error - ",     /* LOG_LEVEL_ERROR */
        "Warning - ",   /* LOG_LEVEL_WARNING */
        "",             /* LOG_LEVEL_INFO */
        "",             /* LOG_LEVEL_VERBOSE */
        "",             /* LOG_LEVEL_DEBUG */
        ""              /* LOG_LEVEL_ALL */
    };

    const char *lvlstr = level_strings[(level >> 5) & 7];

    const signed int logi = (signed int)log;
    int rc = 0;
    char *pretxt = NULL;
    char *logtxt = NULL;

    // va_list ap1, ap2;

    /* exit early if there is no log enabled */
    if ((log_limit < level) ||
        ((log_to_stdout == 0) && (log_to_file == 0) && (log_to_monitor == 0))) {
        return 0;
    }

    if ((logi != LOG_DEFAULT) && (logi != LOG_ERR)) {
        if ((logs == NULL) || (logi < 0)|| (logi >= num_logs) || (logs[logi] == NULL)) {
#ifdef DEBUG
            log_archdep("log_helper: internal error (invalid id or closed log), messages follows:\n", format, ap);
#endif
            return -1;
        }
    }

    /* prepend the log_t prefix, and the loglevel string */
    if ((log_file != NULL) && (logi != LOG_DEFAULT) && (logi != LOG_ERR) && (*logs[logi] != '\0')) {
        pretxt = lib_msprintf("%s: %s", logs[logi], lvlstr);
    } else {
        pretxt = lib_msprintf("%s", lvlstr);
    }
    /* build the log string */
    logtxt = lib_mvsprintf(format, ap);

    if (log_to_stdout) {
        /* output to stdout */
        if (log_archdep(pretxt, logtxt) < 0) {
            rc = -1;
        }
    }

    if (log_to_file) {
        /* output to log file */
        if (log_tofile(pretxt, logtxt) < 0) {
            rc = -1;
        }
    }

    if (log_to_monitor) {
        /* output to monitor */
        if (log_monitor(pretxt, logtxt) < 0) {
            rc = -1;
        }
    }

    lib_free(pretxt);
    lib_free(logtxt);
    return rc;
}

/******************************************************************************
 High level log functions
 ******************************************************************************/

int log_out(log_t log, unsigned int level, const char *format, ...)
{
    va_list ap;
    int rc;

    LOCK();

    va_start(ap, format);
    rc = log_helper(log, level, format, ap);
    va_end(ap);

    UNLOCK_AND_RETURN_INT(rc);
}

int log_message(log_t log, const char *format, ...)
{
    va_list ap;
    int rc;

    LOCK();

    va_start(ap, format);
    rc = log_helper(log, LOG_LEVEL_INFO, format, ap);
    va_end(ap);

    UNLOCK_AND_RETURN_INT(rc);
}

int log_warning(log_t log, const char *format, ...)
{
    va_list ap;
    int rc;

    LOCK();

    va_start(ap, format);
    rc = log_helper(log, LOG_LEVEL_WARNING, format, ap);
    va_end(ap);

    UNLOCK_AND_RETURN_INT(rc);
}

int log_error(log_t log, const char *format, ...)
{
    va_list ap;
    int rc;

    LOCK();

    va_start(ap, format);
    rc = log_helper(log, LOG_LEVEL_ERROR, format, ap);
    va_end(ap);

    UNLOCK_AND_RETURN_INT(rc);
}

int log_debug(const char *format, ...)
{
    va_list ap;
    int rc;

    LOCK();

    va_start(ap, format);
    rc = log_helper(LOG_DEFAULT, LOG_LEVEL_DEBUG, format, ap);
    va_end(ap);

    UNLOCK_AND_RETURN_INT(rc);
}

int log_verbose(const char *format, ...)
{
    va_list ap;
    int rc = 0;

    LOCK();

    va_start(ap, format);
    rc = log_helper(LOG_DEFAULT, LOG_LEVEL_VERBOSE, format, ap);
    va_end(ap);

    UNLOCK_AND_RETURN_INT(rc);
}
