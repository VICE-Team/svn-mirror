/*
 * ui.c - Gnome/Gtk+ based UI
 *
 * Written by
 *  Ettore Perazzoli
 *  Oliver Schaertel
 *  pottendo <pottendo@gmx.net>
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

/* #define DEBUG_X11UI */
/* #define DEBUGMOUSECURSOR */   /* dont use a blank mouse cursor */
/* #define DEBUGNOMOUSEGRAB */  /* dont grab mouse */
/* #define DEBUGNOKBDGRAB */    /* dont explicitly grab keyboard focus */

#ifdef DEBUG_X11UI
#define DBG(_x_) log_debug _x_
#else
#define DBG(_x_)
#endif

#define _UI_C

#include "vice.h"

#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pango/pango.h>

#ifdef ENABLE_NLS
#include <locale.h>
#endif

#ifdef HAVE_HWSCALE
#include <gtk/gtkgl.h>
#include <gdk/gdkgl.h>
#include <GL/gl.h>
#endif

#include "ui.h"
#include "uiapi.h"
#include "uiarch.h"

#include "autostart.h"
#include "charset.h"
#include "drive/drive.h"
#include "fullscreenarch.h"
#include "imagecontents.h"
#include "tapecontents.h"
#include "diskcontents.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "kbd.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "mousedrv.h"
#include "keyboard.h"
#include "resources.h"
#include "types.h"
#include "uiapi.h"
#include "uicolor.h"
#include "uimenu.h"
#include "uipalcontrol.h"
#include "uisettings.h"
#include "uicommands.h"
#include "uicontents.h"
#include "uifileentry.h"
#include "uilib.h"
#include "uistatusbar.h"
#include "uitapestatus.h"
#include "util.h"
#include "version.h"
#include "vsync.h"
#include "video.h"
#include "videoarch.h"
#include "vsidui.h"
#include "vsiduiunix.h"
#include "screenshot.h"
#include "vice-event.h"
#include "uifliplist.h"
#include "c128/c128.h"
#include "lightpen.h"
#include "lightpendrv.h"

/* FIXME: move code  (drivestatus init) so these are not needed here */
extern ui_drive_enable_t enabled_drives;
extern int *drive_active_led;
extern GdkColor drive_led_on_red_pixel, drive_led_on_green_pixel;

#ifdef USE_XF86_EXTENSIONS
#include <gdk/gdkx.h>
#endif
#include "x11ui.h"

#ifdef USE_XF86_EXTENSIONS
static Display *display;
int screen;
static int depth;
#endif

#if !GTK_CHECK_VERSION(2, 12, 0)
void gtk_widget_set_tooltip_text(GtkWidget * widget, const char * text)
{
    static GtkTooltips * tooltips = NULL;

    if (tooltips == NULL) {
        tooltips = gtk_tooltips_new();
        gtk_tooltips_enable(tooltips);
    }
    gtk_tooltips_set_tip(tooltips, widget, text, NULL);
}
#endif

/* UI logging goes here.  */
static log_t ui_log = LOG_ERR;

#define VSID_WINDOW_MINW     (400)
#define VSID_WINDOW_MINH     (300)

#define WINDOW_MINW     (320 / 2)
#define WINDOW_MINH     (200 / 2)

/* FIXME: move respective code (statusbar) so these are not needed here */
extern GtkWidget *video_ctrl_checkbox;
extern GtkWidget *event_rec_checkbox;
extern GtkWidget *event_playback_checkbox;
extern GtkWidget *video_ctrl_checkbox_label;
extern GtkWidget *event_rec_checkbox_label;
extern GtkWidget *event_playback_checkbox_label;

static GtkStyle *ui_style_red;
static GtkStyle *ui_style_green;
static GdkCursor *blankCursor;

char *fixedfontname="CBM 10";
int have_cbm_font = 0;
PangoFontDescription *fixed_font_desc;

static int popped_up_count = 0;

/* Left-button and right-button menu.  */
static GtkWidget *left_menu, *right_menu;

/* Toplevel widget. */
static GdkGC *app_gc = NULL;

/* GdkColormap *colormap; */

app_shell_type app_shells[MAX_APP_SHELLS];
static unsigned int num_app_shells = 0;
static unsigned int active_shell = 0;

/* ------------------------------------------------------------------------- */

static gboolean enter_window_callback(GtkWidget *w, GdkEvent *e, gpointer p);
static gboolean leave_window_callback(GtkWidget *w, GdkEvent *e, gpointer p);
static gboolean configure_callback_app(GtkWidget *w, GdkEventConfigure *e, gpointer p);
static gboolean configure_callback_canvas(GtkWidget *w, GdkEventConfigure *e, gpointer p);
static gboolean exposure_callback_canvas(GtkWidget *w, GdkEventExpose *e, gpointer p);
static gboolean map_callback(GtkWidget *widget, GdkEvent  *event, gpointer user_data);

static gboolean update_menu_cb(GtkWidget *w, GdkEvent *event,gpointer data);

static void toggle_aspect(video_canvas_t *canvas);
static void setup_aspect(video_canvas_t *canvas);
static gfloat get_aspect(video_canvas_t *canvas);

void ui_trigger_resize(void);
gboolean kbd_event_handler(GtkWidget *w, GdkEvent *report,gpointer gp);

/******************************************************************************/
GdkGC *get_toplevel(void)
{
    return app_gc;
}

int get_num_shells(void)
{
    return num_app_shells;
}

static void set_active_shell(unsigned int shell)
{
    DBG(("set_active_shell (%d)", shell));
    if (shell >= num_app_shells) {
        log_error(ui_log, "set_active_shell: bad params (%d)", shell);
        return;
    }
    active_shell = shell;
}
int get_active_shell(void)
{
    return active_shell;
}
GtkWidget *get_active_toplevel(void)
{
    unsigned int key = get_active_shell();
    if (app_shells[key].shell) {
        return gtk_widget_get_toplevel(app_shells[key].shell);
    }
    return NULL;
}

video_canvas_t *get_active_canvas(void)
{
    unsigned int key = get_active_shell();
    return app_shells[key].canvas;
}

/******************************************************************************/

static char windowres[4][14] = { "Window0Xpos", "Window0Ypos", "Window0Width", "Window0height" };

static void set_window_resources(video_canvas_t *canvas, int x, int y, int w, int h)
{
    int i;

    if ((canvas == NULL) || (x < 0) || (y < 0) || (w < WINDOW_MINW) || (h < WINDOW_MINH)) {
        return;
    }

    DBG(("set_window_resources (%d) x:%d y:%d w:%d h:%d", canvas->app_shell, x, y, w, h));
    if ((canvas->app_shell >= num_app_shells) || (canvas != app_shells[canvas->app_shell].canvas)) {
        log_error(ui_log, "set_window_resources: bad params");
        return;
    }

    for (i = 0; i < 4; i++) {
        windowres[i][6] = '0' + canvas->app_shell;
    }
    resources_set_int(windowres[0], x);
    resources_set_int(windowres[1], y);
    resources_set_int(windowres[2], w);
    resources_set_int(windowres[3], h);
}

static void get_window_resources(video_canvas_t *canvas, int *x, int *y, int *w, int *h)
{
    int i;

    if ((canvas == NULL) || (x == NULL) || (y == NULL) || (w == NULL) || (h == NULL)) {
        return;
    }

    if ((canvas->app_shell >= num_app_shells) || (canvas != app_shells[canvas->app_shell].canvas)) {
        log_error(ui_log, "get_window_resources: bad params");
        return;
    }

    for (i = 0; i < 4; i++) {
        windowres[i][6] = '0' + canvas->app_shell;
    }
    resources_get_int(windowres[0], x);
    resources_get_int(windowres[1], y);
    resources_get_int(windowres[2], w);
    resources_get_int(windowres[3], h);
    DBG(("get_window_resources x:%d y:%d w:%d h:%d", *x, *y, *w, *h));
}

/******************************************************************************/
/*
    transfer focus to the monitor ui window

    note: the "focus stealing prevention" feature of eg KDE must be disabled or
          all this will not work at all.

    for the time being, this is a kindof nasty hack which is needed because the
    monitor is not actually running in a "real" gui window (but in a terminal
    window instead). once a real monitor gui is implemented it can be replaced
    by the proper gtk function(s) :)
*/

/*  return the parent pid for a given pid

    note: doing this correctly, cleanly and portable in an application is almost
          impossible. (http://www.steve.org.uk/Reference/Unix/faq_2.html#SEC17)

    because of that, the following function is implemented in a way that it will 
    likely work on a "typical" (linux) setup, without breaking any "exotic"
    setups and/or requiring non standard functions.

    FIXME: we should use the "ps" util as a fallback if reading from proc does
           not work.
 */
#if 0
/* this only works on linux, and requires libproc */
#include <proc/readproc.h>
static pid_t get_ppid_from_pid(pid_t pid) 
{
    proc_t process_info;
    get_proc_stats(pid, &process_info);
    return process_info.ppid;
}
#endif

#if 0
#define PROCSTATLEN     0x200
static pid_t get_ppid_from_pid(pid_t pid) 
{
    pid_t ppid = 0;
    FILE *f;
    char *p;
    char pscmd[0x40];
    char status[PROCSTATLEN + 1];
    int ret;

    sprintf(pscmd, "/proc/%d/status", pid);

    f = fopen(pscmd, "r");
    if (f == NULL) {
        return 0;
    }
    memset(status, 0, PROCSTATLEN + 1);
    ret = fread(status, 1, PROCSTATLEN, f);
    fclose(f);
    if (ret < 1) {
        return 0;
    }

    for (p = status; *p != 0; p++) {
        *p = util_tolower(*p);
    }

    p = strstr(status, "ppid:");
    if (p) {
        p+=5;
        while((*p != 0) && (*p == ' ')) {
            p++;
        }
        ppid = strtoul(p, NULL, 10);
        return ppid;
    }
    return 0;
}

