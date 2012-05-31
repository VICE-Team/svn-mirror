/*
 * dragndrop.c - Drag'n'Drop interface for Vice/2
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_WINSTDDRAG      // Drg*
#define INCL_WINPOINTERS     // WinLoadPointer

#include <os2.h>
#include <stdio.h>      // FILE
#include <string.h>     // strlen, strcat

#include "dialogs.h"
#include "machine.h"
#include "resources.h"
#include "autostart.h"  // autostart_autodetect

#ifdef __X64__
#include "psid.h"       // psid_init_driver
#endif

MRESULT DragOver(PDRAGINFO pDraginfo)
{
    FILE *f;
    char dir[CCHMAXPATH];
    char nam[CCHMAXPATH];
    HPOINTER  hpt;
    DRAGITEM *pditem;

    /*
     * Determine if a drop can be accepted.
     */
    if (pDraginfo->usOperation != DO_MOVE && pDraginfo->usOperation != DO_COPY && pDraginfo->usOperation != DO_LINK &&
        pDraginfo->usOperation != DO_UNKNOWN && pDraginfo->usOperation != DO_DEFAULT) {
        return MRFROM2SHORT(DOR_NODROPOP, 0);
    }

    pditem = DrgQueryDragitemPtr(pDraginfo, 0);

    /*
     * check if it is an OS/2 File
     */
    if (!DrgVerifyRMF(pditem, "DRM_OS2FILE", NULL)) {
        return MRFROM2SHORT(DOR_NEVERDROP, 0);
    }

    DrgQueryStrName(pditem->hstrContainerName, CCHMAXPATH, dir);
    DrgQueryStrName(pditem->hstrSourceName, CCHMAXPATH - strlen(dir) - 1, nam);

    if (!(f = fopen(strcat(dir, nam), "r"))) {
        return MRFROM2SHORT(DOR_NEVERDROP, 0);
    }
    fclose(f);

    hpt = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, 0x100);
    if (hpt) {
        DrgSetDragPointer(pDraginfo, hpt);
    }

    return MRFROM2SHORT(DOR_DROP, DO_UNKNOWN);
}

MRESULT Drop(HWND hwnd, PDRAGINFO pDraginfo)
{
    char dir[CCHMAXPATH];
    char nam[CCHMAXPATH];

    const DRAGITEM *pditem = DrgQueryDragitemPtr(pDraginfo, 0);

    if (!DrgQueryStrName(pditem->hstrContainerName, sizeof(dir), dir) || !DrgQueryStrName(pditem->hstrSourceName, CCHMAXPATH - strlen(dir) - 1, nam)) {
        return NULL;
    }

    strcat(dir, nam);

#ifdef __X64__
    if (machine_class != VICE_MACHINE_VSID) {
#endif
        if (autostart_autodetect(dir, NULL, 0, AUTOSTART_MODE_RUN) >= 0) {
            return NULL;
        }
#ifdef __X64__
    } else {
        if (machine_autodetect_psid(dir) >= 0) {
            psid_init_driver();
            resources_set_int("PSIDTune", 0);
            return NULL;
        }
    }
#endif

    ViceErrorDlg(hwnd, 0x101, " Drop File:\n Cannot autostart/play file.");
    return NULL;
}

MRESULT DragDrop(HWND hwnd, ULONG msg, DRAGINFO *info)
{
    MRESULT mr;

    if (!DrgAccessDraginfo(info)) {
        return MRFROM2SHORT(DOR_NODROPOP, 0);
    }

    switch (msg) {
        case DM_DRAGOVER:
            mr = DragOver(info);
            break;
        case DM_DROP:
            mr = Drop(hwnd, info);
            break;
    }

    DrgFreeDraginfo(info);
    return mr;
}
