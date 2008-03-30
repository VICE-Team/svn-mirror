/*
 * ui.c - Gnome/Gtk+ based UI
 *
 * Written by
 *  Ettore Perazzoli
 *  Oliver Schaertel
 *  Martin Pottendorfer
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

#define _UI_C

#include "vice.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <gnome.h>
#include <gdk/gdkx.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <X11/keysym.h>

#ifdef HAVE_X11_SUNKEYSYM_H
#include <X11/Sunkeysym.h>
#endif

#ifdef USE_VIDMODE_EXTENSION
#define VidMode_MINMAJOR 0
#define VidMode_MINMINOR 0

#include <X11/extensions/xf86vmode.h>
#endif

#include "ui.h"
#include "uiarch.h"

#include "cmdline.h"
#include "datasette.h"
#include "interrupt.h"
#include "log.h"
#include "kbd.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "palette.h"
#include "resources.h"
#include "uihotkey.h"
#include "uimenu.h"
#include "uisettings.h"
#include "uicommands.h"
#include "utils.h"
#include "version.h"
#include "vsidproc.h"
#include "vsync.h"


/* FIXME: We want these to be static.  */
Display *display;
int screen;
GdkVisual *visual;
int depth = X_DISPLAY_DEPTH;
int have_truecolor;
char last_attached_images[NUM_DRIVES][256]; /* FIXME MP */

static int n_allocated_pixels = 0;
static unsigned long allocated_pixels[0x100];

/* UI logging goes here.  */
static log_t ui_log = LOG_ERR;

static GtkTooltips *tape_tooltip;
static GtkTooltips *drive_tooltips[NUM_DRIVES];

static int tape_motor_status = -1;
static int tape_control_status = -1;

#define LED_WIDTH 12
#define LED_HEIGHT 6
#define CTRL_WIDTH 13
#define CTRL_HEIGHT 11

static GtkWidget *drive8menu, *drive9menu, *tape_menu;
static GtkWidget *status_bar;
static GdkCursor *blankCursor;
static GtkWidget *image_preview_list;
static char *(*current_image_contents_func)(const char *);
static GdkFont *fixedfont;
/* FIXME, ask Xresources here */
static char *fixedfontname="-*-lucidatypewriter-medium-r-*-*-12-*";	
static char *current_image_contents;

static int cursor_is_blank = 0;

#ifdef USE_VIDMODE_EXTENSION
int vidmodeavail = 0;
int use_fullscreen = 0;
int use_fullscreen_at_start = 0;

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

/* ------------------------------------------------------------------------- */

ui_resources_t _ui_resources;

#ifdef USE_VIDMODE_EXTENSION

UI_MENU_DEFINE_STRING_RADIO(SelectedFullscreenMode);

void mouse_timeout(int signo) {
    if(use_fullscreen && !cursor_is_blank) XDefineCursor(display,XtWindow(canvas), blankCursor);
}

void ui_set_mouse_timeout() {
    if(!use_fullscreen) return;
    cursor_is_blank = 0;
    if (signal(SIGALRM, mouse_timeout) == SIG_ERR)
        return;
    alarm(5);
}

void mouse_handler(Widget w, XtPointer client_data, XEvent *report,
                       Boolean *ctd) {
    if(report->type == LeaveNotify)
        cursor_is_blank = 1;
    else
        cursor_is_blank = 0;

    if (! _mouse_enabled) {
        XUndefineCursor(display,XtWindow(canvas));
	if(cursor_is_blank == 0) {
	    if (signal(SIGALRM, mouse_timeout) == SIG_ERR)
	      return;
	    alarm(5);
	}
    }
}

int vidmode_available(void)
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

    log_message(LOG_DEFAULT, "Available fullscreen video modes: ");

    for (i = 0; i < vidmodecount; i++) {
        if(allmodes[i]->hdisplay <= 800 &&
	   allmodes[i]->hdisplay >= 320 &&
	   allmodes[i]->vdisplay <= 600  &&
	   allmodes[i]->hdisplay >= 200) {
	      bestmodes[bestmode_counter].modeindex=i;
	      hz = allmodes[i]->dotclock * 1000 /
		( allmodes[i]->vtotal * allmodes[i]->htotal) ;
	      snprintf(bestmodes[bestmode_counter].name,17," %ix%i-%iHz",
		       allmodes[i]->hdisplay,
		       allmodes[i]->vdisplay,
		       hz);
	      log_message(LOG_DEFAULT,"%ix%i-%iHz",
			  allmodes[i]->hdisplay,
			  allmodes[i]->vdisplay,
			  hz);
	      bestmode_counter++;
	      if (bestmode_counter == 10) break;
	}
    }
    return 1;
}

