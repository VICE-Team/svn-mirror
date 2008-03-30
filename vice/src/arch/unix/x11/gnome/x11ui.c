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

#include <gnome.h>
#include <gdk/gdkx.h>
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
#include "imagecontents.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mouse.h"
#include "resources.h"
#include "uicolor.h"
#include "uimenu.h"
#include "uisettings.h"
#include "uicommands.h"
#include "utils.h"
#include "version.h"
#include "vsync.h"
#include "video.h"
#include "videoarch.h"
#include "vsiduiunix.h"
#include "x11ui.h"
#ifdef USE_XF86_DGA2_EXTENSIONS
#include "fullscreen.h"
#endif
#ifdef USE_XF86_VIDMODE_EXT
#include "vidmode.h"
#endif

/* FIXME: We want these to be static.  */
GdkVisual *visual;
static int have_truecolor;
char last_attached_images[NUM_DRIVES][256]; /* FIXME MP */
char *last_attached_tape;

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

static GtkWidget *drive8menu, *drive9menu, *tape_menu, *speed_menu;
GtkWidget *status_bar;
static GdkCursor *blankCursor;
static GtkWidget *image_preview_list, *auto_start_button, *last_file_selection;
static GtkWidget *pal_ctrl_widget, *pal_ctrl_checkbox, *pal_tb;
static char *(*current_image_contents_func)(const char *);
static GdkFont *fixedfont, *textfont;
/* FIXME, ask Xresources here */
static char *textfontname="-*-lucidatypewriter-medium-r-*-*-12-*";
static char *fixedfontname="-freetype-VICE CBM-medium-r-normal-medium-12-120-100-72-m-104-symbol-0";
static int have_cbm_font = 0;
static int cursor_is_blank = 0;

/* ------------------------------------------------------------------------- */

void ui_check_mouse_cursor()
{
    int window_doublesize;

#ifdef USE_XF86_EXTENSIONS
    if (fullscreen_is_enabled)
        return;
#endif

    if(_mouse_enabled) {
#ifdef USE_XF86_EXTENSIONS
        if(fullscreen_is_enabled) { 
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
/*static int alloc_colors(const palette_t *palette, PIXEL pixel_return[]);*/
static GtkWidget* build_file_selector(ui_button_t *button_return,
				      GtkWidget **image_contents_widget,
				      GtkWidget **autostart_button_widget,
				      GtkWidget **attach_write_protect);
static GtkWidget* build_show_text(const String text, int width, int height);
static GtkWidget* build_confirm_dialog(GtkWidget **confirm_dialog_message);
UI_CALLBACK(enter_window_callback);
UI_CALLBACK(exposure_callback);
static GtkWidget* rebuild_contents_menu(int unit, const char *image_name);
extern GtkWidget* build_pal_ctrl_widget(void);

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
    vsync_suspend_speed_eval();
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
    if (event->type == GDK_BUTTON_PRESS) {
        GdkEventButton *bevent = (GdkEventButton*) event;
	if (bevent->button == 1)
	{
	    if ((int) data == 0) 
	    {
		ui_update_flip_menus(8, 8);
		ui_menu_update_all_GTK();
		if (drive8menu)
		    gtk_menu_popup(GTK_MENU(drive8menu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	    }
	    if ((int) data == 1)
	    {
		ui_update_flip_menus(9, 9);
		ui_menu_update_all_GTK();
		if (drive9menu)
		    gtk_menu_popup(GTK_MENU(drive9menu),NULL,NULL,NULL,NULL,
				   bevent->button, bevent->time);
	    }
	}
	else if (bevent->button == 3)
	{
	    if ((int) data == 0) 
	    {
		static char *last8menu;
		static GtkWidget *l8menu;
		
		if (strcmp(last_attached_images[0], "") == 0)
		{
		    if (l8menu)
			gtk_widget_destroy(l8menu);
		    if (last8menu)
			free(last8menu);
		    last8menu = NULL;
		    return;
		}
		
		if ((last8menu == NULL) ||
		    (strcmp(last8menu, last_attached_images[0]) != 0))
		{
		    if (l8menu)
			gtk_widget_destroy(l8menu);
		    if (last8menu)
			free(last8menu);
		    last8menu = stralloc(last_attached_images[0]);
		    l8menu = rebuild_contents_menu(8, last8menu);
		}
		if (l8menu)
		    gtk_menu_popup(GTK_MENU(l8menu),NULL,NULL,NULL,NULL,
				   bevent->button, bevent->time);
	    }
	    if ((int) data == 1)
	    {
		static char *last9menu;
		static GtkWidget *l9menu;
		
		if (strcmp(last_attached_images[1], "") == 0)
		{
		    if (l9menu)
			gtk_widget_destroy(l9menu);
		    if (last9menu)
			free(last9menu);
		    last9menu = NULL;
		    return;
		}
		
		if ((last9menu == NULL) ||
		    (strcmp(last9menu, last_attached_images[1]) != 0))
		{
		    if (l9menu)
			gtk_widget_destroy(l9menu);
		    if (last9menu)
			free(last9menu);
		    last9menu = stralloc(last_attached_images[1]);
		    l9menu = rebuild_contents_menu(9, last9menu);
		}
		if (l9menu)
		    gtk_menu_popup(GTK_MENU(l9menu),NULL,NULL,NULL,NULL,
				   bevent->button, bevent->time);
	    }
	}
    }
}

static void tape_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
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
		    free(lasttapemenu);
		lasttapemenu = NULL;
		return;
	    }

	    if ((lasttapemenu == NULL) ||
		(strcmp(lasttapemenu, last_attached_tape) != 0))
	    {
		if (ltapemenu)
		    gtk_widget_destroy(ltapemenu);
		if (lasttapemenu)
		    free(lasttapemenu);
		lasttapemenu = stralloc(last_attached_tape);
		ltapemenu = rebuild_contents_menu(1, lasttapemenu);
	    }
	    if (ltapemenu)
		gtk_menu_popup(GTK_MENU(ltapemenu),NULL,NULL,NULL,NULL,
			       bevent->button, bevent->time);
	}
    }
}

