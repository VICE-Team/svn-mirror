/*
 * ui.c - Simple Xaw-based graphical user interface.  It uses widgets
 * from the Free Widget Foundation and Robert W. McMullen.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *  André Fachat (fachat@physik.tu-chemnitz.de)
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
#include <signal.h>

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

#ifdef USE_VIDMODE_EXTENSION
#define VidMode_MINMAJOR 0
#define VidMode_MINMINOR 0

#include <X11/extensions/xf86vmode.h>
#endif

#include "widgets/Canvas.h"
#include "widgets/FileSel.h"
#include "widgets/TextField.h"

#include "ui.h"

#include "cmdline.h"
#include "drive.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "resources.h"
#include "uihotkey.h"
#include "uimenu.h"
#include "uisettings.h"
#include "utils.h"
#include "vsync.h"

/* FIXME: We want these to be static.  */
Display *display;
int screen;
Visual *visual;
int depth = X_DISPLAY_DEPTH;
int have_truecolor;

static int n_allocated_pixels = 0;
static unsigned long allocated_pixels[0x100];

/* UI logging goes here.  */
static log_t ui_log = LOG_ERR;

Widget canvas, pane;

Cursor blankCursor;
static int cursor_is_blank = 0;

#ifdef USE_VIDMODE_EXTENSION
static int vidmodeavail = 0;
static int use_fullscreen = 0;
static int use_fullscreen_at_start = 0;
static int timeout;

int vidmodecount;
XF86VidModeModeInfo **allmodes;

static ui_bestvideomode bestmodes[10];
static int bestmode_counter;
static char *selected_videomode;
static char *selected_videomode_at_start;
static int selected_videomode_index;

static ui_menu_entry_t* resolutions_submenu;
extern void video_setfullscreen(int v,int width, int height);
#endif

static void ui_display_drive_current_image2(void);

/* ------------------------------------------------------------------------- */

_ui_resources_t _ui_resources;

#ifdef USE_VIDMODE_EXTENSION

UI_MENU_DEFINE_STRING_RADIO(SelectedFullscreenMode);

static void mouse_timeout(int signo)
{
    if (use_fullscreen && !cursor_is_blank)
        XDefineCursor(display, XtWindow(canvas), blankCursor);
}

void ui_set_mouse_timeout()
{
    if (!use_fullscreen)
        return;

    cursor_is_blank = 0;
    if (signal(SIGALRM, mouse_timeout) == SIG_ERR)
        return;
    alarm(5);
}

static void mouse_handler(Widget w, XtPointer client_data, XEvent *report,
                          Boolean *ctd)
{
    if (report->type == LeaveNotify)
        cursor_is_blank = 1;
    else
        cursor_is_blank = 0;

    if (! _mouse_enabled) {
        XUndefineCursor(display,XtWindow(canvas));
	if (cursor_is_blank == 0) {
	    if (signal(SIGALRM, mouse_timeout) == SIG_ERR)
	      return;
	    alarm(5);
	}
    }
}

static int vidmode_available(void)
{
    int MajorVersion, MinorVersion;
    int EventBase, ErrorBase;
    int i, hz = 0;

    bestmode_counter = 0;

    if (! XF86VidModeQueryVersion (display, &MajorVersion, &MinorVersion)) {
        log_error(ui_log, "Unable to query video extension version");
        return 0;
    }
    if (! XF86VidModeQueryExtension (display, &EventBase, &ErrorBase)) {
        log_error(ui_log, "Unable to query video extension information");
        return 0;
    }
    if (MajorVersion < VidMode_MINMAJOR
        || (MajorVersion == VidMode_MINMAJOR && MinorVersion < VidMode_MINMINOR)
) {
        /* Fail if the extension version in the server is too old */
        log_error(ui_log, "Xserver is running an old XFree86-VidMode version (%d.  %d)",
	       MajorVersion, MinorVersion);
        log_error(ui_log, "Minimum required version is %d.%d",
	         VidMode_MINMAJOR, VidMode_MINMINOR);
        return 0;
    }
    if (! XF86VidModeGetAllModeLines (display, screen, &vidmodecount, &allmodes)) {
        log_error(ui_log, "Error getting video mode information");
        return 0;
    }

    for (i = 0; i < vidmodecount; i++) {
        if (allmodes[i]->hdisplay <= 800 &&
	   allmodes[i]->hdisplay >= 320 &&
	   allmodes[i]->vdisplay <= 600  &&
	   allmodes[i]->vdisplay >= 200) {
	      bestmodes[bestmode_counter].modeindex=i;
	      hz = allmodes[i]->dotclock * 1000 /
		( allmodes[i]->vtotal * allmodes[i]->htotal) ;
	      snprintf(bestmodes[bestmode_counter].name,17," %ix%i-%iHz",
		       allmodes[i]->hdisplay,
		       allmodes[i]->vdisplay,
		       hz);
	      bestmode_counter++;
	      if (bestmode_counter == 10) break;
	}
    }
    return 1;
}

