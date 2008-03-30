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
#include "fullscrn.h"
#include "lib.h"
#include "mon_register.h"
#include "mon_ui.h"
#include "mon_util.h"
#include "res.h"
#include "resources.h"
#include "ui.h"
#include "uimon.h"
#include "winmain.h"


/*
 TODO: @SRT - This is just a quick hack since the MINGW headers don't define this...
*/
#ifndef SIF_TRACKPOS
#define SIF_TRACKPOS        0x0010
#endif

// #define DEBUG_UIMON

/* Debugging stuff.  */
#ifdef DEBUG_UIMON

#include "log.h"

static void uimon_debug(const char *format, ...)
{
        char *buffer;
        va_list args;

        va_start(args, format);
        buffer = lib_mvsprintf(format, args);
        va_end(args);
        OutputDebugString(buffer);
	    log_message(LOG_DEFAULT,buffer);
        printf(buffer);
        lib_free(buffer);
}
#define UIM_DEBUG(x) uimon_debug x
#else
#define UIM_DEBUG(x)
#endif


#define UIMON_EXPERIMENTAL 1

#ifdef UIMON_EXPERIMENTAL

   #define OPEN_DISASSEMBLY_AS_POPUP
   #define OPEN_REGISTRY_AS_POPUP

static HWND hwndConsole   = NULL;
static HWND hwndMdiClient = NULL;
static HWND hwndToolbar   = NULL;
static HWND hwndMonitor   = NULL;


typedef
struct uimon_client_windows_s
{
    HWND hwnd;
    struct uimon_client_windows_s *next;
} uimon_client_windows_t;

uimon_client_windows_t *first_client_window = NULL;


static console_t *console_log = NULL;
static console_t  console_log_for_mon = { -50, -50, -50 };
static char *pchCommandLine   = NULL;

#define REG_CLASS MONITOR_CLASS ":Reg"
#define DIS_CLASS MONITOR_CLASS ":Dis"

static HWND hwndParent = NULL;
static HWND hwndActive = NULL;


/**/
void add_client_window( HWND hwnd )
{
    uimon_client_windows_t *new_client = lib_malloc( sizeof(uimon_client_windows_t) );

    new_client->hwnd = hwnd;
    new_client->next = first_client_window;

    first_client_window = new_client;
}

void delete_client_window( HWND hwnd )
{
    uimon_client_windows_t *pold = NULL;
    uimon_client_windows_t *p;

    for (p = first_client_window; p; pold = p, p = p->next)
    {
        if (p->hwnd == hwnd)
        {
            if (pold)
                pold->next = p->next;
            else
                first_client_window = p->next;

            lib_free(p);
            break;
        }
    }
}
/**/

static monitor_interface_t **monitor_interface;
static int                   count_monitor_interface;

#endif // #ifdef UIMON_EXPERIMENTAL


void uimon_set_interface(monitor_interface_t **monitor_interface_init,
                         int count)
{
#ifdef UIMON_EXPERIMENTAL
    monitor_interface = monitor_interface_init;
    count_monitor_interface = count;
#endif // #ifdef UIMON_EXPERIMENTAL
}

#ifdef UIMON_EXPERIMENTAL


#define WM_OWNCOMMAND (WM_USER+0x100)
#define WM_CHANGECOMPUTERDRIVE (WM_OWNCOMMAND+1)
#define WM_GETWINDOWTYPE       (WM_OWNCOMMAND+2)
#define WM_UPDATEVAL           (WM_OWNCOMMAND+3)
#define WM_UPDATE              (WM_OWNCOMMAND+4)

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

    ptbb = (PTBBUTTON) lib_malloc(pData->wItemCount*sizeof(TBBUTTON));
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

    lib_free(ptbb);
    
unlock:
#ifdef HAS_UNLOCKRESOURCE
    UnlockResource(hGlobal);
#endif
    FreeResource(hGlobal);
quit:
    return hToolbar;
}



static
HWND iOpenDisassembly( HWND hwnd, DWORD dwStyle, int x, int y, int dx, int dy )
{
    HWND hwndDis;

#ifdef OPEN_DISASSEMBLY_AS_POPUP
    #define DEF_DIS_PROG DefWindowProc
    hwndDis = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        DIS_CLASS,
        "Disassembly",
        WS_OVERLAPPEDWINDOW|WS_VSCROLL|dwStyle, // WS_CAPTION|WS_POPUPWINDOW|WS_THICKFRAME|WS_SYSMENU|dwStyle,
        x,
        y,
        dx,
        dy,
        hwndMonitor, // hwndMdiClient,
        NULL,
        winmain_instance,
        NULL);

    add_client_window( hwndDis );

    ShowWindow( hwndDis, SW_SHOW );

#else /* #ifdef OPEN_DISASSEMBLY_AS_POPUP */
    #define DEF_DIS_PROG DefMDIChildProc
    hwndDis = CreateMDIWindow(DIS_CLASS,
        "Disassembly",
        dwStyle,
        x,
        y,
        dx,
        dy,
        hwndMdiClient,
        winmain_instance,
        0);

    add_client_window( hwndDis );

#endif /* #ifdef OPEN_DISASSEMBLY_AS_POPUP */

    return hwndDis;
}

static
HWND OpenDisassembly( HWND hwnd )
{
	// @SRT: TODO: Adjust parameter!
    return iOpenDisassembly( hwnd, 0, 0, 0, 300, 300 );
}

static
void DestroyMdiWindow(HWND hwndMdiClient, HWND hwndChild)
{
	SendMessage(hwndMdiClient,WM_MDIDESTROY,(WPARAM)hwndChild,0);
}

static
HWND iOpenConsole( HWND hwnd, BOOLEAN bOpen, DWORD dwStyle, int x, int y, int dx, int dy )
{
    if (bOpen)
    {
        console_log = uimon_console_open_mdi("Monitor",
            &hwndConsole,&hwndParent,&hwndMdiClient,dwStyle,x,y,dx,dy);
    }
    else
    {
		DestroyMdiWindow(hwndMdiClient,hwndConsole);

        hwndConsole = NULL;
    }
    return hwndConsole;
}

static
HWND OpenConsole( HWND hwnd, BOOLEAN bOpen )
{
	// @SRT: TODO: Adjust parameter!
	return iOpenConsole(hwnd,bOpen,WS_MAXIMIZE,0,0,0,0);
}

static
HWND iOpenRegistry( HWND hwnd, DWORD dwStyle, int x, int y, int dx, int dy )
{
    HWND hwndReg;

#ifdef OPEN_REGISTRY_AS_POPUP

    #define DEF_REG_PROG DefWindowProc
    hwndReg = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        REG_CLASS,
        "Register",
        WS_CAPTION|WS_POPUPWINDOW|dwStyle,
        x,
        y,
        dx,
        dy,
        hwnd,
        NULL,
        winmain_instance,
        NULL);

    add_client_window( hwndReg );
    ShowWindow( hwndReg, SW_SHOW );
    return hwndReg;

