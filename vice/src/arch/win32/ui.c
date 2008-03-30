/*
 * ui.c - Windows user interface.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "ui.h"

#include "attach.h"
#include "autostart.h"
#include "archdep.h"
#include "cmdline.h"
#include "datasette.h"
#include "drive.h"
#include "fliplist.h"
#include "interrupt.h"
#include "kbd.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "res.h"
#include "resources.h"
#include "tape.h"
#include "uiattach.h"
#include "uidrive.h"
#include "uijoystick.h"
#include "uilib.h"
#include "uisnapshot.h"
#include "uisound.h"
#include "utils.h"
#include "vsync.h"
#include "winmain.h"

/* Main window.  */
static HWND main_hwnd;

static HWND status_hwnd;

static int status_height;

/* Main canvas.  */
static char *main_hwnd_title;

/* Exposure handler.  */
canvas_redraw_t exposure_handler;

static HACCEL   ui_accelerator;

/* Forward prototypes.  */
static long CALLBACK window_proc(HWND window, UINT msg,
                                 WPARAM wparam, LPARAM lparam);

/* List of resources that can be switched on and off from the menus.  */
struct {
    /* Name of resource.  */
    const char *name;
    /* ID of the corresponding menu item.  */
    UINT item_id;
} toggle_list[] = {
    { "Sound", IDM_TOGGLE_SOUND },
    { "DriveTrueEmulation", IDM_TOGGLE_DRIVE_TRUE_EMULATION },
    { "DoubleSize", IDM_TOGGLE_DOUBLESIZE },
    { "DoubleScan", IDM_TOGGLE_DOUBLESCAN },
    { "VideoCache", IDM_TOGGLE_VIDEOCACHE },
    { "SidFilters", IDM_TOGGLE_SIDFILTERS },
#ifdef HAVE_RESID
    { "SidUseResid", IDM_TOGGLE_SOUND_RESID },
#endif
    { "WarpMode", IDM_TOGGLE_WARP_MODE },
    { "WarpMode", IDM_TOGGLE_WARP_MODE|0x00010000 },
    { NULL, 0 }
};

/*  List of resources which can have multiple mutual exclusive menu entries. */
typedef struct {
    int value;
    UINT item_id; /* The last item_id has to be zero.  */
} res_possible_values;

res_possible_values RefreshRateValues[] = {
        {0, IDM_REFRESH_RATE_AUTO},
        {1, IDM_REFRESH_RATE_1},
        {2, IDM_REFRESH_RATE_2},
        {3, IDM_REFRESH_RATE_3},
        {4, IDM_REFRESH_RATE_4},
        {5, IDM_REFRESH_RATE_5},
        {6, IDM_REFRESH_RATE_6},
        {7, IDM_REFRESH_RATE_7},
        {8, IDM_REFRESH_RATE_8},
        {9, IDM_REFRESH_RATE_9},
        {10, IDM_REFRESH_RATE_10},
        {-1, 0}
};

res_possible_values SpeedValues[] = {
        {0, IDM_MAXIMUM_SPEED_NO_LIMIT},
        {10, IDM_MAXIMUM_SPEED_10},
        {20, IDM_MAXIMUM_SPEED_20},
        {50, IDM_MAXIMUM_SPEED_50},
        {100, IDM_MAXIMUM_SPEED_100},
        {200, IDM_MAXIMUM_SPEED_200},
        {-1, 0}
};

res_possible_values SyncFactor[] = {
        {DRIVE_SYNC_PAL, IDM_SYNC_FACTOR_PAL},
        {DRIVE_SYNC_NTSC, IDM_SYNC_FACTOR_NTSC},
        {-1, 0}
};

static res_possible_values SidType[] = {
    {0, IDM_SIDTYPE_6581},
    {1, IDM_SIDTYPE_8580},
    {-1,0}
};

struct {
    const char *name;
    const res_possible_values *vals;
} value_list[] = {
    {"RefreshRate", RefreshRateValues},
    {"Speed", SpeedValues},
    {"DriveSyncFactor", SyncFactor},
    {"SidModel", SidType},
    {NULL,NULL}
};

/* ------------------------------------------------------------------------ */

/* UI-related resources.  */

