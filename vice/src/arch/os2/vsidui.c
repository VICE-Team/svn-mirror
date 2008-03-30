/*
 * vsidui.c - Implementation of the VSID UI.
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
 * based on c64ui.c written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
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

#define INCL_DOSPROCESS // DosSetPriority
#include "vice.h"

#include <stdlib.h>

#include "log.h"
#include "dialogs.h"

extern int trigger_shutdown;

void vsid_mainloop(VOID *arg)
{
    APIRET rc;
    HAB   hab;  // Anchor Block to PM
    HMQ   hmq;  // Handle to Msg Queue
    QMSG  qmsg; // Msg Queue Event

    hab = WinInitialize(0);            // Initialize PM
    hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    vsid_dialog();

    if (rc=DosSetPriority(PRTYS_THREAD, PRTYC_REGULAR, +1, 0))
        log_message(LOG_DEFAULT, "vsidui.c: Error DosSetPriority (rc=%li)", rc);

    while (WinGetMsg (hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg (hab, &qmsg);

    if (!WinDestroyMsgQueue(hmq))
        log_message(LOG_DEFAULT,"vsidui.c: Error! Destroying Msg Queue.");
    if (!WinTerminate (hab))
        log_message(LOG_DEFAULT,"vsidui.c: Error! Releasing PM anchor.");

    trigger_shutdown = 1;

    DosSleep(5000); // wait 5 seconds
    log_debug("Brutal Exit!");
    exit(0);        // end VICE in all cases
}

int vsid_ui_init(void)
{
    _beginthread(vsid_mainloop, NULL, 0x4000, NULL);

    log_message(LOG_DEFAULT, "--> SID Player mode <--\n");

    return 0;
}

void vsid_set_tune(int tune)
{
    log_message(LOG_DEFAULT, "vsid_set_tune");
}

