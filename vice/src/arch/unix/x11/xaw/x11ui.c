/*
 * x11ui.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  André Fachat <fachat@physik.tu-chemnitz.de>
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Support for multiple visuals and depths by
 *  Teemu Rantanen <tvr@cs.hut.fi>
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

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/AsciiText.h>

#include <X11/keysym.h>

#ifdef HAVE_X11_SUNKEYSYM_H
#include <X11/Sunkeysym.h>
#endif

#ifdef HAVE_LIBXPM
#ifdef HAVE_XPM_H
#include <xpm.h>
#endif
#ifdef HAVE_X11_XPM_H
#include <X11/xpm.h>
#endif
#endif

#include "drive.h"
#include "fullscreenarch.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mouse.h"
#include "mousedrv.h"
#include "psid.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uicolor.h"
#include "uifliplist.h"
#include "uihotkey.h"
#include "uilib.h"
#include "uimenu.h"
#include "uisettings.h"
#include "util.h"
#include "vsync.h"
#include "widgets/Canvas.h"
#include "widgets/FileSel.h"
#include "widgets/TextField.h"
#include "video.h"
#include "videoarch.h"
#include "screenshot.h"
#include "event.h"
#include "x11ui.h"

/* FIXME: We want these to be static.  */
Visual *visual;
static int have_truecolor;

static Display *display;
/*static*/ int screen;
static int depth;

/* UI logging goes here.  */
static log_t ui_log = LOG_ERR;
extern log_t vsid_log;

Widget canvas, pane;

Cursor blankCursor;
/*static*/ int cursor_is_blank = 0;
static video_canvas_t *ui_cached_video_canvas;

static void ui_display_drive_current_image2(void);

/* ------------------------------------------------------------------------- */

void ui_restore_mouse(void)
{
#if 0
    if (fullscreen_is_enabled)
        return;
#endif
    if (_mouse_enabled && cursor_is_blank) {
        XUndefineCursor(display,XtWindow(canvas));
        XUngrabPointer(display, CurrentTime);
        XUngrabKeyboard(display, CurrentTime);
        cursor_is_blank = 0; 
    }
}

static void initBlankCursor(void)
{
    static char no_data[] = { 0,0,0,0, 0,0,0,0 };
    static Pixmap blank;
    XColor trash, dummy;

    XAllocNamedColor(display,
                     DefaultColormapOfScreen(DefaultScreenOfDisplay(display)),
                     "black",&trash,&dummy);

    blank = XCreateBitmapFromData(display, XtWindow(canvas),
                                  no_data, 8,8);

    blankCursor = XCreatePixmapCursor(display,
                                      blank,
                                      blank,
                                      &trash, &trash, 0, 0);
}

static void mouse_handler1351(Widget w, XtPointer client_data, XEvent *report,
                              Boolean *ctd)
{
    if (!_mouse_enabled) return;

    switch(report->type) {
      case MotionNotify:
        mouse_move(report->xmotion.x,report->xmotion.y);
        break;
      case ButtonPress:
      case ButtonRelease:
        mouse_button(report->xbutton.button - 1, (report->type == ButtonPress));
        break;
    }
}

/* ------------------------------------------------------------------------- */

static int popped_up_count = 0;

/* Left-button and right-button menu.  */
static Widget left_menu, right_menu, drive8_menu, drive9_menu;

/* Translations for the left and right menus.  */
static XtTranslations left_menu_translations, right_menu_translations;
static XtTranslations left_menu_disabled_translations, right_menu_disabled_translations;
static XtTranslations drive8_menu_translations = NULL, drive9_menu_translations = NULL;

/* Application context. */
static XtAppContext app_context;

/* This is needed to catch the `Close' command from the Window Manager. */
static Atom wm_delete_window;

/* Toplevel widget. */
Widget _ui_top_level = NULL;
Widget status_bar = NULL;
Widget rec_button = NULL;
static Widget event_recording_button = NULL;
static int statustext_display_time = 0;

/* Our colormap. */
/*static*/ Colormap colormap;

/* Application icon.  */
static Pixmap icon_pixmap;

/* Enabled drives.  */
ui_drive_enable_t enabled_drives;

/* Color of the drive active LED.  */
static int *drive_active_led;

/* This allows us to pop up the transient shells centered to the last visited
   shell. */
static Widget last_visited_app_shell = NULL;
#define MAX_APP_SHELLS 10
static struct {
    String title;
    Widget shell;
    Widget canvas;
    Widget speed_label;
    Widget statustext_label;
    struct {
        Widget track_label;
        Widget driveled;
	Widget current_image;
	/* those two replace the single LED widget when SFD1001 is selected */
        Widget driveled1;
        Widget driveled2;
    } drive_widgets[NUM_DRIVES];
    int drive_mapping[NUM_DRIVES];
    int drive_nleds[NUM_DRIVES];
} app_shells[MAX_APP_SHELLS];

static int num_app_shells = 0;

char last_attached_images[NUM_DRIVES][256];

/* Pixels for updating the drive LED's state.  */
Pixel drive_led_on_red_pixel, drive_led_on_green_pixel, drive_led_off_pixel;

/* If != 0, we should save the settings. */
/* static int resources_have_changed = 0; */

static char *filesel_dir = NULL;

/* ------------------------------------------------------------------------- */

void ui_check_mouse_cursor()
{
    int i;
#if 0
    int window_doublesize;
    if (fullscreen_is_enabled)
        return;
#endif
    if (_mouse_enabled) {
        for (i = 0; i < num_app_shells; i++) {
            XtOverrideTranslations(app_shells[i].canvas, left_menu_disabled_translations);
            XtOverrideTranslations(app_shells[i].canvas, right_menu_disabled_translations);
        }
#if 0
        if (fullscreen_is_enabled) {
            if (resources_get_int("FullscreenDoubleSize",
                &window_doublesize) < 0)
                return;
        } else
#endif

	if (ui_cached_video_canvas->videoconfig->doublesizex)
	    mouse_accelx = 2;   
	else
	    mouse_accelx = 4;
	
	if (ui_cached_video_canvas->videoconfig->doublesizey)
	    mouse_accely = 2;   
	else
	    mouse_accely = 4;

        XDefineCursor(display,XtWindow(canvas), blankCursor);
        cursor_is_blank = 1;

        XGrabKeyboard(display, XtWindow(canvas),
                      1, GrabModeAsync,
                      GrabModeAsync,  CurrentTime);
        XGrabPointer(display, XtWindow(canvas), 1,
                     PointerMotionMask | ButtonPressMask |
                     ButtonReleaseMask,
                     GrabModeAsync, GrabModeAsync,
                     XtWindow(canvas),
                     None, CurrentTime);
    } else if (cursor_is_blank) {
        XUndefineCursor(display,XtWindow(canvas));
        XUngrabPointer(display, CurrentTime);
        XUngrabKeyboard(display, CurrentTime);
        for (i = 0; i < num_app_shells; i++) {
            XtOverrideTranslations(app_shells[i].canvas, left_menu_translations);
            XtOverrideTranslations(app_shells[i].canvas, right_menu_translations);
        }
    }
}

/* ------------------------------------------------------------------------- */

static int alloc_colormap(void);
static Widget build_file_selector(Widget parent, ui_button_t *button_return);
static Widget build_error_dialog(Widget parent, ui_button_t *button_return,
                                 const String message);
static Widget build_input_dialog(Widget parent, ui_button_t *button_return,
                                 Widget *InputDialogLabel,
                                 Widget *InputDialogField);
static Widget build_show_text(Widget parent, ui_button_t *button_return,
                              const String text, int width, int height);
static Widget build_confirm_dialog(Widget parent,
                                   ui_button_t *button_return,
                                   Widget *ConfirmDialogMessage);
static void close_action(Widget w, XEvent *event, String *params,
                         Cardinal *num_params);

UI_CALLBACK(enter_window_callback);
UI_CALLBACK(exposure_callback_shell);
UI_CALLBACK(exposure_callback_canvas);
static UI_CALLBACK(rec_button_callback)
{
    screenshot_stop_recording();
    XtUnrealizeWidget(rec_button);
}

static UI_CALLBACK(event_recording_button_callback)
{
    event_record_stop();
    XtUnrealizeWidget(event_recording_button);
}

/* ------------------------------------------------------------------------- */