static resource_t resources[] = {
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */
/*
static cmdline_option_t cmdline_options[] = {
    { NULL }
};
*/

int ui_init_cmdline_options(void)
{
    return 0;
}

/* ------------------------------------------------------------------------ */

#define UI_COMMON_HOTKEYS \
    {FVIRTKEY|FCONTROL|FALT|FNOINVERT,'R',IDM_HARD_RESET},  \
    {FVIRTKEY|FALT|FNOINVERT,'R',IDM_SOFT_RESET},           \
    {FVIRTKEY|FALT|FNOINVERT,'8',IDM_ATTACH_8},             \
    {FVIRTKEY|FALT|FNOINVERT,'9',IDM_ATTACH_9},             \
    {FVIRTKEY|FALT|FNOINVERT,'0',IDM_ATTACH_10},            \
    {FVIRTKEY|FALT|FNOINVERT,'1',IDM_ATTACH_11},            \
    {FVIRTKEY|FALT|FNOINVERT,'T',IDM_ATTACH_TAPE},          \
    {FVIRTKEY|FCONTROL|FALT|FNOINVERT,'L',IDM_LOADQUICK},   \
    {FVIRTKEY|FCONTROL|FALT|FNOINVERT,'S',IDM_SAVEQUICK},   \
    {FVIRTKEY|FALT|FNOINVERT,'L',IDM_SNAPSHOT_LOAD},        \
    {FVIRTKEY|FALT|FNOINVERT,'S',IDM_SNAPSHOT_SAVE},        \
    {FVIRTKEY|FALT|FNOINVERT,'M',IDM_MONITOR},              \
    {FVIRTKEY|FALT|FNOINVERT,'X',IDM_EXIT},                 \
    {FVIRTKEY|FALT|FNOINVERT,'W',IDM_TOGGLE_WARP_MODE},     \
    {FVIRTKEY|FALT|FNOINVERT,'I',IDM_FLIP_ADD},             \
    {FVIRTKEY|FALT|FNOINVERT,'K',IDM_FLIP_REMOVE},          \
    {FVIRTKEY|FALT|FNOINVERT,'N',IDM_FLIP_NEXT},            \
    {FVIRTKEY|FCONTROL|FALT|FNOINVERT,'N',IDM_FLIP_PREVIOUS},

static ACCEL c64_accel[] = {
    {FVIRTKEY|FALT|FNOINVERT,'F',IDM_CART_FREEZE},
    UI_COMMON_HOTKEYS
};

static ACCEL c128_accel[] = {
    UI_COMMON_HOTKEYS
};

static ACCEL cbm2_accel[] = {
    UI_COMMON_HOTKEYS
};

static ACCEL vic_accel[] = {
    UI_COMMON_HOTKEYS
};

static ACCEL pet_accel[] = {
    UI_COMMON_HOTKEYS
};

/* Initialize the UI before setting all the resource values.  */
int ui_init(int *argc, char **argv)
{
    WNDCLASS window_class;
    WORD menu;
    RECT    rect;

    switch (machine_class) {
        case VICE_MACHINE_C64:
            menu = IDR_MENUC64;
            ui_accelerator=CreateAcceleratorTable(c64_accel,19);
            break;
        case VICE_MACHINE_C128:
            menu = IDR_MENUC128;
            ui_accelerator=CreateAcceleratorTable(c128_accel,18);
            break;
        case VICE_MACHINE_VIC20:
            menu = IDR_MENUVIC;
            ui_accelerator=CreateAcceleratorTable(vic_accel,18);
            break;
        case VICE_MACHINE_PET:
            menu = IDR_MENUPET;
            ui_accelerator=CreateAcceleratorTable(pet_accel,18);
            break;
        case VICE_MACHINE_CBM2:
            menu = IDR_MENUCBM2;
            ui_accelerator=CreateAcceleratorTable(cbm2_accel,18);
            break;
        default:
            log_debug("UI: No menu entries for this machine defined!");
            log_debug("UI: Using C64 type UI menues.");
            menu = IDR_MENUC64;
            ui_accelerator=CreateAcceleratorTable(c64_accel,19);
    }

    /* Register the window class.  */
    window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    window_class.lpfnWndProc = window_proc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = winmain_instance;
    window_class.hIcon = LoadIcon(winmain_instance,
                                  MAKEINTRESOURCE(IDI_ICON1));
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0) + 1);
    window_class.lpszMenuName = MAKEINTRESOURCE(menu);
    window_class.lpszClassName = APPLICATION_CLASS;
    RegisterClass(&window_class);

    /* Create the main window.  Notice that we are not going to
       `ShowWindow()' it yet; this will be done as soon as the video module
       requires us to do so.  This is needed both because the video module
       needs an application window to be created to initialize itself, and
       because this might allow us to support more than one emulation window
       in the future.  */
    main_hwnd = CreateWindow(APPLICATION_CLASS,
                             "No title", /* (for now) */
                             WS_OVERLAPPED|WS_CLIPCHILDREN|WS_BORDER|WS_DLGFRAME|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             NULL,
                             NULL,
                             winmain_instance,
                             NULL);

    InitCommonControls();
    status_hwnd=CreateStatusWindow(WS_CHILD|WS_VISIBLE,"",main_hwnd,IDM_STATUS_WINDOW);
    GetClientRect(status_hwnd,&rect);
    status_height=rect.bottom-rect.top;

    if (!main_hwnd)
        return -1;

    return 0;
}

/* Initialize the UI after setting all the resource values.  */
int ui_init_finish(void)
{
    atexit(ui_exit);
    return 0;
}

