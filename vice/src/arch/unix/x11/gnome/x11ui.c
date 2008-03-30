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
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <gdk/gdkx.h>
#include <pango/pango.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <X11/keysym.h>

#ifdef HAVE_X11_SUNKEYSYM_H
#include <X11/Sunkeysym.h>
#endif

#include "ui.h"
#include "uiarch.h"

#include "autostart.h"
#include "datasette.h"
#include "charset.h"
#include "drive/drive.h"
#include "fullscreenarch.h"
#include "imagecontents.h"
#include "ioutil.h"
#include "lib.h"
#include "log.h"
#include "kbd.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "mousedrv.h"
#include "resources.h"
#include "uicolor.h"
#include "uimenu.h"
#include "uisettings.h"
#include "uicommands.h"
#include "util.h"
#include "version.h"
#include "vsync.h"
#include "video.h"
#include "videoarch.h"
#include "vsiduiunix.h"
#include "x11ui.h"
#include "screenshot.h"
#include "event.h"
#include "uifliplist.h"


/* FIXME: We want these to be static.  */
GdkVisual *visual;
static int have_truecolor;
char last_attached_images[NUM_DRIVES][256]; /* FIXME MP */
char *last_attached_tape;
static char *last_menus[NUM_DRIVES];
static GtkWidget *last_drive_menus[NUM_DRIVES];

static Display *display;
int screen;
static int depth;

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

static GtkWidget *tape_menu, *speed_menu;
static GtkWidget *drive_menus[NUM_DRIVES];
GtkWidget *status_bar;
GtkWidget *video_ctrl_checkbox;
static GtkWidget *video_ctrl_checkbox_label;
static GtkWidget *event_rec_checkbox;
static GtkWidget *event_rec_checkbox_label;
static GtkWidget *event_playback_checkbox;
static GtkWidget *event_playback_checkbox_label;
static GtkStyle *ui_style_red;
static GtkStyle *ui_style_green;
static GdkCursor *blankCursor;
static GtkWidget *image_preview_list, *auto_start_button, *last_file_selection;
static GtkWidget *pal_ctrl_widget, *pal_ctrl_checkbox, *pal_tb;
static char *(*current_image_contents_func)(const char *, unsigned int unit);
/* static GdkFont *fixedfont, *textfont; */
/* FIXME, ask Xresources here */
/*
  static char *textfontname="-*-lucidatypewriter-medium-r-*-*-12-*";
  static char *fixedfontname="-freetype-VICE CBM-medium-r-normal-medium-12-120-100-72-m-104-symbol-0";
*/
static char *textfontname="Lucida Typewriter 8";
static char *fixedfontname="fxd 8";
/* static char *fixedfontname="Adolescence 12"; */


static int have_cbm_font = 0;
static int cursor_is_blank = 0;
static video_canvas_t *ui_cached_video_canvas;
static int statustext_display_time = 0;

static PangoFontDescription *text_font_desc;
static PangoFontDescription *fixed_font_desc;

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
GdkColormap *colormap;

/* Enabled drives.  */
ui_drive_enable_t enabled_drives;  /* used also in uicommands.c */

/* Color of the drive active LED.  */
static int *drive_active_led;

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
    GtkWidget *led1;
    GdkPixmap *led1_pixmap;
    GtkWidget *led2;
    GdkPixmap *led2_pixmap;
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

#define MAX_APP_SHELLS 10
typedef struct {
    String title;
    GtkWidget *shell;
    GtkWidget *canvas;
    GtkWidget *topmenu;
    GtkLabel *speed_label;
    GtkLabel *statustext;
    GtkAccelGroup *accel;
    drive_status_widget drive_status[NUM_DRIVES];
    tape_status_widget tape_status;
} app_shell_type;

static app_shell_type app_shells[MAX_APP_SHELLS];
static int num_app_shells = 0;

/* Pixels for updating the drive LED's state.  */
GdkColor *drive_led_on_red_pixel, *drive_led_on_green_pixel, 
*drive_led_off_pixel, *motor_running_pixel, *tape_control_pixel;
GdkColor *drive_led_on_red_pixels[16];
GdkColor *drive_led_on_green_pixels[16];


/* If != 0, we should save the settings. */
/* static int resources_have_changed = 0; */

/* ------------------------------------------------------------------------- */

static int alloc_colormap(void);
/*static int alloc_colors(const palette_t *palette, PIXEL pixel_return[]);*/
static GtkWidget* build_file_selector(const char *title,
				      GtkWidget **attach_write_protect,
				      int allow_autostart,
				      int show_preview,
				      const char *pat,
				      const char *default_dir);
static GtkWidget* build_show_text(const String text, int width, int height);
static GtkWidget* build_confirm_dialog(GtkWidget **confirm_dialog_message);
static gboolean enter_window_callback(GtkWidget *w, GdkEvent *e, gpointer p);
static gboolean exposure_callback_app(GtkWidget *w, GdkEventConfigure *e, gpointer p);
static gboolean exposure_callback_canvas(GtkWidget *w, GdkEvent *e, 
					 gpointer p);
static gboolean fliplist_popup_cb(GtkWidget *w, GdkEvent *event, 
				  gpointer data);
static gboolean tape_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data);
static gboolean update_menu_cb(GtkWidget *w, GdkEvent *event,gpointer data);
static gboolean speed_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data);

static GtkWidget* rebuild_contents_menu(int unit, const char *image_name);
extern GtkWidget* build_pal_ctrl_widget(video_canvas_t *canvas);

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

void ui_check_mouse_cursor()
{
#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
        return;
#endif

    if(_mouse_enabled) 
    {
#ifdef USE_XF86_EXTENSIONS
        if(fullscreen_is_enabled) {
	    int window_doublesize;

            if (resources_get_int("FullscreenDoubleSize",
				  &window_doublesize) < 0)
                return;
        }
#endif
	if (ui_cached_video_canvas->videoconfig->doublesizex)
	    mouse_accelx = 2;   
	
	if (ui_cached_video_canvas->videoconfig->doublesizey)
	    mouse_accely = 2;   

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
/*			 _ui_top_level->window,*/
			 app_shells[0].canvas->window,
			 blankCursor,
			 CurrentTime);
    }
    else if (cursor_is_blank) {
	/*        XUndefineCursor(display,XtWindow(canvas));*/
        gdk_keyboard_ungrab(CurrentTime);
        gdk_pointer_ungrab(CurrentTime);
    }
}

void ui_restore_mouse() {
#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
        return;
#endif
    if(_mouse_enabled && cursor_is_blank) {
	/*        XUndefineCursor(display,XtWindow(canvas));*/
        gdk_keyboard_ungrab(CurrentTime);
        gdk_pointer_ungrab(CurrentTime);
	cursor_is_blank = 0; 
    }
}

void initBlankCursor() {
    blankCursor = gdk_cursor_new(GDK_MOUSE);
}

/* ------------------------------------------------------------------------- */

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

    /* set X11 fontpath */
    if (access(PREFIX "/lib/vice/fonts/fonts.dir", R_OK) == 0)
    {
	const char *cmd = "xset fp+ " PREFIX "/lib/vice/fonts";
	
	if (system(cmd) != 0)
	    fprintf(stderr, _("Can't add fontpath `%s'.\n"), cmd);
	else
	    fprintf(stdout, _("Set fontpath: `%s'.\n"), cmd);
    }
}

/* Initialize the GUI and parse the command line. */
int ui_init(int *argc, char **argv)
{
    display = GDK_DISPLAY();

    screen = XDefaultScreen(display);
    atexit(ui_autorepeat_on);

    ui_common_init();

    ui_hotkey_init();

    enabled_drives = UI_DRIVE_ENABLE_NONE;

    return 0;
}

void ui_shutdown(void)
{
    ui_hotkey_shutdown();

    ui_common_shutdown();
}

typedef struct {
    char *name;
    GdkVisualType class;
} namedvisual_t;


gboolean delete_event(GtkWidget *w, GdkEvent *e, gpointer data) 
{
    vsync_suspend_speed_eval();
    ui_exit();
    /* ui_exit() will exit the application if user allows it. So if
       we return here then we should keep going => return TRUE */
    return TRUE;
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
        mouse_move((int)mevent->x, (int)mevent->y);
	/* printf("%d/%d\n", (int)mevent->x, (int)mevent->y); */
    }
}

static gboolean fliplist_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    int d = (int) data;
    if (event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	if (bevent->button == 1)
	{
	    if ((d >= 0)&& (d < NUM_DRIVES))
	    {
		uifliplist_update_menus(d+8, d+8);
		ui_menu_update_all_GTK();
		if (drive_menus[d])
		    gtk_menu_popup(GTK_MENU(drive_menus[d]),
				   NULL,NULL,NULL,NULL,
				   bevent->button, bevent->time);
	    }
	}
	else if (bevent->button == 3)
	{
	    if (strcmp(last_attached_images[d], "") == 0)
	    {
		if (last_drive_menus[d])
		    gtk_widget_destroy(last_drive_menus[d]);
		if (last_menus)
		    lib_free(last_menus[d]);
		last_menus[d] = NULL;
		return 0;
	    }
	    
	    if ((last_menus[d] == NULL) ||
		(strcmp(last_menus[d], last_attached_images[d]) != 0))
	    {
		if (last_drive_menus[d])
		    gtk_widget_destroy(last_drive_menus[d]);
		if (last_menus[d])
		    lib_free(last_menus[d]);
		last_menus[d] = lib_stralloc(last_attached_images[d]);
		last_drive_menus[d] = 
		    rebuild_contents_menu(d+8, last_menus[d]);
	    }
	    if (last_drive_menus[d])
		gtk_menu_popup(GTK_MENU(last_drive_menus[d]),
			       NULL, NULL, NULL, NULL,
			       bevent->button, bevent->time);
	}
    }
    return 0;
}