#else /* #ifdef OPEN_REGISTRY_AS_POPUP */

    #define DEF_REG_PROG DefMDIChildProc
    hwndReg = CreateMDIWindow(REG_CLASS,
        "Register",
        dwStyle,
        x,  // 0 is sufficient here
        y,  // 0 is sufficient here
        dx, // 0 is sufficient here
        dy, // 0 is sufficient here
        hwndMdiClient,
        winmain_instance,
        0);

    add_client_window( hwndReg );

#endif /* #ifdef OPEN_REGISTRY_AS_POPUP */

    return hwndReg;
}

static
HWND OpenRegistry( HWND hwnd )
{
	// @SRT: TODO: Adjust parameter!
    return iOpenRegistry( hwnd, 0, 30, 100, 100, 100 );
}

/**********************************************************************************
***********************************************************************************
***********************************************************************************
***********************************************************************************
**********************************************************************************/

static
void writeString3( char **where, BYTE **what, size_t* s )
{
size_t size = (*s>3) ? 3 : *s;

long what1 = (size>0) ? (*what)[0] : 0;
long what2 = (size>1) ? (*what)[1] : 0;
long what3 = (size>2) ? (*what)[2] : 0;

unsigned long value;
BYTE *b = (BYTE*) &value;

*what += size;
*s    -= size;

value  = (what1 & 0xC0) << 18;
value |= (what2 & 0xC0) << 20;
value |= (what3 & 0xC0) << 22;

value |= (what1 & 0x3F) <<  0;
value |= (what2 & 0x3F) <<  8;
value |= (what3 & 0x3F) << 16;

*(*where)++ = 0x3F + *b++;
*(*where)++ = 0x3F + *b++;
*(*where)++ = 0x3F + *b++;
*(*where)++ = 0x3F + *b++;
}

static
char *encode( BYTE *content, size_t size )
{
char *buffer;
unsigned long prefix = size;
int i;
BYTE *pb;
size_t n;

// calculate integrity check
char *p = (char*) content;
BYTE xor = (size & 0xFF) ^ (size>>8);

if (!content || size==0)
   return NULL;

for (i=size; i; i--)
   {
   xor ^= *p++;
   }

prefix |= (((long)xor) << 16); // put integrity check in prefix

// make string
buffer = lib_malloc(((size+2)/3)*4+5);
p = buffer;
pb = (BYTE*) &prefix;
n = 3;
writeString3( &p, &pb, &n );

while (size)
   {
   writeString3( &p, &content, &size );
   }

*p = 0;

return buffer;
}

static
BOOLEAN getString3( BYTE **where, char **what, size_t* s, BYTE* xor )
{
BOOLEAN ok = TRUE;

size_t size = (*s>3) ? 3 : *s;

unsigned long value;
BYTE *b = (BYTE*) &value;

BYTE what1;
BYTE what2;
BYTE what3;

if ( (**what < 0x3F) || (**what > 0x7E) ) ok = FALSE; *b++ = *(*what)++ - 0x3F;
if ( (**what < 0x3F) || (**what > 0x7E) ) ok = FALSE; *b++ = *(*what)++ - 0x3F;
if ( (**what < 0x3F) || (**what > 0x7E) ) ok = FALSE; *b++ = *(*what)++ - 0x3F;
if ( (**what < 0x3F) || (**what > 0x7E) ) ok = FALSE; *b++ = *(*what)++ - 0x3F;

// intentionally convert from unsigned long to BYTE
what1 = (BYTE) (((value >>  0) & 0x3F) | ((value >> 18) & 0xC0));
what2 = (BYTE) (((value >>  8) & 0x3F) | ((value >> 20) & 0xC0));
what3 = (BYTE) (((value >> 16) & 0x3F) | ((value >> 22) & 0xC0));

*s -= size;

if (size>0)
   {
   *xor ^= what1;
   *(*where)++ = what1;
   }

if (size>1)
   {
   *xor ^= what2;
   *(*where)++ = what2;
   }

if (size>2)
   {
   *xor ^= what3;
   *(*where)++ = what3;
   }

return ok;
}

static
BYTE *decode( char *content, size_t* plen )
{
BOOLEAN ok = FALSE;
size_t size;
BYTE *buffer = NULL;

if (content)
   {
   long prefix;

   BYTE xor = 0;
      {
      BYTE *pBYTE = (BYTE*) &prefix;
      size_t n    = 3;
      ok = getString3( &pBYTE, &content, &n, &xor );
      }

   if (ok)
      {
      BYTE *p;
      p = buffer = lib_malloc((strlen(content)*3)/4); // @SRT
      xor = (BYTE) (prefix >> 16); // extract checksum from prefix
      *plen = size = (size_t)(prefix & 0xFFFF);  // extract size from prefix

      xor ^= (size & 0xFF) ^ (size>>8); // include size in checksum

      while (size && ok)
         {
         ok = getString3( &p, &content, &size, &xor );
         }

      if (xor)
         ok = FALSE;
      }

   }

if (!ok)
   *plen = 0;

return ok ? buffer : NULL;
}


/**********************************************************************************
***********************************************************************************
***********************************************************************************
***********************************************************************************
**********************************************************************************/

enum WindowType
{
    WT_END = 0, WT_CONSOLE, WT_DISASSEMBLY, WT_REGISTER
};
typedef enum WindowType WindowType;


struct WindowDimensions
{
    BYTE *pMonitorDimensionsBuffer;
    BYTE *pMonitorDimensions;
    int   MonitorLen;

	WINDOWPLACEMENT wpPlacement;
};
typedef struct WindowDimensions WindowDimensions;
typedef WindowDimensions *PWindowDimensions;

static
WORD GetByte(BYTE **p, int* len)
{
    --(*len);
	return *(*p)++;
}

static
WORD GetWord(BYTE **p, int* len)
{
	WORD   ah = GetByte(p,len) << 8;
	return ah | GetByte(p,len);
}

static
BOOLEAN GetPlacement( BYTE **p, int* len, WINDOWPLACEMENT *pwp )
{
	UINT i;

	PBYTE pNext = (PBYTE) pwp;
	for (i=sizeof(WINDOWPLACEMENT);(i>0) && (*len>0);i--)
		*pNext++ = (BYTE) GetByte(p,len);

	return (i==0) ? FALSE : TRUE;
}
 

static
BYTE **WriteByte(BYTE **p, BYTE a)
{
	*(*p)++ = a;
	return p;
}

