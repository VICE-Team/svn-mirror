/*
 * fullscreen.c
 *
 * Written by
 *  Oliver Schaertel <orschaer@forwiss.uni-erlangen.de>
 *  Andreas Boose <viceteam@t-online.de>
 *  Martin Pottendorfer <pottendo@utanet.at>
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

/* 
#define FS_DEBUG
*/
#include "vice.h"
#include "log.h"
#include "resources.h"

#if 0
#define CHECK_DGA_V2

#else  /* DGA2 support is in dga2.c; FIXME */

#warning "Trying to compile fullscreen.c. This is not reworked for the new video code yet!"

#if 0

#ifdef FS_DEBUG
#include <signal.h>
#endif
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <X11/extensions/xf86dga.h>
#define DGA_MINMAJOR 0
#define DGA_MINMINOR 0

#include <X11/extensions/xf86vmode.h>
#define VidMode_MINMAJOR 0
#define VidMode_MINMINOR 0

#include "kbd.h"
#include "machine.h"
#include "mouse.h"
#include "resources.h"
#include "log.h"
#include "ui.h"
#include "vsyncapi.h"
#include "utils.h"
#include "videoarch.h"
#include "x11ui.h"


typedef struct {
  int modeindex;
  char name[30];
} fullscreen_bestvideomode_t;

static fullscreen_bestvideomode_t fullscreen_bestmodes[10];


/* dga v1 */
static XF86VidModeModeInfo **fullscreen_allmodes;
static XF86VidModeModeLine restoremodeline;

#ifdef CHECK_DGA_V2
/* dga v2 */
static XDGADevice* dgadev=NULL; 
static XDGAMode *fullscreen_allmodes_dga2;
static GC gcContext;

static Colormap cm;
static unsigned long colors[256];
static void_hook_t old_ui_hook;
#endif

static int fullscreen_vidmodecount;

static int fullscreen_bestmode_counter;
static int fullscreen_vidmodeavail = 0;
static int fullscreen_selected_videomode_index;
static int fullscreen_use_fullscreen_at_start = 0;
static int timeout;
static int fullscreen_is_enabled_restore;
static int EventBase, ErrorBase;

int fullscreen_is_enabled;
int request_fullscreen_mode = 0;
char *fullscreen_selected_videomode;
char *fullscreen_selected_videomode_at_start;
int fullscreen_width, fullscreen_height;

int fullscreen_dga_major;

static int fb_bank, fb_mem;
static char *fb_addr;
static int fb_width; 
static int fb_depth;

static Display *display;
extern int screen;

int fullscreen_mode_on (void);
int fullscreen_mode_off (void);
static void fullscreen_dispatch_events(void);
static void fullscreen_dispatch_events_2(void);

#ifdef FS_DEBUG
void alarm_timeout(int signo) {
    volatile int i = 1;
    
    fullscreen_mode_off();
    while (i)
    {
	log_message(LOG_DEFAULT, _("Attach debugger to pid %d...\n"), 
		    getpid());
	sleep(1);		/* Set breakpoint here to debug */
    }
}  

void set_alarm_timeout() {
    if (signal(SIGSEGV, alarm_timeout) == SIG_ERR)
        return;
#if 0
    if (signal(SIGALRM, alarm_timeout) == SIG_ERR)
        return;
    alarm(20);
#endif
}   
#endif

void fullscreen_refresh_func(video_frame_buffer_t *f,
			     int src_x, int src_y,
			     int dest_x, int dest_y,
			     unsigned int width, unsigned int height) 
{
    int y;
    
    if ((dest_y + height) > fullscreen_height)
      height = fullscreen_height;

    if ((dest_x + width) > fullscreen_width)
      width = fullscreen_width;

#ifdef CHECK_DGA_V2
    if (fullscreen_dga_major <= 1) {
#endif
        for (y = 0; y < height ; y++) { 
	    memcpy(fb_addr + (fb_width * (dest_y + y) + dest_x) * fb_depth, 
		   f->x_image->data + (f->x_image->width * (src_y + y) + src_x) * fb_depth, 
		   width * fb_depth);
	}
#ifdef CHECK_DGA_V2
    } else {
        XPutImage(display, dgadev->pixmap, gcContext,  f->x_image,
		  src_x, src_y, dest_x, dest_y, width, height);
    }
#endif
}   

