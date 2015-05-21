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
#define INCL_WINDIALOGS    // WinSendDlgItemMsg
#define INCL_WINBUTTONS    // BS_DEFAULT
#define INCL_WINFRAMEMGR
#define INCL_WINPOINTERS   // WinLoadPointer
#define INCL_DOSDATETIME   // Date and time values
#define INCL_DOSSEMAPHORES

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "ui.h"
#include "ui_status.h"

#include "dialogs.h"
#include "dlg-drive.h"
#include "dlg-monitor.h"
#include "dlg-datasette.h"
#include "snippets\\pmwin2.h"
#include "lib.h"
#include "log.h"
#include "util.h"
#include "archdep.h"
#include "cmdline.h"
#include "machine.h"
#include "resources.h"
#include "translate.h"

/* ------------------------ ui resources ------------------------ */

HMTX hmtxKey;
int PM_winActive;
ui_status_t ui_status;

/* Flag: Use keyboard LEDs?  */
int use_leds;

static int set_use_leds(int val, void *param)
{
    use_leds = val ? 1 : 0;

    return 0;
}

static const resource_int_t resources_int[] = {
    { "UseLeds", 1, RES_EVENT_NO, NULL,
      &use_leds, set_use_leds, NULL },
    { NULL }
};

int ui_resources_init(void)
{
    return (machine_class == VICE_MACHINE_VSID) ? 0 : resources_register_int(resources_int);
}

void ui_resources_shutdown(void)
{
}

static const cmdline_option_t cmdline_options[] = {
    { "-leds", SET_RESOURCE, 0,
      NULL, NULL, "UseLeds", (resource_value_t) 1,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Enable usage of PC keyboard LEDs" },
    { "+leds", SET_RESOURCE, 0,
      NULL, NULL, "UseLeds", (resource_value_t) 0,
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      IDCLS_UNUSED, IDCLS_UNUSED,
      NULL, "Disable usage of PC keyboard LEDs" },
    { NULL }
};

int ui_cmdline_options_init(void)
{
    return (machine_class == VICE_MACHINE_VSID) ? 0 : cmdline_register_options(cmdline_options);
}

int machine_ui_init(void)
{
    return 0;
}

int c64ui_init(void)
{
    return 0;
}

int c64dtvui_init(void)
{
    return 0;
}

int c64scui_init(void)
{
    return 0;
}

int scpu64ui_init(void)
{
    return 0;
}

int c128ui_init(void)
{
    return 0;
}

int vic20ui_init(void)
{
    return 0;
}

int petui_init(void)
{
    return 0;
}

int plus4ui_init(void)
{
    return 0;
}

int cbm2ui_init(void)
{
    return 0;
}

int cbm5x0ui_init(void)
{
    return 0;
}

int ui_init(int *argc, char **argv)
{
    return 0;
}

void ui_shutdown(void)
{
}

void c64ui_shutdown(void)
{
}

void c64scui_shutdown(void)
{
}

void scpu64ui_shutdown(void)
{
}

void c64dtvui_shutdown(void)
{
}

void c128ui_shutdown(void)
{
}

void vic20ui_shutdown(void)
{
}

void petui_shutdown(void)
{
}

void plus4ui_shutdown(void)
{
}

void cbm2ui_shutdown(void)
{
}

void cbm5x0ui_shutdown(void)
{
}

int ui_init_finish(void)
{
    DATETIME DT = { 0 }; // Date and time information

    DosGetDateTime(&DT);

    log_message(LOG_DEFAULT, "VICE/2-Port done by");
    log_message(LOG_DEFAULT, "T. Bretz.\n");
    log_message(LOG_DEFAULT, "M. van den Heuvel.\n");

    log_message(LOG_DEFAULT, "Starting Vice/2 at %d.%d.%d %d:%02d:%02d\n", DT.day, DT.month, DT.year, DT.hours, DT.minutes, DT.seconds);

    return 0;
}

int ui_init_finalize(void)
{
    return 0;
}

/* --------------------------- Tape related UI -------------------------- */

void ui_set_tape_status(int tape_status)
{
    if (ui_status.lastTapeStatus == tape_status) {
        return;
    }
    ui_status.lastTapeStatus = tape_status;
    WinSendMsg(hwndDatasette, WM_TAPESTAT, (void*)ui_status.lastTapeCtrlStat, (void*)tape_status);
}

void ui_display_tape_motor_status(int motor)
{
    if (ui_status.lastTapeMotor == motor) {
        return;
    }
    ui_status.lastTapeMotor = motor;
    WinSendMsg(hwndDatasette, WM_SPINNING, (void*)motor, (void*)ui_status.lastTapeStatus);
}

void ui_display_tape_control_status(int control)
{
    if (ui_status.lastTapeCtrlStat == control) {
        return;
    }
    ui_status.lastTapeCtrlStat = control;
    WinSendMsg(hwndDatasette, WM_TAPESTAT, (void*)control, (void*)ui_status.lastTapeStatus);
}

void ui_display_tape_counter(int counter)
{
    if (ui_status.lastTapeCounter == counter) {
        return;
    }
    ui_status.lastTapeCounter = counter;
    WinSendMsg(hwndDatasette, WM_COUNTER, (void*)counter, 0);
}

void ui_display_tape_current_image(const char *image)
{
}

/* --------------------------- Drive related UI ------------------------ */

void ui_display_drive_led(int drive_number, unsigned int led_pwm1, unsigned int led_pwm2)
{
    BYTE keyState[256];
    int status = 0;

    if (led_pwm1 > 100) {
        status |= 1;
    }
    if (led_pwm2 > 100) {
        status |= 2;
    }

    DosRequestMutexSem(hmtxKey, SEM_INDEFINITE_WAIT);
    if (PM_winActive && use_leds) {
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, FALSE);
        keyState[VK_CAPSLOCK] = (status != 0);
        WinSetKeyboardStateTable(HWND_DESKTOP, keyState, TRUE);
    }
    DosReleaseMutexSem(hmtxKey);

    WinSendMsg(hwndDrive, WM_DRIVELEDS, (void*)drive_number, (void*)status);
}

