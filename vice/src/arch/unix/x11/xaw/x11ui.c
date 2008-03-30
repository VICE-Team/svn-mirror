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
#include <X11/xpm.h>
#endif

#include "drive.h"
#include "interrupt.h"
#include "fullscreenarch.h"
#include "log.h"
#include "machine.h"
#include "mouse.h"
#include "psid.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "uicolor.h"
#include "uihotkey.h"
#include "uimenu.h"
#include "uisettings.h"
#include "utils.h"
#include "vsync.h"
#include "widgets/Canvas.h"
#include "widgets/FileSel.h"
#include "widgets/TextField.h"
#include "video.h"
#include "videoarch.h"


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

static void ui_display_drive_current_image2(void);

/* ------------------------------------------------------------------------- */

void ui_check_mouse_cursor()
{
    int window_doublesize;
#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
        return;
#endif
    if (_mouse_enabled) {
#ifdef USE_XF86_EXTENSIONS
        if (fullscreen_is_enabled) {
            if (resources_get_value("FullscreenDoubleSize",
                                    (resource_value_t *)&window_doublesize) < 0)                return;
        } else
#endif
        {
            if (resources_get_value("DoubleSize",
                                    (resource_value_t *)&window_doublesize) < 0)                return;
        }

        mouse_accel = 4 - 2 * window_doublesize;
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
    }
}

static void ui_restore_mouse(void)
{
#ifdef USE_XF86_EXTENSIONS
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
static XtTranslations drive8_menu_translations, drive9_menu_translations;

/* Application context. */
static XtAppContext app_context;

/* This is needed to catch the `Close' command from the Window Manager. */
static Atom wm_delete_window;

/* Toplevel widget. */
Widget _ui_top_level = NULL;
Widget status_bar = NULL;

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
/*static*/ int num_app_shells = 0;
char last_attached_images[NUM_DRIVES][256];

/* Pixels for updating the drive LED's state.  */
Pixel drive_led_on_red_pixel, drive_led_on_green_pixel, drive_led_off_pixel;

/* If != 0, we should save the settings. */
/* static int resources_have_changed = 0; */

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
    "*speedStatus.font:                         -*-helvetica-medium-r-*-*-12-*",

    NULL
};

/* ------------------------------------------------------------------------- */

static unsigned int wm_command_size;
static unsigned char *wm_command_data;
static Atom wm_command_atom;
static Atom wm_command_type_atom;

