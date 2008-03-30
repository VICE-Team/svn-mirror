// VIC specific resources.
//
#include "res.r"

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

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
        MENUITEM "&Joystick settings...",           IDM_JOY_SETTINGS
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