static String fallback_resources[] = {
    "*font:					   -*-lucida-bold-r-*-*-12-*",
    "*Command.font:			           -*-lucida-bold-r-*-*-12-*",
    "*fileSelector.width:			     380",
    "*fileSelector.height:			     300",
    "*inputDialog.inputForm.borderWidth:	     0",
    "*inputDialog.inputForm.field.width:	     300",
    "*inputDialog.inputForm.field.scrollHorizontal:  True",
    "*inputDialog.inputForm.label.width:	     250",
    "*inputDialog.inputForm.label.borderWidth:	     0",
    "*inputDialog.inputForm.label.internalWidth:     0",
    "*inputDialog.buttonBox.borderWidth:	     0",
    "*errorDialog.messageForm.borderWidth:	     0",
    "*errorDialog.buttonBox.borderWidth:	     0",
    "*errorDialog.messageForm.label.borderWidth:     0",
    "*jamDialog.messageForm.borderWidth:	     0",
    "*jamDialog.buttonBox.borderWidth:		     0",
    "*jamDialog.messageForm.label.borderWidth:       0",
    "*infoDialogShell.width:			     380",
    "*infoDialogShell.height:			     290",
    "*infoDialog.textForm.infoString.borderWidth:    0",
    "*infoDialog.textForm.borderWidth:		     0",
    "*infoDialog.textForm.defaultDistance:	     0",
    "*infoDialog.buttonBox.borderWidth:		     0",
    "*infoDialog.buttonBox.internalWidth:	     5",
    "*infoDialog.textForm.infoString.internalHeight: 0",
    "*confirmDialogShell.width:			     300",
    "*confirmDialog.messageForm.message.borderWidth: 0",
    "*confirmDialog.messageForm.message.height:      20",
    "*showText.textBox.text.width:		     480",
    "*showText.textBox.text.height:		     305",
    "*showText.textBox.text*font:       -*-lucidatypewriter-medium-r-*-*-12-*",
    "*okButton.label:				     Confirm",
    "*cancelButton.label:			     Cancel",
    "*closeButton.label:			     Dismiss",
    "*yesButton.label:				     Yes",
    "*resetButton.label:			     Reset",
    "*hardResetButton.label:                         Hard Reset",
    "*monButton.label:			   	     Monitor",
    "*noneButton.label:			   	     Continue",
    "*debugButton.label:		   	     XDebugger",
    "*noButton.label:				     No",
    "*licenseButton.label:			     License...",
    "*noWarrantyButton.label:			     No warranty!",
    "*contribButton.label:			     Contributors...",
    "*Text.translations:			     #override \\n"
    "                                                <Key>Return: no-op()\\n"
    "						     <Key>Linefeed: no-op()\\n"
    "						     Ctrl<Key>J: no-op() \\n",

    /* Default color settings (suggestions are welcome...) */
    "*foreground:				     black",
    "*background:				     gray80",
    "*borderColor:				     black",
    "*internalBorderColor:			     black",
    "*TransientShell*Dialog.background:		     gray80",
    "*TransientShell*Label.background:		     gray80",
    "*TransientShell*Box.background:		     gray80",
    "*fileSelector.background:			     gray80",
    "*Command.background:			     gray90",
    "*Menubutton.background:		             gray80",
    "*Scrollbar.background:		             gray80",
    "*Form.background:				     gray80",
    "*Label.background:				     gray80",
    "*Canvas.background:                             black",
    "*driveTrack1.font:                          -*-helvetica-medium-r-*-*-12-*",
    "*driveTrack2.font:                          -*-helvetica-medium-r-*-*-12-*",
    "*driveCurrentImage1.font:                   -*-helvetica-medium-r-*-*-12-*",
    "*driveCurrentImage2.font:                   -*-helvetica-medium-r-*-*-12-*",
    "*driveTrack3.font:                          -*-helvetica-medium-r-*-*-12-*",
    "*driveTrack4.font:                          -*-helvetica-medium-r-*-*-12-*",
    "*driveCurrentImage3.font:                   -*-helvetica-medium-r-*-*-12-*",
    "*driveCurrentImage4.font:                   -*-helvetica-medium-r-*-*-12-*",
    "*speedStatus.font:                         -*-helvetica-medium-r-*-*-12-*",
    "*statustext.font:                         -*-helvetica-medium-r-*-*-8-*",

    NULL
};

/* ------------------------------------------------------------------------- */

static unsigned int wm_command_size;
static unsigned char *wm_command_data = NULL;
static Atom wm_command_atom;
static Atom wm_command_type_atom;

static void prepare_wm_command_data(int argc, char **argv)
{
    unsigned int offset, i;

    wm_command_size = 0;
    for (i = 0; i < (unsigned int) argc; i++)
        wm_command_size += strlen(argv[i]) + 1;

    wm_command_data = lib_malloc(wm_command_size);

    offset = 0;
    for (i = 0; i < (unsigned int) argc; i++) {
        unsigned int len;

        len = strlen(argv[i]);
        memcpy(wm_command_data + offset, argv[i], len);
        wm_command_data[offset + len] = 0;
        offset += len + 1;
    }
}

static void finish_prepare_wm_command(void)
{
    wm_command_atom = XInternAtom(display, "WM_COMMAND", False);
    wm_command_type_atom = XInternAtom(display, "STRING", False);
}

void archdep_ui_init(int argc, char *argv[])
{
    if (console_mode) {
        return;
    }
}

/* Initialize the GUI and parse the command line. */
int ui_init(int *argc, char **argv)
{
    static XtActionsRec actions[] = {
        { "Close", close_action },
    };

    prepare_wm_command_data(*argc, argv);

    /* Create the toplevel. */
    _ui_top_level = XtAppInitialize(&app_context, "VICE", NULL, 0, argc, argv,
                                    fallback_resources, NULL, 0);
    if (!_ui_top_level)
        return -1;

    display = XtDisplay(_ui_top_level);
    screen = XDefaultScreen(display);
    atexit(ui_autorepeat_on);

    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XtAppAddActions(app_context, actions, XtNumber(actions));

    ui_common_init();

    enabled_drives = UI_DRIVE_ENABLE_NONE;

    finish_prepare_wm_command();

    return 0;
}

void ui_shutdown(void)
{
    int i;

    for (i = 0; i < num_app_shells; i++)
        lib_free(app_shells[i].title);

    lib_free(wm_command_data);
    lib_free(filesel_dir);

    ui_common_shutdown();

    uimenu_shutdown();
}

typedef struct {
    char *name;
    int class;
} namedvisual_t;

/* Continue GUI initialization after resources are set. */
int ui_init_finish(void)
{
    static namedvisual_t classes[] = {
        { "PseudoColor", PseudoColor },
        { "TrueColor", TrueColor },
        { "StaticGray", StaticGray },
        { NULL }
    };
    XVisualInfo visualinfo;

    ui_log = log_open("X11");

    resources_get_int("DisplayDepth", &depth);

    if (depth != 0) {
        int i;

        for (i = 0; classes[i].name != NULL; i++) {
            if (XMatchVisualInfo(display, screen, depth, classes[i].class,
                                 &visualinfo))
                break;
        }
        if (!classes[i].name) {
            log_error(ui_log,
                      "This display does not support suitable %dbit visuals.",
                      depth);
            log_error(ui_log,
                      "Please select a bit depth supported by your display.");
            return -1;
        } else {
            log_message(ui_log, "Found %dbit/%s visual.",
                        depth, classes[i].name);
            have_truecolor = (classes[i].class == TrueColor);
        }
    } else {
        /* Autodetect. */
        int i, j, done;
        int depths[8];

        depths[0] = DefaultDepth(display, screen);
        depths[1] = 0;

        for (i = done = 0; depths[i] != 0 && !done; i++)
            for (j = 0; classes[j].name != NULL; j++) {
                if (XMatchVisualInfo(display, screen, depths[i],
                                     classes[j].class, &visualinfo)) {
                    depth = depths[i];
                    log_message(ui_log, "Found %dbit/%s visual.",
                                depth, classes[j].name);
                    have_truecolor = (classes[j].class == TrueColor);
                    done = 1;
                    break;
                }
            }
        if (!done) {
            log_error(ui_log, "Cannot autodetect a proper visual.");
            return -1;
        }
    }

    visual = visualinfo.visual;

    /* Allocate the colormap. */
    alloc_colormap();

    /* Recreate _ui_top_level to support non-default display depths.  */

    /* Goodbye...  */
    XtDestroyWidget(_ui_top_level);

    /* Create the new `_ui_top_level'.  */
    _ui_top_level = XtVaAppCreateShell(machine_name, "VICE",
                                       applicationShellWidgetClass, display,
                                       XtNvisual, visual,
                                       XtNdepth, depth,
                                       XtNcolormap, colormap,
                                       XtNmappedWhenManaged, False,
                                       XtNwidth, 1,
                                       XtNheight, 1,
                                       NULL);
    XtRealizeWidget(_ui_top_level);

    /* Set the `WM_COMMAND' property in the new _ui_top_level. */
    XChangeProperty(display,
                    XtWindow(_ui_top_level),
                    wm_command_atom,
                    wm_command_type_atom,
                    8,
                    PropModeReplace,
                    wm_command_data,
                    wm_command_size);

#ifdef HAVE_FULLSCREEN
    if (fullscreen_init() != 0)
	log_warning(ui_log, "Some fullscreen devices aren't initialized properly.");
#endif 

    return ui_menu_init(app_context, display, screen);
}

int ui_init_finalize(void)
{
    return 0;
}

int x11ui_get_display_depth(void)
{
    return depth;
}

