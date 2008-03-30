// CBM2 specific resources.
//
#include "res.r"

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

IDR_MENUCBM2 MENU DISCARDABLE
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
        POPUP "&Flip list"
        BEGIN
            MENUITEM "&Add current image\tAlt-i",          IDM_FLIP_ADD
            MENUITEM "&Remove current image\tAlt-k",       IDM_FLIP_REMOVE
            MENUITEM "Attach &next image\tAlt-n",          IDM_FLIP_NEXT
            MENUITEM "Attach &previous image\tCtrl+Alt-n", IDM_FLIP_PREVIOUS
        END
        MENUITEM SEPARATOR
        MENUITEM "Attach &tape image...\tAlt+T",    IDM_ATTACH_TAPE
        MENUITEM "Detac&h tape image",              IDM_DETACH_TAPE
        POPUP "Datasette contro&l"
        BEGIN
            MENUITEM "S&top",                     IDM_DATASETTE_CONTROL_STOP
            MENUITEM "&Start",                    IDM_DATASETTE_CONTROL_START
            MENUITEM "&Forward",                  IDM_DATASETTE_CONTROL_FORWARD
            MENUITEM "&Rewind",                   IDM_DATASETTE_CONTROL_REWIND
#if 0
            MENUITEM "Re&cord",                   IDM_DATASETTE_CONTROL_RECORD
            MENUITEM "R&eset",                    IDM_DATASETTE_CONTROL_RESET
#endif
        END
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
    POPUP "S&napshot"
    BEGIN
        MENUITEM "Load snapshot image...\tAlt+L",   IDM_SNAPSHOT_LOAD
        MENUITEM "Save snapshot image\tAlt+S",      IDM_SNAPSHOT_SAVE
        MENUITEM "Load quicksnapshot image\tCtrl+Alt+L", IDM_LOADQUICK
        MENUITEM "Save quicksnapshot image\tCtrl+Alt+S", IDM_SAVEQUICK
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
#ifdef HAVE_RESID
        MENUITEM "&Enable resid",                   IDM_TOGGLE_SOUND_RESID
        , CHECKED
#endif
        MENUITEM "SID &filters",                    IDM_TOGGLE_SIDFILTERS
        , CHECKED
        POPUP "SID Model"
        BEGIN
            MENUITEM "6581 (Old)",                  IDM_SIDTYPE_6581
            MENUITEM "8580 (New)",                  IDM_SIDTYPE_8580
        END
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
        MENUITEM "&CBM2 settings...",               IDM_CBM2_SETTINGS
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
        MENUITEM "H&elp",                           IDM_HELP
        MENUITEM SEPARATOR
        MENUITEM "&About...",                       IDM_ABOUT
    END
END

IDD_CBMII_SETTINGS_MODEL_DIALOG DIALOG DISCARDABLE  0, 0, 230, 145
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "Machine &defaults",IDC_STATIC,5,15,70,111
    PUSHBUTTON "CBM 610",IDC_SELECT_CBMII_610,       15,28,50,14, WS_GROUP
    PUSHBUTTON "CBM 620",IDC_SELECT_CBMII_620,       15,43,50,14, WS_GROUP
    PUSHBUTTON "CBM 620+",IDC_SELECT_CBMII_620P,     15,58,50,14, WS_GROUP
    PUSHBUTTON "CBM 710",IDC_SELECT_CBMII_710,       15,73,50,14, WS_GROUP
    PUSHBUTTON "CBM 720",IDC_SELECT_CBMII_720,       15,88,50,14, WS_GROUP
    PUSHBUTTON "CBM 720+",IDC_SELECT_CBMII_720P,     15,103,50,14, WS_GROUP
    GROUPBOX        "&Model line",IDC_STATIC,95,15,80,60
    AUTORADIOBUTTON "7x0 (50 Hz)",IDC_SELECT_CBMII_HW0, 105,30,65,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "6x0 60 Hz",IDC_SELECT_CBMII_HW1,   105,45,65,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "6x0 50 Hz",IDC_SELECT_CBMII_HW2,   105,60,65,10,
                            BS_AUTORADIOBUTTON
END

IDD_CBMII_SETTINGS_IO_DIALOG DIALOG DISCARDABLE  0, 0, 230, 145
STYLE DS_MODALFRAME | WS_POPUP | WS_CAPTION | WS_SYSMENU
FONT 8, "MS Sans Serif"
BEGIN
    GROUPBOX        "&Memory",IDC_STATIC,5,15,70,75
    AUTORADIOBUTTON "128 kByte",IDC_SELECT_CBMII_MEM_128,  15,30,45,10,
                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
    AUTORADIOBUTTON "256 kByte",IDC_SELECT_CBMII_MEM_256,  15,45,45,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "512 kByte",IDC_SELECT_CBMII_MEM_512,  15,60,45,10,
                            BS_AUTORADIOBUTTON
    AUTORADIOBUTTON "1024 kByte",IDC_SELECT_CBMII_MEM_1024,15,75,45,10,
                            BS_AUTORADIOBUTTON
//    GROUPBOX        "&Keyboard",IDC_STATIC,5,75,40,50
//    AUTORADIOBUTTON "Graphics",IDC_SELECT_CBMII_KEYB_GRAPHICS, 15,90,35,10,
//                            BS_AUTORADIOBUTTON | WS_GROUP | WS_TABSTOP
//    AUTORADIOBUTTON "Business (UK)",IDC_SELECT_CBMII_KEYB_BUSINESS,
//                            15,105,35,10, BS_AUTORADIOBUTTON
    GROUPBOX        "&Ram banks",IDC_STATIC,95,15,130,105
    AUTOCHECKBOX    "Bank 15 $0800-$0FFF RAM",IDC_TOGGLE_CBMII_RAM08,
                    105,30,115,10, BS_AUTOCHECKBOX | WS_GROUP | WS_TABSTOP
    AUTOCHECKBOX    "Bank 15 $1000-$1FFF RAM",IDC_TOGGLE_CBMII_RAM1,
                    105,45,115,10, BS_AUTOCHECKBOX
    AUTOCHECKBOX    "Bank 15 $2000-$3FFF RAM",IDC_TOGGLE_CBMII_RAM2,
                    105,60,115,10, BS_AUTOCHECKBOX
    AUTOCHECKBOX    "Bank 15 $4000-$5FFF RAM",IDC_TOGGLE_CBMII_RAM4,
                    105,75,115,10, BS_AUTOCHECKBOX
    AUTOCHECKBOX    "Bank 15 $6000-$7FFF RAM",IDC_TOGGLE_CBMII_RAM6,
                    105,90,115,10, BS_AUTOCHECKBOX
    AUTOCHECKBOX    "Bank 15 $C000-$CFFF RAM",IDC_TOGGLE_CBMII_RAMC,
                    105,105,115,10, BS_AUTOCHECKBOX
END
