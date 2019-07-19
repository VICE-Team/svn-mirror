/** \file   console_unix.c
 * \brief   Unix specific console access interface for SDL.
 *
 * \author  Hannu Nuotio <hannu.nuotio@tut.fi>
 * \author  Andreas Boose <viceteam@t-online.de>
 */

/*
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

#include "console.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if (defined(sun) || defined(__sun)) && (defined(__SVR4) || defined(__svr4__))
#include <sys/stat.h>
#endif

#include "lib.h"
#include "log.h"
#include "charset.h"

static FILE *mon_input, *mon_output;

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#ifdef WIN32_COMPILE
static int vice_ismsystty(int fd)
{
    intptr_t h_stdin = _get_osfhandle(fd);
    char ntfn_bytes[sizeof(OBJECT_NAME_INFORMATION) + 256 * sizeof(WCHAR)];
    OBJECT_NAME_INFORMATION *ntfn = (OBJECT_NAME_INFORMATION*) ntfn_bytes;
    NTSTATUS status;
    ULONG ntfn_size = sizeof(ntfn_bytes);
    USHORT i, l;
    wchar_t c, *s0;

    memset(ntfn, 0, ntfn_size);
    status = NtQueryObject((HANDLE)h_stdin, ObjectNameInformation, ntfn, ntfn_size, &ntfn_size);

    if (!NT_SUCCESS(status)) {
        return 0;
    }

    l = ntfn->Name.Length;
    s0 = ntfn->Name.Buffer;
    /* Check for "\Device\NamedPipe" */
    {
        USHORT l1 = l;
        wchar_t *s1 = s0;
        wchar_t expect[] = L"\\Device\\NamedPipe\\";

        if (s0[0] == '\\' && s0[1] == '\\' && s0[2] == '?' && s0[3] == '\\') {
            l1 -= 4;
            s1 += 4;
        }
        for (i = 0; i < l1; i++) {
            wchar_t e = expect[i];
            c = s1[i];
            if (!e) {
                break;
            }
            if (c != e) {
                return 0;
            }
        }
    }
    /* Look for "-pty%d-" */
    for (i = 0; i < l; i++) {
        c = s0[i];
        if (c == '-') {
            wchar_t *s = s0 + i + 1;
            if (s[0] == 'p' && s[1] == 't' && s[2] == 'y' && (c = s[3]) && (c >= '0') && (c <= '9'))
            {
                s += 4;
                while ((c = *s) && (c >= '0') && (c <= '9')) {
                    s++;
                }
                if (c == '-' || c == 0) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

static int vice_isatty(int fd)
{
    if (!isatty(fileno(stdin))) {
        return vice_ismsystty(fileno(stdin));
    }
    return 1;
}
#else
#define vice_isatty isatty
#endif

typedef struct console_private_s {
    FILE *input, *output;
} console_private_t;

console_t *native_console_open(const char *id)
{
#ifdef HAVE_SYS_IOCTL_H
    struct winsize w;
#endif
   
    console_t *console;
    console_private_t *conpriv;

    console = lib_malloc(sizeof(console_t));
    conpriv = lib_malloc(sizeof(console_private_t));
    console->private = conpriv;
    conpriv->input = NULL;
    conpriv->output = NULL;
    
    if (!vice_isatty(fileno(stdin))) {
        log_error(LOG_DEFAULT, "native_console_open: stdin is not a tty.");
        goto exitnull;
    }
    if (!vice_isatty(fileno(stdout))) {
        log_error(LOG_DEFAULT, "native_console_open: stdout is not a tty.");
        goto exitnull;
    }
    
    /* change window title for console identification purposes */
    if (getenv("WINDOWID") == NULL) {
        printf("\033]2;VICE monitor console (%d)\007", (int)getpid());
    }

#if !defined(HAVE_READLINE) || !defined(HAVE_READLINE_READLINE_H)
    mon_input = conpriv->input = stdin;
    mon_output = conpriv->output = stdout;
#endif

#ifdef HAVE_SYS_IOCTL_H
    if (ioctl(fileno(stdin), TIOCGWINSZ, &w)) {
        console->console_xres = 80;
        console->console_yres = 25;
    } else {
        console->console_xres = w.ws_col >= 40 ? w.ws_col : 40;
        console->console_yres = w.ws_row >= 22 ? w.ws_row : 22;
    }
#else
    console->console_xres = 80;
    console->console_yres = 25;
#endif
    console->console_can_stay_open = 1;
    console->console_cannot_output = 0;
    
    return console;
    
exitnull:
    lib_free(console->private);
    lib_free(console);
    return NULL;
}

int native_console_close(console_t *log)
{
    lib_free(log);
    return 0;
}

int native_console_out(console_t *log, const char *format, ...)
{
    va_list ap;
    char *buf;

    va_start(ap, format);
    buf = lib_mvsprintf(format, ap);
    va_end(ap);

    if (buf) {
        if (log && (log->private->output)) {
            fprintf(log->private->output, "%s", buf);
        } else {
            fprintf(stdout, "%s", buf);
        }
        lib_free(buf);
    }
    return 0;
}

int native_console_petscii_out(console_t *log, const char *format, ...)
{
    va_list ap;
    char *buf;
    unsigned char c;
    int i;

    va_start(ap, format);
    buf = lib_mvsprintf(format, ap);
    va_end(ap);

    if (buf) {
        for (i = 0; (c = buf[i]) != 0; i++) {
            if (c == '\t') {
                buf[i] = ' ';
            } else if (((c < 32) || (c > 126)) && (c != '\n')) {
                buf[i] = charset_p_toascii(c, 1);
            }
        }

        if (log && (log->private->output)) {
            fprintf(log->private->output, "%s", buf);
        } else {
            fprintf(stdout, "%s", buf);
        }
        lib_free(buf);
    }
    return 0;
}

#ifndef HAVE_READLINE
char *readline(const char *prompt)
{
    char *p = lib_malloc(1024);
    size_t len;

    native_console_out(NULL, "%s", prompt);

    fflush(mon_output);
    if (fgets(p, 1024, mon_input) == NULL) {
        lib_free(p);
        return NULL;
    }

    /* Remove trailing newlines.  */
    for (len = strlen(p);
            len > 0 && (p[len - 1] == '\r' || p[len - 1] == '\n');
            len--) {
        p[len - 1] = '\0';
    }
    return p;
}
#endif

char *native_console_in(console_t *log, const char *prompt)
{
    char *p, *ret_sting;

    p = readline(prompt);
#ifdef HAVE_READLINE
    if (p && *p) {
        add_history(p);
    }
#endif
    ret_sting = lib_strdup(p);
    free(p);

    return ret_sting;
}

int native_console_init(void)
{
#if defined(HAVE_READLINE) && defined(HAVE_RLNAME)
    rl_readline_name = "VICE";
#endif
    return 0;
}

int native_console_close_all(void)
{
    return 0;
}