static void focus_window_again(void)
{
    if (!use_fullscreen)
        return;
    XGrabKeyboard(display, XtWindow(canvas),
		  1, GrabModeAsync,
		  GrabModeAsync,  CurrentTime);
    XGrabPointer(display, XtWindow(canvas), 1,
		 PointerMotionMask | ButtonPressMask |
		 ButtonReleaseMask,
		 GrabModeAsync, GrabModeAsync,
		 XtWindow(canvas),
		 None, CurrentTime);
    XtVaSetValues(XtParent(XtParent(canvas)),
		  XtNx,          0,
		  XtNy,          0,
		  None);
    XWarpPointer(display, None,
		 XtWindow(canvas),
		 0, 0, 0, 0, 0, 0);
    XRaiseWindow(display, XtWindow(XtParent(XtParent(canvas))));
    ui_set_mouse_timeout();
}

static int set_fullscreen(resource_value_t v)
{
    static Dimension x,y,w,h;
    static Dimension canvas_width, canvas_height;
    static int root_x, root_y;
    static int win_x,win_y;
    static int interval,prefer_blanking,allow_exposures;
    static XF86VidModeModeLine restoremodeline;
    static unsigned int dotclock;
    static int window_doublesize;
    static int panecolor;
    int i;

    if ( !vidmodeavail || !bestmode_counter ) {
        use_fullscreen_at_start = (int) v;
	return 0;
    }
    if ( selected_videomode_index == -1) {
	return 0;
    }

    if (v && !use_fullscreen) {
        log_message(LOG_DEFAULT, "Switch to fullscreen %ix%i",
		    allmodes[selected_videomode_index]->hdisplay,
		    allmodes[selected_videomode_index]->vdisplay);

	XF86VidModeGetModeLine(display, screen, &dotclock, &restoremodeline);

	XF86VidModeLockModeSwitch(display, screen, 1);
	XtVaGetValues(XtParent(XtParent(canvas)),
		      XtNx,          &x,
		      XtNy,          &y,
		      XtNwidth,      &w,
		      XtNheight,     &h,
		      NULL);
	XtVaGetValues(canvas,
		      XtNwidth, &canvas_width,
		      XtNheight, &canvas_height,
		      NULL);
	XtVaGetValues(pane,
		      XtNbackground, &panecolor,
		      NULL);
	XtVaSetValues(pane,
		      XtNbackground, BlackPixel(display,screen),
		      NULL);
	if ( ! XF86VidModeSwitchToMode(display, screen,
				       allmodes[selected_videomode_index])) {
	    log_error(ui_log,"Error switching to fullscreen %ix%i\n",
		      allmodes[selected_videomode_index]->hdisplay,
		      allmodes[selected_videomode_index]->vdisplay);
	    return 0;
	}

	XtVaSetValues(XtParent(XtParent(canvas)),
		      XtNx,          0,
		      XtNy,          0,
		      XtNwidth,
		      w + allmodes[selected_videomode_index]->hdisplay -
		      canvas_width + 10,
		      XtNheight,
		      h + allmodes[selected_videomode_index]->vdisplay -
		      canvas_height + 10,
		      NULL);


    /* A small hack!!!!  */
	{
	    Window root, child;
	    int mask;

	    XQueryPointer(display, XtWindow(canvas),
			  &root, &child, &root_x, &root_y,
			  &win_x, &win_y, &mask);

	    XF86VidModeSetViewPort(display,XDefaultScreen(display),
				   root_x - win_x - 1,
				   root_y - win_y - 1);

	}

	XGrabKeyboard(display, XtWindow(canvas),
		      1, GrabModeAsync,
		      GrabModeAsync,  CurrentTime);
	XGrabPointer(display, XtWindow(canvas), 1,
		     PointerMotionMask | ButtonPressMask |
		     ButtonReleaseMask,
		     GrabModeAsync, GrabModeAsync,
		     XtWindow(canvas),
		     None, CurrentTime);

	XGetScreenSaver(display,&timeout,&interval,
			&prefer_blanking,&allow_exposures);
	XSetScreenSaver(display,0,0,DefaultBlanking,DefaultExposures);

	use_fullscreen = 1;
	video_setfullscreen(1,allmodes[selected_videomode_index]->hdisplay - 2,
			    allmodes[selected_videomode_index]->vdisplay - 2);
	XtVaSetValues(XtParent(XtParent(canvas)),
		      XtNwidth,
		      w + allmodes[selected_videomode_index]->hdisplay -
		      canvas_width - 2,
		      XtNheight,
		      h + allmodes[selected_videomode_index]->vdisplay -
		      canvas_height - 2,
		      NULL);
	XWarpPointer(display, None,
		     XtWindow(canvas),
		     0, 0, 0, 0, 0, 0);
	if (resources_get_value("DoubleSize",
                            (resource_value_t *) &window_doublesize) < 0)
	  window_doublesize = 0;
	XtAddEventHandler(canvas,
			  PointerMotionMask | ButtonPressMask |
			  LeaveWindowMask,
			  True,
			  (XtEventHandler) mouse_handler, NULL);
	XRaiseWindow(display, XtWindow(XtParent(XtParent(canvas))));
	ui_set_mouse_timeout();
    } else if ((int) v == 2) {
        int troot_x, troot_y;
        int twin_x,twin_y;

        log_message(LOG_DEFAULT, "Change to fullscreen %ix%i",
		    allmodes[selected_videomode_index]->hdisplay,
		    allmodes[selected_videomode_index]->vdisplay);
	if ( ! XF86VidModeSwitchToMode(display, screen,
				       allmodes[selected_videomode_index])) {
	    log_error(ui_log,"Error switching to fullscreen %ix%i\n",
		      allmodes[selected_videomode_index]->hdisplay,
		      allmodes[selected_videomode_index]->vdisplay);
	    return 0;
	    }
	{
	    Window root, child;
	    int mask;

	    XQueryPointer(display, XtWindow(canvas),
			  &root, &child, &troot_x, &troot_y,
			  &twin_x, &twin_y, &mask);

	    XF86VidModeSetViewPort(display,XDefaultScreen(display),
				   troot_x - twin_x - 1,
				   troot_y - twin_y - 1 );
	}
	XtVaSetValues(XtParent(XtParent(canvas)),
		      XtNx,          0,
		      XtNy,          0,
		      XtNwidth,
		      w + allmodes[selected_videomode_index]->hdisplay -
		      canvas_width - 2 ,
		      XtNheight,
		      h + allmodes[selected_videomode_index]->vdisplay -
		      canvas_height - 2,
		      NULL);
	video_setfullscreen(1,allmodes[selected_videomode_index]->hdisplay - 2,
			    allmodes[selected_videomode_index]->vdisplay - 2);
	XWarpPointer(display, None,
		     XtWindow(canvas),
		     0, 0, 0, 0, 0, 0);
    } else if (use_fullscreen) {
        int ds;
        log_message(LOG_DEFAULT, "Switch to windowmode");

	use_fullscreen = 0;

	XUndefineCursor(display,XtWindow(canvas));

	XtRemoveEventHandler(canvas,
			     PointerMotionMask | ButtonPressMask |
			     LeaveWindowMask,
			     True,
			     (XtEventHandler) mouse_handler,
			     NULL);
	XF86VidModeLockModeSwitch(display, screen, 0);

	XtVaSetValues(pane,
		      XtNbackground, panecolor,
		      NULL);

	/* Oh who has designed the vidmode extension API???? */
	for (i = 0; i < vidmodecount; i++) {
	    if (allmodes[i]->hdisplay == restoremodeline.hdisplay &&
	       allmodes[i]->vdisplay == restoremodeline.vdisplay &&
	       allmodes[i]->dotclock == dotclock ) {
	      XF86VidModeSwitchToMode(display, screen, allmodes[i]);
	      break;
	    }
	}

	video_setfullscreen(0,0,0);

	if (use_fullscreen_at_start) {
	    XtVaSetValues(XtParent(XtParent(canvas)),
			  XtNx, 30,
			  XtNy, 50,
			  NULL);
	    use_fullscreen_at_start = 0;
	} else {
	    XtVaSetValues(XtParent(XtParent(canvas)),
			  XtNx,          x - root_x + win_x + 5,  /*???*/
			  XtNy,          y - root_y + win_y + 5,  /*???*/
			  NULL);
	}

	if (resources_get_value("DoubleSize", (resource_value_t *) &ds) < 0)
            /* FIXME */ ;

	if (ds < window_doublesize) {
	  w -= canvas_width/2;
	  h -= canvas_height/2;
	} else if (ds > window_doublesize) {
	  w += canvas_width;
	  h += canvas_height;
	}
	XtVaSetValues(XtParent(XtParent(canvas)),
		      XtNwidth,      w,
		      XtNheight,     h,
		      NULL);

	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);

	XSetScreenSaver(display,timeout,interval,prefer_blanking,allow_exposures);
	XWarpPointer(display, None,
		     RootWindowOfScreen(XtScreen(canvas)),
		     0, 0, 0, 0, root_x, root_y);

    }
    ui_check_mouse_cursor();
    return 1;
}

