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

#define INCL_GPILCIDS       // Font functions
#define INCL_GPIPRIMITIVES  // GPI primitive functions
#define INCL_DOSMEMMGR      // DOS Memory Manager Functions
#define INCL_WINERRORS
#define INCL_WINSYS         // font
#define INCL_WINLISTBOXES   // WinLbox*
#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"          // xmalloc
#include "dialogs.h"        // WinLbox*
#include "charsets.h"       // a2p, p2a
#include "autostart.h"      // autostart_autodetect
#include "imagecontents.h"

#include "log.h"

static void LoadFont(HWND hwnd)
{
    HWND hwnd2 = WinWindowFromID(hwnd, LB_CONTENTS);

    HPS hps= WinGetPS(hwnd2);                /* presentation space handle            */
    HAB hab= WinQueryAnchorBlock(hwnd2);
    LONG cFonts;            /* fonts not returned                   */
    LONG lTemp = 0L;        /* font count                           */
    FONTMETRICS *pfm;       /* metrics structure                    */

    if (!hps || !hab)
    {
        log_debug("dlg-contents.c: Cannot determine Anchor Block or PS Handle");
        return;
    }


    if (!GpiLoadPublicFonts(hab, "f:\\c64\\src\\vice\\src\\arch\\os2\\fonts\\vice-cbm.fon"))
    {
        log_debug("dlg-contents.c: Unable to load vice-cbm.fon");
        return;
    }
    {
        const char *font = "8.CBM Fixed";
        int rc = WinSetPresParam(hwnd2, PP_FONTNAMESIZE, strlen(font)+1, (void*)font);
    }
    return;



    if (!GpiLoadFonts(hab, "f:\\c64\\src\\vice\\src\\arch\\os2\\fonts\\vice-cbm.fon"))
    {
        log_debug("dlg-contents.c: Unable to load vice-cbm.fon");
        return;
    }

    /* Determine the number of fonts */
    cFonts = GpiQueryFonts(hps, QF_PRIVATE, NULL, &lTemp, 0, NULL);

    log_debug("dlg-contents.c: vice-cbm.fon contains %i fonts (WinLastError=%x)",
              cFonts, WinGetLastError(hab)&0xffff);

    if (cFonts<1)
    {
        log_debug("dlg-contents.c: File does not contain any font (WinLastError=%x)",
                  WinGetLastError(hab)&0xffff);
        //return;
    }

    // Allocate space for the font metrics.
    pfm = (FONTMETRICS*)xmalloc(cFonts*sizeof(FONTMETRICS));
    if (!pfm)
    {
        log_debug("dlg-contents.c: Unable to allocate memory for font metrics");
       // return;
    }

    // Retrieve the font metrics.
    lTemp = GpiQueryFonts(hps, QF_PRIVATE, NULL, &cFonts,
                          (LONG) sizeof(FONTMETRICS), pfm);

    log_debug("dlg-contents.c: %i fonts left (WinLastError=%x)",
              lTemp, WinGetLastError(hab)&0xffff);

    if (lTemp<0)
    {
        log_debug("dlg-contents.c: Unable to query font (WinLastError=%x)",
                  WinGetLastError(hab)&0xffff);
       // return;
    }

    while (cFonts>0)
    {
        cFonts--;
        log_debug("dlg-contents.c: Font.%3i %i. Family: '%s' Face: '%s' Type: %x",
                  cFonts,
                  pfm[cFonts].lAveCharWidth,
                  pfm[cFonts].szFamilyname,
                  pfm[cFonts].szFacename,
                  pfm[cFonts].fsType
                 );
    }
    {
        APIRET rc;
        FATTRS fat;

        int nr = 0;

        fat.usRecordLength = sizeof(FATTRS); /* sets size of structure   */
        fat.fsSelection = 0;         /* uses default selection           */
        fat.lMatch = 0L;             /* does not force match             */
        fat.idRegistry = 0;          /* uses default registry            */
        fat.usCodePage = 850;        /* code-page 850                    */
        fat.lMaxBaselineExt = pfm[0].lMaxBaselineExt;   /* requested font height is 12 pels */
        fat.lAveCharWidth = pfm[0].lAveCharWidth;     /* requested font width is 12 pels  */
        fat.fsType = 0;              /* uses default type                */
        fat.fsFontUse = FATTR_FONTUSE_NOMIX;/* doesn't mix with graphics */

        /* Copy Courier to szFacename field */

        strcpy(fat.szFacename ,pfm[0].szFacename);

        rc=GpiCreateLogFont(hps,        /* presentation space               */
                            (void*)"CBM Fixed",       /* does not use logical font name   */
                            1L,         /* local identifier                 */
                            &fat);      /* structure with font attributes   */

        if (rc==FONT_MATCH)
            log_debug("Font set as logic font!");
        else
        {
            log_debug("GpiCreateLogFont %s failed %d!", fat.szFacename, rc);
            log_debug("dlg-contents.c: (WinLastError=%x)",
                      WinGetLastError(hab)&0xffff);
        }

        // GpiSetCharSet(hps, 1L);      /* sets font for presentation space */
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
    {
        const char *font = "8.CBM Fixed";
        char fnt[250];

        ULONG attr;

        int rc = WinSetPresParam(hwnd2, PP_FONTNAMESIZE, strlen(font)+1, (void*)font);

        int ret = WinQueryPresParam(hwnd2, PP_FONTNAMESIZE, NULL, &attr,
                                    128, &fnt, 0);

        log_debug("dlg-contents.c: rc=%s", (rc?"TRUE":"FALSE"));
        log_debug("1,Set as: %s, %d, %d", fnt, attr, ret);
    }
}

//#undef p2a
//#define p2a(psz) psz

static MRESULT EXPENTRY pm_contents(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    CHAR achFont[] = "11.System VIO";
    static char *image_name=NULL;

    switch (msg)
    {
    case WM_INITDLG:
        {
            char *text;

            image_contents_file_list_t *entry;
            image_contents_t           *image;

            image_name = stralloc(mp2);

            image=image_contents_read_disk(image_name);

            if (!image)
            {
                log_debug("dlg-contents.c: Unable to read '%s'.", image_name);
                WinSendMsg(hwnd, WM_CLOSE, NULL, NULL);
                break;
            }

            entry=image->file_list;

            //                if (!WinSetDlgFont(hwnd, LB_CONTENTS, achFont))
            //                    log_debug("dlg-contents.c: Unable to set font %s.",
            //                              achFont);

            LoadFont(hwnd);

//            text=xmsprintf(" 0 \"%s\" %s", p2a(image->name), p2a(image->id));
            text=xmsprintf(" 0 \"%s\" %s", image->name, image->id);
            WinLboxInsertItem(hwnd, LB_CONTENTS, text);
            free(text);
            while (entry)
            {
                text=xmsprintf(" %-5i\"%s\"%6s", entry->size,
//                               p2a(entry->name), p2a(entry->type));
                               entry->name, entry->type);
                WinLboxInsertItem(hwnd, LB_CONTENTS, text);
                free(text);
                entry = entry->next;
            }
            text=xmsprintf(" %i BLOCKS FREE.", image->blocks_free);
            WinLboxInsertItem(hwnd, LB_CONTENTS, text);
            free(text);

            image_contents_destroy(image);
        }
        break;

    case WM_CLOSE:
        {
            HWND hwnd2 = WinWindowFromID(hwnd, LB_CONTENTS);
            ULONG attr;
            char fnt[250];
            int ret = WinQueryPresParam(hwnd2, PP_FONTNAMESIZE, NULL, &attr,
                                        128, &fnt, 0);
            log_debug("2,Set as: %s, %d, %d", fnt, attr, ret);
        }
        {
            const char *font = "11.System VIO";
            HWND hwnd2 = WinWindowFromID(hwnd, LB_CONTENTS);
            int rc = WinSetPresParam(hwnd2, PP_FONTNAMESIZE, strlen(font)+1, (void*)font);

            rc = GpiUnloadPublicFonts(WinQueryAnchorBlock(WinWindowFromID(hwnd, LB_CONTENTS)), "f:\\c64\\src\\vice\\data\\fonts\\vice-cbm.fon");
            if (!rc)
                log_debug("dlg-contents.c: Unable to unload vice-cbm.fon %d", rc);
        }

        if (!image_name)
            break;

        free(image_name);
        image_name=NULL;
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
        if (mp1==MPFROM2SHORT(LB_CONTENTS, LN_ENTER))
        {
            const int pos=WinQueryLboxSelectedItem((HWND)mp2);
            if (autostart_autodetect(image_name, NULL, pos))
                ;// WinError(hwnd, "Cannot autostart specified image.");
            else
                WinSendMsg(hwnd, WM_CLOSE, NULL, NULL);
            return FALSE;
        }
        break;

    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void contents_dialog(HWND hwnd, char *szFullFile)
{
    HWND hwnd2 = NULLHANDLE;

    if (WinIsWindowVisible(hwnd2))
        return;

    hwnd2 = WinLoadDlg(HWND_DESKTOP, hwnd, pm_contents, NULLHANDLE,
                       DLG_CONTENTS, (void*)szFullFile);
}