Display *x11ui_get_display_ptr(void)
{
    return display;
}

Window x11ui_get_X11_window()
{
    return XtWindow(_ui_top_level);
}

/* Create a shell with a canvas widget in it.  */
int ui_open_canvas_window(video_canvas_t *c, const char *title,
			  int width, int height, int no_autorepeat)
{
    /* Note: this is correct because we never destroy CanvasWindows.  */
    Widget shell, speed_label, statustext_label;
    Widget drive_track_label[NUM_DRIVES], drive_led[NUM_DRIVES];
    Widget drive_current_image[NUM_DRIVES];
    Widget drive_led1[NUM_DRIVES], drive_led2[NUM_DRIVES];
    XSetWindowAttributes attr;
    int i;

    if (!vsid_mode) {
        if (uicolor_alloc_colors(c) < 0)
	    return -1;
    }

    /* colormap might have changed after ui_alloc_colors, so we set it again */
    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);

    if (++num_app_shells > MAX_APP_SHELLS) {
        log_error(ui_log, "Maximum number of toplevel windows reached.");
        return -1;
    }

    shell = XtVaCreatePopupShell(title, applicationShellWidgetClass,
                                 _ui_top_level, XtNinput, True, XtNtitle,
                                 title, XtNiconName, title, NULL);

    /* Xt only allows you to change the visual of a shell widget, so the
       visual and colormap must be created before the shell widget is
       created. When creating another shell widget, the new widget inherits
       the colormap and depth from the parent widget, but it inherits the
       visual from the parent window (the root window). Thus on every shell
       you create you must specify visual, colormap, and depth. Note that
       popup dialogs and menus are also shells. */
    XtVaSetValues(shell,
                  XtNvisual, visual,
                  XtNdepth, depth,
                  XtNcolormap, colormap,
                  NULL);

    pane = XtVaCreateManagedWidget
        ("Form", formWidgetClass, shell,
         XtNdefaultDistance, 2,
         NULL);

    canvas = XtVaCreateManagedWidget
        ("Canvas",
         xfwfcanvasWidgetClass, pane,
         XtNwidth, width,
         XtNheight, height,
         XtNresizable, True,
         XtNbottom, XawChainBottom,
         XtNtop, XawChainTop,
         XtNleft, XawChainLeft,
         XtNright, XawChainRight,
         XtNborderWidth, 0,
         XtNbackground,BlackPixel(display,screen),
         NULL);

    XtAddEventHandler(shell, EnterWindowMask, False,
                      (XtEventHandler)enter_window_callback,
                      NULL);
    /* XVideo must be refreshed when the shell window is moved. */
    if (!vsid_mode) {
        XtAddEventHandler(shell, StructureNotifyMask, False,
                          (XtEventHandler)exposure_callback_shell,
                          (XtPointer)c);

        XtAddEventHandler(canvas, ExposureMask | StructureNotifyMask, False,
                          (XtEventHandler)exposure_callback_canvas,
                          (XtPointer)c);
    }
    XtAddEventHandler(canvas, PointerMotionMask | ButtonPressMask |
                      ButtonReleaseMask, False,
                      (XtEventHandler)mouse_handler1351, NULL);


    /* Create the status bar on the bottom.  */
    {
        Dimension height;
        Dimension led_width = 14, led_height = 5;
        
	speed_label = XtVaCreateManagedWidget
            ("speedStatus",
             labelWidgetClass, pane,
             XtNlabel, "",
             XtNwidth, width / 3,
             XtNfromVert, canvas,
             XtNtop, XawChainBottom,
             XtNbottom, XawChainBottom,
             XtNleft, XawChainLeft,
             XtNright, XawChainRight,
             XtNjustify, XtJustifyLeft,
             XtNborderWidth, 0,
             NULL);

        XtVaGetValues(speed_label, XtNheight, &height, NULL);

        for (i = 0; i < NUM_DRIVES; i++) {
            char *name;

            name = lib_msprintf("driveCurrentImage%d", i + 1);
            drive_current_image[i] = XtVaCreateManagedWidget
                (name,
                 labelWidgetClass, pane,
                 XtNlabel, "",
                 XtNwidth, (width / 3)  - led_width - 2,
                 XtNfromVert, i == 0 ? canvas : drive_current_image[i-1],
                 XtNfromHoriz, speed_label,
                 XtNhorizDistance, 0,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyLeft,
                 XtNborderWidth, 0,
                 NULL);
            lib_free(name);

            name = lib_msprintf("driveTrack%d", i + 1);
            drive_track_label[i] = XtVaCreateManagedWidget
                (name,
                 labelWidgetClass, pane,
                 XtNlabel, "",
                 XtNwidth, (width / 3) - led_width - 2,
                 XtNfromVert, canvas,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_current_image[i],
                 XtNhorizDistance, 0,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 0,
                 NULL);
            lib_free(name);

            name = lib_msprintf("driveLed%d", i + 1);
            drive_led[i] = XtVaCreateManagedWidget
                (name,
                 xfwfcanvasWidgetClass, pane,
                 XtNwidth, led_width,
                 XtNheight, led_height,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_track_label[i],
                 XtNhorizDistance, 8,
                 XtNvertDistance, (height - led_height) / 2 + 1,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 1,
                 NULL);
            lib_free(name);

	    /* double LEDs */

            name = lib_msprintf("driveLedA%d", i + 1);
            drive_led1[i] = XtVaCreateManagedWidget
                (name,
                 xfwfcanvasWidgetClass, pane,
                 XtNwidth, led_width / 2 - 1,
                 XtNheight, led_height,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_track_label[i],
                 XtNhorizDistance, 8,
                 XtNvertDistance, (height - led_height) / 2 + 1,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 1,
                 NULL);
            lib_free(name);

            name = lib_msprintf("driveLedB%d", i + 1);
            drive_led2[i] = XtVaCreateManagedWidget
                (name,
                 xfwfcanvasWidgetClass, pane,
                 XtNwidth, led_width / 2 - 1,
                 XtNheight, led_height,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_led1[i],
                 XtNhorizDistance, 8,
                 XtNvertDistance, (height - led_height) / 2 + 1,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 1,
                 NULL);
            lib_free(name);
        }
	statustext_label = XtVaCreateManagedWidget 
	    ("statustext",
	     labelWidgetClass, pane,
	     XtNwidth, width / 3 - 2,
	     XtNfromVert, speed_label,
	     XtNjustify, XtJustifyLeft,
	     XtNlabel, "",
	     XtNborderWidth, 0,
	     NULL);
	rec_button = XtVaCreateManagedWidget 
	    ("recButton",
	     commandWidgetClass, pane,
	     XtNwidth, width / 3 - 2,
	     XtNfromVert, statustext_label,
	     XtNjustify, XtJustifyLeft,
	     XtNlabel, _("recording..."),
	     NULL);
	XtAddCallback(rec_button, XtNcallback, rec_button_callback, NULL);
	event_recording_button = XtVaCreateManagedWidget 
	    ("eventRecButton",
	     commandWidgetClass, pane,
	     XtNwidth, width / 3 - 2,
	     XtNfromVert, rec_button,
	     XtNjustify, XtJustifyLeft,
	     XtNlabel, _("event recording..."),
	     NULL);
	XtAddCallback(event_recording_button, XtNcallback, 
		      event_recording_button_callback, NULL);
    }

    /* Assign proper translations to open the menus, if already
       defined.  */
    if (left_menu_translations != NULL)
        XtOverrideTranslations(canvas, left_menu_translations);
    if (right_menu_translations != NULL)
        XtOverrideTranslations(canvas, right_menu_translations);

    /* Attach the icon pixmap, if already defined.  */
    if (icon_pixmap)
        XtVaSetValues(shell, XtNiconPixmap, icon_pixmap, NULL);

    XtAddEventHandler(canvas,
                      (EnterWindowMask | LeaveWindowMask | KeyReleaseMask
                      | KeyPressMask), True,
                      (XtEventHandler)kbd_event_handler, NULL);

    /* FIXME: ...REALLY ugly... */
    XtAddEventHandler(XtParent(canvas),
                      (EnterWindowMask | LeaveWindowMask
                      | KeyReleaseMask | KeyPressMask), True,
                      (XtEventHandler)kbd_event_handler, NULL);

    if (no_autorepeat) {
        XtAddEventHandler(canvas, EnterWindowMask, False,
                          (XtEventHandler) ui_autorepeat_off, NULL);
        XtAddEventHandler(canvas, LeaveWindowMask, False,
                          (XtEventHandler) ui_autorepeat_on, NULL);
#if 0
        XtAddEventHandler(shell, KeyPressMask, False,
                          (XtEventHandler) ui_hotkey_event_handler, NULL);
        XtAddEventHandler(canvas, KeyPressMask, False,
                          (XtEventHandler) ui_hotkey_event_handler, NULL);
        XtAddEventHandler(shell, KeyReleaseMask, False,
                          (XtEventHandler) ui_hotkey_event_handler, NULL);
        XtAddEventHandler(canvas, KeyReleaseMask, False,
                          (XtEventHandler) ui_hotkey_event_handler, NULL);
        XtAddEventHandler(shell, FocusChangeMask, False,
                          (XtEventHandler) ui_hotkey_event_handler, NULL);
        XtAddEventHandler(canvas, FocusChangeMask, False,
                          (XtEventHandler) ui_hotkey_event_handler, NULL);
#endif

    }

    XtRealizeWidget(shell);
    XtPopup(shell, XtGrabNone);

    attr.backing_store = Always;
    XChangeWindowAttributes(display, XtWindow(canvas),
                            CWBackingStore, &attr);

    app_shells[num_app_shells - 1].shell = shell;
    app_shells[num_app_shells - 1].canvas = canvas;
    app_shells[num_app_shells - 1].title = lib_stralloc(title);
    app_shells[num_app_shells - 1].speed_label = speed_label;
    app_shells[num_app_shells - 1].statustext_label = statustext_label;
    status_bar = speed_label;
    
    for (i = 0; i < NUM_DRIVES; i++) {
        app_shells[num_app_shells - 1].drive_widgets[i].track_label
            = drive_track_label[i];
        app_shells[num_app_shells - 1].drive_widgets[i].driveled
            = drive_led[i];
        XtUnrealizeWidget(drive_led[i]);
        app_shells[num_app_shells - 1].drive_widgets[i].driveled1
            = drive_led1[i];
        app_shells[num_app_shells - 1].drive_widgets[i].driveled2
            = drive_led2[i];
        XtUnrealizeWidget(drive_led1[i]);
        XtUnrealizeWidget(drive_led2[i]);
        app_shells[num_app_shells - 1].drive_widgets[i].current_image
            = drive_current_image[i];
        strcpy(&(last_attached_images[i][0]), "");
        /* the `current_image' widgets are never `UnRealized'. */
        XtRealizeWidget(app_shells[num_app_shells - 1].
                        drive_widgets[i].current_image);
        XtManageChild(app_shells[num_app_shells - 1].
                      drive_widgets[i].current_image);

    }
    XtUnrealizeWidget(rec_button);
    XtUnrealizeWidget(event_recording_button);
    
    XSetWMProtocols(display, XtWindow(shell), &wm_delete_window, 1);
    XtOverrideTranslations(shell,
                           XtParseTranslationTable
                           ("<Message>WM_PROTOCOLS: Close()"));

    /* This is necessary because the status might have been set before we
       actually open the canvas window.  */
    ui_enable_drive_status(enabled_drives, drive_active_led);

    initBlankCursor();

    c->emuwindow = canvas;
    ui_cached_video_canvas = c;
    
    return 0;
}

