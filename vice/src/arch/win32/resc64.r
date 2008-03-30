//Microsoft Developer Studio generated resource script.
//
#include "resc64.h"
#include "../../config.h"

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

IDR_MENUC64 MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "&Autostart disk/tape image...",   IDM_AUTOSTART
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
        MENUITEM "Attach &tape image...",           IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        POPUP "Attach &cartridge image..."          
        BEGIN
            MENUITEM "&CRT image...",               IDM_CART_ATTACH_CRT
            MENUITEM SEPARATOR
            MENUITEM "Generic &8KB image...",       IDM_CART_ATTACH_8KB
            MENUITEM "Generic &16KB image...",      IDM_CART_ATTACH_16KB
            MENUITEM "&Action Replay image...",     IDM_CART_ATTACH_AR
            MENUITEM "&Super Snapshot 4 image...",  IDM_CART_ATTACH_SS4
            MENUITEM SEPARATOR
            MENUITEM "Set cartridge as &default",   IDM_CART_SET_DEFAULT
        END
        MENUITEM "Detach cartridge &image",         IDM_CART_DETACH
        MENUITEM SEPARATOR
        MENUITEM "Load snapshot image...",          IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image",             IDM_SNAPSHOT_SAVE
        MENUITEM SEPARATOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard",                       IDM_HARD_RESET
            MENUITEM "&Soft",                       IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit",                           IDM_EXIT
    END
    POPUP "&Options"
    BEGIN
        POPUP "&Refresh rate"
        BEGIN
            MENUITEM "&Auto",                       IDM_REFRESH_RATE_AUTO
            MENUITEM "1/&1",                        IDM_REFRESH_RATE_1
            MENUITEM "1/&2",                        IDM_REFRESH_RATE_2
            MENUITEM "1/&3",                        IDM_REFRESH_RATE_3
            MENUITEM "1/&4",                        IDM_REFRESH_RATE_4
            MENUITEM "1/&5",                        IDM_REFRESH_RATE_5
            MENUITEM "1/&6",                        IDM_REFRESH_RATE_6
            MENUITEM "1/&7",                        IDM_REFRESH_RATE_7
            MENUITEM "1/&8",                        IDM_REFRESH_RATE_8
            MENUITEM "1/&9",                        IDM_REFRESH_RATE_9
            MENUITEM "1/1&0",                       IDM_REFRESH_RATE_10
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_REFRESH_RATE_CUSTOM
#endif
        END
        POPUP "&Maximum Speed"
        BEGIN
            MENUITEM "&200%",                       IDM_MAXIMUM_SPEED_200
            MENUITEM "&100%",                       IDM_MAXIMUM_SPEED_100
            MENUITEM "&50%",                        IDM_MAXIMUM_SPEED_50
            MENUITEM "&20%",                        IDM_MAXIMUM_SPEED_20
            MENUITEM "1&0%",                        IDM_MAXIMUM_SPEED_10
            MENUITEM "No &limit",                   IDM_MAXIMUM_SPEED_NO_LIMIT
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_MAXIMUM_SPEED_CUSTOM
#endif
        END
        MENUITEM "&Warp Mode",                      IDM_TOGGLE_WARP_MODE
            , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "Video &cache",                    IDM_TOGGLE_VIDEOCACHE
        , CHECKED
        MENUITEM "&Double size",                    IDM_TOGGLE_DOUBLESIZE
        , CHECKED
        MENUITEM "D&ouble scan",                    IDM_TOGGLE_DOUBLESCAN
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&Sound playback",                 IDM_TOGGLE_SOUND, CHECKED
#ifdef HAVE_RESID
        MENUITEM "&Enable resid",                   IDM_TOGGLE_SOUND_RESID
        , CHECKED