void focus_window_again() {
    if(!use_fullscreen) return;
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
    static int dotclock;
    static int window_doublesize;
    static int panecolor;
    int i;

    if( !vidmodeavail || !bestmode_counter ) {
        use_fullscreen_at_start = (int) v;
	return 0;
    }
    if( selected_videomode_index == -1) {
	return 0;
    }

    if(v && !use_fullscreen) {
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
    } else if((int) v == 2) {
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
    } else if(use_fullscreen) {
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
	    if(allmodes[i]->hdisplay == restoremodeline.hdisplay &&
	       allmodes[i]->vdisplay == restoremodeline.vdisplay &&
	       allmodes[i]->dotclock == dotclock ) {
	      XF86VidModeSwitchToMode(display, screen, allmodes[i]);
	      break;
	    }
	}

	video_setfullscreen(0,0,0);

	if(use_fullscreen_at_start) {
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

	if (resources_get_value("DoubleSize",
                            (resource_value_t *) &ds) < 0);

	if(ds < window_doublesize) {
	  w -= canvas_width/2;
	  h -= canvas_height/2;
	} else if(ds > window_doublesize) {
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
    if(!vidmodeavail) {
      selected_videomode_at_start = (char*) stralloc(v);
      return(0);
    }
    selected_videomode = (char*) v;
    for (i = 0; i < bestmode_counter; i++) {
        if(! strcmp(selected_videomode, bestmodes[i].name)) {
	    selected_videomode_index = bestmodes[i].modeindex;
	    if(use_fullscreen) set_fullscreen((resource_value_t) 2);
	    return(0);
	}
    }
    if(bestmode_counter > 0) {
      selected_videomode_index = bestmodes[0].modeindex;
      selected_videomode = bestmodes[0].name;
    }
    return(0);
}

static UI_CALLBACK(ui_set_bestmode)
{
    set_bestmode(client_data);
}

#endif

void ui_check_mouse_cursor()
{
    int window_doublesize;


    if(_mouse_enabled) {
#ifdef USE_VIDMODE_EXTENSION
        if(use_fullscreen) { 
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
	/*	XDefineCursor(display,XtWindow(canvas), blankCursor);*/
	cursor_is_blank = 1;
	gdk_keyboard_grab(_ui_top_level->window,
			  1,
			  CurrentTime);
	gdk_pointer_grab(_ui_top_level->window,
			 1,
			 GDK_POINTER_MOTION_MASK |
			 GDK_BUTTON_PRESS_MASK |			 
			 GDK_BUTTON_RELEASE_MASK,
			 _ui_top_level->window,
			 blankCursor,
			 CurrentTime);
    }
    else if (cursor_is_blank) {
      /*        XUndefineCursor(display,XtWindow(canvas));*/
        gdk_keyboard_ungrab(CurrentTime);
        gdk_pointer_ungrab(CurrentTime);
#ifdef USE_VIDMODE_EXTENSION
	if(use_fullscreen)
	  ui_set_mouse_timeout();
#endif
    }
}

void ui_restore_mouse() {
    if(_mouse_enabled && cursor_is_blank) {
	/*        XUndefineCursor(display,XtWindow(canvas));*/
        gdk_keyboard_ungrab(CurrentTime);
        gdk_pointer_ungrab(CurrentTime);
	cursor_is_blank = 0; 
    }
}


void initBlankCursor() {
  /*  static unsigned char no_data[] = { 0,0,0,0, 0,0,0,0 };
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
  */
  /*    static GdkPixmap blank;*/
    
    blankCursor = gdk_cursor_new(GDK_MOUSE);
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

static GtkWidget *left_menu, *right_menu;

/* Translations for the left and right menus.  */
/*
static XtTranslations left_menu_translations, right_menu_translations;
*/
/* Application context. */
/*
static XtAppContext app_context;
*/

/* Toplevel widget. */
GtkWidget * _ui_top_level;
GdkGC *app_gc;

/* Our colormap. */
static GdkColormap *colormap;

/* Enabled drives.  */
ui_drive_enable_t enabled_drives;

/* Color of the drive active LED.  */
int *drive_active_led;

/* Drive status widget */
typedef struct 
{
    GtkWidget *box;			/* contains all the widgets */
    char *label;
    GtkWidget *pixmap;
#if 0
    GtkWidget *image;
#endif
    GtkWidget *event_box;
    GtkWidget *track_label;
    GtkWidget *led;
    GdkPixmap *led_pixmap;
} drive_status_widget;

/* Tape status widget */
typedef struct 
{
    GtkWidget *box;
    GtkWidget *event_box;
    GtkWidget *label;
    GtkWidget *control;
    GdkPixmap *control_pixmap;
} tape_status_widget;

/* This allows us to pop up the transient shells centered to the last visited
   shell. */
static GtkWidget *last_visited_app_shell = NULL;
#define MAX_APP_SHELLS 10
typedef struct {
    String title;
    GtkWidget *shell;
    GtkWidget *canvas;
    GtkWidget *topmenu;
    GtkLabel *speed_label;
    drive_status_widget drive_status[NUM_DRIVES];
    tape_status_widget tape_status;
} app_shell_type;

static app_shell_type app_shells[MAX_APP_SHELLS];
static int num_app_shells = 0;

/* Pixels for updating the drive LED's state.  */
GdkColor *drive_led_on_red_pixel, *drive_led_on_green_pixel, 
    *drive_led_off_pixel, *motor_running_pixel, *tape_control_pixel;

/* If != 0, we should save the settings. */
/* static int resources_have_changed = 0; */

/* ------------------------------------------------------------------------- */

static int alloc_colormap(void);
static int alloc_colors(const palette_t *palette, PIXEL pixel_return[]);
static GtkWidget* build_file_selector(ui_button_t *button_return);
static GtkWidget* build_error_dialog(ui_button_t *button_return,
				     const String message);
static GtkWidget* build_input_dialog(ui_button_t *button_return,
				     GtkWidget **InputDialogLabel,
				     GtkWidget **InputDialogField);
static GtkWidget* build_show_text(ui_button_t *button_return,
				  const String text, int width, int height);
static GtkWidget* build_confirm_dialog(ui_button_t *button_return,
				       GtkWidget **confirm_dialog_message);
UI_CALLBACK(enter_window_callback);
UI_CALLBACK(exposure_callback);

/* ------------------------------------------------------------------------- */
#if 0
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
    "*speedStatus.font:                         -*-helvetica-medium-r-*-*-12-*",

    NULL
};
#endif
/* ------------------------------------------------------------------------- */

void ui_proc_start(void)
{
    signal(SIGINT,  SIG_IGN);
    signal(SIGTERM, SIG_DFL);

    gtk_main();
}

void archdep_ui_init(int argc, char *argv[])
{
    /* Fake Gnome to see empty arguments; 
       Generaly we should use a `popt_table', either by converting the
       registered options to this, or to introduce popt in the generic part,
       case we have `libgnomeui' around.
       For now I discard gnome-specific options. FIXME MP */
    char *fake_argv[2];

    if (console_mode) {
        return;
    }

    fake_argv[0] = argv[0];
    fake_argv[1] = NULL;
    gnome_init(PACKAGE, VERSION, 1, fake_argv);
}

/* Initialize the GUI and parse the command line. */
int ui_init(int *argc, char **argv)
{
    display = GDK_DISPLAY();

    screen =  XDefaultScreen(display);
    atexit(ui_autorepeat_on);

    ui_hotkey_init();

    enabled_drives = UI_DRIVE_ENABLE_NONE;

    return 0;
}

typedef struct {
    char *name;
    GdkVisualType class;
} namedvisual_t;


void delete_event(GtkWidget *w, GdkEvent *e, gpointer data) 
{
    suspend_speed_eval();
    ui_exit();
}

void mouse_handler(GtkWidget *w, GdkEvent *event, gpointer data)
{
    if(event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	if(_mouse_enabled) {
	    mouse_button(bevent->button-1,TRUE);
	} else {
	    if(bevent->button == 1) {
	        ui_menu_update_all_GTK();
		gtk_menu_popup(GTK_MENU(left_menu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	    } else if(bevent->button == 3) {
  	        ui_menu_update_all_GTK();
		gtk_menu_popup(GTK_MENU(right_menu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	    }
	}
    } else if (event->type == GDK_BUTTON_RELEASE && _mouse_enabled) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	mouse_button(bevent->button-1,FALSE);
    } else if (event->type == GDK_MOTION_NOTIFY && _mouse_enabled) {
        GdkEventMotion *mevent = (GdkEventMotion*) event;
        mouse_move(mevent->x,mevent->y);
    }
}

void fliplist_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    if(event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	if ((int) data == 0) 
	{
	    ui_update_flip_menus(8, 8);
	    if (drive8menu)
		gtk_menu_popup(GTK_MENU(drive8menu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	}
	if ((int) data == 1)
	{
	    ui_update_flip_menus(9, 9);
	    if (drive9menu)
		gtk_menu_popup(GTK_MENU(drive9menu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	}
    }
}

static void tape_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    if(event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	
	if (tape_menu)
	    gtk_menu_popup(GTK_MENU(tape_menu),NULL,NULL,NULL,NULL,
			   bevent->button, bevent->time);
    }
}

void update_menu_cb(GtkWidget *w, GdkEvent *event,gpointer data)
{
    ui_menu_update_all_GTK();
}

static void filesel_autostart_cb(GtkWidget *w, gpointer data)
{
    GList *selected, *tmp;
    char *s, *t1, *t2;
    int num;
    int found = FALSE;
    
    *((ui_button_t *)data) = UI_BUTTON_AUTOSTART;

    g_return_if_fail(GTK_IS_LIST(image_preview_list));
    g_return_if_fail(current_image_contents != NULL);
    
    selected = GTK_LIST(image_preview_list)->selection;
    if (! selected || !selected->data)
	return;

    g_return_if_fail(GTK_IS_LIST_ITEM(selected->data));
    tmp = gtk_container_children(GTK_CONTAINER(selected->data));
    gtk_label_get(GTK_LABEL(tmp->data), &s);

    /* now find program number from earlier read contents */
    num = 0;
    t1 = t2 = current_image_contents;
    t1 = find_next_line(NULL, t2);
    while (t1 > t2)
    {
	*(t1 - 1) = '\0';
	if (strcmp(t2, s) == 0)
	{
	    found = TRUE;
	    break;
	}
	num++;
	t2 = t1;
	t1 = find_next_line(NULL, t2);
    }

    if (found == FALSE)
    {
	/* last line without \n */
	if (strcmp(t1, s) == 0)
	    ui_set_selected_file(num);
    }
    else
	ui_set_selected_file(num);

    free(current_image_contents);
    current_image_contents = NULL;
    printf("autostart-select: `%s', %d.\n", s, num);
}


/* Continue GUI initialization after resources are set. */
int ui_init_finish(void)
{
    static namedvisual_t classes[] = {
	{ "PseudoColor", GDK_VISUAL_PSEUDO_COLOR },
	{ "TrueColor", GDK_VISUAL_TRUE_COLOR },
	{ "StaticGray", GDK_VISUAL_GRAYSCALE },
	{ NULL }
    };

    if (ui_log == LOG_ERR)
        ui_log = log_open("X11");

    if (depth != 0) {
	int i;

	for (i = 0; classes[i].name != NULL ; i++) {
	    if ((visual = gdk_visual_get_best_with_both(depth,
							classes[i].class)))
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
            have_truecolor = (classes[i].class == GDK_VISUAL_TRUE_COLOR);
        }
    } else {
	/* Autodetect. */
        int j, done;
	int sdepth;

	sdepth = DefaultDepth(display, screen);

	done = 0;
	for (j = 0; classes[j].name != NULL; j++) {
	  if ( (visual = gdk_visual_get_best_with_both(sdepth,
						       classes[j].class)) ||
	       (visual = gdk_visual_get_best_with_type(classes[j].class)) )
	    {
	        depth = visual->depth;
		log_message(ui_log, "Found %dbit/%s visual.",
			    depth, classes[j].name);
		have_truecolor = (classes[j].class == GDK_VISUAL_TRUE_COLOR);
		done = 1;
		break;
	    }
	}
	if (!done) {
	    log_error(ui_log, "Cannot autodetect a proper visual.");
	    return -1;
	}
    }

    fixedfont = gdk_font_load(fixedfontname);
    if (!fixedfont)
	log_error(ui_log, "Cannot load font %s.", fixedfontname);
    return ui_menu_init();
}

void ui_create_status_bar(GtkWidget *pane, int width, int height)
{
    /* Create the status bar on the bottom.  */
    GtkWidget *speed_label, *drive_box, *frame;
    int i;
    app_shell_type *as;

    status_bar = gtk_hbox_new(FALSE, 0);

    gtk_container_add(GTK_CONTAINER(pane),status_bar);
    gtk_widget_show(status_bar);

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(status_bar),frame,TRUE,TRUE,0);
    gtk_widget_show(frame);
    
    speed_label = gtk_label_new("");
    app_shells[num_app_shells - 1].speed_label = (GtkLabel*) speed_label;
    gtk_misc_set_alignment (GTK_MISC (speed_label), 0, -1);
    gtk_container_add(GTK_CONTAINER(frame), speed_label);
    gtk_widget_show(speed_label);      

    as=&app_shells[num_app_shells - 1];
    /* drive stuff */
    drive_box = gtk_hbox_new(FALSE, 0);
    for (i = 0; i < NUM_DRIVES; i++) {
	char label[10];
	
	as->drive_status[i].event_box = gtk_event_box_new();

	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
	
	sprintf(label, "Drive %d: ", i + 8);
	as->drive_status[i].box = gtk_hbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame),
			  as->drive_status[i].box);
	gtk_widget_show(as->drive_status[i].box);

	gtk_container_add(GTK_CONTAINER(as->drive_status[i].event_box),
			  frame);
	gtk_widget_show(frame);

	drive_tooltips[i] = gtk_tooltips_new();
	gtk_tooltips_set_tip(GTK_TOOLTIPS(drive_tooltips[i]),
			     as->drive_status[i].box->parent->parent,
			     "<empty>", NULL);

	/* Label */
	as->drive_status[i].label = (void *)gtk_label_new(g_strdup(label));
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box), 
			   (GtkWidget *)as->drive_status[i].label,
			   TRUE,TRUE,0);
	gtk_widget_show((GtkWidget *)as->drive_status[i].label);

#if 0
	as->drive_status[i].image = (void *)gtk_label_new("<empty>");
	gtk_container_add(GTK_CONTAINER(event_box),
			  as->drive_status[i].image);
	gtk_widget_show(as->drive_status[i].image);
#endif
	
	/* Track Label */
	as->drive_status[i].track_label = gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box),
			   as->drive_status[i].track_label,
			   FALSE, FALSE, 0);
	gtk_widget_show(as->drive_status[i].track_label);      
	

	/* Led */
	as->drive_status[i].led_pixmap = 
	    gdk_pixmap_new(_ui_top_level->window, LED_WIDTH, LED_HEIGHT, 
			   depth);
	as->drive_status[i].led = 
	    gtk_pixmap_new(as->drive_status[i].led_pixmap, NULL);
	gtk_widget_set_usize(as->drive_status[i].led, LED_WIDTH, LED_HEIGHT);
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box),
			   (GtkWidget *)as->drive_status[i].led,
			   FALSE,FALSE, 4);
	gtk_widget_show(as->drive_status[i].led);      
	
	gtk_box_pack_start(GTK_BOX(drive_box), as->drive_status[i].event_box, 
			   FALSE, FALSE, 0);

	gtk_widget_set_events(as->drive_status[i].event_box, 
			      GDK_BUTTON_PRESS_MASK |
			      GDK_BUTTON_RELEASE_MASK |
	                      GDK_ENTER_NOTIFY_MASK);
	gtk_signal_connect(GTK_OBJECT(as->drive_status[i].event_box),
			   "button-press-event",
			   GTK_SIGNAL_FUNC(fliplist_popup_cb), (gpointer) i);
	gtk_widget_show(as->drive_status[i].event_box);
    }
    gtk_widget_show(drive_box);
    gtk_box_pack_start(GTK_BOX(status_bar), drive_box, FALSE, FALSE, 0);

    /* tape stuff */
    as->tape_status.event_box = gtk_event_box_new();

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);

    as->tape_status.box = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(frame),
		      as->tape_status.box);
    gtk_widget_show(as->tape_status.box);

    gtk_container_add(GTK_CONTAINER(as->tape_status.event_box), frame);
    gtk_widget_show(frame);

    tape_tooltip = gtk_tooltips_new();
    gtk_tooltips_set_tip(GTK_TOOLTIPS(tape_tooltip),
			 as->tape_status.box->parent->parent, 
			 "", NULL);
    /* Tape Label */
    as->tape_status.label = gtk_label_new("Tape 000");
    gtk_container_add(GTK_CONTAINER(as->tape_status.box),
		      as->tape_status.label);
    gtk_misc_set_alignment (GTK_MISC (as->tape_status.label), 0, -1);
    gtk_widget_show(as->tape_status.label);

    /* Tape control */
    as->tape_status.control_pixmap = 
	gdk_pixmap_new(_ui_top_level->window, CTRL_WIDTH, CTRL_HEIGHT, depth);
    as->tape_status.control = gtk_pixmap_new(as->tape_status.control_pixmap, 
					      NULL);
    gtk_widget_set_usize(as->tape_status.control, CTRL_WIDTH, CTRL_HEIGHT);
    gtk_box_pack_start(GTK_BOX(as->tape_status.box), as->tape_status.control,
		       FALSE, FALSE, 4);
    gtk_widget_show(as->tape_status.control);

    gtk_widget_set_events(as->tape_status.event_box, 
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK | 
			  GDK_ENTER_NOTIFY_MASK);
    gtk_signal_connect(GTK_OBJECT(as->tape_status.event_box),
		       "button-press-event",
		       GTK_SIGNAL_FUNC(tape_popup_cb), (gpointer) NULL);

    gtk_box_pack_start(GTK_BOX(status_bar), as->tape_status.event_box, 
		       FALSE, FALSE, 0);
    gtk_widget_show(as->tape_status.event_box);
    gtk_widget_show(status_bar);

    for (i = 0; i < NUM_DRIVES; i++) {
#if 0
	int ih, iw;
	gdk_window_get_size(((GtkWidget *)as->drive_status[i].image)->window, 
			    &iw, &ih);
	gtk_widget_set_usize(as->drive_status[i].image, width / 3, ih);
#endif
	gtk_widget_hide(as->drive_status[i].event_box);	/* Hide Tape widget */
	gdk_window_set_cursor (as->drive_status[i].event_box->window, 
			       gdk_cursor_new (GDK_HAND1)); 
    }
    gtk_widget_hide(as->tape_status.event_box);	/* Hide Tape widget */
    gdk_window_set_cursor (as->tape_status.event_box->window, 
			   gdk_cursor_new (GDK_HAND1)); 
}


