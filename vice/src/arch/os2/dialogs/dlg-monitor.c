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
#define INCL_WINSCROLLBARS   // SBM_*, SB_*

#include "vice.h"

#include <os2.h>

#ifdef WATCOM_COMPILE
#include <process.h>
#endif

#include "dialogs.h"
#include "dlg-monitor.h"

#include <string.h>
#include <stdlib.h>

#include "monitor.h"         // e_*
#include "mon_register.h"    // mon_reg_list_t
#include "mon_disassemble.h" // mon_disassemble_to_string_ex

#include "lib.h"
#include "util.h"
#include "mos6510.h"         // P_*
#include "archdep.h"         // archdep_boot_path
#include "snippets\pmwin2.h" // WinSetDlgFont

#include "log.h"

#define LB_VERTSCROLL 0xc001

HWND hwndMonitor = NULLHANDLE;
HWND hwndMonreg = NULLHANDLE;
HWND hwndMonreg8 = NULLHANDLE;
HWND hwndMonreg9 = NULLHANDLE;
HWND hwndMondis = NULLHANDLE;
HWND hwndMondis8 = NULLHANDLE;
HWND hwndMondis9 = NULLHANDLE;
HWND hwndMonmem = NULLHANDLE;
HWND hwndMonmem8 = NULLHANDLE;
HWND hwndMonmem9 = NULLHANDLE;

int trigger_console_exit;

// --------------------------------------------------------------------------

static const char *mon_dis(MEMSPACE mem, WORD loc, unsigned int *size)
{
    const BYTE op = mon_get_mem_val(mem, loc);
    const BYTE p1 = mon_get_mem_val(mem, loc + 1);
    const BYTE p2 = mon_get_mem_val(mem, loc + 2);

    return mon_disassemble_to_string_ex(mem, loc, loc, op, p1, p2, 1, size);
}


int mon_get_reg_val(int i, int j)
{
    monitor_cpu_type_t cpu;

    mon_register6502_init(&cpu);
    //mon_registerz80_init(&cpu);

    return cpu.mon_register_get_val(i, j);
}

static void UpdateDisassembly(HWND hwnd)
{
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(hwnd, QWL_USER);
    const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);
    ULONG cnt = WinQueryLboxCount(lbox);
    WORD loc = mon_get_reg_val(mem, e_PC);
    unsigned int sel = 0;
    unsigned int size;
    unsigned int i = 0;

    //
    // Try to find the actual adress in the list
    //
    for (i = 0; i < cnt; i++) {
        if ((WinLboxItemHandle(lbox, i) & 0xffff) != loc) {
            continue;
        }

        //
        // if the adress is in the list it has to be selected afterwards
        //
        sel = i;
        break;
    }

    if (i == cnt) {
        //
        // if the adress wasn't found create the list from scratch
        //
        WinLboxEmpty(lbox);
        i = 0;
        cnt = 50;
    } else {
        int pos = i;
        //
        // if the adress exists already look for the next adress
        // which can be apended
        //
        LONG val = WinLboxItemHandle(lbox, cnt - 1);
        loc = (val & 0xffff) + (val >> 16);

        //
        // make sure that we have 50 instructions from
        // the actual one in the list
        //
        i = cnt;
        cnt = pos + 50;
    }

    for (; i < cnt; i++) {
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
        char *buffer = lib_msprintf("%04X: %-30s%s%s%s", loc, p, label ? "[" : "", label ? label : "", label ? "]" : "");
        WinLboxInsertItem(lbox, buffer);
        lib_free(buffer);

        //
        // set the adress and size of the instruction as item handle
        //
        WinLboxSetItemHandle(lbox, i, (size << 16) | loc);

        //
        // step to the next instruction
        //
        loc += size;
    }

    WinSetWindowULong(lbox, QWL_USER, sel);
    WinLboxSelectItem(lbox, sel);
}

static void InsertMemLine(HWND lbox, MEMSPACE mem, WORD *addr, LONG idx)
{
#define MAXX 0x10 // 16
    int x;
    char txt[8 + 3 * MAXX + MAXX / 4];

    *addr &= 0xffff;

    sprintf(txt, "%04X: ", *addr);
    for (x = 0; x < MAXX; x++) {
        if (x%4 == 0) {
            strcat(txt, " ");
        }

        sprintf(txt + strlen(txt), "%02x ", mon_get_mem_val(mem, *addr));
        (*addr)++;
    }
    WinInsertLboxItem(lbox, idx, txt);
    WinLboxSetItemHandle(lbox, idx, *addr - MAXX);
}

static void UpdateMemory(HWND hwnd)
{
    int y = 0;
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(hwnd, QWL_USER);
    const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);
    WORD addr = MAXX * (mon_get_reg_val(mem, e_PC) / MAXX) - 0x10000;

    WinLboxEmpty(lbox);

    while (!WinIsControlEnabled(lbox, LB_VERTSCROLL)) {
        InsertMemLine(lbox, mem, &addr, y++);
    }

    InsertMemLine(lbox, mem, &addr, y);

    WinLboxSettopIdx(lbox, 1);
}