void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base, unsigned int half_track_number)
{
    double track_number = (double)half_track_number / 2.0;

    ui_status.lastTrack[drive_number] = track_number;
    WinSendMsg(hwndDrive, WM_TRACK, (void*)drive_number, (void*)(int)(track_number * 2));
}

void ui_enable_drive_status(ui_drive_enable_t state, int *drive_led_color)
{
    ui_status.lastDriveState = state;
    WinSendMsg(hwndDrive, WM_DRIVESTATE,(void*)state, NULL);
    if (state & 1) {
        ui_display_drive_led(0, 0, 0);
    }
    if (state & 2) {
        ui_display_drive_led(1, 0, 0);
    }
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
    if (image && *image) {
        int pos = 0, i;

        while (strcmp(ui_status.imageHist[pos], image) && ui_status.imageHist[pos][0] && pos < 9) {
            pos++;
        }
        for (i = pos; i > 0; i--) {
            strcpy(ui_status.imageHist[i], ui_status.imageHist[i - 1]);
        }
        strcpy(ui_status.imageHist[0], image);
    }

    WinSendMsg(hwndDrive, WM_DRIVEIMAGE, (void*)image, (void*)drive_number);

    if (image) {
        strcpy(ui_status.lastImage[drive_number], image);
    }
}


void ui_display_recording(int recording_status)
{
}

void ui_display_playback(int playback_status, char *version)
{
}

void ui_display_event_time(unsigned int current, unsigned int total)
{
}

void ui_display_joyport(BYTE *joyport)
{
}

void ui_display_volume(int vol)
{
}

/* --------------------------- Dialog Windows --------------------------- */

void ui_error(const char *format,...)
{
    char *txt, *tmp;
    va_list ap;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
    va_end(ap);
    txt = util_concat(" Error in emulation thread:\n ", tmp, NULL);
    lib_free(tmp);

    log_message(LOG_DEFAULT, txt);
    ViceErrorDlg(HWND_DESKTOP, PTR_SKULL, txt);

    lib_free(txt);
}

void ui_message(const char *format,...)
{
    char *txt, *tmp;
    va_list ap;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
    va_end(ap);
    txt = util_concat(" Message from emulation thread:\n ", tmp, NULL);
    lib_free(tmp);

    ViceErrorDlg(HWND_DESKTOP, PTR_SKULL, txt);

    lib_free(txt);
}

ui_jam_action_t ui_jam_dialog(const char *format,...)
{
    ULONG rc;
    char *txt, *tmp;
    va_list ap;
    const int sz = sizeof(MB2INFO) + 3 * sizeof(MB2D);
    MB2INFO *mb = malloc(sz);

    mb->cb = sz;
    mb->hIcon = WinLoadPointer(HWND_DESKTOP, NULLHANDLE, PTR_SKULL);
    mb->cButtons = 4;
    mb->flStyle = MB_CUSTOMICON | WS_VISIBLE;
    mb->hwndNotify = NULLHANDLE;
    strcpy(mb->mb2d[0].achText, "  ~Hard Reset  ");
    strcpy(mb->mb2d[1].achText, "  ~Soft Reset  ");
    strcpy(mb->mb2d[2].achText, "  ~Monitor     ");
    strcpy(mb->mb2d[3].achText, "  ~Continue    ");
    mb->mb2d[0].idButton = UI_JAM_HARD_RESET;
    mb->mb2d[1].idButton = UI_JAM_RESET;
    mb->mb2d[2].idButton = UI_JAM_MONITOR;
    mb->mb2d[3].idButton = UI_JAM_NONE;
    mb->mb2d[0].flStyle = BS_DEFAULT;
    mb->mb2d[1].flStyle = 0;
    mb->mb2d[2].flStyle = 0;

    va_start(ap, format);
    tmp = lib_mvsprintf(format, ap);
    va_end(ap);
    txt = util_concat("    Chipset reported:\n ", tmp, NULL);
    lib_free(tmp);

    rc = WinMessageBox2(HWND_DESKTOP, HWND_DESKTOP, txt, "VICE/2 Error", 0, mb);
    lib_free(txt);
    lib_free(mb);

    //
    // open monitor dialog
    //
    if (rc == UI_JAM_MONITOR) {
        WinShowWindow(hwndMonitor, 1);
    }

    return rc;
}

int ui_extend_image_dialog(void)
{
    return WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, "Do you want to extend the disk image?", "VICE/2 Extend Disk Image", 0, MB_YESNO) == MBID_YES;
}

//------------------------------------------------------------------------

void ui_update_menus(void)
{
}

void ui_display_statustext(const char *text, int fade_out)
{
}

char* ui_get_file(const char *format,...)
{
    return NULL;
}
