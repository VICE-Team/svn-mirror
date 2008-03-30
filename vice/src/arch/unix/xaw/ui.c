/*
 * ui.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat    (fachat@physik.tu-chemnitz.de)
 *
 * Support for multiple visuals and depths by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

#define _UI_XAW_C

#include "vice.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

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

#ifdef XPM
#include <X11/xpm.h>
#include "icon.h"
#endif

#include <X11/keysym.h>

#ifdef HAVE_X11_SUNKEYSYM_H
#include <X11/Sunkeysym.h>
#endif

#include "widgets/Canvas.h"
#include "widgets/FileSel.h"
#include "widgets/TextField.h"

#include "ui.h"
#include "uimenu.h"
#include "serial.h"
#include "interrupt.h"
#include "patchlevel.h"
#include "info.h"
#include "vsync.h"
#include "mem.h"
#include "sound.h"
#include "drive.h"
#include "tape.h"
#include "resources.h"
#include "cmdline.h"
#include "mon.h"
#include "utils.h"
#include "kbd.h"
#include "true1541.h"
#include "attach.h"
#include "autostart.h"

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

#ifdef REU
#include "reu.h"
#endif

/* These are used by video_x.[ch].  FIXME: I (EP) want these to be static.  */
Display *display;
int screen;
Visual *visual;
int depth = X_DISPLAY_DEPTH;

/* ------------------------------------------------------------------------- */

_ui_resources_t _ui_resources;

/* Warning: This cannot actually be changed at runtime.  */
static int set_depth(resource_value_t v)
{
    int d = (int) v;

    /* Minimal sanity check.  */
    if (d < 0 || d > 32)
        return -1;

    _ui_resources.depth = d;
    return 0;
}

static int set_html_browser_command(resource_value_t v)
{
    string_set(&_ui_resources.html_browser_command, (char *)v);
    return 0;
}

static int set_use_private_colormap(resource_value_t v)
{
    _ui_resources.use_private_colormap = (int) v;
    return 0;
}

static int set_save_resources_on_exit(resource_value_t v)
{
    _ui_resources.save_resources_on_exit = (int) v;
    return 0;
}

static resource_t resources[] = {
    { "HTMLBrowserCommand", RES_STRING, (resource_value_t) "netscape %s",
      (resource_value_t *) &_ui_resources.html_browser_command,
      set_html_browser_command },
    { "PrivateColormap", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &_ui_resources.use_private_colormap,
      set_use_private_colormap },
    { "SaveResourcesOnExit", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &_ui_resources.save_resources_on_exit,
      set_save_resources_on_exit },
    { "DisplayDepth", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &_ui_resources.depth,
      set_depth },
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------- */

static cmdline_option_t cmdline_options[] = {
    { "-htmlbrowser", SET_RESOURCE, 1, NULL, NULL, "HTMLBrowserCommand", NULL,
      "<command>", "Specify an HTML browser for the on-line help" },
    { "-install", SET_RESOURCE, 0, NULL, NULL,
      "PrivateColormap", (resource_value_t) 1,
      NULL, "Install a private colormap" },
    { "+install", SET_RESOURCE, 0, NULL, NULL,
      "PrivateColormap", (resource_value_t) 0,
      NULL, "Use the default colormap" },
    { "-saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (resource_value_t) 1,
      NULL, "Save settings (resources) on exit" },
    { "+saveres", SET_RESOURCE, 0, NULL, NULL,
      "SaveResourcesOnExit", (resource_value_t) 0,
      NULL, "Never save settings (resources) on exit" },
    { "-displaydepth", SET_RESOURCE, 1, NULL, NULL,
      "DisplayDepth", NULL,
      "<value>", "Specify X display depth (1..32)" },
    { NULL }
};

int ui_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

static int popped_up_count = 0;

/* Left-button and right-button menu.  */
static Widget left_menu, right_menu;

/* Application context. */
static XtAppContext app_context;

/* This is needed to catch the `Close' command from the Window Manager. */
static Atom wm_delete_window;

#ifdef XPM
/* Icon.  */
static Pixmap icon_pixmap;
#endif

/* Toplevel widget. */
Widget _ui_top_level = NULL;

/* xdebugger, Jarkko's X11-based 6502 debugger. */
static Widget xdebugger;

/* Our colormap. */
static Colormap colormap;

/* This allows us to pop up the transient shells centered to the last visited
   shell. */
static Widget last_visited_app_shell = NULL;
#define MAX_APP_SHELLS 10
static struct {
    String title;
    Widget shell;
    Widget canvas;
    Widget speed_label;
    Widget drive_track_label;
    Widget drive_led;
} app_shells[MAX_APP_SHELLS];
static int num_app_shells = 0;

/* Pixels for updating the drive LED's state.  */
Pixel drive_led_on_pixel, drive_led_off_pixel;

/* If != 0, we should save the settings. */
static int resources_have_changed = 0;

/* ------------------------------------------------------------------------- */

static int alloc_colormap(void);
static int alloc_colors(const palette_t *palette, PIXEL pixel_return[]);
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
static Widget build_info_dialog(Widget parent,
                                ui_button_t *button_return,...);
static void position_submenu(Widget w, Widget parent);
static void close_action(Widget w, XEvent *event, String *params,
                         Cardinal *num_params);
static void handle_special_keys(Widget w, XtPointer closure,
                                XEvent *x_event,
                                Boolean *continue_to_dispatch);

UI_CALLBACK(enter_window_callback);
UI_CALLBACK(exposure_callback);

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
    "*driveTrack.font:                          -*-lucida-medium-r-*-*-12-*",
    "*speedStatus.font:                         -*-lucida-medium-r-*-*-12-*",

    XDEBUG_FALLBACK_RESOURCES,
    NULL
};

