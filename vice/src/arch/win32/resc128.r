// C128 specific resources.
//
#include "res.r"

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

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
        MENUITEM "&Warp mode",                      IDM_TOGGLE_WARP_MODE
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
        MENUITEM "&Device settings...",             IDM_DEVICEMANAGER
        MENUITEM "D&rive settings...",              IDM_DRIVE_SETTINGS
        MENUITEM "&VIC-II settings...",             IDM_VICII_SETTINGS
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

/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_VICII_PALETTE_DIALOG DIALOG DISCARDABLE  0, 0, 180, 120
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    AUTORADIOBUTTON "&Default",IDC_SELECT_VICII_DEFAULT,15,10,35,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "&Custom",IDC_SELECT_VICII_CUSTOM,15,25,35,10,
                            BS_AUTORADIOBUTTON
    COMBOBOX        IDC_VICII_CUSTOM_NAME,80,25,120,12,CBS_DROPDOWN | CBS_SORT |
                    WS_VSCROLL | WS_GROUP | WS_TABSTOP
    PUSHBUTTON      "&Browse...",IDC_VICII_CUSTOM_BROWSE,80,43,50,14,WS_GROUP
    AUTORADIOBUTTON "&C64S",IDC_SELECT_VICII_C64S,15,40,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "CC&S64",IDC_SELECT_VICII_CCS64,15,55,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "&Frodo",IDC_SELECT_VICII_FRODO,15,70,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "&Godot",IDC_SELECT_VICII_GODOT,15,85,35,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "&PC64",IDC_SELECT_VICII_PC64,15,100,35,10,
                            BS_AUTORADIOBUTTON
END

IDD_VICII_SPRITES_DIALOG DIALOG DISCARDABLE  0, 0, 180, 117
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    AUTOCHECKBOX    "Sprite-&sprite collisions", IDC_TOGGLE_VICII_SSC,
                    15,10,135,10, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
    AUTOCHECKBOX    "Sprite-&background collisions", IDC_TOGGLE_VICII_SBC,
                    15,25,135,10, BS_AUTOCHECKBOX | WS_TABSTOP
END