void kbd_event_handler(GtkWidget *w, GdkEvent *report,gpointer gp);

/* Create a shell with a canvas widget in it.  */
ui_window_t ui_open_canvas_window(const char *title, int width, int height,
                                  int no_autorepeat,
                                  ui_exposure_handler_t exposure_proc,
                                  const palette_t *palette,
                                  PIXEL pixel_return[])
{
    GtkWidget *new_window, *new_pane, *new_canvas, *topmenu;
    
    if (++num_app_shells > MAX_APP_SHELLS) {
	log_error(ui_log, "Maximum number of toplevel windows reached.");
	return NULL;
    }

    new_window = gnome_app_new(PACKAGE, PACKAGE);
    if (!_ui_top_level)
	_ui_top_level = new_window;
    
    new_pane = gtk_vbox_new(FALSE, 0);
    gnome_app_set_contents(GNOME_APP(new_window), new_pane);
    gtk_widget_show(new_pane);
    
    topmenu = gtk_menu_bar_new();
    gtk_widget_show(topmenu);
    gtk_signal_connect(GTK_OBJECT(topmenu),"button-press-event",
		       GTK_SIGNAL_FUNC(update_menu_cb),NULL);
    gnome_app_set_menus(GNOME_APP(new_window), GTK_MENU_BAR(topmenu));

    new_canvas = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(new_pane),new_canvas,FALSE,FALSE,0);
    
    gtk_widget_show(new_canvas);
    gtk_widget_set_events(new_canvas,
			  GDK_LEAVE_NOTIFY_MASK |
			  GDK_ENTER_NOTIFY_MASK |			  
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_KEY_PRESS_MASK |
			  GDK_KEY_RELEASE_MASK |
			  GDK_FOCUS_CHANGE_MASK |
			  GDK_POINTER_MOTION_MASK |
			  GDK_EXPOSURE_MASK);

    gtk_widget_show(new_window);