static void prepare_wm_command_data(int argc, char **argv)
{
    unsigned int offset, i;

    wm_command_size = 0;
    for (i = 0; i < (unsigned int) argc; i++)
        wm_command_size += strlen(argv[i]) + 1;

    wm_command_data = xmalloc(wm_command_size);

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

    ui_hotkey_init();

    enabled_drives = UI_DRIVE_ENABLE_NONE;

    finish_prepare_wm_command();

    return 0;
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

    resources_get_value("DisplayDepth", (resource_value_t *)&depth);

    if (depth != 0) {
        int i;

        for (i = 0; classes[i].name != NULL; i++) {
            if (XMatchVisualInfo(display, screen, depth, classes[i].class,
                                 &visualinfo))
                break;
        }
        if (!classes[i].name) {
            log_error(ui_log,
                      _("This display does not support suitable %dbit visuals."),
                      depth);
            log_error(ui_log,
                      _("Please select a bit depth supported by your display."));
            return -1;
        } else {
            log_message(ui_log, _("Found %dbit/%s visual."),
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
                    log_message(ui_log, _("Found %dbit/%s visual."),
                                depth, classes[j].name);
                    have_truecolor = (classes[j].class == TrueColor);
                    done = 1;
                    break;
                }
            }
        if (!done) {
            log_error(ui_log, _("Cannot autodetect a proper visual."));
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

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_init() < 0)
        return -1;
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

/* Create a shell with a canvas widget in it.  */
int x11ui_open_canvas_window(video_canvas_t *c, const char *title,
                             int width, int height, int no_autorepeat)
{
    /* Note: this is correct because we never destroy CanvasWindows.  */
    Widget shell, speed_label;
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
        log_error(ui_log, _("Maximum number of toplevel windows reached."));
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
        Dimension w1 = width - 2 - led_width * NUM_DRIVES;

        speed_label = XtVaCreateManagedWidget
            ("speedStatus",
             labelWidgetClass, pane,
             XtNlabel, "",
             XtNwidth, (w1 - NUM_DRIVES * (w1 / 4)) / 2,
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

            name = xmsprintf("driveCurrentImage%d", i + 1);
            drive_current_image[i] = XtVaCreateManagedWidget
                (name,
                 labelWidgetClass, pane,
                 XtNlabel, "",
                 XtNwidth, (w1 / 2) + 13,
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
            free(name);

            name = xmsprintf("driveTrack%d", i + 1);
            drive_track_label[i] = XtVaCreateManagedWidget
                (name,
                 labelWidgetClass, pane,
                 XtNlabel, "",
                 XtNwidth, w1 / 4,
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
            free(name);

            name = xmsprintf("driveLed%d", i + 1);
            drive_led[i] = XtVaCreateManagedWidget
                (name,
                 xfwfcanvasWidgetClass, pane,
                 XtNwidth, led_width,
                 XtNheight, led_height,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_track_label[i],
                 XtNhorizDistance, 0,
                 XtNvertDistance, (height - led_height) / 2 + 1,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 1,
                 NULL);
            free(name);

	    /* double LEDs */

            name = xmsprintf("driveLedA%d", i + 1);
            drive_led1[i] = XtVaCreateManagedWidget
                (name,
                 xfwfcanvasWidgetClass, pane,
                 XtNwidth, led_width / 2 - 1,
                 XtNheight, led_height,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_track_label[i],
                 XtNhorizDistance, 0,
                 XtNvertDistance, (height - led_height) / 2 + 1,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 1,
                 NULL);
            free(name);

            name = xmsprintf("driveLedB%d", i + 1);
            drive_led2[i] = XtVaCreateManagedWidget
                (name,
                 xfwfcanvasWidgetClass, pane,
                 XtNwidth, led_width / 2 - 1,
                 XtNheight, led_height,
                 XtNfromVert, i == 0 ? canvas : drive_track_label[i-1],
                 XtNfromHoriz, drive_led1[i],
                 XtNhorizDistance, 0,
                 XtNvertDistance, (height - led_height) / 2 + 1,
                 XtNtop, XawChainBottom,
                 XtNbottom, XawChainBottom,
                 XtNleft, XawChainRight,
                 XtNright, XawChainRight,
                 XtNjustify, XtJustifyRight,
                 XtNborderWidth, 1,
                 NULL);
            free(name);
        }
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
    app_shells[num_app_shells - 1].title = stralloc(title);
    app_shells[num_app_shells - 1].speed_label = speed_label;
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

    XSetWMProtocols(display, XtWindow(shell), &wm_delete_window, 1);
    XtOverrideTranslations(shell,
                           XtParseTranslationTable
                           ("<Message>WM_PROTOCOLS: Close()"));

    /* This is necessary because the status might have been set before we
       actually open the canvas window.  */
    ui_enable_drive_status(enabled_drives, drive_active_led);

    initBlankCursor();

    c->emuwindow = canvas;
    return 0;
}

/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(Widget w)
{
    char *translation_table;
    char *name = XtName(w);
    int i;

    translation_table =
        concat("<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "@Num_Lock<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
               "@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               NULL);

    left_menu_translations = XtParseTranslationTable(translation_table);
    free(translation_table);

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].canvas, left_menu_translations);

    if (left_menu != NULL)
        XtDestroyWidget(left_menu);
    left_menu = w;
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(Widget w)
{
    char *translation_table;
    char *name = XtName(w);
    int i;

    translation_table =
        concat("<Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "@Num_Lock<Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "Lock <Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
               "@Scroll_Lock <Btn3Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               NULL);
    right_menu_translations = XtParseTranslationTable(translation_table);
    free(translation_table);

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

void ui_set_drive8_menu (Widget w)
{
    char *translation_table;
    char *name = XtName(w);
    int i;

    for (i = 0; i < num_app_shells; i++)
        if (app_shells[i].drive_mapping[0] < 0) {
            XtDestroyWidget(w);
            return;
	}
    
    translation_table =
        concat("<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "@Num_Lock<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
               "@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               NULL);
    drive8_menu_translations = XtParseTranslationTable(translation_table);
    free(translation_table);

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].drive_widgets[app_shells[i].drive_mapping[0]].current_image, 
            drive8_menu_translations);

    /*ui_destroy_drive8_menu();*/

    drive8_menu = w;
}

void ui_set_drive9_menu(Widget w)
{
    char *translation_table;
    char *name = XtName(w);
    int i;

    for (i = 0; i < num_app_shells; i++)
        if (app_shells[i].drive_mapping[1] < 0) {
            XtDestroyWidget(w);
            return;
	}

    translation_table =
        concat("<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "@Num_Lock<Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               "Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n"
               "@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(", name, ") MenuPopup(", name, ")\n",
               NULL);
    drive9_menu_translations = XtParseTranslationTable(translation_table);
    free(translation_table);

    for (i = 0; i < num_app_shells; i++)
        XtOverrideTranslations(app_shells[i].drive_widgets[app_shells[i].drive_mapping[1]].current_image, 
            drive9_menu_translations);

    /*ui_destroy_drive9_menu();*/

    drive9_menu = w;
}

void ui_set_topmenu(const char *menu_name, ...)
{
}

void ui_set_speedmenu(Widget w)
{
}

void ui_set_tape_menu(Widget w)
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
    char *s = concat ("Exit ", machine_name, _(" emulator"), NULL);

    b = ui_ask_confirmation(s, _("Do you really want to exit?"));

    if (b == UI_BUTTON_YES) {
        int save_resources_on_exit;
        resources_get_value("SaveResourcesOnExit",
                            (resource_value_t *)&save_resources_on_exit);
        if (save_resources_on_exit) {
            b = ui_ask_confirmation(s, _("Save the current settings?"));
            if (b == UI_BUTTON_YES) {
                if (resources_save(NULL) < 0)
                    ui_error(_("Cannot save settings."));
            } else if (b == UI_BUTTON_CANCEL) {
                free(s);
                return;
            }
        }
        ui_autorepeat_on();
        ui_restore_mouse();

#ifdef USE_XF86_EXTENSIONS
        fullscreen_suspend(0);
#endif
        exit(0);
    }

    free(s);
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

    resources_get_value("PrivateColormap",
                        (resource_value_t *)&use_private_colormap);

    if (!use_private_colormap
        && depth == DefaultDepth(display, screen)
        && !have_truecolor) {
        colormap = DefaultColormap(display, screen);
    } else {
        log_message(ui_log, _("Using private colormap."));
        colormap = XCreateColormap(display, RootWindow(display, screen),
                                   visual, AllocNone);
    }

    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
    return 0;
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

            str = xmsprintf("%d%%, %d fps %s", percent_int, framerate_int,
                            warp_flag ? _("(warp)") : "");
            XtVaSetValues(app_shells[i].speed_label, XtNlabel, str, NULL);
            free(str);
        }
    }
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
    resources_get_value("DriveTrueEmulation", (resource_value_t *) &true_emu);
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

void ui_display_drive_track(int drive_number, int drive_base,
                                                        double track_number)
{
    int i;
    /* FIXME: Fixed length.  */
    char str[256];

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

void ui_display_drive_led(int drive_number, int status)
{
    Pixel pixel;

    int i;

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
    if (drive_number >= 2)
        return;

    strcpy(&(last_attached_images[drive_number][0]), image);

    /* update drive mapping */
    ui_enable_drive_status(enabled_drives, drive_active_led);
    ui_update_flip_menus(drive_number + 8, drive_number + 8);
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
            free(name);
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

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;

    for (i = 0; i < num_app_shells; i++) {
        if (flag) {
            char *str;

            str = xmsprintf(_("%s (paused)"), app_shells[i].title);
            XtVaSetValues(app_shells[i].shell, XtNtitle, str, NULL);
            free(str);
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

/* Resize one window. */
void x11ui_resize_canvas_window(ui_window_t w, int width, int height)
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
DEFINE_BUTTON_CALLBACK(UI_BUTTON_DEBUG)
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
    str = xmvsprintf(format, ap);
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
    free(str);
}

/* Report a message to the user.  */
void ui_message(const char *format,...)
{
    char *str;
    va_list ap;
    static Widget error_dialog;
    static ui_button_t button;

    va_start(ap, format);
    str = xmvsprintf(format, ap);
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
    free(str);
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

    str = xmvsprintf(format, ap);
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

    ui_popup(XtParent(jam_dialog), "VICE", False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(jam_dialog));
    XtDestroyWidget(XtParent(jam_dialog));

    vsync_suspend_speed_eval();
    ui_dispatch_events();
    free(str);

    switch (button) {
      case UI_BUTTON_MON:
        ui_restore_mouse();
#ifdef USE_XF86_EXTENSIONS
        fullscreen_suspend(0);
#endif
        return UI_JAM_MONITOR;
      case UI_BUTTON_HARDRESET:
        return UI_JAM_HARD_RESET;
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

/* File browser. */
char *ui_select_file(const char *title,
                     char *(*read_contents_func)(const char *),
                     unsigned int allow_autostart, const char *default_dir,
                     const char *default_pattern, ui_button_t *button_return,
                     unsigned int show_preview, int *attach_wp)
{
    static ui_button_t button;
    char *ret;
    Widget file_selector = NULL;
    XfwfFileSelectorStatusStruct fs_status;
    static char *filesel_dir = NULL;
    char *current_dir;

    /* we preserve the current directory over the invocations */
    current_dir = util_get_current_dir();    /* might be changed elsewhere */
    if (filesel_dir != NULL) {
        chdir(filesel_dir);
    }

    /* We always rebuild the file selector from scratch (which is slow),
       because there seems to be a bug in the XfwfScrolledList that causes
       the file and directory listing to disappear randomly.  I hope this
       fixes the problem...  */
    file_selector = build_file_selector(_ui_top_level, &button);

    XtVaSetValues(file_selector, XtNshowAutostartButton, allow_autostart, NULL);    XtVaSetValues(file_selector, XtNshowContentsButton,
                  read_contents_func ? 1 : 0,  NULL);

    XtVaSetValues(file_selector, XtNpattern,
                  default_pattern ? default_pattern : "*", NULL);

    if (default_dir != NULL) {
        XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) file_selector,
                                        default_dir);
    } else {
        char *newdir = util_get_current_dir();

        XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) file_selector,
                                        newdir);
        free(newdir);
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
            char *contents;
            char *f = concat(fs_status.path, fs_status.file, NULL);

            contents = read_contents_func(f);
            free(f);
            if (contents != NULL) {
                ui_show_text(fs_status.file, contents, 250, 240);
                free(contents);
            } else {
                ui_error(_("Unknown image type."));
            }
        }
    } while ((!fs_status.file_selected && button != UI_BUTTON_CANCEL)
             || button == UI_BUTTON_CONTENTS);

    /* `ret' gets always malloc'ed.  */
    if (fs_status.file_selected)
        ret = concat(fs_status.path, fs_status.file, NULL);
    else
        ret = stralloc("");

    ui_popdown(XtParent(file_selector));
#ifndef __alpha
    /* On Alpha, XtDestroyWidget segfaults, don't know why...  */
    XtDestroyWidget(XtParent(file_selector));
#endif
    if (filesel_dir != NULL) {
        free(filesel_dir);
    }
    filesel_dir = util_get_current_dir();
    if (current_dir != NULL) {
        chdir(current_dir);
        free(current_dir);
    }

    *button_return = button;
    if (button == UI_BUTTON_OK || button == UI_BUTTON_AUTOSTART) {
        /* Caller has to free the filename.  */
        return ret;
    } else {
        free(ret);
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

#ifdef USE_XF86_EXTENSIONS
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
            else if ((signed short)my_x + my_width > root_width)
                my_x = root_width - my_width;

            if ((signed short)my_y < 0)
                my_y = 0;
            else if ((signed short)my_y + my_height > root_height)
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

#ifdef USE_XF86_EXTENSIONS
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
    /*log_debug("Shell");*/
}

UI_CALLBACK(exposure_callback_canvas)
{
    Dimension width, height;

    XtVaGetValues(w, XtNwidth, (XtPointer)&width,
                  XtNheight, (XtPointer)&height, NULL);

    if (client_data) {
        video_canvas_redraw_size((struct video_canvas_s *)client_data,
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

/* ------------------------------------------------------------------------- */

static int is_paused = 0;

static void pause_trap(ADDRESS addr, void *data)
{
    ui_display_paused(1);
    is_paused = 1;
    vsync_suspend_speed_eval();
    while (is_paused)
        ui_dispatch_next_event();
}

void ui_pause_emulation(int flag)
{
    if (flag) {
        maincpu_trigger_trap(pause_trap, 0);
    } else {
        ui_display_paused(0);
        is_paused = 0;
    }
}

int ui_emulation_is_paused(void)
{
    return is_paused;
}