/* ------------------------------------------------------------------------- */

/* Initialize the GUI and parse the command line. */
int ui_init(int *argc, char **argv)
{
    /* Create the toplevel. */
    _ui_top_level = XtAppInitialize(&app_context, "VICE", NULL, 0, argc, argv,
				    fallback_resources, NULL, 0);
    if (!_ui_top_level)
	return -1;
    display = XtDisplay(_ui_top_level);
    screen = XDefaultScreen(display);
    atexit(ui_autorepeat_on);
    return 0;
}

typedef struct {
    char *name;
    int class;
} namedvisual_t;

/* Continue GUI initialization after resources are set. */
int ui_init_finish(void)
{
    static XtActionsRec actions[] = {
	{ "Close", close_action },
    };
    static namedvisual_t classes[] = {
	{ "PseudoColor", PseudoColor },
	{ "TrueColor", TrueColor },
	{ "StaticGray", StaticGray },
	{ NULL }
    };
    XVisualInfo visualinfo;

    if (depth != 0) {
	int i;

	for (i = 0; classes[i].name != NULL; i++) {
	    if (XMatchVisualInfo(display, screen, depth, classes[i].class,
				 &visualinfo))
		break;
	}
	if (!classes[i].name) {
	    fprintf(stderr,
		    "\nThis display does not support suitable %dbit visuals.\n"
#if X_DISPLAY_DEPTH == 0
		    "Please select a bit depth supported by your display.\n",
#else
		    "Please recompile the program for a supported bit depth.\n",
#endif
		    depth);
	    return -1;
	} else
	    printf("Found %dbit/%s visual.\n", depth, classes[i].name);
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
		    printf("Found %dbit/%s visual.\n",
			   depth, classes[j].name);
		    done = 1;
		    break;
		}
	    }
	if (!done) {
	    fprintf(stderr, "Couldn't autodetect a proper visual.\n");
	    return -1;
	}
    }

    visual = visualinfo.visual;

    /* Allocate the colormap. */
    alloc_colormap();

    /* Recreate _ui_top_level to support non-default display depths.  We also
       copy the WM_COMMAND' property for a minimal session-management support
       (WindowMaker loves that).  FIXME: correct way to do so?  This looks like
       a big dirty kludge... I cannot believe there is not a cleaner
       method.  */
    {
	Atom wm_command_atom = XInternAtom(display, "WM_COMMAND", False);
	Atom wm_command_type;
	int wm_command_format;
	unsigned long wm_command_nitems;
	unsigned char *wm_command_data;
	int wm_command_present = 0;

	/* Realize the old toplevel.  This ugliness is required to create the
           window we retrieve the `WM_COMMAND' property from. */
	XtVaSetValues(_ui_top_level,
		      XtNwidth, 1,
		      XtNheight, 1,
		      XtNmappedWhenManaged, False,
		      NULL);
	XtRealizeWidget(_ui_top_level);

	/* Retrieve the `WM_COMMAND' property. */
	if (wm_command_atom != None) {
	    unsigned long dummy;

	    if (Success == XGetWindowProperty(display,
					      XtWindow(_ui_top_level),
					      wm_command_atom,
					      0, (unsigned long)-1,
					      False,
					      AnyPropertyType,
					      &wm_command_type,
					      &wm_command_format,
					      &wm_command_nitems,
					      &dummy,
					      &wm_command_data))
		wm_command_present = 1;
	}

	/* Goodbye... */
	XtDestroyWidget(_ui_top_level);

	/* Create the new _ui_top_level. */
	_ui_top_level = XtVaAppCreateShell(EMULATOR, "VICE",
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
	if (wm_command_present) {
	    XChangeProperty(display, XtWindow(_ui_top_level), wm_command_atom,
			    wm_command_type,
			    wm_command_format, PropModeReplace,
			    (char *)wm_command_data, wm_command_nitems);
	    XtFree(wm_command_data);
	}
    }

#ifdef XPM
    /* Create the icon pixmap. */
    XpmCreatePixmapFromData(display, DefaultRootWindow(display), icon_data,
			    &icon_pixmap, NULL, NULL);
#endif

    wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    XtAppAddActions(app_context, actions, XtNumber(actions));

    return ui_menu_init(app_context, display, screen);
}