static gboolean tape_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    if(event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	if (bevent->button == 1)
	{
	    ui_menu_update_all_GTK();
	    if (tape_menu)
		gtk_menu_popup(GTK_MENU(tape_menu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	}
	else if (bevent->button == 3)
	{
	    static char *lasttapemenu;
	    static GtkWidget *ltapemenu;

	    if (last_attached_tape == NULL)
	    {
		if (ltapemenu)
		    gtk_widget_destroy(ltapemenu);
		if (lasttapemenu)
		    lib_free(lasttapemenu);
		lasttapemenu = NULL;
		return 0;
	    }

	    if ((lasttapemenu == NULL) ||
		(strcmp(lasttapemenu, last_attached_tape) != 0))
	    {
		if (ltapemenu)
		    gtk_widget_destroy(ltapemenu);
		if (lasttapemenu)
		    lib_free(lasttapemenu);
		lasttapemenu = lib_stralloc(last_attached_tape);
		ltapemenu = rebuild_contents_menu(1, lasttapemenu);
	    }
	    if (ltapemenu)
		gtk_menu_popup(GTK_MENU(ltapemenu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	}
    }
    return 0;
}

static gboolean update_menu_cb(GtkWidget *w, GdkEvent *event,gpointer data)
{
    ui_menu_update_all_GTK();
    return 0;
}

static gboolean speed_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
{
    if(event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	
	if (speed_menu)
	{
	    ui_menu_update_all_GTK();
	    gtk_menu_popup(GTK_MENU(speed_menu),NULL,NULL,NULL,NULL,
			   bevent->button, bevent->time);
	}
    }
    return 0;
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

    ui_log = log_open("X11");

    resources_get_int("DisplayDepth", &depth);

    if (depth != 0) {
	int i;

	for (i = 0; classes[i].name != NULL ; i++) {
	    if ((visual = gdk_visual_get_best_with_both(depth,
							classes[i].class)))
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
            have_truecolor = (classes[i].class == GDK_VISUAL_TRUE_COLOR);
        }
    } else {
	/* Autodetect. */
	visual = gdk_visual_get_system();
	have_truecolor = (visual->type == GDK_VISUAL_TRUE_COLOR);
	depth=visual->depth;
	log_message(ui_log, _("Found %dbit visual."), depth);
	
#if 0
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
		log_message(ui_log, _("Found %dbit/%s visual."),
			    depth, classes[j].name);
		have_truecolor = (classes[j].class == GDK_VISUAL_TRUE_COLOR);
		done = 1;
		break;
	    }
	}
	if (!done) {
	    log_error(ui_log, _("Cannot autodetect a proper visual."));
	    return -1;
	}
#endif
    }

    text_font_desc = pango_font_description_from_string(textfontname);
    if (!text_font_desc)
	log_error(ui_log, _("Cannot load text font %s."), textfontname);

    fixed_font_desc = pango_font_description_from_string(fixedfontname);
    if (fixed_font_desc)
	have_cbm_font = TRUE;
    else
    {
	log_warning(ui_log, _("Cannot load CBM font %s."), fixedfontname);
	fixed_font_desc = text_font_desc;
	have_cbm_font = FALSE;
    }
    have_cbm_font = FALSE;	/* XXX Fixme once CBM font is working */
	
/*     printf ("to String: %s\nto FN: %s\n", pango_font_description_to_string(fixed_font_desc),   pango_font_description_to_filename(fixed_font_desc)); */

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_init() < 0)
        return -1;
#endif

    return ui_menu_init();
}

int ui_init_finalize(void)
{
    return 0;
}

static void ui_update_pal_checkbox (GtkWidget *w, gpointer data)
{
    if (!w || !GTK_IS_TOGGLE_BUTTON(w))
	return;

    if (GTK_TOGGLE_BUTTON(w)->active)
	gtk_widget_show(pal_ctrl_widget);
    else
	gtk_widget_hide(pal_ctrl_widget);
}

static void ui_update_video_checkbox (GtkWidget *w, gpointer data)
{
    gtk_widget_hide(video_ctrl_checkbox);
    screenshot_stop_recording();
}

static void ui_update_event_checkbox (GtkWidget *w, gpointer data)
{
    if (data)
	event_playback_stop();
    else
	event_record_stop();
}

static void statusbar_setstatustext(const char *t)
{
    int i;
    
    for (i = 0; i < num_app_shells; i++)
	gtk_label_set_text(app_shells[i].statustext, t);
}

void ui_create_status_bar(GtkWidget *pane, int width, int height)
{
    /* Create the status bar on the bottom.  */
    GtkWidget *speed_label, *drive_box, *frame, *event_box, *pcb, *vcb, *tmp;
    int i;
    app_shell_type *as;
    GtkTooltips *video_tooltip;

    status_bar = gtk_hbox_new(FALSE, 0);

    gtk_box_pack_end(GTK_BOX(pane),status_bar,FALSE,FALSE,0);
    gtk_widget_show(status_bar);

    event_box = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(status_bar), event_box, TRUE, TRUE,0);
    gtk_widget_show(event_box);
    g_signal_connect(G_OBJECT(event_box),
		     "button-press-event",
		     G_CALLBACK(speed_popup_cb), (gpointer) NULL);
    
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(event_box), frame);
    gtk_widget_show(frame);
    
    speed_label = gtk_label_new("");
    app_shells[num_app_shells - 1].speed_label = (GtkLabel*) speed_label;
    gtk_misc_set_alignment (GTK_MISC (speed_label), 0, -1);
    gtk_container_add(GTK_CONTAINER(frame), speed_label);
    gtk_widget_show(speed_label);      

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
    
    tmp = gtk_label_new("");
    app_shells[num_app_shells - 1].statustext = (GtkLabel*) tmp;
    gtk_misc_set_alignment (GTK_MISC (tmp), 0, -1);
    gtk_container_add(GTK_CONTAINER(frame), tmp);
    gtk_widget_show(tmp);      
    gtk_box_pack_start(GTK_BOX(status_bar), frame, TRUE, TRUE,0);
    gtk_widget_show(frame);

    as=&app_shells[num_app_shells - 1];

    /* PAL Control checkbox */
    pal_ctrl_checkbox = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(pal_ctrl_checkbox), GTK_SHADOW_IN);
    pcb = pal_tb = gtk_check_button_new_with_label(_("PAL Controls"));
    GTK_WIDGET_UNSET_FLAGS (pcb, GTK_CAN_FOCUS);
    g_signal_connect(G_OBJECT(pcb), "toggled", 
		     G_CALLBACK(ui_update_pal_checkbox),
		     pcb);
    gtk_container_add(GTK_CONTAINER(pal_ctrl_checkbox), pcb);
    gtk_widget_show(pcb);
    gtk_box_pack_start(GTK_BOX(status_bar), pal_ctrl_checkbox, 
		       FALSE, FALSE, 0);

    if ((resources_get_int("PALEmulation", &i) != -1) && (i > 0))
	gtk_widget_show(pal_ctrl_checkbox);
    else	
	gtk_widget_hide(pal_ctrl_checkbox);
    
    /* Video Control checkbox */
    video_ctrl_checkbox = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(video_ctrl_checkbox), GTK_SHADOW_IN);
    
    video_ctrl_checkbox_label = gtk_label_new(_("audio/video recording"));
    vcb = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(vcb), video_ctrl_checkbox_label);
    gtk_widget_show(video_ctrl_checkbox_label);
    GTK_WIDGET_UNSET_FLAGS (pcb, GTK_CAN_FOCUS);
    g_signal_connect(G_OBJECT(vcb), "clicked", 
		     G_CALLBACK(ui_update_video_checkbox),
		     vcb);
    gtk_container_add(GTK_CONTAINER(video_ctrl_checkbox), vcb);
    gtk_widget_show(vcb);
    gtk_box_pack_start(GTK_BOX(status_bar), video_ctrl_checkbox, 
		       FALSE, FALSE, 0);
    video_tooltip = gtk_tooltips_new();
    gtk_tooltips_set_tip(GTK_TOOLTIPS(video_tooltip), vcb,
			 _("click to stop recording"), NULL);
    /* Event record control checkbox */
    event_rec_checkbox = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(event_rec_checkbox), GTK_SHADOW_IN);
    
    event_rec_checkbox_label = gtk_label_new(_("event recording"));
    vcb = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(vcb), event_rec_checkbox_label);
    gtk_widget_show(event_rec_checkbox_label);
    GTK_WIDGET_UNSET_FLAGS (pcb, GTK_CAN_FOCUS);
    g_signal_connect(G_OBJECT(vcb), "clicked", 
		     G_CALLBACK(ui_update_event_checkbox),
		     (gpointer) 0);
    gtk_container_add(GTK_CONTAINER(event_rec_checkbox), vcb);
    gtk_widget_show(vcb);
    gtk_box_pack_start(GTK_BOX(status_bar), event_rec_checkbox, 
		       FALSE, FALSE, 0);
    video_tooltip = gtk_tooltips_new();
    gtk_tooltips_set_tip(GTK_TOOLTIPS(video_tooltip), vcb,
			 _("click to stop recording"), NULL);
    /* Event playback control checkbox */
    event_playback_checkbox = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(event_playback_checkbox),
			       GTK_SHADOW_IN);
    
    event_playback_checkbox_label = gtk_label_new(_("event playback"));
    vcb = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(vcb), event_playback_checkbox_label);
    gtk_widget_show(event_playback_checkbox_label);
    GTK_WIDGET_UNSET_FLAGS (pcb, GTK_CAN_FOCUS);
    g_signal_connect(G_OBJECT(vcb), "clicked", 
		     G_CALLBACK(ui_update_event_checkbox),
		     (gpointer) 1);
    gtk_container_add(GTK_CONTAINER(event_playback_checkbox), vcb);
    gtk_widget_show(vcb);
    gtk_box_pack_start(GTK_BOX(status_bar), event_playback_checkbox, 
		       FALSE, FALSE, 0);
    video_tooltip = gtk_tooltips_new();
    gtk_tooltips_set_tip(GTK_TOOLTIPS(video_tooltip), vcb,
			 _("click to stop playback"), NULL);
    
    /* drive stuff */
    drive_box = gtk_hbox_new(FALSE, 0);
    for (i = 0; i < NUM_DRIVES; i++) {
	char label[256];
	
	as->drive_status[i].event_box = gtk_event_box_new();

	frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
	
	sprintf(label, _("Drive %d: "), i + 8);
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
			     _("<empty>"), NULL);

	/* Label */
	as->drive_status[i].label = (void *)gtk_label_new(g_strdup(label));
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box), 
			   (GtkWidget *)as->drive_status[i].label,
			   TRUE,TRUE,0);
	gtk_widget_show((GtkWidget *)as->drive_status[i].label);