#endif
        MENUITEM "SID &filters",                    IDM_TOGGLE_SIDFILTERS
        , CHECKED
    END
    POPUP "&Settings"
    BEGIN
        MENUITEM "&True Drive Emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        MENUITEM "&Drive Settings...",              IDM_DRIVE_SETTINGS
        MENUITEM SEPARATOR
        MENUITEM "Device &manager...",              IDM_DEVICEMANAGER
        MENUITEM SEPARATOR
        MENUITEM "&Save current settings",          IDM_SETTINGS_SAVE
        MENUITEM "&Load saved settings",            IDM_SETTINGS_LOAD
        MENUITEM "Set de&fault settings",           IDM_SETTINGS_DEFAULT
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",                       IDM_ABOUT
    END
END

IDR_MENUC128 MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "&Autostart disk/tape image...",   IDM_AUTOSTART
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
        MENUITEM "Attach &tape image...",           IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        MENUITEM "Load snapshot image...",          IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image",             IDM_SNAPSHOT_SAVE
        MENUITEM SEPARATOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard",                       IDM_HARD_RESET
            MENUITEM "&Soft",                       IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit",                           IDM_EXIT
    END
    POPUP "&Options"
    BEGIN
        POPUP "&Refresh rate"
        BEGIN
            MENUITEM "&Auto",                       IDM_REFRESH_RATE_AUTO
            MENUITEM "1/&1",                        IDM_REFRESH_RATE_1
            MENUITEM "1/&2",                        IDM_REFRESH_RATE_2
            MENUITEM "1/&3",                        IDM_REFRESH_RATE_3
            MENUITEM "1/&4",                        IDM_REFRESH_RATE_4
            MENUITEM "1/&5",                        IDM_REFRESH_RATE_5
            MENUITEM "1/&6",                        IDM_REFRESH_RATE_6
            MENUITEM "1/&7",                        IDM_REFRESH_RATE_7
            MENUITEM "1/&8",                        IDM_REFRESH_RATE_8
            MENUITEM "1/&9",                        IDM_REFRESH_RATE_9
            MENUITEM "1/1&0",                       IDM_REFRESH_RATE_10
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_REFRESH_RATE_CUSTOM
#endif
        END
        POPUP "&Maximum Speed"
        BEGIN
            MENUITEM "&200%",                       IDM_MAXIMUM_SPEED_200
            MENUITEM "&100%",                       IDM_MAXIMUM_SPEED_100
            MENUITEM "&50%",                        IDM_MAXIMUM_SPEED_50
            MENUITEM "&20%",                        IDM_MAXIMUM_SPEED_20
            MENUITEM "1&0%",                        IDM_MAXIMUM_SPEED_10
            MENUITEM "No &limit",                   IDM_MAXIMUM_SPEED_NO_LIMIT
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_MAXIMUM_SPEED_CUSTOM
#endif
        END
        MENUITEM "&Warp Mode",                      IDM_TOGGLE_WARP_MODE
            , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "Video &cache",                    IDM_TOGGLE_VIDEOCACHE
        , CHECKED
        MENUITEM "&Double size",                    IDM_TOGGLE_DOUBLESIZE
        , CHECKED
        MENUITEM "D&ouble scan",                    IDM_TOGGLE_DOUBLESCAN
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&Sound playback",                 IDM_TOGGLE_SOUND, CHECKED
#ifdef HAVE_RESID
        MENUITEM "Enable &resid",                   IDM_TOGGLE_SOUND_RESID
        , CHECKED
#endif
        MENUITEM "SID &filters",                    IDM_TOGGLE_SIDFILTERS
        , CHECKED
    END
    POPUP "&Settings"
    BEGIN
        MENUITEM "&True Drive Emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        MENUITEM "D&rive Settings...",              IDM_DRIVE_SETTINGS
        MENUITEM SEPARATOR
        MENUITEM "Device &manager...",              IDM_DEVICEMANAGER
        MENUITEM SEPARATOR
        MENUITEM "&Save current settings",          IDM_SETTINGS_SAVE
        MENUITEM "&Load saved settings",            IDM_SETTINGS_LOAD
        MENUITEM "Set de&fault settings",           IDM_SETTINGS_DEFAULT
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",                       IDM_ABOUT
    END