#ifdef USE_VIDMODE_EXTENSION
    vidmodeavail = vidmode_available();
#endif

    gtk_signal_connect(GTK_OBJECT(new_canvas),"expose-event",
		       GTK_SIGNAL_FUNC(exposure_callback),
		       (void*) exposure_proc);
    gtk_widget_set_usize(new_canvas, width, height);
    gtk_widget_show(new_canvas);
    gtk_widget_queue_resize(new_canvas);

    ui_create_status_bar(new_pane, width, height);

    if (no_autorepeat) {
      gtk_signal_connect(GTK_OBJECT(new_window),"enter-notify-event",
	 		 GTK_SIGNAL_FUNC(ui_autorepeat_off),NULL);
      gtk_signal_connect(GTK_OBJECT(new_window),"leave-notify-event",
	 		 GTK_SIGNAL_FUNC(ui_autorepeat_on),NULL);
    }
    gtk_signal_connect(GTK_OBJECT(new_window),"key-press-event",
		       GTK_SIGNAL_FUNC(kbd_event_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"key-release-event",
		       GTK_SIGNAL_FUNC(kbd_event_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"enter-notify-event",
		       GTK_SIGNAL_FUNC(kbd_event_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"leave-notify-event",
		       GTK_SIGNAL_FUNC(kbd_event_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"button-press-event",
		       GTK_SIGNAL_FUNC(mouse_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"button-release-event",
		       GTK_SIGNAL_FUNC(mouse_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"motion-notify-event",
		       GTK_SIGNAL_FUNC(mouse_handler),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"delete_event",
		       GTK_SIGNAL_FUNC(delete_event),NULL);
    gtk_signal_connect(GTK_OBJECT(new_window),"destroy_event",
		       GTK_SIGNAL_FUNC(delete_event),NULL);

    app_shells[num_app_shells - 1].shell = new_window;
    app_shells[num_app_shells - 1].canvas = new_canvas;
    app_shells[num_app_shells - 1].title = stralloc(title);
    app_shells[num_app_shells - 1].topmenu = topmenu;

    gtk_window_set_title(GTK_WINDOW(new_window),title);

    alloc_colormap();
    if (!app_gc)
	app_gc = gdk_gc_new(new_window->window);
    if (alloc_colors(palette, pixel_return) == -1)
        return NULL;

    /* This is necessary because the status might have been set before we
       actually open the canvas window. e.g. by commandline */
    ui_enable_drive_status(enabled_drives, drive_active_led);

    initBlankCursor();

    return new_canvas;
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
    if(bestmode_counter > 0)
        ui_fullscreen_settings_submenu[5].sub_menu = resolutions_submenu;
#endif

}

#ifndef GNOME_MENUS
/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(GtkWidget *w)
{
    left_menu = w;
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(GtkWidget *w)
{
    right_menu = w;
}

void ui_set_topmenu(void)
{
    int i;
    GtkWidget *commands, *settings, *help, *help_menu;
    
    for (i = 0; i < num_app_shells; i++)
    {
	commands = gtk_menu_item_new_with_label("Commands");
	gtk_widget_show(commands);
	settings = gtk_menu_item_new_with_label("Settings");
	gtk_widget_show(settings);
    
	help_menu = ui_menu_create("Help", ui_help_commands_menu, NULL);
	help = gtk_menu_item_new_with_label("Help");
	gtk_widget_show(help);

	gtk_menu_item_set_submenu(GTK_MENU_ITEM(commands), left_menu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings), right_menu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), help_menu);
	gtk_menu_item_right_justify(GTK_MENU_ITEM(help));

	gtk_menu_bar_append(GTK_MENU_BAR(app_shells[i].topmenu), commands);
	gtk_menu_bar_append(GTK_MENU_BAR(app_shells[i].topmenu), settings);
	gtk_menu_bar_append(GTK_MENU_BAR(app_shells[i].topmenu), help);
	gtk_widget_show(app_shells[i].topmenu);
    }
}

#else  /* GNOME_MENUS */

/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(GnomeUIInfo *w)
{
    left_menu = gtk_menu_new();
    main_menu[0].type = GNOME_APP_UI_SUBTREE;
    main_menu[0].label = "Commands";
    main_menu[0].moreinfo = w;
    
    gnome_app_fill_menu(GTK_MENU_SHELL(left_menu), w, NULL, FALSE, 0);
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(GnomeUIInfo *w)
{
    right_menu = gtk_menu_new();
    main_menu[1].type = GNOME_APP_UI_SUBTREE;
    main_menu[1].label = "Settings";
    main_menu[1].moreinfo = w;
    
    gnome_app_fill_menu(GTK_MENU_SHELL(right_menu), w, NULL, FALSE, 0);
}

void ui_set_topmenu(void)
{
    main_menu[2].type = GNOME_APP_UI_ENDOFINFO;
    gnome_app_create_menus(GNOME_APP(XXXX), main_menu);
}
#endif /* !GNOME_MENUS */

void ui_set_application_icon(const char *icon_data[])
{
    int i;
    GdkPixmap *icon;
    
    icon = gdk_pixmap_create_from_xpm_d(_ui_top_level->window,
					NULL, NULL, (char **) icon_data);
    
    for (i = 0; i < num_app_shells; i++)
        gdk_window_set_icon(app_shells[i].shell->window, NULL,
			    icon, NULL);
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
	ui_dispatch_events();
	if (psid_mode) 
	    ui_proc_exit();
	else 
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
        colormap = gdk_colormap_get_system();
    } else {
        log_message(ui_log, "Using private colormap.");
	colormap = gdk_colormap_new(visual, AllocNone);
    }

    gdk_window_set_colormap(_ui_top_level->window,colormap);

    return 0;
}

/* Allocate colors in the colormap. */
static int do_alloc_colors(const palette_t *palette, PIXEL pixel_return[],
                           int releasefl)
{
    int i, failed;
    GdkColor color;
    XImage *im;
    PIXEL *data = (PIXEL *)xmalloc(4);


    /* This is a kludge to map pixels to zimage values. Is there a better
       way to do this? //tvr */
      /*    im = gdk_image_new(GDK_IMAGE_NORMAL,visual,1,8);*/
    im = XCreateImage(display,GDK_VISUAL_XVISUAL(visual),depth,
		      ZPixmap, 0, (char *)data, 1, 1, 8, 0);
    if (!im)
        return -1;

    n_allocated_pixels = 0;

    for (i = 0, failed = 0; i < palette->num_entries; i++) {
        color.red = palette->entries[i].red << 8;
        color.green = palette->entries[i].green << 8;
        color.blue = palette->entries[i].blue << 8;
        if (!gdk_color_alloc(colormap, &color)) {
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
        gdk_colors_free(colormap, allocated_pixels, n_allocated_pixels, 0);
	n_allocated_pixels = 0;
    }

    XDestroyImage(im);

    if (!failed) {
        GdkColor* exact;
	exact = (GdkColor*) xmalloc(sizeof(GdkColor));
	exact->red = 0;
	exact->green = 0;
	exact->blue = 0;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
            drive_led_off_pixel = exact;
            allocated_pixels[n_allocated_pixels++] = exact->pixel;
        }

	exact = (GdkColor*) xmalloc(sizeof(GdkColor));
	exact->red = 0xffff;
	exact->green = 0;
	exact->blue = 0;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
	    drive_led_on_red_pixel = exact;
	    allocated_pixels[n_allocated_pixels++] = exact->pixel;
	}

	exact = (GdkColor*) xmalloc(sizeof(GdkColor));
	exact->red = 0;
	exact->green = 0xffff;
	exact->blue = 0;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
	    drive_led_on_green_pixel = exact;
	    allocated_pixels[n_allocated_pixels++] = exact->pixel;
	}

	exact = (GdkColor*) xmalloc(sizeof(GdkColor));
	exact->red = 0xffff;
	exact->green = 0xffff;
	exact->blue = 0x7fff;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
	    motor_running_pixel = exact;
	    allocated_pixels[n_allocated_pixels++] = exact->pixel;
	}

	exact = (GdkColor*) xmalloc(sizeof(GdkColor));
	exact->red = 0xafff;
	exact->green = 0xafff;
	exact->blue = 0xafff;
        if (!gdk_color_alloc(colormap,exact))
            return 1;
        else {
	    tape_control_pixel = exact;
	    allocated_pixels[n_allocated_pixels++] = exact->pixel;
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
	if (colormap == gdk_colormap_get_system()) {
            log_warning(ui_log, "Automagically using a private colormap.");
	    colormap = gdk_colormap_new(visual, AllocNone);
	    gdk_window_set_colormap(_ui_top_level->window,colormap);
	    failed = do_alloc_colors(palette, pixel_return, 0);
	}
    }
    return failed ? -1 : 0;
}

/* Change the colormap of window `w' on the fly.  This only works for
   TrueColor visuals.  Otherwise, it would be too messy to re-allocate the
   new colormap.  */
int ui_canvas_set_palette(ui_window_t w, const palette_t *palette,
                          PIXEL *pixel_return)
{

    if (!have_truecolor) {
	int nallocp;
	PIXEL  *xpixel=malloc(sizeof(PIXEL)*palette->num_entries);
	unsigned long *ypixel=malloc(sizeof(unsigned long)*n_allocated_pixels);

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

	if( do_alloc_colors(palette, xpixel, 1) ) {	/* failed */

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
            XFreeColors(display, GDK_COLORMAP_XCOLORMAP(colormap),
			ypixel, nallocp, 0);
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
	gtk_label_set_text(app_shells[i].speed_label, warp_flag ? "(warp)" : "");
      } else {
	sprintf(str, "%d%%, %d fps %s",
		percent_int, framerate_int, warp_flag ? "(warp)" : "");
	gtk_label_set_text(app_shells[i].speed_label, str);
      }
    }
}

/* ------------------------------------------------------------------------- */
/* drive stuff */
void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
    int i, j;

    enabled_drives = enable;
    drive_active_led = drive_led_color;

    for (i = 0; i < num_app_shells; i++) {
        for (j = 0; j < NUM_DRIVES; j++) {
	    if (enabled_drives && 
		(enabled_drives & (1 << j))) 
	    {
		/* enabled + active drive */
		gtk_widget_show(app_shells[i].drive_status[j].event_box);
		gtk_widget_show(app_shells[i].drive_status[j].track_label);
		gtk_widget_show(app_shells[i].drive_status[j].led);
	    } 
	    else if (!enabled_drives &&
		       (strcmp(last_attached_images[j], "") != 0)) 
	    {
		gtk_widget_show(app_shells[i].drive_status[j].event_box);
		gtk_widget_hide(app_shells[i].drive_status[j].track_label);
		gtk_widget_hide(app_shells[i].drive_status[j].led);
	    }
	    else 
	    {
		gtk_widget_hide(app_shells[i].drive_status[j].event_box);
	    }
	}
#if 0
	/* enable this when a dynamic multiline status bar is needed.
	   be aware that x128 has weird sized windows on startup, because of 
	   the uninitialized canvas window(-size) during startup */
	/* resize according to new needs */
	{
	int width, height;

	gdk_window_get_size(app_shells[i].canvas->window, &width, &height);
	ui_resize_canvas_window(app_shells[i].canvas, width, height);
	}
#endif
    }
}

void ui_display_drive_track(int drive_number, int drive_base,
			    double track_number)
{
    int i;
    static char str[256];

    sprintf(str, "%.1f", (double)track_number);
    
    for (i = 0; i < num_app_shells; i++) {
	gtk_label_set_text(GTK_LABEL(app_shells[i].
				     drive_status[drive_number].track_label), 
			   str);
    }
}

void ui_display_drive_led(int drive_number, int status)
{
    int i;
    
    GdkColor *color = status ? (drive_active_led[drive_number] 
				? drive_led_on_green_pixel 
				: drive_led_on_red_pixel) 
	                     : drive_led_off_pixel;

    gdk_gc_set_foreground(app_gc, color);
    for (i = 0; i < num_app_shells; i++)
    {
	drive_status_widget *ds = &app_shells[i].drive_status[drive_number];
	gdk_draw_rectangle(ds->led_pixmap, app_gc, TRUE, 0, 
			   0, LED_WIDTH, LED_HEIGHT);
	gtk_widget_queue_draw(ds->led);
    }
}

void ui_display_drive_current_image(unsigned int drive_number, 
				    const char *image)
{
    int i;
    char *name;
    
    if (console_mode) {
        return;
    }

    /* FIXME: Allow more than two drives.  */
    if (drive_number >= NUM_DRIVES)
        return;

    strcpy(&(last_attached_images[drive_number][0]), image);
    fname_split(&(last_attached_images[drive_number][0]), NULL, &name);
    
    for (i = 0; i < num_app_shells; i++) {
#if 0
	gtk_label_set_text(GTK_LABEL(app_shells[i].
				     drive_status[drive_number].image), 
			   name);
#endif
	gtk_tooltips_set_tip(
	    GTK_TOOLTIPS(drive_tooltips[drive_number]),
	    app_shells[i].drive_status[drive_number].box->parent->parent, 
	    name, NULL);
	
    }
    if (name)
	free(name);
    
    ui_enable_drive_status(enabled_drives, drive_active_led);
}

/* ------------------------------------------------------------------------- */
/* tape stuff */
void ui_set_tape_status(int tape_status)
{
    static int ts;
    int i, w, h;
    
    if (ts == tape_status)
	return;
    ts = tape_status;
    for (i = 0; i < num_app_shells; i++) 
    {
	if (ts)
	    gtk_widget_show(app_shells[i].tape_status.event_box);
	else
	    gtk_widget_hide(app_shells[i].tape_status.event_box);
	gdk_window_get_size(app_shells[i].canvas->window, &w, &h);
	ui_resize_canvas_window(app_shells[i].canvas, w, h);
    }
}

void ui_display_tape_motor_status(int motor)
{   
    
    if (tape_motor_status == motor)
	return;
    tape_motor_status = motor;
    ui_display_tape_control_status(-1);
}

void ui_display_tape_control_status(int control)
{
    GdkColor *color;
    static GdkPoint ffw[] = {{0, 0}, {CTRL_WIDTH/2, CTRL_HEIGHT/2}, 
			     {CTRL_WIDTH/2, 0}, {CTRL_WIDTH-1, CTRL_HEIGHT/2},
                             {CTRL_WIDTH/2, CTRL_HEIGHT-1}, 
			     {CTRL_WIDTH/2, CTRL_HEIGHT/2},
			     {0, CTRL_HEIGHT-1}}; 
    static GdkPoint rew[] = {{0, CTRL_HEIGHT/2}, {CTRL_WIDTH/2, 0}, 
			     {CTRL_WIDTH/2, CTRL_HEIGHT/2}, {CTRL_WIDTH-1, 0},
                             {CTRL_WIDTH-1, CTRL_HEIGHT-1}, 
			     {CTRL_WIDTH/2, CTRL_HEIGHT/2}, 
			     {CTRL_WIDTH/2, CTRL_HEIGHT-1}}; 
    static GdkPoint play[] = {{3, 0}, {CTRL_WIDTH-3, CTRL_HEIGHT/2}, 
			      {3, CTRL_HEIGHT-1}};
    static GdkPoint stop[] = {{3, 2}, {CTRL_WIDTH-3, 2}, 
			      {CTRL_WIDTH-3, CTRL_HEIGHT-2}, 
			      {3, CTRL_HEIGHT-2}};
    GdkPoint *p;
    int i, num;

    if (control < 0)
	control = tape_control_status;
    else
	tape_control_status = control;

    color = tape_motor_status ? motor_running_pixel : drive_led_off_pixel;
    
    /* Set background color depending on motor status */
    gdk_gc_set_foreground(app_gc, color);
    for (i = 0; i < num_app_shells; i++)
    {
	tape_status_widget *ts = &app_shells[i].tape_status;
	
	gdk_draw_rectangle(ts->control_pixmap, app_gc, TRUE,
			   0, 0, CTRL_WIDTH, CTRL_HEIGHT);
    }
    
    switch (control)
    {
    case DATASETTE_CONTROL_START:
	num = 3;
	p = play;
	break;
    case DATASETTE_CONTROL_FORWARD:
	num = 7;
	p = ffw;
	break;
    case DATASETTE_CONTROL_REWIND:
	num = 7;
	p = rew;
	break;
    case DATASETTE_CONTROL_RECORD:
	gdk_gc_set_foreground(app_gc, drive_led_on_red_pixel);
	for (i = 0; i < num_app_shells; i++)
	{
	    tape_status_widget *ts = &app_shells[i].tape_status;

	    gdk_draw_arc(ts->control_pixmap, app_gc, TRUE, 3, 1, 
			 CTRL_WIDTH-6, CTRL_HEIGHT-2, 0, 
			 360 * 64);
	    gtk_widget_queue_draw(ts->control);
	}
	gdk_flush();
	return;
    default:
	num = 4;
	p = stop;
	break;
    }
    
    color = tape_control_pixel;
    gdk_gc_set_foreground(app_gc, color);
    for (i = 0; i < num_app_shells; i++)
    {
	tape_status_widget *ts = &app_shells[i].tape_status;
	gdk_draw_polygon(ts->control_pixmap, app_gc, TRUE, p, num);
    
	gtk_widget_queue_draw(ts->control);
    }
    gdk_flush();
}

void ui_display_tape_counter(int counter)
{
    static char label[10];
    int i;
    
    sprintf(label, "Tape %03d", counter % 1000);
    for (i = 0; i < num_app_shells; i++)
	gtk_label_set_text(GTK_LABEL(app_shells[i].tape_status.label), label);
    
}

void ui_display_tape_current_image(char *image)
{
    char *name;
    int i;
    
    fname_split(image, NULL, &name);

    for (i = 0; i < num_app_shells; i++)
    {
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tape_tooltip),
			     app_shells[i].tape_status.box->parent->parent, 
			     name, NULL);
    }
    if (name)
	free(name);
}

