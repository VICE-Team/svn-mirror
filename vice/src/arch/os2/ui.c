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

#define INCL_WINFRAMEMGR
#define INCL_DOSSEMAPHORES

#include "vice.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
//#ifdef __EMX__
//#include <sys/hw.h>
//#endif

#include "pm/winaddon.h"

#include "ui.h"
#include "ui_status.h"

//#ifdef __EMX__
//#include "dos.h"
//#endif
#include "log.h"
#include "types.h"
#include "utils.h"
#include "vsync.h"
#include "archdep.h"
#include "machine.h"
#include "resources.h"

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

/* Tape related UI */
void ui_set_tape_status(int tape_status)
{   // attached or not attached
    BYTE keyState[256];
    RECTL rectl;

    if (!ui_status.init) return;

    ui_set_rectl_lwth(&rectl, 0, 234, 5, 8, 12);
    WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
    ui_set_rectl_lwth(&rectl, 0, 235, 3, 9, 10);
    WinFillRect(ui_status.hps, &rectl, tape_status?5:SYSCLR_FIELDBACKGROUND);
//    ui_status.lastTapeMotor=motor;
}

void ui_display_tape_motor_status(int motor)
{
    BYTE keyState[256];
    RECTL rectl;

    if (!ui_status.init) return;

    ui_set_rectl_lwth(&rectl, 0, 242, 5, 7, 12);
    WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
    ui_set_rectl_lwth(&rectl, 0, 243, 3, 8, 10);
    WinFillRect(ui_status.hps, &rectl, motor?5:SYSCLR_FIELDBACKGROUND);
    ui_status.lastTapeMotor=motor;
}

void ui_display_tape_control_status(int control)
{
    char str[40];
    RECTL rectl;
    if (!ui_status.init) return;

    sprintf(str,"C:%02i",control);
    ui_set_rectl_lwth(&rectl, 0, 220, 32, 6, 15);
    WinDrawText(ui_status.hps, strlen(str), str, &rectl, 0, 0,
                DT_TEXTATTRS|DT_VCENTER|DT_CENTER|DT_ERASERECT);
//    ui_status.lastTapeCounter=counter;
}

void ui_display_tape_counter(int counter)
{
    char str[40];
    RECTL rectl;
    if (!ui_status.init) return;

    sprintf(str,"%04i",counter);
    ui_set_rectl_lwth(&rectl, 0, 250, 32, 6, 15);
    WinDrawText(ui_status.hps, strlen(str), str, &rectl, 0, 0,
                DT_TEXTATTRS|DT_VCENTER|DT_CENTER|DT_ERASERECT);
    ui_status.lastTapeCounter=counter;
}