void update_menu_cb(GtkWidget *w, GdkEvent *event,gpointer data)
{
    ui_menu_update_all_GTK();
}

static void filesel_autostart_cb(GtkWidget *w, gpointer data)
{
    *((ui_button_t *)data) = UI_BUTTON_AUTOSTART;
}

static void speed_popup_cb(GtkWidget *w, GdkEvent *event, gpointer data)
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

    resources_get_value("DisplayDepth", (resource_value_t *)&depth);

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
    }

    textfont = gdk_font_load(textfontname);
    if (!textfont)
	log_error(ui_log, _("Cannot load text font %s."), fixedfontname);

    fixedfont = gdk_font_load(fixedfontname);
    if (fixedfont)
	have_cbm_font = TRUE;
    else
    {
	log_warning(ui_log, _("Cannot load CBM font %s."), fixedfontname);
	fixedfont = textfont;
	have_cbm_font = FALSE;
    }

#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_vidmode_available();
#endif

#ifdef USE_XF86_VIDMODE_EXT
    vidmode_init(display, screen);
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
    gtk_widget_queue_resize(pal_ctrl_widget->parent);
}

void ui_create_status_bar(GtkWidget *pane, int width, int height)
{
    /* Create the status bar on the bottom.  */
    GtkWidget *speed_label, *drive_box, *frame, *event_box, *pcb;
    int i;
    app_shell_type *as;

    status_bar = gtk_hbox_new(FALSE, 0);

    gtk_box_pack_end(GTK_BOX(pane),status_bar,FALSE,FALSE,0);
    gtk_widget_show(status_bar);

    event_box = gtk_event_box_new();
    gtk_box_pack_start(GTK_BOX(status_bar), event_box, TRUE, TRUE,0);
    gtk_widget_show(event_box);
    gtk_signal_connect(GTK_OBJECT(event_box),
		       "button-press-event",
		       GTK_SIGNAL_FUNC(speed_popup_cb), (gpointer) NULL);
    
    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(frame), GTK_SHADOW_IN);
    gtk_container_add(GTK_CONTAINER(event_box), frame);
    gtk_widget_show(frame);
    
    speed_label = gtk_label_new("");
    app_shells[num_app_shells - 1].speed_label = (GtkLabel*) speed_label;
    gtk_misc_set_alignment (GTK_MISC (speed_label), 0, -1);
    gtk_container_add(GTK_CONTAINER(frame), speed_label);
    gtk_widget_show(speed_label);      

    as=&app_shells[num_app_shells - 1];

    /* PAL Control checkbox */
    pal_ctrl_checkbox = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(pal_ctrl_checkbox), GTK_SHADOW_IN);
    pcb = pal_tb = gtk_check_button_new_with_label(_("PAL Controls"));
    GTK_WIDGET_UNSET_FLAGS (pcb, GTK_CAN_FOCUS);
    gtk_signal_connect(GTK_OBJECT(pcb), "toggled", 
		       GTK_SIGNAL_FUNC(ui_update_pal_checkbox),
		       pcb);
    gtk_container_add(GTK_CONTAINER(pal_ctrl_checkbox), pcb);
    gtk_widget_show(pcb);
    gtk_box_pack_start(GTK_BOX(status_bar), pal_ctrl_checkbox, 
		       FALSE, FALSE, 0);
    gtk_widget_hide(pal_ctrl_checkbox);
    
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
	    gtk_pixmap_new(as->drive_status[i].led_pixmap, NULL);
	gtk_widget_set_usize(as->drive_status[i].led, LED_WIDTH, LED_HEIGHT);
	gtk_box_pack_start(GTK_BOX(as->drive_status[i].box),
			   (GtkWidget *)as->drive_status[i].led,
			   FALSE,FALSE, 4);
	gtk_widget_show(as->drive_status[i].led);

	/* Led1 for double Led drive */
	as->drive_status[i].led1_pixmap = 
	    gdk_pixmap_new(_ui_top_level->window, LED_WIDTH/2, LED_HEIGHT, 
			   depth);
	as->drive_status[i].led1 = 
	    gtk_pixmap_new(as->drive_status[i].led1_pixmap, NULL);
	gtk_widget_set_usize(as->drive_status[i].led1, LED_WIDTH/2, 
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
	    gtk_pixmap_new(as->drive_status[i].led2_pixmap, NULL);
	gtk_widget_set_usize(as->drive_status[i].led2, LED_WIDTH/2, 
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
    as->tape_status.label = gtk_label_new(_("Tape 000"));
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

void kbd_event_handler(GtkWidget *w, GdkEvent *report,gpointer gp);

/* Create a shell with a canvas widget in it.  */
int x11ui_open_canvas_window(video_canvas_t *c, const char *title,
                             int width, int height, int no_autorepeat,
                             const struct palette_s *palette)
{
    GtkWidget *new_window, *new_pane, *new_canvas, *topmenu;
    int i;
    
    if (++num_app_shells > MAX_APP_SHELLS) {
	log_error(ui_log, _("Maximum number of toplevel windows reached."));
	return -1;
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

    gtk_widget_show(new_window);
    if (vsid_mode)
	new_canvas = build_vsid_ctrl_widget();
    else
    {
	GtkRcStyle *rc_style;
	GdkColor color;

	alloc_colormap();
	gtk_widget_push_visual (visual);
        gtk_widget_push_colormap (colormap);
	new_canvas = gtk_drawing_area_new();
        gtk_widget_pop_visual ();
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
			  GDK_EXPOSURE_MASK);
    gtk_box_pack_start(GTK_BOX(new_pane),new_canvas,TRUE,TRUE,0);
    gtk_widget_show(new_canvas);

    /* XVideo must be refreshed when the application window is moved. */
    gtk_signal_connect(GTK_OBJECT(new_window), "configure-event",
		       GTK_SIGNAL_FUNC(exposure_callback),
		       (void*) c);
    gtk_signal_connect(GTK_OBJECT(new_canvas),"expose-event",
		       GTK_SIGNAL_FUNC(exposure_callback),
		       (void*) c);
    gtk_signal_connect(GTK_OBJECT(new_canvas),"enter-notify-event",
		       GTK_SIGNAL_FUNC(enter_window_callback),
		       NULL);
    if (!vsid_mode)
	gtk_widget_set_usize(new_canvas, width, height);
    gtk_widget_show(new_canvas);
    gtk_widget_queue_resize(new_canvas);

    ui_create_status_bar(new_pane, width, height);
    pal_ctrl_widget = build_pal_ctrl_widget();
    gtk_container_add(GTK_CONTAINER(new_pane), pal_ctrl_widget);
    gtk_widget_hide(pal_ctrl_widget);
    
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

    if (vsid_mode)
	return 0;
    
    if (!app_gc)
	app_gc = gdk_gc_new(new_window->window);

    if (uicolor_alloc_colors(c, palette) == -1)
        return -1;

    /* This is necessary because the status might have been set before we
       actually open the canvas window. e.g. by commandline */
    ui_enable_drive_status(enabled_drives, drive_active_led);
    /* make sure that all drive status widgets are initialized.
       This is needed for proper dual disk/dual led drives (8050, 8250). */
    for (i = 0; i < NUM_DRIVES; i++)
	ui_display_drive_led(i, 3);

    initBlankCursor();

    c->emuwindow = new_canvas;
    return 0;
}

void ui_create_dynamic_menues()
{
#ifdef USE_XF86_VIDMODE_EXT
    vidmode_create_menus();
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_create_menus();
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

void ui_set_topmenu(const char *menu_name, ...)
{
    va_list ap;
    GtkWidget *sub_menu, *mitem;
    char *item;
    int i, do_right_justify = 0;
    
    va_start(ap, menu_name);
    
    for (i = 0; i < num_app_shells; i++)
    {
	gtk_container_foreach(
	    GTK_CONTAINER(app_shells[i].topmenu), 
	    (GtkCallback) gtk_widget_destroy,
	    NULL);
    }
    
    while ((item = va_arg(ap, char *)) != NULL) 
    {
	sub_menu = va_arg(ap, GtkWidget *);
	if (! sub_menu)
	{
	    log_error(ui_log, "top_menu: sub_menu empty for '%s'.\n",
		      item);
	    return;
	}

	if (strncmp(item, "RJ", 2) == 0)
	{
	    do_right_justify = 1;
	    item += 2;
	}
	    
	for (i = 0; i < num_app_shells; i++)
	{
	    mitem = gtk_menu_item_new_with_label(item);
	    gtk_widget_show(mitem);
	    gtk_menu_item_set_submenu(GTK_MENU_ITEM(mitem), sub_menu);
	    if (do_right_justify)
		gtk_menu_item_right_justify(GTK_MENU_ITEM(mitem));
	    
	    gtk_menu_bar_append(GTK_MENU_BAR(app_shells[i].topmenu), mitem);
	    gtk_widget_show(app_shells[i].topmenu);
	}
    }
}

void ui_set_speedmenu(GtkWidget *s)
{
    if (speed_menu)
	gtk_widget_destroy(speed_menu);
    speed_menu = s;
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
    char *s = concat ("Exit ", machine_name, _(" emulator"), NULL);

    b = ui_ask_confirmation(s, _("Do you really want to exit?"));

    if (b == UI_BUTTON_YES) 
    {
        int save_resources_on_exit;

        resources_get_value("SaveResourcesOnExit",
                            (resource_value_t *)&save_resources_on_exit);
	if (save_resources_on_exit) 
	{
	    b = ui_ask_confirmation(s, _("Save the current settings?"));
	    if (b == UI_BUTTON_YES) 
	    {
		if (resources_save(NULL) < 0)
		    ui_error(_("Cannot save settings."));
	    } 
	    else if (b == UI_BUTTON_CANCEL) 
	    {
                free(s);
		return;
            }
	}
	ui_autorepeat_on();
	ui_restore_mouse();
#ifdef USE_XF86_DGA2_EXTENSIONS
	fullscreen_mode_off();
#endif
#ifdef USE_XF86_VIDMODE_EXT
	resources_set_value("UseFullscreenVidMode", (resource_value_t) 0);
#endif
	ui_dispatch_events();

	/* remove fontpath, Don't care about result */
	system("xset fp- " PREFIX "/lib/vice/fonts");
	
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
        colormap = gdk_colormap_get_system();
    } else {
        log_message(ui_log, _("Using private colormap."));
	colormap = gdk_colormap_new(visual, AllocNone);
    }

    gdk_window_set_colormap(_ui_top_level->window,colormap);

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
		if (drive_num_leds(j) == 1)
		{
		    gtk_widget_show(app_shells[i].drive_status[j].led);
		    gtk_widget_hide(app_shells[i].drive_status[j].led1);
		    gtk_widget_hide(app_shells[i].drive_status[j].led2);
		}
		else
		{
		    gtk_widget_hide(app_shells[i].drive_status[j].led);
		    gtk_widget_show(app_shells[i].drive_status[j].led1);
		    gtk_widget_show(app_shells[i].drive_status[j].led2);
		}
	    } 
	    else if (!enabled_drives &&
		       (strcmp(last_attached_images[j], "") != 0)) 
	    {
		gtk_widget_show(app_shells[i].drive_status[j].event_box);
		gtk_widget_hide(app_shells[i].drive_status[j].track_label);
		gtk_widget_hide(app_shells[i].drive_status[j].led);
		gtk_widget_hide(app_shells[i].drive_status[j].led1);
		gtk_widget_hide(app_shells[i].drive_status[j].led2);
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
    
    GdkColor *color;

    for (i = 0; i < num_app_shells; i++)
    {
	drive_status_widget *ds = &app_shells[i].drive_status[drive_number];

	color = status ? (drive_active_led[drive_number] 
			  ? drive_led_on_green_pixel 
			  : drive_led_on_red_pixel) 
	    : drive_led_off_pixel;
	gdk_gc_set_foreground(app_gc, color);
	gdk_draw_rectangle(ds->led_pixmap, app_gc, TRUE, 0, 
			   0, LED_WIDTH, LED_HEIGHT);
	gtk_widget_queue_draw(ds->led);

	color = (status & 1) ? (drive_active_led[drive_number] 
				? drive_led_on_green_pixel 
				: drive_led_on_red_pixel) 
	    : drive_led_off_pixel;
	gdk_gc_set_foreground(app_gc, color);
	gdk_draw_rectangle(ds->led1_pixmap, app_gc, TRUE, 0, 
			   0, LED_WIDTH/2, LED_HEIGHT);
	gtk_widget_queue_draw(ds->led1);

	color = (status & 2) ? (drive_active_led[drive_number] 
				? drive_led_on_green_pixel 
				: drive_led_on_red_pixel) 
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
	free(name);
	name = stralloc(_("<empty>"));
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

	gdk_window_get_size(app_shells[i].canvas->window, &w, &h);
	x11ui_resize_canvas_window(app_shells[i].canvas, w, h);
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
	free(last_attached_tape);
    last_attached_tape = stralloc(image);
    util_fname_split(image, NULL, &name);

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
    fullscreen_update();
}

/* Resize one window. */
void x11ui_resize_canvas_window(ui_window_t w, int width, int height)
{
    GtkRequisition req;

    gtk_widget_set_usize(w, width, height);
    gtk_widget_size_request(gtk_widget_get_toplevel(w), &req);
    gdk_window_resize(gdk_window_get_toplevel(w->window), 
		      req.width, req.height);
    gdk_flush();
    return;
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
    gdk_key_repeat_restore();
}

/* Disable autorepeat. */
void ui_autorepeat_off(void)
{
    gdk_key_repeat_disable();
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

#define DEFINE_BUTTON_CALLBACK(button)          \
    void  cb_##button(GtkWidget *w, ui_callback_data_t client_data) \
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

/* Message Helper */
static void ui_message2(const char *type, const char *msg, const char *title)
{
    static GtkWidget* msgdlg;

#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_off();
#endif
    msgdlg = gnome_message_box_new(msg, type,
				   GNOME_STOCK_BUTTON_CLOSE, 
				   NULL);
    
    ui_popup(msgdlg, title, False);
    gnome_dialog_run(GNOME_DIALOG(msgdlg));
    ui_unblock_shells();	/* ui_popdown can't be used in message_boxes */

    /* still needed ? */
    ui_check_mouse_cursor();
    ui_dispatch_events();
    vsync_suspend_speed_eval();
}

/* Report a message to the user.  */
void ui_message(const char *format, ...)
{
    va_list ap;
    char str[1024];

    va_start(ap, format);
    vsprintf(str, format, ap);
    ui_message2(GNOME_MESSAGE_BOX_INFO, str, _("VICE Message"));
}

/* Report an error to the user.  */
void ui_error(const char *format, ...)
{
    va_list ap;
    char str[1024];

#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_off_restore();
#endif

    va_start(ap, format);
    vsprintf(str, format, ap);
    ui_message2(GNOME_MESSAGE_BOX_ERROR, str, _("VICE Error"));
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

    jam_dialog = gnome_dialog_new("", _("Reset"), _("Hard Reset"), _("Monitor"), NULL);
    gtk_signal_connect(GTK_OBJECT(jam_dialog),
		       "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		       &jam_dialog);

    vsprintf(str, format, ap);
    message = gtk_label_new(str);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(jam_dialog)->vbox),
		       message, TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(message);
    gnome_dialog_set_default(GNOME_DIALOG(jam_dialog), 0);

    ui_popup(jam_dialog, "VICE", False);
    res = gnome_dialog_run(GNOME_DIALOG(jam_dialog));
    ui_popdown(jam_dialog);
    if (jam_dialog)
	gtk_widget_destroy(jam_dialog);

    vsync_suspend_speed_eval();
    ui_dispatch_events();

    switch (res) {
      case 2:
	ui_restore_mouse();
#ifdef USE_XF86_DGA2_EXTENSIONS
	fullscreen_mode_off();
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
    
    if (unit == 9)
    {
	ui_message(_("Autostart not possible for unit 9"));
	return;
    }
    else if (unit == 8)
    {
	tmp = stralloc(last_attached_images[0]);
	if (autostart_disk(last_attached_images[0], NULL, selected,
            AUTOSTART_MODE_RUN) < 0)
	    ui_error(_("Can't autostart selection %d in image %s"), selected,
		     tmp);
	free(tmp);
    }
    else if (unit == 1)
    {
	tmp = stralloc(last_attached_tape);
	if (autostart_tape(last_attached_tape, NULL, selected,
            AUTOSTART_MODE_RUN) < 0)
	    ui_error(_("Can't autostart selection %d in image %s"), selected,
		     tmp);
	free(tmp);
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
    image_contents_t *contents;
    ui_menu_entry_t *menu;
    int limit = 16;
    int fno = 0, mask;
    char *title, *tmp, *s, *tmp1, *tmp2;
    GtkWidget *menu_widget;
    GtkStyle *menu_entry_style;

    if (unit == 1)
	contents = image_contents_read_tape(name);
    else
	contents = image_contents_read_disk(name);
    if (contents == NULL)
	return (GtkWidget *) NULL;

    s = image_contents_to_string(contents, IMAGE_CONTENTS_STRING_PETSCII);
    
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
	    tmp2 = charset_petconvstring(tmp2, 1);

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

    menu_widget = ui_menu_create(title, menu, NULL);
    if (fixedfont)
    {
	menu_entry_style = gtk_style_new();
	gdk_font_unref(menu_entry_style->font);
	menu_entry_style->font = fixedfont;
	gdk_font_ref(menu_entry_style->font);
	gtk_container_foreach(GTK_CONTAINER(menu_widget), 
			      menu_set_style, menu_entry_style);
	gtk_style_unref(menu_entry_style);
	menu_entry_style = NULL;
    }

    /* Cleanup */
    free(title);
    g_free(menu);
    image_contents_destroy(contents);
    free(s);
    
    return menu_widget;
}

static void ui_fill_preview(GtkWidget *w, int row, int col, 
			    GdkEventButton *bevent, gpointer data)
{
    char *tmp1, *tmp2, *fname, *contents;
    GtkStyle *style;
    char *text[2];
    gint cwidth, tmpw;

    g_return_if_fail(data != NULL);
    g_return_if_fail(GTK_IS_FILE_SELECTION(data));
    
    fname = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));

    if (!fname || !current_image_contents_func)
	contents = stralloc(_("NO IMAGE CONTENTS AVAILABLE"));
    else
	contents = current_image_contents_func(fname);

    if (!contents)
	contents = stralloc(_("NO IMAGE CONTENTS AVAILABLE"));

    if (fixedfont)
    {
	style = gtk_style_new();
	gdk_font_unref(style->font);
	style->font = fixedfont;
	gdk_font_ref(style->font);
	gtk_widget_set_style(image_preview_list, style);
	gtk_style_unref(style);
    }

    tmp1 = tmp2 = contents;
    text[1] = NULL;
    cwidth = 1;
    gtk_clist_set_column_width(GTK_CLIST(image_preview_list), 0, cwidth);
    gtk_clist_freeze(GTK_CLIST(image_preview_list));
    gtk_clist_clear(GTK_CLIST(image_preview_list));
    
    tmp1 = util_find_next_line(tmp2);
    while (tmp1 > tmp2)
    {
	*(tmp1 - 1) = '\0';
	if (!have_cbm_font)
	    tmp2 = charset_petconvstring(tmp2, 1);
	text[0] = tmp2;
	gtk_clist_append(GTK_CLIST(image_preview_list), text);
	tmpw = gdk_string_width(image_preview_list->style->font, tmp2);
	if (tmpw > cwidth)
	{
	    cwidth = tmpw;
	    gtk_clist_set_column_width(GTK_CLIST(image_preview_list), 
				       0, cwidth);
	}

	tmp2 = tmp1;
	tmp1 = util_find_next_line(tmp2);
    }

    /* Last Line might be without newline char*/
    if (strcmp(tmp2, "") != 0)
    {
	text[0] = tmp2;
	gtk_clist_append(GTK_CLIST(image_preview_list), text);
	tmpw = gdk_string_width(image_preview_list->style->font, tmp2);
	if (tmpw > cwidth)
	{
	    cwidth = tmpw;
	    gtk_clist_set_column_width(GTK_CLIST(image_preview_list), 
				       0, cwidth);
	}
    }
    
    gtk_widget_grab_default(
	GTK_FILE_SELECTION(last_file_selection)->ok_button);
    
    gtk_clist_thaw(GTK_CLIST(image_preview_list));
    free(contents);
}

static void ui_select_contents_cb(GtkWidget *w, int row, int col, 
				  GdkEventButton *bevent, gpointer data)
{
    if (data)
    {
	ui_set_selected_file(row);
	gtk_widget_grab_default(auto_start_button);
	if (bevent->type == GDK_2BUTTON_PRESS)
	    gtk_button_clicked(GTK_BUTTON(auto_start_button));
    }
    else
    {
	ui_set_selected_file(0);
	gtk_widget_grab_default(
	    GTK_FILE_SELECTION(last_file_selection)->ok_button);
    }
}

/* File browser. */
char *ui_select_file(const char *title,
                     char *(*read_contents_func)(const char *),
                     unsigned int allow_autostart, const char *default_dir,
                     const char *default_pattern, ui_button_t *button_return,
		     unsigned int show_preview, int *attach_wp)
{  
    static ui_button_t button;
    static GtkWidget* file_selector = NULL;
    static char *filesel_dir = NULL;
    char *ret;
    char *current_dir = NULL;
    char *filename = NULL;
    char *path;
    GtkWidget *icw, *asb, *wp;

    /* reset old selection */
    ui_set_selected_file(0);

    /* we preserve the current directory over the invocations */
    current_dir = util_get_current_dir();	/* might be changed elsewhere */
    if (filesel_dir != NULL) {
      chdir(filesel_dir);
    }

    if (attach_wp)
	file_selector = build_file_selector(&button, &icw, &asb, &wp);
    else
	file_selector = build_file_selector(&button, &icw, &asb, NULL);

    gtk_signal_connect(GTK_OBJECT(file_selector),
		       "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		       &file_selector);

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
        char *newdir = util_get_current_dir();
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
    if (!allow_autostart)
	gtk_widget_hide(asb);

    if (!show_preview)
	gtk_widget_hide(icw);
    
    ui_popup(file_selector, title, False);
    do {
        button = UI_BUTTON_NONE;
	while (file_selector && /* we might have a killed dlg due to
				   WM actions */
	       (button == UI_BUTTON_NONE))
	    ui_dispatch_next_event();

	if (!file_selector)
	    break;
	
	filename = gtk_file_selection_get_filename(
	    GTK_FILE_SELECTION(file_selector));

    } while ((!filename && button != UI_BUTTON_CANCEL)
	     || button == UI_BUTTON_CONTENTS);

    /* `ret' gets always malloc'ed.  */
    if (filename)
        ret = stralloc(filename);
    else
	ret = stralloc("");

    ui_popdown(file_selector);

    if (attach_wp)
	*attach_wp = GTK_TOGGLE_BUTTON(wp)->active;

    if (file_selector)
    {
	gtk_widget_destroy(file_selector);
	auto_start_button = NULL;
    }

    if (filesel_dir != NULL) {
        free(filesel_dir);
    }
    filesel_dir = util_get_current_dir();
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
    GtkWidget *input_dialog, *entry, *label;
    gint res;
    char *history_id;
    ui_button_t ret;

    input_dialog = gnome_dialog_new(title,
				    GNOME_STOCK_BUTTON_OK,
				    GNOME_STOCK_BUTTON_CANCEL,
				    NULL);
    gtk_signal_connect(GTK_OBJECT(input_dialog),
		       "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		       &input_dialog);

    history_id = concat ("vice: ", prompt, NULL);
    entry = gnome_entry_new(history_id);
    free(history_id);
    
    label = gtk_label_new(prompt);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(input_dialog)->vbox),
		       label, FALSE, FALSE, GNOME_PAD);
    gtk_widget_show(label);

    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(input_dialog)->vbox),
		       entry, FALSE, FALSE, GNOME_PAD);
    gtk_entry_set_text(
	GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(entry))), buf);
    gtk_widget_show(entry);

    gnome_dialog_editable_enters(
	GNOME_DIALOG(input_dialog), 
	GTK_EDITABLE(gnome_entry_gtk_entry(GNOME_ENTRY(entry))));
    
    gnome_dialog_set_default(GNOME_DIALOG(input_dialog), GNOME_OK);

    /*    XtSetKeyboardFocus(input_dialog, input_dialog_field);*/
    ui_popup(input_dialog, title, False);
    res = gnome_dialog_run(GNOME_DIALOG(input_dialog));
    ui_popdown(input_dialog);

    if (res == 0 && input_dialog)
    {
	strncpy(buf, gtk_entry_get_text(
	    GTK_ENTRY(gnome_entry_gtk_entry(GNOME_ENTRY(entry)))),
		buflen);
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

    show_text = build_show_text((String)text, width, height);
    gtk_signal_connect(GTK_OBJECT(show_text),
		       "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		       &show_text);
    ui_popup(show_text, title, False);
    gnome_dialog_run(GNOME_DIALOG(show_text));
    ui_popdown(show_text);
    
    if (show_text)
	gtk_widget_destroy(show_text);
}

/* Ask for a confirmation. */
ui_button_t ui_ask_confirmation(const char *title, const char *text)
{
    static GtkWidget *confirm_dialog, *confirm_dialog_message;
    gint res;
    
    if (!confirm_dialog)
    {
	confirm_dialog = build_confirm_dialog(&confirm_dialog_message);
	gtk_signal_connect(GTK_OBJECT(confirm_dialog),
			   "destroy",
			   GTK_SIGNAL_FUNC(gtk_widget_destroyed),
			   &confirm_dialog);
    }
    
    gtk_label_set_text(GTK_LABEL(confirm_dialog_message),text);

    ui_popup(confirm_dialog, title, False);
    res = gnome_dialog_run(GNOME_DIALOG(confirm_dialog));
    ui_popdown(confirm_dialog);
    
    return (res == 0) ? UI_BUTTON_YES 
	: (res == 1) ? UI_BUTTON_NO 
	: UI_BUTTON_CANCEL;
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
    fullscreen_off();
    
    ui_restore_mouse();
    /* Keep sure that we really know which was the last visited shell. */
    ui_dispatch_events();

#if 0				/* this code centers popups arount the main 
				   emulation window,
				   We decided to let the WM take care of
				   placing popups */
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
#endif
    gtk_window_set_transient_for(GTK_WINDOW(w),GTK_WINDOW(_ui_top_level));
    gtk_widget_show(w);
    
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
    fullscreen_on();
}

/* ------------------------------------------------------------------------- */

/* These functions build all the widgets. */

static GtkWidget *build_file_selector(ui_button_t *button_return, 
				      GtkWidget **image_contents_widget,
				      GtkWidget **autostart_button_widget,
				      GtkWidget **attach_write_protect)
{  
    GtkWidget *fileselect, *button, *scrollw, *buttonbox, *wp_checkbox;
    GtkWidget *hbox = NULL;
    GList *tmp;
    char *contents_title[2];

    fileselect = gtk_file_selection_new("");
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselect)->ok_button),
		       "clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_OK),
		       (gpointer) button_return);
    gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fileselect)->cancel_button),
		       "clicked",
		       GTK_SIGNAL_FUNC(cb_UI_BUTTON_CANCEL),
		       (gpointer) button_return);

    gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION(fileselect));
    
    /* try to find hbox in fileselector, in order to append the preview window
       there, instead of adding it to the action area.
       This should save space in Y-direction */
    if (GTK_IS_VBOX(GTK_FILE_SELECTION(fileselect)->main_vbox))
    {
	tmp = gtk_container_children(
	    GTK_CONTAINER(GTK_FILE_SELECTION(fileselect)->main_vbox));
	while (tmp && !GTK_IS_HBOX(tmp->data))
	{
	    tmp = g_list_next(tmp);
	    if (GTK_IS_HBOX(tmp->data)) /* Yick, but there is another */
		tmp = g_list_next(tmp); /* hbox before the real one */
	}
	if (tmp)
	    hbox = tmp->data;
    }
    
    contents_title[0] = _("Contents");
    contents_title[1] = NULL;
    image_preview_list = 
	gtk_clist_new_with_titles(1, (gchar **) contents_title);
    gtk_widget_set_usize(image_preview_list, 180, 180);

    gtk_clist_column_titles_passive (GTK_CLIST (image_preview_list));
    
    /* Contents preview */
    scrollw = gtk_scrolled_window_new(NULL, NULL);
    
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollw),
				    GTK_POLICY_AUTOMATIC,
				    GTK_POLICY_AUTOMATIC);

    gtk_container_set_border_width(GTK_CONTAINER(scrollw), 5);
    
    gtk_clist_set_selection_mode (GTK_CLIST (image_preview_list), 
				 GTK_SELECTION_SINGLE);