/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(ui_menu_entry_t *menu)
{
    Widget w = ui_menu_create("LeftMenu", menu, NULL);
    char *translation_table;
    char *name = XtName(w);
    int i;

    translation_table =
        util_concat("<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "@Num_Lock<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
               "@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               NULL);

    left_menu_translations = XtParseTranslationTable(translation_table);
    lib_free(translation_table);

    translation_table =
        util_concat("<Btn1Down>: \n",
               "@Num_Lock<Btn1Down>: \n",
               "Lock <Btn1Down>: \n"
               "@Scroll_Lock <Btn1Down>: \n",
               NULL);

    left_menu_disabled_translations = XtParseTranslationTable(translation_table);
    lib_free(translation_table);

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].canvas, left_menu_translations);

    if (left_menu != NULL)
        XtDestroyWidget(left_menu);
    left_menu = w;
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(ui_menu_entry_t *menu)
{
    Widget w = ui_menu_create("RightMenu", menu, NULL);
    char *translation_table;
    char *name = XtName(w);
    int i;

    translation_table =
        util_concat("<Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "@Num_Lock<Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "Lock <Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
               "@Scroll_Lock <Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               NULL);
    right_menu_translations = XtParseTranslationTable(translation_table);
    lib_free(translation_table);

    translation_table =
        util_concat("<Btn3Down>: \n",
               "@Num_Lock<Btn3Down>: \n",
               "Lock <Btn3Down>: \n"
               "@Scroll_Lock <Btn3Down>: \n",
               NULL);

    right_menu_disabled_translations = XtParseTranslationTable(translation_table);
    lib_free(translation_table);

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].canvas, right_menu_translations);

    if (right_menu != NULL)
        XtDestroyWidget(right_menu);
    right_menu = w;
}

void ui_destroy_drive8_menu(void)
{
    if (drive8_menu != NULL) {
        XtDestroyWidget(drive8_menu);
        drive8_menu = NULL;
    }
}

void ui_destroy_drive9_menu(void)
{
    if (drive9_menu != NULL) {
        XtDestroyWidget(drive9_menu);
        drive9_menu = NULL;
    }
}

void ui_set_drive8_menu(Widget w)
{
    int i;

    for (i = 0; i < num_app_shells; i++)
        if (app_shells[i].drive_mapping[0] < 0) {
            XtDestroyWidget(w);
            return;
	}

    if (!drive8_menu_translations) {
	char *translation_table;
	char *name = XtName(w);

	translation_table =
	    util_concat("<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
		"@Num_Lock<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
		"Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
		"@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
		NULL);
	drive8_menu_translations = XtParseTranslationTable(translation_table);
	lib_free(translation_table);
    }

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].drive_widgets[app_shells[i].drive_mapping[0]].current_image, 
            drive8_menu_translations);

    /*ui_destroy_drive8_menu();*/

    drive8_menu = w;
}

void ui_set_drive9_menu(Widget w)
{
    int i;

    for (i = 0; i < num_app_shells; i++)
        if (app_shells[i].drive_mapping[1] < 0) {
            XtDestroyWidget(w);
            return;
	}

    if (!drive9_menu_translations) {
	char *translation_table;
	char *name = XtName(w);

	translation_table =
	    util_concat("<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
		"@Num_Lock<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
		"Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
		"@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
		NULL);
	drive9_menu_translations = XtParseTranslationTable(translation_table);
	lib_free(translation_table);
    }

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].drive_widgets[app_shells[i].drive_mapping[1]].current_image, 
            drive9_menu_translations);

    /*ui_destroy_drive9_menu();*/

    drive9_menu = w;
}

void ui_set_topmenu(ui_menu_entry_t *menu)
{
}

void ui_set_speedmenu(ui_menu_entry_t *menu)
{
}

void ui_set_tape_menu(ui_menu_entry_t *menu)
{
}

void ui_set_application_icon(const char *icon_data[])
{
#ifdef HAVE_LIBXPM
    int i;
    Pixmap icon_pixmap;

    /* Create the icon pixmap. */
    XpmCreatePixmapFromData(display, DefaultRootWindow(display),
                            (char **) icon_data, &icon_pixmap, NULL, NULL);

    for (i = 0; i < num_app_shells; i++)
        XtVaSetValues(app_shells[i].shell, XtNiconPixmap, icon_pixmap, NULL);
#endif
}

/* ------------------------------------------------------------------------- */

void ui_exit(void)
{
    ui_button_t b;
    int value;
    char *s = util_concat("Exit ", machine_name, _(" emulator"), NULL);

    resources_get_int("ConfirmOnExit", &value);
    if( value )
      b = ui_ask_confirmation(s, _("Do you really want to exit?"));
    else
      b = UI_BUTTON_YES;

    if (b == UI_BUTTON_YES) {
        resources_get_int("SaveResourcesOnExit", &value);
        if (value) {
            b = ui_ask_confirmation(s, _("Save the current settings?"));
            if (b == UI_BUTTON_YES) {
                if (resources_save(NULL) < 0)
                    ui_error(_("Cannot save settings."));
            } else if (b == UI_BUTTON_CANCEL) {
                lib_free(s);
                return;
            }
        }
        ui_autorepeat_on();
        ui_restore_mouse();

#ifdef HAVE_FULLSCREEN
        fullscreen_suspend(0);
#endif
        lib_free(s);
        exit(0);
    }

    lib_free(s);
    vsync_suspend_speed_eval();
}

/* ------------------------------------------------------------------------- */

/* Set the colormap variable.  The user must tell us whether he wants the
   default one or not using the `privateColormap' resource.  */
static int alloc_colormap(void)
{
    int use_private_colormap;

    if (colormap)
        return 0;

    resources_get_int("PrivateColormap", &use_private_colormap);

    if (!use_private_colormap
        && depth == DefaultDepth(display, screen)
        && !have_truecolor) {
        colormap = DefaultColormap(display, screen);
    } else {
        log_message(ui_log, "Using private colormap.");
        colormap = XCreateColormap(display, RootWindow(display, screen),
                                   visual, AllocNone);
    }

    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
    return 0;
}

static void statusbar_setstatustext(const char *t)
{
    int i;
    for (i = 0; i < num_app_shells; i++)
	XtVaSetValues(app_shells[i].statustext_label, XtNlabel, t, NULL);
}

/* ------------------------------------------------------------------------- */

