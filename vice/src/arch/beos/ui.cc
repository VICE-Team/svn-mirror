/*
 * ui.cc - BeOS user interface.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
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
#include <signal.h>
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
#include "joy.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "main.h"
#include "maincpu.h"
#include "mem.h"
#include "monitor.h"
#include "mos6510.h"
#include "mouse.h"
#include "resources.h"
#include "sound.h"
#include "statusbar.h"
#include "tape.h"
#include "types.h"
#include "ui.h"
#include "ui_datasette.h"
#include "ui_device.h"
#include "ui_drive.h"
#include "ui_file.h"
#include "ui_joystick.h"
#include "ui_ram.h"
#include "ui_sid.h"
#include "ui_sound.h"
#include "ui_vicii.h"
#include "ui_video.h"
#include "uicmdline.h"
#include "util.h"
#include "version.h"
#include "vsync.h"
#include "viceapp.h"
#include "vicewindow.h"
#include "video.h"
#include "videoarch.h"
}

/* sometimes we may need pointers to the ViceWindows */
#define MAX_WINDOWS 10
ViceWindow *windowlist[MAX_WINDOWS];
int window_count = 0;

/* List of resources that can be switched on and off from the menus.  */
ui_menu_toggle  toggle_list[] = {
    { "Sound", MENU_TOGGLE_SOUND },
    { "DriveTrueEmulation", MENU_TOGGLE_DRIVE_TRUE_EMULATION },
    { "EmuID", MENU_TOGGLE_EMUID },
    { "DirectWindow", MENU_TOGGLE_DIRECTWINDOW },
    { "WarpMode", MENU_TOGGLE_WARP_MODE },
    { "VirtualDevices", MENU_TOGGLE_VIRTUAL_DEVICES },
    { "SaveResourcesOnExit", MENU_TOGGLE_SAVE_SETTINGS_ON_EXIT },
    { "ConfirmOnExit", MENU_TOGGLE_CONFIRM_ON_EXIT },
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
        {MACHINE_SYNC_PAL, MENU_SYNC_FACTOR_PAL},
        {MACHINE_SYNC_NTSC, MENU_SYNC_FACTOR_NTSC},
        {MACHINE_SYNC_NTSCOLD, MENU_SYNC_FACTOR_NTSCOLD},
        {-1, 0}
};