static void UpdateRegisters(HWND hwnd)
{
/* FIXME */
#if 0
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(hwnd, QWL_USER);
    const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);

    monitor_cpu_type_t cpu;
    mon_reg_list_t *list;

    mon_register6502_init(&cpu);

    list = cpu.mon_register_list_get(mem);

    WinLboxEmpty(lbox);
    while (list) {
        char *txt;

        if (list->flags & MON_REGISTER_IS_FLAGS) {
            char str[9] = "00000000";
            char val[33];

            _itoa(list->val, val, 2);

            val[9] = '\0';

            strcpy(str + 8 - strlen(val), val);

            txt = lib_msprintf("%s: %08s", list->name, str);
        } else {
            switch (list->size) {
                case 16:
                    txt = lib_msprintf("%s: 0x%04x", list->name, list->val);
                    break;
                case 8:
                    txt = lib_msprintf("%s: 0x%02x", list->name, list->val);
                    break;
            }

        }
        WinInsertLboxItem(lbox, LIT_END, txt);
        lib_free(txt);

        list = list->next;
    }
#endif
}

// --------------------------------------------------------------------------

static MRESULT EXPENTRY pm_monreg(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch(msg) {
        case WM_INITDLG:
            WinSetWindowPtr(hwnd, QWL_USER, (VOID*)mp2);
            switch ((MEMSPACE)mp2) {
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
    switch (msg) {
        case WM_INITDLG:
            WinSetWindowPtr(hwnd, QWL_USER, (VOID*)mp2);
            switch ((MEMSPACE)mp2) {
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
                SWP *swp = (SWP*)mp1;

                if (!(swp->fl & SWP_SIZE)) {
                    break;
                }

                if (swp->cx < 50) {
                    swp->cx = 300;
                }

                if (swp->cy < 50) {
                    swp->cy = 200;
                }

                WinSetWindowPos(WinWindowFromID(hwnd, LB_MONDIS), 0, 0, 0, swp->cx - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                                swp->cy - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME) - WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) - 2, SWP_SIZE);
            }
            break;
        case WM_CONTROL:
            if (MPFROM2SHORT(LB_MONDIS, LN_SELECT) == mp1) {
                const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);
                const int sel = WinQueryWindowULong(lbox, QWL_USER);

                WinLboxSelectItem(lbox, sel);
            }
            return FALSE;
        case WM_UPDATE:
            UpdateDisassembly(hwnd);
            return FALSE;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

void ScrollUp(HWND hwnd)
{
    const ULONG num = WinQueryLboxCount(hwnd);
    const HWND par = WinQueryWindow(hwnd, QW_PARENT);
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(par, QWL_USER);
    WORD addr = 0x10000 + WinLboxItemHandle(hwnd, 0) - MAXX;

    InsertMemLine(hwnd, mem, &addr, 0);
    WinDeleteLboxItem(hwnd, num);
}

void ScrollDown(HWND hwnd)
{
    const ULONG num = WinQueryLboxCount(hwnd);
    const HWND par = WinQueryWindow(hwnd, QW_PARENT);
    const MEMSPACE mem = (MEMSPACE)WinQueryWindowPtr(par, QWL_USER);
    WORD addr = WinLboxItemHandle(hwnd, num - 1) + MAXX;

    InsertMemLine(hwnd, mem, &addr, num);
    WinDeleteLboxItem(hwnd, 0);
}

static MRESULT EXPENTRY pm_sbar(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    PFNWP pfnwp = (PFNWP)WinQueryWindowPtr(hwnd, QWL_USER);

    if (msg != WM_VSCROLL) {
        return (*pfnwp)(hwnd, msg, mp1, mp2);
    }

    switch (SHORT2FROMMP(mp2)) {
        case SB_LINEUP:
            ScrollUp(hwnd);
            return FALSE;
        case SB_LINEDOWN:
            ScrollDown(hwnd);
            return FALSE;
        case SB_SLIDERTRACK:
            switch (SHORT1FROMMP(mp2)) {
                case 0:
                    ScrollUp(hwnd);
                    return FALSE;
                case 2:
                    ScrollDown(hwnd);
                    return FALSE;
            }
            return FALSE;
        case SB_PAGEUP:
        case SB_PAGEDOWN:
        case SB_SLIDERPOSITION:
            log_debug("test pos:%d cmd:%d", SHORT1FROMMP(mp2), SHORT2FROMMP(mp2));
            return FALSE;
        case SB_ENDSCROLL:
            WinLboxSettopIdx(hwnd, 1);
            return FALSE;
    }
    return (*pfnwp)(hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_monmem(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
        case WM_INITDLG:
            {
                const HWND lbox = WinWindowFromID(hwnd, LB_MONDIS);
                const VOID *ptr = WinQueryWindowPtr(lbox, QWP_PFNWP);

                WinSetWindowULong(lbox, QWL_USER, (ULONG)ptr);
                WinSubclassWindow(lbox, (PFNWP)pm_sbar);
            }

            WinSetWindowPtr(hwnd, QWL_USER, (VOID*)mp2);
            switch ((MEMSPACE)mp2) {
                case e_comp_space:
                    WinSetWindowText(hwnd, "Memory Main CPU");
                    return FALSE;
                case e_disk8_space:
                    WinSetWindowText(hwnd, "Memory Drive #8");
                    return FALSE;
                case e_disk9_space:
                    WinSetWindowText(hwnd, "Memory Drive #9");
                    return FALSE;
            }
            return FALSE;
        case WM_MINMAXFRAME:
        case WM_ADJUSTWINDOWPOS:
            {
                //
                // resize dialog
                //
                SWP *swp = (SWP*)mp1;
                if (!(swp->fl & SWP_SIZE)) {
                    break;
                }

                if (swp->cx < 50) {
                    swp->cx = 300;
                }

                if (swp->cy < 50) {
                    swp->cy = 200;
                }

                WinSetWindowPos(WinWindowFromID(hwnd, LB_MONDIS), 0, 0, 0, swp->cx - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME),
                                swp->cy - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME) - WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) - 2, SWP_SIZE);
            }
            break;
        case WM_UPDATE:
            UpdateMemory(hwnd);
            return FALSE;
    }
    return WinDefDlgProc (hwnd, msg, mp1, mp2);
}