/* ------------------------------------------------------------------------- */

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;
    char str[1024];

    for (i = 0; i < num_app_shells; i++) {
	if (flag) {
	    sprintf(str, "%s (paused)", app_shells[i].title);
	    gtk_window_set_title(GTK_WINDOW(app_shells[i].shell),str);
	} else {
	    gtk_window_set_title(GTK_WINDOW(app_shells[i].shell),app_shells[i].title);
	}
    }
}

/* Dispatch the next Xt event.  If not pending, wait for it. */
void ui_dispatch_next_event(void)
{
    gtk_main_iteration();
}

/* Dispatch all the pending UI events. */
void ui_dispatch_events(void)
{
    if (!psid_mode) 
    {
	while (gtk_events_pending())
	    ui_dispatch_next_event();
	return;
    }
    psid_dispatch_events();
}

/* Resize one window. */
void ui_resize_canvas_window(ui_window_t w, int width, int height)
{
    GtkRequisition req;
#ifdef USE_VIDMODE_EXTENSION
    if( use_fullscreen) {
      XClearWindow(display,XtWindow(canvas));
      return;
    }
#endif

    gtk_widget_set_usize(w, width, height);
    gtk_widget_size_request(gtk_widget_get_toplevel(w), &req);
    gdk_window_resize(gdk_window_get_toplevel(w->window), 
		      req.width, req.height);
    gdk_flush();
    return;
}