/* check if winpid is an ancestor of pid, returns distance if found or 0 if not */
static unsigned long check_ancestor(pid_t winpid, pid_t pid)
{
    pid_t ppid;
    unsigned long n = 0;

    do {
        ppid = get_ppid_from_pid(pid);
        n++;
        if (winpid == ppid) {
            return n;
        }
        pid = ppid;

    } while (ppid > 1);

    return 0;
}

#include <X11/Xlib.h>
#include <X11/Xatom.h>
/* get list of client windows for given display */
static Window *getwinlist (Display *disp, unsigned long *len) 
{
    Atom prop = XInternAtom(disp, "_NET_CLIENT_LIST", False), type;
    int form;
    unsigned long remain;
    unsigned char *list;

    if (XGetWindowProperty(disp, XDefaultRootWindow(disp), prop, 0, 1024, False,
        XA_WINDOW, &type, &form, len, &remain, &list) != Success) {
        log_error(ui_log, "getwinlist: XGetWindowProperty");
        return 0;
    }

    return (Window*)list;
}

/* check window status for given window. returns 1 if the window is visible and
   may recieve window focus */
static int getwinstate (Display *disp, Window win)
{
    Atom prop = XInternAtom(disp, "_NET_WM_STATE", True), type;
    int form;
    unsigned long remain;
    unsigned char *list = NULL;
    unsigned long len;
    int i;

    if (XGetWindowProperty(disp, win, prop, 0, 1024, False, AnyPropertyType,
        &type, &form, &len, &remain, &list) != Success) {
        log_error(ui_log, "getwinstate: XGetWindowProperty");
        return 0;
    }

    if (list) {
        for (i = 0; i < (int)len; i++) {
            if (((long *)(list))[i] == XInternAtom(disp, "_NET_WM_STATE_HIDDEN", False)) {
                XFree(list);
                return 0;
            }
        }
        XFree(list);
    }
    return 1;
}

/* get the pid associated with a given window */
static pid_t getwinpid (Display *disp, Window win)
{
    Atom prop = XInternAtom(disp, "_NET_WM_PID", False), type;
    int form;
    unsigned long remain, len;
    unsigned char *pid = NULL;
    pid_t _pid;

    if (XGetWindowProperty(disp, win, prop, 0, 1024, False, XA_CARDINAL,
        &type, &form, &len, &remain, &pid) != Success || len < 1) {
        log_error(ui_log, "getwinpid: XGetWindowProperty");
        return 0;
    }

    _pid = *(pid_t*)pid;
    XFree(pid);
    return _pid;
}

int ui_focus_monitor(void) 
{
    int i;
    unsigned long len;
    Display *disp;
    Window *list;
    Window foundwin;
    pid_t winpid, mypid;
    unsigned long num, maxnum;

    DBG(("uimon_grab_focus"));

    if (!(disp = XOpenDisplay(NULL))) {
        log_error(ui_log, "uimon_grab_focus: no display");
        return -1;
    }

    mypid = getpid();
    maxnum = ~0;
    foundwin = 0;

    /* get list of all client windows on current display */
    list = (Window*)getwinlist(disp, &len);

    /* for every window, check if it is an ancestor of the current process. the
       one which is the closest ancestor will be the one we are interested in */
    for (i = 0; i < (int)len; i++) {
        winpid = getwinpid(disp, list[i]);
        num = check_ancestor(winpid, mypid);
        if (num) {
            DBG(("found: n:%d win:%d pid:%d\n", num, (int)list[i], winpid));
            if (num < maxnum) {
                maxnum = num;
                foundwin = list[i];
            }
        }
    }

    XFree(list);

    /* if a matching window was found, raise it and transfer focus to it */
    if (foundwin) {
        DBG(("using win: %d\n", (int)foundwin));
        XRaiseWindow(disp, foundwin);
        XFlush(disp);
        /* the window manager may ignore the request to raise the window (for
           example because it is minimized). we have to check if the window
           is actually visible, because a call to XSetInputFocus will crash if
           it is not. */
        if (getwinstate (disp, foundwin)) {
            XSetInputFocus(disp, foundwin, RevertToParent, CurrentTime);
        }
    }

    XCloseDisplay(disp);

    return 0;
}
#endif

/*
    restore the main emulator window and transfer focus to it. in detail this
    function should:

    - move the active toplevel shell window to top of the window stack
    - make sure the window is visible (move if offscreen, de-iconify, etc)
    - transfer the window managers keyboard focus to the window

    note: the "focus stealing prevention" feature of eg KDE must be disabled or
          all this will not work at all.

    this function is called by uimon_window_close and -_suspend, ie when exiting
    the ml monitor.
*/
void ui_restore_focus(void)
{
    GtkWidget *widget = get_active_toplevel();
    GdkWindow *window;

    window = widget ? widget->window : NULL;
    DBG(("ui_restore_focus %p:%p", window, widget));
    if (window) {
        ui_dispatch_events();
        gdk_flush();
        ui_unblock_shells();
        /* yes it looks weird, and it is. GTK sucks */
        gdk_window_raise(window);
        gdk_window_show(window);
        gtk_window_present(GTK_WINDOW(widget));
        gdk_window_focus(window, GDK_CURRENT_TIME);
    }
}

/******************************************************************************/

/*
    grab pointer, set mouse pointer shape

    called by: ui_check_mouse_cursor, ui_restore_mouse, x11ui_fullscreen

    TODO: also route lightpen stuff through this function
*/
static int mouse_grabbed = 0;
static void mouse_cursor_grab(int grab, GdkCursor *cursor)
{
#ifdef DEBUGNOMOUSEGRAB
    DBG(("mouse_cursor_grab disabled (%d)", grab));
#else
    GtkWidget *widget;
    GdkWindow *window;

    DBG(("mouse_cursor_grab (%d, was %d)", grab, mouse_grabbed));

    if (mouse_grabbed) {
        gdk_pointer_ungrab(GDK_CURRENT_TIME);
        mouse_grabbed = 0;
    }

    if (grab) {
        /*ui_dispatch_events();
        gdk_flush();*/

        widget = get_active_toplevel();
        window = widget ? widget->window : NULL;

        if ((widget == NULL) || (window == NULL)) {
            log_error(ui_log, "mouse_cursor_grab: bad params");
            return;
        }
#ifdef DEBUGMOUSECURSOR
        if (cursor == blankCursor) {
            DBG(("mouse_cursor_grab blankCursor disabled"));
            cursor = NULL;
        }
#endif
        gdk_pointer_grab(window, 1, GDK_POINTER_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK, window, cursor, GDK_CURRENT_TIME);
        mouse_grabbed = 1;
    }
#endif
}

#if 0
/*
    grab keyboard focus

    called by: ui_init_finalize
 */
static int keyboard_grabbed = 0;
static void keyboard_grab(int grab)
{
#ifdef DEBUGNOKBDGRAB
    DBG(("keyboard_grab disabled (%d)", grab));
#else
    GtkWidget *widget;
    GdkWindow *window;

    DBG(("keyboard_grab (%d, was %d)", grab, keyboard_grabbed));

    if (grab == keyboard_grabbed) {
        return;
    }

    /*ui_dispatch_events();
    gdk_flush();*/

    if (grab) {
        widget = get_active_toplevel();
        window = widget ? widget->window : NULL;

        if ((widget == NULL) || (window == NULL)) {
            log_error(ui_log, "keyboard_grab: bad params");
            return;
        }

        gdk_keyboard_grab(window, 1, GDK_CURRENT_TIME);
        keyboard_grabbed = 1;
    } else {
        gdk_keyboard_ungrab(GDK_CURRENT_TIME);
        keyboard_grabbed = 0;
    }
#endif
}
#endif

void ui_check_mouse_cursor(void)
{
#ifdef HAVE_FULLSCREEN
    video_canvas_t *canvas;
    if ((canvas = get_active_canvas()) == NULL) {
        log_error(ui_log, "ui_check_mouse_cursor canvas == NULL");
        mouse_cursor_grab(0, NULL);
        return;
    }
    if (canvas->fullscreenconfig == NULL) {
        log_error(ui_log, "ui_check_mouse_cursor canvas->fullscreenconfig == NULL");
        mouse_cursor_grab(0, NULL);
        return;
    }

    if (canvas->fullscreenconfig->enable) {
        if (_mouse_enabled) {
            mouse_cursor_grab(1, blankCursor);
        } else {
            /* FIXME: this case seems odd */
            mouse_cursor_grab(1, NULL);
        }
        return;
    }
#endif
    if (_mouse_enabled) {
        mouse_cursor_grab(1, blankCursor);
    } else {
        mouse_cursor_grab(0, NULL);
    }
}

/*
    ungrab mouse and restore mouse pointer shape

    called by uicommands.c:activate_monitor, ui_jam_dialog, ui_popup. ui_exit
*/
void ui_restore_mouse(void)
{
    mouse_cursor_grab(0, NULL);
}

static void initBlankCursor(void)
{
    static char cursor[] = { 0x00 };
    GdkColor fg = { 0, 0, 0, 0 };
    GdkColor bg = { 0, 0, 0, 0 };
    GdkBitmap *source = gdk_bitmap_create_from_data (NULL, cursor, 1, 1);
    GdkBitmap *mask = gdk_bitmap_create_from_data (NULL, cursor, 1, 1);

    blankCursor = gdk_cursor_new_from_pixmap (source, mask, &fg, &bg, 1, 1); 

    g_object_unref (source);
    g_object_unref (mask); 
}

/******************************************************************************/

void archdep_ui_init(int argc, char *argv[])
{
    /* Fake Gnome to see empty arguments; 
       Generaly we should use a `popt_table', either by converting the
       registered options to this, or to introduce popt in the generic part,
       case we have `libgnomeui' around.
       For now I discard gnome-specific options. FIXME MP */

    char *fake_argv[2];
    int fake_argc = 1;
    char **fake_args = fake_argv;

    if (console_mode) {
        return;
    }

    fake_argv[0] = argv[0];
    fake_argv[1] = NULL;
    gtk_init(&fake_argc, &fake_args);

#ifdef HAVE_HWSCALE
    gtk_gl_init_check(&fake_argc, &fake_args);
#endif
}