END

IDR_MENUVIC MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "&Autostart disk/tape image...",   IDM_AUTOSTART
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
        MENUITEM "Attach &tape image...",           IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        POPUP "Attach &cartridge image..."
        BEGIN
            MENUITEM "4/8KB image at $&2000...",    IDM_CART_VIC20_8KB_2000
            MENUITEM "4/8KB image at $&6000...",    IDM_CART_VIC20_8KB_6000
            MENUITEM "4/8KB image at $&A000...",    IDM_CART_VIC20_8KB_A000
            MENUITEM "4KB image at $&B000...",      IDM_CART_VIC20_4KB_B000
        END
        MENUITEM "Detach c&artridge image",         IDM_CART_DETACH
        MENUITEM SEPARATOR
        MENUITEM "Load snapshot image...",          IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image",             IDM_SNAPSHOT_SAVE
        MENUITEM SEPARATOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard",                       IDM_HARD_RESET
            MENUITEM "&Soft",                       IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit",                           IDM_EXIT
    END
    POPUP "&Options"
    BEGIN
        POPUP "&Refresh rate"
        BEGIN
            MENUITEM "&Auto",                       IDM_REFRESH_RATE_AUTO
            MENUITEM "1/&1",                        IDM_REFRESH_RATE_1
            MENUITEM "1/&2",                        IDM_REFRESH_RATE_2
            MENUITEM "1/&3",                        IDM_REFRESH_RATE_3
            MENUITEM "1/&4",                        IDM_REFRESH_RATE_4
            MENUITEM "1/&5",                        IDM_REFRESH_RATE_5
            MENUITEM "1/&6",                        IDM_REFRESH_RATE_6
            MENUITEM "1/&7",                        IDM_REFRESH_RATE_7
            MENUITEM "1/&8",                        IDM_REFRESH_RATE_8
            MENUITEM "1/&9",                        IDM_REFRESH_RATE_9
            MENUITEM "1/1&0",                       IDM_REFRESH_RATE_10
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_REFRESH_RATE_CUSTOM
#endif
        END
        POPUP "&Maximum Speed"
        BEGIN
            MENUITEM "&200%",                       IDM_MAXIMUM_SPEED_200
            MENUITEM "&100%",                       IDM_MAXIMUM_SPEED_100
            MENUITEM "&50%",                        IDM_MAXIMUM_SPEED_50
            MENUITEM "&20%",                        IDM_MAXIMUM_SPEED_20
            MENUITEM "1&0%",                        IDM_MAXIMUM_SPEED_10
            MENUITEM "No &limit",                   IDM_MAXIMUM_SPEED_NO_LIMIT
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_MAXIMUM_SPEED_CUSTOM
#endif
        END
        MENUITEM "&Warp Mode",                      IDM_TOGGLE_WARP_MODE
            , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "Video &cache",                    IDM_TOGGLE_VIDEOCACHE
        , CHECKED
        MENUITEM "&Double size",                    IDM_TOGGLE_DOUBLESIZE
        , CHECKED
        MENUITEM "D&ouble scan",                    IDM_TOGGLE_DOUBLESCAN
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&Sound playback",                 IDM_TOGGLE_SOUND, CHECKED
        MENUITEM "SID &filters",                    IDM_TOGGLE_SIDFILTERS
        , CHECKED
    END
    POPUP "&Settings"
    BEGIN
        MENUITEM "&True Drive Emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        MENUITEM "D&rive Settings...",              IDM_DRIVE_SETTINGS
        MENUITEM SEPARATOR
        MENUITEM "Device &manager...",              IDM_DEVICEMANAGER
        MENUITEM SEPARATOR
        MENUITEM "&Save current settings",          IDM_SETTINGS_SAVE
        MENUITEM "&Load saved settings",            IDM_SETTINGS_LOAD
        MENUITEM "Set de&fault settings",           IDM_SETTINGS_DEFAULT
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",                       IDM_ABOUT
    END
