/*
 * uimon.c - Monitor access interface.
 *
 * Written by
 *  Spiro Trikaliotis <Spiro.Trikaliotis@gmx.de>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>

#include "console.h"

#include "fullscreen.h"
#include "mon.h"
#include "res.h"
#include "ui.h"
#include "uimon.h"

#include "utils.h"
#include "winmain.h"

#define UIMON_EXPERIMENTAL 1

#ifdef UIMON_EXPERIMENTAL

static HWND hwndConsole   = NULL;
static HWND hwndMdiClient = NULL;
static HWND hwndToolbar = NULL;

static console_t *console_log = NULL;
static console_t  console_log_for_mon;
static char *pRetValue        = NULL;

#define REG_CLASS MONITOR_CLASS ":Reg"
#define DIS_CLASS MONITOR_CLASS ":Dis"

static HWND hwndParent = NULL;

static HWND hwndMdiActive = NULL;


#define WM_OWNCOMMAND (WM_USER+0x100)

/*
 The following definitions (RT_TOOLBAR, CToolBarData) are from the MFC sources
*/
#define RT_TOOLBAR MAKEINTRESOURCE(2410)

typedef struct CToolBarData
{
	WORD wVersion;
	WORD wWidth;
	WORD wHeight;
	WORD wItemCount;
	WORD aItems[1];
} CToolBarData;


static
HWND CreateAToolbar( HWND hwnd )
{
    HMODULE       hMod     = NULL;
    HRSRC         hRes     = NULL;
    HGLOBAL       hGlobal  = NULL;
    HWND          hToolbar = NULL;
    CToolBarData* pData    = NULL;
    int           i;
    int           j;

    TBBUTTON *ptbb = NULL;

    hRes = FindResource( winmain_instance, MAKEINTRESOURCE(IDR_MONTOOLBAR), RT_TOOLBAR );

    if (hRes == NULL)
        goto quit;

    hGlobal = LoadResource( winmain_instance, hRes );
    if (hGlobal == NULL)
        goto quit;

    pData = (CToolBarData*)LockResource(hGlobal);
    if (pData == NULL)
        goto unlock;

    if (pData->wVersion != 1)
        goto unlock;

    ptbb = (PTBBUTTON) xmalloc(pData->wItemCount*sizeof(TBBUTTON));
    if (!ptbb)
        goto unlock;

    for (i = j = 0; i < pData->wItemCount; i++)
    {
        if (pData->aItems[i])
        {
            ptbb[i].iBitmap = j++;
            ptbb[i].fsStyle = TBSTYLE_BUTTON;
        }
        else
        {
            ptbb[i].iBitmap = 5;
            ptbb[i].fsStyle = TBSTYLE_SEP;
        }

        ptbb[i].idCommand = pData->aItems[i];

        ptbb[i].fsState = TBSTATE_ENABLED;
        ptbb[i].dwData  = 0;
        ptbb[i].iString = j;
    }

    hToolbar = CreateToolbarEx( hwnd,
        WS_CHILD, // WORD ws, 
        3, // UINT wID, 
        j, // int nBitmaps, 
        winmain_instance, // HINSTANCE hBMInst, 
        IDR_MONTOOLBAR, // UINT wBMID, 
        ptbb, // LPCTBBUTTON lpButtons, 
        pData->wItemCount, // int iNumButtons, 
        pData->wWidth, // int dxButton, 
        pData->wHeight, // int dyButton, 
        pData->wWidth, // int dxBitmap, 
        pData->wHeight, // int dyBitmap, 
        sizeof(TBBUTTON) // UINT uStructSize 
        );

    if (hToolbar)
        ShowWindow(hToolbar,SW_SHOW);

    free(ptbb);
    
unlock:
#ifdef HAS_UNLOCKRESOURCE
    UnlockResource(hGlobal);
#endif
    FreeResource(hGlobal);
quit:
    return hToolbar;
}

static
HWND OpenDisassembly( HWND hwnd )
{
    return CreateMDIWindow(DIS_CLASS,
        "Disassembly",
        0,
        0, // CW_USEDEFAULT, // 20,
        0, // CW_USEDEFAULT, // 60,
        500, // CW_USEDEFAULT, // 600,
        500, // CW_USEDEFAULT, // 340,
        hwndMdiClient,
        winmain_instance,
        0);
}