static int set_bestmode(resource_value_t v)
{
    int i;
    if (!vidmodeavail) {
      selected_videomode_at_start = (char*) stralloc(v);
      return(0);
    }
    selected_videomode = (char*) v;
    for (i = 0; i < bestmode_counter; i++) {
        if (! strcmp(selected_videomode, bestmodes[i].name)) {
	    selected_videomode_index = bestmodes[i].modeindex;
	    if (use_fullscreen) set_fullscreen((resource_value_t) 2);
	    return(0);
	}
    }
    if (bestmode_counter > 0) {
      selected_videomode_index = bestmodes[0].modeindex;
      selected_videomode = bestmodes[0].name;
    }
    return(0);
}

#endif

void ui_check_mouse_cursor()
{
    int window_doublesize;

    if (_mouse_enabled) {
#ifdef USE_VIDMODE_EXTENSION
        if (use_fullscreen) { 
            if (resources_get_value("FullscreenDoubleSize",
                                    (resource_value_t *) &window_doublesize) < 0)
                return;
        } else
#endif
        {
            if (resources_get_value("DoubleSize",
                                    (resource_value_t *) &window_doublesize) < 0)
                return;
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
#ifdef USE_VIDMODE_EXTENSION
        if (use_fullscreen)
            ui_set_mouse_timeout();
#endif
    }
}

static void ui_restore_mouse(void)
{
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
	  mouse_button(report->xbutton.button-1,(report->type==ButtonPress));
	  break;
    } 
}

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
#ifdef USE_VIDMODE_EXTENSION
    { "UseFullscreen", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &use_fullscreen, set_fullscreen },
    { "SelectedFullscreenMode", RES_STRING, (resource_value_t) "",
      (resource_value_t *) &selected_videomode , set_bestmode },
