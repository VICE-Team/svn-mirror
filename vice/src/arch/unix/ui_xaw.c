/*
 * ui_xaw.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat    (fachat@physik.tu-chemnitz.de)
 *
 * Support for multiple visuals and depths by
 *  Teemu Rantanen (tvr@cs.hut.fi)
 * Joystick options by
 *  Bernhard Kuhn  (kuhn@eikon.e-technik.tu-muenchen.de)
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

#include "checkmark.xbm"
#include "right_arrow.xbm"

#include "ui_xaw.h"
#include "serial.h"
#include "interrupt.h"
#include "patchlevel.h"
#include "info.h"
#include "vsync.h"
#include "mem.h"
#include "sid.h"
#include "drive.h"
#include "tape.h"
#include "resources.h"
#include "mon.h"
#include "utils.h"

#ifdef AUTOSTART
#include "autostart.h"
#endif

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

#ifdef HAVE_TRUE1541
#include "true1541.h"
#endif

#ifdef REU
#include "reu.h"
#endif

extern void video_resize(void);

/* These are used by video_x.[ch]. */
Display *display;
int screen;
Visual *visual;
int depth = X_DISPLAY_DEPTH;

/* Application context. */
static XtAppContext app_context;

/* This is needed to catch the `Close' command from the Window Manager. */
static Atom WM_delete_window;

/* Useful images. */
static Pixmap CheckmarkBitmap, RightArrowBitmap;
#ifdef XPM
static Pixmap IconPixmap;
#endif

/* Toplevel widget. */
static Widget TopLevel = NULL;

/* XDebugger, Jarkko's X11-based 6502 debugger. */
static Widget XDebugger;

/* Our colormap. */
static Colormap colormap;

/* This keeps a list of the menus with a checkmark on the left.  Each time
   some setting is changed, we have to update them. */
#define MAX_UPDATE_MENU_LIST_SIZE 1024
static Widget CheckmarkMenuItems[MAX_UPDATE_MENU_LIST_SIZE];
static int NumCheckmarkMenuItems = 0;

/* This allows us to pop up the transient shells centered to the last visited
   shell. */
static Widget LastVisitedAppShell = NULL;
#define MAX_APP_SHELLS 10
static struct {
    String title;
    Widget shell;
    Widget canvas;
    Widget speed_label;
#ifdef HAVE_TRUE1541
    Widget drive_track_label;
    Widget drive_led;
#endif
} AppShells[MAX_APP_SHELLS];
static int NumAppShells = 0;

#ifdef HAVE_TRUE1541
/* Pixels for updating the drive LED's state.  */
Pixel drive_led_on_pixel, drive_led_off_pixel;
#endif

/* This is for our home-made menu implementation.  Not very clever, but it
   works. */
static int PoppedUpCnt = 0;
static int MenuPopup = 0;
#define MAX_SUBMENUS 1024
static struct Submenu {
    Widget widget;
    Widget parent;
    int level;
} Submenus[MAX_SUBMENUS];
static int NumSubmenus = 0;
static Widget LeftMenuWidget, RightMenuWidget;

/* If != 0, we should save the settings. */
static int resources_have_changed = 0;

/* ------------------------------------------------------------------------- */

static int UiAllocColormap(void);
static int UiAllocColors(int num_colors, const UiColorDef colors[],
			 PIXEL pixel_return[]);
static void UiPopup(Widget w, const char *title, Boolean wait_popdown);
static void UiPopdown(Widget w);
static Widget UiBuildFileSelector(Widget parent, UiButton *button_return);
static Widget UiBuildErrorDialog(Widget parent, UiButton *button_return,
				 const String message);
static Widget UiBuildInputDialog(Widget parent, UiButton *button_return,
				 Widget *InputDialogLabel,
				 Widget *InputDialogField);
static Widget UiBuildShowText(Widget parent, UiButton *button_return,
			      const String text, int width, int height);
static Widget UiBuildConfirmDialog(Widget parent, UiButton *button_return,
				   Widget *ConfirmDialogMessage);
static Widget UiCreateTransientShell(Widget parent, const char *name);
static Widget UiBuildInfoDialog(Widget parent, UiButton *button_return,...);
static void UiPositionSubmenu(Widget w, Widget parent);
static void UiCloseAction(Widget w, XEvent *event, String *params,
			  Cardinal *num_params);
static void UiPositionSubmenuAction(Widget w, XEvent *event, String *params,
				    Cardinal *num_params);
static void UiPopdownSubmenusAction(Widget w, XEvent *event, String *params,
				   Cardinal *num_params);
static void UiMenuUnhighlightAction(Widget w, XEvent *event, String *params,
				    Cardinal *num_params);
static void UiHandleSpecialKeys(Widget w, XtPointer closure, XEvent *x_event,
				Boolean *continue_to_dispatch);
#define CallbackFunc(name)  static void name(Widget w, XtPointer client_data, \
					     XtPointer call_data)

CallbackFunc(UiEnterWindowCallback);
CallbackFunc(UiExposureCallback);
CallbackFunc(UiMenuPopupCallback);
CallbackFunc(UiMenuPopdownCallback);
CallbackFunc(UiSubmenuPopupCallback);
CallbackFunc(UiSubmenuPopdownCallback);
CallbackFunc(UiAttachDisk);
CallbackFunc(UiDetachDisk);
CallbackFunc(UiChangeWorkingDir);
CallbackFunc(UiActivateXDebugger);
CallbackFunc(UiActivateMonitor);
CallbackFunc(UiRunC1541);
CallbackFunc(UiReset);
CallbackFunc(UiPowerUpReset);
CallbackFunc(UiBrowseManual);
CallbackFunc(UiExit);
CallbackFunc(UiInfo);
CallbackFunc(UiSetRefreshRate);
CallbackFunc(UiSetCustomRefreshRate);
CallbackFunc(UiSetMaximumSpeed);
CallbackFunc(UiSetCustomMaximumSpeed);
CallbackFunc(UiInfoDialogNoWarrantyCallback);
CallbackFunc(UiInfoDialogContribCallback);
CallbackFunc(UiInfoDialogLicenseCallback);
CallbackFunc(UiCloseButtonCallback);
CallbackFunc(UiOkButtonCallback);
CallbackFunc(UiCancelButtonCallback);
CallbackFunc(UiYesButtonCallback);
CallbackFunc(UiNoButtonCallback);
CallbackFunc(UiResetButtonCallback);
CallbackFunc(UiMonButtonCallback);
CallbackFunc(UiDebugButtonCallback);
CallbackFunc(UiContentsButtonCallback);
CallbackFunc(UiAutostartButtonCallback);
CallbackFunc(UiToggleVideoCache);
CallbackFunc(UiToggleDoubleSize);
CallbackFunc(UiToggleDoubleScan);
CallbackFunc(UiToggleUseXSync);
CallbackFunc(UiTogglePause);
CallbackFunc(UiSaveResources);
CallbackFunc(UiLoadResources);
CallbackFunc(UiSetDefaultResources);
CallbackFunc(UiToggleSaveResourcesOnExit);
CallbackFunc(UiToggleTurbo);

#if defined(CBM64) || defined(C128)
CallbackFunc(UiAttachTape);
CallbackFunc(UiDetachTape);
CallbackFunc(UiSmartAttach);
CallbackFunc(UiToggleSpriteToSpriteCollisions);
CallbackFunc(UiToggleSpriteToBackgroundCollisions);
CallbackFunc(UiToggleEmuID);
CallbackFunc(UiToggleIEEE488);
CallbackFunc(UiToggleREU);
#endif
#if defined(CBM64) || defined(C128) || defined(PET)
CallbackFunc(UiSwapJoystickPorts);
#ifdef HAS_JOYSTICK
CallbackFunc(UiSetJoystickDevice1);
CallbackFunc(UiSetJoystickDevice2);
#else
CallbackFunc(UiSetNumpadJoystickPort);
#endif
#endif

#ifdef HAVE_TRUE1541
CallbackFunc(UiToggleTrue1541);
CallbackFunc(UiToggleParallelCable);
CallbackFunc(UiSet1541SyncFactor);
CallbackFunc(UiSet1541IdleMethod);
CallbackFunc(UiSetCustom1541SyncFactor);
#endif

#ifdef SOUND
CallbackFunc(UiToggleSound);
CallbackFunc(UiToggleSoundSpeedAdjustment);
CallbackFunc(UiSetSoundSampleRate);
CallbackFunc(UiSetSoundBufferSize);
CallbackFunc(UiSetSoundSuspendTime);
#if defined(CBM64) || defined(C128)
CallbackFunc(UiToggleSidFilters);
CallbackFunc(UiSetSidModel);
#endif
#endif /* SOUND */

#ifdef PET
CallbackFunc(UiToggleNumpadJoystick);
CallbackFunc(UiTogglePetDiag);
#endif

/* This one needs the menu functions to be prototyped first, and that's why we
   include it now. */
#include "menu.h"

