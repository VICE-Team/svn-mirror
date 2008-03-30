/*
 * dga2.c
 *
 * Written by
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
#define FS_TRACE_REFRESH
#define FS_DEBUG_BUFFER
#define FS_DEBUG
*/

#include "vice.h"

#ifndef USE_XF86_DGA2_EXTENSIONS
#error "DGA2 extension not available in config.h, check config.log"
#endif


#ifdef FS_DEBUG
void dump_fb(char *wo);
#endif
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <X11/extensions/xf86dga.h>
#define DGA_MINMAJOR 2
#define DGA_MINMINOR 0

#include "kbd.h"
#include "machine.h"
#include "mouse.h"
#include "resources.h"
#include "raster/raster.h"
#include "palette.h"
#include "log.h"
#include "ui.h"
#include "uimenu.h"
#include "uisettings.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"
#include "vsyncapi.h"
#include "x11ui.h"
#include "fullscreen-common.h"


typedef struct {
  int modeindex;
  char name[30];
} fs_bestvideomode_t;

static fs_bestvideomode_t fs_bestmodes[10];

/* dga v2 */
static XDGADevice* dgadev=NULL; 
static XDGAMode *fs_allmodes_dga2;
#ifdef FS_PIXMAP_DGA
static GC gcContext;
#endif

static Colormap cm;
static void_hook_t old_ui_hook;

static int fs_vidmodecount;
static video_canvas_t *fs_cached_fb;
static int fs_canvas_width, fs_canvas_height;
static BYTE *fs_fb_data;
static int fs_fb_bpl, fs_cached_db_width, fs_depth;
static int fs_bestmode_counter;
static palette_t *fs_cached_palette;
static DWORD *fs_saved_colors;
static int new_palette;
static BYTE *fs_cached_pixels;
static int fs_available = 0;
static int fs_use_fs_at_start = 0;
static int timeout;
static int fullscreen_is_enabled_restore;
static int EventBase, ErrorBase;

int request_fs_mode = 0;
int fs_selected_videomode = -1;
char *fs_selected_videomode_at_start;
int fs_width, fs_height;

static unsigned char *fb_addr, *fb_dump;
static unsigned char *fb_render_target;
static unsigned offs, pitch;

#define BB_DEPTH 2
static unsigned char *fb_page[BB_DEPTH];
static unsigned int fb_offs[BB_DEPTH];
static unsigned int fb_ybegin[BB_DEPTH];
static int fb_current_page = 0;
static int fb_ybegin_static = 0;

static Display *display;
extern int screen;

int fullscreen_mode_on (void);
int fullscreen_mode_off (void);
static void fullscreen_dispatch_events(void);
static void fullscreen_dispatch_events_2(void);

static log_t dga_log = LOG_ERR;

/* ---------------------------------------------------------------------- */
#ifdef FS_TRACE_REFRESH
void ts_diff(struct timespec *ts1, struct timespec *ts2, 
	     struct timespec *d)
{
    d->tv_nsec = ts2->tv_nsec - ts1->tv_nsec;
    d->tv_sec = ts2->tv_sec - ts1->tv_sec;
    if (d->tv_nsec < 0)
    {
	d->tv_nsec += 1000000000L;
	d->tv_sec--;
    }
}
#endif
/* ---------------------------------------------------------------------- */