int fullscreen_vidmode_available(void)
{
    int MajorVersion, MinorVersion;
    int i, j, hz = 0;

    fullscreen_bestmode_counter = 0;
    fullscreen_vidmodeavail = 0;

    display = x11ui_get_display_ptr();

#ifndef FS_DEBUG
    if (XF86DGAForkApp(XDefaultScreen(display)) != 0)
    {
	log_error(LOG_DEFAULT, "Can't fork for DGA; quitting");
	return 1;
    }
    log_message(LOG_DEFAULT, _("Successfully forked DGA\n"));
#endif

    if (! XF86DGAQueryVersion (display, &MajorVersion, &MinorVersion)) {
        log_error(LOG_DEFAULT, _("Unable to query video extension version - disabling fullscreen."));
	fullscreen_vidmodeavail = 0;
        return 1;
    }
    if (! XF86DGAQueryExtension (display, &EventBase, &ErrorBase)) {
        log_error(LOG_DEFAULT, _("Unable to query video extension information - disabling fullscreen."));
	fullscreen_vidmodeavail = 0;
        return 1;
    }  

    if (MajorVersion < DGA_MINMAJOR
        || (MajorVersion == DGA_MINMAJOR && MinorVersion < DGA_MINMINOR))
    {
        log_error(LOG_DEFAULT, _("Xserver is running an old XFree86-DGA version (%d.%d) "), 
				 MajorVersion, MinorVersion);
        log_error(LOG_DEFAULT, _("Minimum required version is %d.%d - disabling fullscreen."),
	          DGA_MINMAJOR, DGA_MINMINOR);
	fullscreen_vidmodeavail = 0;
        return 1;
    }

    fullscreen_dga_major = MajorVersion;

#ifdef CHECK_DGA_V2
    if (fullscreen_dga_major <= 1) {
#endif
        if (! XF86VidModeQueryVersion (display, &MajorVersion, &MinorVersion)) {
	    log_error(LOG_DEFAULT, _("Unable to query video extension version - disabling fullscreen."));
	    fullscreen_vidmodeavail = 0;
	    return 1;
	}
	if (! XF86VidModeQueryExtension (display, &EventBase, &ErrorBase)) {
	    log_error(LOG_DEFAULT, _("Unable to query video extension information - disabling fullscreen."));
	    fullscreen_vidmodeavail = 0;
	    return 1;
	}
	if (MajorVersion < VidMode_MINMAJOR
	    || (MajorVersion == VidMode_MINMAJOR && MinorVersion < VidMode_MINMINOR)
	    ) {
	    /* Fail if the extension version in the server is too old */
	    log_error(LOG_DEFAULT,
		      _("Xserver is running an old XFree86-VidMode version (%d.  %d)"),
		      MajorVersion, MinorVersion);
	    log_error(LOG_DEFAULT, _("Minimum required version is %d.%d - disabling fullscreen."),
		      VidMode_MINMAJOR, VidMode_MINMINOR);
	    fullscreen_vidmodeavail = 0;
	    return 1;
	}
	if (!XF86VidModeGetAllModeLines(display, screen, &fullscreen_vidmodecount,
					&fullscreen_allmodes)) {
	    log_error(LOG_DEFAULT, _("Error getting video mode information - disabling fullscreen."));
	    fullscreen_vidmodeavail = 0;
	    return 1;
	}


	if (geteuid () != 0) {
  	    log_error(LOG_DEFAULT, _("Vice is not running as root, DGA extension not possible - disabling fullscreen."));
	    fullscreen_vidmodeavail = 0;
	    return 1;
	}

	if (! XF86DGAGetVideo (display, screen, &fb_addr, &fb_width, &fb_bank, &fb_mem)
	    || fb_bank < fb_mem) {
	      log_error(LOG_DEFAULT, _("Problems with DGA - disabling fullscreen."));
	      fullscreen_vidmodeavail = 0;
	      return 1;
	}
	for (i = 0; i < fullscreen_vidmodecount; i++) {
  	    if (fullscreen_allmodes[i]->hdisplay <= 800 &&
		fullscreen_allmodes[i]->hdisplay >= 320 &&
		fullscreen_allmodes[i]->vdisplay <= 600  &&
		fullscreen_allmodes[i]->vdisplay >= 200) {
	        fullscreen_bestmodes[fullscreen_bestmode_counter].modeindex=i;
		hz = fullscreen_allmodes[i]->dotclock * 1000 /
		  ( fullscreen_allmodes[i]->vtotal * fullscreen_allmodes[i]->htotal) ;
		snprintf(fullscreen_bestmodes[fullscreen_bestmode_counter].name,17," %ix%i-%iHz",
			 fullscreen_allmodes[i]->hdisplay,
			 fullscreen_allmodes[i]->vdisplay,
			 hz);
		fullscreen_bestmode_counter++;
		if (fullscreen_bestmode_counter == 10) break;
	    }
	}
#ifdef CHECK_DGA_V2
    } else {
        fullscreen_allmodes_dga2 = XDGAQueryModes(display,screen,&fullscreen_vidmodecount);
	for (i = 0; i < fullscreen_vidmodecount; i++) {
  	    if (fullscreen_allmodes_dga2[i].viewportWidth <= 800 &&
		fullscreen_allmodes_dga2[i].viewportWidth >= 320 &&
		fullscreen_allmodes_dga2[i].viewportHeight <= 600  &&
		fullscreen_allmodes_dga2[i].viewportHeight >= 200 &&
		fullscreen_allmodes_dga2[i].depth == x11ui_get_display_depth() &&
		fullscreen_allmodes_dga2[i].visualClass != DirectColor &&
		(fullscreen_allmodes_dga2[i].flags & XDGAPixmap)) {

	        for (j = 0; j < fullscreen_bestmode_counter; j++) {

		  if ( (fullscreen_allmodes_dga2[fullscreen_bestmodes[j].modeindex].viewportWidth == fullscreen_allmodes_dga2[i].viewportWidth) &&
		       fullscreen_allmodes_dga2[fullscreen_bestmodes[j].modeindex].viewportHeight == fullscreen_allmodes_dga2[i].viewportHeight) 
		      break;
		}
		if ( j == fullscreen_bestmode_counter) {
		    fullscreen_bestmodes[fullscreen_bestmode_counter].modeindex=i;
		    snprintf(fullscreen_bestmodes[fullscreen_bestmode_counter].name,17," %ix%i-%.1fHz",
			     fullscreen_allmodes_dga2[i].viewportWidth ,
			     fullscreen_allmodes_dga2[i].viewportHeight,
			     fullscreen_allmodes_dga2[i].verticalRefresh);
		    fullscreen_bestmode_counter++;
		}
		if (fullscreen_bestmode_counter == 10) break;
	    }
	}
    }
#endif
	
    fullscreen_vidmodeavail = 1;
    return 1;
}