/* This one needs the MenuEntry typedef from menu.h */
static Widget UiBuildPopupMenu(Widget parent, const char *name,
			       MenuEntry list[]);

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
    "*driveTrack.font:                          -*-helvetica-medium-r-*-*-10-*",
    "*speedStatus.font:                         -*-helvetica-medium-r-*-*-10-*",

    XDEBUG_FALLBACK_RESOURCES,
    NULL
};

/* ------------------------------------------------------------------------- */

/* Initialize the GUI and parse the command line. */
int UiInit(int *argc, char **argv)
{
    /* Create the toplevel. */
    TopLevel = XtAppInitialize(&app_context, "VICE", NULL, 0, argc, argv,
			       fallback_resources, NULL, 0);
    if (!TopLevel)
	return -1;
    display = XtDisplay(TopLevel);
    screen = XDefaultScreen(display);
    atexit(UiAutoRepeatOn);
    return 0;
}

typedef struct {
    char *name;
    int class;
} namedvisual_t;

/* Continue GUI initialization after resources are set. */
int UiInitFinish(void)
{
    static XtActionsRec actions[] = {
	{ "Close", UiCloseAction },
	{ "PositionSubmenu", UiPositionSubmenuAction },
	{ "PopdownSubmenus", UiPopdownSubmenusAction },
	{ "Unhighlight", UiMenuUnhighlightAction }
    };
    static namedvisual_t classes[] = {
	{ "PseudoColor", PseudoColor },
	{ "TrueColor", TrueColor },
	{ "StaticGray", StaticGray },
	{ NULL }
    };
    XVisualInfo visualinfo;

#if X_DISPLAY_DEPTH == 0
    depth = app_resources.displayDepth;
#endif

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
    UiAllocColormap();

    /* Recreate TopLevel to support non-default display depths.  We also copy
       the WM_COMMAND' property for a minimal session-management support
       (WindowMaker loves that).
       FIXME: correct way to do so?  This looks like a big dirty kludge... I
       cannot believe there is not a cleaner method.  */
    {
	Atom wm_command_atom = XInternAtom(display, "WM_COMMAND", False);
	Atom wm_command_type;
	int wm_command_format;
	unsigned long wm_command_nitems;
	unsigned char *wm_command_data;
	int wm_command_present = 0;

	/* Realize the old toplevel.  This ugliness is required to create the
           window we retrieve the `WM_COMMAND' property from. */
	XtVaSetValues(TopLevel,
		      XtNwidth, 1,
		      XtNheight, 1,
		      XtNmappedWhenManaged, False,
		      NULL);
	XtRealizeWidget(TopLevel);

	/* Retrieve the `WM_COMMAND' property. */
	if (wm_command_atom != None) {
	    unsigned long dummy;

	    if (Success == XGetWindowProperty(display,
					      XtWindow(TopLevel),
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
	XtDestroyWidget(TopLevel);

	/* Create the new TopLevel. */
	TopLevel = XtVaAppCreateShell(EMULATOR, "VICE",
				      applicationShellWidgetClass, display,
				      XtNvisual, visual,
				      XtNdepth, depth,
				      XtNcolormap, colormap,
				      XtNmappedWhenManaged, False,
				      XtNwidth, 1,
				      XtNheight, 1,
				      NULL);
	XtRealizeWidget(TopLevel);

	/* Set the `WM_COMMAND' property in the new TopLevel. */
	if (wm_command_present) {
	    XChangeProperty(display, XtWindow(TopLevel), wm_command_atom,
			    wm_command_type, wm_command_format, PropModeReplace,
			    (char *)wm_command_data, wm_command_nitems);
	    XtFree(wm_command_data);
	}
    }

    /* Create the `tick' bitmap. */
    CheckmarkBitmap = XCreateBitmapFromData(display,
					    DefaultRootWindow(display),
					    checkmark_bits,
					    checkmark_width,
					    checkmark_height);
    /* Create the right arrow bitmap. */
    RightArrowBitmap = XCreateBitmapFromData(display,
					     DefaultRootWindow(display),
					     right_arrow_bits,
					     right_arrow_width,
					     right_arrow_height);

#ifdef XPM
    /* Create the icon pixmap. */
    XpmCreatePixmapFromData(display, DefaultRootWindow(display), icon_data,
			    &IconPixmap, NULL, NULL);
#endif

    WM_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    XtAppAddActions(app_context, actions, XtNumber(actions));

    return 0;
}

/* Create a shell with a canvas widget in it. */
UiWindow UiOpenCanvasWindow(const char *title, int width, int height,
			    int no_autorepeat, UiExposureHandler exposure_proc,
			    int num_colors, const UiColorDef colors[],
			    PIXEL pixel_return[])
{
    /* Note: this is correct because we never destroy CanvasWindows. */
    static int menus_created = 0;
    XtTranslations translations;
    Widget shell, canvas, pane, speed_label;
#ifdef HAVE_TRUE1541
    Widget drive_track_label, drive_led;
#endif
    XSetWindowAttributes attr;

    if (UiAllocColors(num_colors, colors, pixel_return) == -1)
	return NULL;

    /* colormap might have changed after UiAllocColors, so we set it again */
    XtVaSetValues(TopLevel, XtNcolormap, colormap, NULL);

    /* Create the pop-up menus. */
    if (!menus_created) {
	LeftMenuWidget = UiBuildPopupMenu(TopLevel, "LeftMenu", LeftMenu);
	RightMenuWidget = UiBuildPopupMenu(TopLevel, "RightMenu", RightMenu);
	UiUpdateMenus();
	menus_created = 1;
    }

    if (++NumAppShells > MAX_APP_SHELLS) {
	fprintf(stderr,
	  	"UiOpenCanvasWindow(): maximum number of open windows reached."
		"\n");
	return NULL;
    }

    shell = XtVaCreatePopupShell(title, applicationShellWidgetClass,
                                 TopLevel, XtNinput, True, XtNtitle, title,
                                 XtNiconName, title, NULL);

#ifdef XPM
    XtVaSetValues(shell, XtNiconPixmap, IconPixmap, NULL);
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
		      (XtEventHandler) UiEnterWindowCallback, NULL);
    XtAddEventHandler(canvas, ExposureMask | StructureNotifyMask, False,
		      (XtEventHandler) UiExposureCallback, exposure_proc);

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


#ifdef HAVE_TRUE1541
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
#endif
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
			  (XtEventHandler) UiAutoRepeatOff, NULL);
	XtAddEventHandler(canvas, LeaveWindowMask, False,
			  (XtEventHandler) UiAutoRepeatOn, NULL);
	XtAddEventHandler(shell, KeyPressMask, False,
			  (XtEventHandler) UiHandleSpecialKeys, NULL);
	XtAddEventHandler(canvas, KeyPressMask, False,
			  (XtEventHandler) UiHandleSpecialKeys, NULL);
    }

    XtRealizeWidget(shell);
    XtPopup(shell, XtGrabNone);

    attr.backing_store = Always;
    XChangeWindowAttributes(display, XtWindow(canvas),
    	 		    CWBackingStore, &attr);

    XSetWMProtocols(display, XtWindow(shell), &WM_delete_window, 1);
    XtOverrideTranslations(shell, XtParseTranslationTable
                                      ("<Message>WM_PROTOCOLS: Close()"));

    AppShells[NumAppShells - 1].shell = shell;
    AppShells[NumAppShells - 1].canvas = canvas;
    AppShells[NumAppShells - 1].title = stralloc(title);
    AppShells[NumAppShells - 1].speed_label = speed_label;

#ifdef HAVE_TRUE1541
    AppShells[NumAppShells - 1].drive_track_label = drive_track_label;
    AppShells[NumAppShells - 1].drive_led = drive_led;
#endif

    return canvas;
}

/* Set the colormap variable. The user must tell us whether he wants the
   default one or not using the `privateColormap' resource. */
static int UiAllocColormap(void)
{
    if (colormap)
	return 0;

    if (!app_resources.privateColormap
	&& depth == DefaultDepth(display, screen)) {
	colormap = DefaultColormap(display, screen);
    } else {
	colormap = XCreateColormap(display, RootWindow(display, screen),
				   visual, AllocNone);
    }

    XtVaSetValues(TopLevel, XtNcolormap, colormap, NULL);
    return 0;
}

/* Allocate colors in the colormap. */
static int UiDoAllocColors(int num_colors, const UiColorDef color_defs[],
			 PIXEL pixel_return[], int releasefl)
{
    static int allocated_colors;
    int i, failed;
    XColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);
    unsigned long *xpixels = xmalloc(sizeof(unsigned long)*num_colors);

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, visual, depth, ZPixmap, 0, (char *)data,
		      1, 1, 8, 0);
    if (!im)
	return -1;

    color.flags = DoRed | DoGreen | DoBlue;
    for (i = 0, failed = 0; i < num_colors; allocated_colors++, i++) {
	color.red = color_defs[i].red;
	color.green = color_defs[i].green;
	color.blue = color_defs[i].blue;
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
		shade_table[i] = color_defs[i].dither;
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

#ifdef HAVE_TRUE1541
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
#endif

    return failed;
}