static
BYTE **WriteWord(BYTE **p, WORD a)
{
	WriteByte(p,(BYTE) ((a>>8) & 0xFF));
	WriteByte(p,(BYTE) ( a     & 0xFF));
	return p;
}

static
BYTE **WritePlacement( BYTE **p, WINDOWPLACEMENT *pwp )
{
	UINT i;
	PBYTE pNext = (PBYTE) pwp;
	for (i=0;i<pwp->length;i++)
		WriteByte(p,*pNext++);

	return p;
}
 
static
WindowType GetNextMonitorDimensions( PWindowDimensions pwd )
{
    WindowType ret;

    if (pwd->MonitorLen == 0)
    {
        ret = WT_END;
    }
    else
    {
        ret = GetByte(&(pwd->pMonitorDimensions),&pwd->MonitorLen);

        if (pwd->MonitorLen < sizeof(WINDOWPLACEMENT))
        {
            ret = WT_END;

            UIM_DEBUG(( "UIMON.C: pwd->MonitorLen has size %u, "
                "should have size of at least %u.", pwd->MonitorLen, sizeof(WINDOWPLACEMENT) ));
        }
        else
        {
			GetPlacement(&(pwd->pMonitorDimensions),&pwd->MonitorLen,&(pwd->wpPlacement));
        }
    }
    return ret;
}

static
void SetNextMonitorDimensions( HWND hwnd, WindowType wt, BYTE **p )
{
	WINDOWPLACEMENT wpPlacement;
	wpPlacement.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hwnd,&wpPlacement);

    WriteByte(p,(BYTE)wt);

	WritePlacement(p,&wpPlacement);
}

static
void OpenFromWindowDimensions(HWND hwnd,PWindowDimensions wd)
{
    WindowType wt;
    HWND hwndOpened = NULL;

    while ((wt = GetNextMonitorDimensions(wd)) != WT_END)
    {
        switch (wt)
        {
        case WT_CONSOLE:
            hwndOpened = iOpenConsole(hwnd,TRUE,0,0,0,0,0);
            break;

        case WT_DISASSEMBLY:
            hwndOpened = OpenDisassembly(hwnd);
            break;

        case WT_REGISTER:
            hwndOpened = OpenRegistry(hwnd);
            break;
        case WT_END:
            /* this cannot occur, but since gcc complains if not specified... */
            hwndOpened = NULL;
        };

		SetWindowPlacement( hwndOpened, &(wd->wpPlacement) );
    };

    lib_free(wd->pMonitorDimensionsBuffer);
    lib_free(wd);
}

static
PWindowDimensions LoadMonitorDimensions(HWND hwnd)
{
	PWindowDimensions ret = NULL;

    char *dimensions;
    BYTE *buffer;
    int   len;
    BOOLEAN bError = FALSE;

    resources_get_value("MonitorDimensions", (void *)&dimensions);
    buffer = decode(dimensions,&len);

	if (len!=0)
    {
        char *p = buffer;

        if (len<8)
            bError = TRUE;
        else
        {
            ret    = lib_malloc( sizeof(*ret) );
			bError = GetPlacement((BYTE **)(&p), &len, &(ret->wpPlacement));
			SetWindowPlacement( hwnd, &(ret->wpPlacement) );

            ret->pMonitorDimensionsBuffer = buffer;
            ret->pMonitorDimensions       = p;
            ret->MonitorLen               = len;
        }
	}

    if (bError)
        lib_free(buffer);

	return ret;
}

static
VOID iWindowStore( HWND hwnd, BYTE **p )
{
    LONG WindowType = WT_CONSOLE;

    SendMessage( hwnd, WM_GETWINDOWTYPE, 0, (LPARAM) &WindowType );

    SetNextMonitorDimensions( hwnd, WindowType, p );
}

static int  nHwndStack = 0;
static HWND hwndStack[256]; // @SRT

static
VOID WindowStore( BYTE **p )
{
	while (nHwndStack--)
	{
		iWindowStore(hwndStack[nHwndStack], p);
	}
	nHwndStack = 0;
}

static
BOOL CALLBACK WindowStoreProc( HWND hwnd, LPARAM lParam )
{
	hwndStack[nHwndStack++] = hwnd;
	return TRUE;
}

static
void StoreMonitorDimensions(HWND hwnd)
{
    char *dimensions;
    BYTE  buffer[1024]; // @SRT
    BYTE *p = buffer;
    uimon_client_windows_t *clients;

	WINDOWPLACEMENT wpPlacement;
	wpPlacement.length = sizeof(WINDOWPLACEMENT);

	GetWindowPlacement( hwnd, &wpPlacement );

	WritePlacement( &p, &wpPlacement );

	// store info for open windows in structure
	EnumChildWindows(hwndMdiClient,(WNDENUMPROC)WindowStoreProc,0);

    for (clients = first_client_window; clients; clients=clients->next )
        WindowStoreProc( clients->hwnd, 0 );

	WindowStore(&p);

    dimensions = encode(buffer,(int)(p-buffer)); // @SRT
    resources_set_value("MonitorDimensions",(resource_value_t *)dimensions);
    lib_free(dimensions);
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

static
void SetMemspace( HWND hwnd, MEMSPACE memspace )
{
	BOOL bComputer = FALSE;
	BOOL bDrive8   = FALSE;
	BOOL bDrive9   = FALSE;
        BOOL bDrive10  = FALSE;
        BOOL bDrive11  = FALSE;
	HMENU hmnu     = GetMenu(hwnd);
    int drive_true_emulation;

	char *pText = "";

	switch (memspace)
	{
	case e_comp_space:  bComputer = TRUE; pText = "Computer"; break;
	case e_disk8_space: bDrive8   = TRUE; pText = "Drive 8";  break;
	case e_disk9_space: bDrive9   = TRUE; pText = "Drive 9";  break;
        case e_disk10_space: bDrive10 = TRUE; pText = "Drive 10";  break;
        case e_disk11_space: bDrive11 = TRUE; pText = "Drive 11";  break;

    /* 
        these two cases should not occur; 
        they're just there to avoid the warning 
    */
    case e_default_space: /* FALL THROUGH */
    case e_invalid_space: break; 
	}

    resources_get_value("DriveTrueEmulation", (void *)&drive_true_emulation);

    ENABLE( IDM_MON_COMPUTER, drive_true_emulation ? 1 : 0);
	ENABLE( IDM_MON_DRIVE8,   drive_true_emulation ? 1 : 0);
    ENABLE( IDM_MON_DRIVE9,   drive_true_emulation ? 1 : 0);
    CHECK ( IDM_MON_COMPUTER, drive_true_emulation ? bComputer : TRUE);
	CHECK ( IDM_MON_DRIVE8,   drive_true_emulation ? bDrive8   : FALSE);
    CHECK ( IDM_MON_DRIVE9,   drive_true_emulation ? bDrive9   : FALSE);

	if (drive_true_emulation)
	{
		char pOldText[256];
		int  n = GetWindowText( hwnd, pOldText, 256 );

		if (n!=0)
		{
			char *pWrite = strchr(pOldText,':');
			if (pWrite==NULL)
			{
				pWrite = strchr(pOldText,0);
				*pWrite=':';
			}
		
			strcpy( ++pWrite, pText );
			SetWindowText(hwnd,pOldText);
		}
	}
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


void uimon_after_set_command(void)
{
    if (hwndConsole) 
        SendMessage(hwndConsole,WM_CONSOLE_INSERTLINE,0,0 );
}

#define SET_COMMAND( _cmd ) \
    mon_set_command(console_log,_cmd,uimon_after_set_command)

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

static
void OnConsoleResize(void)
{
    if (console_log)
        memcpy( &console_log_for_mon, console_log, sizeof( struct console_s ) );
}

static
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
		if (hwndActive)
			SendMessage( hwndActive, WM_CHANGECOMPUTERDRIVE, wID, 0 );
		break;
    }
}


