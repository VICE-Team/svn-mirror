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
// #include "fullscreen.h"
#include "mon.h"
#include "res.h"
#include "ui.h"
#include "uimon.h"
// #include "utils.h"
#include "winmain.h"

// #define UIMON_EXPERIMENTEL 1

#ifdef UIMON_EXPERIMENTEL

static HWND hwndDis = NULL;
static HWND hwndReg = NULL;
static HWND hwndConsole   = NULL;
static HWND hwndMdiClient = NULL;
static HWND hwndToolbar = NULL;

static console_t *console_log = NULL;

#define REG_CLASS MONITOR_CLASS ":Reg"
#define DIS_CLASS MONITOR_CLASS ":Dis"

/* window procedure */
static long CALLBACK mon_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	switch (msg)
	{
	case WM_CLOSE:
		/* FALL THROUGH */

	case WM_DESTROY:
        PostMessage( hwndConsole, WM_CLOSE, 0, 0 );
        return DefFrameProc(hwnd, hwndMdiClient, msg, wParam, lParam);
/*
	case WM_LBUTTONUP:
        PostMessage( hwndConsole, WM_CHAR, 13, 0 );
		break;
*/
    case WM_CREATE:
        {
            CLIENTCREATESTRUCT ccs;
            ccs.hWindowMenu  = GetSubMenu(GetMenu(hwnd),1);
            ccs.idFirstChild = IDM_WNDCHILD;
            hwndMdiClient = CreateWindow(
                "MdiClient",NULL,WS_CHILD|WS_CLIPCHILDREN|WS_VSCROLL|WS_HSCROLL,
                0,50,0,0,
                hwnd,(HMENU)0xCAC,winmain_instance,(LPSTR)&ccs);

            ShowWindow( hwndMdiClient, SW_SHOW );
        }

        hwndDis = CreateMDIWindow(DIS_CLASS,
            "Disassembly",
		    0,
            0, // CW_USEDEFAULT, // 20,
            0, // CW_USEDEFAULT, // 60,
            500, // CW_USEDEFAULT, // 600,
            500, // CW_USEDEFAULT, // 340,
            hwndMdiClient,
            winmain_instance,
            0);

        hwndReg = CreateMDIWindow(REG_CLASS,
            "Register",
		    0,
            0,
            0,
            0,
            0,
            hwndMdiClient,
            winmain_instance,
            0);

        // create toolbar
        {
    		TBBUTTON tbb[10];
            int i,j;

            for (i=0,j=0;i<10;i++)
            {
                tbb[i].iBitmap = j;
                tbb[i].idCommand = i;

                tbb[i].fsState = TBSTATE_ENABLED;
                tbb[i].fsStyle = ((i%4) || (i==0)) ? TBSTYLE_BUTTON : TBSTYLE_SEP;
                tbb[i].dwData  = 0;
                tbb[i].iString = j;
                if ((i%4) || (i==0)) j++;
            }

            hwndToolbar = CreateToolbarEx( hwnd, 
                WS_CHILD, // WORD ws, 
                3, // UINT wID, 
                8, // int nBitmaps, 
                winmain_instance, // HINSTANCE hBMInst, 
                IDR_MONTOOLBAR, // UINT wBMID, 
                tbb, // LPCTBBUTTON lpButtons, 
                10, // int iNumButtons, 
                16, // int dxButton, 
                16, // int dyButton, 
                16, // int dxBitmap, 
                16, // int dyBitmap, 
                sizeof(TBBUTTON) // UINT uStructSize 
                );

            ShowWindow(hwndToolbar,SW_SHOW);
        }
        break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;

			hdc = BeginPaint(hwnd,&ps);
			EndPaint(hwnd,&ps);

            InvalidateRect(hwndToolbar,NULL,TRUE);
            UpdateWindow(hwndToolbar);

			return 0;
		}
	}

	return DefFrameProc(hwnd, hwndMdiClient, msg, wParam, lParam);
}


