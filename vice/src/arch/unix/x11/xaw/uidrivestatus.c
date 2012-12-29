/*
 * 1uidrivestatus.c - Xaw drive status code
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

#include <X11/Xaw/Tip.h>

#include "drive.h"
#include "drivetypes.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "uicontents.h"
#include "uidrivestatus.h"
#include "uifliplist.h"
#include "uilib.h"
#include "uimenu.h"
#include "util.h"
#include "x11ui.h"

/* #define DEBUG_X11UI */

#ifdef DEBUG_X11UI
#define DBG(_x_) log_debug _x_
#else
#define DBG(_x_)
#endif

char last_attached_images[NUM_DRIVES][ATT_IMG_SIZE];

/* Enabled drives.  */
ui_drive_enable_t enabled_drives = UI_DRIVE_ENABLE_NONE;
int tracks_shown;

/* Color of the drive active LED.  */
static int *drive_active_led;

static Widget left_drive_menu[NUM_DRIVES];
static Widget right_drive_menu[NUM_DRIVES];
static XtTranslations drive_menu_translations[NUM_DRIVES];

static void hide_drive_track(unsigned int drive_number);
static void ui_display_drive_current_image2(void);

void ui_destroy_drive_menu(int drive)
{
    if (drive >= 0 && drive < NUM_DRIVES) {
        if (left_drive_menu[drive]) {
            /* pop down the menu if it is still up */
            XtPopdown(left_drive_menu[drive]);
            XtDestroyWidget(left_drive_menu[drive]);
            left_drive_menu[drive] = 0;
        }
        if (right_drive_menu[drive]) {
            /* pop down the menu if it is still up */
            XtPopdown(right_drive_menu[drive]);
            XtDestroyWidget(right_drive_menu[drive]);
            right_drive_menu[drive] = 0;
        }
    }
}

void ui_set_drive_menu(int drive, ui_menu_entry_t *flipmenu)
{
    char *leftmenuname, *rightmenuname;
    int i;
    Widget w;
    int num_app_shells = get_num_shells();

    if (drive < 0 || drive >= NUM_DRIVES) {
        return;
    }

    leftmenuname = lib_msprintf("LeftDrive%iMenu", drive + 8);
    rightmenuname = lib_msprintf("RightDrive%iMenu", drive + 8);
    if (flipmenu != NULL) {
        w = ui_menu_create(leftmenuname, flipmenu, NULL);
        left_drive_menu[drive] = w;
        {
            char *image = last_attached_images[drive];
            if (image && image[0]) {
                if (right_drive_menu[drive]) {
                    /* pop down the menu if it is still up */
                    XtPopdown(right_drive_menu[drive]);
                    XtDestroyWidget(right_drive_menu[drive]);
                }
                right_drive_menu[drive] =
                    rebuild_contents_menu(rightmenuname, drive + 8, image);
            }
        }
    }

    if (!drive_menu_translations[drive]) {
        char *translation_table;

        translation_table = util_concat(
                "<Btn1Down>: "
                        "XawPositionSimpleMenu(", leftmenuname, ") "
                        "XtMenuPopup(", leftmenuname, ")\n",
                "Meta Shift <KeyDown>z: "
                        "FakeButton(1) "
                        "XawPositionSimpleMenu(", leftmenuname, ") "
                        "XtMenuPopup(", leftmenuname, ")\n",
                "<Btn3Down>: "
                        "XawPositionSimpleMenu(", rightmenuname, ") "
                        "XtMenuPopup(", rightmenuname, ")\n",
                "Meta Shift <KeyDown>x: "
                        "FakeButton(3) "
                        "XawPositionSimpleMenu(", rightmenuname, ") "
                        "XtMenuPopup(", rightmenuname, ")\n",
                NULL);
        drive_menu_translations[drive] =
                                XtParseTranslationTable(translation_table);
        lib_free(translation_table);
    }
    lib_free(leftmenuname);
    lib_free(rightmenuname);

    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive];
        if (n >= 0) {
            XtOverrideTranslations(app_shells[i].
                                        drive_widgets[n].track_label,
                                   drive_menu_translations[drive]);
        }
    }
}