/* Show the speed index to the user.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);

    for (i = 0; i < num_app_shells; i++) {
        if (!percent) {
            XtVaSetValues(app_shells[i].speed_label, XtNlabel,
                          warp_flag ? _("(warp)") : "",
                          NULL);
        } else {
            char *str;
            str = lib_msprintf("%d%%, %dfps %s", percent_int, framerate_int,
                               warp_flag ? _("(warp)") : "");
            XtVaSetValues(app_shells[i].speed_label, XtNlabel, str, NULL);
            lib_free(str);
        }
    }
    if (statustext_display_time > 0) {
        statustext_display_time--;
        if (statustext_display_time == 0)
            statusbar_setstatustext("");
    }

    if (!screenshot_is_recording())
	XtUnrealizeWidget(rec_button);
}
void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
    int i, j, num, k, true_emu;
    int drive_mapping[NUM_DRIVES];

    num = 0;

    enabled_drives = enable;
    drive_active_led = drive_led_color;

    /* -1 should be safe, otherwise the display code in `ui_display_*'
       was wrong before. */
    memset(drive_mapping, -1, sizeof(drive_mapping));

    resources_get_int("DriveTrueEmulation", &true_emu);

    if (true_emu) {
        /* num == number of drives which are active;
           drive_mapping[i] stores the widget number into which the i'th drive
           things should be displayed */
        for (i = 0, j = 1; i < NUM_DRIVES; i++, j <<= 1) {
            if (enabled_drives & j)
                drive_mapping[i] = num++;
        }
    } else {
        for (i = 0; i < NUM_DRIVES; i++) {
            if (strcmp(&(last_attached_images[i][0]), "") != 0)
                drive_mapping[i] = num++;
        }
    }


    for (i = 0; i < num_app_shells; i++) {
        /* now show `num' widgets ... */
        for (j = 0; j < NUM_DRIVES && num && true_emu > 0; j++, num--) {
            XtRealizeWidget(app_shells[i].drive_widgets[j].track_label);
            XtManageChild(app_shells[i].drive_widgets[j].track_label);

            for (k = 0; k < NUM_DRIVES; k++)
                if (drive_mapping[k] == j)
                    break;
            app_shells[i].drive_nleds[j] = drive_num_leds(k);
            if (app_shells[i].drive_nleds[j] == 1) {
                XtRealizeWidget(app_shells[i].drive_widgets[j].driveled);
                XtManageChild(app_shells[i].drive_widgets[j].driveled);
                XtUnrealizeWidget(app_shells[i].drive_widgets[j].driveled1);
                XtUnrealizeWidget(app_shells[i].drive_widgets[j].driveled2);
            } else {
                XtUnrealizeWidget(app_shells[i].drive_widgets[j].driveled);
                XtRealizeWidget(app_shells[i].drive_widgets[j].driveled1);
                XtManageChild(app_shells[i].drive_widgets[j].driveled1);
                XtRealizeWidget(app_shells[i].drive_widgets[j].driveled2);
                XtManageChild(app_shells[i].drive_widgets[j].driveled2);
            }
        }
        /* ...and hide the rest until `NUM_DRIVES' */
        if (! true_emu)
            num = j = 0;        /* hide all label+led widgets in normal mode */

        for (; j < NUM_DRIVES; j++) {
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].track_label);
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].driveled);
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].driveled1);
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].driveled2);
        }
        for (j = 0; j < NUM_DRIVES; j++)
            app_shells[i].drive_mapping[j] = drive_mapping[j];
    }
    /* now update all image names from the cached names */
    ui_display_drive_current_image2();
}

void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base,
                            unsigned int half_track_number)
{
    int i;
    /* FIXME: Fixed length.  */
    char str[256];
    double track_number = (double)half_track_number / 2.0;

    sprintf(str, _("%d: Track %.1f"), drive_number + drive_base,
            (double)track_number);
    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive_number];
        Widget w;
        if (n < 0)
            return;             /* bad mapping */
        w = app_shells[i].drive_widgets[n].track_label;

        XtVaSetValues(w, XtNlabel, str, NULL);
    }
}

void ui_display_drive_led(int drive_number, unsigned int led_pwm1,
                          unsigned int led_pwm2)
{
    Pixel pixel;
    int status = 0;
    int i;

    if (led_pwm1 > 100)
        status |= 1;
    if (led_pwm2 > 100)
        status |= 2;

    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive_number];
        Widget w;

        if (n < 0)
            return;             /* bad mapping */

        pixel = status ? (drive_active_led[drive_number]
                  ? drive_led_on_green_pixel : drive_led_on_red_pixel)
                  : drive_led_off_pixel;
        w = app_shells[i].drive_widgets[n].driveled;
        XtVaSetValues(w, XtNbackground, pixel, NULL);

        pixel = (status & 1) ? (drive_active_led[drive_number]
                  ? drive_led_on_green_pixel : drive_led_on_red_pixel)
                  : drive_led_off_pixel;
        w = app_shells[i].drive_widgets[n].driveled1;
        XtVaSetValues(w, XtNbackground, pixel, NULL);

        pixel = (status & 2) ? (drive_active_led[drive_number]
                  ? drive_led_on_green_pixel : drive_led_on_red_pixel)
                  : drive_led_off_pixel;
        w = app_shells[i].drive_widgets[n].driveled2;
        XtVaSetValues(w, XtNbackground, pixel, NULL);
    }
}

void ui_display_drive_current_image(unsigned int drive_number,
                                    const char *image)
{
    if (console_mode) {
        return;
    }

    /* FIXME: Allow more than two drives.  */
    if (drive_number >= NUM_DRIVES)
        return;

    strcpy(&(last_attached_images[drive_number][0]), image);

    /* update drive mapping */
    ui_enable_drive_status(enabled_drives, drive_active_led);
    uifliplist_update_menus(drive_number + 8, drive_number + 8);
}

static void ui_display_drive_current_image2 (void)
{
    int i, j;
    char *name;

    /* Now update all fields according to drive_mapping */
    for (i = 0; i < num_app_shells; i++) {
        for (j = 0; j < NUM_DRIVES; j++) {
            int n = app_shells[i].drive_mapping[j];
            Widget w;

            /* It is assumed that the j-1'th widget is not touched anymore.
               -> the drive mapping code fills the widgets up from 0 */

            /* first clear the j'th widget */
            w = app_shells[i].drive_widgets[j].current_image;
            XtVaSetValues(w, XtNlabel, "", NULL);

            if (n < 0)
                continue;       /* j'th is drive not mapped */

            /* now fill the j'th widget */
            w = app_shells[i].drive_widgets[n].current_image;

            util_fname_split(&(last_attached_images[j][0]), NULL, &name);
            XtVaSetValues(w, XtNlabel, name, NULL);
            lib_free(name);
        }
    }
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

void ui_display_tape_current_image(const char *image)
{
}

void ui_display_recording(int recording_status)
{
    if (recording_status)
    {
	XtRealizeWidget(event_recording_button);
	XtManageChild(event_recording_button);
    }
    else
	XtUnrealizeWidget(event_recording_button);
}

void ui_display_playback(int playback_status, char *version)
{
}

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;

    for (i = 0; i < num_app_shells; i++) {
        if (flag) {
            char *str;

            str = lib_msprintf(_("%s (paused)"), app_shells[i].title);
            XtVaSetValues(app_shells[i].shell, XtNtitle, str, NULL);
            lib_free(str);
        } else {
            XtVaSetValues(app_shells[i].shell, XtNtitle,
                          app_shells[i].title, NULL);
        }
    }
}

/* Dispatch the next Xt event.  If not pending, wait for it. */
void ui_dispatch_next_event(void)
{
    XEvent report;

    XtAppNextEvent(app_context, &report);
    XtDispatchEvent(&report);
}

/* Dispatch all the pending Xt events. */
void ui_dispatch_events(void)
{
    if (console_mode) {
        return;
    }

    while (XtAppPending(app_context))
        ui_dispatch_next_event();
}

void x11ui_fullscreen(int i)
{
    static Atom _net_wm_state = None;
    static Atom _net_wm_state_fullscreen = None;
    XEvent xev;
    int mode;
    
    if (strcmp(machine_name, "C128") == 0)
    {
	/* mode == 1 -> VICII, mode == 0 VDC */
	resources_get_int("40/80ColumnKey", &mode); 
    }
    else
	mode = 0;
    
    if (_net_wm_state == None)
    {
	_net_wm_state = XInternAtom(display, "_NET_WM_STATE", False);
	_net_wm_state_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);
    }
    memset(&xev, 0, sizeof(xev));
    xev.xclient.type = ClientMessage;
    xev.xclient.window = XtWindow(app_shells[mode].shell); /* hardwired use of resource `40/80ColumnKey' */
    xev.xclient.message_type = _net_wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = i;
    xev.xclient.data.l[1] = _net_wm_state_fullscreen;
  
    XSendEvent (display, DefaultRootWindow(display), False,
		SubstructureRedirectMask, &xev);
}