#if 0
	as->drive_status[i].image = (void *)gtk_label_new(_("<empty>"));
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

	/* Single Led */
	as->drive_status[i].led_pixmap = 
	    gdk_pixmap_new(_ui_top_level->window, LED_WIDTH, LED_HEIGHT, 
			   depth);
	as->drive_status[i].led = 
	    gtk_image_new_from_pixmap(as->drive_status[i].led_pixmap, NULL);
	gtk_widget_set_size_request(as->drive_status[i].led, 
				    LED_WIDTH, LED_HEIGHT);
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box),
			   (GtkWidget *)as->drive_status[i].led,
			   FALSE,FALSE, 4);
	gtk_widget_show(as->drive_status[i].led);

	/* Led1 for double Led drive */
	as->drive_status[i].led1_pixmap = 
	    gdk_pixmap_new(_ui_top_level->window, LED_WIDTH/2, LED_HEIGHT, 
			   depth);
	as->drive_status[i].led1 = 
	    gtk_image_new_from_pixmap(as->drive_status[i].led1_pixmap, NULL);
	gtk_widget_set_size_request(as->drive_status[i].led1, LED_WIDTH/2, 
				    LED_HEIGHT);
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box),
			   (GtkWidget *)as->drive_status[i].led1,
			   FALSE, FALSE, 1);
	gtk_widget_show(as->drive_status[i].led1);

	/* Led2 for double Led drive */
	as->drive_status[i].led2_pixmap = 
	    gdk_pixmap_new(_ui_top_level->window, LED_WIDTH/2, LED_HEIGHT, 
			   depth);
	as->drive_status[i].led2 = 
	    gtk_image_new_from_pixmap(as->drive_status[i].led2_pixmap, NULL);
	gtk_widget_set_size_request(as->drive_status[i].led2, LED_WIDTH/2, 
				    LED_HEIGHT);
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box),
			   (GtkWidget *)as->drive_status[i].led2,
			   FALSE,FALSE, 1);
	gtk_widget_show(as->drive_status[i].led2);

	/* Pack everything together */
	gtk_box_pack_start(GTK_BOX(drive_box), as->drive_status[i].event_box, 
			   FALSE, FALSE, 0);

	gtk_widget_set_events(as->drive_status[i].event_box, 
			      GDK_BUTTON_PRESS_MASK |
			      GDK_BUTTON_RELEASE_MASK |
	                      GDK_ENTER_NOTIFY_MASK);
	g_signal_connect(G_OBJECT(as->drive_status[i].event_box),
			 "button-press-event",
			 G_CALLBACK(fliplist_popup_cb), (gpointer) i);
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
    as->tape_status.label = gtk_label_new(_("Tape 000"));
    gtk_container_add(GTK_CONTAINER(as->tape_status.box),
		      as->tape_status.label);
    gtk_misc_set_alignment (GTK_MISC (as->tape_status.label), 0, -1);
    gtk_widget_show(as->tape_status.label);

    /* Tape control */
    as->tape_status.control_pixmap = 
	gdk_pixmap_new(_ui_top_level->window, CTRL_WIDTH, CTRL_HEIGHT, depth);
    as->tape_status.control = 
	gtk_image_new_from_pixmap(as->tape_status.control_pixmap, NULL);
    gtk_widget_set_size_request(as->tape_status.control, CTRL_WIDTH, 
				CTRL_HEIGHT);
    gtk_box_pack_start(GTK_BOX(as->tape_status.box), as->tape_status.control,
		       FALSE, FALSE, 4);
    gtk_widget_show(as->tape_status.control);

    gtk_widget_set_events(as->tape_status.event_box, 
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK | 
			  GDK_ENTER_NOTIFY_MASK);
    g_signal_connect(G_OBJECT(as->tape_status.event_box),
		     "button-press-event",
		     G_CALLBACK(tape_popup_cb), (gpointer) NULL);

    gtk_box_pack_start(GTK_BOX(status_bar), as->tape_status.event_box, 
		       FALSE, FALSE, 0);
    gtk_widget_show(as->tape_status.event_box);
    gtk_widget_show(status_bar);

    for (i = 0; i < NUM_DRIVES; i++) {
#if 0
	int ih, iw;
	gdk_window_get_size_request(((GtkWidget *)as->drive_status[i].image)->window, 
			    &iw, &ih);
	gtk_widget_set_size_request(as->drive_status[i].image, width / 3, ih);
#endif
	gtk_widget_hide(as->drive_status[i].event_box);	/* Hide Drive widget */
	gdk_window_set_cursor (as->drive_status[i].event_box->window, 
			       gdk_cursor_new (GDK_HAND1)); 
    }
    gtk_widget_hide(as->tape_status.event_box);	/* Hide Tape widget */
    gdk_window_set_cursor (as->tape_status.event_box->window, 
			   gdk_cursor_new (GDK_HAND1)); 

    /* finalize event-box */
    gdk_window_set_cursor (event_box->window, 
			   gdk_cursor_new (GDK_HAND1)); 
}

int x11ui_get_display_depth(void)
{
    return depth;
}

Display *x11ui_get_display_ptr(void)
{
    return display;
}

GdkVisual *x11ui_get_visual()
{
    return visual;
}

Window x11ui_get_X11_window()
{
    return GDK_WINDOW_XID(_ui_top_level->window);
}

int x11ui_get_screen()
{
    return screen;
}

gboolean kbd_event_handler(GtkWidget *w, GdkEvent *report,gpointer gp);

