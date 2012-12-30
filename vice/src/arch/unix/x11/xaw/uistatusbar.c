/*
 * uistatusbar.c - Xaw status-bar code, as far as it isn't in the drive
 *                 or tape status code.
 *
 * Written by
 *  Olaf Seibert <rhialto@falu.nl>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>

#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "uidrivestatus.h"
#include "uilib.h"
#include "uimenu.h"
#include "util.h"
#include "widgets/Canvas.h"
#include "videoarch.h"
#include "screenshot.h"
#include "vice-event.h"
#include "x11ui.h"
#include "uipalcontrol.h"

extern log_t ui_log;  // TODO to header file

Widget rec_button = NULL;       /* referenced in uiscreenshot.c */
static Widget event_recording_button = NULL;

static int statustext_display_time = 0;

static UI_CALLBACK(rec_button_callback)
{
    screenshot_stop_recording();
    XtUnmapWidget(rec_button);
}

static UI_CALLBACK(event_recording_button_callback)
{
    event_record_stop();
    XtUnmapWidget(event_recording_button);
}

void ui_create_status_bar(Widget pane, int width, Widget below, video_canvas_t *c, int app_shell)
{
    Widget speed_label, statustext_label;
    Widget drive_status[NUM_DRIVES];
    Widget drive_track_label[NUM_DRIVES], drive_led[NUM_DRIVES];
    Widget drive_led1[NUM_DRIVES], drive_led2[NUM_DRIVES];
    Widget tape_counter_label[NUM_TAPES];
    Widget tape_button_status[NUM_TAPES];
    Widget pal_ctrl_widget;
    int i;
    char *button_title;

#define DD      2               /* default distance */
#define BW      1               /* border width */

    Dimension height;
    Dimension led_width = 14, led_height = 5;
    Dimension led_dist = 6;
    Widget fromvert;

    speed_label = XtVaCreateManagedWidget("speedStatus",
                                            labelWidgetClass, pane,
                                            XtNlabel, "",
                                            XtNwidth, width / 3,
                                            XtNfromVert, below,
                                            XtNtop, XawChainBottom,
                                            XtNbottom, XawChainBottom,
                                            XtNjustify, XtJustifyLeft,
                                            XtNborderWidth, 0,
                                            XtNtip, _("Emulation Speed Status"),
                                            NULL);

    fromvert = speed_label;

    XtVaGetValues(speed_label, XtNheight, &height, NULL);

    if (machine_class != VICE_MACHINE_VSID) {
        Widget drivefromvert, drivefromhoriz;

        Arg args[] = {
            { XtNlabel, (XtArgVal)_("CRT Controls") },
            { XtNwidth, width / 3 - DD },
            { XtNfromVert, (XtArgVal)fromvert },
            { XtNvertDistance, DD + 2 * BW }, /* 2 + 2*border of drive_current_image */
            { XtNtop, XawChainBottom },
            { XtNbottom, XawChainBottom },
        };
        pal_ctrl_widget = build_pal_ctrl_widget(c, pane,
                                    args, util_arraysize(args));

        fromvert = pal_ctrl_widget;
        drivefromvert = below;
        drivefromhoriz = speed_label;

        for (i = 0; i < NUM_DRIVES; i++) {
            char *name;
            Widget form;
            int status_width = (width / 3) - DD;
            int d = i ^ 1;

            name = lib_msprintf("driveStatus%d", d + 1);
            form =
            drive_status[d] = XtVaCreateManagedWidget(name,
                                formWidgetClass, pane,
                                XtNmappedWhenManaged, False,
                                XtNwidth, status_width,
                                XtNheight, height,
                                XtNborderWidth, 0,
                                XtNdefaultDistance, 0,
                                XtNfromVert, drivefromvert,
                                XtNfromHoriz, drivefromhoriz,
                                XtNhorizDistance, DD,
                                XtNtop, XawChainBottom,
                                XtNbottom, XawChainBottom,
                                XtNleft, XawRubber,
                                XtNright, XawRubber,
                                NULL);
            lib_free(name);

            /* Position for the next Drive GUI element */
            if (i & 1) {
                drivefromhoriz = speed_label;
                drivefromvert = form;
            } else {
                drivefromhoriz = form;
            }

            name = lib_msprintf("driveTrack%d", d + 1);
            drive_track_label[d] = XtVaCreateManagedWidget(name,
                                commandWidgetClass, form,
                                XtNlabel, "",
                                XtNwidth, status_width - led_width - 2 * led_dist - DD,
                                XtNhorizDistance, 0,
                                XtNvertDistance, 0,
                                XtNtop, XawChainBottom,
                                XtNbottom, XawChainBottom,
                                XtNleft, XawChainLeft,
                                XtNright, XawChainRight,
                                XtNjustify, XtJustifyRight,
                                NULL);
            lib_free(name);

            /* single LED */

            name = lib_msprintf("driveLed%d", d + 1);
            drive_led[d] = XtVaCreateManagedWidget(name,
                                xfwfcanvasWidgetClass, form,
                                XtNmappedWhenManaged, False,
                                XtNwidth, led_width,
                                XtNheight, led_height,
                                XtNfromHoriz, drive_track_label[d],
                                XtNhorizDistance, led_dist,
                                XtNvertDistance, (height-led_height)/2 + BW,
                                XtNtop, XawChainBottom,
                                XtNbottom, XawChainBottom,
                                XtNleft, XawChainRight,
                                XtNright, XawChainRight,
                                XtNborderWidth, 0,
                                NULL);
            lib_free(name);

            /* double LEDs */

            name = lib_msprintf("driveLedA%d", d + 1);
            drive_led1[d] = XtVaCreateManagedWidget(name,
                                xfwfcanvasWidgetClass, form,
                                XtNmappedWhenManaged, False,
                                XtNwidth, led_width / 2 - BW,
                                XtNheight, led_height,
                                XtNfromHoriz, drive_track_label[d],
                                XtNhorizDistance, led_dist,
                                XtNvertDistance, (height-led_height)/2 + BW,
                                XtNtop, XawChainBottom,
                                XtNbottom, XawChainBottom,
                                XtNleft, XawChainRight,
                                XtNright, XawChainRight,
                                XtNborderWidth, BW,
                                NULL);
            lib_free(name);

            name = lib_msprintf("driveLedB%d", d + 1);
            drive_led2[d] = XtVaCreateManagedWidget(name,
                                xfwfcanvasWidgetClass, form,
                                XtNmappedWhenManaged, False,
                                XtNwidth, led_width / 2 - BW,
                                XtNheight, led_height,
                                XtNfromHoriz, drive_led1[d],
                                XtNhorizDistance, led_dist,
                                XtNvertDistance, (height-led_height)/2 + BW,
                                XtNtop, XawChainBottom,
                                XtNbottom, XawChainBottom,
                                XtNleft, XawChainRight,
                                XtNright, XawChainRight,
                                XtNborderWidth, BW,
                                NULL);
            lib_free(name);

            app_shells[app_shell].drive_widgets[d].track_label = drive_track_label[d];
            app_shells[app_shell].drive_widgets[d].driveled = drive_led[d];
            //XtUnmapWidget(drive_led[d]);
            app_shells[app_shell].drive_widgets[d].driveled1 = drive_led1[d];
            app_shells[app_shell].drive_widgets[d].driveled2 = drive_led2[d];
            //XtUnmapWidget(drive_led1[d]);
            //XtUnmapWidget(drive_led2[d]);
            app_shells[app_shell].drive_widgets[d].status = drive_status[d];
            strcpy(&(last_attached_images[d][0]), "");
            //XtMapWidget(app_shells[app_shell].drive_widgets[d].status);
        }
    }

    statustext_label = XtVaCreateManagedWidget("statustext",
                                                labelWidgetClass, pane,
                                                XtNwidth, width / 3 - 0, // 0 = 2*borderwidth
                                                XtNfromVert, drive_status[2],
                                                XtNtop, XawChainBottom,
                                                XtNbottom, XawChainBottom,
                                                XtNjustify, XtJustifyLeft,
                                                XtNlabel, "",
                                                XtNborderWidth, 0,
                                                NULL);

    button_title = util_concat(_("recording"), "...", NULL);
    rec_button = XtVaCreateManagedWidget("recButton",
                                            commandWidgetClass, pane,
                                            XtNmappedWhenManaged, False,
                                            XtNwidth, width / 6 - 2 - DD,
                                            XtNtip, _("click to stop recording"),
                                            XtNfromVert, drive_status[2],
                                            XtNfromHoriz, statustext_label,
                                            XtNtop, XawChainBottom,
                                            XtNbottom, XawChainBottom,
                                            XtNjustify, XtJustifyLeft,
                                            XtNlabel, button_title,
                                            NULL);
    lib_free(button_title);

    XtAddCallback(rec_button, XtNcallback, rec_button_callback, NULL);

    button_title = util_concat(_("event recording"), "...", NULL);
    event_recording_button = XtVaCreateManagedWidget("eventRecButton",
                                        commandWidgetClass, pane,
                                        XtNmappedWhenManaged, False,
                                        XtNtip, _("click to stop recording"),
                                        XtNwidth, width / 6 - DD,
                                        XtNfromVert, drive_status[2],
                                        XtNfromHoriz, rec_button,
                                        XtNtop, XawChainBottom,
                                        XtNbottom, XawChainBottom,
                                        XtNjustify, XtJustifyLeft,
                                        XtNlabel, button_title,
                                        NULL);
    lib_free(button_title);
    XtAddCallback(event_recording_button, XtNcallback, event_recording_button_callback, NULL);

    Dimension tape_btn_d = 4;
    Dimension tape_btn_w = 24;

    for (i = 0; i < NUM_TAPES; i++) {
        char *name = lib_msprintf("tapeCounter%d", i + 1);
        tape_counter_label[i] = XtVaCreateManagedWidget(name,
                                        commandWidgetClass, pane,
                                        XtNwidth, width / 3 - tape_btn_d - tape_btn_w - 4 * BW - DD, // 4 borderwidths for 2 widgets, 1 defaultDistance
                                        XtNjustify, XtJustifyLeft,
                                        XtNlabel, "Tape #1",
                                        XtNfromVert, drive_status[3],
                                        XtNfromHoriz, drive_status[3],
                                        XtNtop, XawChainBottom,
                                        XtNbottom, XawChainBottom,
                                        XtNright, XawChainRight,
                                        NULL);
        lib_free(name);

        name = lib_msprintf("tapeButtons%d", i + 1);
        tape_button_status[i] = XtVaCreateManagedWidget(name,
                                        simpleWidgetClass, pane,
                                        XtNwidth, tape_btn_w,
                                        XtNheight, height,
                                        XtNjustify, XtJustifyLeft,
                                        XtNfromVert, drive_status[3],
                                        XtNfromHoriz, tape_counter_label[i],
                                        XtNhorizDistance, tape_btn_d,
                                        XtNtop, XawChainBottom,
                                        XtNbottom, XawChainBottom,
                                        XtNleft, XawChainRight,
                                        XtNright, XawChainRight,
                                        NULL);
        lib_free(name);

        app_shells[app_shell].tape_widgets[i].counter_value = -1;
        app_shells[app_shell].tape_widgets[i].counter_label = tape_counter_label[i];
        app_shells[app_shell].tape_widgets[i].button_status = tape_button_status[i];

        build_tape_status_widget(&app_shells[app_shell].tape_widgets[i], pane, tape_btn_w, height);
    }

    app_shells[app_shell].speed_label = speed_label;
    app_shells[app_shell].statustext_label = statustext_label;
}