/* Initialize the GUI and parse the command line. */
int ui_init(int *argc, char **argv)
{

#ifdef USE_XF86_EXTENSIONS
    display = GDK_DISPLAY();
    depth = gdk_visual_get_system()->depth;
    screen = gdk_screen_get_number(gdk_screen_get_default());
#endif

    atexit(ui_autorepeat_on);

    ui_common_init();

    /* enabled_drives = UI_DRIVE_ENABLE_NONE; */

    return 0;
}

void ui_shutdown(void)
{
    ui_common_shutdown();
}

#if 0
typedef struct {
    char *name;
    GdkVisualType class;
} namedvisual_t;
#endif

/* exit the application */
static gboolean delete_event(GtkWidget *w, GdkEvent *e, gpointer data) 
{
    vsync_suspend_speed_eval();
    ui_exit();
    /* ui_exit() will exit the application if user allows it. So if
       we return here then we should keep going => return TRUE */
    return TRUE;
}

static gint mouse_dx = 0, mouse_dy = 0;
static gint mouse_lasteventx = 0, mouse_lasteventy = 0;
static gint mouse_warped = 0;
static gint mouse_warpx = 0, mouse_warpy = 0;
#define MOUSE_WRAP_MARGIN  50

void mouse_handler(GtkWidget *w, GdkEvent *event, gpointer data)
{
    video_canvas_t *canvas = (video_canvas_t *)data;

    if (event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*)event;
        if (_mouse_enabled || lightpen_enabled) {
            mouse_button(bevent->button-1, TRUE);
            gtk_lightpen_setbutton(bevent->button, TRUE);
        } else {
            if (bevent->button == 1) {
                ui_menu_update_all_GTK();
                gtk_menu_popup(GTK_MENU(left_menu), NULL, NULL, NULL, NULL, bevent->button, bevent->time);
            } else if (bevent->button == 3) {
                ui_menu_update_all_GTK();
                gtk_menu_popup(GTK_MENU(right_menu), NULL, NULL, NULL, NULL, bevent->button, bevent->time);
            }
        }
    } else if (event->type == GDK_BUTTON_RELEASE && (_mouse_enabled || lightpen_enabled)) {
        GdkEventButton *bevent = (GdkEventButton*)event;
        mouse_button(bevent->button-1, FALSE);
        gtk_lightpen_setbutton(bevent->button, FALSE);
   } else if (event->type == GDK_MOTION_NOTIFY) {
        GdkEventMotion *mevent = (GdkEventMotion*)event;
        if (_mouse_enabled) {
            /* handle pointer motion events for mouse emulation */
            gint x=0, y=0, w=0, h=0, warp=0;
            gint xoff=0, yoff=0;
            gint ptrx, ptry;
            GdkDisplay *display = NULL;
            GdkScreen *screen = NULL;

            /* get default display and screen */
            display = gdk_display_get_default ();
            screen = gdk_display_get_default_screen (display);

            /* get cursor position */
            gdk_display_get_pointer (display, NULL, &x, &y, NULL);

            ptrx = (int)mevent->x;
            ptry = (int)mevent->y;
            xoff = x - ptrx;
            yoff = y - ptry;

            w = canvas->draw_buffer->canvas_physical_width;
            h = canvas->draw_buffer->canvas_physical_height;

            /* DBG(("ptrx:%d ptry:%d x:%d y:%d w:%d h:%d", ptrx, ptry, x, y, w, h)); */

            if (mouse_warpx == 1) {
                /* from left to right */
                if ((ptrx > mouse_lasteventx) && (ptrx >= (w - (MOUSE_WRAP_MARGIN * 2))) && (ptrx <= (w - MOUSE_WRAP_MARGIN))) {
                    mouse_warpx = 0;
                    mouse_lasteventx = ptrx;
                }
            } else if (mouse_warpx == 2) {
                /* from right to left */
                if ((ptrx < mouse_lasteventx) && (ptrx <= (MOUSE_WRAP_MARGIN * 2)) && (ptrx >= MOUSE_WRAP_MARGIN)) {
                    mouse_warpx = 0;
                    mouse_lasteventx = ptrx;
                }
            }

            if (mouse_warpy == 1) {
                /* from top to bottom */
                if ((ptry > mouse_lasteventy) && (ptry >= (h - (MOUSE_WRAP_MARGIN * 2))) && (ptry <= (h - MOUSE_WRAP_MARGIN))) {
                    mouse_warpy = 0;
                    mouse_lasteventy = ptry;
                }
            } else if (mouse_warpy == 2) {
                /* from bottom to top */
                if ((ptry < mouse_lasteventy) && (ptry <= (MOUSE_WRAP_MARGIN * 2)) && (ptry >= MOUSE_WRAP_MARGIN)) {
                    mouse_warpy = 0;
                    mouse_lasteventy = ptry;
                }
            }

            if (mouse_warped || mouse_warpx || mouse_warpy) {
                /* ignore this event, its the result of us having moved the pointer */
                /* DBG(("warped!:%d/%d/%d ptrx:%d ptry:%d lastx:%d lasty:%d", mouse_warped, mouse_warpx, mouse_warpy, ptrx, ptry, mouse_lasteventx, mouse_lasteventy)); */
                if (mouse_warped) {
                    --mouse_warped;
                }
            } else {

                if (ptrx < MOUSE_WRAP_MARGIN) {
                    /* from left to right */
                    mouse_lasteventx = ptrx;
                    ptrx = w - (MOUSE_WRAP_MARGIN + 10);
                    mouse_warpx = 1;
                    warp = 1;
                }
                else if (ptrx > (w - MOUSE_WRAP_MARGIN)) {
                    /* from right to left */
                    mouse_lasteventx = ptrx;
                    ptrx = (MOUSE_WRAP_MARGIN + 10);
                    mouse_warpx = 2;
                    warp = 1;
                }

                if (ptry < (MOUSE_WRAP_MARGIN)) {
                    /* from top to bottom */
                    mouse_lasteventy = ptry;
                    ptry = (h - (MOUSE_WRAP_MARGIN + 10));
                    mouse_warpy = 1;
                    warp = 1;
                } else if (ptry > (h - MOUSE_WRAP_MARGIN)) {
                    /* from bottom to top */
                    mouse_lasteventy = ptry;
                    ptry = (MOUSE_WRAP_MARGIN + 10);
                    mouse_warpy = 2;
                    warp = 1;
                }
                /* DBG(("warp:%d ptrx:%d ptry:%d x:%d y:%d w:%d h:%d", warp, ptrx, ptry, x, y, w, h)); */

                if (warp) {
                    /* set new cusor position */
                    ++mouse_warped;
                    /* DBG(("warp to: x:%d y:%d", ptrx, ptry)); */
                    gdk_display_warp_pointer (display, screen, ptrx + xoff, ptry + yoff);
                } else {
                    mouse_dx = (ptrx - mouse_lasteventx) * 2 / (canvas->videoconfig->doublesizex + 1);
                    mouse_dy = (ptry - mouse_lasteventy) * 2 / (canvas->videoconfig->doublesizey + 1);
                    DBG(("mouse move dx:%8d dy:%8d", mouse_dx, mouse_dy));
                    mouse_move((float)mouse_dx, (float)mouse_dy);
                    mouse_lasteventx = ptrx;
                    mouse_lasteventy = ptry;
                }
            }
        }
#ifdef HAVE_FULLSCREEN
        fullscreen_mouse_moved(canvas, (int)mevent->x, (int)mevent->y, 0);
#endif
   }
}

/*
    connected to the "leave-notify-event" of the pane, generates an extra mouse
    event to make sure the cursor can not escape the wraparound area. needed
    because the area handled by mouse grab includes the menu and statusbar.
*/
static gboolean mouse_handler_wrap(GtkWidget *w, GdkEventCrossing *e, gpointer p)
{
    if (_mouse_enabled)
    {
        GdkEventMotion mevent;

        DBG(("mouse_handler_wrap"));

        mevent.x = e->x;
        mevent.y = e->y;
        mevent.type = GDK_MOTION_NOTIFY;

        mouse_warped = 0;
        mouse_warpx = 0;
        mouse_warpy = 0;

        mouse_handler(w, (GdkEvent*)&mevent, p);
    }
    return 0;
}

/* Event handler for popup menus */
static gboolean update_menu_cb(GtkWidget *w, GdkEvent *event,gpointer data)
{
    ui_menu_update_all_GTK();
    return 0;
}

/* Continue GUI initialization after resources are set. */
int ui_init_finish(void)
{
    ui_log = log_open("X11");

    have_cbm_font = TRUE;
    fixed_font_desc = pango_font_description_from_string(fixedfontname);
    if (!fixed_font_desc) {
        log_warning(ui_log, "Cannot load CBM font %s.", fixedfontname);
        have_cbm_font = FALSE;
    }

#ifdef HAVE_FULLSCREEN
    if (fullscreen_init() != 0) {
        log_warning(ui_log, "Some fullscreen devices aren't initialized properly.");
    }
#endif
    return ui_menu_init();
}

int ui_init_finalize(void)
{
    ui_check_mouse_cursor();
    /* keyboard_grab(1); */
    return 0;
}

#ifdef USE_XF86_EXTENSIONS
int x11ui_get_display_depth(void)
{
    return depth;
}

Display *x11ui_get_display_ptr(void)
{
    return display;
}

Window x11ui_get_X11_window(void)
{
    GdkWindow *window = get_active_toplevel()->window;
    if (window) {
        return GDK_WINDOW_XID(window);
    } else {
        return 0;
    }
}

int x11ui_get_screen(void)
{
    return screen;
}
#endif

/*******************************************************************************
 * Drag and Drop support
 *******************************************************************************/