static
void OpenConsole( HWND hwnd, BOOLEAN bOpen )
{
    if (bOpen)
    {
        console_log = arch_console_open_mdi("Monitor",
            &hwndConsole,&hwndParent,&hwndMdiClient);

		/* TODO: [SRT] this is just a temporary fix
		  without this, the console window cannot be accessed almost always on first 
		  opening if the monitor. Any hints why?
		*/
        DestroyWindow( OpenDisassembly(hwnd) );
    }
    else
    {
        DestroyWindow( hwndConsole );
        hwndConsole = NULL;
    }
}

static
HWND OpenRegistry( HWND hwnd )
{
#if 0
    #define DEF_REG_PROG DefWindowProc
    HWND hwndReg = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        REG_CLASS,
        "Register",
        WS_CAPTION|WS_POPUPWINDOW,
        30,
        100,
        0,
        0,
        hwnd,
        NULL,
        winmain_instance,
        NULL);

    ShowWindow( hwndReg, SW_SHOW );
    return hwndReg;
#else
    #define DEF_REG_PROG DefMDIChildProc
    return CreateMDIWindow(REG_CLASS,
        "Register",
        0,
        0,
        0,
        0,
        0,
        hwndMdiClient,
        winmain_instance,
        0);
#endif
}

static
void EnableCommands( HMENU hmnu, HWND hwndToolbar )
{
#define ENABLE( xID, xENABLE) \
    EnableMenuItem( hmnu, (xID), (xENABLE) ? MF_ENABLED : MF_GRAYED ); \
    SendMessage( hwndToolbar, TB_ENABLEBUTTON, (xID), MAKELONG((xENABLE),0) )

#define CHECK( xID, xON) \
    CheckMenuItem( hmnu, (xID), (xON) ? MF_CHECKED : MF_UNCHECKED ); \
    SendMessage( hwndToolbar, TB_CHECKBUTTON, (xID), MAKELONG((xON),0) )

    ENABLE( IDM_MON_OPEN         , 0 );
    ENABLE( IDM_MON_SAVE         , 0 );
    ENABLE( IDM_MON_PRINT        , 0 );
//  ENABLE( IDM_MON_STOP_DEBUG   , 0 );
    ENABLE( IDM_MON_STOP_EXEC    , 0 );
    ENABLE( IDM_MON_CURRENT      , 0 );
//  ENABLE( IDM_MON_STEP_INTO    , 1 );
//  ENABLE( IDM_MON_STEP_OVER    , 1 );
//  ENABLE( IDM_MON_SKIP_RETURN  , 1 );
    ENABLE( IDM_MON_GOTO_CURSOR  , 0 );
    ENABLE( IDM_MON_EVAL         , 0 );
    ENABLE( IDM_MON_WND_EVAL     , 0 );
//  CHECK ( IDM_MON_WND_REG      , hwndReg     ? TRUE : FALSE );
    ENABLE( IDM_MON_WND_MEM      , 0 );
//  CHECK ( IDM_MON_WND_DIS      , hwndDis     ? TRUE : FALSE );
    CHECK ( IDM_MON_WND_CONSOLE  , hwndConsole ? TRUE : FALSE );
    ENABLE( IDM_MON_HELP         , 0 );
//  ENABLE( IDM_MON_CASCADE      , 1 );
//  ENABLE( IDM_MON_TILE_HORIZ   , 1 );
//  ENABLE( IDM_MON_TILE_VERT    , 1 );
//  ENABLE( IDM_MON_ARRANGE_ICONS, 1 );
}

#define SET_COMMAND( _cmd ) \
    pRetValue = _cmd; \
    if (console_log) \
        console_out(console_log,"%s\n",_cmd); \
    if (hwndConsole) \
        SendMessage(hwndConsole,WM_CONSOLE_INSERTLINE,0,0 )