/*
 * In here we try to allocate the given colors. This function is called from
 * UiOpenCanvasWindow. The calling function sets the colormap resource of
 * the toplevel window.
 * If there is not enough place in the colormap for all color entries,
 * we allocate a new one.
 * If we someday open two canvas windows, and the colormap fills up during
 * the second one, we might run into trouble, although I am not sure.
 * (setting the Toplevel colormap will not change the colormap of already
 * opened children)
 *
 * 20jan1998 A.Fachat
 */
static int UiAllocColors(int num_colors, const UiColorDef color_defs[],
			 PIXEL pixel_return[])
{
    int failed;

    failed = UiDoAllocColors(num_colors, color_defs, pixel_return, 1);
    if (failed) {
	/* printf("\nHint: use the `-install' option to install a private colormap.\n"); */
	/* also check if we are allowed to automagically alloc? */
	if (colormap == DefaultColormap(display, screen)) {
            printf("Automagically using a private colormap.\n");
	    /* printf("If you encounter problems, please use `-install'.\n"); */
	    colormap = XCreateColormap(display, RootWindow(display, screen),
				       visual, AllocNone);
	    XtVaSetValues(TopLevel, XtNcolormap, colormap, NULL);
	    failed = UiDoAllocColors(num_colors, color_defs, pixel_return, 0);
	}
    }
    return failed ? -1 : 0;
}

/* Return the drawable for the canvas in the specified UiWindow. */
Window UiCanvasDrawable(UiWindow w)
{
    return XtWindow(w);
}

/* Show the speed index to the user.  */
void UiDisplaySpeed(float percent, float framerate)
{
    int i;
    char str[256];
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);

    for (i = 0; i < NumAppShells; i++) {
	if (!percent) {
	    XtVaSetValues(AppShells[i].speed_label, XtNlabel, "",
			  NULL);
	} else {
	    sprintf(str, "%d%%, %d fps", percent_int, framerate_int);
	    XtVaSetValues(AppShells[i].speed_label, XtNlabel, str, NULL);
	}
    }
}

#ifdef HAVE_TRUE1541
void UiToggleDriveStatus(int state)
{
    int i;

    for (i = 0; i < NumAppShells; i++) {
        if (state) {
            XtRealizeWidget(AppShells[i].drive_track_label);
            XtManageChild(AppShells[i].drive_track_label);
            XtRealizeWidget(AppShells[i].drive_led);
            XtManageChild(AppShells[i].drive_led);
        } else{
            XtUnrealizeWidget(AppShells[i].drive_track_label);
            XtUnrealizeWidget(AppShells[i].drive_led);
        }
    }
}

void UiDisplayDriveTrack(double track_number)
{
    int i;
    char str[256];

    sprintf(str, "Track %.1f", (double)track_number);
    for (i = 0; i < NumAppShells; i++) {
	Widget w = AppShells[i].drive_track_label;

	if (!XtIsRealized(w)) {
	    XtRealizeWidget(w);
	    XtManageChild(w);
	}
        XtVaSetValues(w, XtNlabel, str, NULL);
    }
}

void UiDisplayDriveLed(int status)
{
    Pixel pixel = status ? drive_led_on_pixel : drive_led_off_pixel;
    int i;

    for (i = 0; i < NumAppShells; i++) {
	Widget w = AppShells[i].drive_led;

	if (!XtIsRealized(w)) {
	    XtRealizeWidget(w);
	    XtManageChild(w);
	}
        XtVaSetValues(w, XtNbackground, pixel, NULL);
    }
}
#endif

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void UiDisplayPaused(void)
{
    int i;
    char str[1024];

    for (i = 0; i < NumAppShells; i++) {
	sprintf(str, "%s (paused)", AppShells[i].title);
	XtVaSetValues(AppShells[i].shell, XtNtitle, str, NULL);
    }
}

/* Dispatch the next Xt event.  If not pending, wait for it. */
static void UiDispatchNextEvent(void)
{
    XEvent report;

    XtAppNextEvent(app_context, &report);
    XtDispatchEvent(&report);
}

/* Dispatch all the pending Xt events. */
void UiDispatchEvents(void)
{
    while (XtAppPending(app_context) || MenuPopup)
	UiDispatchNextEvent();
}

/* Resize one window. */
void UiResizeCanvasWindow(UiWindow w, int width, int height)
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
void UiMapCanvasWindow(UiWindow w)
{
    XtPopup(w, XtGrabNone);
}

/* Unmap one window. */
void UiUnmapCanvasWindow(UiWindow w)
{
    XtPopdown(w);
}

/* Enable autorepeat. */
void UiAutoRepeatOn(void)
{
    XAutoRepeatOn(display);
    XFlush(display);
}

/* Disable autorepeat. */
void UiAutoRepeatOff(void)
{
    XAutoRepeatOff(display);
    XFlush(display);
}

/* ------------------------------------------------------------------------- */

/* Handle all special keys. */
static void UiHandleSpecialKeys(Widget w, XtPointer closure,
				XEvent *x_event, Boolean *continue_to_dispatch)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;

    XLookupString(&x_event->xkey, buffer, 20, &key, &compose);

    switch (key) {
    case XK_F9:
	UiToggleTurbo(NULL, NULL, NULL);
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
      default:
	break;
    }
}

/* ------------------------------------------------------------------------- */

