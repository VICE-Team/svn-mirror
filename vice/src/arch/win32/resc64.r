//Microsoft Developer Studio generated resource script.
//
#include "resc64.h"

/////////////////////////////////////////////////////////////////////////////
//
// Icon
//

// Icon with lowest ID value placed first to ensure application icon
// remains consistent on all systems.
IDI_ICON1               ICON    DISCARDABLE     "c64.ico"



//#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 2 resource.
//
//#include "afxres.h"

/////////////////////////////////////////////////////////////////////////////
//#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// English (U.S.) resources

#define DS_MODALFRAME		0x80L
#define LANG_ENGLISH		0x09
#define SUBLANG_ENGLISH_US	0x01
#define WS_POPUP		0x80000000L
#define WS_CAPTION		0x00c00000L
#define WS_VSCROLL		0x00200000L
#define WS_SYSMENU		0x00080000L
#define WS_GROUP		0x00020000L
#define WS_TABSTOP		0x00010000L
#define IDOK			1
#define IDC_STATIC		(-1)
#define BS_AUTOCHECKBOX		0x00000003L
#define BS_AUTORADIOBUTTON	0x00000009L
#define CBS_DROPDOWN		0x0002L
#define CBS_SORT		0x0100L

//#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
//#ifdef _WIN32
LANGUAGE LANG_ENGLISH, SUBLANG_ENGLISH_US
#pragma code_page(1252)
//#endif //_WIN32

//#ifdef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE
//

//1 TEXTINCLUDE DISCARDABLE
//BEGIN
//    "resource.h\0"
//END

//2 TEXTINCLUDE DISCARDABLE
//BEGIN
//    "#include ""afxres.h""\r\n"
//    "\0"
//END

//3 TEXTINCLUDE DISCARDABLE
//BEGIN
//    "\r\n"
//    "\0"
//END

//#endif    // APSTUDIO_INVOKED


/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

IDR_MENU MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "Device &manager...",          IDM_DEVICEMANAGER
        MENUITEM SEPARATOR
        POPUP "Attach &disk image"
        BEGIN
            MENUITEM "Drive &8",                    IDM_ATTACH_8
            MENUITEM "Drive &9",                    IDM_ATTACH_9
            MENUITEM "Drive 1&0",                   IDM_ATTACH_10
            MENUITEM "Drive &11",                   IDM_ATTACH_11
        END
        POPUP "D&etach disk image"
        BEGIN
            MENUITEM "Drive &8",                    IDM_DETACH_8
            MENUITEM "Drive &9",                    IDM_DETACH_9
            MENUITEM "Drive 1&0",                   IDM_DETACH_10
            MENUITEM "Drive &11",                   IDM_DETACH_11
            MENUITEM SEPARATOR
            MENUITEM "&All",                        IDM_DETACH_ALL
        END
        MENUITEM SEPARATOR
        MENUITEM "Attach &tape image...",       IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",          IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        MENUITEM "&Autostart disk/tape image...", IDM_AUTOSTART
        MENUITEM SEPARATOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard",                       IDM_HARD_RESET
            MENUITEM "&Soft",                       IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit",                       IDM_EXIT
    END
    POPUP "&Options"
    BEGIN
        MENUITEM "Video &cache",                IDM_TOGGLE_VIDEOCACHE
        , CHECKED
        MENUITEM "&Double size",                IDM_TOGGLE_DOUBLESIZE
        , CHECKED
        MENUITEM "D&ouble scan",                IDM_TOGGLE_DOUBLESCAN
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&Sound playback",             IDM_TOGGLE_SOUND, CHECKED
        MENUITEM "SID &filters",                IDM_TOGGLE_SIDFILTERS
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&True Drive Emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        MENUITEM "D&rive Settings...",          IDM_DRIVE_SETTINGS
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",                   IDM_ABOUT
    END
END


