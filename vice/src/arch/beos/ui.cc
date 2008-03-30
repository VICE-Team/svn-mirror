/*
 * ui.c - BeOS user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@arcormail.de>
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

#include <Alert.h>
#include <Application.h>
#include <FilePanel.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <TextView.h>
#include <View.h>
#include <Window.h>
#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "attach.h"
#include "autostart.h"
#include "archdep.h"
#include "cmdline.h"
#include "constants.h"
#include "datasette.h"
#include "drive.h"
#include "fliplist.h"
#include "imagecontents.h"
#include "info.h"
#include "interrupt.h" 
#include "kbd.h"
#include "keyboard.h"
#include "machine.h"
#include "main.h"
#include "maincpu.h"
#include "mem.h"
//#include "mon.h"
#include "mos6510.h"
#include "mouse.h"
#include "resources.h"
#include "tape.h"
#include "types.h"
#include "ui.h"
#include "ui_file.h"
#include "utils.h"
#include "version.h"
#include "vsync.h"
#include "vicewindow.h"

/* sometimes we may need pointers to the ViceWindows */
#define MAX_WINDOWS 10
ViceWindow *windowlist[MAX_WINDOWS];
int window_count = 0;

/* List of resources that can be switched on and off from the menus.  */
ui_menu_toggle  toggle_list[] = {
    { "Sound", MENU_TOGGLE_SOUND },
    { "DriveTrueEmulation", MENU_TOGGLE_DRIVE_TRUE_EMULATION },
    { "EmuID", MENU_TOGGLE_EMUID },
    { "WarpMode", MENU_TOGGLE_WARP_MODE },
    { "VirtualDevices", MENU_TOGGLE_VIRTUAL_DEVICES },
    { NULL, 0 }
};

/*  List of resources which can have multiple mutual exclusive menu entries. */
ui_res_possible_values RefreshRateValues[] = {
        {0, MENU_REFRESH_RATE_AUTO},
        {1, MENU_REFRESH_RATE_1},
        {2, MENU_REFRESH_RATE_2},
        {3, MENU_REFRESH_RATE_3},
        {4, MENU_REFRESH_RATE_4},
        {5, MENU_REFRESH_RATE_5},
        {6, MENU_REFRESH_RATE_6},
        {7, MENU_REFRESH_RATE_7},
        {8, MENU_REFRESH_RATE_8},
        {9, MENU_REFRESH_RATE_9},
        {10, MENU_REFRESH_RATE_10},
        {-1, 0}
};

ui_res_possible_values SpeedValues[] = {
        {0, MENU_MAXIMUM_SPEED_NO_LIMIT},
        {10, MENU_MAXIMUM_SPEED_10},
        {20, MENU_MAXIMUM_SPEED_20},
        {50, MENU_MAXIMUM_SPEED_50},
        {100, MENU_MAXIMUM_SPEED_100},
        {200, MENU_MAXIMUM_SPEED_200},
        {-1, 0}
};

ui_res_possible_values SyncFactor[] = {
        {DRIVE_SYNC_PAL, MENU_SYNC_FACTOR_PAL},
        {DRIVE_SYNC_NTSC, MENU_SYNC_FACTOR_NTSC},
        {DRIVE_SYNC_NTSCOLD, MENU_SYNC_FACTOR_NTSCOLD},
        {-1, 0}
};

ui_res_value_list value_list[] = {
    {"RefreshRate", RefreshRateValues},
    {"Speed", SpeedValues},
    {"VideoStandard", SyncFactor},
    {NULL,NULL}
};


ui_menu_toggle      *machine_specific_toggles=NULL;
ui_res_value_list   *machine_specific_values=NULL;

void ui_register_menu_toggles(ui_menu_toggle *toggles)
{
    machine_specific_toggles=toggles;
}

void ui_register_res_values(ui_res_value_list *valuelist)
{
    machine_specific_values=valuelist;
}

ui_machine_specific_t ui_machine_specific = NULL;

void ui_register_machine_specific(ui_machine_specific_t func)
{
    ui_machine_specific = func;
}

/* ------------------------------------------------------------------------ */
/* UI-related resources.  */

static resource_t resources[] = {
    { NULL }
};