static
void ResizeMdiClient(HWND hwnd)
{
    RECT rect;

    if (hwndMdiClient)
    {
        WORD wHeightToolbar = 0;

        if (hwndToolbar)
        {
            GetWindowRect(hwndToolbar, &rect);
            wHeightToolbar = (WORD) (rect.bottom-rect.top);
        }

        GetClientRect(hwnd, &rect);

        MoveWindow(hwndMdiClient,
            rect.left,
            rect.top+wHeightToolbar,
            rect.right-rect.left,
            rect.bottom-rect.top-wHeightToolbar,
            TRUE);
    }
}

static void ResizeToolbar(HWND hwnd)
{
    RECT rect;

    if (hwndToolbar)
    {
        WORD wHeightToolbar;

        GetWindowRect(hwndToolbar, &rect);
        wHeightToolbar = (WORD) (rect.bottom-rect.top);

        GetClientRect(hwnd, &rect);

        MoveWindow(hwndToolbar,
            rect.left,
            rect.top,
            rect.right-rect.left,
            wHeightToolbar,
            TRUE);
    }
}

void OnConsoleResize(void)
{
    if (console_log)
        memcpy( &console_log_for_mon, console_log, sizeof( struct console_s ) );
}

void OnCommand( HWND hwnd, WORD wNotifyCode, WORD wID, HWND hwndCtrl )
{
    switch (wID)
    {
    case IDM_MON_CASCADE:
        SendMessage( hwndMdiClient, WM_MDICASCADE, 0, 0 );
        break;

    case IDM_MON_TILE_HORIZ:
        SendMessage( hwndMdiClient, WM_MDITILE, MDITILE_HORIZONTAL, 0 );
        break;

    case IDM_MON_TILE_VERT:
        SendMessage( hwndMdiClient, WM_MDITILE, MDITILE_VERTICAL, 0 );
        break;

    case IDM_MON_ARRANGE_ICONS:
        SendMessage( hwndMdiClient, WM_MDIICONARRANGE, 0, 0 );
        break;

    case IDM_EXIT:
        /* FALL THROUGH */

    case IDM_MON_STOP_DEBUG:
        SET_COMMAND("x");
        break;

    case IDM_MON_STEP_INTO:
        SET_COMMAND("z");
        break;

    case IDM_MON_STEP_OVER:
        SET_COMMAND("n");
        break;

    case IDM_MON_SKIP_RETURN:
        SET_COMMAND("ret");
        break;

    case IDM_MON_WND_DIS:
        OpenDisassembly(hwnd);
        break;

    case IDM_MON_WND_REG:
        OpenRegistry(hwnd);
        break;

    case IDM_MON_WND_CONSOLE:
        OpenConsole(hwnd,(BOOLEAN)(hwndConsole?FALSE:TRUE));
        EnableCommands(GetMenu(hwnd),hwndToolbar);
        break;

	case IDM_MON_COMPUTER:
		/* FALL THROUGH */

	case IDM_MON_DRIVE8:
		/* FALL THROUGH */

	case IDM_MON_DRIVE9:
		if (hwndMdiActive)
			SendMessage( hwndMdiActive, WM_OWNCOMMAND, wID, 0 );
		break;
    }
}


/* window procedure */
static long CALLBACK mon_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	switch (msg)
	{
	case WM_CLOSE:
        SET_COMMAND("x");
        /* FALL THROUGH */

	case WM_DESTROY:
        return DefFrameProc(hwnd, hwndMdiClient, msg, wParam, lParam);

    case WM_CONSOLE_RESIZED:
        OnConsoleResize();
        return 0;

    case WM_CONSOLE_CLOSED:
        console_log = NULL;
        hwndConsole = NULL;
        EnableCommands(GetMenu(hwnd),hwndToolbar);
        return 0;

    case WM_SIZE:
        {
    	    if (wParam != SIZE_MINIMIZED)
            {
                ResizeToolbar(hwnd);
                ResizeMdiClient(hwnd);
            }
        }
        /* do not call default - it will reposition the MDICLIENT */
        return 0;


    case WM_CREATE:
        hwndToolbar = CreateAToolbar(hwnd);

        EnableCommands(GetMenu(hwnd),hwndToolbar);

        {
            CLIENTCREATESTRUCT ccs;
            ccs.hWindowMenu  = GetSubMenu(GetMenu(hwnd),3);
            ccs.idFirstChild = IDM_WNDCHILD;
            hwndMdiClient = CreateWindow(
                "MdiClient",NULL,WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VSCROLL|WS_HSCROLL,
                0,0,0,0,
                hwnd,(HMENU)0xCAC,winmain_instance,(LPSTR)&ccs);

            ResizeMdiClient(hwnd);
            ShowWindow( hwndMdiClient, SW_SHOW );
        }
        break;

    case WM_COMMAND:
        OnCommand(hwnd,HIWORD(wParam),LOWORD(wParam),(HWND)lParam);
        break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);

			return 0;
		}
	}

	return DefFrameProc(hwnd, hwndMdiClient, msg, wParam, lParam);
}