int fullscreen_request_set_mode(resource_value_t v, void *param)
{
#ifdef CHECK_DGA_V2
    if (!v)
    {
	request_fullscreen_mode = 2; /* toggle to window mode */
	if (old_ui_hook)
	    (void) vsync_set_event_dispatcher(old_ui_hook);
    }
    else
    {
	request_fullscreen_mode = 1; /* toggle to fullscreen mode */
	old_ui_hook = vsync_set_event_dispatcher(fullscreen_dispatch_events);
    }

    if (param)
    {
	request_fullscreen_mode = 0;
	if (v)
	    return fullscreen_mode_on();
	else
	    return fullscreen_mode_off();
    }
    
    return 0;
#else
    return fullscreen_set_mode(v, param);
#endif
}

int fullscreen_set_mode(resource_value_t v, void *param)
{
    static int interval,prefer_blanking,allow_exposures;
    static int dotclock;
    XColor TheColor;
    int i;

    if ( !fullscreen_vidmodeavail || !fullscreen_bestmode_counter ) {
        fullscreen_use_fullscreen_at_start = (int) v;
	return 0;
    }
    if ( fullscreen_selected_videomode_index == -1) {
	return 0;
    }

    if (v && !fullscreen_is_enabled) {
#ifndef CHECK_DGA_V2
        XGrabKeyboard(display,  XRootWindow (display, screen),
		      1, GrabModeAsync,
		      GrabModeAsync,  CurrentTime);
	XGrabPointer(display,  XRootWindow (display, screen), 1,
		     PointerMotionMask | ButtonPressMask |
		     ButtonReleaseMask,
		     GrabModeAsync, GrabModeAsync,
		     None, None, CurrentTime);
    
	XGetScreenSaver(display,&timeout,&interval,
			&prefer_blanking,&allow_exposures);
	XSetScreenSaver(display,0,0,DefaultBlanking,DefaultExposures);
#endif

#ifdef CHECK_DGA_V2
	if (fullscreen_dga_major <= 1) {
#endif
	    log_message(LOG_DEFAULT, _("Switch to fullscreen %ix%i"),
		      fullscreen_allmodes[fullscreen_selected_videomode_index]->hdisplay,
		      fullscreen_allmodes[fullscreen_selected_videomode_index]->vdisplay);

	    XF86VidModeGetModeLine(display, screen, &dotclock, &restoremodeline);

	    XF86VidModeLockModeSwitch(display, screen, 1);
	    if ( ! XF86VidModeSwitchToMode(display, screen,
					   fullscreen_allmodes[fullscreen_selected_videomode_index])) {
	        log_error(LOG_DEFAULT, _("Error switching to fullscreen %ix%i\n"),
			  fullscreen_allmodes[fullscreen_selected_videomode_index]->hdisplay,
			  fullscreen_allmodes[fullscreen_selected_videomode_index]->vdisplay);
		return 0;
	    } else {
		fullscreen_width = fullscreen_allmodes[fullscreen_selected_videomode_index]->hdisplay;
	        fullscreen_height = fullscreen_allmodes[fullscreen_selected_videomode_index]->vdisplay;

	    }

	    XF86DGAGetVideo (display, screen, &fb_addr, &fb_width, &fb_bank, &fb_mem);
	    log_message(LOG_DEFAULT, _("DGA extension: addr:%p, width %d, bank size %d mem size %d\n"),
			fb_addr, fb_width, fb_bank, fb_mem);  


	    XF86DGADirectVideo (display, screen, XF86DGADirectGraphics | XF86DGADirectMouse | XF86DGADirectKeyb);
	    XF86DGASetViewPort (display, screen, 0, 0); 
#ifdef CHECK_DGA_V2
	} else {
	    int pm_x = 0, pm_y = 0;
	    log_message(LOG_DEFAULT, _("Switch to fullscreen %ix%i"),
			fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportWidth,
			fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportHeight);

	    dgadev = XDGASetMode(display,screen,fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].num); 
	    if (!dgadev) {
	        log_error(LOG_DEFAULT, _("Error switching to fullscreen %ix%i\n"),
			  fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportWidth, fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportHeight);
	        return 0;
	    }
	    if (!dgadev->pixmap) {
	        log_error(LOG_DEFAULT, _("Error switching to fullscreen %ix%i\n"),
			  fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportWidth, fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportHeight);
		XDGASetMode(display,screen,0);
		XFree(dgadev);
	        return 0;
	    }
	    
	    fb_width = fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].bytesPerScanline; 
	    fullscreen_width = dgadev->mode.viewportWidth;
	    fullscreen_height = dgadev->mode.viewportHeight;

	    gcContext =  XCreateGC(display, dgadev->pixmap, 0, 0);
 	    if (!gcContext) {
	      log_error(LOG_DEFAULT, _("Error switching to fullscreen %ix%i\n"),
			fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportWidth, fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].viewportHeight);
 	        XDGASetMode(display,screen,0);
		XFree(dgadev);
		ui_display_paused(1);
		return 0;
	    }

	    XDGASelectInput(display, screen,
		    PointerMotionMask | 
		    ButtonPressMask | 
		    KeyPressMask | 
		    KeyReleaseMask);
    
	    cm = XDGACreateColormap(display, screen, dgadev, AllocNone);

	    for(i = 0; i < 256; i++) {
		TheColor.blue = TheColor.red = (i << 8) | i;
		TheColor.green = 128;
	
		TheColor.flags = DoRed | DoGreen | DoBlue;
		XAllocColor(display, cm, &TheColor);
		colors[i] = TheColor.pixel;
	    }
	    XDGAInstallColormap(display, screen, cm);
    
	    XDGAChangePixmapMode(display, screen, &pm_x, &pm_y, 
				 XDGAPixmapModeLarge);
	    XDGASetViewport (display, screen, 0, 0, XDGAFlipRetrace);

	    while(XDGAGetViewportStatus(display, screen));
	    XSetForeground(display, gcContext, 0);

	    XFillRectangle(display, dgadev->pixmap, gcContext, 0, 0, 
			   fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].maxViewportX, 
			   fullscreen_allmodes_dga2[fullscreen_selected_videomode_index].maxViewportY);
	    XFlush(display);
	}