#endif
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
#ifdef USE_VIDMODE_EXTENSION
    { "-fullscreen", SET_RESOURCE, 0, NULL, NULL,
      "UseFullscreen", (resource_value_t) 1,
      NULL, "Enable fullscreen" },
    { "+fullscreen", SET_RESOURCE, 0, NULL, NULL,
      "UseFullscreen", (resource_value_t) 0,
      NULL, "Disable fullscreen" },
#endif
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

/* Our colormap. */
static Colormap colormap;

/* Application icon.  */
static Pixmap icon_pixmap;

/* Enabled drives.  */
ui_drive_enable_t enabled_drives;

/* Color of the drive active LED.  */
int *drive_active_led;

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
        Widget led;
	Widget current_image;
	/* those two replace the single LED widget when SFD1001 is selected */
        Widget led1;
        Widget led2;
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
static void close_action(Widget w, XEvent *event, String *params,
                         Cardinal *num_params);

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

    if (ui_log == LOG_ERR)
        ui_log = log_open("X11");

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
#if X_DISPLAY_DEPTH == 0
            log_error(ui_log,
                      "Please select a bit depth supported by your display.");
#else
            log_error(ui_log,
                      "Please recompile the program for a supported bit depth.");
#endif
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

#ifdef USE_VIDMODE_EXTENSION
    vidmodeavail = vidmode_available();