/* Exit.  */
void ui_exit(void)
{
    if (ui_accelerator) {
        DestroyAcceleratorTable(ui_accelerator);
    }
}

/* Create a Window for the emulation.  */
/* Oh, well, this currently does not actually create a new window; it just
   sets the main one up so that it complies with the specified parameters and
   returns a handler to it.  In the future, we might handle the "more than
   one emulation window" case, so that e.g. X128 gets a window for the VDC
   and one for the VIC-II.  */
HWND ui_open_canvas_window(const char *title, unsigned int width,
                           unsigned int height, canvas_redraw_t exp_handler)
{
    /* This is avoids moving the window around and also makes sure the client
       rectangle has the correct size...  Maybe there is a better way to
       achieve the same result?  (FIXME?)  */
    ui_resize_canvas_window(main_hwnd, width, height);

    /* Set the title...  */
    main_hwnd_title = stralloc(title);
    SetWindowText(main_hwnd, title);

    /* Finally, we can actually display the thing...  */
    ShowWindow(main_hwnd, winmain_cmd_show);

    /* Of course, this will be different if we will happen to have more than
       one window.  */
    exposure_handler = exp_handler;
    return main_hwnd;
}

/* Resize `w' so that the client rectangle is of the requested size.  */
void ui_resize_canvas_window(HWND w, unsigned int width, unsigned int height)
{
    RECT wrect;

    GetClientRect(w, &wrect);
    ClientToScreen(w, (LPPOINT) &wrect);
    ClientToScreen(w, ((LPPOINT) &wrect) + 1);
    wrect.right = wrect.left + width;
    wrect.bottom = wrect.top + height + status_height;
    AdjustWindowRect(&wrect, WS_OVERLAPPED|WS_BORDER|WS_DLGFRAME, TRUE);
    MoveWindow(w,
               wrect.left,
               wrect.top,
               wrect.right - wrect.left,
               wrect.bottom - wrect.top,
               TRUE);
}

/* Update all the menus according to the current settings.  */
void ui_update_menus(void)
{
    HMENU menu = GetMenu(main_hwnd);
    int i,j;
    int value;
    int result;

    for (i = 0; toggle_list[i].name != NULL; i++) {
        resources_get_value(toggle_list[i].name, (resource_value_t *) &value);
        CheckMenuItem(menu, toggle_list[i].item_id,
                      value ? MF_CHECKED : MF_UNCHECKED);
    }

    for (i = 0; value_list[i].name != NULL; i++) {
        result=resources_get_value(value_list[i].name,
                                   (resource_value_t *) &value);
        if (result==0) {
            for (j = 0; value_list[i].vals[j].item_id != 0; j++) {
                if (value == value_list[i].vals[j].value) {
                    CheckMenuItem(menu,value_list[i].vals[j].item_id,
                                  MF_CHECKED);
                } else {
                    CheckMenuItem(menu,value_list[i].vals[j].item_id,
                                  MF_UNCHECKED);
                }
            }
        }
    }
}

/* ------------------------------------------------------------------------- */

static ui_machine_specific_t ui_machine_specific = NULL;

void ui_register_machine_specific(ui_machine_specific_t func)
{
    ui_machine_specific = func;
}

/* ------------------------------------------------------------------------- */

/* Report an error to the user (`printf()' style).  */
void ui_error(const char *format,...)
{
        char tmp[1024];
        va_list args;

        va_start(args, format);
        vsprintf(tmp, format, args);
        va_end(args);
        MessageBox(main_hwnd, tmp, "VICE Error!", MB_OK | MB_ICONSTOP);
}

/* Report a message to the user (`printf()' style).  */
void ui_message(const char *format,...)
{
        char tmp[1024];
        va_list args;

        va_start(args, format);
        vsprintf(tmp, format, args);
        va_end(args);
        MessageBox(main_hwnd, tmp, "VICE Information", MB_OK | MB_ICONASTERISK);
}

/* Handle the "CPU JAM" case.  */
ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    return UI_JAM_HARD_RESET;
}

/* Handle the "Do you want to extend the disk image to 40-track format"?
   dialog.  */
int ui_extend_image_dialog(void)
{
    int ret;

    ret = MessageBox(main_hwnd, "Extend image to 40-track format?",
                     "VICE question", MB_YESNO | MB_ICONQUESTION);
    return ret == IDYES;
}

/* ------------------------------------------------------------------------- */