void ui_init_drive_status_widget()
{
    int i;
    /* This is necessary because the status might have been set before we
    actually open the canvas window. e.g. by commandline */
    ui_enable_drive_status(enabled_drives, drive_active_led);

    /* make sure that all drive status widgets are initialized.
    This is needed for proper dual disk/dual led drives (8050, 8250). */
    for (i = 0; i < NUM_DRIVES; i++) {
        ui_display_drive_led(i, 1000, 1000);
    }
}

void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
    int i, j, num, k, true_emu;
    int drive_mapping[NUM_DRIVES];
    int num_app_shells = get_num_shells();
    int prev_enabled_drives = enabled_drives;

    num = 0;

    enabled_drives = enable;
    drive_active_led = drive_led_color;

    /* -1 should be safe, otherwise the display code in `ui_display_*'
       was wrong before. */
    memset(drive_mapping, -1, sizeof(drive_mapping));

    resources_get_int("DriveTrueEmulation", &true_emu);

    DBG(("ui_enable_drive_status: enable=%x true_emu=%d\n", enable, true_emu));

    if (true_emu) {
        /* num == number of drives which are active;
           drive_mapping[i] stores the widget number into which the i'th drive
           things should be displayed */
        for (i = 0, j = 1; i < NUM_DRIVES; i++, j <<= 1) {
            if (enabled_drives & j) {
                drive_mapping[i] = num++;
            }
        }
    } else {
        int hide_tracks = tracks_shown || prev_enabled_drives != enable;
        int prev_tracks_shown = tracks_shown;

        /*
         * Note that I'm changing the criterion used to decide whether
         * to show the UI elements for the drive. I think it is silly that
         * a drive which has no disk inserted is not shown, since the menu
         * has a handy item to insert (attach) such a disk. (OS)
         * However, if a drive type is changed to/from _NONE, this code
         * is apparently not called if TDE isn't active...
         */
        for (i = 0; i < NUM_DRIVES; i++) {
            /* if (strcmp(&(last_attached_images[i][0]), "") != 0) { //} */
            if (drive_context[i]->drive->type != DRIVE_TYPE_NONE) {
                drive_mapping[i] = num++;
                if (prev_tracks_shown ||
                        app_shells[0].drive_mapping[i] != drive_mapping[i]) {
                    hide_drive_track(i);
                }
            }
        }
    }

    for (i = 0; i < num_app_shells; i++) {
        /* now show `num' widgets ... */
        for (j = 0; j < num; j++) {
            XtMapWidget(app_shells[i].drive_widgets[j].status);
        }
        for (; j < NUM_DRIVES; j++) {
            XtUnmapWidget(app_shells[i].drive_widgets[j].status);
        }
        /* Show led widgets in true drive emulation mode */
        j = 0;
        if (true_emu > 0) {
            for (; j < num; j++) {
                for (k = 0; k < NUM_DRIVES; k++) {
                    if (drive_mapping[k] == j) {
                        break;
                    }
                }
                app_shells[i].drive_nleds[j] = drive_num_leds(k);
                if (app_shells[i].drive_nleds[j] == 1) {
                    XtMapWidget(app_shells[i].drive_widgets[j].driveled);
                    XtUnmapWidget(app_shells[i].drive_widgets[j].driveled1);
                    XtUnmapWidget(app_shells[i].drive_widgets[j].driveled2);
                } else {
                    XtUnmapWidget(app_shells[i].drive_widgets[j].driveled);
                    XtMapWidget(app_shells[i].drive_widgets[j].driveled1);
                    XtMapWidget(app_shells[i].drive_widgets[j].driveled2);
                }
            }
        }

        /* ...and hide the rest until `NUM_DRIVES' */
        for (; j < NUM_DRIVES; j++) {
            XtUnmapWidget(app_shells[i].drive_widgets[j].driveled);
            XtUnmapWidget(app_shells[i].drive_widgets[j].driveled1);
            XtUnmapWidget(app_shells[i].drive_widgets[j].driveled2);
        }
        for (j = 0; j < NUM_DRIVES; j++) {
            app_shells[i].drive_mapping[j] = drive_mapping[j];
        }
    }
    /* now update all image names from the cached names */
    ui_display_drive_current_image2();
}

