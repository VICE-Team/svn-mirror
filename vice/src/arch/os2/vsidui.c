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
#define INCL_WINSTDSPIN // WinSetSpinVal
#define INCL_WINDIALOGS // WinSendDlgItemMsg
#include "vice.h"

#include <stdlib.h>

#include "dialogs.h"

#include "log.h"
#include "utils.h"
#include "drive.h"      // DRIVE_SYNC_PAL
#include "resources.h"

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
    resources_set_value("SoundDeviceName", (resource_value_t*)"dart2");

    _beginthread(vsid_mainloop, NULL, 0x4000, NULL);

    log_message(LOG_DEFAULT, "--> SID Player mode <--\n");

    return 0;
}

void vsid_ui_display_name(const char *name)
{
    WinSetDlgItemText(hwndVsid, ID_TNAME, (char*)name);
}

void vsid_ui_display_author(const char *author)
{
    WinSetDlgItemText(hwndVsid, ID_TAUTHOR, (char*)author);
}

void vsid_ui_display_copyright(const char *copyright)
{
    WinSetDlgItemText(hwndVsid, ID_TCOPYRIGHT, (char*)copyright);
}

void vsid_ui_display_sync(int sync)
{
    WinSetDlgItemText(hwndVsid, ID_TSYNC, sync==DRIVE_SYNC_PAL?"using PAL":"using NTSC");
}

void vsid_ui_set_default_tune(int nr)
{
}

void vsid_ui_display_tune_nr(int nr)
{
    WinSetSpinVal(hwndVsid, SPB_TUNENO, nr);
    WinSetSpinVal(hwndVsid, SPB_SETTUNE, nr);
}

void vsid_ui_display_nr_of_tunes(int count)
{
    WinSetSpinVal(hwndVsid, SPB_TUNES, count);
}