static
void SetMemspace( HWND hwnd, enum t_memspace memspace )
{
	BOOL bComputer = FALSE;
	BOOL bDrive8   = FALSE;
	BOOL bDrive9   = FALSE;
	HMENU hmnu = GetMenu(hwnd);

	switch (memspace)
	{
	case e_comp_space:  bComputer = TRUE; break;
	case e_disk8_space: bDrive8   = TRUE; break;
	case e_disk9_space: bDrive9   = TRUE; break;
	}

	ENABLE( IDM_MON_COMPUTER, 1 );
	ENABLE( IDM_MON_DRIVE8,   1 );
    ENABLE( IDM_MON_DRIVE9,   1 );
	CHECK ( IDM_MON_COMPUTER, bComputer );
	CHECK ( IDM_MON_DRIVE8,   bDrive8   );
    CHECK ( IDM_MON_DRIVE9,   bDrive9   );
}

static
void ClearMemspace( HWND hwnd )
{
	HMENU hmnu = GetMenu(hwnd);

	ENABLE( IDM_MON_COMPUTER, 0 );
	ENABLE( IDM_MON_DRIVE8,   0 );
    ENABLE( IDM_MON_DRIVE9,   0 );
	CHECK ( IDM_MON_COMPUTER, FALSE );
	CHECK ( IDM_MON_DRIVE8,   FALSE );
    CHECK ( IDM_MON_DRIVE9,   FALSE );
}

static
void ActivateMdiChild( HWND hwndDeactivated, HWND hwndOwn, enum t_memspace memspace )
{
	if (hwndDeactivated == hwndOwn)
	{
		// we are deactivated
		hwndMdiActive = NULL;
        ClearMemspace( hwndOwn );
	}
	else
	{
		// we are activated
		hwndMdiActive = hwndOwn;
        SetMemspace( hwndOwn, memspace );
	}
}

typedef
struct reg_private
{
    int charwidth;
    int charheight;
	enum t_memspace memspace;
	int LastShownRegs[16];
} reg_private_t;