static MRESULT EXPENTRY pm_monitor(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static int *wait_for_input;
    static char **input;

    switch (msg) {
        case WM_INITDLG:
            {
                HWND hmenu = WinLoadMenu(hwnd, NULLHANDLE, DLG_MONITOR);

                if (hmenu) {
                    WinSendMsg(hwnd, WM_UPDATEFRAME, MPFROMLONG(FID_MENU), MPVOID);
                }

                trigger_console_exit = FALSE;

                hwndMonreg9 = WinLoadStdDlg(hwnd, pm_monreg, DLG_MONREG, (void*)e_disk9_space);
                hwndMonreg8 = WinLoadStdDlg(hwnd, pm_monreg, DLG_MONREG, (void*)e_disk8_space);
                hwndMonreg = WinLoadStdDlg(hwnd, pm_monreg, DLG_MONREG, (void*)e_comp_space);
                hwndMondis9 = WinLoadStdDlg(hwnd, pm_mondis, DLG_MONDIS, (void*)e_disk9_space);
                hwndMondis8 = WinLoadStdDlg(hwnd, pm_mondis, DLG_MONDIS, (void*)e_disk8_space);
                hwndMondis = WinLoadStdDlg(hwnd, pm_mondis, DLG_MONDIS, (void*)e_comp_space);
                hwndMonmem9 = WinLoadStdDlg(hwnd, pm_monmem, DLG_MONDIS, (void*)e_disk9_space);
                hwndMonmem8 = WinLoadStdDlg(hwnd, pm_monmem, DLG_MONDIS, (void*)e_disk8_space);
                hwndMonmem = WinLoadStdDlg(hwnd, pm_monmem, DLG_MONDIS, (void*)e_comp_space);

                WinActivateWindow(hwndMonreg9, FALSE);
                WinActivateWindow(hwndMonreg8, FALSE);
                WinActivateWindow(hwndMondis9, FALSE);
                WinActivateWindow(hwndMondis8, FALSE);
                WinActivateWindow(hwndMonmem9, FALSE);
                WinActivateWindow(hwndMonmem8, FALSE);
            }
            break;
        case WM_CLOSE:
            if (!wait_for_input) {
                break;
            }

            trigger_console_exit = TRUE;
            input = NULL;
            *wait_for_input = FALSE;
            wait_for_input = NULL;
            break;
        case WM_CHAR:
            if (SHORT1FROMMP(mp1) & KC_CHAR) {
                char txt[80] = "";

                WinQueryDlgText(hwnd, EF_MONIN, txt, 80);
                if (!strlen(txt)) {
                    break;
                }

                if (input) {
                    *input = lib_stralloc(txt);
                }
                WinSetDlgItemText(hwnd, EF_MONIN,"");
                input = NULL;
                *wait_for_input = FALSE;
                wait_for_input = NULL;
            }
            break;
        case WM_INSERT:
            WinDlgLboxInsertItem(hwnd, LB_MONOUT, (char*)mp1);
            WinDlgLboxSettop(hwnd, LB_MONOUT);
            return FALSE;
        case WM_MENUSELECT:
            break;
        case WM_COMMAND:
            switch (SHORT1FROMMP(mp1)) {
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
                case IDM_MEMCPU:
                    WinActivateWindow(hwndMonmem, TRUE);
                    return FALSE;
                case IDM_MEMDRV8:
                    WinActivateWindow(hwndMonmem8, TRUE);
                    return FALSE;
                case IDM_MEMDRV9:
                    WinActivateWindow(hwndMonmem9, TRUE);
                    return FALSE;
            }

            if (!input) {
                return FALSE;
            }

            switch (SHORT1FROMMP(mp1)) {
                case DID_STEPOVER:              // n
                    *input = lib_stralloc("next");
                    break;
                case DID_STEPINTO:              // z
                    *input = lib_stralloc("step");
                    break;
                case DID_MONRETURN:             // ret
                    *input = lib_stralloc("return");
                    break;
                case DID_MONEXIT:               // x
                    *input = lib_stralloc("exit");
                    break;
                case DID_MONREC:                // rec
                    *input = util_concat("record \"", archdep_boot_path(), "\\vice2.dbg\"", NULL);
                    break;
                case DID_MONPLAY:               // pb
                    *input = util_concat("playback \"", archdep_boot_path(), "\\vice2.dbg\"", NULL);
                    break;
                case DID_MONSTOP:               // stop
                    *input = lib_stralloc("stop");
                    break;
                case IDM_SIDEFX:                // sfx
                    *input = lib_stralloc("sidefx"); 
                    break;
                default:
                    return FALSE;
            }

            input = NULL;
            *wait_for_input = FALSE;
            wait_for_input = NULL;
            return FALSE;
        case WM_MINMAXFRAME:
        case WM_ADJUSTWINDOWPOS:
            {
                SWP *swp = (SWP*)mp1;

                if (!(swp->fl & SWP_SIZE)) {
                    break;
                }

                if (swp->cx < 320) {
                    swp->cx = 320;
                }

                if (swp->cy < 200) {
                    swp->cy = 200;
                }

                WinSetWindowPos(WinWindowFromID(hwnd, LB_MONOUT), 0, WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME), WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME) + 22,
                                swp->cx - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME), swp->cy - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CYDLGFRAME) -
                                WinQuerySysValue(HWND_DESKTOP, SV_CYTITLEBAR) - 22 - 30 - 20, SWP_SIZE | SWP_MOVE);
                WinSetWindowPos(WinWindowFromID(hwnd, EF_MONIN), 0, 0, 0, swp->cx - 2 * WinQuerySysValue(HWND_DESKTOP, SV_CXDLGFRAME) - 4, 16, SWP_SIZE);
            }
            break;
        case WM_PROMPT:
            {
                const HWND lbox = WinWindowFromID(hwnd, LB_MONOUT);
                char *out, tmp[90];
                int pos = WinQueryLboxCount(lbox) - 1;

                WinQueryLboxItemText(lbox, pos, tmp, 90);
                WinDeleteLboxItem(lbox, pos);

                out = util_concat(tmp, mp1, NULL);
                WinLboxInsertItem(lbox, out);
                lib_free(out);

                WinLboxSettopIdx(lbox, pos);
            }
            return FALSE;
        case WM_INPUT:
            input = mp1;
            wait_for_input = mp2;
            return FALSE;
        case WM_UPDATE:
            WinSendMsg(hwndMonreg, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMonreg8, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMonreg9, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMondis, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMondis8, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMondis9, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMonmem, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMonmem8, WM_UPDATE, 0, 0);
            WinSendMsg(hwndMonmem9, WM_UPDATE, 0, 0);

            WinSetFocus(HWND_DESKTOP, hwnd);
            WinSetDlgFocus(hwnd, EF_MONIN);
            return FALSE;
        case WM_CONSOLE:
            WinShowWindow(hwndMonreg, (ULONG)mp1);
            WinShowWindow(hwndMonreg8, (ULONG)mp1);
            WinShowWindow(hwndMonreg9, (ULONG)mp1);
            WinShowWindow(hwndMondis, (ULONG)mp1);
            WinShowWindow(hwndMondis8, (ULONG)mp1);
            WinShowWindow(hwndMondis9, (ULONG)mp1);
            WinShowWindow(hwndMonmem, (ULONG)mp1);
            WinShowWindow(hwndMonmem8, (ULONG)mp1);
            WinShowWindow(hwndMonmem9, (ULONG)mp1);
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
    while (WinGetMsg(hab, &qmsg, NULLHANDLE, 0, 0)) {
        WinDispatchMsg(hab, &qmsg);
    }

    WinDestroyMsgQueue(hmq);
    WinTerminate(hab);
}

int console_init()
{
    _beginthread(MonitorThread, NULL, 0x4000, NULL);

    // wait for init?

    return 0;
}