int
ui_fullscreen_statusbar(struct video_canvas_s *canvas, int enable)
{
    log_message(ui_log, 
		"Toggling of Statusbar/Menu in Xaw is not supported.");
    return 0;
}

/* Resize one window. */
void x11ui_resize_canvas_window(ui_window_t w, int width, int height, int hwscale)
{
    Dimension canvas_width, canvas_height;
    Dimension form_width, form_height;

    /* Ok, form widgets are stupid animals; in a perfect world, I should be
       allowed to resize the canvas and let the Form do the rest.  Unluckily,
       this does not happen, so let's do things the dirty way then.  This
       sucks badly.  */

    XtVaGetValues((Widget)w,
                  XtNwidth, &canvas_width,
                  XtNheight, &canvas_height,
                  NULL);
    XtVaGetValues(XtParent(XtParent((Widget)w)),
                  XtNwidth, &form_width,
                  XtNheight, &form_height,
                  NULL);

    XtVaSetValues(XtParent(XtParent((Widget)w)),
                  XtNwidth, form_width + width - canvas_width,
                  XtNheight, form_height + height - canvas_height,
                  NULL);

    return;
}

void x11ui_move_canvas_window(ui_window_t w, int x, int y)
{
    XtVaSetValues(XtParent(XtParent((Widget)w)),
		  XtNwidthInc, 1,
		  XtNheightInc, 1,
		  XtNx, x,
		  XtNy, y,
		  NULL);
}

void x11ui_canvas_position(ui_window_t w, int *x, int *y)
{
    Dimension pos_x, pos_y;
    Dimension tl_x, tl_y;
    Dimension shell_x, shell_y;
    
    XtVaGetValues(XtParent(XtParent((Widget)w)),
		  XtNx, &tl_x,
		  XtNy, &tl_y,
		  NULL);

    XtVaGetValues(XtParent((Widget)w),
		  XtNx, &shell_x,
		  XtNy, &shell_y,
		  NULL);
    
    XtVaGetValues((Widget)w,
		  XtNx, &pos_x,
		  XtNy, &pos_y,
		  NULL);
    *x = (int) (pos_x + tl_x + shell_x);
    *y = (int) (pos_y + tl_y + shell_y);
    XRaiseWindow(display, XtWindow(_ui_top_level));
}

void x11ui_get_widget_size(ui_window_t win, int *w, int *h)
{
    Dimension x, y;
    XtVaGetValues((Widget)win,
                  XtNwidth, &x,
                  XtNheight, &y,
                  NULL);
    *w = (int) x;
    *h = (int) y;
}


void x11ui_destroy_widget(ui_window_t w)
{
    XtDestroyWidget(w);
}


/* Enable autorepeat. */
void ui_autorepeat_on(void)
{
    XAutoRepeatOn(display);
    XFlush(display);
}

/* Disable autorepeat. */
void ui_autorepeat_off(void)
{
    XAutoRepeatOff(display);
    XFlush(display);
}

/* ------------------------------------------------------------------------- */

/* Button callbacks.  */

#define DEFINE_BUTTON_CALLBACK(button)          \
    static UI_CALLBACK(button##_callback)       \
    {                                           \
        *((ui_button_t *)client_data) = button; \
    }

DEFINE_BUTTON_CALLBACK(UI_BUTTON_OK)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_CANCEL)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_YES)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_NO)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_CLOSE)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_MON)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_NONE)
/*DEFINE_BUTTON_CALLBACK(UI_BUTTON_DEBUG)*/
DEFINE_BUTTON_CALLBACK(UI_BUTTON_RESET)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_HARDRESET)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_CONTENTS)
DEFINE_BUTTON_CALLBACK(UI_BUTTON_AUTOSTART)

/* ------------------------------------------------------------------------- */

/* Report an error to the user.  */
void ui_error(const char *format,...)
{
    char *str;
    va_list ap;
    static Widget error_dialog;
    static ui_button_t button;

    va_start(ap, format);
    str = lib_mvsprintf(format, ap);
    error_dialog = build_error_dialog(_ui_top_level, &button, str);
    ui_popup(XtParent(error_dialog), _("VICE Error!"), False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(error_dialog));
    XtDestroyWidget(XtParent(error_dialog));
    ui_dispatch_events();
    vsync_suspend_speed_eval();
    lib_free(str);
}

/* Report a message to the user.  */
void ui_message(const char *format,...)
{
    char *str;
    va_list ap;
    static Widget error_dialog;
    static ui_button_t button;

    va_start(ap, format);
    str = lib_mvsprintf(format, ap);
    error_dialog = build_error_dialog(_ui_top_level, &button, str);
    ui_popup(XtParent(error_dialog), "VICE", False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(error_dialog));
    ui_check_mouse_cursor();
    XtDestroyWidget(XtParent(error_dialog));
    ui_dispatch_events();
    vsync_suspend_speed_eval();
    lib_free(str);
}

/* Report a message to the user, allow different buttons. */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    char *str;
    va_list ap;
    static Widget jam_dialog, shell, tmp, mform, bbox;
    static ui_button_t button;

    va_start(ap, format);

    if (console_mode) {
        vfprintf(stderr, format, ap);
        exit(0);
    }

    shell = ui_create_transient_shell(_ui_top_level, "jamDialogShell");
    jam_dialog = XtVaCreateManagedWidget
        ("jamDialog", panedWidgetClass, shell, NULL);
    mform = XtVaCreateManagedWidget
        ("messageForm", formWidgetClass, jam_dialog, NULL);

    str = lib_mvsprintf(format, ap);
    tmp = XtVaCreateManagedWidget
        ("label", labelWidgetClass, mform,
         XtNresize, False, XtNjustify, XtJustifyCenter, XtNlabel, str,
         NULL);

    bbox = XtVaCreateManagedWidget
        ("buttonBox", boxWidgetClass, jam_dialog,
         XtNshowGrip, False, XtNskipAdjust, True,
         XtNorientation, XtorientHorizontal, NULL);

    tmp = XtVaCreateManagedWidget
        ("resetButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_RESET_callback,
                  (XtPointer) &button);

    tmp = XtVaCreateManagedWidget
        ("hardResetButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_HARDRESET_callback,
                  (XtPointer) &button);

    tmp = XtVaCreateManagedWidget
        ("monButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_MON_callback,
                  (XtPointer) &button);

    tmp = XtVaCreateManagedWidget
        ("noneButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_NONE_callback,
                  (XtPointer) &button);

    ui_popup(XtParent(jam_dialog), "VICE", False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(jam_dialog));
    XtDestroyWidget(XtParent(jam_dialog));

    vsync_suspend_speed_eval();
    ui_dispatch_events();
    lib_free(str);

    switch (button) {
      case UI_BUTTON_MON:
        ui_restore_mouse();
#ifdef HAVE_FULLSCREEN
        fullscreen_suspend(0);
#endif
        return UI_JAM_MONITOR;
      case UI_BUTTON_HARDRESET:
        return UI_JAM_HARD_RESET;
      case UI_BUTTON_NONE:
        return UI_JAM_NONE;
      case UI_BUTTON_RESET:
      default:
        return UI_JAM_RESET;
    }
}

int ui_extend_image_dialog(void)
{
    ui_button_t b;

    vsync_suspend_speed_eval();
    b = ui_ask_confirmation(_("Extend disk image"),
                            ("Do you want to extend the disk image"
                             " to 40 tracks?"));
    return (b == UI_BUTTON_YES) ? 1 : 0;
}

/* this must be in sync with uilib_file_filter_enum_t */
static const char* file_filters[] = {
/* all */ "*",
/* palette */ "*.vpl",
/* snapshot */ "*.vsf",
/* disk */ "*.[gdxGDX]*",
/* tape */ "*.[tT]",
/* cartridge */ "*.[cCbB][rRiI][tTnN]",
/* crt filter */ "*.[cC][rR][tT]",
/* flip_list */ "*.vfl",
/* romset */ "*.vrs",
/* romset archive */ "*.vra",
/* keymap */ "*.vkm",
/* emulator_filter unused in X11 */ "",
/* wav */ "wav",
/* voc */ "voc",
/* iff */ "iff",
/* aiff */ "aiff",
/* mp3 */ "mp3",
/* serial */ "ttyS*",
/* vic20cart */ "*.prg",
/* sid */ "*.[psPS]*",
/* dtvrom */ ".[bB][iI][nN]"
};