/* window procedure */
static long CALLBACK reg_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	reg_private_t *prp = (reg_private_t*) GetWindowLong( hwnd, GWL_USERDATA );

	switch (msg)
	{
//	case WM_CLOSE:
		/* FALL THROUGH */

	case WM_DESTROY:
		// clear the dis_private info 
		free(prp);
		SetWindowLong( hwnd, GWL_USERDATA, 0 );

	    return DEF_REG_PROG(hwnd, msg, wParam, lParam);

	case WM_MDIACTIVATE:
		ActivateMdiChild( (HWND) wParam, hwnd, prp->memspace );
		break;

	case WM_OWNCOMMAND:
		switch (wParam)
		{
		case IDM_MON_COMPUTER:
			prp->memspace = e_comp_space;
			break;

		case IDM_MON_DRIVE8:
			prp->memspace = e_disk8_space;
			break;

		case IDM_MON_DRIVE9:
			prp->memspace = e_disk9_space;
			break;
		}
        SetMemspace( hwnd, prp->memspace );
		InvalidateRect(hwnd,NULL,FALSE);
		break;

    case WM_CREATE:
        {
            HDC hdc = GetDC( hwnd );
           	SIZE size;
            RECT rect;

			prp = xmalloc(sizeof(reg_private_t));
			
			/* store pointer to structure with window */
			SetWindowLong( hwnd, GWL_USERDATA, (long) prp );

	        SelectObject( hdc, GetStockObject( ANSI_FIXED_FONT ) );

            // get height and width of a character
            GetTextExtentPoint32( hdc, " ", 1, &size );
        	prp->charwidth  = size.cx;
	        prp->charheight = size.cy;

			prp->memspace = e_comp_space;

            // resize window
            GetWindowRect( hwnd, &rect );
            rect.right  = rect.left + prp->charwidth*34;
            rect.bottom = rect.top  + prp->charheight*2;
            AdjustWindowRectEx( &rect, 
                GetWindowLong( hwnd, GWL_STYLE ), FALSE, GetWindowLong( hwnd, GWL_EXSTYLE ) );

        	MoveWindow( hwnd, rect.left, rect.top,
		        rect.right - rect.left, rect.bottom - rect.top, TRUE );
            break;
        }

	case WM_COMMAND:
		{
		    switch (HIWORD(wParam))
			{
			case IDM_MON_COMPUTER:
				prp->memspace = e_comp_space;
				break;

			case IDM_MON_DRIVE8:
				prp->memspace = e_disk8_space;
				break;

			case IDM_MON_DRIVE9:
				prp->memspace = e_disk9_space;
				break;
		    }
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
            char buffer[5];
            int lastRegValue;

			hdc = BeginPaint(hwnd,&ps);

            SetTextColor(hdc,RGB(0,0,0));
            TextOut( hdc, 0, 0,        " PC  FL NV-BDIZC AC XR YR SP 00 01", 34 );

#define DO_OUT(_bCond,_x,_val,_len) \
    SetTextColor(hdc,RGB((_bCond)?0xFF:0,0,0)); \
    TextOut( hdc,  (_x)*prp->charwidth, 0+prp->charheight, _val, _len )

#define DO_OUT_REG(_no,_x,_reg,_len) \
    lastRegValue = mon_get_reg_val(prp->memspace,_reg); \
    sprintf(buffer,"%0" #_len "X", lastRegValue ); \
    DO_OUT(lastRegValue!=prp->LastShownRegs[_no],_x,buffer,_len); \
    prp->LastShownRegs[_no]=lastRegValue

#define DO_OUT_FLG(_no,_x,_val) \
    DO_OUT((lastRegValue&_val)!=prp->LastShownRegs[_no],_x,(lastRegValue & _val)?"1":"0",1); \
    prp->LastShownRegs[_no]=lastRegValue & _val

#define DO_OUT_MEM(_no,_x,_addr) \
    lastRegValue = mon_get_mem_val(prp->memspace,_addr); \
    sprintf(buffer,"%02X", lastRegValue ); \
    DO_OUT(lastRegValue!=prp->LastShownRegs[_no],_x,buffer,2); \
    prp->LastShownRegs[_no]=lastRegValue

            DO_OUT_REG(0,  0, e_PC, 4 );

            DO_OUT_REG(1,  5, e_FLAGS,  2 );
            DO_OUT_FLG(2,  8, 0x80 );
            DO_OUT_FLG(3,  9, 0x40 );
            DO_OUT_FLG(4, 10, 0x20 );
            DO_OUT_FLG(5, 11, 0x10 );
            DO_OUT_FLG(6, 12, 0x08 );
            DO_OUT_FLG(7, 13, 0x04 );
            DO_OUT_FLG(8, 14, 0x02 );
            DO_OUT_FLG(9, 15, 0x01 );

            DO_OUT_REG(10, 17, e_A,  2 );
            DO_OUT_REG(11, 20, e_X,  2 );
            DO_OUT_REG(12, 23, e_Y,  2 );
            DO_OUT_REG(13, 26, e_SP, 2 );

            DO_OUT_MEM(14, 29, 0x00 );
            DO_OUT_MEM(15, 32, 0x01 );

#undef DO_OUT
#undef DO_OUT_REG
#undef DO_OUT_FLG
#undef DO_OUT_MEM

			EndPaint(hwnd,&ps);

			return 0;
		}
	}

    return DEF_REG_PROG(hwnd, msg, wParam, lParam);
}


typedef
struct dis_private
{
    int charwidth;
    int charheight;
	enum t_memspace memspace;
    UINT StartAddress;
    UINT EndAddress;

} dis_private_t;

