/*
 * winmain.c - Windows startup.
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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

#include "winmain.h"

HINSTANCE winmain_instance;
HINSTANCE winmain_prev_instance;
LPSTR winmain_cmd_line;
int winmain_cmd_show;

int PASCAL WinMain(HINSTANCE instance, HINSTANCE prev_instance,
                   LPSTR cmd_line, int cmd_show)
{
    winmain_instance = instance;
    winmain_prev_instance = prev_instance;
    winmain_cmd_line = cmd_line;
    winmain_cmd_show = cmd_show;

    MAIN_PROGRAM(_argc, _argv);

    return 0;
}