/* Create a shell with a canvas widget in it.  */
ui_window_t ui_open_canvas_window(const char *title, int width, int height,
                                  int no_autorepeat,
                                  ui_exposure_handler_t exposure_proc,
                                  const palette_t *palette,
                                  PIXEL pixel_return[])
{
    /* Note: this is correct because we never destroy CanvasWindows.  */
    static int menus_created = 0;
    XtTranslations translations;
    Widget shell, canvas, pane, speed_label;
    Widget drive_track_label, drive_led;
    XSetWindowAttributes attr;

    if (alloc_colors(palette, pixel_return) == -1)
	return NULL;

    /* colormap might have changed after ui_alloc_colors, so we set it again */
    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);

    if (++num_app_shells > MAX_APP_SHELLS) {
	fprintf(stderr,
	  	"ui_open_canvas_window(): maximum number of open windows reached.\n");
	return NULL;
    }

    shell = XtVaCreatePopupShell(title, applicationShellWidgetClass,
                                 _ui_top_level, XtNinput, True, XtNtitle, title,
                                 XtNiconName, title, NULL);

#ifdef XPM
    XtVaSetValues(shell, XtNiconPixmap, icon_pixmap, NULL);
#endif

    pane = XtVaCreateManagedWidget("Form", formWidgetClass, shell,
                                   XtNdefaultDistance, 2,
                                   NULL);

    canvas = XtVaCreateManagedWidget("Canvas", xfwfcanvasWidgetClass, pane,
                                     XtNwidth, width,
                                     XtNheight, height,
                                     XtNresizable, True,
                                     XtNbottom, XawChainBottom,
                                     XtNtop, XawChainTop,
                                     XtNleft, XawChainLeft,
                                     XtNright, XawChainRight,
                                     XtNborderWidth, 1,
                                     NULL);

    XtAddEventHandler(shell, EnterWindowMask, False,
		      (XtEventHandler) enter_window_callback, NULL);
    XtAddEventHandler(canvas, ExposureMask | StructureNotifyMask, False,
		      (XtEventHandler) exposure_callback, exposure_proc);

    /* Create the status bar on the bottom.  */
    {
        Dimension height;
        Dimension led_width = 12, led_height = 5;
        Dimension w1 = width - led_width - 2;

        speed_label = XtVaCreateManagedWidget("speedStatus",
                                              labelWidgetClass, pane,
                                              XtNlabel, "",
                                              XtNwidth, w1 - w1 /3,
                                              XtNfromVert, canvas,
                                              XtNtop, XawChainBottom,
                                              XtNbottom, XawChainBottom,
                                              XtNleft, XawChainLeft,
                                              XtNright, XawChainRight,
                                              XtNjustify, XtJustifyLeft,
                                              XtNborderWidth, 0,
                                              NULL);

        drive_track_label = XtVaCreateManagedWidget("driveTrack",
                                                    labelWidgetClass, pane,
                                                    XtNlabel, "",
                                                    XtNwidth, w1 / 3,
                                                    XtNfromVert, canvas,
                                                    XtNfromHoriz, speed_label,
                                                    XtNhorizDistance, 0,
                                                    XtNtop, XawChainBottom,
                                                    XtNbottom, XawChainBottom,
                                                    XtNleft, XawChainRight,
                                                    XtNright, XawChainRight,
                                                    XtNjustify, XtJustifyRight,
                                                    XtNborderWidth, 0,
                                                    NULL);

        XtVaGetValues(speed_label, XtNheight, &height, NULL);

        drive_led = XtVaCreateManagedWidget("driveLed",
                                            xfwfcanvasWidgetClass, pane,
                                            XtNwidth, led_width,
                                            XtNheight, led_height,
                                            XtNfromVert, canvas,
                                            XtNfromHoriz, drive_track_label,
                                            XtNhorizDistance, 0,
                                            XtNvertDistance,
                                            (height - led_height) / 2 + 1,
                                            XtNtop, XawChainBottom,
                                            XtNbottom, XawChainBottom,
                                            XtNleft, XawChainRight,
                                            XtNright, XawChainRight,
                                            XtNjustify, XtJustifyRight,
                                            XtNborderWidth, 1,
                                            NULL);
    }

    /* Assign proper translations to open the menus. */

    translations = XtParseTranslationTable
  	("<Btn1Down>: XawPositionSimpleMenu(LeftMenu) MenuPopup(LeftMenu)\n"
	 "@Num_Lock<Btn1Down>: XawPositionSimpleMenu(LeftMenu) MenuPopup(LeftMenu)\n"
	 "Lock <Btn1Down>: XawPositionSimpleMenu(LeftMenu) MenuPopup(LeftMenu)\n"
	 "@Scroll_Lock <Btn1Down>: XawPositionSimpleMenu(LeftMenu) MenuPopup(LeftMenu)\n"
	 "<Btn3Down>: XawPositionSimpleMenu(RightMenu) MenuPopup(RightMenu)\n"
	 "@Num_Lock<Btn3Down>: XawPositionSimpleMenu(RightMenu) MenuPopup(RightMenu)\n"
	 "Lock <Btn3Down>: XawPositionSimpleMenu(RightMenu) MenuPopup(RightMenu)\n"
	 "@Scroll_Lock <Btn3Down>: XawPositionSimpleMenu(RightMenu) MenuPopup(RightMenu)\n");
    XtOverrideTranslations(canvas, translations);

    if (no_autorepeat) {
	XtAddEventHandler(canvas, EnterWindowMask, False,
			  (XtEventHandler) ui_autorepeat_off, NULL);
	XtAddEventHandler(canvas, LeaveWindowMask, False,
			  (XtEventHandler) ui_autorepeat_on, NULL);
	XtAddEventHandler(shell, KeyPressMask, False,
			  (XtEventHandler) handle_special_keys, NULL);
	XtAddEventHandler(canvas, KeyPressMask, False,
			  (XtEventHandler) handle_special_keys, NULL);
    }

    XtRealizeWidget(shell);
    XtPopup(shell, XtGrabNone);

    attr.backing_store = Always;
    XChangeWindowAttributes(display, XtWindow(canvas),
    	 		    CWBackingStore, &attr);

    XSetWMProtocols(display, XtWindow(shell), &wm_delete_window, 1);
    XtOverrideTranslations(shell, XtParseTranslationTable
                           ("<Message>WM_PROTOCOLS: Close()"));

    app_shells[num_app_shells - 1].shell = shell;
    app_shells[num_app_shells - 1].canvas = canvas;
    app_shells[num_app_shells - 1].title = stralloc(title);
    app_shells[num_app_shells - 1].speed_label = speed_label;
    app_shells[num_app_shells - 1].drive_track_label = drive_track_label;
    app_shells[num_app_shells - 1].drive_led = drive_led;

    return canvas;
}

