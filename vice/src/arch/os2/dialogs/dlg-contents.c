/*
 * dlg-contents.c - The contents-dialog.
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
#define INCL_GPILCIDS           /* Font functions               */
#define INCL_GPIPRIMITIVES      /* GPI primitive functions      */
#define INCL_DOSMEMMGR          /* DOS Memory Manager Functions */
#define INCL_WINERRORS

#define INCL_WINSYS // font
#define INCL_WININPUT
#define INCL_WINDIALOGS
#define INCL_WINLISTBOXES

#include "vice.h"
#include "dialogs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"           // xmalloc
#include "charsets.h"
#include "autostart.h"
#include "imagecontents.h"

#include "log.h"

static void LoadFont(HWND hwnd)
{
    CHAR achFont[] = "11.System VIO";
    // CHAR achFont[] = "27.XXX";
    HPS hps= WinGetPS(hwnd);                /* presentation space handle            */
    HAB hab= WinQueryAnchorBlock(hwnd);
    LONG cFonts;            /* fonts not returned                   */
    LONG lTemp = 0L;        /* font count                           */
    FONTMETRICS *pfm;       /* metrics structure                    */

    if (!hps || !hab)
    {
        log_debug("dlg-contents.c: Cannot determine Anchor Block or PS Handle");
        return;
    }

    if (!GpiLoadFonts(hab, "g:\\c64\\src\\vice\\data\\fonts\\vice-cbm.fon"))
    {
        log_debug("dlg-contents.c: Unable to load vice-cbm.fon");
        return;
    }

    /* Determine the number of fonts. */
    cFonts = GpiQueryFonts(hps, QF_PRIVATE, NULL, &lTemp,
                           (LONG) sizeof(FONTMETRICS), NULL);

    log_debug("dlg-contents.c: vice-cbm.fon contains %i fonts (WinLastError=%x)",
              cFonts, WinGetLastError(hab)&0xffff);

    if (cFonts<1)
    {
        log_debug("dlg-contents.c: File does not contain any font (WinLastError=%x)",
                  WinGetLastError(hab)&0xffff);
        return;
    }

    /* Allocate space for the font metrics. */
    pfm = (FONTMETRICS*)xmalloc(cFonts*sizeof(FONTMETRICS));
    if (!pfm)/*DosAllocMem((VOID *)pfm,(ULONG)(cFonts*sizeof(FONTMETRICS)),
    PAG_COMMIT | PAG_READ | PAG_WRITE)*/
    {
        log_debug("dlg-contents.c: Unable to allocate memory for font metrics");
        return;
    }

    /* Retrieve the font metrics. */
    lTemp = GpiQueryFonts(hps, QF_PRIVATE, NULL, &cFonts,
                          (LONG) sizeof(FONTMETRICS), pfm);

    log_debug("dlg-contents.c: %i fonts left (WinLastError=%x)",
              lTemp, WinGetLastError(hab)&0xffff);

    if (lTemp<0)
    {
        log_debug("dlg-contents.c: Unable to query font (WinLastError=%x)",
                  WinGetLastError(hab)&0xffff);
        return;
    }

    while (cFonts>0)
    {
        cFonts--;
        log_debug("dlg-contents.c: Font  Nr.%3i  %i. Family: %s  Face: %s  Type: %x",
                  cFonts,
                  pfm[cFonts].lAveCharWidth,
                  pfm[cFonts].szFamilyname,
                  pfm[cFonts].szFacename,
                  pfm[cFonts].fsType
                 );
    }
    {
        POINTL ptl = { 10, 10 };
        FATTRS fat;
        APIRET rc;

        fat.usRecordLength = sizeof(FATTRS); /* sets size of structure   */
        fat.fsSelection = 0;       /* uses default selection             */
        fat.lMatch = 0L;           /* does not force match               */
        fat.idRegistry = 0;        /* uses default registry              */
        fat.usCodePage = 850;      /* code-page 850                      */
        fat.lMaxBaselineExt = 8L;  /* requested font height is 8 pels    */
        fat.lAveCharWidth = 8L;    /* requested font width is 8 pels     */
        fat.fsType = 0;            /* uses default type                  */
        fat.fsFontUse = FATTR_FONTUSE_NOMIX;/* doesn't mix with graphics */

        /* Copy Courier to szFacename field */

        strcpy(fat.szFacename ,"CBM Vice");

        rc=GpiCreateLogFont(hps,   /* presentation space             */
                            NULL,  /* does not use logical font name */
                            2L,    /* local identifier               */
                            &fat); /* structure with font attributes */

        if (rc=FONT_MATCH)
            log_debug("Font set as logic font!");

        // GpiSetCharSet(hps, 2L);      /* sets font for presentation space */
        // GpiCharStringAt(hps, &ptl, 5L, "Hello"); /* displays a string    */
    }
    {
        LONG   lcid;      /*  Local identifier. */
        PSTR8  name=NULL; /*  Logical font name. */
        FATTRS attrs;     /*  Attributes of font. */
        LONG   length;    /*  Length of attrs buffer. */
        BOOL   rc;        /*  Success indicator. */

        for (lcid=0; lcid<100; lcid++)
        {
            rc = GpiQueryLogicalFont(hps, lcid, name, &attrs,
                                     sizeof(FATTRS));
            if (rc)
                log_debug("---> %i:  %i.%s", lcid, attrs.lAveCharWidth, attrs.szFacename);
        }
    }
    if (!WinSetDlgFont(hwnd, LB_CONTENTS, achFont))
                       //pfm[0].szFacename))
        log_debug("dlg-contents.c: Unable to set font %s.",
                  pfm[0].szFacename);

}

