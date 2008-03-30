/*
 * os2main.c - OS/2 startup.
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

#include <stdio.h>
#include <signal.h>

#include "log.h"
#include "main.h"


int main(int argc, char **argv)
{
    return main_program(argc, argv);
}

/*
static int    argc;
static char **argv;

void ViceMain(void *arg)
{
    main_program(argc, argv);
}

int main(int ac, char **av)
{
    TID tid;

    argc = ac;
    argv = av;

    tid = _beginthread(ViceMain, NULL, 0x100000, NULL);

    DosWaitThread(&tid, 0);  // * DCWW_WAIT *

    return 0;
}
*/

void main_exit(void)
{
    /* Disable SIGINT.  This is done to prevent the user from keeping C-c
       pressed and thus breaking the cleanup process, which might be
       dangerous.  */
    log_message(LOG_DEFAULT, "\nExiting...");
    signal(SIGINT, SIG_IGN);

    //---    resources_set_int("Sound", (int)FALSE);
    //---    DosSleep(500);

    //---    machine_shutdown();
    //       video_free();
    //       sound_close(); // Be sure sound device is closed.
    // Maybe we need some DosSleep(500)...

    //---#ifdef HAS_JOYSTICK
    //---    joystick_close();
    //---#endif
}

