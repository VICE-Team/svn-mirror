/** \file   archdep_fix_streams.c
 * \brief   Fix stdin, stdout, stderr streams if applicable
 *
 * Redirects stdin, stdout and stderr in case these are not available to the
 * running emulator. Specifically this means Windows compiled with the -mwindows
 * switch, causing stdin, stdout and stderr to be disconnected from the spawning
 * shell (cmd.exe, looks like msys2's shell already does this hack).
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 *
 * OS support:
 *  - Windows
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

#include "archdep_fix_streams.h"

/** \fn archdep_fix_streams
 * \brief   Fix standard streams
 *
 * Make the standard streams `stdin`, `stdout` and `stderr` available to the
 * running application so we can log to the terminal, if any, using those
 * streams.
 *
 * \todo    Properly detect (if possible) if we're already redirected to a file
 *          or pipe so `x64sc -help > help.txt` works. GetFileType() should
 *          return something other than FILE_TYPE_CHAR when redirected or piped,
 *          but of course that doesn't actually work on this shitshow they call
 *          Windows.
 */

#ifdef WINDOWS_COMPILE

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>

void archdep_fix_streams(void)
{
    /* With --enable-debug we compile with -mconsole on Windows, so the streams
     * are available as they should be. */
#ifndef DEBUG
    const char *msystem;

    /* try to attach a console to the spawning process (cmd.exe), but not when
     * running from an msys2 shell */
    msystem = getenv("MSYSTEM");
    if (msystem == NULL || *msystem == '\0') {
        if (AttachConsole(ATTACH_PARENT_PROCESS)) {
            FILE *fp_stdin = stdin;
            FILE *fp_stdout = stdout;
            FILE *fp_stderr = stderr;
            HANDLE stdhandle;

            /* redirect stdin */
            stdhandle = GetStdHandle(STD_INPUT_HANDLE);
            if (stdhandle) {
                if (GetFileType(stdhandle) == FILE_TYPE_CHAR) {
                    freopen_s(&fp_stdin, "CONIN$", "r", stdin);
                }
                CloseHandle(stdhandle);
            }

            /* redirect stdout */
            stdhandle = GetStdHandle(STD_OUTPUT_HANDLE);
            if (stdhandle) {
                if (GetFileType(stdhandle) == FILE_TYPE_CHAR) {
                    freopen_s(&fp_stdout, "CONOUT$", "w", stdout);
                }
            }

            /* redirect stderr */
            stdhandle = GetStdHandle(STD_ERROR_HANDLE);
            if (stdhandle) {
                if (GetFileType(stdhandle) == FILE_TYPE_CHAR) {
                    freopen_s(&fp_stderr, "CONOUT$", "w", stderr);
                }
                CloseHandle(stdhandle);
            }
        } else {
            /* Can't report failure since we can't use (f)printf and VICE's
             * logging system isn't initialized yet =) */
            /* NOP */
        }
    }
#endif
}

#else
void archdep_fix_streams(void)
{
    /* Other OSes aren't retarded */
}
#endif
