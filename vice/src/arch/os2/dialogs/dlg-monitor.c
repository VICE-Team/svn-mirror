/*
 * dlg-monitor.c - The monitor-dialog.
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

#define INCL_WINSYS
#define INCL_WININPUT
#define INCL_DOSPROCESS
#define INCL_WINDIALOGS
#define INCL_WINMENUS        // WinLoadMenu
#define INCL_WINFRAMEMGR     // FID_*
#define INCL_WINWINDOWMGR    // QWL_USER
#define INCL_WINLISTBOXES
#define INCL_WINENTRYFIELDS
#include "vice.h"

#include <os2.h>

#include "dialogs.h"
#include "dlg-monitor.h"

#include <string.h>
#include <stdlib.h>

#include "mon.h"             // e_*
#include "utils.h"
#include "mos6510.h"         // P_*
#include "archdep.h"         // archdep_boot_path
#include "snippets\pmwin2.h" // WinSetDlgFont

#include "log.h"

HWND hwndMonitor = NULLHANDLE;
HWND hwndMonreg  = NULLHANDLE;
HWND hwndMonreg8 = NULLHANDLE;
HWND hwndMonreg9 = NULLHANDLE;
HWND hwndMondis  = NULLHANDLE;
HWND hwndMondis8 = NULLHANDLE;
HWND hwndMondis9 = NULLHANDLE;

int trigger_console_exit;

// --------------------------------------------------------------------------

static char *fmt(unsigned int val, unsigned char sz)
{
    static char txt[5];

    char tmp[7];
    sprintf(tmp, "%%0%ix", sz);
    sprintf(txt, tmp, val);

    return txt;
}

static const char *mon_dis(MEMSPACE mem, ADDRESS loc, unsigned int *size)
{
    const BYTE op = mon_get_mem_val(mem, loc);
    const BYTE p1 = mon_get_mem_val(mem, loc+1);
    const BYTE p2 = mon_get_mem_val(mem, loc+2);

    return mon_disassemble_to_string_ex(mem, loc, op, p1, p2, 1, size);
}

static void UpdateDisassembly(HWND hwnd)
{
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(hwnd, QWL_USER);

    const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);
          ULONG cnt = WinQueryLboxCount(lbox);

    ADDRESS loc = mon_get_reg_val(mem, e_PC);

    unsigned int sel=0;
    unsigned int size;

    unsigned int i = 0;

    //
    // Try to find the actual adress in the list
    //
    for (i=0; i<cnt; i++)
    {
        if ((WinLboxItemHandle(lbox, i)&0xffff)!=loc)
            continue;

        //
        // if the adress is in the list it has to be selected afterwards
        //
        sel = i;
        break;
    }

    if (i==cnt)
    {
        //
        // if the adress wasn't found create the list from scratch
        //
        WinLboxEmpty(lbox);
        i = 0;
        cnt = 50;
    }
    else
    {
        int pos = i;
        //
        // if the adress exists already look for the next adress
        // which can be apended
        //
        LONG val = WinLboxItemHandle(lbox, cnt-1);
        loc = (val&0xffff) + (val>>16);

        //
        // make sure that we have 50 instructions from
        // the actual one in the list
        //
        i = cnt;
        cnt = pos+50;
    }

    for(; i<cnt; i++)
    {
        //
        // check if a label for this adress exists
        //
        const char *label = mon_symbol_table_lookup_name(mem, loc);

        //
        // get the disassembly string for the actual instruction
        //
        const char *p = mon_dis(mem, loc, &size);

        //
        // create the output string and fill it into the list
        //
        char *buffer = xmsprintf("%04X: %-30s%s%s%s", loc, p,
                                 label?"[":"", label?label:"", label?"]":"");
        WinLboxInsertItem(lbox, buffer);
        free(buffer);

        //
        // set the adress and size of the instruction as item handle
        //
        WinLboxSetItemHandle(lbox, i, (size<<16)|loc);

        //
        // step to the next instruction
        //
        loc += size;
    }

    WinSetWindowULong(lbox, QWL_USER, sel);
    WinLboxSelectItem(lbox, sel);
}

static void UpdateRegisters(HWND hwnd)
{
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(hwnd, QWL_USER);

    //
    // Update CPU register values
    //
    const int flags = mon_get_reg_val(mem, e_FLAGS);

    //
    // FIXME: check for changes -> change color
    //
    WinSetDlgItemText(hwnd, ID_REGA,  fmt(mon_get_reg_val(mem, e_PC), 4));
    WinSetDlgItemText(hwnd, ID_REGAC, fmt(mon_get_reg_val(mem, e_A),  2));
    WinSetDlgItemText(hwnd, ID_REGX,  fmt(mon_get_reg_val(mem, e_X),  2));
    WinSetDlgItemText(hwnd, ID_REGY,  fmt(mon_get_reg_val(mem, e_Y),  2));
    WinSetDlgItemText(hwnd, ID_REGSP, fmt(mon_get_reg_val(mem, e_SP), 2));
    WinSetDlgItemText(hwnd, ID_REG01, fmt(mon_get_mem_val(mem, 1),    2));

    WinEnableControl(hwnd, ID_REGC, flags&P_CARRY);
    WinEnableControl(hwnd, ID_REGZ, flags&P_ZERO);
    WinEnableControl(hwnd, ID_REGI, flags&P_INTERRUPT);
    WinEnableControl(hwnd, ID_REGD, flags&P_DECIMAL);
    WinEnableControl(hwnd, ID_REGB, flags&P_BREAK);
    WinEnableControl(hwnd, ID_REGM, flags&P_UNUSED);
    WinEnableControl(hwnd, ID_REGV, flags&P_OVERFLOW);
    WinEnableControl(hwnd, ID_REGN, flags&P_SIGN);
}

// --------------------------------------------------------------------------

static MRESULT EXPENTRY pm_monreg(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg)
    {
    case WM_INITDLG:
        WinSetWindowPtr(hwnd, QWL_USER, (VOID*)mp2);
        switch ((MEMSPACE)mp2)
        {
        case e_comp_space:
            WinSetWindowText(hwnd, "Registers Main CPU");
            return FALSE;
        case e_disk8_space:
            WinSetWindowText(hwnd, "Registers Drive #8");
            return FALSE;
        case e_disk9_space:
            WinSetWindowText(hwnd, "Registers Drive #9");
            return FALSE;
        }
        return FALSE;

    case WM_UPDATE:
        UpdateRegisters(hwnd);
        return FALSE;
    }

    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_mondis(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg)
    {
    case WM_INITDLG:
        WinSetWindowPtr(hwnd, QWL_USER, (VOID*)mp2);
        switch ((MEMSPACE)mp2)
        {
        case e_comp_space:
            WinSetWindowText(hwnd, "Disassembly Main CPU");
            return FALSE;
        case e_disk8_space:
            WinSetWindowText(hwnd, "Disassembly Drive #8");
            return FALSE;
        case e_disk9_space:
            WinSetWindowText(hwnd, "Disassembly Drive #9");
            return FALSE;
        }
        return FALSE;

    case WM_MINMAXFRAME:
    case WM_ADJUSTWINDOWPOS:
        {
            //
            // resize dialog
            //
            SWP *swp=(SWP*)mp1;
            if (!(swp->fl&SWP_SIZE))
                break;

            if (swp->cx<50) swp->cx=300;
            if (swp->cy<50) swp->cy=200;
            WinSetWindowPos(WinWindowFromID(hwnd, LB_MONDIS), 0, 0, 0,
                            swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                            swp->cy-2*WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)
                            -WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)-2,
                            SWP_SIZE);
        }
        break;

    case WM_CONTROL:
        if (MPFROM2SHORT(LB_MONDIS, LN_SELECT)==mp1)
        {
            const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);
            const int  sel  = WinQueryWindowULong(lbox, QWL_USER);

            WinLboxSelectItem(lbox, sel);
        }
        return FALSE;

    case WM_UPDATE:
        UpdateDisassembly(hwnd);
        return FALSE;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_monitor(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int  *wait_for_input;
    static char **input;

    switch (msg)
    {
    case WM_INITDLG:
        {
            HWND hmenu = WinLoadMenu(hwnd, NULLHANDLE, DLG_MONITOR);
            if (hmenu)
                WinSendMsg(hwnd, WM_UPDATEFRAME, MPFROMLONG(FID_MENU), MPVOID);

            trigger_console_exit=FALSE;

            hwndMonreg9 = WinLoadStdDlg(hwnd, pm_monreg, DLG_MONREG, (void*)e_disk9_space);
            hwndMonreg8 = WinLoadStdDlg(hwnd, pm_monreg, DLG_MONREG, (void*)e_disk8_space);
            hwndMonreg  = WinLoadStdDlg(hwnd, pm_monreg, DLG_MONREG, (void*)e_comp_space);
            hwndMondis9 = WinLoadStdDlg(hwnd, pm_mondis, DLG_MONDIS, (void*)e_disk9_space);
            hwndMondis8 = WinLoadStdDlg(hwnd, pm_mondis, DLG_MONDIS, (void*)e_disk8_space);
            hwndMondis  = WinLoadStdDlg(hwnd, pm_mondis, DLG_MONDIS, (void*)e_comp_space);

            WinActivateWindow(hwndMonreg9, FALSE);
            WinActivateWindow(hwndMonreg8, FALSE);
            WinActivateWindow(hwndMondis9, FALSE);
            WinActivateWindow(hwndMondis8, FALSE);
        }
        break;

    case WM_CLOSE:
        if (!wait_for_input)
            break;

        trigger_console_exit=TRUE;
        input=NULL;
        *wait_for_input=FALSE;
        wait_for_input=NULL;
        break;

    case WM_CHAR:
        if (SHORT1FROMMP(mp1)&KC_CHAR)
        {
            char txt[80]="";
            WinQueryDlgText(hwnd, EF_MONIN, txt, 80);
            if (!strlen(txt))
                break;

            if (input)
                *input=stralloc(txt);
            WinSetDlgItemText(hwnd, EF_MONIN,"");
            input=NULL;
            *wait_for_input=FALSE;
            wait_for_input=NULL;
        }
        break;

    case WM_INSERT:
        WinDlgLboxInsertItem(hwnd, LB_MONOUT, (char*)mp1);
        // free(mp1);
        WinDlgLboxSettop(hwnd, LB_MONOUT);
        return FALSE;

    case WM_MENUSELECT:
        switch (SHORT1FROMMP(mp1))
        {
        /* -------- ??????? ----------
        case IDM_REGISTER:
            WinEnableMenuItem(hwnd, IDM_REGDRV8, check_drive_emu_level_ok(8));
            WinEnableMenuItem(hwnd, IDM_REGDRV9, check_drive_emu_level_ok(9));
            break;

        case IDM_DISASSABMBLE:
            WinEnableMenuItem(hwnd, IDM_DISDRV8, check_drive_emu_level_ok(8));
            WinEnableMenuItem(hwnd, IDM_DISDRV9, check_drive_emu_level_ok(9));
            break;
        case IDM_CPUTYPE:
            WinEnableMenuItem(hwnd, IDM_CPU6502, mon_get_cpu_type()==CPU_6502);
            WinEnableMenuItem(hwnd, IDM_CPUZ80,  mon_get_cpu_type()==CPU_Z80);
            break;
            */
        }
        break;

    case WM_COMMAND:
        switch (SHORT1FROMMP(mp1))
        {
        case IDM_REGCPU:
            WinActivateWindow(hwndMonreg, TRUE);
            return FALSE;
        case IDM_REGDRV8:
            WinActivateWindow(hwndMonreg8, TRUE);
            return FALSE;
        case IDM_REGDRV9:
            WinActivateWindow(hwndMonreg9, TRUE);
            return FALSE;

        case IDM_DISCPU:
            WinActivateWindow(hwndMondis, TRUE);
            return FALSE;
        case IDM_DISDRV8:
            WinActivateWindow(hwndMondis8, TRUE);
            return FALSE;
        case IDM_DISDRV9:
            WinActivateWindow(hwndMondis9, TRUE);
            return FALSE;
            /*
        case IDM_CPU6502:
            mon_cpu_type("6502");
            return FALSE;
        case IDM_CPUZ80:
            mon_cpu_type("Z80");
            return FALSE;
            */
        }

        if (!input)
            return FALSE;

        switch (SHORT1FROMMP(mp1))
        {
        case DID_STEPOVER:              // n
            *input=stralloc("next");
            break;
        case DID_STEPINTO:              // z
            *input=stralloc("step");
            break;
        case DID_MONRETURN:             // ret
            *input=stralloc("return");
            break;
        case DID_MONEXIT:               // x
            *input=stralloc("exit");
            break;
        case DID_MONREC:                // rec
            *input=concat("record \"", archdep_boot_path(), "\\vice2.dbg\"", NULL);
            break;
        case DID_MONPLAY:               // pb
            *input=concat("playback \"", archdep_boot_path(), "\\vice2.dbg\"", NULL);
            break;
        case DID_MONSTOP:               // stop
            *input=stralloc("stop");
            break;
        case IDM_SIDEFX:                // sfx
            *input=stralloc("sidefx");
            break;
        default:
            return FALSE;
        }

        input=NULL;
        *wait_for_input=FALSE;
        wait_for_input=NULL;
        return FALSE;

    case WM_MINMAXFRAME:
    case WM_ADJUSTWINDOWPOS:
        {
            SWP *swp=(SWP*)mp1;

            if (!(swp->fl&SWP_SIZE))
                break;

            if (swp->cx<320) swp->cx=320;
            if (swp->cy<200) swp->cy=200;
            WinSetWindowPos(WinWindowFromID(hwnd, LB_MONOUT), 0,
                            WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                            WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)+22,
                            swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                            swp->cy-2*WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME)
                            -WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR)-22-30-20,
                            SWP_SIZE|SWP_MOVE);
            WinSetWindowPos(WinWindowFromID(hwnd, EF_MONIN), 0, 0, 0,
                            swp->cx-2*WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME)-4,
                            16, SWP_SIZE);
        }
        break;

    case WM_PROMPT:
        {
            const HWND lbox = WinWindowFromID(hwnd, LB_MONOUT);

            char *out, tmp[90];
            int pos=WinQueryLboxCount(lbox)-1;

            WinQueryLboxItemText(lbox, pos, tmp, 90);
            WinDeleteLboxItem(lbox, pos);

            out = concat(tmp, mp1, NULL);
            WinLboxInsertItem(lbox, out);
            free(out);

            WinLboxSettopIdx(lbox, pos);
        }
        return FALSE;

    case WM_INPUT:
        input          = mp1;
        wait_for_input = mp2;
        return FALSE;

    case WM_UPDATE:
        WinSendMsg(hwndMonreg,  WM_UPDATE, 0, 0);
        WinSendMsg(hwndMonreg8, WM_UPDATE, 0, 0);
        WinSendMsg(hwndMonreg9, WM_UPDATE, 0, 0);
        WinSendMsg(hwndMondis,  WM_UPDATE, 0, 0);
        WinSendMsg(hwndMondis8, WM_UPDATE, 0, 0);
        WinSendMsg(hwndMondis9, WM_UPDATE, 0, 0);

        WinSetFocus(HWND_DESKTOP, hwnd);
        WinSetDlgFocus(hwnd, EF_MONIN);
        return FALSE;

    case WM_CONSOLE:
        WinShowWindow(hwndMonreg,  (ULONG)mp1);
        WinShowWindow(hwndMonreg8, (ULONG)mp1);
        WinShowWindow(hwndMonreg9, (ULONG)mp1);
        WinShowWindow(hwndMondis,  (ULONG)mp1);
        WinShowWindow(hwndMondis8, (ULONG)mp1);
        WinShowWindow(hwndMondis9, (ULONG)mp1);
        WinActivateWindow(hwndMonitor, (ULONG)mp1);
        return FALSE;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* call to open dialog                                              */
/*----------------------------------------------------------------- */

void MonitorThread(void *arg)
{
    QMSG qmsg; // Msg Queue Event

    HAB hab = WinInitialize(0);            // Initialize PM
    HMQ hmq = WinCreateMsgQueue(hab, 0);   // Create Msg Queue

    HWND focus = WinQueryFocus(HWND_DESKTOP);

    hwndMonitor = WinLoadStdDlg(HWND_DESKTOP, pm_monitor, DLG_MONITOR, NULL);

    WinSetFocus(HWND_DESKTOP, focus);

    //
    // MAINLOOP
    // (don't use WinProcessDlg it ignores the missing WM_VISIBLE flag)
    // returns when a WM_QUIT Msg goes through the queue
    //
    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0))
        WinDispatchMsg(hab, &qmsg);

    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
}

int console_init()
{
    _beginthread(MonitorThread, NULL, 0x4000, NULL);

    // wait for init?

    return 0;
}
