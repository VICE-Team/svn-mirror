/*
 * ui.c - The user interface for Vice/2.
 *
 * Written by
 *  Thomas Bretz (tbretz@gsi.de)
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
#define INCL_WINSYS
#define INCL_GPILCIDS // vac++
#define INCL_WINSTDFILE
#define INCL_WINFRAMEMGR
#define INCL_WINWINDOWMGR
#define INCL_WINSCROLLBARS

#include "vice.h"

#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
//#ifdef __EMX__
//#include <sys/hw.h>
//#endif

#include "ui.h"
#include "ui_status.h"

//#ifdef __EMX__
//#include "dos.h"
//#endif
#include "machine.h"
#include "mon.h"
#include "resources.h"
#include "sound.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "archdep.h"

/*------------------------------------------------*/

static void uilog(char *s, int i)
{
    FILE *fl=fopen("output","a");
    fprintf(fl,"%s %i\n",s,i);
    fclose(fl);
}

/* ------------------------ ui resources ------------------------ */

/* Flag: Use keyboard LEDs?  */
static int use_leds;

static int set_use_leds(resource_value_t v)
{
    use_leds = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "UseLeds", RES_INTEGER, (resource_value_t) 1,
      (resource_value_t *) &use_leds, set_use_leds },
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

static cmdline_option_t cmdline_options[] = {
    { "-leds", SET_RESOURCE, 0, NULL, NULL, "UseLeds", (resource_value_t) 1,
      NULL, "Enable usage of PC keyboard LEDs" },
    { "+leds", SET_RESOURCE, 0, NULL, NULL, "UseLeds", (resource_value_t) 0,
      NULL, "Disable usage of PC keyboard LEDs" },
    { NULL },
};

int ui_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------ VICE/2 Status Window ------------------------ */


void ui_display_drive_led(int drive_number, int status)
{
    BYTE keyState[256];
    RECTL rectl=ui_status.rectl;
    int  height=rectl.yTop-rectl.yBottom;
    if (PM_winActive) {
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        if (status) keyState[VK_CAPSLOCK] |=  1;
        else        keyState[VK_CAPSLOCK] &= ~1;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    if (!ui_status.init || drive_number>1) return;
    rectl.xLeft   =rectl.xRight-(2-drive_number)*20-2;
    rectl.xRight  =rectl.xLeft+10+3;
    rectl.yTop    =height/2+10;
    rectl.yBottom =height/2+5;
    WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
    rectl.xLeft   +=1;
    rectl.xRight  -=1;
    rectl.yBottom +=1;
    rectl.yTop    -=1;
    WinFillRect(ui_status.hps, &rectl, status?4:SYSCLR_FIELDBACKGROUND);
}

void ui_display_drive_track(int drive_number, int drive_base,
                            double track_number)
{
    char str[40];
    RECTL rectl=ui_status.rectl;
    int height=rectl.yTop-rectl.yBottom;
    if (!ui_status.init || drive_number>1) return;
    rectl.xLeft   =rectl.xRight-(2-drive_number)*20-4;
    rectl.xRight  =rectl.xLeft+10+6;
    rectl.yBottom =height/2-10;
    rectl.yTop    =height/2+5;
    sprintf(str,"%.0f",track_number);
    WinDrawText(ui_status.hps, strlen(str), str, &rectl, 0, 0,
                DT_TEXTATTRS|DT_VCENTER|DT_CENTER|DT_ERASERECT|
                (track_number-(int)track_number?DT_UNDERSCORE:0));
    ui_status.lastTrack[drive_number]=track_number;
}

void ui_enable_drive_status(int state, int *drive_led_color)
{
    int i;
    RECTL rectl;
    int height=ui_status.rectl.yTop-ui_status.rectl.yBottom;
    if (!ui_status.init) return;
    for (i=0; i<2; i++) {
        rectl=ui_status.rectl;
        rectl.xLeft   = rectl.xRight-(2-i)*20-2;
        rectl.xRight  = rectl.xLeft+10+3;
        rectl.yTop    = height/2+10;
        rectl.yBottom = height/2+5;
        if ((state&(1<<i))) {
            ui_display_drive_track(i,0,ui_status.lastTrack[i]);
            WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
            rectl.xLeft   +=1;
            rectl.xRight  -=1;
            rectl.yBottom +=1;
            rectl.yTop    -=1;
        }
        WinFillRect(ui_status.hps, &rectl, SYSCLR_FIELDBACKGROUND);
    }
    ui_status.lastDriveState=state;
}

void ui_display_drive_current_image(int drive_number, const char *image)
{
}

/* tape-related ui, dummies so far */
void ui_set_tape_status(int tape_status)
{
}

void ui_display_tape_motor_status(int motor)
{
}

void ui_display_tape_control_status(int control)
{
}

void ui_display_tape_counter(int counter)
{
}

/* ------------------------ VICE only stuff ------------------------ */

int ui_init(int *argc, char **argv)
{
    return 0;
}

int ui_init_finish(void)
{
    ui_open_status_window();
    return 0;
}

void ui_main(char hotkey)
{
//    sound_suspend();
//    speed_index = vsync_get_avg_speed_index();
//    frame_rate = vsync_get_avg_frame_rate();

//    if (speed_index > 0.0 && frame_rate > 0.0)
//	sprintf(s, "%s emulator at %d%% speed, %d fps",
//		machine_name, (int)floor(speed_index), (int)floor(frame_rate));
//    suspend_speed_eval();
}

void ui_error(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    ui_OK_dialog("VICE/2 Error", txt);
}

ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    ui_OK_dialog("VICE/2 CPU JAM happend", txt);
    return UI_JAM_HARD_RESET;  // Always hard reset.
}

void ui_show_text(char *title, char *text)
{
    ui_OK_dialog(title, text);
}

void ui_update_menus(void)
{
/*    if (ui_main_menu != NULL)
        tui_menu_update(ui_main_menu);*/
}

int ui_extend_image_dialog(void)
{
    return ui_yesno_dialog("VICE/2 Extend Disk Image",
                           "Extend disk image in drive 8 to 40 tracks?");
}

/* ------------------------ OS/2 specific stuff ------------------------ */

void ui_OK_dialog(char *title, char *msg)
{
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, msg, title, 0, MB_OK);
}

int ui_yesno_dialog(char *title, char *msg)
{
    return (WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                          msg, title, 0, MB_YESNO)==MBID_YES);
}

int ui_file_dialog(char *title, char *drive,
                   char *path, char *button, char *result)
{
    FILEDLG filedlg;                      // File dialog info structure
    memset(&filedlg, 0, sizeof(FILEDLG)); // Initially set all fields to 0

    // Initialize used fields in the FILEDLG structure
    filedlg.cbSize      = sizeof(FILEDLG); // Size of structure
    filedlg.fl          = FDS_CENTER | FDS_OPEN_DIALOG;
    filedlg.pszTitle    = title;
    filedlg.pszOKButton = button;
    filedlg.pszIDrive   = drive;

    strcpy(filedlg.szFullFile, path); // Init Path, Filter (*.t64)

    // Display the dialog and get the file
    if (WinFileDlg(HWND_DESKTOP, HWND_DESKTOP, &filedlg))
    {
        strcpy(result, filedlg.szFullFile);
        return filedlg.lReturn;
    }
    else return FALSE;
}