/* window procedure */
static 
long CALLBACK mon_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	switch (msg)
	{
	case WM_CLOSE:
        SET_COMMAND("x");
        /*
        return 0 so we don't use DefFrameProc(), so we're sure 
        we do exactly the same as we would do if "x" was 
        entered from the keyboard.
        */
        return 0;

	case WM_DESTROY:
        return DefFrameProc(hwnd, hwndMdiClient, msg, wParam, lParam);

    case WM_CONSOLE_RESIZED:
        OnConsoleResize();
        return 0;

    case WM_CONSOLE_ACTIVATED:
		hwndActive = NULL;
        ClearMemspace( hwnd );
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
                // Tell the toolbar to resize itself to fill the top of the window.
                if (hwndToolbar)
                   SendMessage(hwndToolbar, TB_AUTOSIZE, 0L, 0L);

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

		ClearMemspace( hwnd );
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
void ActivateChild( BOOL bActivated, HWND hwndOwn, MEMSPACE memspace )
{
	if (bActivated)
	{
		// we are activated
		hwndActive = hwndOwn;
        SetMemspace( hwndOwn, memspace );
	}
	else
	{
/**/
		// we are deactivated
		hwndActive = NULL;
        ClearMemspace( hwndOwn );
/**/
	}
}


typedef
struct reg_private
{
    int           charwidth;
    int           charheight;
	MEMSPACE      memspace;
	unsigned int *LastShownRegs;
    unsigned int  RegCount;
} reg_private_t;

static
void update_last_shown_regs( reg_private_t *prp )
{
    mon_reg_list_t *pMonRegs = mon_register_list_get(prp->memspace);
    mon_reg_list_t *p;
    unsigned int    cnt;

    if (prp->LastShownRegs!=NULL)
    {
        for (p = pMonRegs, cnt = 0; p != NULL; p = p->next, cnt++ )
        {
            if (cnt < prp->RegCount)
            {
                prp->LastShownRegs[cnt] = p->val;
            }
        }
    }

    lib_free( pMonRegs );
}

static
BOOLEAN output_register(HDC hdc, reg_private_t *prp, RECT *clientrect)
{
    mon_reg_list_t *pMonRegs = mon_register_list_get(prp->memspace);
    mon_reg_list_t *p;

    BOOLEAN      changed_dimensions = FALSE;
    int          x                  = 0;
    unsigned int cnt;

    if (prp->LastShownRegs==NULL)
    {
        // initialize the values which have been last shown
        for (p = pMonRegs, cnt = 0; p != NULL; p = p->next, cnt++ )
            ;

        prp->RegCount      = cnt;
        prp->LastShownRegs = lib_malloc( sizeof(int) * cnt );

        // ensure that ALL registers appear changed this time!
        for (p = pMonRegs, cnt = 0; p != NULL; p = p->next )
        {
            prp->LastShownRegs[cnt++] = ~(p->val);
        }
    }

    // clear client area of window
    {
        HGDIOBJ hg = SelectObject( hdc, GetStockObject( NULL_PEN ) );
        Rectangle( hdc, clientrect->left, clientrect->top, 
            clientrect->right, clientrect->bottom );
        SelectObject( hdc, hg );
    }

    for (p = pMonRegs, cnt = 0; p != NULL; p = p->next, cnt++ )
    {
        char  buffer[5];
        int   namelen    = strlen(p->name);
        int   center     = 0;
        int   vallen;

        int   changedbits = 0;

        if (cnt < prp->RegCount)
        {
            changedbits = prp->LastShownRegs[cnt] ^ p->val;
        }

        if (p->flags)
        {
            unsigned int i;
            unsigned int val       = p->val << (16-p->size);
            unsigned int changed_i = changedbits << (16-p->size);

            for (i=0;i<p->size;i++)
            {
                char pw = val & 0x8000 ? '1' : '0';
                BOOL changed = changed_i & 0x8000 ? TRUE : FALSE;

                val       <<= 1;
                changed_i <<= 1;

                // output value of register
                SetTextColor(hdc,RGB(changed?0xFF:0,0,0));
                TextOut( hdc, (x+i)*prp->charwidth, prp->charheight, &pw, 1 );
            }

            vallen = p->size;
        }
        else
        {
            switch (p->size)
            {
            case 8:
                vallen = 2;
                sprintf(buffer, "%02X ", p->val );
                break;

            case 16:
                vallen = 4;
                sprintf(buffer, "%04X ", p->val );
                break;

            default:
                vallen    = namelen;
                buffer[0] = 0;
                break;
            };

            // output value of register
            SetTextColor(hdc,RGB(changedbits?0xFF:0,0,0));
            TextOut( hdc, x*prp->charwidth, prp->charheight, buffer, vallen );
        }

        center = (vallen-namelen)/2;

        // output name of register
        SetTextColor(hdc,RGB(0,0,0));
        TextOut( hdc, (x+center)*prp->charwidth, 0, p->name, namelen );

        x += vallen+1;
    }

    --x;
    if ( x*prp->charwidth != clientrect->right)
    {
        clientrect->right = x*prp->charwidth;
        changed_dimensions = TRUE;
    }

    if ( 2*prp->charheight != clientrect->bottom)
    {
        clientrect->bottom = 2*prp->charheight;
        changed_dimensions = TRUE;
    }

    if (changed_dimensions)
    {
        lib_free( prp->LastShownRegs );
        prp->LastShownRegs = NULL;
        prp->RegCount      = 0;

        /* we will be redrawn in the not so far future! */
    }

    lib_free( pMonRegs );

    return changed_dimensions;
}

/* window procedure */
static 
long CALLBACK reg_window_proc(HWND hwnd, 
	UINT msg, WPARAM wParam, LPARAM lParam)

{
	reg_private_t *prp = (reg_private_t*) GetWindowLong( hwnd, GWL_USERDATA );

	switch (msg)
	{
    case WM_UPDATEVAL:
        update_last_shown_regs(prp);
        return 0;

	case WM_DESTROY:
        delete_client_window(hwnd);
		// free the reg_private info 
		lib_free(prp);
		SetWindowLong( hwnd, GWL_USERDATA, 0 );

	    return DEF_REG_PROG(hwnd, msg, wParam, lParam);

#ifdef OPEN_REGISTRY_AS_POPUP

    case WM_ACTIVATE:
        ActivateChild( (LOWORD(wParam)!=WA_INACTIVE) ? TRUE:FALSE, hwnd, prp->memspace );
        break;

#else  // #ifdef OPEN_REGISTRY_AS_POPUP

	case WM_MDIACTIVATE:
        ActivateChild( ((HWND) wParam==hwnd)?FALSE:TRUE, hwnd, prp->memspace );
		break;

#endif  // #ifdef OPEN_REGISTRY_AS_POPUP

    case WM_GETWINDOWTYPE:
        {
            LONG* lp = (PLONG) lParam;
            *lp = WT_REGISTER;
        }
        return 0;

	case WM_CHANGECOMPUTERDRIVE:
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

			prp = lib_malloc(sizeof(reg_private_t));

            prp->LastShownRegs = NULL;
            prp->RegCount      = 0;
			
			/* store pointer to structure with window */
			SetWindowLong( hwnd, GWL_USERDATA, (long) prp );

	        SelectObject( hdc, GetStockObject( ANSI_FIXED_FONT ) );

            // get height and width of a character
            GetTextExtentPoint32( hdc, " ", 1, &size );
        	prp->charwidth  = size.cx;
	        prp->charheight = size.cy;

			prp->memspace = e_comp_space;

            break;
        }

	case WM_COMMAND:
		{
		    switch (LOWORD(wParam))
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
			HDC         hdc;
            RECT        rect;
            BOOLEAN     changed_dimension;

            GetClientRect( hwnd, &rect );

			hdc = BeginPaint(hwnd,&ps);
            changed_dimension = output_register( hdc, prp, &rect );
            EndPaint(hwnd,&ps);

            if (changed_dimension)
            {
                // resize the window

            	ClientToScreen( hwnd,  (LPPOINT) &rect);
	            ClientToScreen( hwnd, ((LPPOINT) &rect) + 1);

/**/
#ifdef OPEN_REGISTRY_AS_POPUP
#else  // #ifdef OPEN_REGISTRY_AS_POPUP
                ScreenToClient( hwndMdiClient,  (LPPOINT) &rect);
		        ScreenToClient( hwndMdiClient, ((LPPOINT) &rect) + 1);
#endif // #ifdef OPEN_REGISTRY_AS_POPUP
/**/

                AdjustWindowRectEx( &rect, 
                    GetWindowLong( hwnd, GWL_STYLE ), FALSE, GetWindowLong( hwnd, GWL_EXSTYLE ) );

            	MoveWindow( hwnd, rect.left, rect.top,
	    	        rect.right - rect.left, rect.bottom - rect.top, TRUE );
            }

			return 0;
		}
	}

    return DEF_REG_PROG(hwnd, msg, wParam, lParam);
}