void ui_set_left_menu(Widget w)
{
    if (left_menu != NULL)
        XtDestroyWidget(w);
    left_menu = w;
}

void ui_set_right_menu(Widget w)
{
    if (right_menu != NULL)
        XtDestroyWidget(w);
    right_menu = w;
}

/* ------------------------------------------------------------------------- */

/* Set the colormap variable.  The user must tell us whether he wants the
   default one or not using the `privateColormap' resource.  */
static int alloc_colormap(void)
{
    if (colormap)
	return 0;

    if (!_ui_resources.use_private_colormap
	&& depth == DefaultDepth(display, screen)) {
	colormap = DefaultColormap(display, screen);
    } else {
	colormap = XCreateColormap(display, RootWindow(display, screen),
				   visual, AllocNone);
    }

    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
    return 0;
}

/* Allocate colors in the colormap. */
static int do_alloc_colors(const palette_t *palette, PIXEL pixel_return[],
                           int releasefl)
{
    static int allocated_colors;
    int i, failed;
    XColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);
    unsigned long *xpixels = (unsigned long *) xmalloc(sizeof(unsigned long)
                                                       * palette->num_entries);

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, visual, depth,
		      ZPixmap, 0, (char *)data, 1, 1, 8, 0);
    if (!im)
        return -1;

    color.flags = DoRed | DoGreen | DoBlue;
    for (i = 0, failed = 0; i < palette->num_entries; allocated_colors++, i++)
    {
        printf("Allocating color `%s'...\n", palette->entries[i].name);
        color.red = palette->entries[i].red << 8;
        color.green = palette->entries[i].green << 8;
        color.blue = palette->entries[i].blue << 8;
        if (!XAllocColor(display, colormap, &color)) {
            failed = 1;
            fprintf(stderr,
                    "Warning: cannot allocate color \"#%04X%04X%04X\"\n",
                    color.red, color.green, color.blue);
        }
        XPutPixel(im, 0, 0, color.pixel);
        xpixels[i] = color.pixel;
#if X_DISPLAY_DEPTH == 0
        {
            /* XXX: prototypes where? */
            extern PIXEL  real_pixel1[];
            extern PIXEL2 real_pixel2[];
            extern PIXEL4 real_pixel4[];
            extern long   real_pixel[];
            extern BYTE   shade_table[];
            pixel_return[i] = i;
            if (depth == 8)
                pixel_return[i] = *data;
            else if (im->bits_per_pixel == 8)
                real_pixel1[i] = *(PIXEL *)data;
            else if (im->bits_per_pixel == 16)
                real_pixel2[i] = *(PIXEL2 *)data;
            else if (im->bits_per_pixel == 32)
                real_pixel4[i] = *(PIXEL4 *)data;
            else
                real_pixel[i] = color.pixel;
            if (im->bits_per_pixel == 1)
                shade_table[i] = palette->entries[i].dither;
        }
#else
        pixel_return[i] = *data;
#endif
    }

    if (releasefl && failed && (--i)) {
        if (colormap != DefaultColormap(display, screen))
            XFreeColors(display, colormap, xpixels, i, 0);
        allocated_colors -= i + 1;
    }

    free(xpixels);
    XDestroyImage(im);

    if (!failed) {
        XColor screen, exact;

        if (!XAllocNamedColor(display, colormap, "black", &screen, &exact))
            failed = 1;
        else
            drive_led_off_pixel = screen.pixel;

        if (!failed) {
            if (!XAllocNamedColor(display, colormap, "red", &screen, &exact))
                failed = 1;
            else
                drive_led_on_pixel = screen.pixel;
        }
    }

    return failed;
}

