/*
 * ui.c - The user interface for Vice/2.
 *
 * Written by
 *  Thomas Bretz <tbretz@gsi.de>
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

#define INCL_WINSYS        // SYSCLR_*
#define INCL_WININPUT      // VK_*
#define INCL_WINFRAMEMGR
#define INCL_DOSSEMAPHORES
#include <os2.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//#ifdef __EMX__
//#include <sys/hw.h>
//#endif

#include "ui.h"
#include "ui_status.h"

//#ifdef __EMX__
//#include "dos.h"
//#endif
#include "log.h"
#include "utils.h"
#include "archdep.h"
#include "dialogs.h"
#include "cmdline.h"
#include "resources.h"

/* ------------------------ ui resources ------------------------ */

HMTX hmtxKey;
int PM_winActive;
ui_status_t ui_status;

/* Flag: Use keyboard LEDs?  */
int use_leds;

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

int ui_init(int *argc, char **argv)
{
    return 0;
}

int ui_init_finish(void)
{
    log_message(LOG_DEFAULT, "VICE/2-Port done by");
    log_message(LOG_DEFAULT, "T. Bretz.\n");
    // ui_open_status_window();
    return 0;
}

/* --------------------------- Tape related UI -------------------------- */

void ui_set_tape_status(int tape_status)
{
    if (ui_status.lastTapeStatus==tape_status) return;
    ui_status.lastTapeStatus=tape_status;
    WinSendMsg(hwndDatasette, WM_TAPESTAT,
               (void*)ui_status.lastTapeCtrlStat, (void*)tape_status);
}

void ui_display_tape_motor_status(int motor)
{
    if (ui_status.lastTapeMotor==motor) return;
    ui_status.lastTapeMotor=motor;
    WinSendMsg(hwndDatasette, WM_SPINNING,
               (void*)motor, (void*)ui_status.lastTapeStatus);
}

void ui_display_tape_control_status(int control)
{
    if (ui_status.lastTapeCtrlStat==control) return;
    ui_status.lastTapeCtrlStat=control;
    WinSendMsg(hwndDatasette, WM_TAPESTAT,
               (void*)control, (void*)ui_status.lastTapeStatus);
}

void ui_display_tape_counter(int counter)
{
    if (ui_status.lastTapeCounter==counter) return;
    ui_status.lastTapeCounter=counter;
    WinSendMsg(hwndDatasette, WM_COUNTER, (void*)counter, 0);
}

/* --------------------------- Drive related UI ------------------------ */

void ui_display_drive_led(int drive_number, int status)
{
    BYTE keyState[256];

    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    if (PM_winActive && use_leds) {
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        keyState[VK_CAPSLOCK] = (status!=0);
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    DosReleaseMutexSem(hmtxKey);

    WinSendMsg(hwndDrive, WM_DRIVELEDS, (void*)drive_number, (void*)status);
}

void ui_display_drive_track(int drive_number, int drive_base,
                            double track_number)
{
    ui_status.lastTrack[drive_number]=track_number;
    WinSendMsg(hwndDrive, WM_TRACK,
               (void*)drive_number, (void*)(int)(track_number*2));
}

void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    ui_status.lastDriveState=state;
    WinSendMsg(hwndDrive, WM_DRIVESTATE, (void*)state, NULL);
    if (state&1) ui_display_drive_led(0, 0);
    if (state&2) ui_display_drive_led(1, 0);
}

void ui_display_drive_current_image(unsigned int drive_number,
                                    const char *image)
{
    const ULONG flCmd =
        DT_TEXTATTRS|DT_VCENTER|DT_LEFT|DT_ERASERECT|DT_WORDBREAK;

    if (image && *image)
    {
        int pos=0, i;
        while (strcmp(ui_status.imageHist[pos], image) &&
               ui_status.imageHist[pos][0] && pos<9) pos++;
        for (i=pos; i>0; i--)
            strcpy(ui_status.imageHist[i], ui_status.imageHist[i-1]);
        strcpy(ui_status.imageHist[0], image);
    }

    WinSendMsg(hwndDrive, WM_DRIVEIMAGE, (void*)image, (void*)drive_number);

    if (image)
        strcpy(ui_status.lastImage[drive_number], image);
}

/* --------------------------- Dialog Windows --------------------------- */

void ui_error(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                  txt, "VICE/2 Error", 0, MB_OK);
}

ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                  txt, "VICE/2 CPU JAM happend", 0, MB_OK);
    return UI_JAM_HARD_RESET;  // Always hard reset.
}

int ui_extend_image_dialog(void)
{
    return WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                         "Extend disk image in drive 8 to 40 tracks?",
                         "VICE/2 Extend Disk Image",
                         0, MB_YESNO)==MBID_YES;
}

//------------------------------------------------------------------------

void ui_update_menus(void)
{
}

void ui_proc_write_msg(char* msg)
{
}