END

IDR_MENUPET MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "&Autostart disk/tape image...",   IDM_AUTOSTART
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
        MENUITEM "Attach &tape image...",           IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        MENUITEM "Load snapshot image...",          IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image",             IDM_SNAPSHOT_SAVE
        MENUITEM SEPARATOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard",                       IDM_HARD_RESET
            MENUITEM "&Soft",                       IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit",                           IDM_EXIT
    END
    POPUP "&Options"
    BEGIN
        POPUP "&Refresh rate"
        BEGIN
            MENUITEM "&Auto",                       IDM_REFRESH_RATE_AUTO
            MENUITEM "1/&1",                        IDM_REFRESH_RATE_1
            MENUITEM "1/&2",                        IDM_REFRESH_RATE_2
            MENUITEM "1/&3",                        IDM_REFRESH_RATE_3
            MENUITEM "1/&4",                        IDM_REFRESH_RATE_4
            MENUITEM "1/&5",                        IDM_REFRESH_RATE_5
            MENUITEM "1/&6",                        IDM_REFRESH_RATE_6
            MENUITEM "1/&7",                        IDM_REFRESH_RATE_7
            MENUITEM "1/&8",                        IDM_REFRESH_RATE_8
            MENUITEM "1/&9",                        IDM_REFRESH_RATE_9
            MENUITEM "1/1&0",                       IDM_REFRESH_RATE_10
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_REFRESH_RATE_CUSTOM
#endif
        END
        POPUP "&Maximum Speed"
        BEGIN
            MENUITEM "&200%",                       IDM_MAXIMUM_SPEED_200
            MENUITEM "&100%",                       IDM_MAXIMUM_SPEED_100
            MENUITEM "&50%",                        IDM_MAXIMUM_SPEED_50
            MENUITEM "&20%",                        IDM_MAXIMUM_SPEED_20
            MENUITEM "1&0%",                        IDM_MAXIMUM_SPEED_10
            MENUITEM "No &limit",                   IDM_MAXIMUM_SPEED_NO_LIMIT
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_MAXIMUM_SPEED_CUSTOM
#endif
        END
        MENUITEM "&Warp Mode",                      IDM_TOGGLE_WARP_MODE
            , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "Video &cache",                    IDM_TOGGLE_VIDEOCACHE
        , CHECKED
        MENUITEM "&Double size",                    IDM_TOGGLE_DOUBLESIZE
        , CHECKED
        MENUITEM "D&ouble scan",                    IDM_TOGGLE_DOUBLESCAN
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&Sound playback",                 IDM_TOGGLE_SOUND, CHECKED
        MENUITEM "SID &filters",                    IDM_TOGGLE_SIDFILTERS
        , CHECKED
    END
    POPUP "&Settings"
    BEGIN
        MENUITEM "&True Drive Emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        MENUITEM "D&rive Settings...",              IDM_DRIVE_SETTINGS
        MENUITEM SEPARATOR
        MENUITEM "Device &manager...",              IDM_DEVICEMANAGER
        MENUITEM SEPARATOR
        MENUITEM "&Save current settings",          IDM_SETTINGS_SAVE
        MENUITEM "&Load saved settings",            IDM_SETTINGS_LOAD
        MENUITEM "Set de&fault settings",           IDM_SETTINGS_DEFAULT
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",                       IDM_ABOUT
    END
END

