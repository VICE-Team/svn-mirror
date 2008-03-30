// PET specific resources.
//
#include "res.r"

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

IDR_MENUPET MENU DISCARDABLE
BEGIN
    POPUP "&File"
    BEGIN
        MENUITEM "&Autostart disk/tape image...",   IDM_AUTOSTART
        MENUITEM SEPARATOR
        POPUP "Attach &disk image"
        BEGIN
            MENUITEM "Drive &8\tAlt+8",             IDM_ATTACH_8
            MENUITEM "Drive &9\tAlt+9",             IDM_ATTACH_9
            MENUITEM "Drive 1&0\tAlt+0",            IDM_ATTACH_10
            MENUITEM "Drive &11\tAlt+1",            IDM_ATTACH_11
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
        MENUITEM "Attach &tape image...\tAlt+T",    IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        MENUITEM SEPARATOR
        MENUITEM "Load snapshot image...\tAlt+L",   IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image\tAlt+S",      IDM_SNAPSHOT_SAVE
        MENUITEM "Load quicksnapshot image\tCtrl+Alt+L", IDM_LOADQUICK
        MENUITEM "Save quicksnapshot image\tCtrl+Alt+S", IDM_SAVEQUICK
        MENUITEM SEPARATOR
        MENUITEM "Monitor\tAlt+M",                  IDM_MONITOR
        POPUP "&Reset"
        BEGIN
            MENUITEM "&Hard\tCtrl+Alt+R",           IDM_HARD_RESET
            MENUITEM "&Soft\tAlt+R",                IDM_SOFT_RESET
        END
        MENUITEM SEPARATOR
        MENUITEM "E&xit\tAlt+X",                    IDM_EXIT
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
        MENUITEM "&Warp mode\tAlt+W",               IDM_TOGGLE_WARP_MODE
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
        MENUITEM SEPARATOR
        MENUITEM "&True drive emulation",       IDM_TOGGLE_DRIVE_TRUE_EMULATION, CHECKED
        POPUP "Drive s&ync factor"
        BEGIN
            MENUITEM "&PAL",                        IDM_SYNC_FACTOR_PAL
            MENUITEM "&NTSC",                       IDM_SYNC_FACTOR_NTSC
        END
    END
    POPUP "&Settings"
    BEGIN
        MENUITEM "&PET settings...",                IDM_PET_SETTINGS
        MENUITEM "&Device settings...",             IDM_DEVICEMANAGER
        MENUITEM "D&rive settings...",              IDM_DRIVE_SETTINGS
        MENUITEM "&Joystick settings...",           IDM_JOY_SETTINGS
        MENUITEM "S&ound settings...",              IDM_SOUND_SETTINGS
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

IDD_PET_SETTINGS_MODEL_DIALOG DIALOG DISCARDABLE  0, 0, 180, 225
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&Machine defaults",IDC_STATIC,5,15,70,201
    PUSHBUTTON "PET 2001-8N",IDC_SELECT_PET_2001_8N,15,28,50,14, WS_GROUP
    PUSHBUTTON "PET 3008",   IDC_SELECT_PET_3008,   15,43,50,14, WS_GROUP
    PUSHBUTTON "PET 3016",   IDC_SELECT_PET_3016,   15,58,50,14, WS_GROUP
    PUSHBUTTON "PET 3032",   IDC_SELECT_PET_3032,   15,73,50,14, WS_GROUP
    PUSHBUTTON "PET 3032B",  IDC_SELECT_PET_3032B,  15,88,50,14, WS_GROUP
    PUSHBUTTON "PET 4016",   IDC_SELECT_PET_4016,   15,103,50,14, WS_GROUP
    PUSHBUTTON "PET 4032",   IDC_SELECT_PET_4032,   15,118,50,14, WS_GROUP
    PUSHBUTTON "PET 4032B",  IDC_SELECT_PET_4032B,  15,133,50,14, WS_GROUP
    PUSHBUTTON "PET 8032",   IDC_SELECT_PET_8032,   15,148,50,14, WS_GROUP
    PUSHBUTTON "PET 8096",   IDC_SELECT_PET_8096,   15,163,50,14, WS_GROUP
    PUSHBUTTON "PET 8296",   IDC_SELECT_PET_8296,   15,178,50,14, WS_GROUP
    PUSHBUTTON "SuperPET",   IDC_SELECT_PET_SUPER,  15,193,50,14, WS_GROUP
    GROUPBOX        "&Memory",IDC_STATIC,95,15,65,105
    AUTORADIOBUTTON "4KB",  IDC_SELECT_PET_MEM4K,  105,30,45,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "8KB",  IDC_SELECT_PET_MEM8K,  105,45,45,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "16KB", IDC_SELECT_PET_MEM16K, 105,60,45,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "32KB", IDC_SELECT_PET_MEM32K, 105,75,45,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "96KB", IDC_SELECT_PET_MEM96K, 105,90,45,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "128KB",IDC_SELECT_PET_MEM128K,105,105,45,10,
                            BS_AUTORADIOBUTTON
END

IDD_PET_SETTINGS_IO_DIALOG DIALOG DISCARDABLE  0, 0, 180, 225
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&Video",IDC_STATIC,5,15,80,60
    AUTORADIOBUTTON "Auto (from ROM)",IDC_SELECT_PET_VIDEO_AUTO,15,30,65,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "40 Columns",IDC_SELECT_PET_VIDEO_40,       15,45,55,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "80 Columns",IDC_SELECT_PET_VIDEO_80,       15,60,55,10,
                            BS_AUTORADIOBUTTON

    GROUPBOX        "&IO Size",IDC_STATIC,5,85,80,60
    AUTORADIOBUTTON "256 Byte",IDC_SELECT_PET_IO256,15,100,45,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "2 kByte", IDC_SELECT_PET_IO2K ,15,115,45,10,
                            BS_AUTORADIOBUTTON

    GROUPBOX        "&CRTC",IDC_STATIC,105,15,90,60
    AUTOCHECKBOX    "CRTC chip enable",IDC_TOGGLE_PET_CRTC, 115,30,75,10,
                            BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP

    GROUPBOX        "&Keyboard",IDC_STATIC,105,85,90,60
    AUTORADIOBUTTON "Graphics",     IDC_SELECT_PET_KEYB_GRAPHICS,115,100,75,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "Business (UK)",IDC_SELECT_PET_KEYB_BUSINESS,115,115,75,10,
                            BS_AUTORADIOBUTTON
END

IDD_PET_SETTINGS_SUPER_DIALOG DIALOG DISCARDABLE  0, 0, 180, 225
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&IO Settings",IDC_STATIC,5,15,160,30
    AUTOCHECKBOX    "SuperPET I/O enable (disables 8x96)",
                            IDC_TOGGLE_PET_SUPER_IO_ENABLE, 15,30,145,10,
                            BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
END

IDD_PET_SETTINGS_8296_DIALOG DIALOG DISCARDABLE  0, 0, 180, 225
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&RAM Settings",IDC_STATIC,5,15,90,45
    AUTOCHECKBOX "$9*** as RAM",IDC_TOGGLE_PET_8296_RAM9,15,30,75,10,
                            BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
    AUTOCHECKBOX "$A*** as RAM",IDC_TOGGLE_PET_8296_RAMA,15,45,75,10,
                            BS_AUTOCHECKBOX
END