/* window procedure */
static long CALLBACK dis_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	dis_private_t *pdp = (dis_private_t*) GetWindowLong( hwnd, GWL_USERDATA );

	switch (msg)
	{
	case WM_DESTROY:
		// clear the dis_private info 
		free(pdp);
		SetWindowLong( hwnd, GWL_USERDATA, 0 );

		return DefMDIChildProc(hwnd, msg, wParam, lParam);

	case WM_MDIACTIVATE:
		ActivateMdiChild( (HWND) wParam, hwnd, pdp->memspace );
		break;

	case WM_OWNCOMMAND:
		switch (wParam)
		{
		case IDM_MON_COMPUTER:
			pdp->memspace = e_comp_space;
			break;

		case IDM_MON_DRIVE8:
			pdp->memspace = e_disk8_space;
			break;

		case IDM_MON_DRIVE9:
			pdp->memspace = e_disk9_space;
			break;
		}
        SetMemspace( hwnd, pdp->memspace );
		InvalidateRect(hwnd,NULL,FALSE);
		break;

    case WM_CREATE:
        {
            HDC hdc = GetDC( hwnd );
           	SIZE size;

			pdp = xmalloc(sizeof(dis_private_t));
			
			/* store pointer to structure with window */
			SetWindowLong( hwnd, GWL_USERDATA, (long) pdp );

	        SelectObject( hdc, GetStockObject( ANSI_FIXED_FONT ) );

            // get height and width of a character
            GetTextExtentPoint32( hdc, " ", 1, &size );
        	pdp->charwidth    = size.cx;
	        pdp->charheight   = size.cy;

			// initialize some window parameter
			pdp->memspace     = e_comp_space;
			pdp->StartAddress = -1;
			pdp->EndAddress   = 0;
            break;
        }

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
            RECT rect;
            unsigned int uAddress = mon_get_reg_val(pdp->memspace,e_PC);
            unsigned int loc;
            unsigned int size;
            char buffer[512];
            const char *p;
            char *label;
            int  i;
            int  nHeightToPrint, nHeightToNextPage;

            if ((uAddress < pdp->StartAddress) || (uAddress > pdp->EndAddress))
            {
                pdp->StartAddress = uAddress;
                pdp->EndAddress = 0;
            }

            loc = pdp->StartAddress;

            // get the height to paint
            GetClientRect(hwnd,&rect);
            nHeightToPrint    = rect.bottom - rect.top;
            nHeightToNextPage = nHeightToPrint * 2 / 3;

			hdc = BeginPaint(hwnd,&ps);


#define DO_OUT(_y,_addr) \
    SetTextColor(hdc,RGB((_addr)==uAddress?0xFF:0,0,0)); \
    TextOut( hdc,  0, _y*pdp->charheight, buffer, strlen(buffer) )

            for (i=0; i*pdp->charheight<nHeightToPrint; i++)
            {
               if (i*pdp->charheight < nHeightToNextPage)
                   pdp->EndAddress = loc;

               label = mon_symbol_table_lookup_name(pdp->memspace, loc);
               if (label)
               {
                  sprintf( buffer, "%s:",label);
                  DO_OUT(i++,loc);
               }

               p = mon_disassemble_to_string_ex( loc,
                       mon_get_mem_val(pdp->memspace,loc),
                       mon_get_mem_val(pdp->memspace,loc+1),
                       mon_get_mem_val(pdp->memspace,loc+2),
                       1,
                       &size );

               sprintf( buffer, "%04X: %s%10s",
                   loc,
                   p,
                   "");

               DO_OUT( i, loc );

               loc += size;
            }
#undef DO_OUT

			EndPaint(hwnd,&ps);

			return 0;
		}
	}

	return DefMDIChildProc(hwnd, msg, wParam, lParam);
}