#endif

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
    Widget shell, speed_label;
    Widget drive_track_label[NUM_DRIVES], drive_led[NUM_DRIVES];
    Widget drive_current_image[NUM_DRIVES];
    Widget drive_led1[NUM_DRIVES], drive_led2[NUM_DRIVES];
    XSetWindowAttributes attr;
    int i;

    if (alloc_colors(palette, pixel_return) == -1)
	return NULL;

    /* colormap might have changed after ui_alloc_colors, so we set it again */
    XtVaSetValues(_ui_top_level, XtNcolormap, colormap, NULL);

    if (++num_app_shells > MAX_APP_SHELLS) {
	log_error(ui_log, "Maximum number of toplevel windows reached.");
	return NULL;
    }

    shell = XtVaCreatePopupShell
        (title, applicationShellWidgetClass,
         _ui_top_level, XtNinput, True, XtNtitle, title,
         XtNiconName, title, NULL);

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
		      (XtEventHandler) enter_window_callback,
                      NULL);
    XtAddEventHandler(canvas, ExposureMask | StructureNotifyMask, False,
		      (XtEventHandler) exposure_callback,
                      (XtPointer) exposure_proc);

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
            char name[256];

            sprintf(name, "driveCurrentImage%d", i + 1);

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
            sprintf(name, "driveTrack%d", i + 1);

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

            sprintf(name, "driveLed%d", i + 1);

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

	    /* double LEDs */

            sprintf(name, "driveLedA%d", i + 1);

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

            sprintf(name, "driveLedB%d", i + 1);

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

    for (i = 0; i < NUM_DRIVES; i++) {
        app_shells[num_app_shells - 1].drive_widgets[i].track_label
            = drive_track_label[i];
        app_shells[num_app_shells - 1].drive_widgets[i].led
            = drive_led[i];
        XtUnrealizeWidget(drive_led[i]);
        app_shells[num_app_shells - 1].drive_widgets[i].led1
            = drive_led1[i];
        app_shells[num_app_shells - 1].drive_widgets[i].led2
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

    return canvas;
}