/* Create a shell with a canvas widget in it.  */
int x11ui_open_canvas_window(video_canvas_t *c, const char *title,
                             int width, int height, int no_autorepeat)
{
    GtkWidget *new_window, *new_pane, *new_canvas, *topmenu;
    int i;
    
    if (++num_app_shells > MAX_APP_SHELLS) {
	log_error(ui_log, _("Maximum number of toplevel windows reached."));
	return -1;
    }

    new_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_events(new_window,
			  GDK_LEAVE_NOTIFY_MASK |
			  GDK_ENTER_NOTIFY_MASK |			  
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_KEY_PRESS_MASK |
			  GDK_KEY_RELEASE_MASK |
			  GDK_FOCUS_CHANGE_MASK |
			  GDK_POINTER_MOTION_MASK |
			  GDK_EXPOSURE_MASK);
    if (!_ui_top_level)
	_ui_top_level = new_window;

    new_pane = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(new_window), new_pane);
    gtk_widget_show(new_pane);
    
    topmenu = gtk_menu_bar_new();
    gtk_widget_show(topmenu);
    g_signal_connect(G_OBJECT(topmenu),"button-press-event",
		     G_CALLBACK(update_menu_cb),NULL);
    gtk_box_pack_start(GTK_BOX(new_pane),topmenu, FALSE, TRUE,0);

    gtk_widget_show(new_window);
    if (vsid_mode)
	new_canvas = build_vsid_ctrl_widget();
    else
    {
	GtkRcStyle *rc_style;
	GdkColor color;

	alloc_colormap();
        gtk_widget_push_colormap (colormap);
	new_canvas = gtk_drawing_area_new();
        gtk_widget_pop_colormap ();

	/* Go through the motions to change the background to black.
	   GTK+ 2.0 simplifies this with gtk_widget_modify_bg. */
	gdk_color_parse("black", &color);
	rc_style = gtk_rc_style_new();
	rc_style->bg[GTK_STATE_NORMAL] = color;
	rc_style->color_flags[GTK_STATE_NORMAL] = GTK_RC_BG;
	gtk_widget_modify_style(new_canvas, rc_style);
        /* FIXME: Old gtk libraries do stupid things if this is
	   unreferenced.  */
	/* gtk_rc_style_unref(rc_style); */
    }
    
    gtk_widget_set_events(new_canvas,
			  GDK_LEAVE_NOTIFY_MASK |
			  GDK_ENTER_NOTIFY_MASK |			  
			  GDK_BUTTON_PRESS_MASK |
			  GDK_BUTTON_RELEASE_MASK |
			  GDK_KEY_PRESS_MASK |
			  GDK_KEY_RELEASE_MASK |
			  GDK_FOCUS_CHANGE_MASK |
			  GDK_POINTER_MOTION_MASK |
			  GDK_STRUCTURE_MASK |
			  GDK_EXPOSURE_MASK);

    gtk_box_pack_start(GTK_BOX(new_pane),new_canvas,TRUE,TRUE,0);
    gtk_widget_show(new_canvas);

    /* XVideo must be refreshed when the application window is moved. */
    g_signal_connect(G_OBJECT(new_window), "configure-event",
		     G_CALLBACK(exposure_callback_app),
		     (void*) c);
    g_signal_connect(G_OBJECT(new_canvas),"expose-event",
		     G_CALLBACK(exposure_callback_canvas),
		     (void*) c);
    g_signal_connect(G_OBJECT(new_canvas),"enter-notify-event",
		     G_CALLBACK(enter_window_callback),
		     NULL);

    if (!vsid_mode)
    {
	x11ui_resize_canvas_window(new_canvas, width, height, c->videoconfig->hwscale);
    }
    
    if (c->videoconfig->hwscale && !vsid_mode) {
        gint window_width, window_height;
        resources_get_int("WindowWidth", &window_width);
        resources_get_int("WindowHeight", &window_height);
        gtk_window_resize(GTK_WINDOW(new_window->window), window_width, 
			  window_height);
    }
    gtk_widget_show(new_canvas);

    ui_create_status_bar(new_pane, width, height);
    pal_ctrl_widget = build_pal_ctrl_widget(c);
    gtk_box_pack_end(GTK_BOX(new_pane), pal_ctrl_widget, FALSE, FALSE, 0);
    gtk_widget_hide(pal_ctrl_widget);
    
    GTK_WIDGET_SET_FLAGS(new_canvas,GTK_CAN_FOCUS);
    gtk_widget_grab_focus (new_canvas);
    if (no_autorepeat) {
        g_signal_connect(G_OBJECT(new_window),"enter-notify-event",
			 G_CALLBACK(ui_autorepeat_off),NULL);
        g_signal_connect(G_OBJECT(new_window),"leave-notify-event",
			 G_CALLBACK(ui_autorepeat_on),NULL);
    }
    g_signal_connect(G_OBJECT(new_canvas),"key-press-event",
		     G_CALLBACK(kbd_event_handler),NULL);
    g_signal_connect(G_OBJECT(new_canvas),"key-release-event",
		     G_CALLBACK(kbd_event_handler),NULL);
    g_signal_connect(G_OBJECT(new_window),"enter-notify-event",
		     G_CALLBACK(kbd_event_handler),NULL);
    g_signal_connect(G_OBJECT(new_window),"leave-notify-event",
		     G_CALLBACK(kbd_event_handler),NULL);
    g_signal_connect(G_OBJECT(new_canvas),"button-press-event",
		     G_CALLBACK(mouse_handler),NULL);
    g_signal_connect(G_OBJECT(new_canvas),"button-release-event",
		     G_CALLBACK(mouse_handler),NULL);
    g_signal_connect(G_OBJECT(new_window),"motion-notify-event",
		     G_CALLBACK(mouse_handler),NULL);
    g_signal_connect(G_OBJECT(new_window),"delete_event",
		     G_CALLBACK(delete_event),NULL);
    g_signal_connect(G_OBJECT(new_window),"destroy_event",
		     G_CALLBACK(delete_event),NULL);

    GtkAccelGroup* accel = gtk_accel_group_new();
    gtk_window_add_accel_group (GTK_WINDOW (new_window), accel);

    app_shells[num_app_shells - 1].shell = new_window;
    app_shells[num_app_shells - 1].canvas = new_canvas;
    app_shells[num_app_shells - 1].title = lib_stralloc(title);
    app_shells[num_app_shells - 1].topmenu = topmenu;
    app_shells[num_app_shells - 1].accel = accel;

    gtk_window_set_title(GTK_WINDOW(new_window),title);

    if (vsid_mode)
	return 0;
    
    if (!app_gc)
	app_gc = gdk_gc_new(new_window->window);

    if (uicolor_alloc_colors(c) < 0)
        return -1;

    /* This is necessary because the status might have been set before we
       actually open the canvas window. e.g. by commandline */
    ui_enable_drive_status(enabled_drives, drive_active_led);
    /* make sure that all drive status widgets are initialized.
       This is needed for proper dual disk/dual led drives (8050, 8250). */
    for (i = 0; i < NUM_DRIVES; i++)
	ui_display_drive_led(i, 1000, 1000);

    ui_style_red = gtk_style_new();
    ui_style_red->fg[GTK_STATE_NORMAL] = *drive_led_on_red_pixel;
    ui_style_red->fg[GTK_STATE_ACTIVE] = *drive_led_on_red_pixel;
    ui_style_red->fg[GTK_STATE_SELECTED] = *drive_led_on_red_pixel;
    ui_style_red->fg[GTK_STATE_PRELIGHT] = *drive_led_on_red_pixel;
    gtk_widget_set_style(video_ctrl_checkbox_label, ui_style_red);
    gtk_widget_set_style(event_rec_checkbox_label, ui_style_red);

    ui_style_green = gtk_style_new();
    ui_style_green->fg[GTK_STATE_NORMAL] = *drive_led_on_green_pixel;
    ui_style_green->fg[GTK_STATE_ACTIVE] = *drive_led_on_green_pixel;
    ui_style_green->fg[GTK_STATE_SELECTED] = *drive_led_on_green_pixel;
    ui_style_green->fg[GTK_STATE_PRELIGHT] = *drive_led_on_green_pixel;
    gtk_widget_set_style(event_playback_checkbox_label, ui_style_green);

    initBlankCursor();

    c->emuwindow = new_canvas;
    ui_cached_video_canvas = c;
    
    return 0;
}

#ifndef GNOME_MENUS
/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(ui_menu_entry_t *menu)
{
    left_menu = gtk_menu_new();
    ui_menu_create(left_menu, NULL, "LeftMenu", menu);
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(ui_menu_entry_t *menu)
{
    right_menu = gtk_menu_new();
    ui_menu_create(right_menu, NULL, "RightMenu", menu);
}

#ifdef OLD_TOPMENU
void ui_set_topmenu(void)
{
    int i;
    GtkWidget *commands, *settings, *help, *help_menu;
    
    for (i = 0; i < num_app_shells; i++)
    {
	commands = gtk_menu_item_new_with_label(_("Commands"));
	gtk_widget_show(commands);
	settings = gtk_menu_item_new_with_label(_("Settings"));
	gtk_widget_show(settings);
    
	help_menu = ui_menu_create("Help", ui_help_commands_menu, NULL);
	help = gtk_menu_item_new_with_label(_("Help"));
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
#endif

void ui_set_topmenu(ui_menu_entry_t *menu)
{
    int i;

    for (i = 0; i < num_app_shells; i++)
    {
	gtk_container_foreach(
	    GTK_CONTAINER(app_shells[i].topmenu), 
	    (GtkCallback) gtk_widget_destroy,
	    NULL);
    }


        for (i = 0; i < num_app_shells; i++)
        {
            ui_menu_create(app_shells[i].topmenu, app_shells[i].accel, "TopLevelMenu", menu);
        }
}

void ui_set_speedmenu(ui_menu_entry_t *menu)
{
    if (speed_menu)
	gtk_widget_destroy(speed_menu);
    speed_menu = gtk_menu_new();
    ui_menu_create(speed_menu, NULL, "SpeedMenu", menu);
}

#else  /* GNOME_MENUS */

/* Attach `w' as the left menu of all the current open windows.  */
void ui_set_left_menu(GnomeUIInfo *w)
{
    left_menu = gtk_menu_new();
    main_menu[0].type = GNOME_APP_UI_SUBTREE;
    main_menu[0].label = _("Commands");
    main_menu[0].moreinfo = w;
    
    gnome_app_fill_menu(GTK_MENU_SHELL(left_menu), w, NULL, FALSE, 0);
}

/* Attach `w' as the right menu of all the current open windows.  */
void ui_set_right_menu(GnomeUIInfo *w)
{
    right_menu = gtk_menu_new();
    main_menu[1].type = GNOME_APP_UI_SUBTREE;
    main_menu[1].label = _("Settings");
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
    int value;
    char *s = util_concat("Exit ", machine_name, _(" emulator"), NULL);

#ifdef USE_XF86_EXTENSIONS
    fullscreen_suspend(1);
#endif
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
	    } 
	    else if (b == UI_BUTTON_CANCEL) {
                lib_free(s);
		return;
            }
	}
	ui_autorepeat_on();
	ui_restore_mouse();
#ifdef USE_XF86_EXTENSIONS
        fullscreen_suspend(0);
#endif
	ui_dispatch_events();

	/* remove fontpath, Don't care about result */
	system("xset fp- " PREFIX "/lib/vice/fonts");

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
        colormap = gdk_colormap_get_system();
    } else {
        log_message(ui_log, _("Using private colormap."));
	colormap = gdk_colormap_new(visual, AllocNone);
    }

    gdk_drawable_set_colormap(_ui_top_level->window, colormap);

    return 0;
}

/* ------------------------------------------------------------------------- */

