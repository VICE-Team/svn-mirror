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

static HWND hwnd;

void vsid_mainloop(VOID *arg)
{
    APIRET rc;

    //
    // get pm anchor, create msg queue
    //
    HAB hab = WinInitialize(0);            // Initialize PM
    HMQ hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    //
    // open dialog
    //
    hwnd = vsid_dialog();

    if (rc=DosSetPriority(PRTYS_THREAD, PRTYC_REGULAR, +1, 0))
        log_message(LOG_DEFAULT, "vsidui.c: Error DosSetPriority (rc=%li)", rc);

    //
    // MAINLOOP
    //
    WinProcessDlg(hwnd);

    //
    // WinProcessDlg() does NOT destroy the window on return! Do it here,
    // otherwise the window procedure won't ever get a WM_DESTROY,
    // which we may want :-)
    //
    WinDestroyWindow(hwnd);

    //
    // destroy msg queue, release pm anchor
    //
    if (!WinDestroyMsgQueue(hmq))
        log_message(LOG_DEFAULT,"vsidui.c: Error! Destroying Msg Queue.");
    if (!WinTerminate (hab))
        log_message(LOG_DEFAULT,"vsidui.c: Error! Releasing PM anchor.");

    //
    // shutdown emulator thread
    //
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
    WinSetDlgItemText(hwnd, ID_TNAME, (char*)name);
}

void vsid_ui_display_author(const char *author)
{
    WinSetDlgItemText(hwnd, ID_TAUTHOR, (char*)author);
}

void vsid_ui_display_copyright(const char *copyright)
{
    WinSetDlgItemText(hwnd, ID_TCOPYRIGHT, (char*)copyright);
}

void vsid_ui_display_sync(int sync)
{
    WinSetDlgItemText(hwnd, ID_TSYNC, sync==DRIVE_SYNC_PAL?"using PAL":"using NTSC");
}

void vsid_ui_set_default_tune(int nr)
{
}

void vsid_ui_display_tune_nr(int nr)
{
    char txt[3]="-";
    if (nr<100)
        sprintf(txt, "%d", nr);
    WinSetDlgItemText(hwnd, ID_TUNENO, txt);
    WinSetSpinVal(hwnd, SPB_SETTUNE, nr);
}

void vsid_ui_display_nr_of_tunes(int count)
{
    char txt[3]="-";
    if (count<100)
        sprintf(txt, "%d", count);
    WinSetDlgItemText(hwnd, ID_TUNES, txt);
}

void vsid_ui_display_time(unsigned int sec)
{
    char txt[6]="--:--";
    if (sec<600)
        sprintf(txt, "%02d:%02d", (sec/60)%100, sec%60);
    WinSetDlgItemText(hwnd, ID_TIME, txt);
}