/* In here we try to allocate the given colors. This function is called from
 * 1ui_open_canvas_window()'.  The calling function sets the colormap
 * resource of the toplevel window.  If there is not enough place in the
 * colormap for all color entries, we allocate a new one.  If we someday open
 * two canvas windows, and the colormap fills up during the second one, we
 * might run into trouble, although I am not sure.  (setting the Toplevel
 * colormap will not change the colormap of already opened children)
 *
 * 20jan1998 A.Fachat */
static int alloc_colors(const palette_t *palette, PIXEL pixel_return[])
{
    int failed;

    failed = do_alloc_colors(palette, pixel_return, 1);
    if (failed) {
	if (colormap == DefaultColormap(display, screen)) {
            printf("Automagically using a private colormap.\n");
	    colormap = XCreateColormap(display, RootWindow(display, screen),
				       visual, AllocNone);
	    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);
	    failed = do_alloc_colors(palette, pixel_return, 0);
	}
    }
    return failed ? -1 : 0;
}

/* Return the drawable for the canvas in the specified ui_window_t. */
Window ui_canvas_get_drawable(ui_window_t w)
{
    return XtWindow(w);
}

/* Show the speed index to the user.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    char str[256];
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);

    for (i = 0; i < num_app_shells; i++) {
	if (!percent) {
	    XtVaSetValues(app_shells[i].speed_label, XtNlabel,
                          warp_flag ? "(warp)" : "",
			  NULL);
	} else {
	    sprintf(str, "%d%%, %d fps %s",
                    percent_int, framerate_int, warp_flag ? "(warp)" : "");
	    XtVaSetValues(app_shells[i].speed_label, XtNlabel, str, NULL);
	}
    }
}

void ui_toggle_drive_status(int state)
{
    int i;

    printf("%s(%d)\n", __FUNCTION__, state);
    for (i = 0; i < num_app_shells; i++) {
        if (state) {
            XtRealizeWidget(app_shells[i].drive_track_label);
            XtManageChild(app_shells[i].drive_track_label);
            XtRealizeWidget(app_shells[i].drive_led);
            XtManageChild(app_shells[i].drive_led);
        } else{
            XtUnrealizeWidget(app_shells[i].drive_track_label);
            XtUnrealizeWidget(app_shells[i].drive_led);
        }
    }
}

void ui_display_drive_track(double track_number)
{
    int i;
    char str[256];

    sprintf(str, "Track %.1f", (double)track_number);
    for (i = 0; i < num_app_shells; i++) {
	Widget w = app_shells[i].drive_track_label;

	if (!XtIsRealized(w)) {
	    XtRealizeWidget(w);
	    XtManageChild(w);
	}
        XtVaSetValues(w, XtNlabel, str, NULL);
    }
}

void ui_display_drive_led(int status)
{
    Pixel pixel = status ? drive_led_on_pixel : drive_led_off_pixel;
    int i;

    for (i = 0; i < num_app_shells; i++) {
	Widget w = app_shells[i].drive_led;

	if (!XtIsRealized(w)) {
	    XtRealizeWidget(w);
	    XtManageChild(w);
	}
        XtVaSetValues(w, XtNbackground, pixel, NULL);
    }
}

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;
    char str[1024];

    for (i = 0; i < num_app_shells; i++) {
	if (flag) {
	    sprintf(str, "%s (paused)", app_shells[i].title);
	    XtVaSetValues(app_shells[i].shell, XtNtitle, str, NULL);
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
    while (XtAppPending(app_context) || ui_menu_any_open())
	ui_dispatch_next_event();
}

/* Resize one window. */
void ui_resize_canvas_window(ui_window_t w, int width, int height)
{
    Dimension canvas_width, canvas_height;
    Dimension form_width, form_height;

    /* Ok, form widgets are stupid animals; in a perfect world, I should be
       allowed to resize the canvas and let the Form do the rest.  Unluckily,
       this does not happen, so let's do things the dirty way then.  This
       code sucks badly.  */

    XtVaGetValues((Widget)w, XtNwidth, &canvas_width, XtNheight,
		  &canvas_height, NULL);
    XtVaGetValues(XtParent(XtParent((Widget)w)), XtNwidth, &form_width,
		  XtNheight, &form_height, NULL);

    XtVaSetValues(XtParent(XtParent((Widget)w)),
		  XtNwidth, form_width + width - canvas_width,
		  XtNheight, form_height + height - canvas_height,
		  NULL);

    return;
}