void alarm_timeout(int signo) {
    volatile int i = 1;
    
    XDGASetMode(display, screen, 0);
    XDGACloseFramebuffer(display, screen);
    fullscreen_is_enabled = 0;
    while (i)
    {
	log_message(dga_log, _("Attach debugger to pid %d..."), 
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

void fullscreen_refresh_func(struct canvas_s *canvas, 
			     int src_x, int src_y,
			     int dest_x, int dest_y,
			     unsigned int width, unsigned int height) 
{
#ifndef FS_DEBUG_BUFFER
    int oldp;

#if 0
    printf("curr page = %d, src_x = %d, src_y = %d, dest_x = %d, dest_y = %d, w = %d, h = %d\n",
	   fb_current_page, 
	   src_x, src_y, dest_x, dest_y, width, height);
#endif
#ifdef FS_TRACE_REFRESH
    struct timespec ts1, ts2, d;
    clock_gettime (CLOCK_REALTIME, &ts1);
#endif
    if (XDGAGetViewportStatus(display, screen))
    {
	log_message(dga_log, _("refresh, not ready, skipping frame"));
	goto end;
    }

    if ((dest_y + height) > fs_height)
      height = fs_height;

    if ((dest_x + width) > fs_width)
      width = fs_width;

    /* convert buffer for PAL emulation */
    video_canvas_render(canvas, 
		        fb_render_target,
		        width, height, src_x, src_y, src_x, src_y, 
		        fs_fb_bpl, fs_depth);

    XDGASetViewport(display, screen, 
		    0, fb_ybegin[fb_current_page], 
		    XDGAFlipRetrace);
    oldp = fb_current_page;
    fb_current_page = ((++fb_current_page) % BB_DEPTH);
    XDGACopyArea(display, screen,
		 src_x,  fb_ybegin_static + src_y,
		 width, height, 
		 dest_x, fb_ybegin[fb_current_page] + dest_y);
    XDGASync(display, screen);
/*
    f->tmpframebuffer = fb_page[fb_current_page]; 
*/

  end:
#ifdef FS_TRACE_REFRESH
    clock_gettime (CLOCK_REALTIME, &ts2);
    ts_diff(&ts1, &ts2, &d);
    
    printf("%09ld.%09ld - %09ld.%09ld: %09ld.%09ld\n",
	   ts1.tv_sec, ts1.tv_nsec,
	   ts2.tv_sec, ts2.tv_nsec,
	   d.tv_sec, d.tv_nsec);
#endif /* FS_TRACE_REFRESH */
#else 
    dump_fb("refresh");
#endif /* FS_DEBUG_BUFFER */
    return;
}   

int fullscreen_vidmode_available(void)
{
    int MajorVersion, MinorVersion;
    int i, j;

    fs_bestmode_counter = 0;
    fs_available = 0;

    display = x11ui_get_display_ptr();

#if 0
#ifndef FS_DEBUG
    if (XF86DGAForkApp(XDefaultScreen(display)) != 0)
    {
	log_error(dga_log, "Can't fork for DGA; quitting");
	return 1;
    }
    log_message(dga_log, _("Successfully forked DGA"));
#endif
#endif
    if (! XDGAQueryVersion (display, &MajorVersion, &MinorVersion)) {
        log_error(dga_log, 
		  _("Unable to query video extension version - disabling fullscreen."));
	fs_available = 0;
        return 1;
    }
    if (! XDGAQueryExtension (display, &EventBase, &ErrorBase)) {
        log_error(dga_log, _("Unable to query video extension information - disabling fullscreen."));
	fs_available = 0;
        return 1;
    }  

    if (MajorVersion < DGA_MINMAJOR || 
	(MajorVersion == DGA_MINMAJOR && MinorVersion < DGA_MINMINOR))
    {
        log_error(dga_log, 
		  _("Xserver is running an old XFree86-DGA version (%d.%d) "), 
		  MajorVersion, MinorVersion);
        log_error(dga_log, 
		  _("Minimum required version is %d.%d - disabling fullscreen."),
	          DGA_MINMAJOR, DGA_MINMINOR);
	fs_available = 0;
        return 1;
    }

    fs_allmodes_dga2 = XDGAQueryModes(display,screen, &fs_vidmodecount);
    for (i = 0; i < fs_vidmodecount; i++) 
    {
/*
	if (fs_allmodes_dga2[i].viewportWidth <= 800 &&
	    fs_allmodes_dga2[i].viewportWidth >= 320 &&
	    fs_allmodes_dga2[i].viewportHeight <= 600  &&
	    fs_allmodes_dga2[i].viewportHeight >= 200 &&
	    fs_allmodes_dga2[i].depth == x11ui_get_display_depth() &&
	    fs_allmodes_dga2[i].visualClass != DirectColor &&
	    (fs_allmodes_dga2[i].flags & XDGAPixmap)) 
*/
	if ((fs_allmodes_dga2[i].viewportWidth <= 800) &&
	    (fs_allmodes_dga2[i].viewportWidth >= 320) &&
	    (fs_allmodes_dga2[i].viewportHeight <= 600)  &&
	    (fs_allmodes_dga2[i].viewportHeight >= 200) &&
#if 0 /* works when you define a mode named "vice" in your XF86Config */
	    (strcmp(fs_allmodes_dga2[i].name, "vice") == 0) &&
#endif
	    (fs_allmodes_dga2[i].depth >= 16) && 
	    (fs_allmodes_dga2[i].viewportFlags & XDGAFlipRetrace) && 
	    (fs_allmodes_dga2[i].flags & XDGABlitRect)
	    )
	{
	    log_message(dga_log, _("Found suitable mode for fullscreen."));
	    for (j = 0; j < fs_bestmode_counter; j++) 
	    {
		if ( (fs_allmodes_dga2[fs_bestmodes[j].modeindex].viewportWidth == fs_allmodes_dga2[i].viewportWidth) &&
		     fs_allmodes_dga2[fs_bestmodes[j].modeindex].viewportHeight == fs_allmodes_dga2[i].viewportHeight) 
		    break;
	    }
	    if ( j == fs_bestmode_counter) 
	    {
		fs_bestmodes[fs_bestmode_counter].modeindex=i;
		snprintf(fs_bestmodes[fs_bestmode_counter].name,17," %ix%i-%.1fHz",
			 fs_allmodes_dga2[i].viewportWidth ,
			 fs_allmodes_dga2[i].viewportHeight,
			 fs_allmodes_dga2[i].verticalRefresh);
		log_message(dga_log, "Found mode: %s, %d",
			    fs_bestmodes[fs_bestmode_counter].name,
			    fs_bestmodes[fs_bestmode_counter].modeindex);
		
		fs_bestmode_counter++;
	    }
	    if (fs_bestmode_counter == 10) 
		break;
	}
	
    }
    if (fs_vidmodecount == 0)
	return 0;
    fs_available = 1;
    fs_selected_videomode = 0;
    resources_set_value("SelectedDGA2Mode", 
			(resource_value_t) fs_selected_videomode);

    return 1;
}

int fullscreen_request_set_mode(resource_value_t v, void *param)
{
    if (!v)
    {
	request_fs_mode = 2; /* toggle to window mode */
	if (old_ui_hook)
	    (void) vsync_set_event_dispatcher(old_ui_hook);
    }
    else
    {
	request_fs_mode = 1; /* toggle to fullscreen mode */
	old_ui_hook = vsync_set_event_dispatcher(fullscreen_dispatch_events);
    }

    if (param)
    {
	request_fs_mode = 0;
	if (v)
	    return fullscreen_mode_on();
	else
	    return fullscreen_mode_off();
    }
    
    return 0;
}

void fullscreen_set_canvas(video_canvas_t *c)
{
    fs_cached_fb = c;
}

void fullscreen_resize(int w, int h)
{
    fs_canvas_width = w;
    fs_canvas_height = h;
}

static BYTE *fs_cached_pixel_values;
static DWORD fs_cached_physical_colors[256]; /* from video.h */

void fullscreen_set_palette(video_canvas_t *c, const palette_t *palette)
{
    fs_cached_palette = palette;
    fs_cached_pixels = pixels;
    if (fs_cached_pixel_values)
	free(fs_cached_pixel_values);
    fs_cached_pixel_values = malloc(palette->num_entries * sizeof(BYTE));
    if (!fs_cached_pixel_values)
    {
	log_error(dga_log, _("Couldn't allocate pixel value cache."));
	exit(1);
    }
    
    memcpy(fs_cached_pixel_values, pixels, 
	   palette->num_entries * sizeof(BYTE));
    memcpy(fs_cached_physical_colors, c->videoconfig->physical_colors,
	   sizeof(DWORD) * 256);
    new_palette = 1;
}

int fs_draw_buffer_alloc(struct video_canvas_s *c, BYTE **draw_buffer, 
			 unsigned int w, unsigned int h, unsigned int *pitch)
{
    *draw_buffer = xmalloc (w * h);
    fs_fb_data = *draw_buffer;
    fs_cached_db_width = w;
    if (*draw_buffer)
	return 0;
    return 1;
}

void fs_draw_buffer_free(struct video_canvas_s *c, BYTE *draw_buffer)
{
    free(draw_buffer);
}

void fs_draw_buffer_clear(struct video_canvas_s *c, BYTE *draw_buffer, 
			  BYTE value, unsigned int w, unsigned int h,
			  unsigned int pitch)
{
    memset(draw_buffer, value, w * h);
}

int fullscreen_set_mode(resource_value_t v, void *param)
{
    static int interval,prefer_blanking,allow_exposures;
    XColor color;
    int i;

    if ( !fs_available || !fs_bestmode_counter ) {
        fs_use_fs_at_start = (int) v;
	return 0;
    }
    if ( fs_selected_videomode == -1) {
	return 0;
    }

    if (v && !fullscreen_is_enabled) 
    {
#ifdef XXX_CHECK_DGA2
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
	log_message(dga_log, _("Switch to fullscreen %ix%i"),
		    fs_allmodes_dga2[fs_selected_videomode].viewportWidth,
		    fs_allmodes_dga2[fs_selected_videomode].viewportHeight);

	if (XDGAOpenFramebuffer(display, screen) == False)
	{
	    log_message(dga_log, 
			_("Need root privileges for DGA2 fullscreen"));
	    fullscreen_request_set_mode(0, NULL);
	    return 0;
	}

	dgadev = XDGASetMode(display, screen,
			     fs_allmodes_dga2[fs_selected_videomode].num); 
	if (!dgadev) 
	{
	    log_error(dga_log, 
		      _("Error switching to fullscreen (SetMode) %ix%i"),
		      fs_allmodes_dga2[fs_selected_videomode].viewportWidth, 
		      fs_allmodes_dga2[fs_selected_videomode].viewportHeight);
	    return 0;
	}

#if 0
	if (!dgadev->pixmap) 
	{
	    log_error(dga_log, 
		      _("Error switching to fullscreen (pixmap) %ix%i"),
		      fs_allmodes_dga2[fs_selected_videomode].viewportWidth, 
		      fs_allmodes_dga2[fs_selected_videomode].viewportHeight);
	    XDGASetMode(display,screen,0);
	    XFree(dgadev);
	    return 0;
	}
#endif

	fb_addr = dgadev->data;
/*
	fb_width = fs_allmodes_dga2[fs_selected_videomode].bytesPerScanline; 
*/
	fs_width = dgadev->mode.viewportWidth;
	fs_height = dgadev->mode.viewportHeight;
	pitch = dgadev->mode.bytesPerScanline / 
	    (dgadev->mode.bitsPerPixel >> 3);
	offs = dgadev->mode.bytesPerScanline * fs_height;
	for (i = 0; i < BB_DEPTH; i++)
	{
	    fb_offs[i] = i * offs;
	    fb_ybegin[i] = i * fs_height;
	    fb_page[i] = fb_addr + fb_offs[i];
	    
#ifdef FS_DEBUG	    
	    log_message(dga_log, "page: %p, offs %d, ybegin: %d", 
			fb_page[i], fb_offs[i], fb_ybegin[i]);
#endif	    
	}
	fb_ybegin_static = i*fs_height;	/* framebuffer is the fixed 
					   last page */
	if (fb_ybegin_static > dgadev->mode.maxViewportY)
	{
	    log_message(dga_log, 
			_("Not enough video memory pages in mode %s, disabling fullscreen (%d,%d)."),
			dgadev->mode.name, fb_ybegin_static, dgadev->mode.maxViewportY);
	    fullscreen_request_set_mode(0, NULL);
	    goto nodga;
	}
	
	fb_current_page = 0;

#ifdef FS_PIXMAP_DGA
	gcContext =  XCreateGC(display, dgadev->pixmap, 0, 0);
	if (!gcContext) {
	    log_error(dga_log, 
		      _("Error switching to fullscreen (CreateGC) %ix%i"),
		      fs_allmodes_dga2[fs_selected_videomode].viewportWidth, 
		      fs_allmodes_dga2[fs_selected_videomode].viewportHeight);
	    XDGASetMode(display,screen,0);
	    XFree(dgadev);
	    ui_display_paused(1);
	    return 0;
	}
#endif

	XDGASelectInput(display, screen,
			PointerMotionMask | 
			ButtonPressMask | 
			KeyPressMask | 
			KeyReleaseMask);
	
	if (new_palette)
	{
	    cm = XDGACreateColormap(display, screen, dgadev, AllocNone);
	
	    for (i = 0; i < fs_cached_palette->num_entries; i++)
	    {
		color.blue = (fs_cached_palette->entries[i].blue << 8);
		color.red = (fs_cached_palette->entries[i].red << 8);
		color.green = (fs_cached_palette->entries[i].green << 8);
		color.flags = DoRed | DoGreen | DoBlue;
		XAllocColor(display, cm, &color);
#ifdef FS_DEBUG
		log_message (dga_log, "colors[%s]: %x", 
			     fs_cached_palette->entries[i].name, 
			     fs_cached_pixels[i]);
#endif
		video_render_setphysicalcolor(fs_cached_fb->videoconfig, 
					      i, color.pixel, fs_depth);
		fs_cached_pixels[i] = color.pixel;
	    }
	    
	    /* Save pixel values of fullscreen mode for reuse in case no new
	       palette is allocated until next fullscreen activation */
	    if (fs_saved_colors)
		free(fs_saved_colors);
	    fs_saved_colors = (DWORD *)malloc (sizeof(DWORD) * 256);
	    if (!fs_saved_colors)
	    {
		log_error(dga_log, _("Couldn't allocate color cache"));
		goto nodga;
	    }
	    
	    memcpy(fs_saved_colors, 
		   fs_cached_fb->videoconfig->physical_colors, 
		   sizeof(DWORD) * 256);

	    new_palette = 0;
	}
	else
	{
	    /* Reuse pixel values from earlier activation */
	    if (fs_saved_colors == NULL)
	    {
		log_error(dga_log, "inconsistent view for color management, disabling fullscreen.");
		goto nodga;
	    }
	    memcpy(fs_cached_fb->videoconfig->physical_colors,
		   fs_saved_colors, sizeof(DWORD) * 256);
	}
	
	XDGAInstallColormap(display, screen, cm);
#ifdef FS_PIXMAP_DGA
	XDGAChangePixmapMode(display, screen, &pm_x, &pm_y, 
			     XDGAPixmapModeLarge);
#endif

	XDGASetViewport (display, screen, 0, 0, XDGAFlipImmediate);
	while(XDGAGetViewportStatus(display, screen));
#ifdef FS_PIXMAP_DGA
	XSetForeground(display, gcContext, 0);
	XFillRectangle(display, dgadev->pixmap, gcContext, 0, 0, 
		       fs_allmodes_dga2[fs_selected_videomode].maxViewportX, 
		       fs_allmodes_dga2[fs_selected_videomode].maxViewportY);
#endif
	XFlush(display);
	fullscreen_is_enabled = 1;

#ifdef FS_DEBUG
	log_message(dga_log, "membase = %p, pitch = %i, offs = %i", 
		    fb_addr, pitch, offs);
#endif
	/* add a segfault handler, just in case fullscreen bombs, it 
	   restores the window mode */
	set_alarm_timeout();

#ifdef FS_DEBUG_BUFFER
	fb_addr = malloc(offs * (BB_DEPTH + 1) * sizeof(char));
#endif
	for (i = 0; i < (BB_DEPTH + 1); i++)
	    memset (fb_addr + i * offs, 0, offs);

	fs_fb_bpl = dgadev->mode.bytesPerScanline;
	fs_depth = dgadev->mode.depth;
	fb_render_target = fb_dump = fb_addr + ((i - 1)* offs);
	
	raster_force_repaint(fs_cached_raster);
	raster_rebuild_tables(fs_cached_raster);
	raster_resize_viewport(fs_cached_raster, fs_width, fs_height);
#ifdef FS_DEBUG_BUFFER
	goto nodga;
#endif
    }
    
    if (!v && fullscreen_is_enabled) 
    {
        log_message(dga_log, _("Switch to windowmode"));

	/* Restore framebuffer details */
#ifndef FS_DEBUG_BUFFER
	fb_addr = (unsigned char *)0;
	/* Restore pixel values of window mode */
	memcpy(fs_cached_fb->videoconfig->physical_colors,
	       fs_cached_physical_colors,
	       sizeof(DWORD) * 256);
	memcpy(fs_cached_pixels, fs_cached_pixel_values,
	       fs_cached_palette->num_entries * sizeof(BYTE));
#endif
	raster_resize_viewport(fs_cached_raster, fs_canvas_width, 
			       fs_canvas_height);
	raster_rebuild_tables(fs_cached_raster);
	raster_force_repaint(fs_cached_raster);

      nodga:
	XDGASetMode(display, screen, 0);
	XDGACloseFramebuffer(display, screen);
	XFree(dgadev); 
#ifdef FS_PIXMAP_DGA
	XFreeGC(display, gcContext);
#endif
	XSetScreenSaver(display, timeout, interval, 
			prefer_blanking, allow_exposures);

	XUngrabPointer(display, CurrentTime);
	XUngrabKeyboard(display, CurrentTime);
	fullscreen_is_enabled = 0;
    }
#if 0
    machine_video_refresh();
#endif
    ui_check_mouse_cursor();
    return 1;
}

int fullscreen_set_bestmode(resource_value_t v, void *param)
{
    int i = (int) v;

    if (i < 0 || fs_available == 0)
	return 0;

    log_message(dga_log, "selected mode: %s", fs_bestmodes[i].name);
    fs_selected_videomode = fs_bestmodes[i].modeindex;

    return 0;
    
}

#if 0
int fs_set_bestmode(resource_value_t v, void *param)
{
    int i;

    if (!v)
	return 0;
    
    if (!fs_vidmodeavail) {
        fs_selected_videomode_at_start = (char*) stralloc(v);
        return(0);
    }
    fs_selected_videomode = (char*) v;
    for (i = 0; i < fs_bestmode_counter; i++) {
        if (! strcmp(fs_selected_videomode, fs_bestmodes[i].name)) {
	    fs_selected_videomode_index = fs_bestmodes[i].modeindex;
	    if (fullscreen_is_enabled)
                fullscreen_set_mode((resource_value_t) 1, NULL);
	    return(0);
	}
    }
    if (fs_bestmode_counter > 0) {
        fs_selected_videomode_index = fs_bestmodes[0].modeindex;
        fs_selected_videomode = fs_bestmodes[0].name;
    }

    return(0);
}
#endif

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
	XDGASync(display, screen);
	fullscreen_dispatch_events_2();
        fullscreen_set_mode(0, NULL);
        ui_update_menus();
        return 1;
    }
    return 0;
}

void fullscreen_mode_on_restore(void)
{
#ifdef FS_DEBUG
    printf("fs-on_restore: %d\n", fullscreen_is_enabled_restore);
#endif
    if (fullscreen_is_enabled_restore)
	fullscreen_is_enabled_restore = 
	    fullscreen_request_set_mode((resource_value_t) 1, (void*)1);
}

void fullscreen_mode_off_restore(void)
{
#ifdef FS_DEBUG
    printf("fs-off_restore: %d\n", fullscreen_is_enabled_restore);
#endif     
    fullscreen_is_enabled_restore = 
	fullscreen_request_set_mode((resource_value_t) 0, (void*)0);;
}

void fullscreen_mode_init(void)
{
    if (dga_log == LOG_ERR)
	dga_log = log_open("DGA2");
    
#if 0
    fullscreen_set_bestmode(fs_selected_videomode_at_start, NULL);
#endif
    if (fs_selected_videomode == -1 && fs_bestmode_counter > 0)
        fs_selected_videomode = fs_bestmodes[0].modeindex;

    if (fs_use_fs_at_start) {
        fullscreen_mode_on();
    }
    ui_update_menus();
}



void fullscreen_mode_exit(void)
{
    fullscreen_mode_off();
    XFree(fs_allmodes_dga2);
}

int dga2_available(void)
{
    return (fs_bestmode_counter ? 1 : 0);
}

int fullscreen_available_modes(void)
{
    return fs_bestmode_counter;
}

char *fullscreen_mode_name(int mode)
{
    return fs_bestmodes[mode].name;
}

UI_MENU_DEFINE_RADIO(SelectedDGA2Mode);

void fullscreen_create_menus(void)
{
    int i, index = -1;
    char buf[50];
    ui_menu_entry_t *resolutions_submenu;

    buf[0] = '*';
    buf[50] = '\0';

    resolutions_submenu = (ui_menu_entry_t*)xmalloc(sizeof(ui_menu_entry_t) *
                          (size_t)(fs_bestmode_counter + 1));

    for(i = 0; i < fs_bestmode_counter ; i++) {

        buf[1] = '\0';
        strncat(buf + 1, fs_bestmodes[i].name, 48);
        resolutions_submenu[i].string =
            (ui_callback_data_t) stralloc(buf);
        resolutions_submenu[i].callback =
            (ui_callback_t) radio_SelectedDGA2Mode;
        resolutions_submenu[i].callback_data =
            (ui_callback_data_t) fs_bestmodes[i].modeindex;
        resolutions_submenu[i].sub_menu = NULL;
        resolutions_submenu[i].hotkey_keysym = 0;
        resolutions_submenu[i].hotkey_modifier =
            (ui_hotkey_modifier_t) 0;
    }

    resolutions_submenu[i].string = (ui_callback_data_t) NULL;
    i = 0;
    while (ui_fullscreen_settings_submenu[i].string)
    {
	if (strncmp(ui_fullscreen_settings_submenu[i].string, 
		    "DGA2", 4) == 0)
	{
	    index = i;
	    break;
	}
	i++;
    }
    
    if (fs_bestmode_counter > 0 && index >= 0)
        ui_fullscreen_settings_submenu[index].sub_menu = resolutions_submenu;

}

void fullscreen_mode_update(void)
{
    if (request_fs_mode == 1)
	fullscreen_mode_on();
    if (request_fs_mode == 2)
	fullscreen_mode_off();
    request_fs_mode = 0;
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
	    if (key == XK_u)
		dump_fb("on demand");
#endif
	    x11kbd_press((signed long)key);
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

#ifdef FS_DEBUG
int fs_count = 0;
void dump_fb(char *wo)
{
    int x, y;
    
    if (!fb_dump)
	return;

    for (y = 0; y < 100; y++)
    {
	printf("%d, %s: %2d: ", fs_count, wo, y);
	for (x = 0 ; x < 60; x++)
	{
	    printf("%02x", (int) *(fb_dump + 200+x + (y * pitch)));
	}
	printf("\n");
    }
    fs_count++;
}
#endif
