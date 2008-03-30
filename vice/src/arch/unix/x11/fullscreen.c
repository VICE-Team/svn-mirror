/*
 * fullscreen.c
 *
 * Written by
 *  Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>

#define VidMode_MINMAJOR 0
#define VidMode_MINMINOR 0
#include <X11/extensions/xf86vmode.h>

#include "mouse.h"
#include "resources.h"
#include "log.h"
#include "ui.h"
#include "utils.h"

extern Widget canvas, pane;
extern int screen;

extern Cursor blankCursor;
extern int cursor_is_blank;

typedef struct {
  int modeindex;
  char name[17];
} fullscreen_bestvideomode_t;

static fullscreen_bestvideomode_t bestmodes[10];

static XF86VidModeModeInfo **allmodes;
static int vidmodecount;

static int bestmode_counter;
static int vidmodeavail = 0;
static int selected_videomode_index;
extern int use_fullscreen;
static int use_fullscreen_at_start = 0;
static int timeout;

static char *selected_videomode;
static char *selected_videomode_at_start;

/* Woah, away with this! */
extern void video_setfullscreen(int v,int width, int height);


static void mouse_timeout(int signo)
{
    if (use_fullscreen && !cursor_is_blank)
        XDefineCursor(ui_get_display_ptr(), XtWindow(canvas), blankCursor);
}

void fullscreen_set_mouse_timeout(void)
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

    if (!_mouse_enabled) {
        XUndefineCursor(ui_get_display_ptr(), XtWindow(canvas));
        if (cursor_is_blank == 0) {
            if (signal(SIGALRM, mouse_timeout) == SIG_ERR)
                return;
            alarm(5);
        }
    }
}

int fullscreen_vidmode_available(void)
{
    int MajorVersion, MinorVersion;
    int EventBase, ErrorBase;
    int i, hz = 0;
    Display *display = ui_get_display_ptr();

    bestmode_counter = 0;
    vidmodeavail = 0;

    if (! XF86VidModeQueryVersion (display, &MajorVersion, &MinorVersion)) {
        log_error(LOG_DEFAULT, _("Unable to query video extension version"));
        return 0;
    }
    if (! XF86VidModeQueryExtension (display, &EventBase, &ErrorBase)) {
        log_error(LOG_DEFAULT, _("Unable to query video extension information"));
        return 0;
    }
    if (MajorVersion < VidMode_MINMAJOR
        || (MajorVersion == VidMode_MINMAJOR && MinorVersion < VidMode_MINMINOR)
) {
        /* Fail if the extension version in the server is too old */
        log_error(LOG_DEFAULT,
                  _("Xserver is running an old XFree86-VidMode version (%d.  %d)"),
	          MajorVersion, MinorVersion);
        log_error(LOG_DEFAULT, _("Minimum required version is %d.%d"),
	          VidMode_MINMAJOR, VidMode_MINMINOR);
        return 0;
    }
    if (!XF86VidModeGetAllModeLines(display, screen, &vidmodecount,
        &allmodes)) {
        log_error(LOG_DEFAULT, _("Error getting video mode information"));
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

    vidmodeavail = 1;
    return 1;
}

/*static*/ int set_fullscreen(resource_value_t v)
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
    Display *display = ui_get_display_ptr();

    if ( !vidmodeavail || !bestmode_counter ) {
        use_fullscreen_at_start = (int) v;
	return 0;
    }
    if ( selected_videomode_index == -1) {
	return 0;
    }

    if (v && !use_fullscreen) {
        log_message(LOG_DEFAULT, _("Switch to fullscreen %ix%i"),
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
	    log_error(LOG_DEFAULT, _("Error switching to fullscreen %ix%i\n"),
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
	    unsigned int mask;

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
	fullscreen_set_mouse_timeout();
    } else if ((int) v == 2) {
        int troot_x, troot_y;
        int twin_x,twin_y;

        log_message(LOG_DEFAULT, _("Change to fullscreen %ix%i"),
		    allmodes[selected_videomode_index]->hdisplay,
		    allmodes[selected_videomode_index]->vdisplay);
	if ( ! XF86VidModeSwitchToMode(display, screen,
				       allmodes[selected_videomode_index])) {
	    log_error(LOG_DEFAULT, _("Error switching to fullscreen %ix%i\n"),
		      allmodes[selected_videomode_index]->hdisplay,
		      allmodes[selected_videomode_index]->vdisplay);
	    return 0;
	    }
	{
	    Window root, child;
	    unsigned int mask;

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
        log_message(LOG_DEFAULT, _("Switch to windowmode"));

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

void fullscreen_focus_window_again(void)
{
    Display *display;

    if (!use_fullscreen)
        return;

    display = ui_get_display_ptr();

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
    fullscreen_set_mouse_timeout();
}

/*static*/ int set_bestmode(resource_value_t v)
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

/* ------------------------------------------------------------------------- */

int fullscreen_mode_on(void)
{
    if (!use_fullscreen) {
        set_fullscreen((resource_value_t) 1);
        ui_update_menus();
        return 0;
    }
    return 1;
}

int fullscreen_mode_off(void)
{
    if (use_fullscreen) {
        set_fullscreen(0);
        ui_update_menus();
        return 1;
    }
    return 0;
}

void fullscreen_mode_init(void)
{
    set_bestmode(selected_videomode_at_start);
    if (selected_videomode_index == -1 && bestmode_counter > 0)
        selected_videomode_index = bestmodes[0].modeindex;
    if (use_fullscreen_at_start) {
        fullscreen_mode_on();
        XtVaSetValues(XtParent(XtParent(canvas)),
                      XtNx,          0,
                      XtNy,          0,
                      None);
        XWarpPointer(ui_get_display_ptr(), None,
                     XtWindow(canvas),
                     0, 0, 0, 0, 0, 0);
    }
    ui_update_menus();
}

int fullscreen_available(void)
{
    return (bestmode_counter ? 1 : 0);
}

int fullscreen_available_modes(void)
{
    return bestmode_counter;
}

char *fullscreen_mode_name(int mode)
{
    return bestmodes[mode].name;
}