ui_res_value_list value_list[] = {
    {"RefreshRate", RefreshRateValues},
    {"Speed", SpeedValues},
    {"MachineVideoStandard", SyncFactor},
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

static int joystickdisplay;
static int save_resources_on_exit;
static int confirm_on_exit;

static int set_joystickdisplay(resource_value_t v, void *param)
{
	joystickdisplay = (int) v;
	ui_enable_joyport();
	return 0;
}

static int set_save_resources_on_exit(resource_value_t v, void *param)
{
    save_resources_on_exit=(int) v;
    return 0;
}

static int set_confirm_on_exit(resource_value_t v, void *param)
{
    confirm_on_exit=(int) v;
    return 0;
}


static const resource_t resources[] = {
    { "JoystickDisplay", RES_INTEGER, (resource_value_t)0,
      (void *)&joystickdisplay, set_joystickdisplay, NULL },
    { "SaveResourcesOnExit",RES_INTEGER, (resource_value_t)0,
      (void *)&save_resources_on_exit, set_save_resources_on_exit, NULL },
    { "ConfirmOnExit",RES_INTEGER, (resource_value_t)1,
      (void *)&confirm_on_exit, set_confirm_on_exit, NULL },
    { NULL }
};

int ui_resources_init(void)
{
    return resources_register(resources);
}

void ui_resources_shutdown(void)
{
}

/* ------------------------------------------------------------------------ */

/* UI-related command-line options.  */
static const cmdline_option_t cmdline_options[] = {
    { NULL }
};

int ui_cmdline_options_init(void)
{
    return 0;
}


/* Exit.  */
void ui_exit(void)
{
}

static void ui_exit_early(void)
{
    /* Disable SIGINT.  This is done to prevent the user from keeping C-c
       pressed and thus breaking the cleanup process, which might be
       dangerous.  */
    signal(SIGINT, SIG_IGN);
    log_message(LOG_DEFAULT, "\nExiting...");
    machine_shutdown();
    putchar ('\n');
}


static void save_snapshot_trap(WORD unused_addr, void *unused_data)
{
	ui_select_file(windowlist[0]->savepanel,SNAPSHOTSAVE_FILE,(void*)0);
}

static void load_snapshot_trap(WORD unused_addr, void *unused_data)
{
	ui_select_file(windowlist[0]->filepanel,SNAPSHOTLOAD_FILE,(void*)0);
}

typedef struct {
    char    name[256];
    int     valid;
} snapfiles;

static snapfiles    files[10];
static int          lastindex;
static int          snapcounter;

static void save_quicksnapshot_trap(WORD unused_addr, void *unused_data)
{
int     i,j;
char    *fullname;
char    *fullname2;

    if (lastindex==-1) {
        lastindex=0;
        strcpy(files[lastindex].name,"quicksnap0.vsf");
    } else {
        if (lastindex==9) {
            if (snapcounter==10) {
                fullname=util_concat(archdep_boot_path(),"/",machine_name,"/",files[0].name,NULL);
                remove(fullname);
                free(fullname);
                for (i=1; i<10; i++) {
                    fullname=util_concat(archdep_boot_path(),"/",machine_name,"/",files[i].name,NULL);
                    fullname2=util_concat(archdep_boot_path(),"/",machine_name,"/",files[i-1].name,NULL);
                    rename(fullname,fullname2);
                    free(fullname);
                    free(fullname2);
                }
            } else {
                for (i=0; i<10; i++) {
                    if (files[i].valid==0) break;
                }
                for (j=i+1; j<10; j++) {
                    if (files[j].valid) {
                        strcpy(files[i].name,files[j].name);
                        files[i].name[strlen(files[i].name)-5]='0'+i;
                        fullname=util_concat(archdep_boot_path(),"/",machine_name,"/",files[j].name,NULL);
                        fullname2=util_concat(archdep_boot_path(),"/",machine_name,"/",files[i].name,NULL);
                        rename(fullname,fullname2);
                        free(fullname);
                        free(fullname2);
                        i++;
                    }
                }
                strcpy(files[i].name,files[0].name);
                files[i].name[strlen(files[i].name)-5]='0'+i;
                lastindex=i;
            }
        } else {
            strcpy(files[lastindex+1].name,files[lastindex].name);
            lastindex++;
            files[lastindex].name[strlen(files[lastindex].name)-5]='0'+lastindex;
        }
    }

    fullname=util_concat(archdep_boot_path(),"/",machine_name,"/",files[lastindex].name,NULL);
    if (machine_write_snapshot(fullname,0,0,0)<0) {
        ui_error("Can't write snapshot file %s.",fullname);
    }
    free(fullname);
}

static void load_quicksnapshot_trap(WORD unused_addr, void *unused_data)
{
    char *fullname;

    fullname=util_concat(archdep_boot_path(),"/",machine_name,"/",files[lastindex].name,NULL);
    if (machine_read_snapshot(fullname,0)<0) {
        ui_error("Cannot read snapshot image");
    }
    free(fullname);
}

static void scan_files(void)
{
	int                 i;
	char				*filename;
	char				*fullname;
	BEntry				entry;
	
    snapcounter=0;
    lastindex=-1;
    for (i=0; i<10; i++) {
        files[i].valid=0;
    	filename = lib_stralloc("quicksnap?.vsf");
    	filename[strlen(filename)-5] = '0'+i;
    	fullname = util_concat(archdep_boot_path(),"/",machine_name,"/",filename,NULL);
    	entry.SetTo(fullname);
    	if (entry.Exists()) {
	        strcpy(files[i].name,filename);
                files[i].valid=1;
                if (i>lastindex) {
                    lastindex=i;
                }
                snapcounter++;
        }
        free(filename);
        free(fullname);
    }
}


/* ------------------------------------------------------------------------- */
static int is_paused = 0;

/* Toggle displaying of paused state.  */
void ui_display_paused(int flag)
{
	/* use ui_display_speed() and warp flag to encode pause mode */ 
	if (flag)
		ui_display_speed(0,0,-1);
	else
		ui_display_speed(0,0,-2);
}

static void pause_trap(WORD addr, void *data)
{
    ui_display_paused(1);
    vsync_suspend_speed_eval();
    while (is_paused)
    {
        snooze(1000);
        ui_dispatch_events();
    }
}

void ui_pause_emulation(void)
{
    is_paused = is_paused ? 0 : 1;
    if (is_paused) {
        interrupt_maincpu_trigger_trap(pause_trap, 0);
    } else {
        ui_display_paused(0);
    }
}

int ui_emulation_is_paused(void)
{
    return is_paused;
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
	ViceFilePanel *filepanel = windowlist[0]->filepanel;
	
	for (i=0; i<num_queued_messages; i++) {

	    /* Handle machine specific commands first.  */
	    if (ui_machine_specific)
        ui_machine_specific(&message_queue[i], windowlist[0]);

		switch (message_queue[i].what) {
			case MENU_EXIT_REQUESTED:
			{	int32 button = 0;
				BAlert *alert;
				if (confirm_on_exit) {
					alert = new BAlert("Quit BeVICE", 
					"Do you really want to exit BeVICE??",
					"Yes","No", NULL, B_WIDTH_AS_USUAL, B_INFO_ALERT);
					vsync_suspend_speed_eval();
					button = alert->Go();
				}
				if (button == 0) {
					if (save_resources_on_exit) {
				        if (resources_save(NULL) < 0) {
	    			        ui_error("Cannot save settings.");
    	    			}
    	    		}
						
					/* send message to quit the application */
					/* and exit the emulation thread        */
					ui_exit_early();
					BMessenger messenger(APP_SIGNATURE);
					BMessage message(WINDOW_CLOSED);
					messenger.SendMessage(&message, be_app);
					exit_thread(0);
				}
				break;
			}
			case MENU_RESET_SOFT:
				machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
				break;
			case MENU_RESET_HARD:
				machine_trigger_reset(MACHINE_RESET_MODE_HARD);
				break;
			case MENU_AUTOSTART:
				ui_select_file(filepanel,AUTOSTART_FILE,(void*)0);
				break;
			case MENU_ATTACH_DISK8:
				attachdrive = 8;
				ui_select_file(filepanel,DISK_FILE,(void*)&attachdrive);
				break;
			case MENU_ATTACH_DISK9:
				attachdrive = 9;
				ui_select_file(filepanel,DISK_FILE,(void*)&attachdrive);
				break;
			case MENU_ATTACH_DISK10:
				attachdrive = 10;
				ui_select_file(filepanel,DISK_FILE,(void*)&attachdrive);
				break;
			case MENU_ATTACH_DISK11:
				attachdrive = 11;
				ui_select_file(filepanel,DISK_FILE,(void*)&attachdrive);
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
				ui_select_file(filepanel,TAPE_FILE,(void*)1);
				break;
			case MENU_DETACH_TAPE:
				tape_image_detach(1);
				break;
			case MENU_FLIP_ADD:
				flip_add_image(8);
				break;
			case MENU_FLIP_REMOVE:
				flip_remove(8, NULL);
				break;
			case MENU_FLIP_NEXT:
				flip_attach_head(8, 1);
				break;
			case MENU_FLIP_PREVIOUS:
				flip_attach_head(8, 0);
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
			case MENU_SNAPSHOT_LOAD:
				interrupt_maincpu_trigger_trap(
					load_snapshot_trap,	(void*) 0);
				break;
			case MENU_SNAPSHOT_SAVE:
				interrupt_maincpu_trigger_trap(
					save_snapshot_trap, (void*) 0);
				break;
			case MENU_LOADQUICK:
				scan_files();
				if (snapcounter>0)
					interrupt_maincpu_trigger_trap(
						load_quicksnapshot_trap, (void *) 0);
				break;
			case MENU_SAVEQUICK:
				scan_files();
				interrupt_maincpu_trigger_trap(
					save_quicksnapshot_trap, (void *) 0);
				break;
			case MENU_MONITOR:
				monitor_startup_trap();
				break;
			case MENU_PAUSE:
        		ui_pause_emulation();
        		break;
        	case MENU_DRIVE_SETTINGS:
        		ui_drive();
        		break;	
        	case MENU_DEVICE_SETTINGS:
        		ui_device();
        		break;	
        	case MENU_DATASETTE_SETTINGS:
        		ui_datasette();
        		break;	
			case MENU_JOYSTICK_SETTINGS:
				ui_joystick();
				break;
			case MENU_SOUND_SETTINGS:
				ui_sound();
				break;
			case MENU_VIDEO_SETTINGS:
				ui_video();
				break;
			case MENU_SID_SETTINGS:
				ui_sid();
				break;
			case MENU_RAM_SETTINGS:
				ui_ram();
				break;
			case MENU_SETTINGS_LOAD:
	        	if (resources_load(NULL) < 0) {
	            	ui_error("Cannot load settings.");
    	    	}
        		ui_update_menus();
				break;
			case MENU_SETTINGS_SAVE:
		        if (resources_save(NULL) < 0) {
	    	        ui_error("Cannot save settings.");
    	    	}
                flip_save_list((unsigned int) -1, 
                    archdep_default_fliplist_file_name());
				break;
			case MENU_SETTINGS_DEFAULT:
	        	resources_set_defaults();
				ui_update_menus();
				break;
			case MENU_ABOUT:
				char *abouttext;
				abouttext = util_concat(
					"BeVICE Version ", VERSION,"\n",
					"(c) 1998-2004 Andreas Boose\n",
					"(c) 1998-2004 Dag Lem\n",
					"(c) 1998-2004 Tibor Biczo\n",
					"(c) 1999-2004 Andreas Dehmel\n",
					"(c) 1999-2004 Thomas Bretz\n",
					"(c) 1999-2004 Andreas Matthies\n",
					"(c) 1999-2004 Martin Pottendorfer\n",
					"(c) 2000-2004 Markus Brenner\n",
					"(c) 2000-2004 Spiro Trikaliotis\n",
                                        "(c) 2003-2004 David Hansel\n",
                                        "(c) 1997-2001 Daniel Sladic\n",
					"\nOfficial VICE homepage:\n",
					"http://www.viceteam.org/",
					NULL);
				ui_message(abouttext);
				free(abouttext);
				break;
			case MENU_CONTRIBUTORS:
				ui_show_text("Contributors","Who made what?", info_contrib_text);
				break;
			case MENU_LICENSE:
		    	ui_show_text("License",
        	    	"VICE license (GNU General Public License)", info_license_text);
        		break;
      		case MENU_WARRANTY:
        		ui_show_text("No warranty!",
            		"VICE is distributed WITHOUT ANY WARRANTY!",
                     info_warranty_text);
        		break;
			case MENU_CMDLINE:
				ui_cmdline_show_options(NULL);
				break;
			case B_KEY_DOWN:
			case B_UNMAPPED_KEY_DOWN:
				message_queue[i].FindInt32("key",(int32*)&key);
				if (!vsid_mode) kbd_handle_keydown(key);
				break;
			case B_KEY_UP:
			case B_UNMAPPED_KEY_UP:
				message_queue[i].FindInt32("key",(int32*)&key);
				if (!vsid_mode) kbd_handle_keyup(key);
				break;
			case B_SAVE_REQUESTED:
			case B_REFS_RECEIVED:
				/* the file- or save-panel was closed */
				/* now we can use the selected file */
				ui_select_file_action(&message_queue[i]);
				break;
			case MESSAGE_ATTACH_READONLY:
			{
				int res_val;
				message_queue[i].FindInt32("device",(int32*)&attachdrive);
				resources_get_sprintf("AttachDevice%dReadonly",
				(resource_value_t*)&res_val, attachdrive);
				resources_set_sprintf("AttachDevice%dReadonly",
				(resource_value_t)!res_val, attachdrive);
				break;
			}
			case MESSAGE_SET_RESOURCE:
			{
				const char *res_name;
				int32 res_val;
				const char *res_val_str;
				if (message_queue[i].FindString("resname", &res_name) == B_OK)
					if (message_queue[i].FindInt32("resval", &res_val) == B_OK)
						resources_set_value(res_name, 
							(resource_value_t) res_val);
					if (message_queue[i].FindString("resvalstr", &res_val_str) == B_OK)
						resources_set_value(res_name, 
							(resource_value_t) res_val_str);
				break;
			}

			default:
				if (message_queue[i].what >= 'M000' &&
					message_queue[i].what <= 'M999') {
					
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

        	    	/* Handle VALUE-Menuitems */
	            	for (m = 0; value_list[m].name != NULL; m++) {
	            		
	            		ui_res_possible_values *vl = value_list[m].vals;
	            		
	            		for (; vl->item_id > 0; vl++) {
	            			if (vl->item_id == message_queue[i].what) {
    	            			resources_set_value(value_list[m].name, (resource_value_t) vl->value);
		           	        	break;
		           	        }
                		}
					}
					if (machine_specific_values) {
	            		for (m = 0; machine_specific_values[m].name != NULL; m++) {
	            		
	            			ui_res_possible_values *vl = machine_specific_values[m].vals;
	            		
	            			for (; vl->item_id > 0; vl++) {
	            				if (vl->item_id == message_queue[i].what) {
    	            				resources_set_value(machine_specific_values[m].name, (resource_value_t) vl->value);
		           	        		break;
		           	        	}
                			}
						}
        	    	}
        	    	 
            		ui_update_menus();
				}            		
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

void ui_shutdown(void)
{

}

/* Initialize the UI after setting all the resource values.  */
int ui_init_finish(void)
{
	atexit(ui_exit);
    return 0;
}

int ui_init_finalize(void)
{
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
		caption, B_DOCUMENT_WINDOW, B_NOT_ZOOMABLE|B_NOT_RESIZABLE) {
	
	textview = new BTextView(	
		BRect(0,0,400-B_V_SCROLL_BAR_WIDTH,300),
		"VICE textview",
		BRect(10,10,390-B_V_SCROLL_BAR_WIDTH,290),
		B_FOLLOW_NONE,
		B_WILL_DRAW);
	textview->MakeEditable(false);
	textview->MakeSelectable(false);
	textview->SetViewColor(230,240,230,0);

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
void ui_cmdline_show_options(void *param)
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
	vsync_suspend_speed_eval();
	messagebox->Go();
}

/* Report an error to the user (one string).  */
void ui_error_string(const char *text)
{
	ui_error(text);
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
	vsync_suspend_speed_eval();
    messagebox->Go();
}

/* Handle the "CPU JAM" case.  */
ui_jam_action_t ui_jam_dialog(const char *format,...)
{
	ui_error("Jam %s - Resetting the machine...",format);
    return UI_JAM_HARD_RESET;
}

/* Handle the "Do you want to extend the disk image to 40-track format"?
   dialog.  */
int ui_extend_image_dialog(void)
{
	int ret;
	BAlert *mb;
	
	mb = new BAlert("VICE question", "Extend image to 40-track format?",
		"Yes", "No", NULL, B_WIDTH_AS_USUAL, B_IDEA_ALERT);
	ret = mb->Go();
	
	return ret == 0;
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
	int i;
	
	for (i=0; i<window_count; i++) {
		if (windowlist[i]->Lock()) {
			if (windowlist[i]->statusbar)
				windowlist[i]->statusbar->DisplaySpeed(
					percent,framerate, warp_flag);
			windowlist[i]->Unlock();
		}
	}
}

static ui_drive_enable_t    ui_drive_enabled;
static int                  ui_status_led[DRIVE_NUM];
static double               ui_status_track[DRIVE_NUM];
static int                  *ui_drive_active_led;
static char 				*ui_drive_image_name[DRIVE_NUM];
static char 				*ui_tape_image_name;
	

static void ui_display_drive_status(int drive_num)
{
	int i;
	int drive_num_active;
	int drive_led_index;

    for (i=0; i<window_count; i++) {
		while (!windowlist[i]->Lock());
		if (windowlist[i]->statusbar) {
			int this_drive = 1 << drive_num;
		    if (ui_drive_enabled & this_drive)
        	{
        		drive_num_active = drive_num;
        	} else {
        		drive_num_active = -1-drive_num; /* this codes erasing the statusbar */
        	}
        	if (!ui_status_led[drive_num])
        		drive_led_index = -1;
        	else
        		drive_led_index = ui_drive_active_led[drive_num];
       		windowlist[i]->statusbar->DisplayDriveStatus(
				drive_num_active, drive_led_index,
				ui_status_track[drive_num]);
		}
		windowlist[i]->Unlock();
    }
}

void ui_enable_drive_status(ui_drive_enable_t enable, int *drive_led_color)
{
	ui_drive_enabled = enable;
	ui_drive_active_led = drive_led_color;	
	ui_display_drive_status(0);
	ui_display_drive_status(1);
	ui_display_drive_status(2);
	ui_display_drive_status(3);
	
}

/* Toggle displaying of the drive track.  */
/* drive_base is either 8 or 0 depending on unit or drive display.
   Dual drives display drive 0: and 1: instead of unit 8: and 9: */
void ui_display_drive_track(unsigned int drive_number, unsigned int drive_base,
                            unsigned int half_track_number)
{
    double track_number = (double)half_track_number / 2.0;

	ui_status_track[drive_number] = track_number;
	ui_display_drive_status(drive_number);
}

/* Toggle displaying of the drive LED.  */
void ui_display_drive_led(int drivenum, int status)
{
	ui_status_led[drivenum] = status;
	ui_display_drive_status(drivenum);
}

static void ui_display_image(int drivenum)
{
    int i;
    char *image;
    
    if (drivenum<0)
    	image = ui_tape_image_name;
    else
    	image = ui_drive_image_name[drivenum];
    for (i=0; i<window_count; i++) {
		while (!windowlist[i]->Lock());
		if (windowlist[i]->statusbar) {
       		windowlist[i]->statusbar->DisplayImage(drivenum, image);
		}
		windowlist[i]->Unlock();
	}
}

/* display current image */
void ui_display_drive_current_image(unsigned int drivenum, const char *image)
{
	char *directory_name;
	
	if (drivenum>=4)
		return;
		
	if (ui_drive_image_name[drivenum]) free(ui_drive_image_name[drivenum]);
				
	util_fname_split(image, &directory_name, &ui_drive_image_name[drivenum]);
    free(directory_name);
    ui_display_image(drivenum);
}

void ui_display_tape_current_image(const char *image)
{
	char *directory_name;
	
	if (ui_tape_image_name) free(ui_tape_image_name);
				
	util_fname_split(image, &directory_name, &ui_tape_image_name);
    free(directory_name);
    ui_display_image(-1);
}


static int ui_tape_enabled = 0;
static int ui_tape_counter = -1;
static int ui_tape_motor = -1;
static int ui_tape_control = -1;

static void ui_draw_tape_status()
{
	int i;

    for (i=0; i<window_count; i++) {
		while (!windowlist[i]->Lock());
		if (windowlist[i]->statusbar) {
       		windowlist[i]->statusbar->DisplayTapeStatus(
				ui_tape_enabled, ui_tape_counter, ui_tape_motor, ui_tape_control);
		}
		windowlist[i]->Unlock();
    }
}

/* tape-status on*/
void ui_set_tape_status(int tape_status)
{
	if (ui_tape_enabled != tape_status)
	{
	    ui_tape_enabled = tape_status;
    	ui_draw_tape_status();
    }
}

void ui_display_tape_motor_status(int motor)
{  
	if (ui_tape_motor != motor)
	{
	    ui_tape_motor = motor;
    	ui_draw_tape_status();
    }
}

void ui_display_tape_control_status(int control)
{
    if (ui_tape_control != control)
    {
    	ui_tape_control = control;
    	ui_draw_tape_status();
	}
}

void ui_display_tape_counter(int counter)
{
    if (ui_tape_counter != counter)
    {
    	ui_tape_counter = counter;
    	ui_draw_tape_status();
	}
}

void ui_display_recording(int recording_status)
{
}

void ui_display_playback(int playback_status)
{
}

static BYTE ui_joystick_status[3] = { 255, 255, 255 };

static void ui_display_joyport(int port_num)
{
	int i;
	
	if (!joystickdisplay)
		return;
	
    for (i=0; i<window_count; i++) {
		while (!windowlist[i]->Lock());
		if (windowlist[i]->statusbar) {
   			windowlist[i]->statusbar->DisplayJoyport(
				port_num, ui_joystick_status[port_num]);
		}
		windowlist[i]->Unlock();
    }
}
		
void ui_enable_joyport(void)
{
	int i;
	
    for (i=0; i<window_count; i++) {
		while (!windowlist[i]->Lock());
		if (windowlist[i]->statusbar) {
   			windowlist[i]->statusbar->EnableJoyport(joystickdisplay);
			windowlist[i]->Unlock();
    	}
    }
	ui_joystick_status[1]=ui_joystick_status[2]=0;
	ui_display_joyport(1);
	ui_display_joyport(2);
}


void ui_display_joyport(BYTE *joyport)
{
    int i;

    for (i =1; i <= 2; i++) {
	    if (ui_joystick_status[i] != joyport[i])
        {
            ui_joystick_status[i] = joyport[i];
		    ui_display_joyport(i);
        }
    }
}

void ui_statusbar_update()
{
	ui_display_drive_status(0);
	ui_display_image(0);
	ui_display_drive_status(1);
	ui_display_image(1);
	ui_display_image(-1);
	ui_draw_tape_status();
	ui_enable_joyport();
}	

int ui_set_window_mode(int use_direct_window)
{
	int i;
	ViceWindow *w;
	struct video_canvas_s *c;
	
	for (i=0; i<window_count; i++) {
		w = windowlist[i];
		if (w != NULL) {
			
			w->Lock();
			
			/* only use DirectWindow if Window Mode is supported */
			if (!w->SupportsWindowMode())
				use_direct_window = 0;
			
			w->use_direct_window = use_direct_window;
			
			if (use_direct_window)
			{
				w->view->SetViewColor(B_TRANSPARENT_32_BIT);
			} else {
				w->view->SetViewColor(255,255,255);
			}
			
			w->Unlock();
			
			c = (struct video_canvas_s*) (w->canvas);
			if (c != NULL)
				video_canvas_refresh_all(c);
		}
	}
	return use_direct_window;
}