/* Map one window. */
void ui_map_canvas_window(ui_window_t w)
{
    XtPopup(w, XtGrabNone);
}

/* Unmap one window. */
void ui_unmap_canvas_window(ui_window_t w)
{
    XtPopdown(w);
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
    UI_CALLBACK(##button##_callback)            \
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

/* Handle all special keys. */
static void handle_special_keys(Widget w, XtPointer closure,
                                XEvent *x_event,
                                Boolean *continue_to_dispatch)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;

    XLookupString(&x_event->xkey, buffer, 20, &key, &compose);

    switch (key) {
        /* FIXME: THese should be defined in the menu definitions.  */
#if 0
      case XK_F9:
	ui_toggle_warp_mode(NULL, NULL, NULL);
	break;
      case XK_F10:
	UiAttachDisk(NULL, (XtPointer) 8, NULL);
	break;
#if defined(CBM64) || defined(C128)
      case XK_F11:
#ifdef HAVE_X11_SUNKEYSYM_H
      case SunXK_F36:		/* SUN does it its own way... */
#endif
	UiSwapJoystickPorts(NULL, NULL, NULL);
	break;
#endif
      case XK_F12:
#ifdef HAVE_X11_SUNKEYSYM_H
      case SunXK_F37:		/* SUN does it its own way... */
#endif
	UiPowerUpReset(NULL, NULL, NULL);
	break;
#endif
      default:
	break;
    }
}

/* ------------------------------------------------------------------------- */

/* Report an error to the user. */
void ui_error(const char *format,...)
{
    char str[1024];
    va_list ap;
    static Widget error_dialog;
    static ui_button_t button;

    va_start(ap, format);
    vsprintf(str, format, ap);
    error_dialog = build_error_dialog(_ui_top_level, &button, str);
    ui_popup(XtParent(error_dialog), "VICE Error!", False);
    button = UI_BUTTON_NONE;
    do
	ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(error_dialog));
    XtDestroyWidget(XtParent(error_dialog));
    ui_dispatch_events();
    suspend_speed_eval();
}

/* Report a message to the user. */
void ui_message(const char *format,...)
{
    char str[1024];
    va_list ap;
    static Widget error_dialog;
    static ui_button_t button;

    va_start(ap, format);
    vsprintf(str, format, ap);
    error_dialog = build_error_dialog(_ui_top_level, &button, str);
    ui_popup(XtParent(error_dialog), "VICE", False);
    button = UI_BUTTON_NONE;
    do
	ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(error_dialog));
    XtDestroyWidget(XtParent(error_dialog));
    ui_dispatch_events();
    suspend_speed_eval();
}

void activate_xdebug_window(void)
{
    if (!xdebugger) {
	xdebugger = XtVaCreatePopupShell("XDebugger",
					 applicationShellWidgetClass,
					 _ui_top_level,
					 NULL);
	XtOverrideTranslations(xdebugger,
			       (XtParseTranslationTable
				("<Message>WM_PROTOCOLS: Close()")));
	xdebug_create(xdebugger);
    }
    XtPopup(xdebugger, XtGrabNone);
    XSetWMProtocols(display, XtWindow(xdebugger), &wm_delete_window, 1);
    xdebug_enable();
}