void ui_create_dynamic_menues()
{

#ifdef USE_VIDMODE_EXTENSION
    {
	int i;
	char buf[50];
	buf[0] = '*';
	buf[50] = '\0';
        resolutions_submenu = (ui_menu_entry_t*)
        xmalloc(sizeof(ui_menu_entry_t)*
                (size_t) (bestmode_counter + 1));

	for(i = 0; i < bestmode_counter ; i++) {
	    buf[1] = '\0';
	    strncat(buf+1,bestmodes[i].name,48);
	    resolutions_submenu[i].string =
	      (ui_callback_data_t) stralloc(buf);
	    resolutions_submenu[i].callback =
	      (ui_callback_t) radio_SelectedFullscreenMode;
	    resolutions_submenu[i].callback_data =
	      (ui_callback_data_t) bestmodes[i].name;
	    resolutions_submenu[i].sub_menu = NULL;
	    resolutions_submenu[i].hotkey_keysym = 0;
	    resolutions_submenu[i].hotkey_modifier =
	      (ui_hotkey_modifier_t) 0;
	}

	resolutions_submenu[i].string =
	  (ui_callback_data_t) NULL;
    }
    if (bestmode_counter > 0)
        ui_fullscreen_settings_submenu[5].sub_menu = resolutions_submenu;
#endif

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

void ui_set_drive8_menu (Widget w)
{
    char *translation_table;
    char *name = XtName(w);
    int i;

    for (i = 0; i < num_app_shells; i++)
	if (app_shells[i].drive_mapping[0] < 0)
	{
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

    if (drive8_menu != NULL)
        XtDestroyWidget(drive8_menu);
    drive8_menu = w;
}

void ui_set_drive9_menu (Widget w)
{
    char *translation_table;
    char *name = XtName(w);
    int i;

    for (i = 0; i < num_app_shells; i++)
	if (app_shells[i].drive_mapping[1] < 0)
	{
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

    if (drive9_menu != NULL)
        XtDestroyWidget(drive9_menu);
    drive9_menu = w;
}

void ui_set_application_icon(Pixmap icon_pixmap)
{
    int i;

    for (i = 0; i < num_app_shells; i++)
        XtVaSetValues(app_shells[i].shell, XtNiconPixmap, icon_pixmap, NULL);
}

/* ------------------------------------------------------------------------- */

void ui_exit(void)
{
    ui_button_t b;
    char *s = concat ("Exit ", machine_name, " emulator", NULL);

    b = ui_ask_confirmation(s, "Do you really want to exit?");

    if (b == UI_BUTTON_YES) {
	if (_ui_resources.save_resources_on_exit) {
	    b = ui_ask_confirmation(s, "Save the current settings?");
	    if (b == UI_BUTTON_YES) {
		if (resources_save(NULL) < 0)
		    ui_error("Cannot save settings.");
	    } else if (b == UI_BUTTON_CANCEL) {
                free(s);
		return;
            }
	}
	ui_autorepeat_on();
	ui_restore_mouse();
#ifdef USE_VIDMODE_EXTENSION
	ui_set_windowmode();
#endif
	exit(0);
    }

    free(s);
}

/* ------------------------------------------------------------------------- */

/* Set the colormap variable.  The user must tell us whether he wants the
   default one or not using the `privateColormap' resource.  */
static int alloc_colormap(void)
{
    if (colormap)
	return 0;

    if (!_ui_resources.use_private_colormap
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

/* Allocate colors in the colormap. */
static int do_alloc_colors(const palette_t *palette, PIXEL pixel_return[],
                           int releasefl)
{
    int i, failed;
    XColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);

    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
    im = XCreateImage(display, visual, depth,
		      ZPixmap, 0, (char *)data, 1, 1, 8, 0);
    if (!im)
        return -1;

    n_allocated_pixels = 0;

    color.flags = DoRed | DoGreen | DoBlue;
    for (i = 0, failed = 0; i < palette->num_entries; i++) {
        color.red = palette->entries[i].red << 8;
        color.green = palette->entries[i].green << 8;
        color.blue = palette->entries[i].blue << 8;
        if (!XAllocColor(display, colormap, &color)) {
            failed = 1;
            log_warning(ui_log, "Cannot allocate color \"#%04X%04X%04X\".",
                        color.red, color.green, color.blue);
        } else {
            allocated_pixels[n_allocated_pixels++] = color.pixel;
	}
        XPutPixel(im, 0, 0, color.pixel);
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

    if (releasefl && failed && n_allocated_pixels) {
        XFreeColors(display, colormap, allocated_pixels, n_allocated_pixels, 0);
	n_allocated_pixels = 0;
    }

    XDestroyImage(im);

    if (!failed) {
        XColor screen, exact;

        if (!XAllocNamedColor(display, colormap, "black", &screen, &exact))
            failed = 1;
        else {
            drive_led_off_pixel = screen.pixel;
            allocated_pixels[n_allocated_pixels++] = screen.pixel;
        }

        if (!failed) {
            if (!XAllocNamedColor(display, colormap, "red", &screen, &exact))
                failed = 1;
            else {
                drive_led_on_red_pixel = screen.pixel;
                allocated_pixels[n_allocated_pixels++] = screen.pixel;
            }
            if (!failed) {
                if (!XAllocNamedColor(display, colormap, "green", &screen, &exact))
                    failed = 1;
                else {
                    drive_led_on_green_pixel = screen.pixel;
                    allocated_pixels[n_allocated_pixels++] = screen.pixel;
                }
            }
        }

    }

    return failed;
}

/* In here we try to allocate the given colors. This function is called from
 * 'ui_open_canvas_window()'.  The calling function sets the colormap
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
            log_warning(ui_log, "Automagically using a private colormap.");
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

/* Change the colormap of window `w' on the fly.  This only works for
   TrueColor visuals.  Otherwise, it would be too messy to re-allocate the
   new colormap.  */
int ui_canvas_set_palette(ui_window_t w, const palette_t *palette,
                          PIXEL *pixel_return)
{
    if (!have_truecolor) {
	int nallocp;
	PIXEL  *xpixel=xmalloc(sizeof(PIXEL)*palette->num_entries);
	unsigned long *ypixel=xmalloc(sizeof(unsigned long)*n_allocated_pixels);

#if X_DISPLAY_DEPTH == 0
        extern PIXEL  real_pixel1[];
        extern PIXEL2 real_pixel2[];
        extern PIXEL4 real_pixel4[];
        extern long   real_pixel[];
        extern BYTE   shade_table[];
	PIXEL  my_real_pixel1[256];
	PIXEL2 my_real_pixel2[256];
	PIXEL4 my_real_pixel4[256];
	long   my_real_pixel[256];
	BYTE   my_shade_table[256];

	/* save pixels */
	memcpy(my_real_pixel, real_pixel, sizeof(my_real_pixel));
	memcpy(my_real_pixel1, real_pixel1, sizeof(my_real_pixel1));
	memcpy(my_real_pixel2, real_pixel2, sizeof(my_real_pixel2));
	memcpy(my_real_pixel4, real_pixel4, sizeof(my_real_pixel4));
	memcpy(my_shade_table, shade_table, sizeof(my_shade_table));
#endif

	/* save the list of already allocated X pixel values */
	nallocp = n_allocated_pixels;
	memcpy(ypixel, allocated_pixels, sizeof(unsigned long)*nallocp);
	n_allocated_pixels = 0;

	if ( do_alloc_colors(palette, xpixel, 1) ) {	/* failed */

	    /* restore list of previously allocated X pixel values */
	    n_allocated_pixels = nallocp;
	    memcpy(allocated_pixels, ypixel, sizeof(unsigned long)*nallocp);

#if X_DISPLAY_DEPTH == 0
	    memcpy(real_pixel, my_real_pixel, sizeof(my_real_pixel));
	    memcpy(real_pixel1, my_real_pixel1, sizeof(my_real_pixel1));
	    memcpy(real_pixel2, my_real_pixel2, sizeof(my_real_pixel2));
	    memcpy(real_pixel4, my_real_pixel4, sizeof(my_real_pixel4));
	    memcpy(shade_table, my_shade_table, sizeof(my_shade_table));
#endif
	    log_error(ui_log, "Cannot allocate enough colors.");
	} else {					/* successful */
	    /* copy the new return values to the real return values */
	    memcpy(pixel_return, xpixel, sizeof(PIXEL) * palette->num_entries);

	    /* free the previously allocated pixel values */
            XFreeColors(display, colormap, ypixel, nallocp, 0);
	}
	free(xpixel);

        return 0;
    }

    return alloc_colors(palette, pixel_return);
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
                XtRealizeWidget(app_shells[i].drive_widgets[j].led);
                XtManageChild(app_shells[i].drive_widgets[j].led);
                XtUnrealizeWidget(app_shells[i].drive_widgets[j].led1);
                XtUnrealizeWidget(app_shells[i].drive_widgets[j].led2);
	    } else {
                XtUnrealizeWidget(app_shells[i].drive_widgets[j].led);
                XtRealizeWidget(app_shells[i].drive_widgets[j].led1);
                XtManageChild(app_shells[i].drive_widgets[j].led1);
                XtRealizeWidget(app_shells[i].drive_widgets[j].led2);
                XtManageChild(app_shells[i].drive_widgets[j].led2);
	    }
        }
	/* ...and hide the rest until `NUM_DRIVES' */
	if (! true_emu)
	    num = j = 0;	/* hide all label+led widgets in normal mode */
	
        for (; j < NUM_DRIVES; j++) {
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].track_label);
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].led);
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].led1);
            XtUnrealizeWidget(app_shells[i].drive_widgets[j].led2);
        }
        for (j = 0; j < NUM_DRIVES; j++)
            app_shells[i].drive_mapping[j] = drive_mapping[j];
    }
    /* now update all image names from the cached names */
    ui_display_drive_current_image2();
}
	