static void statusbar_setstatustext(const char *t)
{
    int num_app_shells = get_num_shells();
    int i;

    for (i = 0; i < num_app_shells; i++) {
        XtVaSetValues(app_shells[i].statustext_label,
                      XtNlabel, t,
                      XtNtip, t,
                      NULL);
    }
}

/* Show the speed index to the user.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);
    char *str = NULL;
    int num_app_shells = get_num_shells();

    if (percent) {
        str = lib_msprintf("%d%%, %dfps %s", percent_int, framerate_int, warp_flag ? _("(warp)") : "");
    }

    for (i = 0; i < num_app_shells; i++) {
        if (!percent) {
            XtVaSetValues(app_shells[i].speed_label, XtNlabel, warp_flag ? _("(warp)") : "", NULL);
        } else {
            XtVaSetValues(app_shells[i].speed_label, XtNlabel, str, NULL);
        }
    }
    lib_free(str);
    if (statustext_display_time > 0) {
        statustext_display_time--;
        if (statustext_display_time == 0) {
            statusbar_setstatustext("");
        }
    }

    if (!screenshot_is_recording()) {
        XtUnmapWidget(rec_button);
    }
}

void ui_display_recording(int recording_status)
{
    if (recording_status) {
        XtMapWidget(event_recording_button);
    } else {
        XtUnmapWidget(event_recording_button);
    }
}

void ui_display_playback(int playback_status, char *version)
{
}

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;
    int num_app_shells = get_num_shells();

    for (i = 0; i < num_app_shells; i++) {
        if (flag) {
            char *str;

            str = lib_msprintf(_("%s (paused)"), app_shells[i].title);
            XtVaSetValues(app_shells[i].shell, XtNtitle, str, NULL);
            lib_free(str);
        } else {
            XtVaSetValues(app_shells[i].shell, XtNtitle, app_shells[i].title, NULL);
        }
    }
}

static int stb = 1;
int ui_fullscreen_statusbar(struct video_canvas_s *canvas, int enable)
{
    if (stb) {
        log_message(ui_log, "Toggling of Statusbar/Menu in Xaw is not supported.");
        stb = 0;
    }
    return 0;
}

void ui_display_statustext(const char *text, int fade_out)
{
    log_message(LOG_DEFAULT, "%s", text);
    statusbar_setstatustext(text);
    if (fade_out) {
        statustext_display_time = 5;
    } else {
        statustext_display_time = 0;
    }
}