typedef
struct dis_private
{
    int      charwidth;
    int      charheight;

    struct mon_disassembly_private *pmdp;

} dis_private_t;


static
int ExecuteDisassemblyPopup( HWND hwnd, dis_private_t *pdp, LPARAM lParam, BOOL bExecuteDefault )
{
    WORD ulDefault;
    WORD ulMask;
    WORD xPos = LOWORD(lParam) / pdp->charwidth;
    WORD yPos = HIWORD(lParam) / pdp->charheight;

    /* determine which commands should be visible, and which one is the default */
    mon_disassembly_determine_popup_commands( pdp->pmdp, xPos, yPos, &ulMask, &ulDefault );

    /* now, create the appropriate pop up menu */
    {
        MENUITEMINFO mii;
        HMENU        hPopupMenu;
        POINT        curpos;
        int          nMenuCount = 0;
        UINT         uDefaultCommand = 0;

        hPopupMenu     = CreatePopupMenu();

        /* global initializations */
        mii.cbSize     = sizeof(mii);
        mii.fMask      = MIIM_STATE | MIIM_ID | MIIM_TYPE;
        mii.fType      = MFT_STRING;

/* make shotcuts for defining menu entries */
#define IMAKE_ENTRY( _FLAG_, _ID_, _TEXT_, _ENABLE_ ) \
        mii.fState     = _ENABLE_ | ((ulDefault & _FLAG_) ? MFS_DEFAULT : 0); \
        mii.wID        = _ID_; \
        mii.dwTypeData = _TEXT_; \
        mii.cch        = strlen(mii.dwTypeData); \
        InsertMenuItem( hPopupMenu, nMenuCount++, 1, &mii );

#define MAKE_ENTRY( _ID_, _TEXT_ ) \
    IMAKE_ENTRY( 0, _ID_, _TEXT_, MFS_ENABLED )

#define MAKE_COND_ENTRY( _FLAG_, _ID_, _TEXT_ ) \
        if (ulMask & _FLAG_) \
        { \
            if (ulDefault & _FLAG_) \
                uDefaultCommand = _ID_; \
            IMAKE_ENTRY( _FLAG_, _ID_, _TEXT_, MFS_ENABLED ); \
        }

#define MAKE_ENDISABLE_ENTRY( _FLAGS_, _ID_, _TEXT_ ) \
        IMAKE_ENTRY( 0, _ID_, _TEXT_, ((ulMask & _FLAGS_) ? MFS_ENABLED : (MFS_GRAYED|MFS_DISABLED)) );


#define MAKE_SEPARATOR() \
            mii.fType      = MFT_SEPARATOR; \
            mii.fState     = MFS_ENABLED;   \
            mii.wID        = 0;             \
            InsertMenuItem( hPopupMenu, nMenuCount++, 1, &mii ); \
            mii.fType      = MFT_STRING;

        MAKE_ENTRY( IDM_MON_GOTO_PC, "go&to PC" );
        MAKE_ENTRY( IDM_MON_GOTO_ADDRESS, "&goto address" );

        MAKE_SEPARATOR();

        MAKE_COND_ENTRY( MDDPC_SET_BREAKPOINT,     IDM_MON_SET_BP,     "&set breakpoint" );
        MAKE_COND_ENTRY( MDDPC_UNSET_BREAKPOINT,   IDM_MON_UNSET_BP,   "&unset breakpoint" );
        MAKE_COND_ENTRY( MDDPC_ENABLE_BREAKPOINT,  IDM_MON_ENABLE_BP,  "&enable breakpoint" );
        MAKE_COND_ENTRY( MDDPC_DISABLE_BREAKPOINT, IDM_MON_DISABLE_BP, "&disable breakpoint" );

        if (ulMask & (MDDPC_SET_COMPUTER | MDDPC_SET_DRIVE8 | MDDPC_SET_DRIVE9) )
        {
            MAKE_SEPARATOR();

            MAKE_ENDISABLE_ENTRY( MDDPC_SET_COMPUTER, IDM_MON_COMPUTER, "&Computer" );
            MAKE_ENDISABLE_ENTRY( MDDPC_SET_DRIVE8,   IDM_MON_DRIVE8,   "Drive &8"  );
            MAKE_ENDISABLE_ENTRY( MDDPC_SET_DRIVE9,   IDM_MON_DRIVE9,   "Drive &9"  );
        }

/* don't need the shortcuts anymore */
#undef IMAKE_ENTRY
#undef MAKE_ENTRY
#undef MAKE_COND_ENTRY
#undef MAKE_SEPARATOR
#undef MAKE_ENDISABLE_ENTRY

        GetCursorPos(&curpos);

        if (bExecuteDefault)
        {
            SendMessage( hwnd, WM_COMMAND, uDefaultCommand, 0 );
        }
        else
        {
            TrackPopupMenu(hPopupMenu, 
                /*TPM_TOPALIGN* |*/ TPM_LEFTALIGN /*| TPM_NONOTIFY */
                /* | TPM_RETURNCMD */ | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
                curpos.x, curpos.y, 0, hwnd, 0);
        }
        DestroyMenu(hPopupMenu);
    }

    return 0;
}