/* Dispay the current emulation speed.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    char buf[256];

    sprintf(buf, "%s at %d%% speed, %d fps%s",
            main_hwnd_title, (int)(percent + .5), (int)(framerate + .5),
            warp_flag ? " (warp)" : "");
    SetWindowText(main_hwnd, buf);
}

static ui_drive_enable_t    status_enabled;
static int                  status_led[2];
static int                  status_map[2];
static double               status_track[2];
static int                  status_unit[2];
static int                 *drive_active_led;

static void SetStatusWindowParts(void)
{
int     number_of_parts;
RECT    rect;
int     posx[3];
int     width;
int     i;

    number_of_parts=0;
    if (status_enabled&UI_DRIVE_ENABLE_0) {
        status_map[0]=number_of_parts;
        status_unit[number_of_parts]=8;
        number_of_parts++;
    }
    if (status_enabled&UI_DRIVE_ENABLE_1) {
        status_map[1]=number_of_parts;
        status_unit[number_of_parts]=9;
        number_of_parts++;
    }
    GetWindowRect(status_hwnd,&rect);
    width=rect.right-rect.left;
    for (i=number_of_parts; i>=0; i--) {
        posx[i]=width;
        width-=110;
    }
    SendMessage(status_hwnd,SB_SETPARTS,number_of_parts+1,(LPARAM)posx);
    if (number_of_parts==2) {
        SendMessage(status_hwnd,SB_SETTEXT,2|SBT_OWNERDRAW,0);
    }
    if (number_of_parts==1) {
        SendMessage(status_hwnd,SB_SETTEXT,1|SBT_OWNERDRAW,0);
    }
}

void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
    status_enabled=enable;
    drive_active_led = drive_led_color;
    SetStatusWindowParts();
}

/* Toggle displaying of the drive track.  */
void ui_display_drive_track(int drivenum, double track_number)
{
    status_track[status_map[drivenum]]=track_number;
    SendMessage(status_hwnd,SB_SETTEXT,(status_map[drivenum]+1)|SBT_OWNERDRAW,0);
}

/* Toggle displaying of the drive LED.  */
void ui_display_drive_led(int drivenum, int status)
{
    status_led[status_map[drivenum]]=status;
    SendMessage(status_hwnd,SB_SETTEXT,(status_map[drivenum]+1)|SBT_OWNERDRAW,0);
}

/* Toggle displaying of paused state.  */
void ui_display_paused(int flag)
{
}

ui_button_t ui_ask_confirmation(const char *title, const char *text)
{
    return UI_BUTTON_NONE;
}

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

static void save_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
    ui_snapshot_save_dialog(main_hwnd);
}

static void load_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
    ui_snapshot_load_dialog(main_hwnd);
}

typedef struct {
    char    name[MAX_PATH];
    int     valid;
} snapfiles;

static snapfiles    files[10];
static int          lastindex;
static int          snapcounter;

static void save_quicksnapshot_trap(ADDRESS unused_addr, void *unused_data)
{
int     i,j;
char    *fullname;
char    *fullname2;

    if (lastindex==-1) {
        lastindex=0;
        strcpy(files[lastindex].name,"quicksnap0.vsf");
    } else {
        if (lastindex==9) {
            if (snapcounter==10) {
                fullname=concat(archdep_boot_path(),"\\",machine_name,"\\",files[0].name,NULL);
                DeleteFile(fullname);
                free(fullname);
                for (i=1; i<10; i++) {
                    fullname=concat(archdep_boot_path(),"\\",machine_name,"\\",files[i].name,NULL);
                    fullname2=concat(archdep_boot_path(),"\\",machine_name,"\\",files[i-1].name,NULL);
                    MoveFile(fullname,fullname2);
                    free(fullname);
                    free(fullname2);
                }
            } else {
                for (i=0; i<10; i++) {
                    if (files[i].valid==0) break;
                }
                for (j=i+1; j<10; j++) {
                    if (files[j].valid) {
                        strcpy(files[i].name,files[j].name);
                        files[i].name[strlen(files[i].name)-5]='0'+i;
                        fullname=concat(archdep_boot_path(),"\\",machine_name,"\\",files[j].name,NULL);
                        fullname2=concat(archdep_boot_path(),"\\",machine_name,"\\",files[i].name,NULL);
                        MoveFile(fullname,fullname2);
                        free(fullname);
                        free(fullname2);
                        i++;
                    }
                }
                strcpy(files[i].name,files[0].name);
                files[i].name[strlen(files[i].name)-5]='0'+i;
                lastindex=i;
            }
        } else {
            strcpy(files[lastindex+1].name,files[lastindex].name);
            lastindex++;
            files[lastindex].name[strlen(files[lastindex].name)-5]='0'+lastindex;
        }
    }

    fullname=concat(archdep_boot_path(),"\\",machine_name,"\\",files[lastindex].name,NULL);
    if (machine_write_snapshot(fullname,0,0)<0) {
        ui_error("Can't write snapshot file.");
    }
    free(fullname);
}

static void load_quicksnapshot_trap(ADDRESS unused_addr, void *unused_data)
{
    char *fullname;

    fullname=concat(archdep_boot_path(),"\\",machine_name,"\\",files[lastindex].name,NULL);
    if (machine_read_snapshot(fullname)<0) {
        ui_error("Cannot read snapshot image");
    }
    free(fullname);
}

/* ------------------------------------------------------------------------ */