/* Show the speed index to the user.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
    int i;
    char str[256];
    int percent_int = (int)(percent + 0.5);
    int framerate_int = (int)(framerate + 0.5);
    for (i = 0; i < num_app_shells; i++) {
	if (!percent) {
	    gtk_label_set_text(app_shells[i].speed_label, warp_flag ? _("(warp)") : "");
	} else {
	    sprintf(str, "%d%%, %d fps %s",
		    percent_int, framerate_int, warp_flag ? _("(warp)") : "");
	    gtk_label_set_text(app_shells[i].speed_label, str);
	}
    }
    if (statustext_display_time > 0) {
        statustext_display_time--;
        if (statustext_display_time == 0)
            statusbar_setstatustext("");
    }
    if (!screenshot_is_recording())
	ui_update_video_checkbox(video_ctrl_checkbox, NULL);
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
	    if (enabled_drives && (enabled_drives & (1 << j))) {
		/* enabled + active drive */
		gtk_widget_show(app_shells[i].drive_status[j].event_box);
		gtk_widget_show(app_shells[i].drive_status[j].track_label);
		if (drive_num_leds(j) == 1) {
		    gtk_widget_show(app_shells[i].drive_status[j].led);
		    gtk_widget_hide(app_shells[i].drive_status[j].led1);
		    gtk_widget_hide(app_shells[i].drive_status[j].led2);
		} else {
		    gtk_widget_hide(app_shells[i].drive_status[j].led);
		    gtk_widget_show(app_shells[i].drive_status[j].led1);
		    gtk_widget_show(app_shells[i].drive_status[j].led2);
		}
	    } 
	    else if (!enabled_drives &&
		     (strcmp(last_attached_images[j], "") != 0)) {
		gtk_widget_show(app_shells[i].drive_status[j].event_box);
		gtk_widget_hide(app_shells[i].drive_status[j].track_label);
		gtk_widget_hide(app_shells[i].drive_status[j].led);
		gtk_widget_hide(app_shells[i].drive_status[j].led1);
		gtk_widget_hide(app_shells[i].drive_status[j].led2);
	    } else {
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

void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base,
			    unsigned int half_track_number)
{
    int i;
    static char str[256];
    double track_number = (double)half_track_number / 2.0;

    sprintf(str, "%.1f", (double)track_number);
    
    for (i = 0; i < num_app_shells; i++) {
	gtk_label_set_text(GTK_LABEL(app_shells[i].
				     drive_status[drive_number].track_label), 
			   str);
    }
}

void ui_display_drive_led(int drive_number, unsigned int led_pwm1,
                          unsigned int led_pwm2)
{
    int status = 0;
    int i, ci1, ci2;
    
    GdkColor *color;

    if (led_pwm1 > 100)
        status |= 1;
    if (led_pwm2 > 100)
        status |= 2;

    ci1 = (int) (((float)led_pwm1 / 1000) * 16) - 1;
    ci2 = (int) (((float)led_pwm2 / 1000) * 16) - 1;
    if (ci1 < 0)
	ci1 = 0;
    if (ci2 < 0)
	ci2 = 0;
    
    for (i = 0; i < num_app_shells; i++)
    {
	drive_status_widget *ds = &app_shells[i].drive_status[drive_number];

	color = status ? (drive_active_led[drive_number] 
			  ? drive_led_on_green_pixels[ci1] 
			  : drive_led_on_red_pixels[ci1]) 
	    : drive_led_off_pixel;
	gdk_gc_set_foreground(app_gc, color);
	gdk_draw_rectangle(ds->led_pixmap, app_gc, TRUE, 0, 
			   0, LED_WIDTH, LED_HEIGHT);
	gtk_widget_queue_draw(ds->led);

	color = (status & 1) ? (drive_active_led[drive_number] 
				? drive_led_on_green_pixels[ci1] 
				: drive_led_on_red_pixels[ci1]) 
	    : drive_led_off_pixel;
	gdk_gc_set_foreground(app_gc, color);
	gdk_draw_rectangle(ds->led1_pixmap, app_gc, TRUE, 0, 
			   0, LED_WIDTH/2, LED_HEIGHT);
	gtk_widget_queue_draw(ds->led1);

	color = (status & 2) ? (drive_active_led[drive_number] 
				? drive_led_on_green_pixels[ci2] 
				: drive_led_on_red_pixels[ci2]) 
	    : drive_led_off_pixel;
	gdk_gc_set_foreground(app_gc, color);
	gdk_draw_rectangle(ds->led2_pixmap, app_gc, TRUE, 0, 
			   0, LED_WIDTH/2, LED_HEIGHT);
	gtk_widget_queue_draw(ds->led2);
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
    util_fname_split(&(last_attached_images[drive_number][0]), NULL, &name);
    
    if (strcmp(name, "") == 0)
    {
	lib_free(name);
	name = lib_stralloc(_("<empty>"));
    }

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
	lib_free(name);
    
    ui_enable_drive_status(enabled_drives, drive_active_led);
}

/* ------------------------------------------------------------------------- */
/* tape stuff */
void ui_set_tape_status(int tape_status)
{
    static int ts;
    int i;

    if (ts == tape_status)
	return;
    ts = tape_status;
    for (i = 0; i < num_app_shells; i++) 
    {
	if (ts)
	{
	    gtk_widget_show(app_shells[i].tape_status.event_box);
	    gtk_widget_show(app_shells[i].tape_status.control);
	}
	else
	{
	    if (last_attached_tape)
	    {
		gtk_widget_show(app_shells[i].tape_status.event_box);
		gtk_widget_hide(app_shells[i].tape_status.control);
	    }
	    else
		gtk_widget_hide(app_shells[i].tape_status.event_box);
	}
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
    static char label[1024];
    int i;

    /* Translators: the string is fixed with 1024 byte! Don't exceed this
       limit !*/
    sprintf(label, _("Tape %03d"), counter % 1000);
    for (i = 0; i < num_app_shells; i++)
	gtk_label_set_text(GTK_LABEL(app_shells[i].tape_status.label), label);
    
}

void ui_display_tape_current_image(const char *image)
{
    char *name;
    int i;
    
    if (last_attached_tape)
	lib_free(last_attached_tape);
    last_attached_tape = lib_stralloc(image);
    util_fname_split(image, NULL, &name);

    for (i = 0; i < num_app_shells; i++)
    {
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tape_tooltip),
			     app_shells[i].tape_status.box->parent->parent, 
			     name, NULL);
    }
    if (name)
	lib_free(name);
}

/* ------------------------------------------------------------------------- */
void ui_display_recording(int recording_status)
{
    if (recording_status)
	gtk_widget_show(event_rec_checkbox);
    else
	gtk_widget_hide(event_rec_checkbox);
}

void ui_display_playback(int playback_status, char *version)
{
    if (playback_status)
	gtk_widget_show(event_playback_checkbox);
    else
	gtk_widget_hide(event_playback_checkbox);
}

/* Display a message in the title bar indicating that the emulation is
   paused.  */