/* window procedure */
static long CALLBACK dis_window_proc(HWND hwnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam)
{
	dis_private_t *pdp = (dis_private_t*)GetWindowLong(hwnd, GWL_USERDATA);

	switch (msg)
	{
	case WM_DESTROY:
        delete_client_window(hwnd);
		// clear the dis_private info 
		SetWindowLong( hwnd, GWL_USERDATA, 0 );
		lib_free(pdp);

		return DEF_DIS_PROG(hwnd, msg, wParam, lParam);

#ifdef OPEN_DISASSEMBLY_AS_POPUP

    case WM_ACTIVATE:
        ActivateChild( (LOWORD(wParam)!=WA_INACTIVE) ? TRUE:FALSE, hwnd, mon_disassembly_get_memspace(pdp->pmdp) );
        break;

#else  // #ifdef OPEN_DISASSEMBLY_AS_POPUP

	case WM_MDIACTIVATE:
        ActivateChild( ((HWND) wParam==hwnd)?FALSE:TRUE, hwnd, pdp->memspace );
		break;

#endif // #ifdef OPEN_DISASSEMBLY_AS_POPUP

    case WM_GETWINDOWTYPE:
        {
            LONG* lp = (PLONG) lParam;
            *lp = WT_DISASSEMBLY;
        }
        return 0;

	case WM_CHANGECOMPUTERDRIVE:
		switch (wParam)
		{
		case IDM_MON_COMPUTER:
            mon_disassembly_set_memspace(pdp->pmdp, e_comp_space);
			break;

		case IDM_MON_DRIVE8:
            mon_disassembly_set_memspace(pdp->pmdp, e_disk8_space);
			break;

		case IDM_MON_DRIVE9:
            mon_disassembly_set_memspace(pdp->pmdp, e_disk9_space);
			break;
		}
        SetMemspace( hwnd, mon_disassembly_get_memspace(pdp->pmdp) );
		InvalidateRect(hwnd,NULL,FALSE);
		break;

    case WM_CREATE:
        {
            HDC hdc = GetDC( hwnd );
           	SIZE size;

			pdp = lib_malloc(sizeof(dis_private_t));
			
			/* store pointer to structure with window */
			SetWindowLong( hwnd, GWL_USERDATA, (long) pdp );

	        SelectObject( hdc, GetStockObject( ANSI_FIXED_FONT ) );

            // get height and width of a character
            GetTextExtentPoint32( hdc, " ", 1, &size );
        	pdp->charwidth    = size.cx;
	        pdp->charheight   = size.cy;

            {
            SCROLLINFO ScrollInfo;
            ScrollInfo.cbSize = sizeof(ScrollInfo);
            ScrollInfo.fMask  = SIF_RANGE;
//          GetScrollInfo( hwnd, SB_VERT, &ScrollInfo );

            ScrollInfo.nMin = 0;
            ScrollInfo.nMax = 0x10000;

            SetScrollInfo( hwnd, SB_VERT, &ScrollInfo, FALSE );
            }

			// initialize some window parameter
            pdp->pmdp = mon_disassembly_init();

            {
                SCROLLINFO ScrollInfo;

                ScrollInfo.cbSize = sizeof(ScrollInfo);
                ScrollInfo.fMask  = SIF_POS;
                GetScrollInfo( hwnd, SB_VERT, &ScrollInfo );

                ScrollInfo.nPos   = mon_disassembly_scroll( pdp->pmdp, MON_SCROLL_NOTHING );

                SetScrollInfo( hwnd, SB_VERT, &ScrollInfo, TRUE );
                InvalidateRect( hwnd, NULL, FALSE );
                UpdateWindow( hwnd );
            }
            break;
        }

    case WM_UPDATE:
        mon_disassembly_update(pdp->pmdp);
        return 0;

    case WM_LBUTTONDOWN:
        return ExecuteDisassemblyPopup( hwnd, pdp, lParam, TRUE );

    case WM_RBUTTONDOWN:
        return ExecuteDisassemblyPopup( hwnd, pdp, lParam, FALSE );

    case WM_VSCROLL:
        {
            SCROLLINFO ScrollInfo;
            BOOLEAN    changed = FALSE;

            ScrollInfo.cbSize = sizeof(ScrollInfo);
            ScrollInfo.fMask  = SIF_POS|SIF_TRACKPOS;
            GetScrollInfo( hwnd, SB_VERT, &ScrollInfo );

            ScrollInfo.fMask  = SIF_POS;

            switch ( LOWORD(wParam) )
            {
            case SB_THUMBPOSITION:
        	    return DEF_DIS_PROG(hwnd, msg, wParam, lParam);

            case SB_THUMBTRACK:
                ScrollInfo.nPos = mon_disassembly_scroll_to( pdp->pmdp, (WORD)ScrollInfo.nTrackPos );
                changed         = TRUE;
        	    break;

            case SB_LINEUP:
                ScrollInfo.nPos = mon_disassembly_scroll( pdp->pmdp, MON_SCROLL_UP );
                changed         = TRUE;
                break;

            case SB_PAGEUP:
                ScrollInfo.nPos = mon_disassembly_scroll( pdp->pmdp, MON_SCROLL_PAGE_UP );
                changed         = TRUE;
                break;

            case SB_LINEDOWN:
                ScrollInfo.nPos = mon_disassembly_scroll( pdp->pmdp, MON_SCROLL_DOWN );
                changed         = TRUE;
                break;

            case SB_PAGEDOWN:
                ScrollInfo.nPos = mon_disassembly_scroll( pdp->pmdp, MON_SCROLL_PAGE_DOWN );
                changed         = TRUE;
                break;
            };

            if (changed)
            {
                SetScrollInfo( hwnd, SB_VERT, &ScrollInfo, TRUE );
                InvalidateRect( hwnd, NULL, FALSE );
                UpdateWindow( hwnd );
            }
        }
        break;

    case WM_KEYDOWN:
        switch ((int)wParam) /* nVirtKey */
        {
        case VK_UP:
            SendMessage( hwnd, WM_VSCROLL, SB_LINEUP, 0 );
            return 0;

        case VK_DOWN:
            SendMessage( hwnd, WM_VSCROLL, SB_LINEDOWN, 0 );
            return 0;

        case VK_PRIOR:
            SendMessage( hwnd, WM_VSCROLL, SB_PAGEUP, 0 );
            return 0;

        case VK_NEXT:
            SendMessage( hwnd, WM_VSCROLL, SB_PAGEDOWN, 0 );
            return 0;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_MON_GOTO_PC:
            mon_disassembly_goto_pc( pdp->pmdp );
            break;

        case IDM_MON_GOTO_ADDRESS:
/* @@@@@SRT not yet implemented
            {
                char *result;
                result = uimon_inputaddress( "Please enter the address you want to go to:" );
                if (result)
                {
                    mon_disassembly_goto_string( pdp->pmdp, result );
                    lib_free( result );
                }
            }
*/
            break;

        case IDM_MON_SET_BP:     mon_disassembly_set_breakpoint( pdp->pmdp );     break;
        case IDM_MON_UNSET_BP:   mon_disassembly_unset_breakpoint( pdp->pmdp );   break;
        case IDM_MON_ENABLE_BP:  mon_disassembly_enable_breakpoint( pdp->pmdp );  break;
        case IDM_MON_DISABLE_BP: mon_disassembly_disable_breakpoint( pdp->pmdp ); break;

        case IDM_MON_COMPUTER:
        case IDM_MON_DRIVE8:
        case IDM_MON_DRIVE9:
            SendMessage( hwnd, WM_CHANGECOMPUTERDRIVE, LOWORD(wParam), 0 );
            mon_disassembly_goto_pc( pdp->pmdp );
            break;
        }

        InvalidateRect( hwnd, NULL, FALSE );
        UpdateWindow( hwnd );
        break;

    case WM_PAINT:
        {
            struct mon_disassembly *md_contents = NULL;
            PAINTSTRUCT ps;
			HDC         hdc;
            RECT        rect;
            int         nHeightToPrint;
            COLORREF    crOldTextColor;
            COLORREF    crOldBkColor;
            HPEN        hpenOld;
            HBRUSH      hbrushOld;

            int i;

            typedef enum LINETYPE_S { 
                LT_NORMAL,     
                LT_EXECUTE, 
                LT_EXECUTE_BREAKPOINT,
                LT_EXECUTE_BREAKPOINT_INACTIVE,
                LT_BREAKPOINT, 
                LT_BREAKPOINT_INACTIVE,
                LT_LAST } LINETYPE; 

            const COLORREF crTextLineType[LT_LAST] = 
                { RGB( 0x00, 0x00, 0x00 ), // LT_NORMAL
                  RGB( 0xFF, 0xFF, 0xFF ), // LT_EXECUTE
                  RGB( 0xFF, 0xFF, 0xFF ), // LT_EXECUTE_BREAKPOINT
                  RGB( 0xFF, 0xFF, 0xFF ), // LT_EXECUTE_BREAKPOINT_INACTIVE
                  RGB( 0x00, 0x00, 0x00 ), // LT_BREAKPOINT
                  RGB( 0x00, 0x00, 0x00 )  // LT_BREAKPOINT_INACTIVE
            };

            const COLORREF crBackLineType[LT_LAST] = 
                { RGB( 0xFF, 0xFF, 0xFF ), // LT_NORMAL
                  RGB( 0x00, 0x00, 0xFF ), // LT_EXECUTE
                  RGB( 0x00, 0x80, 0x80 ), // LT_EXECUTE_BREAKPOINT
                  RGB( 0x00, 0x00, 0xFF ), // LT_EXECUTE_BREAKPOINT_INACTIVE
                  RGB( 0xFF, 0x00, 0x00 ), // LT_BREAKPOINT
                  RGB( 0xFF, 0xFF, 0x00 )  // LT_BREAKPOINT_INACTIVE
            };

            HBRUSH hbrushBack[LT_LAST];
            HPEN   hpenBack  [LT_LAST];

            GetClientRect(hwnd,&rect);
            nHeightToPrint = (rect.bottom - rect.top) / pdp->charheight + 1;

			hdc = BeginPaint(hwnd,&ps);

            for (i=0; i<LT_LAST; i++)
            {
                hbrushBack[i] = CreateSolidBrush( crBackLineType[i] );
                hpenBack[i]   = CreatePen( PS_SOLID, 1, crBackLineType[i] );
            }

            crOldTextColor = SetTextColor( hdc, RGB(0xFF,0xFF,0xFF) );
            crOldBkColor   = SetBkColor  ( hdc, RGB(0,0,0) );
            hpenOld        = SelectObject( hdc, GetStockObject( BLACK_PEN   ) );
            hbrushOld      = SelectObject( hdc, GetStockObject( BLACK_BRUSH ) );

            md_contents = mon_disassembly_get_lines( pdp->pmdp, nHeightToPrint, nHeightToPrint-1 );

            for (i=0; i<nHeightToPrint; i++)
            {
                struct mon_disassembly *next = md_contents->next;

                LINETYPE    lt;

                COLORREF crText;
                COLORREF crBack;

                if (md_contents->flags.active_line)
                {
                    if (md_contents->flags.is_breakpoint)
                    {
                        if (md_contents->flags.breakpoint_active)
                            lt = LT_EXECUTE_BREAKPOINT;
                        else
                            lt = LT_EXECUTE_BREAKPOINT_INACTIVE;
                    }
                    else
                        lt = LT_EXECUTE;
                }
                else
                {
                    if (md_contents->flags.is_breakpoint)
                    {
                        if (md_contents->flags.breakpoint_active)
                            lt = LT_BREAKPOINT;
                        else
                            lt = LT_BREAKPOINT_INACTIVE;
                    }
                    else
                        lt = LT_NORMAL;
                }

                crText = crTextLineType[lt];
                crBack = crBackLineType[lt];

                SetTextColor( hdc, crText );
                SetBkColor  ( hdc, crBack );

                TextOut( hdc, 0, i*pdp->charheight, md_contents->content, md_contents->length );

                /* make sure we clear all that is right from the text */
                SelectObject( hdc, hbrushBack[lt] );
                SelectObject( hdc, hpenBack[lt]   );
                Rectangle( hdc, md_contents->length*pdp->charwidth, i*pdp->charheight, rect.right+1, (i+1)*pdp->charheight );

                lib_free(md_contents->content);
                lib_free(md_contents);
                md_contents = next;
            }

            /* restore old settings */
            SelectObject( hdc, hpenOld        );
            SelectObject( hdc, hbrushOld      );
            SetTextColor( hdc, crOldTextColor );
            SetBkColor  ( hdc, crOldBkColor   );

            for (i=0; i<LT_LAST; i++)
            {
                DeleteObject( hbrushBack[i] );
                DeleteObject( hpenBack[i]   );
            }

			EndPaint(hwnd,&ps);
        }
	}

	return DEF_DIS_PROG(hwnd, msg, wParam, lParam);
}