/* Define a list of data types called "targets" that a destination widget will
 * accept. The string type is arbitrary, and negotiated between DnD widgets by
 * the developer. An enum or GQuark can serve as the integer target id. */
enum {
    TARGET_STRING,
};

#if 0
/* datatype (string), restrictions on DnD (GtkTargetFlags), datatype (int) */
static GtkTargetEntry target_list[] = {
        { "STRING",     0, TARGET_STRING },
        { "text/plain", 0, TARGET_STRING },
};
#endif

static int dropdata = 0;
static int (*drop_cb)(char*) = NULL; 

/* Emitted when the user releases (drops) the selection. It should check that
 * the drop is over a valid part of the widget (if its a complex widget), and
 * itself to return true if the operation should continue. Next choose the
 * target type it wishes to ask the source for. Finally call gtk_drag_get_data
 * which will emit "drag-data-get" on the source. */
static gboolean drag_drop_handler(GtkWidget *widget, GdkDragContext *context, 
    gint x, gint y, guint time, gpointer user_data)
{
    GdkAtom target_type;

    DBG(("drag_drop_handler"));

    /* If the source offers a target */
    if (context-> targets) {
        /* Choose the best target type */
        target_type = GDK_POINTER_TO_ATOM(g_list_nth_data (context->targets, TARGET_STRING));

        dropdata = 1;
        /* Request the data from the source. */
        gtk_drag_get_data(
            widget,         /* will receive 'drag-data-received' signal */
            context,        /* represents the current state of the DnD */
            target_type,    /* the target type we want */
            time            /* time stamp */
        );
        return TRUE;
    }
    /* No target offered by source => error */
    return FALSE;
}

/* Emitted when the data has been received from the source. It should check
 * the GtkSelectionData sent by the source, and do something with it. Finally
 * it needs to finish the operation by calling gtk_drag_finish, which will emit
 * the "data-delete" signal if told to. */
static void drag_data_received_handler(GtkWidget *widget, GdkDragContext *context, 
    gint x, gint y, GtkSelectionData *selection_data, guint target_type, guint time,
    gpointer data)
{
    char *filename, *p;
    gboolean dnd_success = FALSE;
    gboolean delete_selection_data = FALSE;

    DBG(("drag_data_received_handler"));

    /* Deal with what we are given from source */
    if(dropdata && (selection_data != NULL) && (selection_data->length >= 0))
    {
        dropdata = 0;
        if (context->action == GDK_ACTION_MOVE) {
            delete_selection_data = TRUE;
        }

        /* FIXME; Check that we got a format we can use */
        filename = (char*)selection_data->data;
        DBG(("DnD got string: %s", filename));
        dnd_success = TRUE;

        /* the filename may be an URI starting with file: and/or a varying
           number of forward slashes (skip them) */
        if (strncasecmp("file:", filename, 5) == 0) {
            filename += 5;
        }
        while ((filename[0] == '/') && (filename[1] == '/')) {
            filename++;
        }
        /* incase we got a list of files, terminate the list after the first
           file */
        p = filename;
        while (p) {
            if ((*p == '\n') || (*p == '\r')) {
                *p = 0;
                break;
            }
            p++;
        }
        DBG(("DnD using filename: '%s'", filename));
        /* finally call the drop callback set by the individual ui */
        if (drop_cb) {
            drop_cb(filename);
        }
    }

    if (dnd_success == FALSE) {
        DBG(("DnD data transfer failed!"));
    }

    gtk_drag_finish (context, dnd_success, delete_selection_data, time);
}

static void set_drop_target_widget(GtkWidget *w)
{
    gtk_drag_dest_set(w, 
        GTK_DEST_DEFAULT_ALL, 
        NULL, /* set targets to NULL */
        0, 
        GDK_ACTION_COPY | GDK_ACTION_MOVE /* must be copy AND move or it won't 
                                             work with all WMs / Filemanagers */
    );
    gtk_drag_dest_add_text_targets(w); /* add text targets */
    gtk_drag_dest_add_uri_targets(w); /* add uri targets, to eg include nautilus list view drops */

    g_signal_connect (G_OBJECT(w), "drag-data-received", G_CALLBACK(drag_data_received_handler), NULL);
    g_signal_connect (G_OBJECT(w), "drag-drop", G_CALLBACK(drag_drop_handler), NULL);
}

void ui_set_drop_callback(void *cb)
{
    drop_cb = cb;
}

/******************************************************************************/

static void build_screen_canvas_widget(video_canvas_t *c)
{
    GtkWidget *new_canvas = gtk_drawing_area_new();

    /* if the eventbox already has a child, get rid of it, we are resizing */
    GtkWidget *kid = gtk_bin_get_child(GTK_BIN(c->pane));
    if (kid != NULL) {
        gtk_container_remove(GTK_CONTAINER(c->pane), kid);
    }

#ifdef HAVE_HWSCALE
    if (c->videoconfig->hwscale) {
        GdkGLConfig *gl_config = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGBA | GDK_GL_MODE_DOUBLE);

        if (gl_config == NULL) {
            log_warning(ui_log, "HW scaling will not be available");
            c->videoconfig->hwscale = 0;
            resources_set_int("HwScalePossible", 0);
        } else {
            if (!gtk_widget_set_gl_capability(GTK_WIDGET(new_canvas), gl_config, NULL, TRUE, GDK_GL_RGBA_TYPE)) {
                g_critical("Failed to add gl capability");
            }
        }
    }
#endif
    /* supress events, add mask later */
    gtk_widget_set_events(new_canvas, 0);
    /* XVideo must be refreshed when the application window is moved. */
    g_signal_connect(G_OBJECT(new_canvas), "configure-event", G_CALLBACK(configure_callback_canvas), (void*)c);
    g_signal_connect(G_OBJECT(new_canvas), "expose-event", G_CALLBACK(exposure_callback_canvas), (void*)c);
    g_signal_connect(G_OBJECT(new_canvas), "enter-notify-event", G_CALLBACK(enter_window_callback), (void *)c);
    g_signal_connect(G_OBJECT(new_canvas), "leave-notify-event", G_CALLBACK(leave_window_callback), (void *)c);
    g_signal_connect(G_OBJECT(new_canvas), "focus-out-event", G_CALLBACK(leave_window_callback), (void *)c);
    g_signal_connect(G_OBJECT(new_canvas), "map-event", G_CALLBACK(map_callback), NULL);
    g_signal_connect(G_OBJECT(new_canvas), "button-press-event", G_CALLBACK(mouse_handler), (void *) c);
    g_signal_connect(G_OBJECT(new_canvas), "button-release-event", G_CALLBACK(mouse_handler), (void *) c);
    g_signal_connect(G_OBJECT(new_canvas), "motion-notify-event", G_CALLBACK(mouse_handler), (void *) c);
    g_signal_connect(G_OBJECT(new_canvas), "key-press-event", G_CALLBACK(kbd_event_handler), (void *) c);
    g_signal_connect(G_OBJECT(new_canvas), "key-release-event", G_CALLBACK(kbd_event_handler), (void *) c);
    g_signal_connect(G_OBJECT(new_canvas), "focus-in-event", G_CALLBACK(enter_window_callback), (void *) c);
    g_signal_connect(G_OBJECT(new_canvas), "visibility-notify-event", G_CALLBACK(enter_window_callback), (void *) c);

    if (c->videoconfig->hwscale) {
        /* For hwscale, it's a feature that new_canvas must bloat to 100% size
         * of the containing GtkEventWindow. Unfortunately, for the other
         * path, it's a PITA. */
        gtk_container_add(GTK_CONTAINER(c->pane), new_canvas);
    } else {
        /* Believe it or not, but to get a gtkdrawingarea of fixed dimensions
         * with a black background within our layout vbox requires this:
         *
         * toplvl < ui   < black bg < centering < SCREEN HERE
         * window < vbox < eventbox < hbox<vbox < drawingarea.
         *
         * We do this to make fullscreen work. More gory details in the
         * x11ui_fullscreen about how "nice" that is to get to work. */
        GtkWidget *canvascontainer1 = gtk_hbox_new(FALSE, 0);
        gtk_widget_show(canvascontainer1);
        gtk_container_add(GTK_CONTAINER(c->pane), canvascontainer1);
        GtkWidget *canvascontainer2 = gtk_vbox_new(FALSE, 0);
        gtk_widget_show(canvascontainer2);
        gtk_box_pack_start(GTK_BOX(canvascontainer1), canvascontainer2, TRUE, FALSE, 0);
        gtk_box_pack_start(GTK_BOX(canvascontainer2), new_canvas, TRUE, FALSE, 0);
    }

    gtk_widget_show(new_canvas);
    GTK_WIDGET_SET_FLAGS(new_canvas, GTK_CAN_FOCUS);
    gtk_widget_grab_focus(new_canvas);
    c->emuwindow = new_canvas;
/*
explicitly setup the events we want to handle. the following are the remaining
events that are NOT handled:

GDK_BUTTON_MOTION_MASK         GDK_MOTION_NOTIFY (while a button is pressed) 
GDK_BUTTON1_MOTION_MASK        GDK_MOTION_NOTIFY (while button 1 is pressed) 
GDK_BUTTON2_MOTION_MASK        GDK_MOTION_NOTIFY (while button 2 is pressed) 
GDK_BUTTON3_MOTION_MASK        GDK_MOTION_NOTIFY (while button 3 is pressed) 
GDK_PROPERTY_CHANGE_MASK       GDK_PROPERTY_NOTIFY 
GDK_PROXIMITY_IN_MASK          GDK_PROXIMITY_IN 
GDK_PROXIMITY_OUT_MASK         GDK_PROXIMITY_OUT 
GDK_SUBSTRUCTURE_MASK          Receive  GDK_STRUCTURE_MASK events for child windows 
*/
    gtk_widget_add_events(new_canvas,
                          GDK_LEAVE_NOTIFY_MASK |
                          GDK_ENTER_NOTIFY_MASK |
                          GDK_BUTTON_PRESS_MASK |
                          GDK_BUTTON_RELEASE_MASK |
                          GDK_KEY_PRESS_MASK |
                          GDK_KEY_RELEASE_MASK |
                          GDK_FOCUS_CHANGE_MASK |
                          GDK_POINTER_MOTION_MASK |
                          GDK_STRUCTURE_MASK |
                          GDK_VISIBILITY_NOTIFY_MASK |
                          GDK_EXPOSURE_MASK);
}