/* Report an error to the user. */
void UiError(const char *format,...)
{
    char str[1024];
    va_list ap;
    static Widget ErrorDialog;
    static UiButton button;

    va_start(ap, format);
    vsprintf(str, format, ap);
    ErrorDialog = UiBuildErrorDialog(TopLevel, &button, str);
    UiPopup(XtParent(ErrorDialog), "VICE Error!", False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    UiPopdown(XtParent(ErrorDialog));
    XtDestroyWidget(XtParent(ErrorDialog));
    UiDispatchEvents();
    suspend_speed_eval();
}

/* Report a message to the user. */
void UiMessage(const char *format,...)
{
    char str[1024];
    va_list ap;
    static Widget ErrorDialog;
    static UiButton button;

    va_start(ap, format);
    vsprintf(str, format, ap);
    ErrorDialog = UiBuildErrorDialog(TopLevel, &button, str);
    UiPopup(XtParent(ErrorDialog), "VICE", False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    UiPopdown(XtParent(ErrorDialog));
    XtDestroyWidget(XtParent(ErrorDialog));
    UiDispatchEvents();
    suspend_speed_eval();
}

void activate_xdebug_window(void)
{
    if (!XDebugger) {
	XDebugger = XtVaCreatePopupShell("XDebugger",
					 applicationShellWidgetClass, TopLevel,
					 NULL);
	XtOverrideTranslations(XDebugger,
			       (XtParseTranslationTable
				("<Message>WM_PROTOCOLS: Close()")));
	xdebug_create(XDebugger);
    }
    XtPopup(XDebugger, XtGrabNone);
    XSetWMProtocols(display, XtWindow(XDebugger), &WM_delete_window, 1);
    xdebug_enable();
}

/* Report a message to the user, allow different buttons. */
int UiJamDialog(const char *format, ...)
{
    char str[1024];
    va_list ap;
    static Widget JamDialog, shell, tmp, mform, bbox;
    static UiButton button;

    va_start(ap, format);

    shell = UiCreateTransientShell(TopLevel, "jamDialogShell");
    JamDialog = XtVaCreateManagedWidget
	("jamDialog", panedWidgetClass, shell, NULL);
    mform = XtVaCreateManagedWidget
	("messageForm", formWidgetClass, JamDialog, NULL);

    vsprintf(str, format, ap);
    tmp = XtVaCreateManagedWidget
	("label", labelWidgetClass, mform,
	 XtNresize, False, XtNjustify, XtJustifyCenter, XtNlabel, str,
	 NULL);

    bbox = XtVaCreateManagedWidget
	("buttonBox", boxWidgetClass, JamDialog,
	 XtNshowGrip, False, XtNskipAdjust, XtNorientation,
	 XtorientHorizontal, NULL);

    tmp = XtVaCreateManagedWidget
	("resetButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UiResetButtonCallback,
		  (XtPointer) &button);

    tmp = XtVaCreateManagedWidget
	("monButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UiMonButtonCallback,
		  (XtPointer) &button);

    tmp = XtVaCreateManagedWidget
	("debugButton", commandWidgetClass, bbox, NULL);
    XtAddCallback(tmp, XtNcallback, UiDebugButtonCallback,
		  (XtPointer) &button);

    UiPopup(XtParent(JamDialog), "VICE", False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    UiPopdown(XtParent(JamDialog));
    XtDestroyWidget(XtParent(JamDialog));

    if (button == Button_Debug) {
	activate_xdebug_window();
	XSync(display, False);
	UiDispatchEvents();
    }

    suspend_speed_eval();
    UiDispatchEvents();

    if (button==Button_Mon)
	return 1;
    if (button==Button_Debug)
	return 2;
    return 0;
}

/* File browser. */
char *UiFileSelect(const char *title,
		   char *(*read_contents_func)(const char *),
		   int allow_autostart,
		   UiButton *button_return)
{
    static UiButton button;
    static char *ret = NULL;
    static Widget FileSelector = NULL;
    XfwfFileSelectorStatusStruct fs_status;
    char *curdir;

#ifndef __alpha
    /* We always rebuild the file selector from scratch (which is slow),
       because there seems to be a bug in the XfwfScrolledList that causes
       the file and directory listing to disappear randomly.  I hope this
       fixes the problem...  */
    FileSelector = UiBuildFileSelector(TopLevel, &button);
#else
    /* Unluckily, this does not work on Alpha (segfault when the widget is
       popped down).  There is probably something wrong in some widget, but
       we have no time to check this...  FIXME: Then Alpha users could get
       the "disappearing list" bug.  Grpmf.  */
    if (FileSelector == NULL)
	FileSelector = UiBuildFileSelector(TopLevel, &button);
#endif

#ifdef AUTOSTART
    XtVaSetValues(FileSelector, XtNshowAutostartButton, allow_autostart, NULL);
#else
    XtVaSetValues(FileSelector, XtNshowAutostartButton, False, NULL);
#endif

    curdir = get_current_dir();
    XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) FileSelector,
				    curdir);
    free(curdir);

    UiPopup(XtParent(FileSelector), title, False);
    do {
	button = Button_None;
	while (button == Button_None)
	    UiDispatchNextEvent();
	XfwfFileSelectorGetStatus((XfwfFileSelectorWidget)FileSelector,
				  &fs_status);
	if (fs_status.file_selected
	    && button == Button_Contents
	    && read_contents_func != NULL) {
	    char *contents;
	    char *f = concat(fs_status.path, fs_status.file, NULL);

	    contents = read_contents_func(f);
	    free(f);
	    if (contents != NULL) {
		UiShowText(fs_status.file, contents, 250, 240);
		free(contents);
	    } else {
		UiError("Unknown image type.");
	    }
	}
    } while ((!fs_status.file_selected && button != Button_Cancel)
	     || button == Button_Contents);

    if (ret != NULL)
	free(ret);

    if (fs_status.file_selected)
	ret = concat(fs_status.path, fs_status.file, NULL);
    else
	ret = stralloc("");

    UiPopdown(XtParent(FileSelector));

#ifndef __alpha
    /* On Alpha, XtDestroyWidget segfaults, don't know why...  */
    XtDestroyWidget(XtParent(FileSelector));
#endif

    *button_return = button;
    if (button == Button_Ok || button == Button_Autostart)
	return ret;
    else
	return NULL;
}

/* Ask for a string.  The user can confirm or cancel. */
UiButton UiInputString(const char *title, const char *prompt, char *buf,
		       unsigned int buflen)
{
    String str;
    static Widget InputDialog, InputDialogLabel, InputDialogField;
    static UiButton button;

    if (!InputDialog)
	InputDialog = UiBuildInputDialog(TopLevel, &button, &InputDialogLabel,
					 &InputDialogField);
    XtVaSetValues(InputDialogLabel, XtNlabel, prompt, NULL);
    XtVaSetValues(InputDialogField, XtNstring, buf, NULL);
    XtSetKeyboardFocus(InputDialog, InputDialogField);
    UiPopup(XtParent(InputDialog), title, False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    XtVaGetValues(InputDialogField, XtNstring, &str, NULL);
    strncpy(buf, str, buflen);
    UiPopdown(XtParent(InputDialog));
    return button;
}

/* Display a text to the user. */
void UiShowText(const char *title, const char *text, int width, int height)
{
    static UiButton button;
    Widget ShowText;

    ShowText = UiBuildShowText(TopLevel, &button, (String)text, width, height);
    UiPopup(XtParent(ShowText), title, False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    UiPopdown(XtParent(ShowText));
    XtDestroyWidget(XtParent(ShowText));
}

/* Ask for a confirmation. */
UiButton UiAskConfirmation(const char *title, const char *text)
{
    static Widget ConfirmDialog, ConfirmDialogMessage;
    static UiButton button;

    if (!ConfirmDialog)
	ConfirmDialog = UiBuildConfirmDialog(TopLevel, &button,
					     &ConfirmDialogMessage);
    XtVaSetValues(ConfirmDialogMessage, XtNlabel, text, NULL);
    UiPopup(XtParent(ConfirmDialog), title, False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    UiPopdown(XtParent(ConfirmDialog));
    return button;
}

/* Update the menu items with a checkmark according to the current resource
   values. */
void UiUpdateMenus(void)
{
    int i;

    for (i = 0; i < NumCheckmarkMenuItems; i++)
	XtCallCallbacks(CheckmarkMenuItems[i], XtNcallback, (XtPointer) ! NULL);
}

/* ------------------------------------------------------------------------- */

/* Pop up a popup shell and center it to the last visited AppShell */
static void UiPopup(Widget w, const char *title, Boolean wait_popdown)
{
    Widget s = NULL;

    /* Keep sure that we really know which was the last visited shell. */
    UiDispatchEvents();

    if (LastVisitedAppShell)
	s = LastVisitedAppShell;
    else {
	/* Choose one realized shell. */
	int i;
	for (i = 0; i < NumAppShells; i++)
	    if (XtIsRealized(AppShells[i].shell)) {
		s = AppShells[i].shell;
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
    XSetWMProtocols(display, XtWindow(w), &WM_delete_window, 1);

    /* If requested, wait for this widget to be popped down before
       returning. */
    if (wait_popdown) {
	int oldcnt = PoppedUpCnt++;
	while (oldcnt != PoppedUpCnt)
	    UiDispatchNextEvent();
    } else
	PoppedUpCnt++;
}

/* Pop down a popup shell. */
static void UiPopdown(Widget w)
{
    XtPopdown(w);
    if (--PoppedUpCnt < 0)
	PoppedUpCnt = 0;
}

/* ------------------------------------------------------------------------- */

/* These functions build all the widgets. */

static Widget UiBuildFileSelector(Widget parent, UiButton * button_return)
{
    Widget shell = UiCreateTransientShell(parent, "fileSelectorShell");
    Widget FileSelector = XtVaCreateManagedWidget("fileSelector",
						  xfwfFileSelectorWidgetClass,
						  shell,
						  XtNflagLinks, True, NULL);

    XtAddCallback((Widget) FileSelector, XtNokButtonCallback,
		  UiOkButtonCallback, (XtPointer) button_return);
    XtAddCallback((Widget) FileSelector,
		  XtNcancelButtonCallback, UiCancelButtonCallback,
		  (XtPointer) button_return);
    XtAddCallback((Widget) FileSelector,
		  XtNcontentsButtonCallback, UiContentsButtonCallback,
		  (XtPointer) button_return);
    XtAddCallback((Widget) FileSelector,
		  XtNautostartButtonCallback, UiAutostartButtonCallback,
		  (XtPointer) button_return);
    return FileSelector;
}

static Widget UiBuildErrorDialog(Widget parent, UiButton * button_return,
				 const String message)
{
    Widget shell, ErrorDialog, tmp;

    shell = UiCreateTransientShell(parent, "errorDialogShell");
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
    XtAddCallback(tmp, XtNcallback, UiCloseButtonCallback,
		  (XtPointer) button_return);
    return ErrorDialog;
}

static Widget UiBuildInputDialog(Widget parent, UiButton * button_return,
				 Widget * InputDialogLabel,
				 Widget * InputDialogField)
{
    Widget shell, InputDialog, tmp1, tmp2;

    shell = UiCreateTransientShell(parent, "inputDialogShell");
    InputDialog = XtVaCreateManagedWidget
	("inputDialog", panedWidgetClass, shell, NULL);
    tmp1 = XtVaCreateManagedWidget
	("inputForm", formWidgetClass, InputDialog, NULL);
    *InputDialogLabel = XtVaCreateManagedWidget
	("label", labelWidgetClass, tmp1, XtNresize, False, XtNjustify,
	 XtJustifyLeft, NULL);
    *InputDialogField = XtVaCreateManagedWidget
	("field", textfieldWidgetClass, tmp1, XtNfromVert, *InputDialogLabel,
	 NULL);
    XtAddCallback(*InputDialogField, XtNactivateCallback, UiOkButtonCallback,
		  (XtPointer) button_return);
    tmp1 = XtVaCreateManagedWidget
	("buttonBox", boxWidgetClass, InputDialog,
	 XtNshowGrip, False, XtNskipAdjust, True,
	 XtNorientation, XtorientHorizontal, NULL);
    tmp2 = XtVaCreateManagedWidget
	("okButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2,
		  XtNcallback, UiOkButtonCallback, (XtPointer) button_return);
    tmp2 = XtVaCreateManagedWidget
	("cancelButton", commandWidgetClass, tmp1, XtNfromHoriz, tmp2, NULL);
    XtAddCallback(tmp2,
		  XtNcallback, UiCancelButtonCallback, (XtPointer) button_return);
    return InputDialog;
}

static Widget UiBuildShowText(Widget parent, UiButton * button_return,
			      const String text, int width, int height)
{
    Widget shell, tmp;
    Widget ShowText;

    shell = UiCreateTransientShell(parent, "showTextShell");
    ShowText = XtVaCreateManagedWidget
	("showText", panedWidgetClass, shell, NULL);
    tmp = XtVaCreateManagedWidget
	("textBox", formWidgetClass, ShowText, NULL);
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
	("buttonBox", boxWidgetClass, ShowText,
	 XtNshowGrip, False, XtNskipAdjust, True,
	 XtNorientation, XtorientHorizontal, NULL);
    tmp = XtVaCreateManagedWidget("closeButton", commandWidgetClass, tmp, NULL);
    XtAddCallback(tmp,
		  XtNcallback, UiCloseButtonCallback,
		  (XtPointer) button_return);
    return ShowText;
}

static Widget UiBuildConfirmDialog(Widget parent, UiButton *button_return,
				   Widget *ConfirmDialogMessage)
{
    Widget shell, ConfirmDialog, tmp1, tmp2;

    shell = UiCreateTransientShell(parent, "confirmDialogShell");
    ConfirmDialog = XtVaCreateManagedWidget
	("confirmDialog", panedWidgetClass, shell, NULL);
    tmp1 = XtVaCreateManagedWidget("messageForm", formWidgetClass,
				   ConfirmDialog, NULL);
    *ConfirmDialogMessage = XtVaCreateManagedWidget
	("message", labelWidgetClass, tmp1, XtNresize, False,
	 XtNjustify, XtJustifyCenter, NULL);
    tmp1 = XtVaCreateManagedWidget
	("buttonBox", boxWidgetClass, ConfirmDialog,
	 XtNshowGrip, False, XtNskipAdjust, True,
	 XtNorientation, XtorientHorizontal, NULL);
    tmp2 = XtVaCreateManagedWidget
	("yesButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2,
		  XtNcallback, UiYesButtonCallback, (XtPointer) button_return);
    tmp2 = XtVaCreateManagedWidget
	("noButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2,
		  XtNcallback, UiNoButtonCallback, (XtPointer) button_return);
    tmp2 = XtVaCreateManagedWidget
	("cancelButton", commandWidgetClass, tmp1, NULL);
    XtAddCallback(tmp2,
		  XtNcallback, UiCancelButtonCallback, (XtPointer) button_return);
    return ConfirmDialog;
}

static Widget UiCreateTransientShell(Widget parent, const char *name)
{
    Widget w;

    w = XtVaCreatePopupShell
	(name, transientShellWidgetClass, parent, XtNinput, True, NULL);
    XtOverrideTranslations(w, XtParseTranslationTable
			   ("<Message>WM_PROTOCOLS: Close()"));
    return w;
}

static Widget UiBuildPopupMenu(Widget parent, const char *name,
			       MenuEntry list[])
{
    Widget w;
    unsigned int i, j;
    static int level = 0;

    level++;
    w = XtCreatePopupShell(name, simpleMenuWidgetClass, parent, NULL, 0);
    if (level == 1) {
	XtAddCallback(w, XtNpopupCallback, UiMenuPopupCallback, NULL);
	XtAddCallback(w, XtNpopdownCallback, UiMenuPopdownCallback, NULL);
    }
    XtOverrideTranslations
	(w, XtParseTranslationTable
	 ("<BtnMotion>: highlight() PositionSubmenu()\n"
	  "@Num_Lock<BtnMotion>: highlight() PositionSubmenu()\n"
	  "<LeaveWindow>: Unhighlight()\n"
	  "<BtnUp>: PopdownSubmenus() MenuPopdown() notify() unhighlight()"));

    for (i = j = 0; list[i].string; i++) {
	Widget new_item;
	char name[256];

	sprintf(name, "MenuItem%d", j);	/* ugly... */
	switch (*list[i].string) {
	  case '-':		/* line */
	    new_item = XtCreateManagedWidget("separator", smeLineObjectClass,
					     w, NULL, 0);
	    break;
	  case '*':		/* toggle */
	    new_item = XtVaCreateManagedWidget(name, smeBSBObjectClass, w,
					       XtNrightMargin, 20,
					       XtNleftMargin, 20,
					       XtNlabel, list[i].string + 1,
					       NULL);
	    /* Add this item to the list of calls to perform to update the
	       menu status. */
	    if (list[i].callback) {
		if (NumCheckmarkMenuItems < MAX_UPDATE_MENU_LIST_SIZE)
		    CheckmarkMenuItems[NumCheckmarkMenuItems++] = new_item;
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

	    if (NumSubmenus > MAX_SUBMENUS) {
		fprintf(stderr,
			"Maximum number of sub menus reached! "
			"Please fix the code.\n");
		exit(-1);
	    }
	    XtVaSetValues(new_item, XtNrightBitmap, RightArrowBitmap, NULL);
	    sub = UiBuildPopupMenu(parent, "SUB", list[i].sub_menu);
	    Submenus[NumSubmenus].widget = sub;
	    Submenus[NumSubmenus].parent = new_item;
	    Submenus[NumSubmenus].level = level;
	    XtAddCallback(sub, XtNpopupCallback, UiSubmenuPopupCallback,
			  Submenus + NumSubmenus);
	    XtAddCallback(sub, XtNpopdownCallback, UiSubmenuPopdownCallback,
			  (XtPointer) w);
	    NumSubmenus++;
	}
    }

    level--;
    return w;
}

static Widget UiBuildInfoDialog(Widget parent, UiButton * button_return,...)
{
    Widget shell, pane, info_form, button_form, tmp, prevlabel = NULL;
    va_list arglist;
    String str;

    shell = UiCreateTransientShell(parent, "infoDialogShell");
    pane = XtVaCreateManagedWidget
	("infoDialog", panedWidgetClass, shell, NULL);
    info_form = XtVaCreateManagedWidget
	("textForm", formWidgetClass, pane, NULL);
    button_form = XtVaCreateManagedWidget
	("buttonBox", boxWidgetClass, pane, XtNshowGrip, False,
	 XtNskipAdjust, True, XtNorientation, XtorientHorizontal, NULL);
    va_start(arglist, button_return);
    while ((str = va_arg(arglist, String))) {
	tmp = XtVaCreateManagedWidget
	    ("infoString", labelWidgetClass, info_form,
	     XtNlabel, str, XtNjustify, XtJustifyCenter, XtNresize, False,
	     XtNwidth, 220, NULL);
	if (prevlabel)
	    XtVaSetValues(tmp, XtNfromVert, prevlabel, NULL);
	prevlabel = tmp;
    }
    tmp = XtVaCreateManagedWidget
	("closeButton", commandWidgetClass, button_form, NULL);
    XtAddCallback(tmp, XtNcallback,
		  UiCloseButtonCallback, (XtPointer)button_return);
    tmp = XtVaCreateManagedWidget
	("licenseButton", commandWidgetClass, button_form,
	 XtNfromHoriz, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, UiInfoDialogLicenseCallback, NULL);
    tmp = XtVaCreateManagedWidget
	("noWarrantyButton", commandWidgetClass, button_form,
	 XtNfromHoriz, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, UiInfoDialogNoWarrantyCallback, NULL);
    tmp = XtVaCreateManagedWidget
	("contribButton", commandWidgetClass, button_form,
	 XtNfromHoriz, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, UiInfoDialogContribCallback, NULL);
    return pane;
}

CallbackFunc(UiInfoDialogLicenseCallback)
{
    UiShowText("VICE is FREE software!", license_text, -1, -1);
}

CallbackFunc(UiInfoDialogNoWarrantyCallback)
{
    UiShowText("No warranty!", warranty_text, -1, -1);
}

CallbackFunc(UiInfoDialogContribCallback)
{
    UiShowText("Contributors to the VICE project", contrib_text, -1, -1);
}

CallbackFunc(UiOkButtonCallback)
{
    *((UiButton *)client_data) = Button_Ok;
}

CallbackFunc(UiCancelButtonCallback)
{
    *((UiButton *)client_data) = Button_Cancel;
}

CallbackFunc(UiYesButtonCallback)
{
    *((UiButton *)client_data) = Button_Yes;
}

CallbackFunc(UiNoButtonCallback)
{
    *((UiButton *)client_data) = Button_No;
}

CallbackFunc(UiCloseButtonCallback)
{
    *((UiButton *)client_data) = Button_Close;
}

CallbackFunc(UiMonButtonCallback)
{
    *((UiButton *)client_data) = Button_Mon;
}

CallbackFunc(UiDebugButtonCallback)
{
    *((UiButton *)client_data) = Button_Debug;
}

CallbackFunc(UiResetButtonCallback)
{
    *((UiButton *)client_data) = Button_Reset;
}

CallbackFunc(UiContentsButtonCallback)
{
    *((UiButton *)client_data) = Button_Contents;
}

CallbackFunc(UiAutostartButtonCallback)
{
    *((UiButton *)client_data) = Button_Autostart;
}

CallbackFunc(UiEnterWindowCallback)
{
    LastVisitedAppShell = w;
}

CallbackFunc(UiExposureCallback)
{
    Dimension width, height;

    suspend_speed_eval();
    XtVaGetValues(w, XtNwidth, (XtPointer) & width,
		  XtNheight, (XtPointer) & height, NULL);
    ((UiExposureHandler) client_data)((unsigned int)width,
				      (unsigned int)height);
}

CallbackFunc(UiMenuPopupCallback)
{
    MenuPopup++;
    suspend_speed_eval();
}

CallbackFunc(UiMenuPopdownCallback)
{
    if (MenuPopup > 0)
	MenuPopup--;
}

/* FIXME: this does not handle multiple application shells. */
static void UiCloseAction(Widget w, XEvent * event, String * params,
			  Cardinal * num_params)
{
    suspend_speed_eval();
    if (w == XDebugger) {
	xdebug_disable();
	XtPopdown(XDebugger);
    } else {
	int i;
	for (i = 0; i < NumAppShells; i++)
	    if (AppShells[i].shell == w)
		UiExit(w, NULL, NULL);
	LastVisitedAppShell = NULL;
    }
}

/* ------------------------------------------------------------------------- */

/* Here is a quick & dirty implementation of cascade submenus.  This is
   ugly, mean and smells badly.  It should be rewritten.  Replacements are
   welcome.  */

static Widget active_submenu, active_entry;
static int SubmenuPoppedUp = 0;

static void UiPositionSubmenuAction(Widget w, XEvent * event, String * params,
				    Cardinal * num_params)
{
    Widget new_active_submenu, new_active_entry;

    new_active_entry = XawSimpleMenuGetActiveEntry(w);

    if (new_active_entry != active_entry) {
	int i, level;

	new_active_submenu = NULL;

	/* Find the submenu for the current active menu item.  */
	for (i = 0; i < NumSubmenus; i++) {
	    if (Submenus[i].parent == new_active_entry) {
		new_active_submenu = Submenus[i].widget;
		break;
	    }
	}

	/* Find the level of this submenu (0 = root menu).  */
	for (level = i = 0; i < NumSubmenus ; i++) {
	    if (Submenus[i].widget == w) {
		level = Submenus[i].level;
		break;
	    }
	}

	/* Remove all the submenus whose level is higher than this submenu.  */
	for (i = 0; i < NumSubmenus; i++) {
	    if (Submenus[i].level > level)
		XtPopdown(Submenus[i].widget);
	}

	/* Position the submenu for this menu item.  */
	if (new_active_submenu != NULL && new_active_entry != NULL)
	    UiPositionSubmenu(new_active_submenu, new_active_entry);

	active_submenu = new_active_submenu;
	active_entry = new_active_entry;
    }
}

static void UiPopdownSubmenusAction(Widget w, XEvent * event, String * params,
				    Cardinal * num_params)
{
    int i;

    /* Pop down all the submenus.  */
    for (i = 0; i < NumSubmenus; i++) {
	XtPopdown(Submenus[i].widget);
    }

    MenuPopup = 0;

    /* Pop down the two main menus too.  */
    XtPopdown(LeftMenuWidget);
    XtPopdown(RightMenuWidget);
}

static void UiMenuUnhighlightAction(Widget w, XEvent * event, String * params,
				    Cardinal * num_params)
{
    XtCallActionProc(w, "unhighlight", event, params, *num_params);
}

static void UiPositionSubmenu(Widget w, Widget parent)
{
    Position parent_x, parent_y, my_x, my_y;
    Dimension parent_width, my_width, my_height;
    int root_width, root_height, foo;
    Window foowin;

    XtVaGetValues(parent, XtNx, &parent_x, XtNy, &parent_y,
		  XtNwidth, &parent_width, NULL);
    XtVaGetValues(w, XtNwidth, &my_width, XtNheight, &my_height, NULL);
    XtTranslateCoords(XtParent(parent), parent_x, parent_y,
		      &parent_x, &parent_y);
    my_x = parent_x + parent_width - 2;
    my_y = parent_y + 1;
    XGetGeometry(display, RootWindow(display, screen), &foowin, &foo,
		 &foo, &root_width, &root_height, &foo, &foo);
    if ((my_x + my_width) > root_width)
	my_x -= my_width + parent_width - 2;
    if ((my_y + my_height) > root_height)
	my_y = root_height - my_height;
    XtVaSetValues(w, XtNx, my_x, XtNy, my_y, NULL);
    XtPopup(w, XtGrabNonexclusive);
}

CallbackFunc(UiSubmenuPopupCallback)
{
    SubmenuPoppedUp++;
}

CallbackFunc(UiSubmenuPopdownCallback)
{
    SubmenuPoppedUp--;
    if (XawSimpleMenuGetActiveEntry(w))
	XtPopdown((Widget)client_data);
}

/* ------------------------------------------------------------------------- */

/* These functions simply perform the actions requested in the menus. */

CallbackFunc(UiAttachDisk)
{
    int unit = (int)client_data;
    char *filename;
    char title[1024];
    UiButton button;

    suspend_speed_eval();
    sprintf(title, "Attach Disk Image as unit #%d", unit);
    filename = UiFileSelect(title, read_disk_image_contents,
			    unit == 8 ? True : False, &button);

    switch (button) {
      case Button_Ok:
 	if (serial_select_file(DT_DISK | DT_1541, unit, filename) < 0)
	    UiError("Invalid Disk Image");
	break;
#ifdef AUTOSTART
      case Button_Autostart:
	if (autostart_disk(filename) < 0)
	    UiError("Invalid Disk Image");
	break;
#endif
      default:
	/* Do nothing special.  */
        break;
    }
}

CallbackFunc(UiDetachDisk)
{
    int unit = (int)client_data;

    suspend_speed_eval();
    if (unit < 0) {
	remove_serial(8);
	remove_serial(9);
	remove_serial(10);
    } else {
	remove_serial(unit);
    }
}

#if defined CBM64 || defined C128

CallbackFunc(UiAttachTape)
{
    char *filename;
    UiButton button;

    suspend_speed_eval();

    filename = UiFileSelect("Attach a tape image", read_tape_image_contents,
			    True, &button);

    switch (button) {
      case Button_Ok:
	if (serial_select_file(DT_TAPE, 1, filename) < 0)
	    UiError("Invalid Tape Image");
	break;
#ifdef AUTOSTART
      case Button_Autostart:
	if (autostart_tape(filename) < 0)
	    UiError("Invalid Tape Image");
	break;
#endif
      default:
	/* Do nothing special.  */
        break;
    }
}

CallbackFunc(UiDetachTape)
{
    remove_serial(1);
}

static char *read_disk_or_tape_image_contents(const char *fname)
{
    char *tmp;

    tmp = read_disk_image_contents(fname);
    if (tmp)
	return tmp;
    return read_tape_image_contents(fname);
}

CallbackFunc(UiSmartAttach)
{
    char *filename;
    UiButton button;

    suspend_speed_eval();

    filename = UiFileSelect("Smart-attach a file",
			    read_disk_or_tape_image_contents,
			    True, &button);

    switch (button) {
      case Button_Ok:
 	if (serial_select_file(DT_DISK | DT_1541, 8, filename) < 0
	    && serial_select_file(DT_TAPE, 1, filename) < 0) {
	    UiError("Unknown image type");
	}
	break;
#ifdef AUTOSTART
      case Button_Autostart:
	if (autostart_autodetect(filename) < 0)
	    UiError("Unknown image type");
	break;
#endif
      default:
	/* Do nothing special.  */
        break;
    }
}

#endif /* CBM64 || C128 */

CallbackFunc(UiChangeWorkingDir)
{
    PATH_VAR(wd);
    int path_max = GET_PATH_MAX;

    getcwd(wd, path_max);
    suspend_speed_eval();
    if (UiInputString("VICE setting", "Change current working directory",
		      wd, path_max) != Button_Ok)
	return;
    else if (chdir(wd) < 0)
	UiError("Directory not found");
}

CallbackFunc(UiActivateXDebugger)
{
    suspend_speed_eval();
    activate_xdebug_window();
}

CallbackFunc(UiActivateMonitor)
{
    suspend_speed_eval();
    UiDispatchEvents();		/* popdown the menu */
    UiAutoRepeatOn();
    maincpu_trigger_trap(mon);
}

CallbackFunc(UiRunC1541)
{
    suspend_speed_eval();
    switch (system("xterm -sb -e c1541 &")) {
      case 127:
	UiError("Couldn't run /bin/sh???");
	break;
      case -1:
	UiError("Couldn't run xterm");
	break;
      case 0:
	break;
      default:
	UiError("Unknown error while running c1541");
    }
}

CallbackFunc(UiReset)
{
    suspend_speed_eval();
    maincpu_trigger_reset();
}

CallbackFunc(UiPowerUpReset)
{
    suspend_speed_eval();
    mem_powerup();
    maincpu_trigger_reset();
}

CallbackFunc(UiBrowseManual)
{
    if (app_resources.htmlBrowserCommand == NULL
	|| *app_resources.htmlBrowserCommand == '\0') {
	UiError("No HTML browser is defined.");
    } else {
#define BROWSE_CMD_BUF_MAX 16384
	char buf[BROWSE_CMD_BUF_MAX];
	static const char manual_path[] = LIBDIR "/" DOCDIR "/MANUAL.html";
	char *res_ptr;
	int manual_path_len, cmd_len;

	cmd_len = strlen(app_resources.htmlBrowserCommand);
	manual_path_len = strlen(manual_path);

	res_ptr = strstr(app_resources.htmlBrowserCommand, "%s");
	if (res_ptr == NULL) {
	    /* No substitution. */
	    if (cmd_len + 2 > BROWSE_CMD_BUF_MAX - 1) {
		UiError("Browser command too long.");
		return;
	    }
	    sprintf(buf, "%s &", app_resources.htmlBrowserCommand);
	} else {
	    char *tmp_ptr, *cmd_ptr;
	    int offs;

	    /* Replace each occurrence of "%s" with the path of the HTML
               manual. */

	    cmd_len += manual_path_len - 2;
	    cmd_len += 2;	/* Trailing " &". */
	    if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
		UiError("Browser command too long.");
		return;
	    }

	    offs = res_ptr - app_resources.htmlBrowserCommand;
	    memcpy(buf, app_resources.htmlBrowserCommand, offs);
	    strcpy(buf + offs, manual_path);
	    cmd_ptr = buf + offs + manual_path_len;
	    res_ptr += 2;

	    while ((tmp_ptr = strstr(res_ptr, "%s")) != NULL) {
		cmd_len += manual_path_len - 2;
		if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
		    UiError("Browser command too long.");
		    return;
		}
		offs = tmp_ptr - res_ptr;
		memcpy(cmd_ptr, res_ptr, offs);
		strcpy(cmd_ptr + offs, manual_path);
		cmd_ptr += manual_path_len + offs;
		res_ptr = tmp_ptr + 2;
	    }

	    sprintf(cmd_ptr, "%s &", res_ptr);
	}

	printf("Executing `%s'...\n", buf);
	if (system(buf) != 0)
	    UiError("Cannot run HTML browser.");
    }
}

CallbackFunc(UiExit)
{
    UiButton b;

    b = UiAskConfirmation("Exit " EMULATOR " emulator",
			  "Do you really want to exit?");

    if (b == Button_Yes) {
	if (app_resources.saveResourcesOnExit && resources_have_changed) {
	    b = UiAskConfirmation("Exit " EMULATOR " emulator",
				  "Save the current settings?");
	    if (b == Button_Yes)
		UiSaveResources(NULL, NULL, NULL);
	    else if (b == Button_Cancel)
		return;
	}
	UiAutoRepeatOn();
	exit(-1);
    }
}

CallbackFunc(UiInfo)
{
    static Widget InfoDialog;
    static UiButton button;

    if (!InfoDialog) {
	InfoDialog = UiBuildInfoDialog
	    (TopLevel, &button,
	     "", "V I C E", "the Versatile Commodore Emulator", "",
	     "Version " VERSION,
#ifdef UNSTABLE
	     "(unstable)",
#endif
	     "", "Copyright (c) 1993-1998",
	     "E. Perazzoli, T. Rantanen, A. Fachat,",
	     "J. Valta, D. Sladic and J. Sonninen", "",
	     "Official VICE homepage:",
	     "http://www.tu-chemnitz.de/~fachat/vice/vice.html", "", NULL);
    }
    suspend_speed_eval();
    UiPopup(XtParent(InfoDialog), "VICE Information", False);
    button = Button_None;
    do
	UiDispatchNextEvent();
    while (button == Button_None);
    UiPopdown(XtParent(InfoDialog));
}

CallbackFunc(UiSetRefreshRate)
{
    if (!call_data) {
	if (app_resources.refreshRate != (int)client_data) {
	    app_resources.refreshRate = (int)client_data;
	    resources_have_changed = 1;
	    UiUpdateMenus();
	}
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.refreshRate == (int)client_data
		       ? CheckmarkBitmap : 0), NULL);
	if (client_data == 0 && app_resources.speed == 0) {
	    /* Cannot enable the `automatic' setting if a speed limit is not
	       specified. */
	    XtVaSetValues(w, XtNsensitive, False, NULL);
	} else {
	    XtVaSetValues(w, XtNsensitive, True, NULL);
	}
    }
}

CallbackFunc(UiSetCustomRefreshRate)
{
    static char input_string[32];
    char msg_string[256];
    UiButton button;
    int i, found;
    MenuEntry *m = &SetRefreshRateSubmenu[0];

    if (!*input_string)
	sprintf(input_string, "%d", app_resources.refreshRate);

    if (call_data) {
	for (found = i = 0; m[i].callback == UiSetRefreshRate; i++) {
	    if (app_resources.refreshRate == (int)m[i].callback_data)
		found++;
	}
	XtVaSetValues(w, XtNleftBitmap, found ? 0 : CheckmarkBitmap, NULL);
    } else {
	suspend_speed_eval();
	sprintf(msg_string, "Enter refresh rate");
	button = UiInputString("Refresh rate", msg_string,
			       input_string, 32);
	if (button == Button_Ok) {
	    i = atoi(input_string);
	    if (!(app_resources.speed <= 0 && i <= 0) && i >= 0
		&& app_resources.refreshRate != i) {
		resources_have_changed = 1;
		app_resources.refreshRate = i;
		UiUpdateMenus();
	    }
	}
    }
}

CallbackFunc(UiSetMaximumSpeed)
{
    if (!call_data) {
	if (app_resources.speed != (int)client_data) {
	    app_resources.speed = (int)client_data;
	    resources_have_changed = 1;
	    UiUpdateMenus();
	}
    } else {
	XtVaSetValues(w, XtNleftBitmap, app_resources.speed == (int) client_data
		      ? CheckmarkBitmap : 0, NULL);
	if (client_data == 0 && app_resources.refreshRate == 0) {
	    /* Cannot enable the `no limit' setting if the refresh rate is set
	       to `automatic'. */
	    XtVaSetValues(w, XtNsensitive, False, NULL);
	} else {
	    XtVaSetValues(w, XtNsensitive, True, NULL);
	}
    }
}

CallbackFunc(UiSetCustomMaximumSpeed)
{
    static char input_string[32];
    char msg_string[256];
    UiButton button;
    int i, found;
    MenuEntry *m = &SetMaximumSpeedSubmenu[0];

    if (!*input_string)
	sprintf(input_string, "%d", app_resources.speed);

    if (call_data) {
	for (found = i = 0; m[i].callback == UiSetMaximumSpeed; i++) {
	    if (app_resources.speed == (int)m[i].callback_data)
		found++;
	}
	XtVaSetValues(w, XtNleftBitmap, found ? 0 : CheckmarkBitmap, NULL);
    } else {
	suspend_speed_eval();
	sprintf(msg_string, "Enter speed");
	button = UiInputString("Maximum run speed", msg_string, input_string,
			       32);
	if (button == Button_Ok) {
	    i = atoi(input_string);
	    if (!(app_resources.refreshRate <= 0 && i <= 0) && i >= 0
		&& app_resources.speed != i) {
		resources_have_changed = 1;
		app_resources.speed = i;
		UiUpdateMenus();
	    } else
		UiError("Invalid speed value");
	}
    }
}

CallbackFunc(UiSaveResources)
{
    suspend_speed_eval();
    if (resources_save(NULL, EMULATOR) < 0)
	UiError("Cannot save settings.");
    else {
	if (w != NULL)
	    UiMessage("Settings saved successfully.");
	resources_have_changed = 0;
    }
    UiUpdateMenus();
}

CallbackFunc(UiLoadResources)
{
    suspend_speed_eval();
    if (resources_load(NULL, EMULATOR) < 0)
	UiError("Cannot load settings.");
#if 0
    else if (w != NULL)
            UiMessage("Settings loaded.");
#endif
    UiUpdateMenus();
}

CallbackFunc(UiSetDefaultResources)
{
    suspend_speed_eval();
    resources_set_defaults();
    resources_have_changed = 1;
    UiUpdateMenus();
}

/* ------------------------------------------------------------------------- */

/* These are the callbacks for the toggle menus (the ones with a checkmark on
   the left).  If call_data is NULL, they simply set/unset the checkmark
   according to the value of the corrisponding resource.  If not NULL, they
   set the value of the corresponding resource before doing so.  For this
   reason, to update the checkmarks, we simply have to call all the callbacks
   with a NULL call_data parameter. */

#define DEFINE_TOGGLE(f, resource, update_func)				\
    CallbackFunc(f)							\
    {									\
	if (!call_data) {						\
            void (*ff)() = update_func;					\
	    app_resources.resource = !app_resources.resource;	        \
	    resources_have_changed = 1;					\
	    if (ff)							\
		ff();							\
	    UiUpdateMenus();						\
	} else {							\
	    XtVaSetValues(w, XtNleftBitmap,				\
			  app_resources.resource ? CheckmarkBitmap	\
			                         : 0, NULL);		\
	}								\
    }

DEFINE_TOGGLE(UiToggleVideoCache, videoCache, video_resize)

DEFINE_TOGGLE(UiToggleDoubleSize, doubleSize, video_resize)

DEFINE_TOGGLE(UiToggleDoubleScan, doubleScan, video_resize)

DEFINE_TOGGLE(UiToggleUseXSync, useXSync, NULL)

DEFINE_TOGGLE(UiToggleSaveResourcesOnExit, saveResourcesOnExit, NULL)

CallbackFunc(UiTogglePause)
{
    static int paused;

    if (paused) {
	if (call_data == NULL)
	    paused = 0;
    } else {			/* !paused */
	if (call_data == NULL) {
	    paused = 1;
	    XtVaSetValues(w, XtNleftBitmap, CheckmarkBitmap, NULL);
	    UiDisplayPaused();
	    suspend_speed_eval();
	    while (paused)
		UiDispatchNextEvent();
	}
    }
    XtVaSetValues(w, XtNleftBitmap, paused ? CheckmarkBitmap : 0, NULL);
    UiDisplaySpeed(0.0, 0.0);
}

CallbackFunc(UiToggleTurbo)
{
    static int refresh = 0, speed = 100;
    suspend_speed_eval();
    if (!app_resources.speed && app_resources.refreshRate == 25)
    {
	app_resources.speed = speed;
	app_resources.refreshRate = refresh;
    }
    else
    {
	refresh = app_resources.refreshRate;
	speed = app_resources.speed;
	app_resources.speed = 0;
	app_resources.refreshRate = 25;
    }
    UiUpdateMenus();
}

/* ------------------------------------------------------------------------- */

/* C64/128 specific menu items. */

#if defined(CBM64) || defined(C128) || defined(PET)

DEFINE_TOGGLE(UiToggleSpriteToSpriteCollisions, checkSsColl, NULL)
DEFINE_TOGGLE(UiToggleSpriteToBackgroundCollisions, checkSbColl, NULL)

#ifdef HAS_JOYSTICK

CallbackFunc(UiSetJoystickDevice1)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.joyDevice1 = (int) client_data;
	UiUpdateMenus();
    } else
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.joyDevice1 == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    joyport1select(app_resources.joyDevice1);
}

CallbackFunc(UiSetJoystickDevice2)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.joyDevice2 = (int) client_data;
	UiUpdateMenus();
    } else
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.joyDevice2 == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    joyport2select(app_resources.joyDevice2);
}