/* Report a message to the user, allow different buttons. */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    char str[1024];
    va_list ap;
    static Widget jam_dialog, shell, tmp, mform, bbox;
    static ui_button_t button;

    va_start(ap, format);

    shell = ui_create_transient_shell(_ui_top_level, "jamDialogShell");
    jam_dialog = XtVaCreateManagedWidget
	("jamDialog", panedWidgetClass, shell, NULL);
    mform = XtVaCreateManagedWidget
	("messageForm", formWidgetClass, jam_dialog, NULL);

    vsprintf(str, format, ap);
    tmp = XtVaCreateManagedWidget
	("label", labelWidgetClass, mform,
	 XtNresize, False, XtNjustify, XtJustifyCenter, XtNlabel, str,
	 NULL);

    bbox = XtVaCreateManagedWidget
	("buttonBox", boxWidgetClass, jam_dialog,
	 XtNshowGrip, False, XtNskipAdjust, XtNorientation,
	 XtorientHorizontal, NULL);

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
	("debugButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_DEBUG_callback,
		  (XtPointer) &button);

    ui_popup(XtParent(jam_dialog), "VICE", False);
    button = UI_BUTTON_NONE;
    do
	ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(XtParent(jam_dialog));
    XtDestroyWidget(XtParent(jam_dialog));

    if (button == UI_BUTTON_DEBUG) {
	activate_xdebug_window();
	XSync(display, False);
    }

    suspend_speed_eval();
    ui_dispatch_events();

    switch (button) {
      case UI_BUTTON_MON:
	return UI_JAM_MONITOR;
      case UI_BUTTON_HARDRESET:
        return UI_JAM_HARD_RESET;
      case UI_BUTTON_DEBUG:
        return UI_JAM_DEBUG;
      case UI_BUTTON_RESET:
      default:
        return UI_JAM_RESET;
    }
}

int ui_extend_image_dialog(void)
{
    ui_button_t b;

    suspend_speed_eval();
    b = ui_ask_confirmation("Extend disk image",
                            ("Do you want to extend the disk image"
                             " to 40 tracks?"));
    return (b == UI_BUTTON_YES) ? 1 : 0;
}

/* File browser. */
char *ui_select_file(const char *title,
                     char *(*read_contents_func)(const char *),
                     int allow_autostart,
                     ui_button_t *button_return)
{
    static ui_button_t button;
    static char *ret = NULL;
    static Widget file_selector = NULL;
    XfwfFileSelectorStatusStruct fs_status;
    char *curdir;

#ifndef __alpha
    /* We always rebuild the file selector from scratch (which is slow),
       because there seems to be a bug in the XfwfScrolledList that causes
       the file and directory listing to disappear randomly.  I hope this
       fixes the problem...  */
    file_selector = build_file_selector(_ui_top_level, &button);
#else
    /* Unluckily, this does not work on Alpha (segfault when the widget is
       popped down).  There is probably something wrong in some widget, but
       we have no time to check this...  FIXME: Then Alpha users could get
       the "disappearing list" bug.  Grpmf.  */
    if (file_selector == NULL)
	file_selector = build_file_selector(_ui_top_level, &button);
#endif

    XtVaSetValues(file_selector, XtNshowAutostartButton, allow_autostart, NULL);

    curdir = get_current_dir();
    XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) file_selector,
				    curdir);
    free(curdir);

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
		ui_error("Unknown image type.");
	    }
	}
    } while ((!fs_status.file_selected && button != UI_BUTTON_CANCEL)
	     || button == UI_BUTTON_CONTENTS);

    if (ret != NULL)
	free(ret);

    if (fs_status.file_selected)
	ret = concat(fs_status.path, fs_status.file, NULL);
    else
	ret = stralloc("");

    ui_popdown(XtParent(file_selector));

#ifndef __alpha
    /* On Alpha, XtDestroyWidget segfaults, don't know why...  */
    XtDestroyWidget(XtParent(file_selector));
#endif

    *button_return = button;
    if (button == UI_BUTTON_OK || button == UI_BUTTON_AUTOSTART)
	return ret;
    else
	return NULL;
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

Widget ui_create_transient_shell(Widget parent, const char *name)
{
    Widget w;

    w = XtVaCreatePopupShell
	(name, transientShellWidgetClass, parent, XtNinput, True, NULL);
    XtOverrideTranslations(w, XtParseTranslationTable
			   ("<Message>WM_PROTOCOLS: Close()"));
    return w;
}