/* Map one window. */
void ui_map_canvas_window(ui_window_t w)
{
    gtk_widget_show(w);
    /*grap*/
}

/* Unmap one window. */
void ui_unmap_canvas_window(ui_window_t w)
{
    gtk_widget_hide(w);
    /*ungrap*/
}

void ui_destroy_widget(ui_window_t w)
{
    gtk_widget_destroy(w);
}

/* Enable autorepeat. */
void ui_autorepeat_on(void)
{
    gdk_key_repeat_restore();
}

/* Disable autorepeat. */
void ui_autorepeat_off(void)
{
    gdk_key_repeat_disable();
}

/* ------------------------------------------------------------------------- */

/* Button callbacks.  */

#define DEFINE_BUTTON_CALLBACK(button)          \
    void  cb_##button##(GtkWidget *w, ui_callback_data_t client_data) \
    {                                           \
        *((ui_button_t *)client_data) = ##button##; \
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
    static GtkWidget* error_dialog;
    static ui_button_t button;

#ifdef USE_VIDMODE_EXTENSION
    ui_set_windowmode();
#endif
    va_start(ap, format);
    vsprintf(str, format, ap);
    error_dialog = build_error_dialog(&button, str);
    ui_popup(error_dialog, "VICE Error!", False);
    button = UI_BUTTON_NONE;
    do
	ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(error_dialog);
    gtk_widget_destroy(error_dialog);
    ui_dispatch_events();
    suspend_speed_eval();
}

/* Report a message to the user.  */
void ui_message(const char *format,...)
{
    char str[1024];
    va_list ap;
    static GtkWidget *error_dialog;
    static ui_button_t button;

    va_start(ap, format);
    vsprintf(str, format, ap);
    error_dialog = build_error_dialog(&button, str);
    ui_popup(error_dialog, "VICE", False);
    button = UI_BUTTON_NONE;
    do
      ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(error_dialog);
    ui_check_mouse_cursor();
    gtk_widget_destroy(error_dialog);
    ui_dispatch_events();
    suspend_speed_eval();
}

void ui_make_window_transient(GtkWidget *parent,GtkWidget *window)
{
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(parent));
    gdk_window_set_colormap(window->window,colormap);
}

/* Report a message to the user, allow different buttons. */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    char str[1024];
    va_list ap;
    static GtkWidget *jam_dialog, *message, *buttonw;
    static ui_button_t button;

    
    va_start(ap, format);

    if (console_mode) {
        vfprintf(stderr, format, ap);
	exit(0);
    }

    if (psid_mode) {
        vfprintf(stderr, format, ap);
	machine_play_psid(-1);
	/* return UI_JAM_MONITOR; */
	return UI_JAM_HARD_RESET;
    }

    jam_dialog = gtk_dialog_new();

    ui_make_window_transient(_ui_top_level,jam_dialog);

    vsprintf(str, format, ap);
    message = gtk_label_new(str);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jam_dialog)->vbox),
		       message,TRUE,TRUE,0);
    gtk_widget_show(message);

    buttonw = gtk_button_new_with_label("RESET");
    gtk_signal_connect(GTK_OBJECT(buttonw),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_RESET),
		       (gpointer) &button);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jam_dialog)->action_area),
		       buttonw,TRUE,TRUE,0);
    gtk_widget_show(buttonw);

    buttonw = gtk_button_new_with_label("HARDRESET");
    gtk_signal_connect(GTK_OBJECT(buttonw),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_HARDRESET),
		       (gpointer) &button);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jam_dialog)->action_area),
		       buttonw,TRUE,TRUE,0);
    gtk_widget_show(buttonw);

    buttonw = gtk_button_new_with_label("Monitor");
    gtk_signal_connect(GTK_OBJECT(buttonw),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_MON),
		       (gpointer) &button);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jam_dialog)->action_area),
		       buttonw,TRUE,TRUE,0);
    gtk_widget_show(buttonw);

    ui_popup(jam_dialog, "VICE", False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(jam_dialog);
    gtk_widget_destroy(jam_dialog);

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

    return 0;
  
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

static void ui_fill_preview(GtkWidget *w, int row, int col, 
			    GdkEventButton *bevent, gpointer data)
{
    char *tmp1, *tmp2, *fname, *contents;
    GtkWidget *li;
    GtkStyle *style;

    g_return_if_fail(data != NULL);
    g_return_if_fail(GTK_IS_FILE_SELECTION(data));
    
    fname = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));

    if (current_image_contents)
	free(current_image_contents);
    
    if (!fname || !current_image_contents_func)
	current_image_contents = stralloc("No image contents available");
    else
	current_image_contents = current_image_contents_func(fname);

    if (!current_image_contents)
	current_image_contents = stralloc("No image contents available");

    style = gtk_style_new();
    gdk_font_unref(style->font);
    style->font = fixedfont;
    gdk_font_ref(style->font);
    gtk_widget_set_style(image_preview_list, style);
    gtk_style_unref(style);

    /* copy contents, in order to save '\n's for finding the selection later */
    tmp1 = tmp2 = contents = stralloc(current_image_contents);
    gtk_list_clear_items(GTK_LIST(image_preview_list), 0, -1);
    
    tmp1 = find_next_line(NULL, tmp2);
    while (tmp1 > tmp2)
    {
	GtkWidget *label;
	
	*(tmp1 - 1) = '\0';
	label = gtk_label_new(tmp2);
	li = gtk_list_item_new();
	gtk_container_add(GTK_CONTAINER(li), label);
	gtk_widget_set_style(label, style);
	gtk_misc_set_alignment (GTK_MISC (label), 0, -1);
	gtk_widget_show(label);
	
	gtk_container_add(GTK_CONTAINER(image_preview_list), 
			  li);
	gtk_widget_show(li);
	tmp2 = tmp1;
	tmp1 = find_next_line(NULL, tmp2);
    }
    
    /* Last Line might be without newline char*/
    li = gtk_list_item_new_with_label(tmp2);
    gtk_container_add(GTK_CONTAINER(image_preview_list), 
		      li);
    gtk_widget_show(li);
    free(contents);
}