/* Create a shell with a canvas widget in it.  
   called from arch/unix/gui/vsidui.c:vsid_ui_init (vsid) or
               arch/unix/x11/gnome/gnomevideo.c:video_canvas_create (other)
 */
int ui_open_canvas_window(video_canvas_t *c, const char *title, int w, int h, int no_autorepeat)
{
    GtkWidget *new_window, *topmenu, *panelcontainer, *sb, *pal_ctrl_widget = NULL;
    GtkAccelGroup* accel;
    GdkColor black = { 0, 0, 0, 255 };
    int i;
    gint window_width, window_height, window_xpos, window_ypos;

    DBG(("ui_open_canvas_window %p (w: %d h: %d)", c, w, h));

    if (++num_app_shells > MAX_APP_SHELLS) {
        log_error(ui_log, "Maximum number of toplevel windows reached.");
        return -1;
    }
    memset(&app_shells[num_app_shells - 1], 0, sizeof(app_shell_type));

    new_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    /* supress events, add mask later */
    gtk_widget_set_events(new_window, 0);

    set_active_shell(num_app_shells - 1);

    app_shells[num_app_shells - 1].shell = new_window;
    app_shells[num_app_shells - 1].canvas = c;
    c->app_shell = num_app_shells - 1;

    panelcontainer = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_window), panelcontainer);
    gtk_widget_show(panelcontainer);

    topmenu = gtk_menu_bar_new();
    gtk_widget_show(topmenu);
    g_signal_connect(G_OBJECT(topmenu), "button-press-event", G_CALLBACK(update_menu_cb), NULL);
    gtk_box_pack_start(GTK_BOX(panelcontainer), topmenu, FALSE, TRUE, 0);

    c->pane = gtk_event_box_new();
    gtk_widget_modify_bg(c->pane, GTK_STATE_NORMAL, &black);
    gtk_box_pack_start(GTK_BOX(panelcontainer), c->pane, TRUE, TRUE, 0);
    gtk_widget_show(c->pane);
    g_signal_connect(G_OBJECT(c->pane), "leave-notify-event", G_CALLBACK(mouse_handler_wrap), (void*)c);

    gtk_widget_show(new_window);
    if (machine_class == VICE_MACHINE_VSID) {
        GtkWidget *new_canvas = build_vsid_ctrl_widget();
        gtk_container_add(GTK_CONTAINER(c->pane), new_canvas);
        gtk_widget_show(new_canvas);
        c->emuwindow = NULL;
    } else {
        build_screen_canvas_widget(c);
    }

    sb = ui_create_status_bar(panelcontainer);
    if (machine_class != VICE_MACHINE_VSID) {
        pal_ctrl_widget = build_pal_ctrl_widget(c, &app_shells[num_app_shells - 1].pal_ctrl_data);
        gtk_box_pack_end(GTK_BOX(panelcontainer), pal_ctrl_widget, FALSE, FALSE, 0);
        gtk_widget_hide(pal_ctrl_widget);
    }
#if 0
    if (no_autorepeat) {
        g_signal_connect(G_OBJECT(new_window), "enter-notify-event", G_CALLBACK(ui_autorepeat_off), NULL);
        g_signal_connect(G_OBJECT(new_window), "leave-notify-event", G_CALLBACK(ui_autorepeat_on), NULL);
    }
#endif
    g_signal_connect(G_OBJECT(new_window), "configure-event", G_CALLBACK(configure_callback_app), (void*)c);
    g_signal_connect(G_OBJECT(new_window), "enter-notify-event", G_CALLBACK(kbd_event_handler), NULL);
    g_signal_connect(G_OBJECT(new_window), "leave-notify-event", G_CALLBACK(kbd_event_handler), NULL);
    g_signal_connect(G_OBJECT(new_window), "delete_event", G_CALLBACK(delete_event), NULL);
    g_signal_connect(G_OBJECT(new_window), "destroy_event", G_CALLBACK(delete_event), NULL);

    set_drop_target_widget(new_window);

    accel = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(new_window), accel);

    app_shells[num_app_shells - 1].title = lib_stralloc(title);
    app_shells[num_app_shells - 1].topmenu = topmenu;
    app_shells[num_app_shells - 1].accel = accel;
    app_shells[num_app_shells - 1].status_bar = sb;
    app_shells[num_app_shells - 1].pal_ctrl = pal_ctrl_widget;

    gtk_window_set_title(GTK_WINDOW(new_window), title);

    get_window_resources(c, &window_xpos, &window_ypos, &window_width, &window_height);
    DBG(("ui_open_canvas_window (winx: %d winy: %d winw: %d winh: %d)", 
            window_xpos, window_ypos, window_width, window_height));
    if (machine_class == VICE_MACHINE_VSID) {
        gtk_window_resize(GTK_WINDOW(new_window), VSID_WINDOW_MINW, VSID_WINDOW_MINH);
        if (!((window_xpos < 0) || (window_ypos < 0))) {
            gtk_window_move(GTK_WINDOW(new_window), window_xpos, window_ypos);
        }
    } else {
        if (!((window_width < WINDOW_MINW) || (window_height < WINDOW_MINH))) {
            gtk_window_resize(GTK_WINDOW(new_window), window_width, window_height);
        }
        if (!((window_xpos < 0) || (window_ypos < 0))) {
            gtk_window_move(GTK_WINDOW(new_window), window_xpos, window_ypos);
        }

        if (!app_gc) {
            app_gc = gdk_gc_new(new_window->window);
        }

        if (uicolor_alloc_colors(c) < 0) {
            return -1;
        }

        /* This is necessary because the status might have been set before we
        actually open the canvas window. e.g. by commandline */
        ui_enable_drive_status(enabled_drives, drive_active_led);

        /* make sure that all drive status widgets are initialized.
        This is needed for proper dual disk/dual led drives (8050, 8250). */
        for (i = 0; i < NUM_DRIVES; i++) {
            ui_display_drive_led(i, 1000, 1000);
        }

        ui_style_red = gtk_style_new();
        ui_style_red->fg[GTK_STATE_NORMAL] = drive_led_on_red_pixel;
        ui_style_red->fg[GTK_STATE_ACTIVE] = drive_led_on_red_pixel;
        ui_style_red->fg[GTK_STATE_SELECTED] = drive_led_on_red_pixel;
        ui_style_red->fg[GTK_STATE_PRELIGHT] = drive_led_on_red_pixel;
        gtk_widget_set_style(video_ctrl_checkbox_label, ui_style_red);
        gtk_widget_set_style(event_rec_checkbox_label, ui_style_red);

        ui_style_green = gtk_style_new();
        ui_style_green->fg[GTK_STATE_NORMAL] = drive_led_on_green_pixel;
        ui_style_green->fg[GTK_STATE_ACTIVE] = drive_led_on_green_pixel;
        ui_style_green->fg[GTK_STATE_SELECTED] = drive_led_on_green_pixel;
        ui_style_green->fg[GTK_STATE_PRELIGHT] = drive_led_on_green_pixel;
        gtk_widget_set_style(event_playback_checkbox_label, ui_style_green);

        initBlankCursor();
        gtk_init_lightpen();

        c->offx = c->geometry->screen_size.width - w;
    }
/*
explicitly setup the events we want to handle. the following are the remaining
events that are NOT handled:

GDK_BUTTON_MOTION_MASK         GDK_MOTION_NOTIFY (while a button is pressed) 
GDK_BUTTON1_MOTION_MASK        GDK_MOTION_NOTIFY (while button 1 is pressed) 
GDK_BUTTON2_MOTION_MASK        GDK_MOTION_NOTIFY (while button 2 is pressed) 
GDK_BUTTON3_MOTION_MASK        GDK_MOTION_NOTIFY (while button 3 is pressed) 
GDK_PROPERTY_CHANGE_MASK       GDK_PROPERTY_NOTIFY 
GDK_PROXIMITY_IN_MASK          GDK_PROXIMITY_IN 
GDK_PROXIMITY_OUT_MASK         GDK_PROXIMITY_OUT 
GDK_SUBSTRUCTURE_MASK          Receive  GDK_STRUCTURE_MASK events for child windows 
*/
    gtk_widget_add_events(new_window,
                            GDK_LEAVE_NOTIFY_MASK |
                            GDK_ENTER_NOTIFY_MASK |
                            GDK_BUTTON_PRESS_MASK |
                            GDK_BUTTON_RELEASE_MASK |
                            GDK_KEY_PRESS_MASK |
                            GDK_KEY_RELEASE_MASK |
                            GDK_FOCUS_CHANGE_MASK |
                            GDK_POINTER_MOTION_MASK |
                            GDK_VISIBILITY_NOTIFY_MASK |
                            GDK_STRUCTURE_MASK |
                            GDK_EXPOSURE_MASK);

    return 0;
}