static 
void uimon_init( void )
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

static
BOOL CALLBACK WindowUpdateProc( HWND hwnd, LPARAM lParam )
{
    SendMessage( hwnd, WM_UPDATE, 0, 0 );
	InvalidateRect(hwnd,NULL,FALSE);
	UpdateWindow(hwnd);
	return TRUE;
}
 
static
void UpdateAll(void)
{
    uimon_client_windows_t *p;

	EnumChildWindows(hwndMdiClient,(WNDENUMPROC)WindowUpdateProc,(LPARAM)NULL);

    for (p = first_client_window; p; p=p->next )
        WindowUpdateProc( p->hwnd, 0 );

	InvalidateRect(hwndMonitor,NULL,FALSE);
    UpdateWindow( hwndMonitor );
}

static
BOOL CALLBACK WindowUpdateShown( HWND hwnd, LPARAM lParam )
{
    SendMessage( hwnd, WM_UPDATEVAL, 0, 0 );
	return TRUE;
}
 
static
void update_shown(void)
{
    uimon_client_windows_t *p;

	EnumChildWindows(hwndMdiClient,(WNDENUMPROC)WindowUpdateShown,(LPARAM)NULL);

    for (p = first_client_window; p; p=p->next )
        WindowUpdateShown( p->hwnd, 0 );
}