int ui_init_resources(void)
{
    return resources_register(resources);
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */
static cmdline_option_t cmdline_options[] = {
    { NULL }
};

int ui_init_cmdline_options(void)
{
    return 0;
}


/* Exit.  */
void ui_exit(void)
{
}

/* here the stuff for queueing and dispatching ui commands */
/*---------------------------------------------------------*/
#define MAX_MESSAGE_QUEUE_SIZE	256
static BMessage message_queue[MAX_MESSAGE_QUEUE_SIZE];
static int num_queued_messages;

void ui_add_event(void *msg)
{
	if (num_queued_messages < MAX_MESSAGE_QUEUE_SIZE)
		message_queue[num_queued_messages++] = *(BMessage*)msg;
}

void ui_dispatch_events(void)
{
	int i;
	int m;
	int attachdrive;
	int key;
	BFilePanel *filepanel = windowlist[0]->filepanel;
	
	for (i=0; i<num_queued_messages; i++) {

	    /* Handle machine specific commands first.  */
	    if (ui_machine_specific)
        ui_machine_specific(&message_queue[i]);

		switch (message_queue[i].what) {
			case MENU_EXIT:
			{
				BMessenger messenger(APP_SIGNATURE);
				BMessage message(WINDOW_CLOSED);
				messenger.SendMessage(&message, be_app);
				break;
			}
			case MENU_RESET_SOFT:
				maincpu_trigger_reset();
				break;
			case MENU_RESET_HARD:
				machine_powerup();
				break;
			case MENU_AUTOSTART:
				ui_select_file(filepanel,AUTOSTART_FILE,0);
				break;
			case MENU_ATTACH_DISK8:
				attachdrive = 8;
				ui_select_file(filepanel,DISK_FILE,attachdrive);
				break;
			case MENU_ATTACH_DISK9:
				attachdrive = 9;
				ui_select_file(filepanel,DISK_FILE,attachdrive);
				break;
			case MENU_ATTACH_DISK10:
				attachdrive = 10;
				ui_select_file(filepanel,DISK_FILE,attachdrive);
				break;
			case MENU_ATTACH_DISK11:
				attachdrive = 11;
				ui_select_file(filepanel,DISK_FILE,attachdrive);
				break;
			case MENU_DETACH_DISK8:
				file_system_detach_disk(8);
				break;
			case MENU_DETACH_DISK9:
				file_system_detach_disk(9);
				break;
			case MENU_DETACH_DISK10:
				file_system_detach_disk(10);
				break;
			case MENU_DETACH_DISK11:
				file_system_detach_disk(11);
				break;
			case MENU_ATTACH_TAPE:
				attachdrive = 1;
				ui_select_file(filepanel,TAPE_FILE,1);
				break;
			case MENU_DETACH_TAPE:
				tape_detach_image();
				break;
			case MENU_DATASETTE_START:
		        datasette_control(DATASETTE_CONTROL_START);
				break;
			case MENU_DATASETTE_STOP:
		        datasette_control(DATASETTE_CONTROL_STOP);
				break;
			case MENU_DATASETTE_FORWARD:
		        datasette_control(DATASETTE_CONTROL_FORWARD);
				break;
			case MENU_DATASETTE_REWIND:
		        datasette_control(DATASETTE_CONTROL_REWIND);
				break;
			case MENU_DATASETTE_RECORD:
		        datasette_control(DATASETTE_CONTROL_RECORD);
				break;
			case MENU_DATASETTE_RESET:
		        datasette_control(DATASETTE_CONTROL_RESET);
				break;
			case MENU_DATASETTE_COUNTER:
		        datasette_control(DATASETTE_CONTROL_RESET_COUNTER);
				break;
			case MENU_EXIT_REQUESTED:
				windowlist[0]->QuitRequested();
				break;
	      	case MENU_REFRESH_RATE_AUTO:
    	    	resources_set_value("RefreshRate", (resource_value_t) 0);
        		break;
	      	case MENU_REFRESH_RATE_1:
		        resources_set_value("RefreshRate", (resource_value_t) 1);
    		    break;
      		case MENU_REFRESH_RATE_2:
        		resources_set_value("RefreshRate", (resource_value_t) 2);
	        	break;
		    case MENU_REFRESH_RATE_3:
    	    	resources_set_value("RefreshRate", (resource_value_t) 3);
        		break;
	      	case MENU_REFRESH_RATE_4:
    	    	resources_set_value("RefreshRate", (resource_value_t) 4);
        		break;
			case MENU_REFRESH_RATE_5:
    	    	resources_set_value("RefreshRate", (resource_value_t) 5);
        		break;
		    case MENU_REFRESH_RATE_6:
    		    resources_set_value("RefreshRate", (resource_value_t) 6);
        		break;
		    case MENU_REFRESH_RATE_7:
    		    resources_set_value("RefreshRate", (resource_value_t) 7);
        		break;
        	case MENU_REFRESH_RATE_8:
    		    resources_set_value("RefreshRate", (resource_value_t) 8);
        		break;
			case MENU_REFRESH_RATE_9:
        		resources_set_value("RefreshRate", (resource_value_t) 9);
        		break;
    		case MENU_REFRESH_RATE_10:
        		resources_set_value("RefreshRate", (resource_value_t) 10);
        		break;
	     	case MENU_MAXIMUM_SPEED_200:
    	    	resources_set_value("Speed", (resource_value_t) 200);
        		break;
			case MENU_MAXIMUM_SPEED_100:
        		resources_set_value("Speed", (resource_value_t) 100);
	        	break;
    		case MENU_MAXIMUM_SPEED_50:
        		resources_set_value("Speed", (resource_value_t) 50);
        		break;
		    case MENU_MAXIMUM_SPEED_20:
    		    resources_set_value("Speed", (resource_value_t) 20);
        		break;
	      	case MENU_MAXIMUM_SPEED_10:
    	    	resources_set_value("Speed", (resource_value_t) 10);
        		break;
      		case MENU_MAXIMUM_SPEED_NO_LIMIT:
        		resources_set_value("Speed", (resource_value_t) 0);
        		break;
			case MENU_SETTINGS_LOAD:
	        	if (resources_load(NULL) < 0) {
	            	ui_error("Cannot load settings.");
    	    	} else {
        	    	ui_message("Settings loaded successfully.");
        		}
				break;
			case MENU_SETTINGS_SAVE:
		        if (resources_save(NULL) < 0) {
	    	        ui_error("Cannot save settings.");
    	    	} else {
        	    	ui_message("Settings saved successfully.");
        		}
				break;
			case MENU_SETTINGS_DEFAULT:
	        	resources_set_defaults();
	        	ui_message("Default settings restored.");
				break;
			case MENU_ABOUT:
				about_vice();
				break;
			case MENU_CONTRIBUTORS:
				ui_show_text("Contributors","Who made what?",contrib_text);
				break;
			case MENU_LICENSE:
		    	ui_show_text("License",
        	    	"VICE license (GNU General Public License)", license_text);
        		break;
      		case MENU_WARRANTY:
        		ui_show_text("No warranty!",
            		"VICE is distributed WITHOUT ANY WARRANTY!",
                     warranty_text);
        		break;
			case MENU_CMDLINE:
				ui_cmdline_show_options();
				break;
			case B_KEY_DOWN:
			case B_UNMAPPED_KEY_DOWN:
				message_queue[i].FindInt32("key",(int32*)&key);
				kbd_handle_keydown(key);
				break;
			case B_KEY_UP:
			case B_UNMAPPED_KEY_UP:
				message_queue[i].FindInt32("key",(int32*)&key);
				kbd_handle_keyup(key);
				break;
			case B_REFS_RECEIVED:
				/* the file panel was closed, now we can use the selected file */
				ui_select_file_action(&message_queue[i]);
				break;
			default: 
				/* Handle the TOGGLE-Menuitems */
	            for (m = 0; toggle_list[m].name != NULL; m++) {
    	            if (toggle_list[m].item_id == message_queue[i].what) {
    	            	resources_toggle(toggle_list[m].name, NULL);
            	        break;
                	}
				}
        	    if (machine_specific_toggles) {
            	    for (m = 0; machine_specific_toggles[m].name != NULL; m++) {
                	    if (machine_specific_toggles[m].item_id == message_queue[i].what) {
    		            	resources_toggle(machine_specific_toggles[m].name,NULL);
                        	break;
	                    }
    	            }
        	    }
            	ui_update_menus();
            	break;
		}
	}
	num_queued_messages = 0;	
}	

/* -----------------------------------------------------------*/
/* Initialize the UI before setting all the resource values.  */

int ui_init(int *argc, char **argv)
{
	num_queued_messages = 0;
    return 0;
}

/* Initialize the UI after setting all the resource values.  */
int ui_init_finish(void)
{
	atexit(ui_exit);
    return 0;
}

/* ------------------------------------------------------------------------- */

/* this displays text in a seperate window with scroller */
class TextWindow : public BWindow {
	public:
		TextWindow(const char *, const char*, const char*);
		~TextWindow();
	private:
		BTextView *textview;
		BScrollView *scrollview;
};

TextWindow::TextWindow(		
	const char *caption,
	const char *header,
	const char *text) : BWindow(BRect(0,0,400,300),
		caption, B_TITLED_WINDOW, B_NOT_ZOOMABLE|B_NOT_RESIZABLE) {
	
	textview = new BTextView(	
		BRect(0,0,400-B_V_SCROLL_BAR_WIDTH,300),
		"VICE textview",
		BRect(10,10,390-B_V_SCROLL_BAR_WIDTH,290),
		B_FOLLOW_NONE,
		B_WILL_DRAW);
	scrollview = new BScrollView(
		"vice scroller",
		textview,B_FOLLOW_NONE,0,false,true);
	textview->SetText(text);
	textview->Insert("\n\n");
	textview->Insert(0,header,strlen(header));
	AddChild(scrollview);
	MoveTo(50,50);
	Show();
}

TextWindow::~TextWindow() {
	RemoveChild(scrollview);
	delete textview;
	delete scrollview;
}	
	
void ui_show_text(
	const char *caption,
	const char *header,
	const char *text)
{
	new TextWindow(caption, header, text);
}


/* display all the commandline options */
void ui_cmdline_show_options()
{
    char *options;

    options = cmdline_options_string();
    ui_show_text("Command line options",
                 "Which command line options are available?", options);
    free(options);
}



/* Report an error to the user (`printf()' style).  */
void ui_error(const char *format, ...)
{
	BAlert *messagebox;
    char tmp[1024];
    va_list args;

    va_start(args, format);
    vsprintf(tmp, format, args);
    va_end(args);
    messagebox = new BAlert("error", tmp, "OK", NULL, NULL, 
    	B_WIDTH_AS_USUAL, B_STOP_ALERT);
	messagebox->Go();
}

/* Report an error to the user (one string).  */
void ui_error_string(const char *text)
{
}

/* Report a message to the user (`printf()' style).  */
void ui_message(const char *format,...)
{
	BAlert *messagebox;
    char tmp[1024];
    va_list args;

    va_start(args, format);
    vsprintf(tmp, format, args);
    va_end(args);
    messagebox = new BAlert("info", tmp, "OK", NULL, NULL, 
    	B_WIDTH_AS_USUAL, B_INFO_ALERT);
    messagebox->Go();

}

/* Handle the "CPU JAM" case.  */
ui_jam_action_t ui_jam_dialog(const char *format,...)
{
	ui_error("CPU jam. Resetting the machine...");
    return UI_JAM_HARD_RESET;
}

/* Handle the "Do you want to extend the disk image to 40-track format"?
   dialog.  */
int ui_extend_image_dialog(void)
{
}

void ui_update_menus(void)
{
	int i;
	for (i=0; i<window_count; i++)
		windowlist[i]->Update_Menus(toggle_list,value_list);
}


/* ------------------------------------------------------------------------- */
/* Dispay the current emulation speed.  */
void ui_display_speed(float percent, float framerate, int warp_flag)
{
}

void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
}

/* Toggle displaying of the drive track.  */
/* drive_base is either 8 or 0 depending on unit or drive display.
   Dual drives display drive 0: and 1: instead of unit 8: and 9: */
void ui_display_drive_track(int drivenum, int drive_base, double track_number)
{
}

/* Toggle displaying of the drive LED.  */
void ui_display_drive_led(int drivenum, int status)
{
}

/* display current image */
void ui_display_drive_current_image(unsigned int drivenum, const char *image)
{
    /* just a dummy so far */
}

/* tape-status on*/
void ui_set_tape_status(int tape_status)
{
}

void ui_display_tape_motor_status(int motor)
{   
}

void ui_display_tape_control_status(int control)
{
}

void ui_display_tape_counter(int counter)
{
}

/* Toggle displaying of paused state.  */
void ui_display_paused(int flag)
{
}

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

static void save_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
}

static void load_snapshot_trap(ADDRESS unused_addr, void *unused_data)
{
}


} /* extern "C" */