CallbackFunc(UiSwapJoystickPorts)
{
    int tmp;

    if (w != NULL)
	suspend_speed_eval();
    tmp = app_resources.joyDevice1;
    app_resources.joyDevice1 = app_resources.joyDevice2;
    app_resources.joyDevice2 = tmp;
    resources_have_changed = 1;
    UiUpdateMenus();
}

#else  /* !HAS_JOYSTICK */

CallbackFunc(UiSetNumpadJoystickPort)
{
    suspend_speed_eval();
    if (!call_data) {
	if (app_resources.joyPort != (int)client_data) {
	    app_resources.joyPort = (int)client_data;
	    resources_have_changed = 1;
	    UiUpdateMenus();
	}
    } else
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.joyPort == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
}

CallbackFunc(UiSwapJoystickPorts)
{
    suspend_speed_eval();
    app_resources.joyPort = 3 - app_resources.joyPort;
    printf("Numpad joystick now in port #%d.\n", app_resources.joyPort);
    UiUpdateMenus();
}

#endif

#endif
#if defined(CBM64) || defined(C128)

DEFINE_TOGGLE(UiToggleEmuID, emuID, NULL)

CallbackFunc(UiToggleIEEE488)
{
    if (!call_data) {
        app_resources.ieee488 = !app_resources.ieee488;
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time. */
	if (app_resources.ieee488)
	    app_resources.reu = 0;
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      app_resources.ieee488 ? CheckmarkBitmap : 0, NULL);
    }
}