/* File browser. */
char *ui_select_file(const char *title,
                     read_contents_func_type read_contents_func,
                     unsigned int allow_autostart, const char *default_dir,
                     enum uilib_file_filter_enum_s* patterns,
                     int num_patterns,
                     ui_button_t *button_return,
                     unsigned int show_preview, int *attach_wp,
		     ui_filechooser_t action)
{
    static ui_button_t button;
    char *ret;
    Widget file_selector = NULL;
    XfwfFileSelectorStatusStruct fs_status;
    char *current_dir;

    /* we preserve the current directory over the invocations */
    current_dir = ioutil_current_dir();    /* might be changed elsewhere */
    if (filesel_dir != NULL) {
        ioutil_chdir(filesel_dir);
    }

    /* We always rebuild the file selector from scratch (which is slow),
       because there seems to be a bug in the XfwfScrolledList that causes
       the file and directory listing to disappear randomly.  I hope this
       fixes the problem...  */
    file_selector = build_file_selector(_ui_top_level, &button);

    XtVaSetValues(file_selector, XtNshowAutostartButton, allow_autostart, NULL);
    XtVaSetValues(file_selector, XtNshowContentsButton,
                  read_contents_func ? 1 : 0,  NULL);

    XtVaSetValues(file_selector, XtNpattern,
                  file_filters[patterns[0]], NULL);

    if (default_dir != NULL) {
        XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) file_selector,
                                        default_dir);
    } else {
        char *newdir = ioutil_current_dir();

        XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) file_selector,
                                        newdir);
        lib_free(newdir);
    }

    ui_popup(XtParent(file_selector), title, False);
    do {
        button = UI_BUTTON_NONE;
        while (button == UI_BUTTON_NONE)
            ui_dispatch_next_event();
        XfwfFileSelectorGetStatus((XfwfFileSelectorWidget)file_selector,
                                  &fs_status);
        if (fs_status.file_selected
            && button == UI_BUTTON_CONTENTS
            && read_contents_func != NULL) {
            image_contents_t *contents;
            char *f = util_concat(fs_status.path, fs_status.file, NULL);

            contents = read_contents_func(f);
            lib_free(f);
            if (contents != NULL) {
                char *buf, *tmp;
                image_contents_file_list_t *p;
                int buf_size;
                size_t max_buf_size;

                max_buf_size = 4096;
                buf = (BYTE *)lib_malloc(max_buf_size);
                buf_size = 0;

                tmp = image_contents_to_string(contents, 1);
#define BUFCAT(s) \
    util_bufcat(buf, &buf_size, &max_buf_size, ((BYTE *)s), strlen(s))
                BUFCAT(tmp);
                lib_free(tmp);

                if (contents->file_list == NULL)
                    BUFCAT("\n(Empty image.)");
                for (p = contents->file_list; p != NULL; p = p->next) {
                    BUFCAT("\n");
                    tmp = image_contents_file_to_string(p, 1);
                    BUFCAT(tmp);
                    lib_free(tmp);
                }
                if (contents->blocks_free >= 0) {
                    tmp = lib_msprintf("\n%d BLOCKS FREE.", contents->blocks_free);
                    BUFCAT(tmp);
                    lib_free(tmp);
                }

                ui_show_text(fs_status.file, buf, 250, 240);
                image_contents_destroy(contents);
                lib_free(buf);
            } else {
                ui_error(_("Unknown image type"));
            }
        }
    } while ((!fs_status.file_selected && button != UI_BUTTON_CANCEL)
             || button == UI_BUTTON_CONTENTS);

    /* `ret' gets always malloc'ed.  */
    if (fs_status.file_selected)
        ret = util_concat(fs_status.path, fs_status.file, NULL);
    else
        ret = lib_stralloc("");

    ui_popdown(XtParent(file_selector));
#ifndef __alpha
    /* On Alpha, XtDestroyWidget segfaults, don't know why...  */
    XtDestroyWidget(XtParent(file_selector));
#endif
    if (filesel_dir != NULL) {
        lib_free(filesel_dir);
    }
    filesel_dir = ioutil_current_dir();
    if (current_dir != NULL) {
        ioutil_chdir(current_dir);
        lib_free(current_dir);
    }

    *button_return = button;
    if (button == UI_BUTTON_OK || button == UI_BUTTON_AUTOSTART) {
        /* Caller has to free the filename.  */
        return ret;
    } else {
        lib_free(ret);
        return NULL;
    }
}