#if 0 
    gtk_scrolled_window_add_with_viewport
        (GTK_SCROLLED_WINDOW (scrollw), image_preview_list);
#endif
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

    if (hbox)
	gtk_box_pack_start(GTK_BOX(hbox), scrollw, TRUE, TRUE, 0);
    else
	gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(
	    fileselect)->action_area), scrollw, TRUE, TRUE, 0);
    gtk_widget_show(scrollw);

    if (attach_write_protect)
    {
	/* write-protect checkbox */
	wp_checkbox = gtk_check_button_new_with_label(
	    _("Attach write protected"));
	gtk_box_pack_start(GTK_BOX(GTK_FILE_SELECTION(
	    fileselect)->action_area), wp_checkbox, TRUE, TRUE, 0);
	gtk_widget_show(wp_checkbox);
	*attach_write_protect = wp_checkbox;
    }
	
    gtk_signal_connect_after(
	GTK_OBJECT(GTK_FILE_SELECTION(fileselect)->file_list),
	"select-row",
	(GtkSignalFunc) ui_fill_preview,
	(gpointer) fileselect);

    gtk_signal_connect_after(
	GTK_OBJECT(image_preview_list),
	"select-row",
	(GtkSignalFunc) ui_select_contents_cb,
	(gpointer) 1);

    gtk_signal_connect_after(
	GTK_OBJECT(image_preview_list),
	"unselect-row",
	(GtkSignalFunc) ui_select_contents_cb,
	(gpointer) 0);

    buttonbox = GTK_FILE_SELECTION(fileselect)->ok_button->parent;
    auto_start_button = button = gnome_stock_or_ordinary_button(_("Autostart"));
    gtk_signal_connect(GTK_OBJECT(button),"clicked",
                       GTK_SIGNAL_FUNC(filesel_autostart_cb),
                       (gpointer) button_return);

    if (buttonbox)
	gtk_box_pack_start(GTK_BOX(buttonbox), button, FALSE, FALSE, 0);
    else
	gtk_box_pack_start(
	    GTK_BOX(GTK_FILE_SELECTION(fileselect)->action_area),
	    button, FALSE, FALSE, 0);
    GTK_WIDGET_SET_FLAGS(button, GTK_CAN_DEFAULT);
    gtk_widget_show(button);
    *image_contents_widget = scrollw;
    *autostart_button_widget = button;

    last_file_selection = fileselect;
    return fileselect;
}

