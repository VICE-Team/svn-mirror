/*
 * sdlmain.c - SDL startup.
 *
 * Written by
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

#include "log.h"
#include "machine.h"
#include "main.h"

#include "vice_sdl.h"

#ifdef __XBOX__
void XBoxStartup(void)
{
    int argc = 1;
    char *argv[2];

    argv[0] = "vice";
    argv[1] = NULL;

    main_program(argc, argv);
}
#else
int main(int argc, char **argv)
{
#ifdef DINGOO_NATIVE
    int argc_local = 1;
    char *argv_local[2];
    set_dingoo_pwd(argv[0]);

    argv_local[0] = "vice";
    argv_local[1] = NULL;

    return main_program(argc_local, argv_local);
#else
    return main_program(argc, argv);
#endif
}
#endif

void main_exit(void)
{
    log_message(LOG_DEFAULT, "\nExiting...");

    machine_shutdown();

    putchar('\n');
}