/* Ask for a string.  The user can confirm or cancel. */
ui_button_t ui_input_string(const char *title, const char *prompt, char *buf,
                            unsigned int buflen)
{
    String str;
    static Widget input_dialog, input_dialog_label, input_dialog_field;
    static ui_button_t button;

    if (!input_dialog)
        input_dialog = build_input_dialog(_ui_top_level, &button,
                                          &input_dialog_label,
                                          &input_dialog_field);
    XtVaSetValues(input_dialog_label, XtNlabel, prompt, NULL);
    XtVaSetValues(input_dialog_field, XtNstring, buf, NULL);
    XtSetKeyboardFocus(input_dialog, input_dialog_field);
    ui_popup(XtParent(input_dialog), title, False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    XtVaGetValues(input_dialog_field, XtNstring, &str, NULL);
    strncpy(buf, str, buflen);
    ui_popdown(XtParent(input_dialog));
    return button;
}

/* Display a text to the user. */
void ui_show_text(const char *title, const char *text, int width, int height)
{
    static ui_button_t button;
    Widget show_text;

    show_text = build_show_text(_ui_top_level, &button, (String)text,
                                width, height);
    ui_popup(XtParent(show_text), title, False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(show_text));
    XtDestroyWidget(XtParent(show_text));
}

/* Ask for a confirmation. */
ui_button_t ui_ask_confirmation(const char *title, const char *text)
{
    static Widget confirm_dialog, confirm_dialog_message;
    static ui_button_t button;

    if (!confirm_dialog)
        confirm_dialog = build_confirm_dialog(_ui_top_level, &button,
                                              &confirm_dialog_message);
    XtVaSetValues(confirm_dialog_message, XtNlabel, text, NULL);
    ui_popup(XtParent(confirm_dialog), title, False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(confirm_dialog));
    return button;
}

/* Update the menu items with a checkmark according to the current resource
   values.  */
void ui_update_menus(void)
{
    ui_menu_update_all();
}

Widget ui_create_shell(Widget parent, const char *name,
                              WidgetClass class)
{
    Widget w;

    w = XtVaCreatePopupShell
        (name, class, parent, XtNinput, True, NULL);

    XtVaSetValues(w,
                  XtNvisual, visual,
                  XtNdepth, depth,
                  XtNcolormap, colormap,
                  NULL);

    return w;
}

Widget ui_create_transient_shell(Widget parent, const char *name)
{
    return ui_create_shell(parent, name, transientShellWidgetClass);
}

/* Pop up a popup shell and center it to the last visited AppShell */
void ui_popup(Widget w, const char *title, Boolean wait_popdown)
{
    Widget s = NULL;

#ifdef HAVE_FULLSCREEN
    fullscreen_suspend(1);
#endif

    ui_restore_mouse();
    /* Keep sure that we really know which was the last visited shell. */
    ui_dispatch_events();

    if (last_visited_app_shell)
        s = last_visited_app_shell;
    else {
        /* Choose one realized shell. */
        int i;
        for (i = 0; i < num_app_shells; i++)
            if (XtIsRealized(app_shells[i].shell)) {
                s = app_shells[i].shell;
                break;
            }
    }

    {
        /* Center the popup. */
        Dimension my_width, my_height, shell_width, shell_height;
        Dimension my_x, my_y;
        Position tlx, tly;
        int foo;
        unsigned int root_width, root_height, ufoo;
        Window foowin;

        XtRealizeWidget(w);
        XtVaGetValues(w, XtNwidth, &my_width, XtNheight, &my_height, NULL);

        /* Now make sure the whole widget is visible.  */
        XGetGeometry(display, RootWindow(display, screen), &foowin, &foo,
                     &foo, &root_width, &root_height, &ufoo, &ufoo);

        if (s != NULL) {
            XtVaGetValues(s, XtNwidth, &shell_width, XtNheight, &shell_height,
                          XtNx, &tlx, XtNy, &tly, NULL);
            XtTranslateCoords(XtParent(s), tlx, tly, &tlx, &tly);
            my_x = tlx + (shell_width - my_width) / 2;
            my_y = tly + (shell_height - my_height) / 2;

            /* FIXME: Is it really OK to cast to `signed short'?  */
            if ((signed short)my_x < 0)
                my_x = 0;
            else if ((unsigned int)((signed short)my_x + my_width) > root_width)
                my_x = root_width - my_width;

            if ((signed short)my_y < 0)
                my_y = 0;
            else if ((unsigned int)((signed short)my_y + my_height)
                > root_height)
                my_y = root_height - my_height;
        } else {
            /* We don't have an AppWindow to refer to: center to the root
               window.  */
            my_x = (root_width - my_width) / 2;
            my_y = (root_height - my_height) / 2;
        }

        XtVaSetValues(w, XtNx, my_x, XtNy, my_y, NULL);
    }
    XtVaSetValues(w, XtNtitle, title, NULL);
    XtPopup(w, XtGrabExclusive);
    XSetWMProtocols(display, XtWindow(w), &wm_delete_window, 1);

    /* If requested, wait for this widget to be popped down before
       returning. */
    if (wait_popdown) {
        int oldcnt = popped_up_count++;
        while (oldcnt != popped_up_count)
            ui_dispatch_next_event();
    } else
        popped_up_count++;
}

/* Pop down a popup shell. */
void ui_popdown(Widget w)
{
    XtPopdown(w);
    ui_check_mouse_cursor();
    if (--popped_up_count < 0)
        popped_up_count = 0;

#ifdef HAVE_FULLSCREEN
    fullscreen_resume();
#endif
}

/* ------------------------------------------------------------------------- */

/* These functions build all the widgets. */

static Widget build_file_selector(Widget parent,
                                  ui_button_t * button_return)
{
    Widget shell = ui_create_transient_shell(parent, "fileSelectorShell");
    Widget file_selector = XtVaCreateManagedWidget("fileSelector",
                                                   xfwfFileSelectorWidgetClass,
                                                   shell,
                                                   XtNflagLinks, True, NULL);

    XtAddCallback((Widget) file_selector,
                  XtNokButtonCallback, UI_BUTTON_OK_callback,
                  (XtPointer) button_return);
    XtAddCallback((Widget) file_selector,
                  XtNcancelButtonCallback, UI_BUTTON_CANCEL_callback,
                  (XtPointer) button_return);
    XtAddCallback((Widget) file_selector,
                  XtNcontentsButtonCallback, UI_BUTTON_CONTENTS_callback,
                  (XtPointer) button_return);
    XtAddCallback((Widget) file_selector,
                  XtNautostartButtonCallback, UI_BUTTON_AUTOSTART_callback,
                  (XtPointer) button_return);
    return file_selector;
}

static Widget build_error_dialog(Widget parent, ui_button_t * button_return,
                                 const String message)
{
    Widget shell, ErrorDialog, tmp;

    shell = ui_create_transient_shell(parent, "errorDialogShell");
    ErrorDialog = XtVaCreateManagedWidget
        ("errorDialog", panedWidgetClass, shell, NULL);
    tmp = XtVaCreateManagedWidget
        ("messageForm", formWidgetClass, ErrorDialog, NULL);
    tmp = XtVaCreateManagedWidget
        ("label", labelWidgetClass, tmp,
         XtNresize, False, XtNjustify, XtJustifyCenter, XtNlabel, message,
         NULL);
    tmp = XtVaCreateManagedWidget
        ("buttonBox", boxWidgetClass, ErrorDialog,
         XtNshowGrip, False, XtNskipAdjust, True,
         XtNorientation, XtorientHorizontal, NULL);
    tmp = XtVaCreateManagedWidget
        ("closeButton", commandWidgetClass, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_CLOSE_callback,
                  (XtPointer) button_return);
    return ErrorDialog;
}

static Widget build_input_dialog(Widget parent, ui_button_t * button_return,
                                 Widget *input_dialog_label,
                                 Widget *input_dialog_field)
{
    Widget shell, input_dialog, tmp1, tmp2;

    shell = ui_create_transient_shell(parent, "inputDialogShell");
    input_dialog = XtVaCreateManagedWidget
        ("inputDialog", panedWidgetClass, shell, NULL);
    tmp1 = XtVaCreateManagedWidget
        ("inputForm", formWidgetClass, input_dialog, NULL);
    *input_dialog_label = XtVaCreateManagedWidget
        ("label", labelWidgetClass, tmp1, XtNresize, False, XtNjustify,
         XtJustifyLeft, NULL);
    *input_dialog_field = XtVaCreateManagedWidget
        ("field", textfieldWidgetClass, tmp1, XtNfromVert, *input_dialog_label,
         NULL);
    XtAddCallback(*input_dialog_field, XtNactivateCallback,
                  UI_BUTTON_OK_callback, (XtPointer) button_return);
    tmp1 = XtVaCreateManagedWidget
        ("buttonBox", boxWidgetClass, input_dialog,
         XtNshowGrip, False, XtNskipAdjust, True,
         XtNorientation, XtorientHorizontal, NULL);
    tmp2 = XtVaCreateManagedWidget
        ("okButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2, XtNcallback,
                  UI_BUTTON_OK_callback, (XtPointer) button_return);
    tmp2 = XtVaCreateManagedWidget
        ("cancelButton", commandWidgetClass, tmp1, XtNfromHoriz, tmp2, NULL);
    XtAddCallback(tmp2, XtNcallback,
                  UI_BUTTON_CANCEL_callback, (XtPointer) button_return);
    return input_dialog;
}

static Widget build_show_text(Widget parent, ui_button_t * button_return,
                              const String text, int width, int height)
{
    Widget shell, tmp;
    Widget show_text;

    shell = ui_create_transient_shell(parent, "showTextShell");
    show_text = XtVaCreateManagedWidget
        ("showText", panedWidgetClass, shell, NULL);
    tmp = XtVaCreateManagedWidget
        ("textBox", formWidgetClass, show_text, NULL);
    tmp = XtVaCreateManagedWidget
        ("text", asciiTextWidgetClass, tmp,
         XtNtype, XawAsciiString, XtNeditType, XawtextRead,
         XtNscrollVertical, XawtextScrollWhenNeeded, XtNdisplayCaret, False,
         XtNstring, text, NULL);
    if (width > 0)
        XtVaSetValues(tmp, XtNwidth, (Dimension)width, NULL);
    if (height > 0)
        XtVaSetValues(tmp, XtNheight, (Dimension)height, NULL);
    tmp = XtVaCreateManagedWidget
        ("buttonBox", boxWidgetClass, show_text,
         XtNshowGrip, False, XtNskipAdjust, True,
         XtNorientation, XtorientHorizontal, NULL);
    tmp = XtVaCreateManagedWidget("closeButton", commandWidgetClass, tmp, NULL);    XtAddCallback(tmp, XtNcallback, UI_BUTTON_CLOSE_callback,
                  (XtPointer) button_return);
    return show_text;
}

static Widget build_confirm_dialog(Widget parent,
                                   ui_button_t *button_return,
                                   Widget *confirm_dialog_message)
{
    Widget shell, confirm_dialog, tmp1, tmp2;

    shell = ui_create_transient_shell(parent, "confirmDialogShell");
    confirm_dialog = XtVaCreateManagedWidget
        ("confirmDialog", panedWidgetClass, shell, NULL);
    tmp1 = XtVaCreateManagedWidget("messageForm", formWidgetClass,
                                   confirm_dialog, NULL);
    *confirm_dialog_message = XtVaCreateManagedWidget
        ("message", labelWidgetClass, tmp1,
         /* XtNresize, False, */
         XtNjustify, XtJustifyCenter, NULL);
    tmp1 = XtVaCreateManagedWidget
        ("buttonBox", boxWidgetClass, confirm_dialog,
         XtNshowGrip, False, XtNskipAdjust, True,
         XtNorientation, XtorientHorizontal, NULL);
    tmp2 = XtVaCreateManagedWidget
        ("yesButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2, XtNcallback, UI_BUTTON_YES_callback,
                  (XtPointer) button_return);
    tmp2 = XtVaCreateManagedWidget
        ("noButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2,
                  XtNcallback, UI_BUTTON_NO_callback,
                  (XtPointer) button_return);
    tmp2 = XtVaCreateManagedWidget
        ("cancelButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2,
                  XtNcallback, UI_BUTTON_CANCEL_callback,
                  (XtPointer) button_return);
    return confirm_dialog;
}

/* ------------------------------------------------------------------------- */

/* Miscellaneous callbacks.  */

UI_CALLBACK(enter_window_callback)
{
    last_visited_app_shell = w;
}

UI_CALLBACK(exposure_callback_shell)
{
    video_canvas_t *canvas = (video_canvas_t *)client_data;

    /* XVideo must be refreshed when the shell window is moved. */
    if (canvas && canvas->videoconfig->hwscale
	&& (canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_1X1
	    || canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_2X2))
    {
        video_canvas_refresh_all(canvas);
    }
}

UI_CALLBACK(exposure_callback_canvas)
{
    video_canvas_t *canvas = (video_canvas_t *)client_data;

    if (!canvas) {
        return;
    }

    /* No resize for XVideo. */
    if (canvas->videoconfig->hwscale
	&& (canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_1X1
	    || canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_2X2))
    {
        video_canvas_refresh_all(canvas);
    }
    else {
        Dimension width, height;

	XtVaGetValues(w, XtNwidth, (XtPointer)&width,
		      XtNheight, (XtPointer)&height, NULL);
        video_canvas_redraw_size(canvas,
				 (unsigned int)width, (unsigned int)height);
    }
}


/* FIXME: this does not handle multiple application shells. */
static void close_action(Widget w, XEvent * event, String * params,
                         Cardinal * num_params)
{
    vsync_suspend_speed_eval();

    ui_exit();
}

void ui_display_statustext(const char *text, int fade_out)
{
    log_message(LOG_DEFAULT, text);
    statusbar_setstatustext(text);
    if (fade_out)
	statustext_display_time = 5;
    else
	statustext_display_time = 0;
}