/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_ABOUT DIALOG DISCARDABLE  0, 0, 193, 90
STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
CAPTION "About VICE"
FONT 8, "MS Sans Serif"
BEGIN
    DEFPUSHBUTTON   "OK",IDOK,135,8,50,14
    CTEXT           "VICE",IDC_STATIC,7,8,118,8
    CTEXT           "Versatile Commodore Emulator",IDC_STATIC,7,21,118,8
    CTEXT           "Windows port by Ettore Perazzoli",IDC_STATIC,7,60,118,8
    CTEXT           "(ettore@comm2000.it)",IDC_STATIC,7,73,118,8
    CTEXT           "0.14.2.0",IDC_ABOUT_VERSION,7,34,118,8
END

IDD_DISKDEVICE_DIALOG DIALOG DISCARDABLE  0, 0, 307, 210
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    AUTORADIOBUTTON "Disk &image",IDC_SELECTDISK,15,20,51,10,BS_AUTORADIOBUTTON |
                    WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "&Directory",IDC_SELECTDIR,15,75,44,10,BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "&None",IDC_SELECTNONE,15,180,33,10,BS_AUTORADIOBUTTON
    COMBOBOX        IDC_DISKIMAGE,81,19,210,12,CBS_DROPDOWN | CBS_SORT |
                    WS_VSCROLL | WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&Browse...",IDC_BROWSEDISK,81,37,50,14,WS_GROUP
    PUSHBUTTON      "&Autostart",IDC_AUTOSTART,241,37,50,14,WS_GROUP
    COMBOBOX        IDC_DIR,81,74,210,12,CBS_DROPDOWN | CBS_SORT |
                    WS_VSCROLL | WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&Browse...",IDC_BROWSEDIR,81,92,50,14,WS_GROUP
    GROUPBOX        "&Options",IDC_STATIC,195,90,95,63
    AUTOCHECKBOX    "&Read P00 files",IDC_TOGGLE_READP00,205,105,62,10,
                    BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
    AUTOCHECKBOX    "&Write P00 files",IDC_TOGGLE_WRITEP00,205,120,61,10,
                    BS_AUTOCHECKBOX | WS_TABSTOP
    AUTOCHECKBOX    "&Hide non-P00 files",IDC_TOGGLE_HIDENONP00,205,135,73,10,
                    BS_AUTOCHECKBOX | WS_TABSTOP
END

IDD_DRIVE_SETTINGS_DIALOG DIALOG DISCARDABLE  0, 0, 307, 210
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&Drive type",IDC_STATIC,5,5,70,100
    AUTORADIOBUTTON "&None",IDC_SELECT_DRIVE_TYPE_NONE,15,20,51,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "15&41",IDC_SELECT_DRIVE_TYPE_1541,15,35,51,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "15&71",IDC_SELECT_DRIVE_TYPE_1571,15,50,51,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "15&81",IDC_SELECT_DRIVE_TYPE_1581,15,65,51,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "20&31",IDC_SELECT_DRIVE_TYPE_2031,15,80,51,10,
                            BS_AUTORADIOBUTTON
END


/////////////////////////////////////////////////////////////////////////////
//
// DESIGNINFO
//

//#ifdef APSTUDIO_INVOKED
//GUIDELINES DESIGNINFO DISCARDABLE
//BEGIN
//    IDD_ABOUT, DIALOG
//    BEGIN
//        LEFTMARGIN, 6
//        RIGHTMARGIN, 184
//        TOPMARGIN, 7
//        BOTTOMMARGIN, 81
//    END

//    IDD_DISKDEVICE_DIALOG, DIALOG
//    BEGIN
//        LEFTMARGIN, 7
//        RIGHTMARGIN, 300
//        TOPMARGIN, 7
//        BOTTOMMARGIN, 203
//    END
//END
//#endif    // APSTUDIO_INVOKED

//#endif    // English (U.S.) resources
/////////////////////////////////////////////////////////////////////////////



//#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//


/////////////////////////////////////////////////////////////////////////////
//#endif    // not APSTUDIO_INVOKED