static GtkWidget* build_show_text(const String text, int width, int height)
{
    GtkWidget *show_text, *textw, *scrollw;

    show_text = gnome_dialog_new("",
				 GNOME_STOCK_BUTTON_CLOSE,
				 NULL);

    gtk_widget_set_usize(show_text, width, height);

    scrollw = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollw),
				    GTK_POLICY_AUTOMATIC,
				    GTK_POLICY_AUTOMATIC);
    
    textw = gtk_text_new(NULL,NULL);
    gtk_text_set_line_wrap(GTK_TEXT(textw),FALSE);
    gtk_text_insert(GTK_TEXT(textw), textfont, NULL, NULL, text, -1);
    gtk_container_add(GTK_CONTAINER(scrollw), textw);
    gtk_widget_show(textw);
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(show_text)->vbox),
		       scrollw, TRUE, TRUE, GNOME_PAD);
    gtk_widget_show(scrollw);

    return show_text;
}

static GtkWidget* build_confirm_dialog(GtkWidget **confirm_dialog_message)
{
    GtkWidget *confirm_dialog;

    confirm_dialog = gnome_dialog_new("",
				      GNOME_STOCK_BUTTON_YES,
				      GNOME_STOCK_BUTTON_NO,
				      GNOME_STOCK_BUTTON_CANCEL,
				      NULL);


    *confirm_dialog_message = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(GNOME_DIALOG(confirm_dialog)->vbox),
		       *confirm_dialog_message,TRUE,TRUE,0);
    gtk_widget_show(*confirm_dialog_message);

    return confirm_dialog;
}