IDR_MENUCBM2 MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "&Autostart disk/tape image...",   IDM_AUTOSTART
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
        MENUITEM "Attach &tape image...",           IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        MENUITEM "Load snapshot image...",          IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image",             IDM_SNAPSHOT_SAVE
        MENUITEM SEPARATOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard",                       IDM_HARD_RESET
            MENUITEM "&Soft",                       IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit",                           IDM_EXIT
    END
    POPUP "&Options"
    BEGIN
        POPUP "&Refresh rate"
        BEGIN
            MENUITEM "&Auto",                       IDM_REFRESH_RATE_AUTO
            MENUITEM "1/&1",                        IDM_REFRESH_RATE_1
            MENUITEM "1/&2",                        IDM_REFRESH_RATE_2
            MENUITEM "1/&3",                        IDM_REFRESH_RATE_3
            MENUITEM "1/&4",                        IDM_REFRESH_RATE_4
            MENUITEM "1/&5",                        IDM_REFRESH_RATE_5
            MENUITEM "1/&6",                        IDM_REFRESH_RATE_6
            MENUITEM "1/&7",                        IDM_REFRESH_RATE_7
            MENUITEM "1/&8",                        IDM_REFRESH_RATE_8
            MENUITEM "1/&9",                        IDM_REFRESH_RATE_9
            MENUITEM "1/1&0",                       IDM_REFRESH_RATE_10
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_REFRESH_RATE_CUSTOM
#endif
        END
        POPUP "&Maximum Speed"
        BEGIN
            MENUITEM "&200%",                       IDM_MAXIMUM_SPEED_200
            MENUITEM "&100%",                       IDM_MAXIMUM_SPEED_100
            MENUITEM "&50%",                        IDM_MAXIMUM_SPEED_50
            MENUITEM "&20%",                        IDM_MAXIMUM_SPEED_20
            MENUITEM "1&0%",                        IDM_MAXIMUM_SPEED_10
            MENUITEM "No &limit",                   IDM_MAXIMUM_SPEED_NO_LIMIT
#if 0
            MENUITEM SEPARATOR
            MENUITEM "&Custom",                     IDM_MAXIMUM_SPEED_CUSTOM
#endif
        END
        MENUITEM "&Warp Mode",                      IDM_TOGGLE_WARP_MODE
            , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "Video &cache",                    IDM_TOGGLE_VIDEOCACHE
        , CHECKED
        MENUITEM "&Double size",                    IDM_TOGGLE_DOUBLESIZE
        , CHECKED
        MENUITEM "D&ouble scan",                    IDM_TOGGLE_DOUBLESCAN
        , CHECKED
        MENUITEM SEPARATOR
        MENUITEM "&Sound playback",                 IDM_TOGGLE_SOUND, CHECKED
        MENUITEM "SID &filters",                    IDM_TOGGLE_SIDFILTERS
        , CHECKED
    END
    POPUP "&Settings"
    BEGIN
        MENUITEM "&True Drive Emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        MENUITEM "D&rive Settings...",              IDM_DRIVE_SETTINGS
        MENUITEM SEPARATOR
        MENUITEM "Device &manager...",              IDM_DEVICEMANAGER
        MENUITEM SEPARATOR
        MENUITEM "&Save current settings",          IDM_SETTINGS_SAVE
        MENUITEM "&Load saved settings",            IDM_SETTINGS_LOAD
        MENUITEM "Set de&fault settings",           IDM_SETTINGS_DEFAULT
    END
    POPUP "&Help"
    BEGIN
        MENUITEM "&About...",                       IDM_ABOUT
    END
END

/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_ABOUT DIALOG DISCARDABLE  0, 0, 195, 147
STYLE DS_MODALFRAME | WS_CAPTION | WS_SYSMENU
CAPTION "About VICE"
FONT 8, "MS Sans Serif"
BEGIN
    DEFPUSHBUTTON "OK",IDOK,132,14,50,14
    CTEXT    "VICE",IDC_STATIC,7,5,118,8
    CTEXT    "Versatile Commodore Emulator",IDC_STATIC,7,18,118,8
    CTEXT    "Preview " VERSION "- ALPHA version",IDC_STATIC,7,31,118,8
    CTEXT    "Copyright (c) 1996-1998 Ettore Perazzoli",IDC_STATIC,16,50,158,8
    CTEXT    "Copyright (c) 1996-1998 André Fachat",IDC_STATIC,16,59,158,8
    CTEXT    "Copyright (c) 1993-1994, 1997-1998 Teemu Rantanen",IDC_STATIC,5,68,180,8
    CTEXT    "Copyright (c) 1997-1998 Daniel Sladic",IDC_STATIC,16,77,158,8
    CTEXT    "Copyright (c) 1998 Andreas Boose",IDC_STATIC,36,86,118,8
    CTEXT    "Copyright (c) 1993-1996 Jouko Valta",IDC_STATIC,36,95,118,8
    CTEXT    "Copyright (c) 1993-1994 Jarkko Sonninen",IDC_STATIC,16,104,158,8

    CTEXT    "Initial Windows port by Ettore Perazzoli",IDC_STATIC,36,122,118,8
    CTEXT    "(ettore@comm2000.it)",IDC_STATIC,36,131,118,8