#endif
	fullscreen_is_enabled = 1;
#ifdef FS_DEBUG
	set_alarm_timeout();
#endif
    } else if (!v && fullscreen_is_enabled) {
        log_message(LOG_DEFAULT, _("Switch to windowmode"));

#ifdef CHECK_DGA_V2
	if (fullscreen_dga_major <= 1) {
#endif
	    XF86DGADirectVideo (display, screen, 0);

	    XF86VidModeLockModeSwitch(display, screen, 0);

	    /* Oh who has designed the vidmode extension API???? */
	    for (i = 0; i < fullscreen_vidmodecount; i++) {
	      if (fullscreen_allmodes[i]->hdisplay == restoremodeline.hdisplay &&
		  fullscreen_allmodes[i]->vdisplay == restoremodeline.vdisplay &&
		  fullscreen_allmodes[i]->dotclock == dotclock ) {
		XF86VidModeSwitchToMode(display, screen, fullscreen_allmodes[i]);
		break;
	      }
	    }
#ifdef CHECK_DGA_V2
	} else {
	    XDGASetMode(display,screen,0);
	    XFree(dgadev);
	    XFreeGC(display, gcContext);
	}

#endif
	XSetScreenSaver(display,timeout,interval,prefer_blanking,allow_exposures);

	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);

	fullscreen_is_enabled = 0;
    }
    machine_video_refresh();
    ui_check_mouse_cursor();
    return 1;
}