void ui_display_paused(int flag)
{
    int i;
    char str[1024];

    for (i = 0; i < num_app_shells; i++) {
	if (flag) {
	    sprintf(str, _("%s (paused)"), app_shells[i].title);
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
    while (gtk_events_pending())
	ui_dispatch_next_event();
#ifdef USE_XF86_DGA2_EXTENSIONS
    {
	void dga2_mode_update(void);
	dga2_mode_update();
    }
#endif
}

void
x11ui_fullscreen(int i)
{
    if (i)
	gtk_window_fullscreen(GTK_WINDOW(_ui_top_level));
    else
	gtk_window_unfullscreen(GTK_WINDOW(_ui_top_level));
}

/* Resize one window. */
void x11ui_resize_canvas_window(ui_window_t w, int width, int height, int hwscale)
{
    gtk_window_set_resizable(GTK_WINDOW(gtk_widget_get_toplevel(w)), (gboolean)hwscale);

    if (hwscale)
    {
       width = 0;
       height = 0;
    }
    gtk_widget_set_size_request(w, width, height);
}

void x11ui_move_canvas_window(ui_window_t w, int x, int y)
{
    gdk_window_move(gdk_window_get_toplevel(w->window), x, y);
    gdk_flush();
}

void x11ui_canvas_position(ui_window_t w, int *x, int *y)
{
    gint tl_x, tl_y, pos_x, pos_y;
    gdk_flush();
    gdk_window_get_position(gdk_window_get_toplevel(w->window),
			    &tl_x, &tl_y);
    gdk_window_get_position(w->window, &pos_x, &pos_y);
    *x = (pos_x + tl_x);
    *y = (pos_y + tl_y);
/*
  printf("pos_x=%d, pos_y=%d, tl_x=%d, tl_y=%d, x=%d, y=%d\n",
  pos_x, pos_y, tl_x, tl_y, *x, *y);
*/
    gdk_window_raise(gdk_window_get_toplevel(w->window));
}

void x11ui_get_widget_size(ui_window_t win, int *w, int *h)
{
    GtkRequisition req;

    gtk_widget_size_request(win, &req);
    *w = (int) req.width;
    *h = (int) req.height;
}

void x11ui_destroy_widget(ui_window_t w)
{
    gtk_widget_destroy(w);
}

/* Enable autorepeat. */
void ui_autorepeat_on(void)
{
}

/* Disable autorepeat. */
void ui_autorepeat_off(void)
{
}

void ui_update_pal_ctrls(int v)
{
    if (!pal_ctrl_checkbox)
	return;

    if (v)
    {
	if (pal_tb)
	{
	    gtk_widget_show(pal_ctrl_checkbox);
	    ui_update_pal_checkbox(GTK_WIDGET(pal_tb), NULL);
	}
	return;
    }
    gtk_widget_hide(pal_ctrl_checkbox);
    gtk_widget_hide(pal_ctrl_widget);
}

/* ------------------------------------------------------------------------- */

/* Button callbacks.  */

#define DEFINE_BUTTON_CALLBACK(button)					\
    void  cb_##button(GtkWidget *w, ui_callback_data_t client_data)	\
    {									\
        *((ui_button_t *)client_data) = button;				\
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

/* Message Helper */
    static void 
    ui_message2(const GtkMessageType type, const char *msg, const char *title)
{
    static GtkWidget* msgdlg;

    vsync_suspend_speed_eval();
    msgdlg = gtk_message_dialog_new(GTK_WINDOW(_ui_top_level), 
				    GTK_DIALOG_DESTROY_WITH_PARENT,
				    type,
				    GTK_BUTTONS_OK,
				    msg,
				    NULL);
    
    ui_popup(msgdlg, title, False);
    gtk_dialog_run(GTK_DIALOG(msgdlg));
    ui_unblock_shells();	/* ui_popdown can't be used in message_boxes */
    gtk_widget_destroy(msgdlg);

    /* still needed ? */
    ui_check_mouse_cursor();
    ui_dispatch_events();
}

/* Report a message to the user.  */
void ui_message(const char *format, ...)
{
    va_list ap;
    char str[1024];

    va_start(ap, format);
    vsprintf(str, format, ap);
    ui_message2(GTK_MESSAGE_INFO, str, _("VICE Message"));
}

/* Report an error to the user.  */
void ui_error(const char *format, ...)
{
    va_list ap;
    char str[1024];

    va_start(ap, format);
    vsprintf(str, format, ap);
    ui_message2(GTK_MESSAGE_ERROR, str, _("VICE Error"));
}

void ui_make_window_transient(GtkWidget *parent,GtkWidget *window)
{
    gtk_window_set_transient_for(GTK_WINDOW(window),GTK_WINDOW(parent));
#if 0
    gdk_window_set_colormap(window->window,colormap);
#endif
}

/* Report a message to the user, allow different buttons. */
ui_jam_action_t ui_jam_dialog(const char *format, ...)
{
    char str[1024];
    va_list ap;
    static GtkWidget *jam_dialog, *message;
    gint res;
    
    va_start(ap, format);

    if (console_mode) {
        vfprintf(stderr, format, ap);
	exit(0);
    }

    vsync_suspend_speed_eval();
    jam_dialog = gtk_dialog_new_with_buttons("", 
					     NULL,
					     GTK_DIALOG_DESTROY_WITH_PARENT,
					     _("Reset"), 0, 
					     _("Hard Reset"), 1,
					     _("Monitor"), 2,
					     NULL);
    g_signal_connect(G_OBJECT(jam_dialog),
		     "destroy",
		     G_CALLBACK(gtk_widget_destroyed),
		     &jam_dialog);

    vsprintf(str, format, ap);
    message = gtk_label_new(str);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(jam_dialog)->vbox),
		       message, TRUE, TRUE, 0);
    gtk_widget_show(message);
    gtk_dialog_set_default_response(GTK_DIALOG(jam_dialog), 0);

    ui_popup(jam_dialog, "VICE", False);
    res = gtk_dialog_run(GTK_DIALOG(jam_dialog));
    ui_popdown(jam_dialog);
    if (jam_dialog)
	gtk_widget_destroy(jam_dialog);

    ui_dispatch_events();

    switch (res) {
    case 2:
	ui_restore_mouse();
#ifdef USE_XF86_EXTENSIONS
        fullscreen_suspend(0);
#endif
	return UI_JAM_MONITOR;
    case 1:
        return UI_JAM_HARD_RESET;
    case 0:
    default:
        return UI_JAM_RESET;
    }

    return 0;
}

int ui_extend_image_dialog(void)
{
    ui_button_t b;

    vsync_suspend_speed_eval();
    b = ui_ask_confirmation(_("Extend disk image"),
                            (_("Do you want to extend the disk image"
			       " to 40 tracks?")));
    return (b == UI_BUTTON_YES) ? 1 : 0;
}

UI_CALLBACK(ui_popup_selected_file)
{
    int unit = ((int) UI_MENU_CB_PARAM) >> 24;
    int selected = ((int) UI_MENU_CB_PARAM) & 0x00ffffff;
    char *tmp;
    
    if (unit > 8)
    {
	ui_message(_("Autostart not possible for unit %d"), unit);
	return;
    }
    else if (unit == 8)
    {
	tmp = lib_stralloc(last_attached_images[0]);
	if (autostart_disk(last_attached_images[0], NULL, selected,
			   AUTOSTART_MODE_RUN) < 0)
	    ui_error(_("Can't autostart selection %d in image %s"), selected,
		     tmp);
	lib_free(tmp);
    }
    else if (unit == 1)
    {
	tmp = lib_stralloc(last_attached_tape);
	if (autostart_tape(last_attached_tape, NULL, selected,
			   AUTOSTART_MODE_RUN) < 0)
	    ui_error(_("Can't autostart selection %d in image %s"), selected,
		     tmp);
	lib_free(tmp);
    }
}

static void menu_set_style(GtkWidget *w, gpointer data)
{
    if (GTK_IS_CONTAINER(w))
	gtk_container_foreach(GTK_CONTAINER(w), menu_set_style, data);
    else if (GTK_IS_LABEL(w))
	gtk_widget_set_style(w, (GtkStyle *) data);
}

static GtkWidget *rebuild_contents_menu(int unit, const char *name)
{
    ui_menu_entry_t *menu;
    int limit = 16;
    int fno = 0, mask;
    char *title, *tmp, *s, *tmp1, *tmp2;
    GtkWidget *menu_widget;
    GtkStyle *menu_entry_style;

    if (unit == 1)
	s = image_contents_read_string(IMAGE_CONTENTS_TAPE, name, 0,
                                       IMAGE_CONTENTS_STRING_PETSCII);
    else
	s = image_contents_read_string(IMAGE_CONTENTS_DISK, name, 0,
                                       IMAGE_CONTENTS_STRING_PETSCII);

    if (s == NULL)
        return (GtkWidget *)NULL;

    menu = g_new(ui_menu_entry_t, limit + 1); /* +1 because we have to store
					         NULL as end delimiter */

    mask = unit << 24;
    memset(menu, 0, 2 * sizeof(ui_menu_entry_t));
    util_fname_split(name, NULL, &title);
    for (tmp = title; *tmp; tmp++)
	*tmp = toupper(*tmp);
    menu[fno].string = title;
    menu[fno].callback = (ui_callback_t) ui_popup_selected_file;
    menu[fno].callback_data = (ui_callback_data_t) (fno | mask);
    menu[fno].sub_menu = NULL;
    menu[fno].hotkey_keysym = 0;
    menu[fno].hotkey_modifier = 0;
    fno++;
    menu[fno].string = "--";
    fno++;
    
    tmp1 = tmp2 = s;
    tmp1 = util_find_next_line(tmp2);
    while (tmp1 > tmp2)
    {
	if (fno >= limit)
	{
	    limit *= 2;
	    menu = g_renew(ui_menu_entry_t, menu, limit + 1); /* ditto */
	}

	*(tmp1 - 1) = '\0';
	if (!have_cbm_font)
	    tmp2 = (char *)charset_petconvstring((BYTE *)tmp2, 1);

	menu[fno].string = tmp2;
	if (menu[fno].string[0] == '-')
	    menu[fno].string[0] = ' ';	    /* Arg, this is the line magic */
	menu[fno].callback = (ui_callback_t) ui_popup_selected_file;
	menu[fno].callback_data = (ui_callback_data_t) ((fno - 2) | mask);
	menu[fno].sub_menu = NULL;
	menu[fno].hotkey_keysym = 0;
	menu[fno].hotkey_modifier = 0;
	fno++;

	tmp2 = tmp1;
	tmp1 = util_find_next_line(tmp2);
    }
    if (strcmp(tmp2, "") != 0)	/* last line may be without newline */
    {
	menu[fno].string = tmp2;
	if (menu[fno].string[0] == '-')
	    menu[fno].string[0] = ' ';	    /* Arg, this is the line magic */
	menu[fno].callback = (ui_callback_t) ui_popup_selected_file;
	menu[fno].callback_data = (ui_callback_data_t) ((fno - 1) | mask);
	menu[fno].sub_menu = NULL;
	menu[fno].hotkey_keysym = 0;
	menu[fno].hotkey_modifier = 0;
	fno++;
    }
    memset(&menu[fno++], 0, sizeof(ui_menu_entry_t)); /* end delimiter */

    menu_widget = gtk_menu_new();
    ui_menu_create(menu_widget, NULL, title, menu);
    if (fixed_font_desc)
    {
	menu_entry_style = gtk_style_new();
	pango_font_description_free(menu_entry_style->font_desc);
	menu_entry_style->font_desc = fixed_font_desc;
	gtk_container_foreach(GTK_CONTAINER(menu_widget), 
			      menu_set_style, menu_entry_style);
	menu_entry_style = NULL;
    }

    /* Cleanup */
    lib_free(title);
    g_free(menu);
    lib_free(s);
    
    return menu_widget;
}

static void ui_fill_preview(GtkFileChooser *fs, gpointer data)
{
    char *tmp1, *tmp2, *contents = NULL;
    gchar *fname;
    GtkStyle *style;
    char *text[2];
    GtkListStore *store;
    GtkTreeIter iter;
    int row;
        
    g_return_if_fail(fs != NULL);
    g_return_if_fail(GTK_IS_FILE_CHOOSER(fs));

    fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fs));
    if (!fname || !current_image_contents_func)
	contents = lib_stralloc(_("NO IMAGE CONTENTS AVAILABLE"));
    else
    {
	struct stat st;
	if (stat(fname, &st) == 0)
	    if (S_ISREG(st.st_mode))
		contents = current_image_contents_func(fname, 0);
	lib_free(fname);
    }
    if (!contents)
	contents = lib_stralloc(_("NO IMAGE CONTENTS AVAILABLE"));

    if (fixed_font_desc)
    {
	style = gtk_style_new();
	pango_font_description_free(style->font_desc);
	style->font_desc = fixed_font_desc;
	gtk_widget_set_style(image_preview_list, style);
/*	g_object_unref(style);*/
    }

    tmp1 = tmp2 = contents;
    text[1] = NULL;

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(image_preview_list)));
    gtk_list_store_clear(store);

    row = 0;
    tmp1 = util_find_next_line(tmp2);
    while (tmp1 > tmp2)
    {
	*(tmp1 - 1) = '\0';
	if (!have_cbm_font)
	    tmp2 = (char *) charset_petconvstring((BYTE *) tmp2, 1);
	text[0] = tmp2;

	text[0]= g_strescape(text[0], NULL);

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, text[0], 1, row, -1);

	row++;
	tmp2 = tmp1;
	tmp1 = util_find_next_line(tmp2);
    }

    /* Last Line might be without newline char*/
    if (strcmp(tmp2, "") != 0)
    {
	text[0] = tmp2;

	gtk_list_store_append(store, &iter);
	gtk_list_store_set(store, &iter, 0, text[0], 1, row, -1);
    }

    lib_free(contents);
}