CallbackFunc(UiToggleREU)
{
    if (!call_data) {
        app_resources.reu = !app_resources.reu;
	/* The REU and the IEEE488 interface share the same address space, so
	   they cannot be enabled at the same time. */
	if (app_resources.reu) {
	    app_resources.ieee488 = 0;
	    activate_reu();
	}
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      app_resources.reu ? CheckmarkBitmap : 0, NULL);
    }
}

#endif /* CBM64 || C128 */

/* ------------------------------------------------------------------------- */

/* True 1541 support items. */

#ifdef HAVE_TRUE1541

DEFINE_TOGGLE(UiToggleTrue1541, true1541, true1541_ack_switch)

DEFINE_TOGGLE(UiToggleParallelCable, true1541ParallelCable, NULL)

CallbackFunc(UiSetCustom1541SyncFactor)
{
    static char input_string[256];
    char msg_string[256];
    UiButton button;

    if (!*input_string)
	sprintf(input_string, "%d", app_resources.true1541SyncFactor);

    if (call_data) {
	if (app_resources.true1541SyncFactor != TRUE1541_PAL_SYNC_FACTOR
	    && app_resources.true1541SyncFactor != TRUE1541_NTSC_SYNC_FACTOR)
	    XtVaSetValues(w, XtNleftBitmap, CheckmarkBitmap, NULL);
	else
	    XtVaSetValues(w, XtNleftBitmap, 0, NULL);
    } else {
	suspend_speed_eval();
	sprintf(msg_string, "Enter factor (PAL %d, NTSC %d)",
		TRUE1541_PAL_SYNC_FACTOR, TRUE1541_NTSC_SYNC_FACTOR);
	button = UiInputString("1541 Sync Factor", msg_string, input_string,
			       256);
	if (button == Button_Ok) {
	    int v;

	    v = atoi(input_string);
	    if (v != app_resources.true1541SyncFactor) {
		true1541_set_sync_factor(atoi(input_string));
		resources_have_changed = 1;
		UiUpdateMenus();
	    }
	}
    }
}