int fullscreen_set_bestmode(resource_value_t v, void *param)
{
    int i;

    if (!fullscreen_vidmodeavail) {
        fullscreen_selected_videomode_at_start = (char*) stralloc(v);
        return(0);
    }
    fullscreen_selected_videomode = (char*) v;
    for (i = 0; i < fullscreen_bestmode_counter; i++) {
        if (! strcmp(fullscreen_selected_videomode, fullscreen_bestmodes[i].name)) {
	    fullscreen_selected_videomode_index = fullscreen_bestmodes[i].modeindex;
	    if (fullscreen_is_enabled)
                fullscreen_set_mode((resource_value_t) 1, NULL);
	    return(0);
	}
    }
    if (fullscreen_bestmode_counter > 0) {
        fullscreen_selected_videomode_index = fullscreen_bestmodes[0].modeindex;
        fullscreen_selected_videomode = fullscreen_bestmodes[0].name;
    }

    return(0);
}

/* ------------------------------------------------------------------------- */

int fullscreen_mode_on(void)
{
    if (!fullscreen_is_enabled) {
	x11kbd_focus_change();
        fullscreen_set_mode((resource_value_t) 1, NULL);
        ui_update_menus();
        return 0;
    }
    return 1;
}

int fullscreen_mode_off(void)
{
    fullscreen_is_enabled_restore = 0;
    if (fullscreen_is_enabled) {
	x11kbd_focus_change();
#ifdef CHECK_DGA_V2
	XDGASync(display, screen);
	fullscreen_dispatch_events_2();
#endif
        fullscreen_set_mode(0, NULL);
        ui_update_menus();
        return 1;
    }
    return 0;
}