static gboolean 
ui_select_contents_cb(GtkTreeSelection *selection,
		      GtkTreeModel *model,
		      GtkTreePath *path,
		      gboolean path_currently_selected,
		      gpointer userdata)
{
    GtkTreeIter iter;
    int row = 0;
	
    if (gtk_tree_model_get_iter(model, &iter, path))
	gtk_tree_model_get(model, &iter, 1, &row, -1);
    ui_set_selected_file(row);
    if (row)
	gtk_button_clicked(GTK_BUTTON(auto_start_button));
    return TRUE;
}

/* File browser. */
char *ui_select_file(const char *title,
                     char *(*read_contents_func)(const char *,
						 unsigned int unit), unsigned int unit,
                     unsigned int allow_autostart, const char *default_dir,
                     const char *default_pattern, ui_button_t *button_return,
		     unsigned int show_preview, int *attach_wp)
{  
    static GtkWidget* file_selector = NULL;
    static char *filesel_dir = NULL;
    char *ret;
    char *current_dir = NULL;
    char *filename = NULL;
    GtkWidget *wp;
    gint res;

    /* reset old selection */
    ui_set_selected_file(0);

    /* we preserve the current directory over the invocations */
    current_dir = ioutil_current_dir(); /* might be changed elsewhere */
    if (filesel_dir != NULL) {
        ioutil_chdir(filesel_dir);
    }

    if (attach_wp)
	file_selector = build_file_selector(title, &wp, allow_autostart, 
					    show_preview, default_pattern, 
					    default_dir);
    else
	file_selector = build_file_selector(title, NULL, allow_autostart, 
					    show_preview, default_pattern, 
					    default_dir);

    g_signal_connect(G_OBJECT(file_selector),
		     "destroy",
		     G_CALLBACK(gtk_widget_destroyed),
		     &file_selector);

    current_image_contents_func = read_contents_func;
    res = gtk_dialog_run(GTK_DIALOG(file_selector));
    x11kbd_enter_leave();	/* reset meta_count if called via hotkey */
    switch (res)
    {
    case GTK_RESPONSE_ACCEPT:
    case GTK_RESPONSE_OK:
	filename = gtk_file_chooser_get_filename(
	    GTK_FILE_CHOOSER(file_selector));
	*button_return = UI_BUTTON_OK;
	break;
    case GTK_RESPONSE_APPLY:
	filename = gtk_file_chooser_get_filename(
	    GTK_FILE_CHOOSER(file_selector));
	*button_return = UI_BUTTON_AUTOSTART;
	break;
    default:
	*button_return = UI_BUTTON_CANCEL;
	auto_start_button = NULL;
	gtk_widget_destroy(file_selector);
	return NULL;
	break;
    }
    
    if (attach_wp)
	*attach_wp = GTK_TOGGLE_BUTTON(wp)->active;
    
    auto_start_button = NULL;
    gtk_widget_destroy(file_selector);

    /* `ret' gets always malloc'ed. caller has to free  */
    if (filename)
    {
        ret = lib_stralloc(filename);
	lib_free(filename);
    }
    else
	ret = lib_stralloc("");

    if (filesel_dir != NULL) {
        lib_free(filesel_dir);
    }
    filesel_dir = ioutil_current_dir();
    if (current_dir != NULL) {
        ioutil_chdir(current_dir);
	lib_free(current_dir);
    }
    return ret;
}

/* Ask for a string.  The user can confirm or cancel. */
ui_button_t ui_input_string(const char *title, const char *prompt, char *buf,
                            unsigned int buflen)
{
    GtkWidget *input_dialog, *entry, *label;
    gint res;
    ui_button_t ret;

    vsync_suspend_speed_eval();
    input_dialog = gtk_dialog_new_with_buttons(title,
					       NULL,
					       GTK_DIALOG_DESTROY_WITH_PARENT,
					       GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					       GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
					       NULL);
    g_signal_connect(G_OBJECT(input_dialog),
		     "destroy",
		     G_CALLBACK(gtk_widget_destroyed),
		     &input_dialog);

    entry = gtk_entry_new();
    
    label = gtk_label_new(prompt);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(input_dialog)->vbox),
		       label, FALSE, FALSE, 0);
    gtk_widget_show(label);

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(input_dialog)->vbox),
		       entry, FALSE, FALSE, 0);
    gtk_entry_set_text(
	GTK_ENTRY(GTK_ENTRY(entry)), buf);
    gtk_widget_show(entry);

    gtk_dialog_set_default_response(GTK_DIALOG(input_dialog), GTK_RESPONSE_ACCEPT);
    /*    XtSetKeyboardFocus(input_dialog, input_dialog_field);*/
    ui_popup(input_dialog, title, False);
    res = gtk_dialog_run(GTK_DIALOG(input_dialog));
    ui_popdown(input_dialog);

    if ((res == GTK_RESPONSE_ACCEPT) && input_dialog)
    {
	strncpy(buf, gtk_entry_get_text(GTK_ENTRY(entry)), buflen);
	ret = UI_BUTTON_OK;
    }
    else
	ret = UI_BUTTON_CANCEL;

    if (input_dialog)
	gtk_widget_destroy(input_dialog);
    
    return ret;
}

/* Display a text to the user. */
void ui_show_text(const char *title, const char *text, int width, int height)
{
    GtkWidget *show_text;

    vsync_suspend_speed_eval();
    show_text = build_show_text((String)text, width, height);
    g_signal_connect(G_OBJECT(show_text),
		     "destroy",
		     G_CALLBACK(gtk_widget_destroyed),
		     &show_text);
    ui_popup(show_text, title, False);
    gtk_dialog_run(GTK_DIALOG(show_text));
    ui_popdown(show_text);
    
    if (show_text)
	gtk_widget_destroy(show_text);
}

/* Ask for a confirmation. */
ui_button_t ui_ask_confirmation(const char *title, const char *text)
{
    static GtkWidget *confirm_dialog, *confirm_dialog_message;
    gint res;
    
    vsync_suspend_speed_eval();
    if (!confirm_dialog)
    {
	confirm_dialog = build_confirm_dialog(&confirm_dialog_message);
	g_signal_connect(G_OBJECT(confirm_dialog),
			 "destroy",
			 G_CALLBACK(gtk_widget_destroyed),
			 &confirm_dialog);
    }
    
    gtk_label_set_text(GTK_LABEL(confirm_dialog_message),text);

    ui_popup(confirm_dialog, title, False);
    res = gtk_dialog_run(GTK_DIALOG(confirm_dialog));
    ui_popdown(confirm_dialog);
    
    return (res == GTK_RESPONSE_YES) ? UI_BUTTON_YES 
	: (res == GTK_RESPONSE_NO) ? UI_BUTTON_NO 
	: UI_BUTTON_CANCEL;
}

ui_button_t 
ui_change_dir(const char *title, const char *prompt, char *buf, 
	      unsigned int buflen)
{
    GtkWidget *fc;
    gint res;
    gchar *fname = NULL;
    ui_button_t r;
    
    fc = gtk_file_chooser_dialog_new(title, GTK_WINDOW(_ui_top_level),
				     GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				     GTK_STOCK_CANCEL, 
				     GTK_RESPONSE_CANCEL,
				     GTK_STOCK_OPEN, 
				     GTK_RESPONSE_ACCEPT,
				     NULL);
    ui_popup(fc, title, False);
    res = gtk_dialog_run(GTK_DIALOG(fc));
    ui_popdown(fc);

    if ((res == GTK_RESPONSE_ACCEPT) && 
	(fname = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(fc))))
    {
	strncpy(buf, fname, buflen);
	r = UI_BUTTON_OK;
    }
    else
	r = UI_BUTTON_CANCEL;
    
    if (fname)
	lib_free(fname);
    return r;
}

/* Update the menu items with a checkmark according to the current resource
   values.  */
void ui_update_menus(void)
{
    ui_menu_update_all();
}

void ui_block_shells(void)
{
    int i;
    for (i = 0; i < num_app_shells; i++)
	gtk_widget_set_sensitive(app_shells[i].shell, FALSE);
}

void ui_unblock_shells(void)
{
    int i;
    for (i = 0; i < num_app_shells; i++)
	gtk_widget_set_sensitive(app_shells[i].shell, TRUE);
}