/* window procedure */
static long CALLBACK reg_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
    static int width  = 0;
    static int height = 0;

	switch (msg)
	{
	case WM_CLOSE:
		/* FALL THROUGH */

	case WM_DESTROY:
		return 0;

    case WM_CREATE:
        {
            HDC hdc = GetDC( hwnd );
           	SIZE size;
            RECT rect;

	        SelectObject( hdc, GetStockObject( ANSI_FIXED_FONT ) );

            // get height and width of a character
            GetTextExtentPoint32( hdc, " ", 1, &size );
        	width  = size.cx;
	        height = size.cy;

            // resize window
            GetWindowRect( hwnd, &rect );
            rect.right  += width*34;
            rect.bottom += height*2;
//            AdjustWindowRect( &rect, GetWindowLong( hwnd, GWL_STYLE ), FALSE );

/**/
        	MoveWindow( hwnd, rect.left, rect.top,
		        rect.right - rect.left, rect.bottom - rect.top, TRUE );
/**/
            break;
        }

	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
            char buffer[5];
                     int lastRegValue;
            static   int LastShownRegs[16];

			hdc = BeginPaint(hwnd,&ps);

            SetTextColor(hdc,RGB(0,0,0));
            TextOut( hdc, 0, 0,        " PC  FL NV-BDIZC AC XR YR SP 00 01", 34 );

#define DO_OUT(_bCond,_x,_val,_len) \
    SetTextColor(hdc,RGB((_bCond)?0xFF:0,0,0)); \
    TextOut( hdc,  (_x)*width, 0+height, _val, _len )

#define DO_OUT_REG(_no,_x,_reg,_len) \
    lastRegValue = mon_get_reg_val(e_comp_space,_reg); \
    sprintf(buffer,"%0" #_len "X", lastRegValue ); \
    DO_OUT(lastRegValue!=LastShownRegs[_no],_x,buffer,_len); \
    LastShownRegs[_no]=lastRegValue

#define DO_OUT_FLG(_no,_x,_val) \
    DO_OUT((lastRegValue&_val)!=LastShownRegs[_no],_x,(lastRegValue & _val)?"1":"0",1); \
    LastShownRegs[_no]=lastRegValue & _val

#define DO_OUT_MEM(_no,_x,_addr) \
    lastRegValue = mon_get_mem_val(e_comp_space,_addr); \
    sprintf(buffer,"%02X", lastRegValue ); \
    DO_OUT(lastRegValue!=LastShownRegs[_no],_x,buffer,2); \
    LastShownRegs[_no]=lastRegValue

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

	return DefMDIChildProc(hwnd, msg, wParam, lParam);
}


/* window procedure */
static long CALLBACK dis_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
    static int width  = 0;
    static int height = 0;

	switch (msg)
	{
	case WM_CLOSE:
		/* FALL THROUGH */

	case WM_DESTROY:
		return 0;

    case WM_CREATE:
        {
            HDC hdc = GetDC( hwnd );
           	SIZE size;

	        SelectObject( hdc, GetStockObject( ANSI_FIXED_FONT ) );

            // get height and width of a character
            GetTextExtentPoint32( hdc, " ", 1, &size );
        	width  = size.cx;
	        height = size.cy; 
            break;
        }

	case WM_PAINT:
		{
            static unsigned int StartAddress = -1;
            static unsigned int EndAddress   = 0;

			PAINTSTRUCT ps;
			HDC hdc;
            unsigned int uAddress = mon_get_reg_val(e_comp_space,e_PC);
            unsigned int loc;
            unsigned int size;
            char buffer[512];
            const char *p;
            char *label;
            int  i;

            if ((uAddress < StartAddress) || (uAddress > EndAddress))
            {
                StartAddress = uAddress;
                EndAddress = 0;
            }

            loc = StartAddress;

			hdc = BeginPaint(hwnd,&ps);


#define DO_OUT(_y,_addr) \
    SetTextColor(hdc,RGB((_addr)==uAddress?0xFF:0,0,0)); \
    TextOut( hdc,  0, _y*height, buffer, strlen(buffer) )

            for (i=0; i*height<400; i++)
            {
               if (i*height < 300)
                   EndAddress = loc;

               label = mon_symbol_table_lookup_name(e_comp_space, loc);
               if (label)
               {
                  sprintf( buffer, "%s:",label);
                  DO_OUT(i++,loc);
               }

               p = mon_disassemble_to_string_ex( loc,
                       mon_get_mem_val(e_comp_space,loc),
                       mon_get_mem_val(e_comp_space,loc+1),
                       mon_get_mem_val(e_comp_space,loc+2),
                       1,
                       &size );

               sprintf( buffer, "%04X: %s",
                   loc,
                   p );

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
	WNDCLASSEX wc;

	/* Register 2nd window class for the monitor window */
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

static HWND hwnd = NULL;

#endif // #ifdef UIMON_EXPERIMENTEL


void arch_mon_window_close( void )
{
#ifdef UIMON_EXPERIMENTEL

	DestroyWindow(hwnd);
	hwnd          =
    hwndMdiClient = NULL;

#else // #ifdef UIMON_EXPERIMENTEL
    console_close(console_log);

#endif // #ifdef UIMON_EXPERIMENTEL

    console_log = NULL;
}


console_t *arch_mon_window_open( void )
{

#ifdef UIMON_EXPERIMENTEL

    HWND hwndParent = GetActiveWindow();

    arch_mon_init();

    hwnd = CreateWindow(MONITOR_CLASS,
		"VICE monitor",
		WS_OVERLAPPED|WS_CLIPCHILDREN|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
        0,
        0,
        800,
        600,
        GetActiveWindow(),
        NULL,
        winmain_instance,
        NULL);

    ShowWindow( hwnd, SW_SHOW );

    console_log = arch_console_open_mdi("Monitor",&hwndConsole,&hwndParent,&hwndMdiClient);

#else // #ifdef UIMON_EXPERIMENTEL

    console_log = console_open("Monitor");

#endif // #ifdef UIMON_EXPERIMENTEL

    return console_log;
}

void arch_mon_window_suspend( void )
{
#ifdef UIMON_EXPERIMENTEL

#else // #ifdef UIMON_EXPERIMENTEL

    arch_mon_window_close();

#endif // #ifdef UIMON_EXPERIMENTEL
}

console_t *arch_mon_window_resume( void )
{
#ifdef UIMON_EXPERIMENTEL

    InvalidateRect( hwndDis, NULL, FALSE );
    InvalidateRect( hwndReg, NULL, FALSE );
    UpdateWindow( hwnd );
    return console_log;

#else // #ifdef UIMON_EXPERIMENTEL

    return arch_mon_window_open();

#endif // #ifdef UIMON_EXPERIMENTEL
}