CallbackFunc(UiSet1541SyncFactor)
{
    if (!call_data) {
	if (app_resources.true1541SyncFactor != (int)client_data) {
	    true1541_set_sync_factor((int) client_data);
	    UiUpdateMenus();
	    resources_have_changed = 1;
	}
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.true1541SyncFactor == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    }
}

CallbackFunc(UiSet1541IdleMethod)
{
    if (!call_data) {
	/* XXX: not sure this is needed */
	if (app_resources.true1541)
	    true1541_cpu_execute();
	app_resources.true1541IdleMethod = (int)client_data;
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.true1541IdleMethod == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    }
}

#endif /* HAVE_TRUE1541 */

/* ------------------------------------------------------------------------- */

/* PET stuff */

#ifdef PET

CallbackFunc(UiTogglePetDiag)
{
    if (!call_data) {
        app_resources.petdiag = !app_resources.petdiag;
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      app_resources.petdiag ? CheckmarkBitmap : 0, NULL);
    }
}

CallbackFunc(UiToggleNumpadJoystick)
{
    if (!call_data) {
        app_resources.numpadJoystick = !app_resources.numpadJoystick;
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap, app_resources.numpadJoystick
		? CheckmarkBitmap : 0, NULL);
    }
}

#endif /* PET */