/* File browser. */
char *ui_select_file(const char *title,
                     char *(*read_contents_func)(const char *),
                     int allow_autostart, const char *default_dir,
                     const char *default_pattern, ui_button_t *button_return)
{  
    static ui_button_t button;
    static char *ret = NULL;
    static GtkWidget* file_selector = NULL;
    static char *filesel_dir = NULL;
    char *current_dir = NULL;
    char *filename = NULL;
    char *path;
    
    /* reset old selection */
    ui_set_selected_file(0);

    /* we preserve the current directory over the invocations */
    current_dir = get_current_dir();	/* might be changed elsewhere */
    if (filesel_dir != NULL) {
      chdir(filesel_dir);
    }

    file_selector = build_file_selector(&button);
    current_image_contents_func = read_contents_func;
    
    if (default_dir != NULL) {
        if(default_pattern) {
	    path = concat(default_dir,"/",default_pattern,NULL);
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file_selector),
					    path);
	    gtk_file_selection_complete(GTK_FILE_SELECTION(file_selector),
					path);
	    free(path);
	} else {
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file_selector), 
					    default_dir);
	}
    }  else {
        char *newdir = get_current_dir();
        if(default_pattern) {
	    path = concat(newdir,"/",default_pattern,NULL);
	    gtk_file_selection_set_filename(GTK_FILE_SELECTION(file_selector),
					    path);
	    gtk_file_selection_complete(GTK_FILE_SELECTION(file_selector),
					path);
	    free(path);
	} else {
	    gtk_file_selection_complete(GTK_FILE_SELECTION(file_selector),
					newdir);
	}
	free(newdir);
    }
    ui_popup(file_selector, title, False);
    do {
        button = UI_BUTTON_NONE;
	while (button == UI_BUTTON_NONE)
	  ui_dispatch_next_event();

	filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(file_selector));

#ifdef BUTTON_CONTENTS
	if (filename
	    && button == UI_BUTTON_CONTENTS
	    && read_contents_func != NULL)
	{
	    char *contents;
	    contents = read_contents_func(filename);
	    if (contents != NULL) {
	        ui_show_text((const char*) current_dir,(const char*) contents, 250, 240);
		free(contents);
	    } else {
	      ui_error("Unknown image type.");
	    }
	}
#endif
    } while ((!filename  && button != UI_BUTTON_CANCEL)
	     || button == UI_BUTTON_CONTENTS);
    if (ret != NULL)
        free(ret);
    if (filename)
        ret = stralloc(filename);
    else
	ret = stralloc("");

    ui_popdown(file_selector);

    if (filesel_dir != NULL) {
        free(filesel_dir);
    }
    filesel_dir = get_current_dir();
    if (current_dir != NULL) {
        chdir(current_dir);
	free(current_dir);
    }
#if 0
    printf("filename %s\n",ret);
    printf("%s %s\n",
	   GTK_LABEL(GTK_FILE_SELECTION(file_selector)->selection_text)->label,
	   gtk_entry_get_text (GTK_ENTRY (GTK_FILE_SELECTION(file_selector)->selection_entry))
	   );
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
    static GtkWidget *input_dialog, *input_dialog_label, *input_dialog_field;
    static ui_button_t button;
    if (!input_dialog)
      input_dialog = build_input_dialog(&button,
					&input_dialog_label,
					&input_dialog_field);

    gtk_label_set_text(GTK_LABEL(input_dialog_label),prompt);
    gtk_entry_set_text(GTK_ENTRY(input_dialog_field),buf);
    /*    XtSetKeyboardFocus(input_dialog, input_dialog_field);*/
    ui_popup(input_dialog, title, False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    
    strncpy(buf, gtk_entry_get_text(GTK_ENTRY(input_dialog_field)),
	    buflen);
    ui_popdown(input_dialog);
    return button;
}

/* Display a text to the user. */
void ui_show_text(const char *title, const char *text, int width, int height)
{
    static ui_button_t button;
    GtkWidget *show_text;

    show_text = build_show_text(&button, (String)text,
				width, height);
    ui_popup(show_text, title, False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(show_text);    
}

/* Ask for a confirmation. */
ui_button_t ui_ask_confirmation(const char *title, const char *text)
{

    static GtkWidget *confirm_dialog, *confirm_dialog_message;
    static ui_button_t button;

    if (!confirm_dialog)
	confirm_dialog = build_confirm_dialog(&button,
                                              &confirm_dialog_message);
    gtk_label_set_text(GTK_LABEL(confirm_dialog_message),text);

    ui_popup(confirm_dialog, title, False);
    button = UI_BUTTON_NONE;
    do
        ui_dispatch_next_event();
    while (button == UI_BUTTON_NONE);
    ui_popdown(confirm_dialog);
    
    return button;
}

/* Update the menu items with a checkmark according to the current resource
   values.  */
void ui_update_menus(void)
{
    ui_menu_update_all();
}

static void ui_block_shells(void)
{
    int i;
    for (i = 0; i < num_app_shells; i++)
	gtk_widget_set_sensitive(app_shells[i].shell, FALSE);
}

static void ui_unblock_shells(void)
{
    int i;
    for (i = 0; i < num_app_shells; i++)
	gtk_widget_set_sensitive(app_shells[i].shell, TRUE);
}

/* Pop up a popup shell and center it to the last visited AppShell */
void ui_popup(GtkWidget *w, const char *title, Boolean wait_popdown)
{
    
    ui_restore_mouse();
    /* Keep sure that we really know which was the last visited shell. */
    ui_dispatch_events();
    
    gtk_widget_set_parent_window(w,_ui_top_level->window);
    gtk_window_set_title(GTK_WINDOW(w),title);
    {
	/* Center the popup. */
      
	gint my_width, my_height, shell_width, shell_height;
	gint my_x, my_y;
	gint tlx, tly;
	gint root_width, root_height;

	gtk_widget_show(w);

	gdk_window_get_size(w->window,&my_width, &my_height);


        /* Now make sure the whole widget is visible.  */
	root_width = gdk_screen_width();
	root_height = gdk_screen_height();

	gdk_window_get_size(_ui_top_level->window,
			    &shell_width,&shell_height);
	gdk_window_get_root_origin(_ui_top_level->window,&tlx, &tly);

	/* XtTranslateCoords(XtParent(s), tlx, tly, &tlx, &tly);*/
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
	gdk_window_move(w->window,my_x,my_y);
    }
    gdk_flush();


    ui_block_shells();
    /* If requested, wait for this widget to be popped down before
       returning. */
    if (wait_popdown) {
	int oldcnt = popped_up_count++;
    
	while (oldcnt != popped_up_count)
	    ui_dispatch_next_event();
	ui_unblock_shells();
    } else 
	popped_up_count++;
}

/* Pop down a popup shell. */
void ui_popdown(GtkWidget *w)
{
    ui_check_mouse_cursor();
    if (w)
	gtk_widget_hide(w);
    if (--popped_up_count < 0)
	popped_up_count = 0;
    ui_unblock_shells();
    
#ifdef USE_VIDMODE_EXTENSION
    focus_window_again();
#endif
}

/* ------------------------------------------------------------------------- */

/* These functions build all the widgets. */

static GtkWidget *build_file_selector(ui_button_t *button_return)
{  
    GtkWidget *fileselect, *button, *scrollw, *frame, *table;

    fileselect = gtk_file_selection_new("");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselect)->ok_button),
		       "clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_OK),
		       (gpointer) button_return);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselect)->cancel_button),
		       "clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CANCEL),
		       (gpointer) button_return);

