/*
 * winmain.c - Windows startup.
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#ifdef HAVE_CRTDBG
#include <crtdbg.h>
#endif
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include "log.h"
#include "machine.h"
#include "main.h"
#include "winmain.h"
#include "video.h"

HINSTANCE winmain_instance;
HINSTANCE winmain_prev_instance;
int winmain_cmd_show;

#ifndef IDE_COMPILE
#  if !defined(__MSVCRT__) && !defined(_MSC_VER) && !defined(_WIN64) && !defined(__WATCOMC__) && !defined(WATCOM_COMPILE)
extern void __GetMainArgs(int *, char ***, char ***, int);
#  else
typedef struct {
    int newmode;
} _startupinfo;

extern void __wgetmainargs(int *, wchar_t ***, wchar_t ***, int, _startupinfo *);
#  endif
#endif

int PASCAL WinMain(HINSTANCE instance, HINSTANCE prev_instance, TCHAR *cmd_line, int cmd_show)
{
    winmain_instance = instance;
    winmain_prev_instance = prev_instance;
    winmain_cmd_show = cmd_show;

#if defined(__MSVCRT__) || defined(_MSC_VER) || defined(_WIN64) || defined(__WATCOMC__) || defined(WATCOM_COMPILE)
#  ifdef _DEBUG
    {
        int tmpFlag;

        // Get current flag
        tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

        // Turn on leak-checking bit
        tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
        tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

        // Set flag to the new value
        _CrtSetDbgFlag(tmpFlag);
    }
#  endif
#  ifndef IDE_COMPILE
    if (!__argc) {
        _startupinfo start_info;
        int vice_argc = 0;
        char **vice_argv = 0;
        char **dummy = 0;

        start_info.newmode = 0;

        __wgetmainargs(&vice_argc, &vice_argv, &dummy, -1, &start_info);

        main_program(vice_argc, vice_argv);
    } else {
        main_program(__argc, __argv);
    }
#  else
    main_program(__argc, __argv);
#  endif
#else
#  ifndef IDE_COMPILE
    if (_argc) {
        main_program(_argc, _argv);
    } else {
        int vice_argc = 0;
        char **vice_argv = 0;
        char **dummy = 0;

        __GetMainArgs(&vice_argc, &vice_argv, &dummy, -1);
        main_program(vice_argc, vice_argv);
    }
#else
    main_program(_argc, _argv);
#endif
#endif

    return 0;
}

void main_exit(void)
{
#ifdef HAVE_SIGNAL_H
    /* Disable SIGINT.  This is done to prevent the user from keeping C-c
       pressed and thus breaking the cleanup process, which might be
       dangerous.  */
    signal(SIGINT, SIG_IGN);
#endif

    log_message(LOG_DEFAULT, "\nExiting...");

    machine_shutdown();
}