/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(ui_menu_entry_t *menu)
{
    int i;
    static GtkAccelGroup *accel;

    DBG(("ui_set_left_menu"));

    ui_block_shells();

    if (accel) {
        g_object_unref(accel);
    }

    accel = gtk_accel_group_new();
    for (i = 0; i < num_app_shells; i++) {
        gtk_window_add_accel_group (GTK_WINDOW (app_shells[i].shell), accel);
    }

    if (left_menu != NULL) {
        gtk_widget_destroy(left_menu);
    }
    left_menu = gtk_menu_new();
    ui_menu_create(left_menu, accel, "LeftMenu", menu);

    ui_unblock_shells();
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(ui_menu_entry_t *menu)
{
    int i;
    static GtkAccelGroup *accel;

    DBG(("ui_set_right_menu"));

    ui_block_shells();

    if (accel) {
        g_object_unref(accel);
    }

    accel = gtk_accel_group_new();
    for (i = 0; i < num_app_shells; i++) {
        gtk_window_add_accel_group (GTK_WINDOW (app_shells[i].shell), accel);
    }

    if (right_menu != NULL) {
        gtk_widget_destroy(right_menu);
    }
    right_menu = gtk_menu_new();
    ui_menu_create(right_menu, accel, "RightMenu", menu);

    ui_unblock_shells();
}

void ui_set_topmenu(ui_menu_entry_t *menu)
{
    int i;

    DBG(("ui_set_topmenu (%d)", num_app_shells));

    ui_block_shells();

    for (i = 0; i < num_app_shells; i++) {
        DBG(("ui_set_topmenu %d: %p", i, app_shells[i].topmenu));
        if (app_shells[i].topmenu) {
            gtk_container_foreach(GTK_CONTAINER(app_shells[i].topmenu), (GtkCallback)gtk_widget_destroy, NULL);
            ui_menu_create(app_shells[i].topmenu, app_shells[i].accel, "TopLevelMenu", menu);
        }
    }

    ui_unblock_shells();
}

void ui_set_application_icon(const char *icon_data[])
{
    int i;
    GdkPixmap *icon;
    GdkWindow *window = get_active_toplevel()->window;

    icon = gdk_pixmap_create_from_xpm_d(window, NULL, NULL, (char **)icon_data);

    for (i = 0; i < num_app_shells; i++) {
        gdk_window_set_icon(app_shells[i].shell->window, NULL, icon, NULL);
    }
}

/* ------------------------------------------------------------------------- */

void ui_exit(void)
{
    ui_button_t b;
    int value;
    char *s = util_concat("Exit ", machine_name, _(" emulator"), NULL);

#ifdef HAVE_FULLSCREEN
    fullscreen_suspend(1);
#endif
    resources_get_int("ConfirmOnExit", &value);
    if (value) {
        b = ui_ask_confirmation(s, _("Do you really want to exit?"));
    } else {
        b = UI_BUTTON_YES;
    }

    if (b == UI_BUTTON_YES) {
        resources_get_int("SaveResourcesOnExit", &value);
        if (value) {
            b = ui_ask_confirmation(s, _("Save the current settings?"));
            if (b == UI_BUTTON_YES) {
                if (resources_save(NULL) < 0) {
                    ui_error(_("Cannot save settings."));
                }
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
        ui_dispatch_events();

        lib_free(s);
        exit(0);
    }
    lib_free(s);
    vsync_suspend_speed_eval();
}


/* Dispatch the next Xt event.  If not pending, wait for it. */
void ui_dispatch_next_event(void)
{
    gtk_main_iteration();
}

/* Dispatch all the pending UI events. */
void ui_dispatch_events(void)
{
    while (gtk_events_pending()) {
        ui_dispatch_next_event();
    }
}

/*
    enable / disable fullscreen mode

    NOTE: we must make sure that no events resulting from operations done in
          this function fire (asynchronous) with a wrong value in
          ->fullscreenconfig->enable for the active canvas.

    FIXME: this is still buggy when changing mode rapidly, eg by holding ALT-D
 */
static volatile int fslock = 0;
static int fsoldx = 0, fsoldy = 0, fsoldw = WINDOW_MINW, fsoldh = WINDOW_MINH;

int x11ui_fullscreen(int enable)
{
#ifdef HAVE_FULLSCREEN
    video_canvas_t *canvas;
    GtkWidget *s;

    ui_dispatch_events();
    gdk_flush();

    if (fslock) {
        log_debug("x11ui_fullscreen (%d) ignored (locked).", enable);
        return -1;
    }
    fslock = 1;

    if ((canvas = get_active_canvas()) == NULL) {
        log_debug("x11ui_fullscreen (%d) ignored (canvas == NULL).", enable);
        fslock = 0;
        return -1;
    }

    if (enable == canvas->fullscreenconfig->enable) {
        log_debug("x11ui_fullscreen (%d) ignored (was %d).", enable, enable);
        fslock = 0;
        return 0;
    }

    set_active_shell(canvas->app_shell);
    s = get_active_toplevel();

    DBG(("x11ui_fullscreen (shell: %d fullscreen: %d->%d)", canvas->app_shell, canvas->fullscreenconfig->enable, enable));

    if (enable) {
        /* save window dimensions before going to fullscreen */
        get_window_resources(canvas, &fsoldx, &fsoldy, &fsoldw, &fsoldh);
        DBG(("x11ui_fullscreen (fs:%d saved winx: %d winy: %d winw: %d winh: %d)", enable, fsoldx, fsoldy, fsoldw, fsoldh));
        /* when switching to fullscreen, set the flag first */
        canvas->fullscreenconfig->enable = 1;
        /* window managers (bug detected on compiz 0.7.4) may ignore
         * fullscreen requests for windows not visible inside the screen.
         * This can happen especially when using XRandR to resize the desktop.
         * This tries to workaround that problem by ensuring^Whinting that the
         * window should be placed to the top-left corner. GTK/X sucks. */
        gtk_window_move(GTK_WINDOW(s), 0, 0);
        gtk_window_fullscreen(GTK_WINDOW(s));
        gtk_window_present(GTK_WINDOW(s));
        ui_dispatch_events();
        gdk_flush();
    } else {
        canvas->fullscreenconfig->enable = 0;

        gtk_window_unfullscreen(GTK_WINDOW(s));
        gtk_window_present(GTK_WINDOW(s));
        ui_dispatch_events();
        gdk_flush();

        /* restore previously saved window dimensions */
        DBG(("x11ui_fullscreen (fs:%d restore winx: %d winy: %d winw: %d winh: %d)", enable, fsoldx, fsoldy, fsoldw, fsoldh));
        gtk_window_resize(GTK_WINDOW(s), fsoldw, fsoldh);
        gtk_window_move(GTK_WINDOW(s), fsoldx, fsoldy);
    }
    ui_check_mouse_cursor();
    ui_dispatch_events();
    gdk_flush();

    fslock = 0;
    DBG(("x11ui_fullscreen done"));
#endif
    return 0;
}

int ui_fullscreen_statusbar(struct video_canvas_s *canvas, int enable)
{
#ifdef HAVE_FULLSCREEN
    app_shell_type *appshell = &app_shells[canvas->app_shell];
#endif
    int j;

    DBG(("ui_fullscreen_statusbar (enable:%d)", enable));

    if (!enable
#ifdef HAVE_FULLSCREEN
        && canvas->fullscreenconfig->enable
#endif
       ) {
        for (j = 0; j < num_app_shells; j++) {
            gtk_widget_hide(app_shells[j].status_bar);
            gtk_widget_hide(app_shells[j].topmenu);
        }
    } else {
        for (j = 0; j < num_app_shells; j++) {
            gtk_widget_show(app_shells[j].status_bar);
            gtk_widget_show(app_shells[j].topmenu);
        }
    }

#ifdef HAVE_FULLSCREEN
    if (enable) {
        canvas->fullscreenconfig->ui_border_top = appshell->topmenu->allocation.height;
        canvas->fullscreenconfig->ui_border_bottom = appshell->status_bar->allocation.height;
    } else {
        canvas->fullscreenconfig->ui_border_top = 0;
        canvas->fullscreenconfig->ui_border_bottom = 0;
    }
#endif

    ui_trigger_resize();
    return 0;
}

static void toggle_aspect(video_canvas_t *canvas)
{
    int keep_aspect_ratio, flags = 0;
    app_shell_type *appshell = &app_shells[canvas->app_shell];

    DBG(("toggle_aspect"));
    if ((appshell != NULL) && (appshell->shell != NULL)) {
#ifdef HAVE_FULLSCREEN
        DBG(("toggle_aspect fs:%d", canvas->fullscreenconfig->enable));
        if (!canvas->fullscreenconfig->enable) {
#endif
            resources_get_int("KeepAspectRatio", &keep_aspect_ratio);
            if (keep_aspect_ratio) {
                flags |= GDK_HINT_ASPECT;
                if (appshell->geo.max_width) {
                    flags |= GDK_HINT_MAX_SIZE;
                }
            }
            gtk_window_set_geometry_hints (GTK_WINDOW(appshell->shell), NULL, &appshell->geo, GDK_HINT_MIN_SIZE | flags);
#ifdef HAVE_FULLSCREEN
        }
#endif
    }
}

static gfloat get_aspect(video_canvas_t *canvas)
{
    int keep_aspect_ratio, true_aspect_ratio;
    resources_get_int("KeepAspectRatio", &keep_aspect_ratio);
    if (keep_aspect_ratio) {
        resources_get_int("TrueAspectRatio", &true_aspect_ratio);
        if (true_aspect_ratio) {
#ifdef HAVE_HWSCALE
            if (canvas->videoconfig->hwscale) {
                return canvas->geometry->pixel_aspect_ratio;
            }
#endif
        }
        return 1.0f;
    }
    return 0.0f;
}

static void setup_aspect(video_canvas_t *canvas)
{
    gfloat aspect, taspect;
    int w, h, winw, winh;
    app_shell_type *appshell = &app_shells[canvas->app_shell];
    GtkWidget *win;
    GtkWidget *topmenu;
    GtkWidget *sb;
    GtkWidget *palctrl;

#ifdef HAVE_FULLSCREEN
    DBG(("setup_aspect fullscreen:%d", canvas->fullscreenconfig->enable));
#else
    DBG(("setup_aspect"));
#endif

    if (appshell == NULL) {
        return;
    }

    win = appshell->shell;
    topmenu = appshell->topmenu;
    sb = appshell->status_bar;
    palctrl = appshell->pal_ctrl;

    if ((win == NULL) || (topmenu == NULL) || (sb == NULL) || (palctrl == NULL)) {
        return;
    }

    /* get size of drawing buffer */
    w = canvas->draw_buffer->canvas_width;
    h = canvas->draw_buffer->canvas_height;
    if (canvas->videoconfig->doublesizex) {
        w *= (canvas->videoconfig->doublesizex + 1);
    }
    if (canvas->videoconfig->doublesizey) {
        h *= (canvas->videoconfig->doublesizey + 1);
    }
    /* calculate unscaled size of window */
    winw = w;
    winh = h + topmenu->allocation.height + sb->allocation.height;

#if GTK_CHECK_VERSION(2,18,0)
    if (gtk_widget_get_visible(palctrl)) {
        winh += palctrl->allocation.height;
    }
#else
    if (GTK_WIDGET_VISIBLE(palctrl)) {
        winh += palctrl->allocation.height;
    }
#endif

    /* default geometry hints, no scaling, 1:1 aspect */
    appshell->geo.min_width = winw;
    appshell->geo.min_height = winh;
    appshell->geo.max_width = winw;
    appshell->geo.max_height = winh;
    appshell->geo.min_aspect = 1.0f;
    appshell->geo.max_aspect = 1.0f;

#ifdef HAVE_FULLSCREEN
    if (canvas->fullscreenconfig->enable) {
        /* TODO */
    } else {
#endif
        taspect = get_aspect(canvas);
        if (taspect > 0.0f) {
            aspect = ((float)winw * taspect) / ((float)winh);
            appshell->geo.min_aspect = aspect;
            appshell->geo.max_aspect = aspect;
            appshell->geo.min_width = (int)((float)winw * taspect);
            if (canvas->videoconfig->hwscale) {
                appshell->geo.max_width = 0;
                appshell->geo.max_height = 0;
            }
        }
#ifdef HAVE_FULLSCREEN
    }
#endif
}

/* Resize one window. */
void ui_resize_canvas_window(video_canvas_t *canvas)
{
    int window_xpos, window_ypos, window_width, window_height;
    app_shell_type *appshell;
    int def;

    get_window_resources(canvas, &window_xpos, &window_ypos, &window_width, &window_height);

    DBG(("ui_resize_canvas_window (winw: %d winh: %d hwscale:%d)", window_width, window_height,canvas->videoconfig->hwscale));

    def = 0;
    if (!canvas->videoconfig->hwscale || (window_width < WINDOW_MINW) || (window_height < WINDOW_MINH)) {
        def = 1;
        window_width = canvas->draw_buffer->canvas_physical_width;
        window_height = canvas->draw_buffer->canvas_physical_height;
    }

    appshell = &app_shells[canvas->app_shell];

    build_screen_canvas_widget(canvas);
    if (! canvas->videoconfig->hwscale) {
        gtk_widget_set_size_request(canvas->emuwindow, window_width, window_height);
    }

    if (def) {
        /* maintain aspect ratio */
        setup_aspect(canvas);
        toggle_aspect(canvas);
        /* set initial (properly scaled) window size */
        window_width = appshell->geo.min_width;
        window_height = appshell->geo.min_height;
    }
    gtk_window_resize(GTK_WINDOW(appshell->shell), window_width, window_height);

#ifdef HAVE_FULLSCREEN
    if (!canvas->fullscreenconfig->enable)
#endif
    {
        set_window_resources(canvas, window_xpos, window_ypos, window_width, window_height);
    }

    DBG(("ui_resize_canvas_window exit (w:%d h:%d)", window_width, window_height));
}

void x11ui_move_canvas_window(ui_window_t w, int x, int y)
{
    DBG(("x11ui_move_canvas_window x:%d y:%d", x, y));
    gdk_window_move(gdk_window_get_toplevel(w->window), x, y);
    gdk_flush();
}

void x11ui_canvas_position(ui_window_t w, int *x, int *y)
{
    gint tl_x, tl_y, pos_x, pos_y;

    gdk_flush();
    gdk_window_get_position(gdk_window_get_toplevel(w->window), &tl_x, &tl_y);
    gdk_window_get_position(w->window, &pos_x, &pos_y);
    *x = (pos_x + tl_x);
    *y = (pos_y + tl_y);
    DBG(("x11ui_canvas_position x:%d y:%d", *x, *y));
    gdk_window_raise(gdk_window_get_toplevel(w->window));
}

void x11ui_get_widget_size(ui_window_t win, int *w, int *h)
{
    GtkRequisition req;

    gtk_widget_size_request(win, &req);
    *w = (int)req.width;
    *h = (int)req.height;
}

void x11ui_destroy_widget(ui_window_t w)
{
    gtk_widget_destroy(w);
}


/* Enable autorepeat. */
void ui_autorepeat_on(void)
{
}
#if 0
/* Disable autorepeat. */
void ui_autorepeat_off(void)
{
}
#endif

void ui_make_window_transient(GtkWidget *parent,GtkWidget *window)
{
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(parent));
#if 0
    gdk_window_set_colormap(window->window, colormap);
#endif
}

unsigned char *convert_utf8(unsigned char *s)
{
    unsigned char *d, *r;

    r = d = lib_malloc((size_t)(strlen((char *)s) * 2 + 1));
    while (*s) {
        if (*s < 0x80) {
            *d = *s;
        } else {
            /* special latin1 character handling */
            if (*s == 0xa0) {
                *d = 0x20;
            } else {
                if (*s == 0xad) {
                    *s = 0xed;
                }
                *d++ = 0xc0 | (*s >> 6);
                *d = (*s & ~0xc0) | 0x80;
            }
        }
        s++;
        d++;
    }
    *d = '\0';
    return r;
}

/* Update the menu items with a checkmark according to the current resource
   values.

   Note: ui_menu_update_all actually is empty (?)
*/
void ui_update_menus(void)
{
    ui_menu_update_all();
    ui_update_palctrl();
}

void ui_block_shells(void)
{
    int i;

    DBG(("ui_block_shells (%d)", num_app_shells));

    for (i = 0; i < num_app_shells; i++) {
        gtk_widget_set_sensitive(app_shells[i].shell, FALSE);
    }
}

void ui_unblock_shells(void)
{
    video_canvas_t *canvas = get_active_canvas();
    int i;

    DBG(("ui_unblock_shells (%d)", num_app_shells));

    for (i = 0; i < num_app_shells; i++) {
        gtk_widget_set_sensitive(app_shells[i].shell, TRUE);
    }
    /* this is an ugly workaround to fix the focus issue on pop-down reported by count zero 
       - dead keyboard after popdown of some dialogs 
       this is neither correct nor elegant, as it messes with the mousecursor,
       which is evil UI design, imho; unfortunately I don't know a "better" way :( - pottendo */
    keyboard_key_clear();
    if (canvas) {
        if (machine_class == VICE_MACHINE_VSID) {
            /* FIXME */
        } else {
            gdk_pointer_grab(canvas->emuwindow->window, 1, 0, 
                            canvas->emuwindow->window, 
                            blankCursor, GDK_CURRENT_TIME);
        }
        gdk_pointer_ungrab(GDK_CURRENT_TIME);
        ui_check_mouse_cursor();
    }
}

/* Pop up a popup shell and center it to the last visited AppShell */
void ui_popup(GtkWidget *w, const char *title, gboolean wait_popdown)
{
#ifdef HAVE_FULLSCREEN
    fullscreen_suspend(1);
#endif

    ui_restore_mouse();

    /* Keep sure that we really know which was the last visited shell. */
    ui_dispatch_events();
    gtk_window_set_title(GTK_WINDOW(w),title);

    gtk_window_set_transient_for(GTK_WINDOW(w),GTK_WINDOW(get_active_toplevel()));
    gtk_widget_show(w);
    gtk_window_present(GTK_WINDOW(w));

    gdk_window_set_decorations(w->window, GDK_DECOR_ALL | GDK_DECOR_MENU);
    gdk_window_set_functions(w->window, GDK_FUNC_ALL | GDK_FUNC_RESIZE);

    ui_block_shells();
    /* If requested, wait for this widget to be popped down before
       returning. */
    if (wait_popdown) {
        int oldcnt = popped_up_count++;

        while (oldcnt != popped_up_count) {
            ui_dispatch_next_event();
        }
        ui_unblock_shells();
    } else {
        popped_up_count++;
    }
}

/* Pop down a popup shell. */
void ui_popdown(GtkWidget *w)
{
    ui_check_mouse_cursor();
    if (w) {
        gtk_widget_hide(w);
    }
    if (--popped_up_count < 0) {
        popped_up_count = 0;
    }
    ui_unblock_shells();
#ifdef HAVE_FULLSCREEN
    fullscreen_resume();
#endif
}

/* ------------------------------------------------------------------------- */

/* Miscellaneous callbacks.  */

gboolean enter_window_callback(GtkWidget *w, GdkEvent *e, gpointer p)
{
    DBG(("enter_window_callback %p", p));

    set_active_shell(((video_canvas_t *)p)->app_shell);

    /* cv: ensure focus after dialogs were opened */
    gtk_widget_grab_focus(w);
    gtk_lightpen_update_canvas(p, TRUE);
    keyboard_key_clear();

#ifdef HAVE_FULLSCREEN
    fullscreen_mouse_moved((struct video_canvas_s *)p, 0, 0, 2);
#endif

    return 0;
}

gboolean leave_window_callback(GtkWidget *w, GdkEvent *e, gpointer p)
{
    DBG(("leave_window_callback %p", p));
#ifdef HAVE_FULLSCREEN
    fullscreen_mouse_moved((struct video_canvas_s *)p, 0, 0, 1);
#endif
    return 0;
}

gboolean map_callback(GtkWidget *w, GdkEvent *event, gpointer user_data)
{
#ifdef HAVE_HWSCALE
    video_canvas_t *canvas = (video_canvas_t *)user_data;

    if (canvas) {
        GdkGLContext *gl_context = gtk_widget_get_gl_context(w);
        GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(w);

        gdk_gl_drawable_gl_begin (gl_drawable, gl_context);

        glGenTextures(1, &canvas->screen_texture);

        gdk_gl_drawable_gl_end(gl_drawable);
    }
#endif
    DBG(("map_callback %p", user_data));

    return FALSE;
}

gboolean configure_callback_canvas(GtkWidget *w, GdkEventConfigure *e, gpointer client_data)
{
    video_canvas_t *canvas = (video_canvas_t *) client_data;
#ifdef HAVE_HWSCALE
    float ow, oh;
#ifdef HAVE_FULLSCREEN
    int keep_aspect_ratio;
#endif
    GdkGLContext *gl_context = gtk_widget_get_gl_context (w);
    GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable (w);
#endif

    if ((e->width < WINDOW_MINW) || (e->height < WINDOW_MINH)) {
        /* DBG(("configure_callback_canvas skipped")); */
        return 0;
    }

    if ((canvas == NULL) || (canvas->app_shell >= num_app_shells) || 
        (canvas != app_shells[canvas->app_shell].canvas)) {
        log_error(ui_log, "configure_callback_canvas: bad params");
        return 0;
    }

    DBG(("configure_callback_canvas (e->width %d e->height %d canvas_width %d canvas_height %d)",
         e->width, e->height, canvas->draw_buffer->canvas_width, canvas->draw_buffer->canvas_height));

    /* This should work, but doesn't... Sigh...
    c->draw_buffer->canvas_width = e->width;
    c->draw_buffer->canvas_height = e->height;
    if (c->videoconfig->doublesizex) {
        c->draw_buffer->canvas_width /= 2;
    }
    if (c->videoconfig->doublesizey) {
        c->draw_buffer->canvas_height /= 2;
    }
    video_viewport_resize(c);
    */

#ifdef HAVE_HWSCALE
    /* get size of drawing buffer */
    ow = canvas->draw_buffer->canvas_physical_width;
    oh = canvas->draw_buffer->canvas_physical_height;

#ifdef HAVE_FULLSCREEN
    /* in fullscreen mode, scale with aspect ratio */
    if (canvas->fullscreenconfig->enable) {
        resources_get_int("KeepAspectRatio", &keep_aspect_ratio);
        if (keep_aspect_ratio) {
            if ((float)e->height >= (oh / get_aspect(canvas)) * ((float)e->width / ow)) {
                /* full width, scale height */
                oh = (float)e->height / ((float)e->width / ow);
                oh *= get_aspect(canvas);
            } else {
                /* full height, scale width */
                ow = (float)e->width / ((float)e->height / oh);
                ow /= get_aspect(canvas);
            }
        }
    }
    DBG(("configure_callback_canvas (ow: %f oh:%f)", ow, oh));
#endif

    if (gl_context != NULL && gl_drawable != NULL) {
        gdk_gl_drawable_gl_begin(gl_drawable, gl_context);
        /* setup viewport */
        glViewport(0, 0, e->width, e->height);
        /* projection and model view matrix */
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-(ow/2),(ow/2),-(oh/2),(oh/2),-100,100);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        gdk_gl_drawable_gl_end(gl_drawable);
    }
#endif
    /* maintain aspect ratio */
    setup_aspect(canvas);
    toggle_aspect(canvas);

    return 0;
}

/*
  connected to "configure-event" of the window, which is emitted to size, 
  position and stack order events. 
*/
gboolean configure_callback_app(GtkWidget *w, GdkEventConfigure *e, gpointer client_data)
{
    video_canvas_t *canvas = (video_canvas_t *) client_data;
    app_shell_type *appshell;
    GdkEventConfigure e2;

    if ((canvas == NULL) || (e->width < WINDOW_MINW) || (e->height < WINDOW_MINH)) {
        /* DBG(("configure_callback_app skipped")); */
        return 0;
    }

    appshell = &app_shells[canvas->app_shell];

    if ((canvas->app_shell >= num_app_shells) || (canvas != appshell->canvas)) {
        log_error(ui_log, "configure_callback_app: bad params (%p) %d", client_data, canvas->app_shell);
        return 0;
    }

#ifdef DEBUG_X11UI
#ifdef HAVE_FULLSCREEN
    if (canvas->fullscreenconfig) {
        DBG(("configure_callback_app (fullscreen: %d x %d y %d w %d h %d) (%p)",canvas->fullscreenconfig->enable, e->x, e->y,e->width, e->height, canvas));
    } else {
#endif
        DBG(("configure_callback_app (fullscreen: -- x %d y %d w %d h %d) (%p)", e->x, e->y,e->width, e->height, canvas));
#ifdef HAVE_FULLSCREEN
    }
#endif
#endif

#ifdef HAVE_FULLSCREEN
    if ((machine_class == VICE_MACHINE_VSID) || (!canvas->fullscreenconfig->enable)) {
#endif
        set_window_resources(canvas, e->x, e->y, e->width, e->height);
#ifdef HAVE_FULLSCREEN
    }
#endif
    if (machine_class != VICE_MACHINE_VSID) {
        /* HACK: propagate the event to the canvas widget to make ui_trigger_resize
        *       work.
        */
        e2.x = e2.y = 0;
        e2.width = e->width;

#ifdef HAVE_FULLSCREEN
        if (canvas->fullscreenconfig->enable) {
            e2.height = e->height - (canvas->fullscreenconfig->ui_border_top + canvas->fullscreenconfig->ui_border_bottom);
        } else {
#endif
            e2.height = e->height - (appshell->topmenu->allocation.height + appshell->status_bar->allocation.height);
#ifdef HAVE_FULLSCREEN
        }
#endif
#if GTK_CHECK_VERSION(2,18,0)
        if (gtk_widget_get_visible(appshell->pal_ctrl)) {
            e2.height -= appshell->pal_ctrl->allocation.height;
        }
#else
        if (GTK_WIDGET_VISIBLE(appshell->pal_ctrl)) {
            e2.height -= appshell->pal_ctrl->allocation.height;
        }
#endif
        DBG(("configure_callback_app2 (x %d y %d w %d h %d)", e2.x, e2.y, e2.width, e2.height));
        configure_callback_canvas(canvas->emuwindow, &e2, canvas);
    }
    return 0;
}


gboolean exposure_callback_canvas(GtkWidget *w, GdkEventExpose *e, gpointer client_data)
{
    video_canvas_t *canvas = (video_canvas_t *)client_data;

    if ((canvas == NULL) || (canvas->app_shell >= num_app_shells) || 
        (canvas != app_shells[canvas->app_shell].canvas)) {
        log_error(ui_log, "exposure_callback_canvas: bad params");
        return 0;
    }

    /* DBG(("exposure_callback_canvas canvas w/h %d/%d", canvas->gdk_image->width, canvas->gdk_image->height)); */

#ifdef HAVE_HWSCALE
    if (canvas->videoconfig->hwscale) {
        int tw, th;
        GdkGLContext *gl_context = gtk_widget_get_gl_context(w);
        GdkGLDrawable *gl_drawable = gtk_widget_get_gl_drawable(w);
        gdk_gl_drawable_gl_begin(gl_drawable, gl_context);

/* XXX make use of glXBindTexImageEXT aka texture from pixmap extension */

        glClear(GL_COLOR_BUFFER_BIT);
        glDisable (GL_DEPTH_TEST);

/* GL_TEXTURE_RECTANGLE is standardised as _EXT in OpenGL 1.4. Here's some
 * aliases in the meantime. */
#ifndef GL_TEXTURE_RECTANGLE_EXT
    #if defined(GL_TEXTURE_RECTANGLE_NV)
        #define GL_TEXTURE_RECTANGLE_EXT GL_TEXTURE_RECTANGLE_NV
    #elif defined(GL_TEXTURE_RECTANGLE_ARB)
        #define GL_TEXTURE_RECTANGLE_EXT GL_TEXTURE_RECTANGLE_ARB
    #else
        #error "Your headers do not supply GL_TEXTURE_RECTANGLE. Disable HWSCALE and try again."
    #endif
#endif

        glEnable(GL_TEXTURE_RECTANGLE_EXT);
        glBindTexture(GL_TEXTURE_RECTANGLE_EXT, canvas->screen_texture);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_RECTANGLE_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        tw = canvas->gdk_image->width;
        th = canvas->gdk_image->height;
#ifdef __BIG_ENDIAN__
#ifndef GL_ABGR_EXT
    #error "Your headers do not supply GL_ABGR_EXT. Disable HWSCALE and try again."
#endif
        glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, tw, th, 0, GL_ABGR_EXT, GL_UNSIGNED_BYTE, canvas->hwscale_image);
#else
        glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas->hwscale_image);
#endif

        glBegin (GL_QUADS);

        /* Lower Right Of Texture */
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-(tw/2), (th/2));
        /* Upper Right Of Texture */
        glTexCoord2f(0.0f, th); glVertex2f(-(tw/2), -(th/2));
        /* Upper Left Of Texture */
        glTexCoord2f(tw, th); glVertex2f((tw/2), -(th/2));
        /* Lower Left Of Texture */
        glTexCoord2f(tw, 0.0f); glVertex2f((tw/2), (th/2));

        glEnd ();

        gdk_gl_drawable_swap_buffers (gl_drawable);
        gdk_gl_drawable_gl_end (gl_drawable);
    } else
#endif
    {
        int x = e->area.x;
        int y = e->area.y;
        int width = e->area.width;
        int height = e->area.height;

        gdk_draw_image(w->window, app_gc, canvas->gdk_image, x, y, x, y, width, height);
    }

    return 0;
}

/* ------------------------------------------------------------------------- */
/*
    trigger recalculation of screen/window dimensions
 */
void ui_trigger_resize(void)
{
    GtkWidget *toplevel = get_active_toplevel();
    if ((toplevel) && (toplevel->window)) {
        DBG(("ui_trigger_resize"));
        gdk_flush();
        gdk_window_raise(toplevel->window);
    }
}