#ifdef BUTTON_CONTENTS
    button = gtk_button_new_with_label("Contents");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CONTENTS),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(fileselect)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);
#endif

    table = gtk_table_new(2, 1, FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(fileselect)->action_area),
		       table, FALSE, FALSE, 10);
    gtk_widget_show(table);
    
    frame = gtk_frame_new("Image contents");
    gtk_table_attach(GTK_TABLE(table),
		     frame,
		     0, 1,
		     0, 1,
		     GTK_FILL,
		     GTK_FILL | GTK_EXPAND,
		     10, 
		     10);

    gtk_widget_show(frame);
    
    /* Contents preview */
    scrollw = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(frame), scrollw);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollw),
				    GTK_POLICY_AUTOMATIC,
				    GTK_POLICY_AUTOMATIC);

    gtk_widget_set_usize(scrollw, 250, 200);
    gtk_container_set_border_width(GTK_CONTAINER(scrollw), 4);
    
    image_preview_list = gtk_list_new();
    
    gtk_list_set_selection_mode (GTK_LIST (image_preview_list), 
				 GTK_SELECTION_SINGLE);
    gtk_scrolled_window_add_with_viewport
        (GTK_SCROLLED_WINDOW (scrollw), image_preview_list);
    gtk_container_set_focus_vadjustment
        (GTK_CONTAINER (image_preview_list),
         gtk_scrolled_window_get_vadjustment
         (GTK_SCROLLED_WINDOW (scrollw)));
    gtk_container_set_focus_hadjustment
        (GTK_CONTAINER (image_preview_list),
         gtk_scrolled_window_get_hadjustment
         (GTK_SCROLLED_WINDOW (scrollw)));

    gtk_widget_show(image_preview_list);
    gtk_widget_show(scrollw);

    gtk_signal_connect_after(
	GTK_OBJECT(GTK_FILE_SELECTION(fileselect)->file_list),
	"select-row",
	(GtkSignalFunc) ui_fill_preview,
	(gpointer) fileselect);

    button = gtk_button_new_with_label("Autostart");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(filesel_autostart_cb),
		       (gpointer) button_return);
    gtk_table_attach(GTK_TABLE(table),
		     button,
		     1, 2,
		     0, 1,
		     0,
		     0,
		     10, 
		     10);
    
    gtk_widget_show(button);

    return fileselect;
}

static GtkWidget* build_error_dialog(ui_button_t * button_return,
				     const String message)
{
    GtkWidget *error_dialog, *button ,*mess;

    error_dialog = gtk_dialog_new();


    mess = gtk_label_new(message);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(error_dialog)->vbox),
		       mess,TRUE,TRUE,0);
    gtk_widget_show(mess);


    button = gtk_button_new_with_label("Close");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CLOSE),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(error_dialog)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);
    

    return error_dialog;

}

static GtkWidget* build_input_dialog(ui_button_t * button_return,
				     GtkWidget **input_dialog_label,
				     GtkWidget **input_dialog_field)
{
    GtkWidget *input_dialog, *button;

    input_dialog = gtk_dialog_new();

    *input_dialog_label = gtk_label_new("");

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(input_dialog)->vbox),
		       *input_dialog_label,TRUE,TRUE,0);
    gtk_widget_show(*input_dialog_label);

    *input_dialog_field = gtk_entry_new();
    gtk_entry_set_editable(GTK_ENTRY(*input_dialog_field),TRUE);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(input_dialog)->vbox),
		       *input_dialog_field,TRUE,TRUE,0);

    gtk_widget_show(*input_dialog_field);


    button = gtk_button_new_with_label("Confirm");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_OK),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(input_dialog)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("Cancel");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CANCEL),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(input_dialog)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);

    return input_dialog;
}

static GtkWidget* build_show_text(ui_button_t * button_return,
				  const String text, int width, int height)
{
    GtkWidget *show_text, *button, *textw, *vs, *hs , *table;

    show_text = gtk_dialog_new();

    gtk_widget_set_usize(show_text, width, height);

    table = gtk_table_new(2,2,FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(show_text)->vbox),
		       table,TRUE,TRUE,0);
    gtk_widget_show(table);

    textw = gtk_text_new(NULL,NULL);
    gtk_text_set_line_wrap(GTK_TEXT(textw),FALSE);
    gtk_text_insert(GTK_TEXT(textw), fixedfont, NULL, NULL, text, -1);
    gtk_table_attach(GTK_TABLE(table), textw, 0, 1, 0, 1,
		     GTK_FILL|GTK_EXPAND,GTK_FILL|GTK_EXPAND,0,0);
    gtk_widget_show(textw);

    vs = gtk_vscrollbar_new(GTK_TEXT(textw)->vadj);
    gtk_table_attach(GTK_TABLE(table), vs, 1, 2, 0, 1,
		     GTK_FILL,GTK_FILL,0,0);
    gtk_widget_show(vs);

    hs = gtk_hscrollbar_new(GTK_TEXT(textw)->hadj);
    gtk_table_attach(GTK_TABLE(table), hs, 0, 1, 1, 2,
		     GTK_FILL,GTK_FILL,0,0);
    gtk_widget_show(hs);

    button = gtk_button_new_with_label("Close");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CLOSE),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(show_text)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);

    return show_text;
}

static GtkWidget* build_confirm_dialog(ui_button_t *button_return,
				       GtkWidget **confirm_dialog_message)
{
    GtkWidget *confirm_dialog, *button;

    confirm_dialog = gtk_dialog_new();


    *confirm_dialog_message = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(confirm_dialog)->vbox),
		       *confirm_dialog_message,TRUE,TRUE,0);
    gtk_widget_show(*confirm_dialog_message);


    button = gtk_button_new_with_label("Yes");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_YES),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(confirm_dialog)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("No");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_NO),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(confirm_dialog)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);

    button = gtk_button_new_with_label("Cancel");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CANCEL),
		       (gpointer) button_return);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(confirm_dialog)->action_area),
		       button,TRUE,TRUE,0);
    gtk_widget_show(button);
    

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
    GtkRequisition req;
    static int oldw, oldh;
    
    suspend_speed_eval();
    
    gtk_widget_size_request(gtk_widget_get_toplevel(w), &req);
    if (oldw != req.width ||
	oldh != req.height)
    {
	oldw = req.width;
	oldh = req.height;
	gdk_window_resize(gdk_window_get_toplevel(w->window), 
			  req.width, req.height);
    }
    
#if 0    
    if (client_data)
	((ui_exposure_handler_t) client_data)((unsigned int)req.width,
						   (unsigned int)req.height);
#endif

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

void ui_set_drive8_menu (GtkWidget *w)
{
    if (drive8menu)
	gtk_widget_destroy(drive8menu);
    drive8menu = w;
}

void ui_set_drive9_menu (GtkWidget *w)
{
    if (drive9menu)
	gtk_widget_destroy(drive9menu);
    drive9menu = w;
}

void ui_set_tape_menu (GtkWidget *w)
{
    if (tape_menu)
	gtk_widget_destroy(tape_menu);
    tape_menu = w;
}


#ifdef USE_VIDMODE_EXTENSION
int ui_set_windowmode(void)
{
    if(use_fullscreen) {
        set_fullscreen(0);
	ui_update_menus();
	return(1);
    }
    return(0);
}

int ui_set_fullscreenmode(void)
{
    if(!use_fullscreen) {
        set_fullscreen((resource_value_t) 1);
	ui_update_menus();
	return(0);
    }
    return(1);
}

void ui_set_fullscreenmode_init(void)
{
    set_bestmode(selected_videomode_at_start);
    if( selected_videomode_index == -1 &&
	bestmode_counter > 0) {
        selected_videomode_index = bestmodes[0].modeindex;
    }
    if(use_fullscreen_at_start) {
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

int ui_is_fullscreen_available() {
    return(bestmode_counter?1:0);
}

#endif