void fullscreen_mode_on_restore(void)
{
    printf("fs-on_restore: %d\n", fullscreen_is_enabled_restore);
  if (fullscreen_is_enabled_restore)
    fullscreen_is_enabled_restore = 
	fullscreen_request_set_mode((resource_value_t) 1, (void*)1);
}

void fullscreen_mode_off_restore(void)
{
#ifdef CHECK_DGA_V2
    printf("fs-off_restore: %d\n", fullscreen_is_enabled_restore);
#endif     
    fullscreen_is_enabled_restore = 
	fullscreen_request_set_mode((resource_value_t) 0, (void*)0);;
}

void fullscreen_mode_init(void)
{
    fullscreen_set_bestmode(fullscreen_selected_videomode_at_start, NULL);
    if (fullscreen_selected_videomode_index == -1 && fullscreen_bestmode_counter > 0)
        fullscreen_selected_videomode_index = fullscreen_bestmodes[0].modeindex;

    fb_depth = x11ui_get_display_depth() / 8;

    if (fullscreen_use_fullscreen_at_start) {
        fullscreen_mode_on();
    }
    ui_update_menus();
}

void fullscreen_mode_exit(void)
{
    fullscreen_mode_off();
#ifdef CHECK_DGA_V2
    if (fullscreen_dga_major <= 1) {
#endif
#ifdef CHECK_DGA_V2
    } else {
        XFree(fullscreen_allmodes_dga2);
    }
#endif
}

int fullscreen_available(void)
{
    return (fullscreen_bestmode_counter ? 1 : 0);
}

int fullscreen_available_modes(void)
{
    return fullscreen_bestmode_counter;
}

char *fullscreen_mode_name(int mode)
{
    return fullscreen_bestmodes[mode].name;
}

#ifdef CHECK_DGA_V2

void fullscreen_mode_update(void)
{
    if (request_fullscreen_mode == 1)
	fullscreen_mode_on();
    if (request_fullscreen_mode == 2)
	fullscreen_mode_off();
    request_fullscreen_mode = 0;
}

static void fullscreen_dispatch_events_2(void)
{
    XDGAEvent event;
    XKeyEvent xk;
    KeySym key;
    XComposeStatus compose;
    static char buffer[20];
    
    while (XPending(display))
    {
	XNextEvent(display, (XEvent *) &event);
	switch (event.type - EventBase)
	{
	case KeyPress:
 	    XDGAKeyEventToXKeyEvent(&(event.xkey), &xk); 
	    XLookupString(&xk, buffer, 20, &key, &compose);
#ifdef FS_DEBUG
	    if (key == XK_d)
		fullscreen_request_set_mode(0, NULL);
#endif
	    x11kbd_press((signed_long)key);
	    break;
	    
	case KeyRelease:
 	    XDGAKeyEventToXKeyEvent(&(event.xkey), &xk);
	    XLookupString(&xk, buffer, 20, &key, &compose);
	    x11kbd_release((signed long)key);
	    break;

	default:
	    break;
	}
    }
}

static void fullscreen_dispatch_events(void)
{
    fullscreen_dispatch_events_2();
    fullscreen_mode_update();
}

#endif

#endif /* if 0 for warning */
#endif /* USE_XF86_DGA2_EXTENSIONS */