END

IDD_DISKDEVICE_DIALOG DIALOG DISCARDABLE  0, 0, 307, 190
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    AUTORADIOBUTTON "Disk &image",IDC_SELECTDISK,15,20,51,10,BS_AUTORADIOBUTTON |
                    WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "&Directory",IDC_SELECTDIR,15,75,44,10,BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "&None",IDC_SELECTNONE,15,160,33,10,BS_AUTORADIOBUTTON
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

IDD_DRIVE_SETTINGS_DIALOG DIALOG DISCARDABLE  0, 0, 250, 120
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&Drive type",IDC_STATIC,5,15,60,90
    AUTORADIOBUTTON "1541",IDC_SELECT_DRIVE_TYPE_1541,15,30,35,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "1571",IDC_SELECT_DRIVE_TYPE_1571,15,45,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "1581",IDC_SELECT_DRIVE_TYPE_1581,15,60,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "2031",IDC_SELECT_DRIVE_TYPE_2031,15,75,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "None",IDC_SELECT_DRIVE_TYPE_NONE,15,90,35,10,
                            BS_AUTORADIOBUTTON
    GROUPBOX        "&40 track handling",IDC_STATIC,75,15,90,60
    AUTORADIOBUTTON "Never extend",IDC_SELECT_DRIVE_EXTEND_NEVER,85,30,65,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "Ask on extend",IDC_SELECT_DRIVE_EXTEND_ASK,85,45,65,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "Extend on access",IDC_SELECT_DRIVE_EXTEND_ACCESS,
                            85,60,65,10, BS_AUTORADIOBUTTON
    GROUPBOX        "&Idle method",IDC_STATIC,175,15,70,60
    AUTORADIOBUTTON "None",IDC_SELECT_DRIVE_IDLE_NO_IDLE,185,30,51,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "Trap idle",IDC_SELECT_DRIVE_IDLE_TRAP_IDLE,185,45,51,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "Skip cycles",IDC_SELECT_DRIVE_IDLE_SKIP_CYCLES,
                            185,60,51,10,BS_AUTORADIOBUTTON
    AUTOCHECKBOX    "Enable &parallel cable",IDC_TOGGLE_DRIVE_PARALLEL_CABLE,
                    85,90,80,10, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
END

IDD_SNAPSHOT_SAVE_DIALOG DIALOG DISCARDABLE  0, 0, 220, 95 
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    COMBOBOX        IDC_SNAPSHOT_SAVE_IMAGE,5,15,210,12,CBS_DROPDOWN |
                    CBS_SORT | WS_VSCROLL | WS_GROUP | WS_TABSTOP
    GROUPBOX        "&Options",IDC_STATIC,5,35,150,45
    AUTOCHECKBOX    "Save currently attached &disks images",
                    IDC_TOGGLE_SNAPSHOT_SAVE_DISKS,
                    15,50,135,10, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
    AUTOCHECKBOX    "Save currently loaded &ROM images",
                    IDC_TOGGLE_SNAPSHOT_SAVE_ROMS,
                    15,65,135,10, BS_AUTOCHECKBOX | WS_TABSTOP
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