//#undef p2a
//#define p2a(psz) psz

static MRESULT EXPENTRY pm_contents(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CHAR achFont[] = "11.System VIO";
    static char *image_name=NULL;
    static int first = TRUE;

    switch (msg)
    {
    case WM_INITDLG:
        setDlgOpen(DLGO_CONTENTS);

        image_name = stralloc(mp2);
        first = TRUE;
        break;
    case WM_CLOSE:
        if (image_name)
            free(image_name);
        delDlgOpen(DLGO_CONTENTS);
        break;
    case WM_PAINT:
        {
            if (first)
            {
                char *text;
                image_contents_file_list_t *entry;

                image_contents_t *image=image_contents_read_disk(image_name);

                if (!image)
                {
                    log_debug("dlg-contents.c: Unable to read '%s'.", image_name);
                    WinSendMsg(hwnd, WM_CLOSE, NULL, NULL);
                    break;
                }

                entry=image->file_list;

                if (!WinSetDlgFont(hwnd, LB_CONTENTS, achFont))
                    log_debug("dlg-contents.c: Unable to set font %s.",
                              achFont);

                // LoadFont(hwnd);

                text=xmsprintf(" 0 \"%s\" %s", p2a(image->name), p2a(image->id));
                WinLboxInsertItem(hwnd, LB_CONTENTS, text);
                free(text);
                while (entry)
                {
                    text=xmsprintf(" %-5i\"%s\"%6s", entry->size,
                                   p2a(entry->name), p2a(entry->type));
                    WinLboxInsertItem(hwnd, LB_CONTENTS, text);
                    free(text);
                    entry = entry->next;
                }
                text=xmsprintf(" %i blocks free.", image->blocks_free);
                WinLboxInsertItem(hwnd, LB_CONTENTS, text);
                free(text);

                image_contents_destroy(image);
                first=FALSE;
            }
        }
        break;
    case WM_ADJUSTWINDOWPOS:
        {
            SWP *swp=(SWP*)mp1;
            if (swp->fl&SWP_SIZE)
            {
                /*if (swp->cx<273)*/ swp->cx=273;
                if (swp->cy<160) swp->cy=160;
                WinSetWindowPos(WinWindowFromID(hwnd, LB_CONTENTS), 0, 0, 0,
                                swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                                swp->cy-2*WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)
                                -WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)-2,
                                SWP_SIZE);
            }
        }
        break;
    case WM_CONTROL:
        switch(SHORT1FROMMP(mp1))
        {
        case LB_CONTENTS:
            if (SHORT2FROMMP(mp1)==LN_ENTER)
            {
                int pos=WinLboxQuerySelectedItem(hwnd, LB_CONTENTS);
                if (autostart_autodetect(image_name, NULL, pos))
                    ;// WinError(hwnd, "Cannot autostart specified image.");
                else
                    WinSendMsg(hwnd, WM_CLOSE, NULL, NULL);
                return FALSE;
            }
        }
        break;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void contents_dialog(HWND hwnd, char *szFullFile)
{
    if (dlgOpen(DLGO_CONTENTS)) return;
    WinLoadDlg(HWND_DESKTOP, hwnd, pm_contents, NULLHANDLE,
               DLG_CONTENTS, (void*)szFullFile);
}