/* ------------------------------------------------------------------------- */

/* Sound support. */

#ifdef SOUND
CallbackFunc(UiToggleSound)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.sound = !app_resources.sound;
	if (app_resources.sound) {
	    /* Disable the vsync timer, or SIGALRM might happen while writing
	       to the sound device. */
	    vsync_disable_timer();
	} else
	    close_sound();
	resources_have_changed = 1;
	UiUpdateMenus();
    } else
	XtVaSetValues(w, XtNleftBitmap,
		      app_resources.sound ? CheckmarkBitmap : 0, NULL);
}

CallbackFunc(UiToggleSoundSpeedAdjustment)
{
    if (!call_data) {
	app_resources.soundSpeedAdjustment =
	    !app_resources.soundSpeedAdjustment;
	resources_have_changed = 1;
	UiUpdateMenus();
    } else
	XtVaSetValues(w, XtNleftBitmap,
		      app_resources.soundSpeedAdjustment ? CheckmarkBitmap : 0,
		      NULL);
}

CallbackFunc(UiSetSoundSampleRate)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.soundSampleRate = (int) client_data;
	close_sound();
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.soundSampleRate == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    }
}

CallbackFunc(UiSetSoundBufferSize)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.soundBufferSize = (int) client_data;
	close_sound();
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.soundBufferSize == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    }
}

CallbackFunc(UiSetSoundSuspendTime)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.soundSuspendTime = (int) client_data;
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.soundSuspendTime == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    }
}

#if defined(CBM64) || defined(C128)

DEFINE_TOGGLE(UiToggleSidFilters, sidFilters, close_sound)

CallbackFunc(UiSetSidModel)
{
    suspend_speed_eval();
    if (!call_data) {
	app_resources.sidModel = (int) client_data;
	close_sound();
	resources_have_changed = 1;
	UiUpdateMenus();
    } else {
	XtVaSetValues(w, XtNleftBitmap,
		      (app_resources.sidModel == (int) client_data
		       ? CheckmarkBitmap : 0), NULL);
    }
}

#endif /* CBM64 || C128 */

#endif /* SOUND */