void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base, unsigned int half_track_number)
{
    int i;
    /* FIXME: Fixed length.  */
    char str[256];
    double track_number = (double)half_track_number / 2.0;
    int d = drive_base ? (drive_base + drive_number) : (drive_number & 1);
    int num_app_shells = get_num_shells();

    sprintf(str, _("%d: Track %.1f"), d, (double)track_number);
    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive_number];
        Widget w;

        if (n < 0) {
            return;             /* bad mapping */
        }
        w = app_shells[i].drive_widgets[n].track_label;

        XtVaSetValues(w, XtNlabel, str, NULL);
    }

    tracks_shown = 1;
}

static void hide_drive_track(unsigned int drive_number)
{
    int i;
    /* FIXME: Fixed length.  */
    char str[256];
    int d = 8 + drive_number;
    int num_app_shells = get_num_shells();

    sprintf(str, _("Drive %d"), d);
    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive_number];
        Widget w;

        if (n < 0) {
            return;             /* bad mapping */
        }
        w = app_shells[i].drive_widgets[n].track_label;

        XtVaSetValues(w, XtNlabel, str, NULL);
    }

    tracks_shown = 0;
}

void ui_display_drive_led(int drive_number, unsigned int led_pwm1, unsigned int led_pwm2)
{
    Pixel pixel;
    int status = 0;
    int i;
    int num_app_shells = get_num_shells();

    if (led_pwm1 > 100) {
        status |= 1;
    }
    if (led_pwm2 > 100) {
        status |= 2;
    }

    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive_number];
        Widget w;
        Pixel on_pixel;

        if (n < 0) {
            return;             /* bad mapping */
        }

        on_pixel = drive_active_led[drive_number] ? drive_led_on_green_pixel
                                                  : drive_led_on_red_pixel;
        pixel = status ? on_pixel : drive_led_off_pixel;
        w = app_shells[i].drive_widgets[n].driveled;
        XtVaSetValues(w, XtNbackground, pixel, NULL);

        pixel = (status & 1) ? on_pixel : drive_led_off_pixel;
        w = app_shells[i].drive_widgets[n].driveled1;
        XtVaSetValues(w, XtNbackground, pixel, NULL);

        pixel = (status & 2) ? on_pixel : drive_led_off_pixel;
        w = app_shells[i].drive_widgets[n].driveled2;
        XtVaSetValues(w, XtNbackground, pixel, NULL);
    }
}

void ui_display_drive_current_image(unsigned int drive_number, const char *image)
{
    if (console_mode) {
        return;
    }

    if (drive_number >= NUM_DRIVES) {
        return;
    }

    /*if (strcmp(image, "") == 0) {
        image = "<detached>";
    }*/
    strncpy(&(last_attached_images[drive_number][0]), image, ATT_IMG_SIZE);

    /* update drive mapping */
    ui_enable_drive_status(enabled_drives, drive_active_led);
    uifliplist_update_menus(drive_number + 8, drive_number + 8);
}

static void ui_display_drive_current_image2 (void)
{
    int i, j;
    char *name;
    int num_app_shells = get_num_shells();

    /* Now update all fields according to drive_mapping */
    for (i = 0; i < num_app_shells; i++) {
        for (j = 0; j < NUM_DRIVES; j++) {
            int n = app_shells[i].drive_mapping[j];
            Widget w2;

            /* It is assumed that the j-1'th widget is not touched anymore.
               -> the drive mapping code fills the widgets up from 0 */

            /* first clear the j'th widget */
            w2 = app_shells[i].drive_widgets[j].track_label;
            XtVaSetValues(w2, XtNtip, NULL, NULL);

            if (n < 0) {
                continue;       /* j'th is drive not mapped */
            }

            /* now fill the j'th widget */
            w2 = app_shells[i].drive_widgets[n].track_label;

            util_fname_split(&(last_attached_images[j][0]), NULL, &name);
            XtVaSetValues(w2, XtNtip, name, NULL);
            lib_free(name);

            /* Also update drive menu; will call ui_set_drive_menu() */
            if (i == 0) {
                uifliplist_update_menus(8 + j, 8 + j);
            }
        }
    }
}