/* ------------------------------------------------------------------------- */

/* Miscellaneous callbacks.  */

UI_CALLBACK(enter_window_callback)
{
    _ui_top_level = gtk_widget_get_toplevel(w);
}

UI_CALLBACK(exposure_callback)
{
    GtkRequisition req;
    static int oldw, oldh;

    gtk_widget_size_request(gtk_widget_get_toplevel(w), &req);
    if (oldw != req.width || oldh != req.height) {
        oldw = req.width;
        oldh = req.height;
        gdk_window_resize(gdk_window_get_toplevel(w->window),
                          req.width, req.height);
    }
    if (client_data)
        video_canvas_refresh_all((struct video_canvas_s *)client_data);

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

void ui_destroy_drive8_menu(void)
{
    if (drive8menu)
        gtk_widget_destroy(drive8menu);
}

void ui_destroy_drive9_menu(void)
{
    if (drive9menu)
        gtk_widget_destroy(drive9menu);
}

void ui_set_drive8_menu (GtkWidget *w)
{
    drive8menu = w;
}

void ui_set_drive9_menu (GtkWidget *w)
{
    drive9menu = w;
}

void ui_set_tape_menu (GtkWidget *w)
{
    if (tape_menu)
	gtk_widget_destroy(tape_menu);
    tape_menu = w;
}