/* Pop up a popup shell and center it to the last visited AppShell */
void ui_popup(Widget w, const char *title, Boolean wait_popdown)
{
    Widget s = NULL;

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

    if (s) {
	/* Center the popup. */
	Dimension my_width, my_height, shell_width, shell_height;
	Dimension my_x, my_y;
	Position tlx, tly;

	XtRealizeWidget(w);
	XtVaGetValues(w, XtNwidth, &my_width, XtNheight, &my_height, NULL);
	XtVaGetValues(s, XtNwidth, &shell_width, XtNheight, &shell_height,
		      XtNx, &tlx, XtNy, &tly, NULL);
	XtTranslateCoords(XtParent(s), tlx, tly, &tlx, &tly);
	my_x = tlx + (shell_width - my_width) / 2;
	my_y = tly + (shell_height - my_height) / 2;
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
    if (--popped_up_count < 0)
	popped_up_count = 0;
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
	 XtNshowGrip, False, XtNskipAdjust, XtNorientation,
	 XtorientHorizontal, NULL);
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
    tmp = XtVaCreateManagedWidget("closeButton", commandWidgetClass, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, UI_BUTTON_CLOSE_callback,
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
	("message", labelWidgetClass, tmp1, XtNresize, False,
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

#if 0
static Widget build_popup_menu(Widget parent, const char *name, ...)
{
    Widget w;
    unsigned int i, j;
    static int level = 0;
    ui_menu_entry_t *list;
    va_list ap;

    level++;
    w = XtCreatePopupShell(name, simpleMenuWidgetClass, parent, NULL, 0);
    if (level == 1) {
	XtAddCallback(w, XtNpopupCallback, menu_popup_callback, NULL);
	XtAddCallback(w, XtNpopdownCallback, menu_popdown_callback, NULL);
    }
    XtOverrideTranslations
	(w, XtParseTranslationTable
	 ("<BtnMotion>: highlight() PositionSubmenu()\n"
	  "@Num_Lock<BtnMotion>: highlight() PositionSubmenu()\n"
	  "<LeaveWindow>: Unhighlight()\n"
	  "<BtnUp>: Popdownsubmenus() MenuPopdown() notify() unhighlight()"));

    va_start(ap, name);
    while ((list = va_arg(ap, ui_menu_entry_t *)) != NULL) {
        for (i = j = 0; list[i].string; i++) {
            Widget new_item;
            char name[256];

            sprintf(name, "MenuItem%d", j);	/* ugly... */
            switch (*list[i].string) {
              case '-':		/* line */
                new_item = XtCreateManagedWidget("separator",
                                                 smeLineObjectClass, w,
                                                 NULL, 0);
                break;
              case '*':		/* toggle */
                new_item = XtVaCreateManagedWidget(name,
                                                   smeBSBObjectClass, w,
                                                   XtNrightMargin, 20,
                                                   XtNleftMargin, 20,
                                                   XtNlabel,
                                                   list[i].string + 1,
                                                   NULL);
                /* Add this item to the list of calls to perform to update the
                   menu status. */
                if (list[i].callback) {
                    if (num_checkmark_menu_items < MAX_UPDATE_MENU_LIST_SIZE)
                        checkmark_menu_items[num_checkmark_menu_items++] = new_item;
                    else {
                        fprintf(stderr,
                                "Maximum number of menus reached!  "
                                "Please fix the code.\n");
                        exit(-1);
                    }
                }
                j++;
                break;
              default:
                new_item = XtVaCreateManagedWidget(name, smeBSBObjectClass, w,
                                                   XtNleftMargin, 20,
                                                   XtNrightMargin, 20,
                                                   XtNlabel, list[i].string,
                                                   NULL);
                j++;
            }
            if (list[i].callback)
                XtAddCallback(new_item, XtNcallback,
                              (XtCallbackProc) list[i].callback,
                              list[i].callback_data);
            if (list[i].sub_menu) {
                Widget sub;

                if (num_submenus > MAX_SUBMENUS) {
                    fprintf(stderr,
                            "Maximum number of sub menus reached! "
                            "Please fix the code.\n");
                    exit(-1);
                }
                XtVaSetValues(new_item, XtNrightBitmap, right_arrow_bitmap,
                              NULL);
                sub = build_popup_menu(parent,
                                       "SUB", list[i].sub_menu, NULL);
                submenus[num_submenus].widget = sub;
                submenus[num_submenus].parent = new_item;
                submenus[num_submenus].level = level;
                XtAddCallback(sub,
                              XtNpopupCallback, submenu_popup_callback,
                              submenus + num_submenus);
                XtAddCallback(sub,
                              XtNpopdownCallback, submenu_popdown_callback,
                              (XtPointer) w);
                num_submenus++;
            }
        }
    }

    level--;

    va_end(ap);
    return w;
}
#endif

/* ------------------------------------------------------------------------- */

/* Miscellaneous callbacks.  */

UI_CALLBACK(enter_window_callback)
{
    last_visited_app_shell = w;
}

UI_CALLBACK(exposure_callback)
{
    Dimension width, height;

    suspend_speed_eval();
    XtVaGetValues(w, XtNwidth, (XtPointer) & width,
		  XtNheight, (XtPointer) & height, NULL);
    ((ui_exposure_handler_t) client_data)((unsigned int)width,
                                          (unsigned int)height);
}

/* FIXME: this does not handle multiple application shells. */
static void close_action(Widget w, XEvent * event, String * params,
                         Cardinal * num_params)
{
    suspend_speed_eval();
    if (w == xdebugger) {
	xdebug_disable();
	XtPopdown(xdebugger);
    } else {
	int i;

#if 0
	for (i = 0; i < num_app_shells; i++)
	    if (app_shells[i].shell == w)
		UiExit(w, NULL, NULL);
#endif
	last_visited_app_shell = NULL;
    }
}

