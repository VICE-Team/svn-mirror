/** \file   uimon-fallback.c
 * \brief   Fallback implementation for the ML-Monitor for when the VTE library
 *          is not available
 *
 * \author  Fabrizio Gennari <fabrizio.ge@tiscali.it>
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

/* FIXME: this code is simple enough to be merged back into uimon.c */

#include "vice.h"

#include "debug_gtk3.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#if (defined(sun) || defined(__sun)) && (defined(__SVR4) || defined(__svr4__))
#include <sys/stat.h>
#endif

#ifdef WIN32_COMPILE
#include <windows.h>
#include <winternl.h>
#endif

#include "console.h"
#include "lib.h"
#include "log.h"
#include "ui.h"
#include "uimon.h"
#include "uimon-fallback.h"

static console_t *console_log_local = NULL;

/** \brief  NOP
 *
 * \return  0
 */
int consolefb_close_all(void)
{
    /* This is a no-op on GNOME, should be fine here too */
    return native_console_close_all();
}


/** \brief  NOP
 *
 * \return  0
 */
int consolefb_init(void)
{
    return native_console_init();
}

int consolefb_out(console_t *log, const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    native_console_out(console_log_local, format, ap);
    va_end(ap);

    return 0;
}

console_t *uimonfb_window_open(void)
{
    console_log_local = native_console_open(0);
    if (console_log_local == NULL) {
        return NULL;
    }
    /* partially implemented */
    INCOMPLETE_IMPLEMENTATION();
#ifdef HAVE_MOUSE
    /* ui_restore_mouse(); */
#endif
    /* ui_focus_monitor(); */
    return console_log_local;
}

void uimonfb_window_close(void)
{
    native_console_close(console_log_local);

    uimon_window_suspend();
}

void uimonfb_window_suspend( void )
{
    /* ui_restore_focus(); */
#ifdef HAVE_MOUSE
    /* ui_check_mouse_cursor(); */
#endif
    NOT_IMPLEMENTED_WARN_ONLY();
}

console_t *uimonfb_window_resume(void)
{
    if (console_log_local) {
        /* partially implemented */
        INCOMPLETE_IMPLEMENTATION();
#ifdef HAVE_MOUSE
        /* ui_restore_mouse(); */
#endif
        /* ui_focus_monitor(); */
        return console_log_local;
    }
    log_error(LOG_DEFAULT, "uimonfb_window_resume: log was not opened.");
    return uimon_window_open();
}

int uimonfb_out(const char *buffer)
{
    native_console_out(console_log_local, "%s", buffer);
    return 0;
}

char *uimonfb_get_in(char **ppchCommandLine, const char *prompt)
{
    return native_console_in(console_log_local, prompt);
}

void uimonfb_notify_change( void )
{
}

void uimonfb_set_interface(struct monitor_interface_s **monitor_interface_init, int count)
{
}