static void arch_mon_init( void )
{
    static BOOLEAN bFirstTime = TRUE;

    if (bFirstTime)
    {
    	WNDCLASSEX wc;

        bFirstTime = FALSE;

        /* Register window class for the monitor window */
        wc.cbSize        = sizeof(WNDCLASSEX);
        wc.style         = CS_CLASSDC;
        wc.lpfnWndProc   = mon_window_proc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = winmain_instance;
        wc.hIcon         = LoadIcon(winmain_instance, MAKEINTRESOURCE(IDI_ICON1));
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0xc0, 0xc0, 0xc0));
        wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MENUMONITOR);
        wc.lpszClassName = MONITOR_CLASS;
        wc.hIconSm       = NULL;

        RegisterClassEx(&wc);

        /* Register window class for the disassembler window */
        wc.lpfnWndProc   = dis_window_proc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
        wc.hbrBackground = CreateSolidBrush(RGB(0xFF,0xFF,0xFF));
        wc.lpszMenuName  = 0;
        wc.lpszClassName = DIS_CLASS;
        wc.hIconSm       = NULL;

        RegisterClassEx(&wc);

        /* Register window class for the register window */
        wc.lpfnWndProc   = reg_window_proc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
        wc.hbrBackground = CreateSolidBrush(RGB(0xFF,0xFF,0xFF));
        wc.lpszMenuName  = 0;
        wc.lpszClassName = REG_CLASS;
        wc.hIconSm       = NULL;

        RegisterClassEx(&wc);
    }
}

static HWND hwnd = NULL;

#endif // #ifdef UIMON_EXPERIMENTAL


void arch_mon_window_close( void )
{
#ifdef UIMON_EXPERIMENTAL

	DestroyWindow(hwnd);
	hwnd          =
    hwndMdiClient = NULL;

	ResumeFullscreenMode( hwndParent );

#else // #ifdef UIMON_EXPERIMENTAL
    console_close(console_log);

#endif // #ifdef UIMON_EXPERIMENTAL

    console_log = NULL;
}


console_t *arch_mon_window_open( void )
{

#ifdef UIMON_EXPERIMENTAL

    hwndParent = GetActiveWindow();

	SuspendFullscreenMode( hwndParent );

    arch_mon_init();

    hwnd = CreateWindow(MONITOR_CLASS,
		"VICE monitor",
		WS_OVERLAPPED|WS_CLIPCHILDREN/*|WS_CLIPSIBLINGS*/|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SIZEBOX,
        0,
        0,
        640,
        480,
        GetActiveWindow(),
        NULL,
        winmain_instance,
        NULL);


    
	
	ShowWindow( hwnd, SW_SHOW );

    OpenConsole(hwnd,TRUE);

    EnableCommands(GetMenu(hwnd),hwndToolbar);

	SetActiveWindow( hwnd );

    memcpy( &console_log_for_mon, console_log, sizeof( struct console_s ) );

    return &console_log_for_mon;

#else // #ifdef UIMON_EXPERIMENTAL

    console_log = console_open("Monitor");
    return console_log;

#endif // #ifdef UIMON_EXPERIMENTAL
}

void arch_mon_window_suspend( void )
{
#ifdef UIMON_EXPERIMENTAL

#else // #ifdef UIMON_EXPERIMENTAL

    arch_mon_window_close();

#endif // #ifdef UIMON_EXPERIMENTAL
}

console_t *arch_mon_window_resume( void )
{
#ifdef UIMON_EXPERIMENTAL

	pRetValue = NULL;

    UpdateWindow( hwnd );
    return &console_log_for_mon;

#else // #ifdef UIMON_EXPERIMENTAL

    return arch_mon_window_open();

#endif // #ifdef UIMON_EXPERIMENTAL
}

#define MAX_OUTPUT_LENGTH 2000

int arch_mon_out(const char *format, ...)
{
    va_list ap;
    char buffer[MAX_OUTPUT_LENGTH];

    if (console_log)
    {
        va_start(ap, format);
        vsprintf(buffer, format, ap);
        return console_out(console_log, buffer);
    }
    return 0;
}

char *arch_mon_in()
{
#ifdef UIMON_EXPERIMENTAL

    char *p = NULL;

	while (!p && !pRetValue)
	{
		if (console_log)
		{
	        p = console_in(console_log);
			if (p)
			{
				if (*p==0)
				{
					free(p);
					p = NULL;
				}
			}
		}
		else
		{
	        while (!pRetValue && !console_log)
		    {
			    ui_dispatch_next_event();
			}
		}
    }

    if (pRetValue)
    {
        if (p)
            free(p);
        p = stralloc(pRetValue);
        pRetValue = NULL;
    }

    return p;

#else // #ifdef UIMON_EXPERIMENTAL

    return console_in(console_log);

#endif // #ifdef UIMON_EXPERIMENTAL
}