/* Return the main window handler.  */
HWND ui_get_main_hwnd(void)
{
    return main_hwnd;
}

/* Dispatch the next pending event, if any.  Otherwise, just return.  */
void ui_dispatch_next_event(void)
{
    MSG msg;

    if (!GetMessage(&msg, NULL, 0, 0))
        exit(msg.wParam);
    if (ui_accelerator) {
        if (!TranslateAccelerator(main_hwnd,ui_accelerator,&msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    } else {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/* Dispatch all the current pending events; return as soon as no more events
   are pending.  */
void ui_dispatch_events(void)
{
    MSG msg;

    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        ui_dispatch_next_event();
}

/* ------------------------------------------------------------------------ */

int CALLBACK about_dialog_proc(HWND dialog, UINT msg,
                               UINT wparam, LONG lparam)
{
    char version[256];

    switch (msg) {
        case WM_INITDIALOG:
            sprintf(version, "Version %s", VERSION);
            SetDlgItemText(dialog, IDC_ABOUT_VERSION, version);
            return TRUE;
        case WM_CLOSE:
            EndDialog(dialog,0);
            return TRUE;
        case WM_COMMAND:
            if (wparam == IDOK) {
                EndDialog(dialog, 0);
                return TRUE;
            }
            break;
    }
    return FALSE;
}

static void scan_files(void)
{
WIN32_FIND_DATA     file_info;
HANDLE              search_handle;
int                 i;
char                *dirname;

    dirname=concat(archdep_boot_path(),"\\",machine_name,"\\quicksnap?.vsf",NULL);
    search_handle=FindFirstFile(dirname,&file_info);
    snapcounter=0;
    lastindex=-1;
    for (i=0; i<10; i++) {
        files[i].valid=0;
    }
    if (search_handle!=INVALID_HANDLE_VALUE) {
        do {
            char c;
            c=file_info.cFileName[strlen(file_info.cFileName)-5];
            if ((c>='0') && (c<='9')) {
                strcpy(files[c-'0'].name,file_info.cFileName);
                files[c-'0'].valid=1;
                if ((c-'0')>lastindex) {
                    lastindex=c-'0';
                }
                snapcounter++;
            }
        } while (FindNextFile(search_handle,&file_info));
        FindClose(search_handle);
    }
    free(dirname);
}

/* ------------------------------------------------------------------------ */

/* FIXME: tmp hack.  */
int syscolorchanged, displaychanged, querynewpalette, palettechanged;

static void handle_wm_command(WPARAM wparam, LPARAM lparam)
{
char *fname;
char *dname;

    /* Handle machine specific commands first.  */
    if (ui_machine_specific)
        ui_machine_specific(wparam, main_hwnd);

    switch (wparam) {
      case IDM_DEVICEMANAGER:
        ui_attach_dialog(main_hwnd);
        break;
      case IDM_EXIT|0x00010000:
      case IDM_EXIT:
        PostMessage(main_hwnd, WM_CLOSE, wparam, lparam);
        break;
      case IDM_ABOUT:
        DialogBox(winmain_instance, MAKEINTRESOURCE(IDD_ABOUT), main_hwnd,
                  (DLGPROC) about_dialog_proc);
        break;
        case IDM_HELP:
            fname=concat(archdep_boot_path(),"\\DOC\\vice_toc.html",NULL);
            dname=concat(archdep_boot_path(),"\\DOC",NULL);
            ShellExecute(main_hwnd,"open",fname,NULL,dname,0);
            free(fname);
            free(dname);
            break;
      case IDM_ATTACH_8|0x00010000:
      case IDM_ATTACH_9|0x00010000:
      case IDM_ATTACH_10|0x00010000:
      case IDM_ATTACH_11|0x00010000:
      case IDM_ATTACH_8:
      case IDM_ATTACH_9:
      case IDM_ATTACH_10:
      case IDM_ATTACH_11:
        {
            char *s;
            int unit = 8;

            switch (wparam&0xffff) {
              case IDM_ATTACH_8:
                unit = 8;
                break;
              case IDM_ATTACH_9:
                unit = 9;
                break;
              case IDM_ATTACH_10:
                unit = 10;
                break;
              case IDM_ATTACH_11:
                unit = 11;
                break;
            }
            if ((s = ui_select_file("Attach disk image",
                "Disk image files (*.d64;*.d71;*.d81;*.g64;*.g41;*.x64)\0*.d64;*.d71;*.d81;*.g64;*.g41;*.x64\0"
                "All files (*.*)\0*.*\0", main_hwnd)) != NULL) {
                if (file_system_attach_disk(unit, s) < 0)
                    ui_error("Cannot attach specified file");
                free(s);
            }
        }
        break;
      case IDM_DETACH_8:
        file_system_detach_disk(8);
        break;
      case IDM_DETACH_9:
        file_system_detach_disk(9);
        break;
      case IDM_DETACH_10:
        file_system_detach_disk(10);
        break;
      case IDM_DETACH_11:
        file_system_detach_disk(11);
        break;
      case IDM_DETACH_ALL:
        file_system_detach_disk(8);
        file_system_detach_disk(9);
        file_system_detach_disk(10);
        file_system_detach_disk(11);
        break;
      case IDM_FLIP_ADD|0x00010000:
      case IDM_FLIP_ADD:
        flip_add_image();
        break;
      case IDM_FLIP_REMOVE|0x00010000:
      case IDM_FLIP_REMOVE:
        flip_remove(-1, NULL);
        break;
      case IDM_FLIP_NEXT|0x00010000:
      case IDM_FLIP_NEXT:
        flip_attach_head(1);
        break;
      case IDM_FLIP_PREVIOUS|0x00010000:
      case IDM_FLIP_PREVIOUS:
        flip_attach_head(0);
        break;
      case IDM_ATTACH_TAPE|0x00010000:
      case IDM_ATTACH_TAPE:
        {
            char *s;

            if ((s = ui_select_file("Attach tape image",
                "Tape image files (*.t64;*.tap)\0*.t64;*.tap\0"
                "All files (*.*)\0*.*\0", main_hwnd)) != NULL) {
                if (tape_attach_image(s) < 0)
                    ui_error("Cannot attach specified file.");
                free(s);
            }
        }
        break;
      case IDM_DETACH_TAPE:
        tape_detach_image();
        break;
      case IDM_DATASETTE_CONTROL_STOP:
        datasette_control(DATASETTE_CONTROL_STOP);
        break;
      case IDM_DATASETTE_CONTROL_START:
        datasette_control(DATASETTE_CONTROL_START);
        break;
      case IDM_DATASETTE_CONTROL_FORWARD:
        datasette_control(DATASETTE_CONTROL_FORWARD);
        break;
      case IDM_DATASETTE_CONTROL_REWIND:
        datasette_control(DATASETTE_CONTROL_REWIND);
        break;
      case IDM_DATASETTE_CONTROL_RECORD:
        datasette_control(DATASETTE_CONTROL_RECORD);
        break;
      case IDM_DATASETTE_CONTROL_RESET:
        datasette_control(DATASETTE_CONTROL_RESET);
        break;
      case IDM_AUTOSTART:
        {
            char *s;

            if ((s = ui_select_file("Autostart disk/tape image",
                "Disk image files (*.d64;*.d71;*.d81;*.g64;*.g41;*.x64)\0*.d64;*.d71;*.d81;*.g64;*.g41;*.x64\0"
                "Tape image files (*.t64;*.p00)\0*.t64;*.p00\0"
                "All files (*.*)\0*.*\0", main_hwnd)) != NULL) {
                if (autostart_autodetect(s, "*") < 0)
                    ui_error("Cannot autostart specified file.");
                free(s);
            }
        }
        break;
      case IDM_SNAPSHOT_LOAD|0x00010000:
      case IDM_SNAPSHOT_LOAD:
        if (1 /* !ui_emulation_is_paused()*/ )
            maincpu_trigger_trap(load_snapshot_trap, (void *) 0);
        else
            load_snapshot_trap(0, 0);
        /* ui_snapshot_load_dialog(main_hwnd);*/
        break;
      case IDM_SNAPSHOT_SAVE|0x00010000:
      case IDM_SNAPSHOT_SAVE:
        maincpu_trigger_trap(save_snapshot_trap, (void *) 0);
        break;
        case IDM_SAVEQUICK|0x00010000:
        case IDM_SAVEQUICK:
            scan_files();
            maincpu_trigger_trap(save_quicksnapshot_trap, (void*) 0);
            break;
        case IDM_LOADQUICK|0x00010000:
        case IDM_LOADQUICK:
            scan_files();
            if (snapcounter>0) {
                maincpu_trigger_trap(load_quicksnapshot_trap, (void *) 0);
            }
            break;
      case IDM_MONITOR|0x00010000:
      case IDM_MONITOR:
          if (1 /* !ui_emulation_is_paused()*/ )
              maincpu_trigger_trap(mon_trap, (void *) 0);
          else
              mon_trap(MOS6510_REGS_GET_PC(&maincpu_regs), 0);
              break;
        case IDM_HARD_RESET+0x00010000:
        case IDM_SOFT_RESET+0x00010000:
      case IDM_HARD_RESET:
      case IDM_SOFT_RESET:
        kbd_clear_keymatrix();
        if (MessageBox(main_hwnd, "Do you really want to reset the emulated machine?",
                       ((wparam&0xffff) == IDM_HARD_RESET ? "Hard reset"
                        : "Soft reset"),
                       MB_YESNO | MB_ICONQUESTION) == IDYES) {
            if ((wparam&0xffff) == IDM_HARD_RESET) {
                machine_powerup();
            } else {
                maincpu_trigger_reset();
            }
        }
        break;
      case IDM_REFRESH_RATE_AUTO:
        resources_set_value("RefreshRate", (resource_value_t) 0);
        break;
      case IDM_REFRESH_RATE_1:
        resources_set_value("RefreshRate", (resource_value_t) 1);
        break;
      case IDM_REFRESH_RATE_2:
        resources_set_value("RefreshRate", (resource_value_t) 2);
        break;
      case IDM_REFRESH_RATE_3:
        resources_set_value("RefreshRate", (resource_value_t) 3);
        break;
      case IDM_REFRESH_RATE_4:
        resources_set_value("RefreshRate", (resource_value_t) 4);
        break;
      case IDM_REFRESH_RATE_5:
        resources_set_value("RefreshRate", (resource_value_t) 5);
        break;
      case IDM_REFRESH_RATE_6:
        resources_set_value("RefreshRate", (resource_value_t) 6);
        break;
      case IDM_REFRESH_RATE_7:
        resources_set_value("RefreshRate", (resource_value_t) 7);
        break;
      case IDM_REFRESH_RATE_8:
        resources_set_value("RefreshRate", (resource_value_t) 8);
        break;
      case IDM_REFRESH_RATE_9:
        resources_set_value("RefreshRate", (resource_value_t) 9);
        break;
      case IDM_REFRESH_RATE_10:
        resources_set_value("RefreshRate", (resource_value_t) 10);
        break;
      case IDM_MAXIMUM_SPEED_200:
        resources_set_value("Speed", (resource_value_t) 200);
        break;
      case IDM_MAXIMUM_SPEED_100:
        resources_set_value("Speed", (resource_value_t) 100);
        break;
      case IDM_MAXIMUM_SPEED_50:
        resources_set_value("Speed", (resource_value_t) 50);
        break;
      case IDM_MAXIMUM_SPEED_20:
        resources_set_value("Speed", (resource_value_t) 20);
        break;
      case IDM_MAXIMUM_SPEED_10:
        resources_set_value("Speed", (resource_value_t) 10);
        break;
      case IDM_MAXIMUM_SPEED_NO_LIMIT:
        resources_set_value("Speed", (resource_value_t) 0);
        break;
        case IDM_SIDTYPE_6581:
            resources_set_value("SidModel", (resource_value_t) 0);
            break;
        case IDM_SIDTYPE_8580:
            resources_set_value("SidModel", (resource_value_t) 1);
            break;
      case IDM_DRIVE_SETTINGS:
        ui_drive_settings_dialog(main_hwnd);
        break;
        case IDM_JOY_SETTINGS:
            ui_joystick_settings_dialog(main_hwnd);
            break;
        case IDM_SOUND_SETTINGS:
            ui_sound_settings_dialog(main_hwnd);
            break;
      case IDM_SYNC_FACTOR_PAL:
        resources_set_value("DriveSyncFactor",
                            (resource_value_t) DRIVE_SYNC_PAL);
        break;
      case IDM_SYNC_FACTOR_NTSC:
        resources_set_value("DriveSyncFactor",
                            (resource_value_t) DRIVE_SYNC_NTSC);
        break;
      case IDM_SETTINGS_SAVE:
        if (resources_save(NULL) < 0)
            ui_error("Cannot save settings.");
        else
            ui_message("Settings saved successfully.");
        break;
      case IDM_SETTINGS_LOAD:
        if (resources_load(NULL) < 0) {
            ui_error("Cannot load settings.");
        } else {
            ui_message("Settings loaded successfully.");
            ui_update_menus();
        }
        break;
      case IDM_SETTINGS_DEFAULT:
        resources_set_defaults();
        ui_message("Default settings restored.");
        ui_update_menus();
        break;
      default:
        {
            int i;

            for (i = 0; toggle_list[i].name != NULL; i++)
                if (toggle_list[i].item_id == wparam) {
                    resources_toggle(toggle_list[i].name, NULL);
                    break;
                }
            break;
        }
    }
}

static void clear(HDC hdc, int x1, int y1, int x2, int y2)
{
static HBRUSH   back_color;
RECT            clear_rect;

    if (back_color==NULL) back_color=CreateSolidBrush(0);
    clear_rect.left=x1;
    clear_rect.top=y1;
    clear_rect.right=x2;
    clear_rect.bottom=y2;
    FillRect(hdc,&clear_rect,back_color);
}


/* Window procedure.  All messages are handled here.  */
static long CALLBACK window_proc(HWND window, UINT msg,
                                 WPARAM wparam, LPARAM lparam)
{
RECT    led;
char    text[256];
RECT    client_rect;
POINT   *point;

    switch (msg) {
        case WM_SIZE:
            SendMessage(status_hwnd,msg,wparam,lparam);
            SetStatusWindowParts();
            GetClientRect(window, &client_rect);

            if (exposure_handler) {
                exposure_handler(client_rect.right - client_rect.left,
                                client_rect.bottom - client_rect.top - status_height);
            }
            return 0;
        case WM_DRAWITEM:
            if (wparam==IDM_STATUS_WINDOW) {
                led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2;
                led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+18;
                led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+2;
                led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+84;
                sprintf(text,"%d: Track: %.1f",status_unit[((DRAWITEMSTRUCT*)lparam)->itemID-1],status_track[((DRAWITEMSTRUCT*)lparam)->itemID-1]);
                SetBkColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_MENU));
                SetTextColor(((DRAWITEMSTRUCT*)lparam)->hDC,(COLORREF)GetSysColor(COLOR_MENUTEXT));
                DrawText(((DRAWITEMSTRUCT*)lparam)->hDC,text,-1,&led,0);

                led.top=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2;
                led.bottom=((DRAWITEMSTRUCT*)lparam)->rcItem.top+2+12;
                led.left=((DRAWITEMSTRUCT*)lparam)->rcItem.left+86;
                led.right=((DRAWITEMSTRUCT*)lparam)->rcItem.left+86+16;
                FillRect(((DRAWITEMSTRUCT*)lparam)->hDC,&led,CreateSolidBrush(status_led[((DRAWITEMSTRUCT*)lparam)->itemID-1] ? (drive_active_led[status_unit[((DRAWITEMSTRUCT*)lparam)->itemID-1]-8] ? 0xff00 : 0xff ) : 0x00));
            }
            return 0;
      case WM_COMMAND:
        handle_wm_command(wparam, lparam);
        return 0;
      case WM_ENTERSIZEMOVE:
        suspend_speed_eval();
        break;
      case WM_ENTERMENULOOP:
        suspend_speed_eval();
        ui_update_menus();
        break;
      case WM_KEYDOWN:
        kbd_handle_keydown(wparam, lparam);
        break;
      case WM_KEYUP:
        kbd_handle_keyup(wparam, lparam);
        break;
      case WM_SYSCOLORCHANGE:
        syscolorchanged = 1;
        break;
      case WM_DISPLAYCHANGE:
        displaychanged = 1;
        break;
      case WM_QUERYNEWPALETTE:
        querynewpalette = 1;
        break;
      case WM_PALETTECHANGED:
        if ((HWND) wparam != window)
            palettechanged = 1;
        break;
      case WM_CLOSE:
        if (MessageBox(window,
                       "Do you really want to exit?\n\n"
                       "All the data present in the emulated RAM will be lost.",
                       "VICE",
                       MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)
            == IDYES)
            DestroyWindow(window);
        return 0;
      case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
      case WM_PAINT:
        {
            RECT update_rect;

            if (GetUpdateRect(window, &update_rect, FALSE)) {
                PAINTSTRUCT ps;
                HDC hdc;
                int frame_coord[6];

                hdc = BeginPaint(window, &ps);

                frame_coord[0]=update_rect.left;
                frame_coord[1]=update_rect.top;
                frame_coord[2]=update_rect.right;
                frame_coord[3]=update_rect.bottom;

                translate_client_to_framebuffer(&frame_coord);

                //  Check if it's out
                if ((frame_coord[3]<=0) || (frame_coord[1]>=frame_coord[5]) ||
                    (frame_coord[2]<=0) || (frame_coord[0]>=frame_coord[4])) {
                    clear(hdc,frame_coord[0],frame_coord[1],frame_coord[2],frame_coord[3]);
                }

                //  Cut top
                if (frame_coord[1]<0) {
                    clear(hdc,frame_coord[0],frame_coord[1],frame_coord[2],0);
                    update_rect.top-=frame_coord[1];
                    frame_coord[1]=0;
                }
                //  Cut left
                if (frame_coord[0]<0) {
                    clear(hdc,frame_coord[0],frame_coord[1],0,frame_coord[3]);
                    update_rect.left-=frame_coord[0];
                    frame_coord[0]=0;
                }
                //  Cut bottom
                if (frame_coord[3]>frame_coord[5]) {
                    clear(hdc,frame_coord[0],frame_coord[5],frame_coord[2],frame_coord[3]);
                    update_rect.bottom-=frame_coord[3]-frame_coord[5];
                    frame_coord[3]=frame_coord[5];
                }
                //  Cut right
                if (frame_coord[2]>frame_coord[4]) {
                    clear(hdc,frame_coord[4],frame_coord[1],frame_coord[2],frame_coord[3]);
                    update_rect.right-=frame_coord[2]-frame_coord[4];
                    frame_coord[2]=frame_coord[4];
                }

                canvas_render(main_canvas, main_fbuff, frame_coord[0], frame_coord[1], update_rect.left, update_rect.top, update_rect.right-update_rect.left, update_rect.bottom-update_rect.top);

                EndPaint(window, &ps);

                return 0;
            } else
                break;
        }
    }

    return DefWindowProc(window, msg, wparam, lparam);
}