void ui_display_drive_track(int drive_number, double track_number)
{
    int i;
    char str[256];

    sprintf(str, "%d: Track %.1f", drive_number + 8, (double)track_number);
    for (i = 0; i < num_app_shells; i++) {
        int n = app_shells[i].drive_mapping[drive_number];
	Widget w;
	if (n < 0)
	    return;		/* bad mapping */
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
	    return;		/* bad mapping */
	
        pixel = status ? (drive_active_led[drive_number] ? drive_led_on_green_pixel : drive_led_on_red_pixel) : drive_led_off_pixel;
	w = app_shells[i].drive_widgets[n].led;
        XtVaSetValues(w, XtNbackground, pixel, NULL);

        pixel = (status & 1) ? (drive_active_led[drive_number] ? drive_led_on_green_pixel : drive_led_on_red_pixel) : drive_led_off_pixel;
	w = app_shells[i].drive_widgets[n].led1;
        XtVaSetValues(w, XtNbackground, pixel, NULL);

        pixel = (status & 2) ? (drive_active_led[drive_number] ? drive_led_on_green_pixel : drive_led_on_red_pixel) : drive_led_off_pixel;
	w = app_shells[i].drive_widgets[n].led2;
        XtVaSetValues(w, XtNbackground, pixel, NULL);
    }
}

void ui_display_drive_current_image(int drive_number, const char *image)
{
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
		continue;	/* j'th is drive not mapped */
	    
	    /* now fill the j'th widget */
	    w = app_shells[i].drive_widgets[n].current_image;

	    fname_split(&(last_attached_images[j][0]), NULL, &name);
	    XtVaSetValues(w, XtNlabel, name, NULL);
	}
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

#ifdef USE_VIDMODE_EXTENSION
    if ( use_fullscreen) {
      XClearWindow(display,XtWindow(canvas));
      return;
    }