void ui_display_drive_led(int drive_number, int status)
{
    BYTE keyState[256];
    RECTL rectl;
    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    if (PM_winActive) {
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        if (status) keyState[VK_CAPSLOCK] |=  1;
        else        keyState[VK_CAPSLOCK] &= ~1;
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    DosReleaseMutexSem(hmtxKey);
    if (!ui_status.init || drive_number>1) return;

    ui_set_rectl_lwth(&rectl, 0, drive_number*20+153, 12, 2, 3);
    WinFillRect(ui_status.hps, &rectl, status?4:SYSCLR_FIELDBACKGROUND);
}

void ui_display_drive_track(int drive_number, int drive_base,
                            double track_number)
{
    char str[40];
    RECTL rectl;
    if (!ui_status.init || drive_number>1) return;

    sprintf(str,"%.0f",track_number);
    ui_set_rectl_lwth(&rectl, 0, drive_number*20+150, 16, 6, 15);
    WinDrawText(ui_status.hps, strlen(str), str, &rectl, 0, 0,
                DT_TEXTATTRS|DT_VCENTER|DT_CENTER|DT_ERASERECT|
                (track_number-(int)track_number?DT_UNDERSCORE:0));
    ui_status.lastTrack[drive_number]=track_number;
}

extern void ui_enable_drive_status(ui_drive_enable_t state,
                                   int *drive_led_color)
{
    int i;
    if (!ui_status.init) return;
    for (i=0; i<2; i++) {
        RECTL rectl;
        ui_set_rectl_lwth(&rectl, 0, i*20+152, 14, 1, 20);
        WinFillRect(ui_status.hps, &rectl, SYSCLR_FIELDBACKGROUND);
        if ((state&(1<<i))) {
            ui_set_rectl_lwth(&rectl, 0, i*20+152, 14, 1, 5);
            WinFillRect(ui_status.hps, &rectl, SYSCLR_BUTTONDARK);
            ui_display_drive_led(i,0);
            ui_display_drive_track(i,0,ui_status.lastTrack[i]);
        }
    }
    ui_status.lastDriveState=state;
}

#define WM_DRIVEIMAGE WM_USER+0x2
extern HWND hwndDrive;

void ui_display_drive_current_image(unsigned int drive_number,
                                    const char *image)
{
    const ULONG flCmd =
        DT_TEXTATTRS|DT_VCENTER|DT_LEFT|DT_ERASERECT|DT_WORDBREAK;

    WinSendMsg(hwndDrive, WM_DRIVEIMAGE, (void*)image, (void*)drive_number);

    if (image && ui_status.init)
    {
        RECTL rectl;
        char *text = xmalloc(strlen(image)+11);

        strcpy(ui_status.lastImage[drive_number], image);
        sprintf(text, "Drive %2i: %s", drive_number+8, image);

        ui_set_rectl_lrth(&rectl, drive_number+1, 4, 4, 0, 1);
        WinDraw3dLine(ui_status.hps, &rectl, 0);

        ui_set_rectl_lrtb(&rectl, drive_number+1, 6, 6, 2, 0);
        if (WinDrawText(ui_status.hps,
                        strlen(text), text, &rectl, 0, 0, flCmd)<strlen(text))
        {
            sprintf(text, "Drive %2i: %s", drive_number+8, strrchr(image, '\\')+1);
            WinDrawText(ui_status.hps, strlen(text), text, &rectl, 0, 0, flCmd);
        }
        free (text);
    }
}

/* ------------------------ VICE only stuff ------------------------ */

int ui_init(int *argc, char **argv)
{
    return 0;
}

int ui_init_finish(void)
{
    log_message(LOG_DEFAULT, "VICE/2-Port done by");
    log_message(LOG_DEFAULT, "T. Bretz.\n");
    ui_open_status_window();
    return 0;
}

void ui_error(const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    ui_OK_dialog("VICE/2 Error", txt);
}

void WinError(HWND hwnd, const char *format,...)
{
    char txt[1024];
    va_list ap;
    va_start(ap, format);
    vsprintf(txt, format, ap);
    WinOkDlg(hwnd, "VICE/2 Error", txt);
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

void ui_update_menus(void)
{
}

int ui_extend_image_dialog(void)
{
    return ui_yesno_dialog(HWND_DESKTOP, "VICE/2 Extend Disk Image",
                           "Extend disk image in drive 8 to 40 tracks?");
}

void ui_proc_write_msg(char* msg)
{
 // write(ui_pipefd[1], msg, strlen(msg));
}

/* ------------------------ OS/2 specific stuff ------------------------ */

void WinOkDlg(HWND hwnd, char *title, char *msg)
{
    WinMessageBox(HWND_DESKTOP, hwnd, msg, title, 0, MB_OK);
}

void ui_OK_dialog(char *title, char *msg)
{
    WinOkDlg(HWND_DESKTOP, title, msg);
}

int ui_yesno_dialog(HWND hwnd, char *title, char *msg)
{
    return (WinMessageBox(HWND_DESKTOP, hwnd,
                          msg, title, 0, MB_YESNO)==MBID_YES);
}

//-------------------------------------------------------------------