/* Pop up a popup shell and center it to the last visited AppShell */
void ui_popup(GtkWidget *w, const char *title, Boolean wait_popdown)
{
#ifdef USE_XF86_EXTENSIONS
    fullscreen_suspend(1);
#endif
    
    ui_restore_mouse();
    /* Keep sure that we really know which was the last visited shell. */
    ui_dispatch_events();
    gtk_window_set_title(GTK_WINDOW(w),title);

#if 0				/* this code centers popups arount the main 
				   emulation window,
				   We decided to let the WM take care of
				   placing popups */
    gtk_widget_set_parent_window(w,_ui_top_level->window);
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
#endif
    gtk_window_set_transient_for(GTK_WINDOW(w),GTK_WINDOW(_ui_top_level));
    gtk_widget_show(w);
    gdk_window_set_decorations (w->window, GDK_DECOR_ALL | GDK_DECOR_MENU);
    gdk_window_set_functions (w->window, GDK_FUNC_ALL | GDK_FUNC_RESIZE);
    
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
#ifdef USE_XF86_EXTENSIONS
    fullscreen_resume();
#endif
}

/* ------------------------------------------------------------------------- */
static void
sh_checkbox_cb(GtkWidget *w, gpointer data)
{
    g_return_if_fail(GTK_IS_CHECK_BUTTON(w));
    g_return_if_fail(GTK_IS_FILE_CHOOSER(data));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w)))
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(data), TRUE);
    else
	gtk_file_chooser_set_show_hidden(GTK_FILE_CHOOSER(data), FALSE);
}

/* These functions build all the widgets. */
static GtkWidget *build_file_selector(const char *title,
				      GtkWidget **attach_write_protect,
				      int allow_autostart,
				      int show_preview,
				      const char *pat,
				      const char *default_dir)
{  
    GtkWidget *fileselect, *scrollw, *wp_checkbox, *sh_checkbox, *extra;
    GtkFileFilter *ff = NULL, *allf;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    
    if (pat)
    {
	ff = gtk_file_filter_new();
	gtk_file_filter_add_pattern(ff, pat);
	gtk_file_filter_set_name(ff, pat);
    }
    allf = gtk_file_filter_new();
    gtk_file_filter_add_pattern(allf, "*");
    gtk_file_filter_set_name(allf, _("all files"));

    fileselect = gtk_file_chooser_dialog_new(title, GTK_WINDOW(_ui_top_level), 
					     GTK_FILE_CHOOSER_ACTION_OPEN, 
					     GTK_STOCK_CANCEL, 
					     GTK_RESPONSE_CANCEL,
					     GTK_STOCK_OPEN, 
					     GTK_RESPONSE_ACCEPT,
					     NULL);
    if (ff)
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fileselect), ff);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(fileselect), allf);

    if (default_dir)
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileselect), 
					    default_dir);
    /* Contents preview */
    if (show_preview)
    {
	store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);
	image_preview_list = gtk_tree_view_new_with_model(
	    GTK_TREE_MODEL (store));
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Contents"),
							   renderer,
							   "text", 0,
							   NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW(image_preview_list), column);
	
	gtk_widget_set_size_request(image_preview_list, 180, 180);
	gtk_tree_view_set_headers_clickable (GTK_TREE_VIEW(image_preview_list),
					     FALSE);
    
	gtk_container_set_border_width(GTK_CONTAINER(image_preview_list), 1);
	
	gtk_tree_selection_set_mode(
	    gtk_tree_view_get_selection(GTK_TREE_VIEW(image_preview_list)),
	    GTK_SELECTION_SINGLE);
	
	scrollw = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollw),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_container_set_border_width(GTK_CONTAINER(scrollw), 1);
	gtk_container_add(GTK_CONTAINER(scrollw), image_preview_list);
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
	gtk_file_chooser_set_preview_widget(GTK_FILE_CHOOSER(fileselect), scrollw);
	g_signal_connect (fileselect, "update-preview",
			  G_CALLBACK (ui_fill_preview), image_preview_list);
	if (allow_autostart)
	    gtk_tree_selection_set_select_function(
		gtk_tree_view_get_selection(GTK_TREE_VIEW(image_preview_list)), 
		ui_select_contents_cb, NULL, NULL);
    }
    
    extra = gtk_hbox_new(FALSE, 5);
    
    /* Write protect checkbox */
    if (attach_write_protect)
    {
	/* write-protect checkbox */
	wp_checkbox = gtk_check_button_new_with_label(
	    _("Attach write protected"));
	gtk_box_pack_start(GTK_BOX(extra), wp_checkbox, FALSE, FALSE, 5);
	gtk_widget_show(wp_checkbox);
	*attach_write_protect = wp_checkbox;
    }
    /* show hidden files */
    sh_checkbox = gtk_check_button_new_with_label(_("Show hidden files"));
    g_signal_connect(G_OBJECT(sh_checkbox), "toggled",
		     G_CALLBACK(sh_checkbox_cb), (gpointer) fileselect);
    gtk_box_pack_start(GTK_BOX(extra), sh_checkbox, FALSE, FALSE, 5);
    gtk_widget_show(sh_checkbox);

    gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER(fileselect), 
				       extra);
    gtk_widget_show(extra);
    
    if (allow_autostart)
	auto_start_button = gtk_dialog_add_button(
	    GTK_DIALOG(fileselect), _("Autostart"), GTK_RESPONSE_APPLY);
    
    last_file_selection = fileselect;
    return fileselect;
}

static GtkWidget* build_show_text(const String text, int width, int height)
{
    GtkWidget *show_text, *textw, *scrollw;
    GtkTextBuffer *tb;
    gchar *utf8_text;
    GError *error = NULL;
    
    utf8_text = g_convert(text, strlen(text), "UTF-8", "ISO-8859-1",
			  NULL, NULL, &error);
    if (error)
    {
	util_string_set(&utf8_text, _("Text cannot be displayed.\n"));
	g_error_free(error);
    }
    
    show_text = gtk_dialog_new_with_buttons("",
					    NULL,
					    GTK_DIALOG_DESTROY_WITH_PARENT,
					    GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
					    NULL);
    gtk_widget_set_size_request(show_text, width, height);

    scrollw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollw),
				    GTK_POLICY_AUTOMATIC,
				    GTK_POLICY_AUTOMATIC);
    
    textw = gtk_text_view_new();
    tb = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textw));
    gtk_text_buffer_set_text(tb, utf8_text, -1);
    gtk_container_add(GTK_CONTAINER(scrollw), textw);
    gtk_widget_show(textw);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(show_text)->vbox),
		       scrollw, TRUE, TRUE, 0);
    gtk_widget_show(scrollw);
    g_free(utf8_text);
    return show_text;
}

static GtkWidget* 
build_confirm_dialog(GtkWidget **confirm_dialog_message)
{
    GtkWidget *confirm_dialog;

    confirm_dialog = 
	gtk_dialog_new_with_buttons("",
				    NULL,
				    GTK_DIALOG_DESTROY_WITH_PARENT,
				    GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				    GTK_STOCK_NO, GTK_RESPONSE_NO,
				    GTK_STOCK_YES, GTK_RESPONSE_YES,
				    NULL);
    *confirm_dialog_message = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(confirm_dialog)->vbox),
		       *confirm_dialog_message,TRUE,TRUE,0);
    gtk_widget_show(*confirm_dialog_message);

    return confirm_dialog;
}

/* ------------------------------------------------------------------------- */

/* Miscellaneous callbacks.  */

gboolean enter_window_callback(GtkWidget *w, GdkEvent *e, gpointer p)
{
    _ui_top_level = gtk_widget_get_toplevel(w);

    /* cv: ensure focus after dialogs were opened */
    gtk_widget_grab_focus(w);

    return 0;
}

gboolean exposure_callback_app(GtkWidget *w, GdkEventConfigure *e, gpointer client_data)
{
    video_canvas_t *canvas = (video_canvas_t *)client_data;

    resources_set_int("WindowWidth", e->width);
    resources_set_int("Windowheight", e->height);
    /* XVideo must be refreshed when the shell window is moved. */
    if (canvas && canvas->videoconfig->hwscale
	&& (canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_1X1
	    || canvas->videoconfig->rendermode == VIDEO_RENDER_PAL_2X2))
    {
        video_canvas_refresh_all(canvas);
    }
    return 0;
}

gboolean exposure_callback_canvas(GtkWidget *w, GdkEvent *e, 
				  gpointer client_data)
{
    video_canvas_t *canvas = (video_canvas_t *)client_data;
    
    if (canvas) 
        video_canvas_refresh_all(canvas);
    
    return 0;
}


/* ------------------------------------------------------------------------- */

void ui_destroy_drive_menu(int drvnr)
{
    if (drive_menus[drvnr])
	gtk_widget_destroy(drive_menus[drvnr]);
}

void ui_set_drive_menu(int drvnr, ui_menu_entry_t *menu)
{
    drive_menus[drvnr] = gtk_menu_new();
    ui_menu_create(drive_menus[drvnr], NULL, "DriveMenu", menu);
}

void ui_set_tape_menu(ui_menu_entry_t *menu)
{
    if (tape_menu)
	gtk_widget_destroy(tape_menu);
    tape_menu = gtk_menu_new();
    ui_menu_create(tape_menu, NULL, "TapeMenu", menu);
}

void ui_display_statustext(const char *text, int fade_out)
{
    statusbar_setstatustext(text);
    if (fade_out)
	statustext_display_time = 5;
    else
	statustext_display_time = 0;
}