#endif
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
    static UI_CALLBACK(##button##_callback)     \
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
    char str[1024];
    va_list ap;
    static Widget error_dialog;
    static ui_button_t button;

#ifdef USE_VIDMODE_EXTENSION
    ui_set_windowmode();
#endif
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

/* Report a message to the user.  */
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
    ui_check_mouse_cursor();
    XtDestroyWidget(XtParent(error_dialog));
    ui_dispatch_events();
    suspend_speed_eval();

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

    suspend_speed_eval();
    ui_dispatch_events();

    switch (button) {
      case UI_BUTTON_MON:
	ui_restore_mouse();
#ifdef USE_VIDMODE_EXTENSION
	ui_set_windowmode();
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

    suspend_speed_eval();
    b = ui_ask_confirmation("Extend disk image",
                            ("Do you want to extend the disk image"
                             " to 40 tracks?"));
    return (b == UI_BUTTON_YES) ? 1 : 0;
}

/* File browser. */
char *ui_select_file(const char *title,
                     char *(*read_contents_func)(const char *),
                     int allow_autostart, const char *default_dir,
                     const char *default_pattern, ui_button_t *button_return)
{
    static ui_button_t button;
    static char *ret = NULL;
    static Widget file_selector = NULL;
    XfwfFileSelectorStatusStruct fs_status;
    static char *filesel_dir = NULL;
    char *current_dir = NULL;

    /* we preserve the current directory over the invocations */
    current_dir = get_current_dir();	/* might be changed elsewhere */
    if (filesel_dir != NULL) {
	chdir(filesel_dir);
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
                  default_pattern ? default_pattern : "*", NULL);

    if (default_dir != NULL) {
        XfwfFileSelectorChangeDirectory((XfwfFileSelectorWidget) file_selector,
                                        default_dir);
    } else {
        char *newdir = get_current_dir();

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

    if (filesel_dir != NULL) {
	free(filesel_dir);
    }
    filesel_dir = get_current_dir();
    if (current_dir != NULL) {
	chdir(current_dir);
	free(current_dir);
    }

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

    XtVaSetValues(w,
		  XtNvisual, visual,
		  XtNdepth, depth,
		  XtNcolormap, colormap,
		  NULL);

    return w;
}

/* Pop up a popup shell and center it to the last visited AppShell */
void ui_popup(Widget w, const char *title, Boolean wait_popdown)
{
    Widget s = NULL;

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
#ifdef USE_VIDMODE_EXTENSION
    focus_window_again();
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

UI_CALLBACK(exposure_callback)
{
    Dimension width, height;

    suspend_speed_eval();
    XtVaGetValues(w, XtNwidth, (XtPointer) & width,
		  XtNheight, (XtPointer) & height, NULL);

    if (client_data != NULL)
        ((ui_exposure_handler_t) client_data)((unsigned int)width,
                                              (unsigned int)height);
}

/* FIXME: this does not handle multiple application shells. */
static void close_action(Widget w, XEvent * event, String * params,
                         Cardinal * num_params)
{
    suspend_speed_eval();

    ui_exit();
}

/* ------------------------------------------------------------------------- */

static int is_paused = 0;

static void pause_trap(ADDRESS addr, void *data)
{
    ui_display_paused(1);
    is_paused = 1;
    suspend_speed_eval();
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


#ifdef USE_VIDMODE_EXTENSION
int ui_set_windowmode(void)
{
    if (use_fullscreen) {
        set_fullscreen(0);
	ui_update_menus();
	return(1);
    }
    return(0);
}

int ui_set_fullscreenmode(void)
{
    if (!use_fullscreen) {
        set_fullscreen((resource_value_t) 1);
	ui_update_menus();
	return(0);
    }
    return(1);
}

void ui_set_fullscreenmode_init(void)
{
    set_bestmode(selected_videomode_at_start);
    if (selected_videomode_index == -1 && bestmode_counter > 0)
        selected_videomode_index = bestmodes[0].modeindex;
    if (use_fullscreen_at_start) {
        ui_set_fullscreenmode();
	XtVaSetValues(XtParent(XtParent(canvas)),
		      XtNx,          0,
		      XtNy,          0,
		      None);
	XWarpPointer(display, None,
		     XtWindow(canvas),
		     0, 0, 0, 0, 0, 0);
    }
    ui_update_menus();
}

int ui_is_fullscreen_available(void)
{
    return(bestmode_counter?1:0);
}
#endif