#endif // #ifdef UIMON_EXPERIMENTAL

void uimon_notify_change()
{
#ifdef UIMON_EXPERIMENTAL
	UpdateAll();
#endif // #ifdef UIMON_EXPERIMENTAL
}



void uimon_window_close( void )
{
#ifdef UIMON_EXPERIMENTAL

    update_shown();

    StoreMonitorDimensions(hwndMonitor);
	DestroyWindow(hwndMonitor);
	hwndMonitor   =
    hwndMdiClient = NULL;

	ResumeFullscreenMode( hwndParent );

#else // #ifdef UIMON_EXPERIMENTAL
    console_close(console_log);

#endif // #ifdef UIMON_EXPERIMENTAL

    console_log = NULL;
}


console_t *uimon_window_open( void )
{

#ifdef UIMON_EXPERIMENTAL

	WindowDimensions *wd;
    int x  = 0;
    int y  = 0;
    int dx = 472;
    int dy = (dx * 3) / 4;

    hwndParent = GetActiveWindow();

	SuspendFullscreenMode( hwndParent );

    uimon_init();

    hwndMonitor = CreateWindow(MONITOR_CLASS,
		"VICE monitor",
		WS_OVERLAPPED|WS_CLIPCHILDREN/*|WS_CLIPSIBLINGS*/|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SIZEBOX,
        x,
        y,
        dx,
        dy,
        NULL, // @SRT GetActiveWindow(),
        NULL,
        winmain_instance,
        NULL);

    wd = LoadMonitorDimensions(hwndMonitor);

    if (!wd)
    {
        OpenConsole(hwndMonitor,TRUE);
    }
    else
    {
        OpenFromWindowDimensions(hwndMonitor,wd);
    }

    if (console_log)
    {
        memcpy( &console_log_for_mon, console_log, sizeof( struct console_s ) );
    }
    else
    {
        /*
         @SRT: Temporary work-around...
        */
        console_log_for_mon.console_xres          = 80;
        console_log_for_mon.console_yres          = 5;
        console_log_for_mon.console_can_stay_open = 1;
    }

    EnableCommands(GetMenu(hwndMonitor),hwndToolbar);

	SetActiveWindow( hwndMonitor );

	ShowWindow( hwndMonitor, SW_SHOW );

    return &console_log_for_mon;

#else // #ifdef UIMON_EXPERIMENTAL

    console_log = console_open("Monitor");
    return console_log;

#endif // #ifdef UIMON_EXPERIMENTAL
}

void uimon_window_suspend( void )
{
#ifdef UIMON_EXPERIMENTAL

    update_shown();
    
#else // #ifdef UIMON_EXPERIMENTAL

    uimon_window_close();

#endif // #ifdef UIMON_EXPERIMENTAL
}

console_t *uimon_window_resume( void )
{
#ifdef UIMON_EXPERIMENTAL

	pchCommandLine = NULL;

    return &console_log_for_mon;

#else // #ifdef UIMON_EXPERIMENTAL

    return uimon_window_open();

#endif // #ifdef UIMON_EXPERIMENTAL
}

int uimon_out(const char *format, ...)
{
    va_list ap;
    char *buffer;
    int   rc = 0;

    if (console_log)
    {
        va_start(ap, format);
        buffer = lib_mvsprintf(format, ap);
        rc = console_out(console_log, buffer);
        lib_free(buffer);
    }
    return rc;
}

char *uimon_get_in(char **ppchCommandLine, const char *prompt)
{
#ifdef UIMON_EXPERIMENTAL
    char *p = NULL;

    if (console_log)
    {
        /* we have a console, so try to input data from there... */
        p = console_in(console_log, prompt);
    }
    else
    {
        /* we don't have a console, make sure we can do something useful
           by dispatching the events
        */
        while (!*ppchCommandLine && !console_log)
        {
            ui_dispatch_next_event();
        }
    }
    return p;
#else // #ifdef UIMON_EXPERIMENTAL
    return console_in(console_log, prompt);
#endif // #ifdef UIMON_EXPERIMENTAL
}
